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
#include "LoRaMesh.h"
#include "LoRaPhy.h"

#define LOG_LEVEL_TRACE
#include "debug.h"
/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
/*! Class A&B receive delay in us  */
#define RECEIVE_DELAY1                      (LORAMESH_CONFIG_RECEIVE_DELAY1)
#define RECEIVE_DELAY2                      (LORAMESH_CONFIG_RECEIVE_DELAY2)

/*! Join accept receive delay in us */
#define JOIN_ACCEPT_DELAY1                  (LORAMESH_CONFIG_JOIN_ACCEPT_DELAY1)
#define JOIN_ACCEPT_DELAY2                  (LORAMESH_CONFIG_JOIN_ACCEPT_DELAY2)

/*! Class A&B maximum receive window delay in us */
#define MAX_RX_WINDOW                       (LORAMESH_CONFIG_MAX_RX_WINDOW)

/* Advertising constants */
#define ADV_CHANNEL_FREQUENCY               (LORAMESH_CONFIG_ADV_CHANNEL_FREQUENCY)
#define ADV_BANDWIDTH                       (LORAMESH_CONFIG_ADV_BANDWIDTH)
#define ADV_DATARATE                        (LORAMESH_CONFIG_ADV_DATARATE)
#define ADV_TX_POWER                        (LORAMESH_CONFIG_ADV_TX_POWER)
#define ADV_INTERVAL                        (LORAMESH_CONFIG_ADV_INTERVAL)
#define ADV_SLOT_DURATION                   (LORAMESH_CONFIG_ADV_SLOT_DURATION)
#define ADV_EXPLICIT_HDR_OFF                (LORAMESH_CONFIG_ADV_EXPLICIT_HDR_OFF)
#define ADV_PACKET_LEN                      (LORAMESH_CONFIG_ADV_PACKET_LEN)
#define ADV_CRC_ON                          (LORAMESH_CONFIG_ADV_CRC_ON)

/* Configuration for tx and rx queues */
#define MSG_QUEUE_RX_NOF_ITEMS                  (LORAMESH_CONFIG_MSG_QUEUE_RX_LENGTH) /* number of items in the queue */
#define MSG_QUEUE_TX_NOF_ITEMS                  (LORAMESH_CONFIG_MSG_QUEUE_TX_LENGTH) /* number of items in the queue */
#define MSG_QUEUE_PUT_WAIT                      (LORAMESH_CONFIG_MSG_QUEUE_PUT_BLOCK_TIME_MS) /* blocking time for putting messages into queue */

#define LORAPHY_TXTYPE_ADVERTISING              (0)
#define LORAPHY_TXTYPE_REGULAR                  (1)
#define LORAPHY_TXTYPE_MULTICAST                (2)

#define LORAPHY_RXSLOT_ADVERTISING              (0)
#define LORAPHY_RXSLOT_RX1WINDOW                (1)
#define LORAPHY_RXSLOT_RX2WINDOW                (2)
#define LORAPHY_RXSLOT_TIME_SYNCHRONIZED        (3)

/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/
typedef union {
    uint8_t Value;
    struct {
        uint8_t TxDone :1; /* 1: Tx done */
        uint8_t TxType :2; /* Transmission type
         *   0: Advertising
         *   1: Regular (Open Rx1 & Rx2 windows)
         *   2: Multicast (No Rx windows)
         */
        uint8_t RxDone :1; /* 1: Rx done */
        uint8_t RxSlot :2; /* Reception window open
         *   0: Advertising
         *   1: Rx1
         *   2: Rx2
         *   3: Synch Rx
         */
        uint8_t reserved :2;
    } Bits;
} LoRaPhyFlags_t;

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
/*! Radio events function pointer */
static RadioEvents_t RadioEvents;

/*! Rx message queue handler */
static xQueueHandle msgRxQueue;

/*! Tx message queue handler */
static xQueueHandle msgTxQueue;

/*! Phy application status */
static LoRaPhy_AppStatus_t PhyStatus = PHY_INITIAL_STATE;

/*! Phy flags structure */
static LoRaPhyFlags_t phyFlags;

/* Incoming packet descriptor */
static LoRaPhy_PacketDesc rxPacket;

/* Incoming packet buffer */
static uint8_t rxPacketBuffer[LORAPHY_BUFFER_SIZE];

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

/*! LoRaMAC 2nd reception window settings */
static Rx2ChannelParams_t Rx2Channel = RX_WND_2_CHANNEL;

/*! Datarate offset between uplink and downlink on first window */
static uint8_t Rx1DrOffset = 0;

/*! LoRaMac maximum time a reception window stays open */
static uint32_t MaxRxWindow;

/*! Agregated duty cycle management */
static uint16_t MaxDCycle;
static uint16_t AggregatedDCycle;
static TimerTime_t AggregatedLastTxDoneTime;
static TimerTime_t AggregatedTimeOff;

/*! LoRaPhy bands */
static Band_t Bands[LORA_MAX_NB_BANDS] = { BAND0, BAND1, BAND2, BAND3, BAND4, };

/*! LoRaMAC channels */
static ChannelParams_t Channels[LORA_MAX_NB_CHANNELS] = { LC1, LC2, LC3, LC4, LC5, LC6, LC7, LC8,
        LC9, };

/*! Last transmission time on air */
static TimerTime_t TxTimeOnAir = 0;

/*! LoRa reception window timers */
static TimerEvent_t RxWindowTimers[configTIMER_QUEUE_LENGTH];

/* LoRa reception window configurations */
static RadioRxConfig_t RxWindowConfigs[configTIMER_QUEUE_LENGTH];

/* LoRa transmission configurations */
static RadioTxConfig_t TxRadioConfigs[configTIMER_QUEUE_LENGTH];

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*! \brief */
static void HandleStateMachine( void );

/*! \brief Check if tx queue contains any messages and send them if so */
static uint8_t CheckTx( void );

/*! \brief Retrieves outgoing message from tx queue */
static uint8_t GetTxMsg( uint8_t *buf, size_t bufSize );

/*! \brief Retrieves incoming message from rx queue */
static uint8_t GetRxMsg( uint8_t *buf, size_t bufSize );

/*! \brief Adds an element to rx or tx queue */
static uint8_t QueuePut( uint8_t *buf, size_t bufSize, size_t payloadSize, bool fromISR, bool isTx,
        bool toBack, uint8_t flags );

/*! \brief Sets next transmission channel */
static uint8_t SetNextChannel( void );

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

    msgRxQueue = xQueueCreate(MSG_QUEUE_RX_NOF_ITEMS, LORAPHY_BUFFER_SIZE);
    if ( msgRxQueue == NULL ) { /* queue creation failed! */
        LOG_ERROR("Could not create Rx queue at %s line %d", __FILE__,
        __LINE__);
        for ( ;; ) {
        } /* not enough memory? */
    }
    vQueueAddToRegistry(msgRxQueue, "RadioRxMsg");

    msgTxQueue = xQueueCreate(MSG_QUEUE_TX_NOF_ITEMS, LORAPHY_BUFFER_SIZE);
    if ( msgTxQueue == NULL ) { /* queue creation failed! */
        LOG_ERROR("Could not create Rx queue at %s line %d", __FILE__,
        __LINE__);
        for ( ;; ) {
        } /* not enough memory? */
    }
    vQueueAddToRegistry(msgTxQueue, "RadioTxMsg");

    /* Initialize phy flags structure */
    phyFlags.Value = 0U;

    /* Initialize rx window delays*/
    RxWindow1Delay = RECEIVE_DELAY1 - RADIO_WAKEUP_TIME;
    RxWindow2Delay = RECEIVE_DELAY2 - RADIO_WAKEUP_TIME;

    /* Initialize duty cycle variables */
    MaxDCycle = 0;
    AggregatedDCycle = 1;
    AggregatedLastTxDoneTime = 0;
    AggregatedTimeOff = 0;

    /* Default channels mask */
    pLoRaDevice->channelsMask[0] = LC(1) + LC(2) + LC(3);

    /* init Rx descriptor */
    rxPacket.phyData = &rxPacketBuffer[0];
    rxPacket.phySize = sizeof(rxPacketBuffer);
    rxPacket.rxtx = LORAPHY_BUF_PAYLOAD_START(rxPacket.phyData); /* we transmit the size too */

    /*
     * Initialize Timers
     */
    /* Advertising delay timer & config */
    TimerInit(&RxWindowTimers[LORAPHY_RXSLOT_ADVERTISING], "AdvRxWindowTimer",
            (uint32_t) & RxWindowConfigs[LORAPHY_RXSLOT_ADVERTISING], ADV_INTERVAL,
            OnRxWindowTimerEvent, false);
    TimerStart(&RxWindowTimers[LORAPHY_RXSLOT_ADVERTISING]);
    /* RX1 delay timer & config */
    TimerInit(&RxWindowTimers[LORAPHY_RXSLOT_RX1WINDOW], "RxWindowTimer1",
            (uint32_t) & RxWindowConfigs[LORAPHY_RXSLOT_RX1WINDOW], RxWindow1Delay,
            OnRxWindowTimerEvent, false);
    /* RX2 delay timer & config */
    TimerInit(&RxWindowTimers[LORAPHY_RXSLOT_RX2WINDOW], "RxWindowTimer2",
            (uint32_t) & RxWindowConfigs[LORAPHY_RXSLOT_RX2WINDOW], RxWindow2Delay,
            OnRxWindowTimerEvent, false);

    /* Initialize Radio driver */
    RadioEvents.CadDone = OnCadDone;
    RadioEvents.TxDone = OnRadioTxDone;
    RadioEvents.RxDone = OnRadioRxDone;
    RadioEvents.RxError = OnRadioRxError;
    RadioEvents.TxTimeout = OnRadioTxTimeout;
    RadioEvents.RxTimeout = OnRadioRxTimeout;
    Radio.Init(&RadioEvents);
}

uint8_t LoRaPhy_Process( void )
{
    uint8_t result;

    HandleStateMachine(); /* process state machine */
    /* process rx message */
    result = GetRxMsg(rxPacket.phyData, rxPacket.phySize);
    if ( result == ERR_OK ) {
        /* Handle incoming packet */
        if ( LoRaPhy_OnPacketRx(&rxPacket) == ERR_OK ) {
            /* Packet handled */
        }
    }
    return ERR_OK;
}

uint8_t LoRaPhy_PutPayload( uint8_t *buf, size_t bufSize, size_t payloadSize, uint8_t flags )
{
    return QueuePut(buf, bufSize, payloadSize, false, true, true, flags);
}

uint8_t LoRaPhy_OnPacketRx( LoRaPhy_PacketDesc *packet )
{
    LOG_TRACE("%s - Size %d", __FUNCTION__, LORAPHY_BUF_SIZE(packet->phyData));
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < (LORAPHY_BUF_SIZE(packet->phyData) + 2); i++ )
        LOG_TRACE_BARE("0x%02x ", packet->phyData[i]);
    LOG_TRACE_BARE("\r\n");

    return LoRaMac_OnPacketRx(packet); /* Pass message up the stack */
}

void LoRaPhy_OpenRxWindow( uint32_t freq, uint8_t datarate, uint32_t bandwidth, uint16_t timeout,
        bool rxContinuous )
{

}

uint16_t LoRaPhy_GenerateNonce( void )
{
    return (uint16_t)(Radio.Random() && 0xFFFF);
}

/*******************************************************************************
 * SETUP FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaPhy_SetChannel( uint8_t id, ChannelParams_t params )
{
    params.Band = 0;
    Channels[id] = params;
    // Activate the newly created channel
    if ( id < 16 ) {
        pLoRaDevice->channelsMask[0] |= 1 << id;
    } else if ( id < 32 ) {
        pLoRaDevice->channelsMask[1] |= 1 << (id - 16);
    } else if ( id < 48 ) {
        pLoRaDevice->channelsMask[2] |= 1 << (id - 32);
    } else if ( id < 64 ) {
        pLoRaDevice->channelsMask[3] |= 1 << (id - 48);
    } else if ( id < 72 ) {
        pLoRaDevice->channelsMask[4] |= 1 << (id - 64);
    } else {
        // Don't activate the channel
    }

    if ( (Channels[id].Frequency >= 865000000) && (Channels[id].Frequency <= 868000000) ) {
        if ( Channels[id].Band != BAND_G1_0 ) {
            Channels[id].Band = BAND_G1_0;
        }
    } else if ( (Channels[id].Frequency > 868000000) && (Channels[id].Frequency <= 868600000) ) {
        if ( Channels[id].Band != BAND_G1_1 ) {
            Channels[id].Band = BAND_G1_1;
        }
    } else if ( (Channels[id].Frequency >= 868700000) && (Channels[id].Frequency <= 869200000) ) {
        if ( Channels[id].Band != BAND_G1_2 ) {
            Channels[id].Band = BAND_G1_2;
        }
    } else if ( (Channels[id].Frequency >= 869400000) && (Channels[id].Frequency <= 869650000) ) {
        if ( Channels[id].Band != BAND_G1_3 ) {
            Channels[id].Band = BAND_G1_3;
        }
    } else if ( (Channels[id].Frequency >= 869700000) && (Channels[id].Frequency <= 870000000) ) {
        if ( Channels[id].Band != BAND_G1_4 ) {
            Channels[id].Band = BAND_G1_4;
        }
    } else {
        Channels[id].Frequency = 0;
        Channels[id].DrRange.Value = 0;
    }

    // Check if it is a valid channel
    if ( Channels[id].Frequency == 0 ) {
        LoRaPhy_ChannelRemove(id);
    }
}

void LoRaPhy_ChannelRemove( uint8_t id )
{
    if ( PhyStatus != PHY_IDLE ) {
        return;
    }
    if ( id < 3 ) {
        return;
    }

    uint8_t index = 0;
    index = id / 16;

    if ( (index > 4) || (id >= LORA_MAX_NB_CHANNELS) ) {
        return;
    }

    // Deactivate channel
    pLoRaDevice->channelsMask[index] &= ~(1 << (id % 16));

    return;
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

void LoRaPhy_SetDownLinkSettings( uint8_t rx1DrOffset, uint8_t rx2Dr )
{
    Rx1DrOffset = rx1DrOffset;
    Rx2Channel.Datarate = rx2Dr;
}

/*******************************************************************************
 * TEST FUNCTION PROTOTYPES (PUBLIC) (FOR DEBUG PURPOSES ONLY)
 ******************************************************************************/
uint8_t LoRaPhy_QueueRxMessage( uint8_t *payload, size_t payloadSize, bool toBack, uint8_t flags )
{
    return QueuePut(payload, LORAPHY_BUFFER_SIZE, payloadSize, false, false, toBack, flags);
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
/*!
 * Physical layer state machine implementation
 */
static uint32_t txTimeout = 0;
static void HandleStateMachine()
{
    uint8_t result;

    for ( ;; ) {
        switch (PhyStatus) {
            case PHY_INITIAL_STATE:
                Radio.Reset();
                /* Random seed initialization */
                srand1(Radio.Random());
                if ( pLoRaDevice->devClass != CLASS_C ) PhyStatus = PHY_IDLE;
                else PhyStatus = PHY_RECEIVING;
                break;
            case PHY_IDLE:
                result = CheckTx();
                if ( result == ERR_OK ) { /* there was data and it has been sent */
                    PhyStatus = PHY_WAIT_FOR_TXDONE;
                    break; /* process switch again */
                }
                return;
            case PHY_POWER_DOWN:
                Radio.Sleep();
                return;
            case PHY_WAIT_FOR_TXDONE:
                if ( phyFlags.Bits.TxDone == 1 ) {
                    phyFlags.Bits.TxDone = 0;
                    if ( pLoRaDevice->devClass != CLASS_C ) PhyStatus = PHY_IDLE;
                    else PhyStatus = PHY_RECEIVING;
                    break;
                }
                if ( (txTimeout++) > 3 ) {
                    PhyStatus = PHY_TIMEOUT;
                    txTimeout = 0;
                    break;
                }
                return;
            case PHY_RECEIVING:
                return;
            case PHY_TIMEOUT:
                PhyStatus = PHY_IDLE;
                LOG_ERROR("Radio timeout.");
                break;
            default:
                return;
        }
    }
}

/*!
 * Check tx message queue to see if any messages are pending.
 */
static uint8_t CheckTx( void )
{
    ChannelParams_t channel;
    uint8_t flags;
    uint8_t TxDataBuffer[LORAPHY_BUFFER_SIZE];

    if ( GetTxMsg(TxDataBuffer, sizeof(TxDataBuffer)) == ERR_OK ) {
        if ( SetNextChannel() != ERR_OK ) {
            return ERR_NOTAVAIL;
        }
        flags = LORAPHY_BUF_FLAGS(TxDataBuffer);
        channel = Channels[pLoRaDevice->currChannelIndex];

        Radio.SetChannel(channel.Frequency);
        Radio.SetMaxPayloadLength(MODEM_LORA, LORAPHY_BUF_SIZE(TxDataBuffer));

        if ( pLoRaDevice->currDataRateIndex == DR_7 ) { // High Speed FSK channel
            Radio.SetTxConfig(MODEM_FSK, TxPowers[pLoRaDevice->currTxPowerIndex], 25e3, 0,
                    Datarates[pLoRaDevice->currDataRateIndex] * 1e3, 0, 5, false, true, 0, 0, false,
                    3e6);
            TxTimeOnAir = Radio.TimeOnAir(MODEM_FSK, LORAPHY_BUF_SIZE(TxDataBuffer));
        } else if ( pLoRaDevice->currDataRateIndex == DR_6 ) { // High speed LoRa channel
            Radio.SetTxConfig(MODEM_LORA, TxPowers[pLoRaDevice->currTxPowerIndex], 0, 1,
                    Datarates[pLoRaDevice->currDataRateIndex], 1, 8, false, true, 0, 0, false, 3e3);
            TxTimeOnAir = Radio.TimeOnAir(MODEM_LORA, LORAPHY_BUF_SIZE(TxDataBuffer));
        } else { // Normal LoRa channel
            Radio.SetTxConfig(MODEM_LORA, TxPowers[pLoRaDevice->currTxPowerIndex], 0, 0,
                    Datarates[pLoRaDevice->currDataRateIndex], 1, 8, false, true, 0, 0, false, 3e3);
            TxTimeOnAir = Radio.TimeOnAir(MODEM_LORA, LORAPHY_BUF_SIZE(TxDataBuffer));
        }

        if ( MaxDCycle == 255 ) {
            return 6;
        }
        if ( MaxDCycle == 0 ) {
            AggregatedTimeOff = 0;
        }

        if ( MAX(Bands[channel.Band].TimeOff, AggregatedTimeOff) > (TimerGetCurrentTime()) ) {
            // Schedule transmission
            LOG_TRACE("Send in %d ticks on channel %d (DR: %u).",
                    MAX(Bands[channel.Band].TimeOff, AggregatedTimeOff), channel.Frequency,
                    pLoRaDevice->currDataRateIndex);
            vTaskDelay(
                    MAX(Bands[channel.Band].TimeOff, AggregatedTimeOff)
                            / MAX(Bands[channel.Band].TimeOff, AggregatedTimeOff));
        } else {
            // Send now
            LOG_TRACE("Sending now on channel %d (DR: %u).", channel.Frequency,
                    pLoRaDevice->currDataRateIndex);
            Radio.Send(LORAPHY_BUF_PAYLOAD_START(TxDataBuffer), LORAPHY_BUF_SIZE(TxDataBuffer));
        }

        if ( (flags & LORAPHY_PACKET_FLAGS_FRM_MASK) == LORAPHY_PACKET_FLAGS_FRM_ADVERTISING ) {
            phyFlags.Bits.TxType = LORAPHY_TXTYPE_ADVERTISING;
        } else if ( (flags & LORAPHY_PACKET_FLAGS_FRM_MASK) == LORAPHY_PACKET_FLAGS_FRM_REGULAR ) {
            phyFlags.Bits.TxType = LORAPHY_TXTYPE_REGULAR;
        } else if ( (flags & LORAPHY_PACKET_FLAGS_FRM_MASK)
                == LORAPHY_PACKET_FLAGS_FRM_MULTICAST ) {
            phyFlags.Bits.TxType = LORAPHY_TXTYPE_MULTICAST;
        } else {
            return ERR_VALUE;
        }

        return ERR_OK;
    }
    return ERR_NOTAVAIL; /* no data to send? */
}

/*!
 * \brief Retrieve outgoing message from tx queue.
 *
 * \param buf Pointer to the buffer to which the message will be copied.
 * \param bufSize Size of the target buffer.
 */
static uint8_t GetTxMsg( uint8_t *buf, size_t bufSize )
{
    if ( bufSize < LORAPHY_BUFFER_SIZE ) {
        return ERR_OVERFLOW; /* not enough space in buffer */
    }
    if ( xQueueReceive(msgTxQueue, buf, 0) == pdPASS ) {
        /* received message from queue */
        LOG_TRACE("LoRaPhy %s - Size %d", __FUNCTION__, LORAPHY_BUF_SIZE(buf));
        LOG_TRACE_BARE("\t");
        for ( uint8_t i = 0; i < (LORAPHY_BUF_SIZE(buf) + 2); i++ )
            LOG_TRACE_BARE("0x%02x ", buf[i]);
        LOG_TRACE_BARE("\r\n");
        return ERR_OK;
    }
    return ERR_RXEMPTY;
}

/*!
 * \brief Retrieve incoming message from rx queue.
 *
 * \param buf Pointer to the buffer to which the message will be copied.
 * \param bufSize Size of the target buffer.
 */
static uint8_t GetRxMsg( uint8_t *buf, size_t bufSize )
{
    /* first byte in the queue is the size of the item */
    if ( bufSize < LORAPHY_BUFFER_SIZE ) {
        return ERR_OVERFLOW; /* not enough space in buffer */
    }
    if ( xQueueReceive(msgRxQueue, buf, 0) == pdPASS ) { /* immediately returns if queue is empty */
        /* received message from queue */
        return ERR_OK;
    }
    return ERR_RXEMPTY;
}

/*!
 * \brief Queues a message to be sent to the radio transceiver.
 *
 * \param buf Pointer to the message data to be sent.
 * \param bufSize Size of buffer.
 * \param payloadSize Size of payload data.
 * \param fromISR If called from an ISR routine.
 * \param isTx If message is TX or RX.
 * \param flags Packet flags.
 *
 * \return Error code, ERR_OK if message has been queued.
 */
static uint8_t QueuePut( uint8_t *buf, size_t bufSize, size_t payloadSize, bool fromISR, bool isTx,
        bool toBack, uint8_t flags )
{
    /* data format is: dataSize(8bit) data */
    uint8_t res = ERR_OK;
    xQueueHandle queue;
    BaseType_t qRes;

    if ( bufSize != LORAPHY_BUFFER_SIZE ) {
        return ERR_OVERFLOW; /* must be exactly this buffer size!!! */
    }

    if ( isTx ) {
        queue = msgTxQueue;
    } else {
        queue = msgRxQueue;
    }

    LORAPHY_BUF_FLAGS(buf) = flags;
    LORAPHY_BUF_SIZE(buf) = payloadSize;

    LOG_TRACE("LoRaPhy %s - Size %d", __FUNCTION__, payloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < (payloadSize + 2); i++ )
        LOG_TRACE_BARE("0x%02x ", buf[i]);
    LOG_TRACE_BARE("\r\n");

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
            res = ERR_BUSY;
        }
    } else {
        if ( toBack ) {
            qRes = xQueueSendToBack(queue, buf, MSG_QUEUE_PUT_WAIT);
        } else {
            qRes = xQueueSendToFront(queue, buf, MSG_QUEUE_PUT_WAIT);
        }
        if ( qRes != pdTRUE ) {
            res = ERR_BUSY;
        }
    }
    return res;
}

/*
 * Set next transmission channel according to duty cycle boundries
 * unless it is a scheduled transmission on a specific channel (Class D)
 *
 * \return ERR_OK if a channel was set successfully
 */
static uint8_t SetNextChannel( void )
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    uint8_t nbEnabledChannels = 0;
    uint8_t enabledChannels[LORA_MAX_NB_CHANNELS];
    TimerTime_t curTime = TimerGetCurrentTime();

    memset1(enabledChannels, 0, LORA_MAX_NB_CHANNELS);

    // Update Aggregated duty cycle
    if ( AggregatedTimeOff < (curTime - AggregatedLastTxDoneTime) ) {
        AggregatedTimeOff = 0;
    }

    // Update bands Time OFF
    TimerTime_t minTime = (TimerTime_t)(-1);
    for ( i = 0; i < LORA_MAX_NB_BANDS; i++ ) {
        if ( pLoRaDevice->dbgFlags.Bits.dutyCycleCtrlOff == 0 ) {
            if ( Bands[i].TimeOff < (curTime - Bands[i].LastTxDoneTime) ) {
                Bands[i].TimeOff = 0;
            }
            if ( Bands[i].TimeOff != 0 ) {
                minTime = MIN(Bands[i].TimeOff, minTime);
            }
        } else {
            minTime = 0;
            Bands[i].TimeOff = 0;
        }
    }

    // Search how many channels are enabled
    for ( i = 0, k = 0; i < LORA_MAX_NB_CHANNELS; i += 16, k++ ) {
        for ( j = 0; j < 16; j++ ) {
            if ( (pLoRaDevice->channelsMask[k] & (1 << j)) != 0 ) {
                if ( Channels[i + j].Frequency == 0 ) { // Check if the channel is enabled
                    continue;
                }
                if ( ((Channels[i + j].DrRange.Fields.Min <= pLoRaDevice->currChannelIndex)
                        && (pLoRaDevice->currDataRateIndex <= Channels[i + j].DrRange.Fields.Max))
                        == false ) { // Check if the current channel selection supports the given datarate
                    continue;
                }
                if ( Bands[Channels[i + j].Band].TimeOff > 0 ) { // Check if the band is available for transmission
                    continue;
                }
                if ( AggregatedTimeOff > 0 ) { // Check if there is time available for transmission
                    continue;
                }
                enabledChannels[nbEnabledChannels++] = i + j;
            }
        }
    }
    if ( nbEnabledChannels > 0 ) {
        pLoRaDevice->currChannelIndex = enabledChannels[randr(0, nbEnabledChannels - 1)];
        return 0;
    }

    return ERR_OK;
}

static void OnRadioTxDone( void )
{
    LOG_TRACE("Transmitted successfully.");
    TimerTime_t curTime = TimerGetCurrentTime();

    // Update Band Time OFF
    Bands[Channels[pLoRaDevice->currChannelIndex].Band].LastTxDoneTime = curTime;
    if ( pLoRaDevice->dbgFlags.Bits.dutyCycleCtrlOff == 0 ) {
        Bands[Channels[pLoRaDevice->currChannelIndex].Band].TimeOff = TxTimeOnAir
                * Bands[Channels[pLoRaDevice->currChannelIndex].Band].DCycle - TxTimeOnAir;
    } else {
        Bands[Channels[pLoRaDevice->currChannelIndex].Band].TimeOff = 0;
    }
    // Update Agregated Time OFF
    AggregatedLastTxDoneTime = curTime;
    AggregatedTimeOff = AggregatedTimeOff + (TxTimeOnAir * AggregatedDCycle - TxTimeOnAir);

    if ( phyFlags.Bits.TxType == LORAPHY_TXTYPE_ADVERTISING ) {
        /* Open advertising beacon reception window */

    } else if ( phyFlags.Bits.TxType == LORAPHY_TXTYPE_REGULAR
            && pLoRaDevice->dbgFlags.Bits.rxWindowsDisabled != 1 ) {
        TimerSetValue(&RxWindowTimers[LORAPHY_RXSLOT_RX1WINDOW], RxWindow1Delay);
        TimerSetValue(&RxWindowTimers[LORAPHY_RXSLOT_RX2WINDOW], RxWindow2Delay);
    } else {
        phyFlags.Bits.TxDone = 1;
    }

    /* Uplink message repetition is only valid for unconfirmed messages */
    if ( pLoRaDevice->ctrlFlags.Bits.ackPending == 0 ) {
        pLoRaDevice->nbRepCounter++;
    }
}

static void OnRadioRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    LoRaPhy_PacketDesc packet;

    packet.flags = LORAPHY_PACKET_FLAGS_NONE;
    packet.phyData = payload;
    packet.phySize = LORAPHY_BUFFER_SIZE;

    if ( QueuePut(packet.rxtx, packet.phySize, size, true, false, true, packet.flags) == ERR_OK ) {
        phyFlags.Bits.RxDone = 1;
    }
}

static void OnCadDone( bool channelActivityDetected )
{
    uint64_t curTime = TimerGetCurrentTime();
    Radio.Sleep();
    if ( !channelActivityDetected ) {
        LOG_TRACE("Channel clear. Send packet now (%d%d)", *(((int*) (&curTime)) + 1), curTime);
//        Radio.Send(LoRaMacBuffer, LoRaMacBufferPktLen);
    }
}

static void OnRadioTxTimeout( void )
{
    LOG_ERROR("Tx timeout occurred.");
    if ( pLoRaDevice->devClass != CLASS_C ) {
        Radio.Sleep();
    } else {
//        OnRxWindow2TimerEvent();
    }

    phyFlags.Bits.TxDone = 1;
}

static void OnRadioRxTimeout( void )
{
    LOG_ERROR("Rx timeout occurred (Slot %d).", phyFlags.Bits.RxSlot);

    if ( pLoRaDevice->devClass != CLASS_C ) {
        Radio.Sleep();
    } else {
        if ( phyFlags.Bits.RxSlot == 1 ) phyFlags.Bits.TxDone = 1;
//        OnRxWindow2TimerEvent();
    }

    if ( phyFlags.Bits.RxSlot == 2 ) {
        phyFlags.Bits.TxDone = 1;
    }
}

static void OnRadioRxError( void )
{
    LOG_ERROR("Rx error occurred (Slot %d).", phyFlags.Bits.RxSlot);
    if ( pLoRaDevice->devClass != CLASS_C ) {
        Radio.Sleep();
    } else {
//        OnRxWindow2TimerEvent();
    }
    if ( phyFlags.Bits.RxSlot == 2 ) {
        phyFlags.Bits.TxDone = 1;
    }
}

static void OnRxWindowTimerEvent( TimerHandle_t xTimer )
{
//    RadioRxConfig_t* rxConfig;

//    rxConfig = (RadioRxConfig_t*) pvTimerGetTimerID(xTimer);
    LOG_TRACE("Reception timer %s expired.", pcTimerGetTimerName(xTimer));
}

static void RxWindowSetup( uint32_t freq, uint8_t dr, uint32_t bw, uint16_t timeout,
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
