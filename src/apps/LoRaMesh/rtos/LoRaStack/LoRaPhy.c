/**
 * \file LoRaPhy.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 03.12.2015
 * \version 1.0
 *
 * \brief LoRa stack physical layer implementation
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "board.h"
#include "LoRaMesh-config.h"
#include "LoRaPhy.h"

#define LOG_LEVEL_TRACE
#include "debug.h"
/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
/*! Maximum PHY layer payload size */
#define LORA_PHY_MAXPAYLOAD                      255
/* Configuration for tx and rx queues */
#define MSG_QUEUE_RX_NOF_ITEMS                  (LORANET_CONFIG_MSG_QUEUE_RX_LENGTH) /* number of items in the queue */
#define MSG_QUEUE_TX_NOF_ITEMS                  (LORANET_CONFIG_MSG_QUEUE_TX_LENGTH) /* number of items in the queue */
#define MSG_QUEUE_PUT_WAIT                      (LORANET_CONFIG_MSG_QUEUE_PUT_BLOCK_TIME_MS) /* blocking time for putting messages into queue */

/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/
#define ADV_RXWINDOW_CONFIG_IDX         0
#define RX1WINDOW_CONFIG_IDX            1
#define RX2WINDOW_CONFIG_IDX            2

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
/*! Radio events function pointer */
static RadioEvents_t RadioEvents;

/*! Rx message queue handler */
static xQueueHandle LoRaPhy_MsgRxQueue;

/*! Tx message queue handler */
static xQueueHandle LoRaPhy_MsgTxQueue;

/*! LoRaMac reception windows delay from end of Tx */
static uint32_t ReceiveDelay1;
static uint32_t ReceiveDelay2;
static uint32_t JoinAcceptDelay1;
static uint32_t JoinAcceptDelay2;

/*! LoRa reception windows delay
 * \remark normal frame: RxWindowXDelay = ReceiveDelayX - RADIO_WAKEUP_TIME
 *         join frame  : RxWindowXDelay = JoinAcceptDelayX - RADIO_WAKEUP_TIME
 */
static uint32_t RxWindow1Delay;
static uint32_t RxWindow2Delay;

/*! LoRaMac maximum time a reception window stays open */
static uint32_t MaxRxWindow;

/*! LoRa reception window timers */
static TimerEvent_t RxWindowTimers[configTIMER_QUEUE_LENGTH];

/* LoRa reception window configurations */
static RadioRxConfig_t RxWindowConfigs[configTIMER_QUEUE_LENGTH];

/* LoRa transmission configurations */
static RadioTxConfig_t TxRadioConfigs[configTIMER_QUEUE_LENGTH];

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*! \brief Function to be executed on Radio Tx Done event */
static void OnRadioTxDone( void );

/*! \brief Function to be executed on Radio Rx Done event */
static void OnRadioRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*! \brief Function to be executed on Radio Cad Done event */
static void OnCadDone( bool channelActivityDetected );

/*! \brief Function executed on Radio Tx Timeout event */
static void OnRadioTxTimeout( void );

/*! \brief Function executed on Radio Rx error event */
static void OnRadioRxError( void );

/*! \brief Function executed on Radio Rx Timeout event */
static void OnRadioRxTimeout( void );

/*! Function executed on second Rx window timer event */
static void OnRxWindowTimerEvent( TimerHandle_t xTimer );

/*! brief Adds an element to the correct queue */
static uint8_t QueuePut( uint8_t *buf, size_t bufSize, bool fromISR, bool isTx, bool toBack,
        PacketDesc_t packedDesc );

/*******************************************************************************
 * API FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaPhy_Init( void )
{
    /* Initialize structures */
    uint8_t i;

    for ( i = 0; i < configTIMER_QUEUE_LENGTH; i++ ) {
        /* Rx timers */
        RxWindowTimers[i].Handle = NULL;
        RxWindowTimers[i].Callback = NULL;
        RxWindowTimers[i].PeriodInMs = 0;
        RxWindowTimers[i].IsRunning = false;
        RxWindowTimers[i].AutoReload = false;
    }

    LoRaPhy_MsgRxQueue = xQueueCreate(MSG_QUEUE_RX_NOF_ITEMS, LORA_PHY_MAXPAYLOAD);
    if ( LoRaPhy_MsgRxQueue == NULL ) { /* queue creation failed! */
        LOG_ERROR("Could not create Rx queue at %s line %d", __FILE__, __LINE__);
        for ( ;; ) {
        } /* not enough memory? */
    }
    vQueueAddToRegistry(LoRaPhy_MsgRxQueue, "RadioRxMsg");

    LoRaPhy_MsgTxQueue = xQueueCreate(MSG_QUEUE_TX_NOF_ITEMS, LORA_PHY_MAXPAYLOAD);
    if ( LoRaPhy_MsgTxQueue == NULL ) { /* queue creation failed! */
        LOG_ERROR("Could not create Rx queue at %s line %d", __FILE__, __LINE__);
        for ( ;; ) {
        } /* not enough memory? */
    }
    vQueueAddToRegistry(LoRaPhy_MsgTxQueue, "RadioTxMsg");

    RxWindow1Delay = RECEIVE_DELAY1 - RADIO_WAKEUP_TIME;
    RxWindow2Delay = RECEIVE_DELAY2 - RADIO_WAKEUP_TIME;

    /*
     * Initialize Timers
     */
    /* Advertising delay timer & config */
    TimerInit(&RxWindowTimers[ADV_RXWINDOW_CONFIG_IDX], "AdvRxWindowTimer",
            (uint32_t) & RxWindowConfigs[ADV_RXWINDOW_CONFIG_IDX], 100, OnRxWindowTimerEvent,
            false);
    TimerStart(&RxWindowTimers[ADV_RXWINDOW_CONFIG_IDX]);
    /* RX1 delay timer & config */
    TimerInit(&RxWindowTimers[RX1WINDOW_CONFIG_IDX], "RxWindowTimer1",
            (uint32_t) & RxWindowConfigs[RX1WINDOW_CONFIG_IDX], RxWindow1Delay,
            OnRxWindowTimerEvent, false);
    /* RX2 delay timer & config */
    TimerInit(&RxWindowTimers[RX2WINDOW_CONFIG_IDX], "RxWindowTimer2",
            (uint32_t) & RxWindowConfigs[RX2WINDOW_CONFIG_IDX], RxWindow2Delay,
            OnRxWindowTimerEvent, false);
#if 0
    /* Initialize Radio driver */
    RadioEvents.CadDone = OnCadDone;
    RadioEvents.TxDone = OnRadioTxDone;
    RadioEvents.RxDone = OnRadioRxDone;
    RadioEvents.RxError = OnRadioRxError;
    RadioEvents.TxTimeout = OnRadioTxTimeout;
    RadioEvents.RxTimeout = OnRadioRxTimeout;
    Radio.Init(&RadioEvents);

    /* Random seed initialization */
    srand1(Radio.Random());

    Radio.Sleep();
#endif
}

uint8_t LoRaPhy_PutPayload( uint8_t *buf, size_t bufSize, uint8_t payloadSize, uint8_t txConfig )
{
    return QueuePut(buf, bufSize, false, true, true, (PacketDesc_t) & TxRadioConfigs[txConfig]);
}

uint8_t LoRaPhy_GetPayload( RxPacketDesc_t *packet )
{
    return LORA_ERR_OK;
}

void LoRaPhy_OpenRxWindow( uint32_t freq, int8_t datarate, uint32_t bandwidth, uint16_t timeout,
        bool rxContinuous )
{

}

void LoRaPhy_SetMaxRxWindow( uint32_t delay )
{
    MaxRxWindow = delay;
}

void LoRaPhy_SetReceiveDelay1( uint32_t delay )
{
    ReceiveDelay1 = delay;
}

void LoRaPhy_SetReceiveDelay2( uint32_t delay )
{
    ReceiveDelay2 = delay;
}

void LoRaPhy_SetJoinAcceptDelay1( uint32_t delay )
{
    JoinAcceptDelay1 = delay;
}

void LoRaPhy_SetJoinAcceptDelay2( uint32_t delay )
{
    JoinAcceptDelay2 = delay;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
uint8_t GetTxMsg( uint8_t *buf, size_t bufSize )
{
    if ( bufSize < LORA_PHY_MAXPAYLOAD ) {
        return FAIL; /* not enough space in buffer */
    }
    if ( xQueueReceive(LoRaPhy_MsgTxQueue, buf, 0) == pdPASS ) {
        /* received message from queue */
        return SUCCESS;
    }
    return FAIL;
}

uint8_t GetRxMsg( uint8_t *buf, size_t bufSize )
{
    /* first byte in the queue is the size of the item */
    if ( bufSize < LORA_PHY_MAXPAYLOAD ) {
        return LORA_ERR_OVERFLOW; /* not enough space in buffer */
    }
    if ( xQueueReceive(LoRaPhy_MsgRxQueue, buf, 0) == pdPASS ) { /* immediately returns if queue is empty */
        /* received message from queue */
        return LORA_ERR_OK;
    }
    return LORA_ERR_RXEMPTY;
}

static uint8_t QueuePut( uint8_t *buf, size_t bufSize, bool fromISR, bool isTx, bool toBack,
        PacketDesc_t packedDesc )
{
    /* data format is: dataSize(8bit) data */
    uint8_t res = LORA_ERR_OK;
    xQueueHandle queue;
    BaseType_t qRes;

    if ( bufSize != LORA_PHY_MAXPAYLOAD ) {
        return LORA_ERR_OVERFLOW; /* must be exactly this buffer size!!! */
    }

    if ( isTx ) {
        queue = LoRaPhy_MsgTxQueue;
    } else {
        queue = LoRaPhy_MsgRxQueue;
    }

    if ( fromISR ) {
        signed portBASE_TYPE
        pxHigherPriorityTaskWoken;

        if ( toBack ) {
            qRes = xQueueSendToBackFromISR(queue, buf, &pxHigherPriorityTaskWoken);
        } else {
            qRes = xQueueSendToFrontFromISR(queue, buf, &pxHigherPriorityTaskWoken);
        }
        if ( qRes != pdTRUE ) {
            /* was not able to send to the queue. Well, not much we can do here... */
            res = LORA_ERR_BUSY;
        }
    } else {
        if ( toBack ) {
            qRes = xQueueSendToBack(queue, buf, MSG_QUEUE_PUT_WAIT);
        } else {
            qRes = xQueueSendToFront(queue, buf, MSG_QUEUE_PUT_WAIT);
        }
        if ( qRes != pdTRUE ) {
            res = LORA_ERR_BUSY;
        }
    }
    return res;
}

static void OnRadioTxDone( void )
{

}

static void OnRadioRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{

}

static void OnCadDone( bool channelActivityDetected )
{

}

static void OnRadioTxTimeout( void )
{

}

static void OnRadioRxTimeout( void )
{

}

static void OnRadioRxError( void )
{

}

static void OnRxWindowTimerEvent( TimerHandle_t xTimer )
{
    RadioRxConfig_t* rxConfig;

    rxConfig = (RadioRxConfig_t*) pvTimerGetTimerID(xTimer);
}

static void RxWindowSetup( uint32_t freq, int8_t dr, uint32_t bw, uint16_t timeout,
        bool rxContinuous, RxWindowType_t windowType )
{
    if ( Radio.GetStatus() == RF_IDLE ) {
        RadioModems_t modem;
        uint32_t bandwidth, datarate, bandwidthAfc;
        uint16_t preambleLen, symbTimeout;
        uint8_t coderate, payloadLen;
        bool fixLen, crcOn;

        Radio.SetChannel(freq);

        if ( windowType == RX_TYPE_ADV ) {
            rxContinuous = true;
            crcOn = ADV_CRC_ON;
            payloadLen = ADV_PACKET_LEN;
            fixLen = ADV_EXPLICIT_HDR_OFF;
        } else {
            crcOn = true;
            payloadLen = 0;
            fixLen = false;
        }

        if ( dr == 50 ) {
            modem = MODEM_FSK;
            bandwidth = 50e3;
            datarate = dr * 1e3;
            coderate = 0;
            bandwidthAfc = 83.333e3;
            preambleLen = 5;
            symbTimeout = 0;
        } else {
            modem = MODEM_LORA;
            bandwidth = bw;
            datarate = dr;
            coderate = 1;
            bandwidthAfc = 0;
            preambleLen = 8;
            symbTimeout = timeout;
        }

        Radio.SetRxConfig(modem, bandwidth, datarate, coderate, bandwidthAfc, preambleLen,
                symbTimeout, fixLen, payloadLen, crcOn, 0, 0, false, rxContinuous);

        if ( rxContinuous == false ) {
            Radio.Rx(MaxRxWindow);
        } else {
            Radio.Rx(0);   // Continuous mode
        }
    }
}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
