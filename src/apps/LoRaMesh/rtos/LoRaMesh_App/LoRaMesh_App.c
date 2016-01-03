/**
 * \file LoRaMesh_App.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 03.12.2015
 * \version 1.0
 *
 * \brief LoRa mesh application
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "board.h"

#include "LoRaMesh.h"
#include "LoRaMesh_App.h"
#include "LoRaMesh_AppConfig.h"
#include "LoRaTest_App.h"

#define LOG_LEVEL_TRACE
#include "debug.h"
/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
#define RADIO_PROCESS_INTERVAL                      100 /* in [ms] = 10ms */
#define RADIO_RTOS_TICK_DELAY                       (RADIO_PROCESS_INTERVAL/portTICK_RATE_MS)

/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
#define APP_CNTR_VALUE(interval)                    (LORAMESH_APP_TX_INTERVAL/RADIO_PROCESS_INTERVAL)

/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/
typedef enum {
    LORAMESH_INITIAL, LORAMESH_LOWPOWER, LORAMESH_TX_RX
} LoRaMeshApp_State_t;

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
static LoRaMeshApp_State_t appState = LORAMESH_INITIAL;

static LoRaMeshCallbacks_t sLoRaMeshCallbacks;
#if( OVER_THE_AIR_ACTIVATION != 0 )

static uint8_t DevEui[] = LORAWAN_DEVICE_EUI;
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

#else

static uint8_t NwkSKey[] = LORAWAN_NWKSKEY;
static uint8_t AppSKey[] = LORAWAN_APPSKEY;

/*! Device address */
static uint32_t DevAddr;
#endif

/*! Application port */
static uint8_t AppPort = LORAMESH_APP_PORT;

/*! User application data size */
static uint8_t AppDataSize = LORAMESH_APP_DATA_SIZE;

/*! User application data */
static uint8_t AppData[LORAMESH_APP_DATA_MAX_SIZE];

/*! Indicates if the node is sending confirmed or unconfirmed messages */
static uint8_t IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;

/*! Data entries */
DataEntry_t dataEntries[5];
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/* Mesh app state machine function */
static void Process( void );

/* Send frame on configured app port */
static void SendFrame( void* param );

static uint8_t ProcessFrame( uint8_t *buf, uint8_t payloadSize, uint32_t devAddr,
        uint8_t fPort );

/* RTOS task function */
static portTASK_FUNCTION(LoRaMeshTask, pvParameters);

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaMesh_AppInit( void )
{
    LoRaMesh_Init(&sLoRaMeshCallbacks);
    LoRaMesh_RegisterApplication((PortHandlerFunction_t) & ProcessFrame, AppPort);
    LoRaMesh_RegisterTransmission(LORAMESH_APP_TX_INTERVAL, &SendFrame, (void*) NULL);

    for ( uint8_t i = 0; i < 5; i++ ) {
        dataEntries[i].DevAddr = 0;
        dataEntries[i].Timestamp = 0;
        dataEntries[i].EntryInfo.Value = 0;
        dataEntries[i].LatitudeBinary = 0;
        dataEntries[i].LongitudeBinary = 0;
        dataEntries[i].Altitude.Barometric = 0;
        dataEntries[i].Altitude.GPS = 0;
        dataEntries[i].VectorTrack.Vector = 0;
        dataEntries[i].VectorTrack.Track = 0;
        dataEntries[i].WindSpeed = 0;
    }
#if(LORAMESH_TEST_APP_ACTIVATED == 1)
    LoRaTest_AppInit();
#endif /* LORAMESH_TEST_APP_ACTIVATED */

#if( OVER_THE_AIR_ACTIVATION == 0 )
    // NwkAddr
    DevAddr = 0x013D02AB;

    LoRaMesh_SetNwkIds(LORAWAN_NETWORK_ID, DevAddr, NwkSKey, AppSKey);
    LOG_DEBUG("LoRaMesh network IDs initialized. Network ID: %u, DevAddr: 0x%08x.",
            LORAWAN_NETWORK_ID, DevAddr);
#else
    // Initialize LoRaMac device unique ID
    BoardGetUniqueId( DevEui );
#endif /* OVER_THE_AIR_ACTIVATION */

    LoRaMesh_SetAdrOn (LORAWAN_ADR_ON);
    LoRaMesh_SetPublicNetwork (LORAWAN_PUBLIC_NETWORK);
//    LoRaMesh_SetDeviceClass (CLASS_C);
    LoRaMesh_TestSetDutyCycleCtrlOff (LORAWAN_DUTYCYCLE_OFF);

    if ( xTaskCreate(LoRaMeshTask, "LoRaMesh", configMINIMAL_STACK_SIZE, (void*) NULL,
            tskIDLE_PRIORITY, (xTaskHandle*) NULL) != pdPASS ) {
        /*lint -e527 */
        for ( ;; ) {
        }; /* error! probably out of memory */
        /*lint +e527 */
    }
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
static void Process( void )
{
    for ( ;; ) {
        switch (appState) {
            case LORAMESH_INITIAL:
                appState = LORAMESH_TX_RX;
                continue;
            case LORAMESH_LOWPOWER:
                break;
            case LORAMESH_TX_RX:
                (void) LoRaPhy_Process();
                break;
        } /* end switch */
        break; /* break for loop */
    } /* end for loop */
}

static uint8_t ProcessFrame( uint8_t *buf, uint8_t payloadSize, uint32_t devAddr,
        uint8_t fPort )
{
    LOG_TRACE("Incoming message on port %u", fPort);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < payloadSize; i++ ) {
        LOG_TRACE_BARE("%c", (char) buf[i]);
    }
    LOG_TRACE_BARE("\r\n");

    return ERR_OK;
}

static void SendFrame( void* param )
{
    AppData[0] = 'H';
    AppData[1] = 'e';
    AppData[2] = 'l';
    AppData[3] = 'l';
    AppData[4] = 'o';
    AppData[5] = ' ';
    AppData[6] = 'W';
    AppData[7] = 'o';
    AppData[8] = 'r';
    AppData[9] = 'l';
    AppData[10] = 'd';
    AppData[11] = '\0';

    LOG_TRACE("Sending frame.");
//     LoRaMesh_SendFrame(AppData, AppDataSize, AppPort, true, IsTxConfirmed);
}

static portTASK_FUNCTION(LoRaMeshTask, pvParameters)
{
    uint32_t cntr = 0; /* initialize send counter */
    appState = LORAMESH_INITIAL; /* initialize state machine state */
    (void)pvParameters; /* not used */

    LOG_DEBUG_BARE("Starting LoRaMesh application...\r\n");

    for(;;) {
        /* Task interval of 100 ms */
        vTaskDelay(100/portTICK_RATE_MS);

        Process(); /* process state machine */
        if(LoRaMesh_IsNetworkJoined() && cntr > APP_CNTR_VALUE(LORAMESH_APP_TX_INTERVAL)) {
#if(LORAMESH_TEST_APP_ACTIVATED == 1)
            LoRaTest_AddFrame();
#endif
#if( OVER_THE_AIR_ACTIVATION != 0 )
        } else if(cntr > APP_CNTR_VALUE(LORAWAN_OTAA_INTERVAL)) {
            /* Send join request */
            LoRaMesh_JoinReq((uint8_t*)&DevEui, (uint8_t*)&AppEui, (uint8_t*)&AppKey);
#if(LORAMESH_TEST_APP_ACTIVATED == 1)
            vTaskDelay(500/portTICK_RATE_MS);
            LoRaTest_AddJoinAcc((uint8_t*)&DevEui, (uint8_t*)&AppEui, (uint8_t*)&AppKey, false);
#endif /* LORAMESH_TEST_APP_ACTIVATED */
#endif /* OVER_THE_AIR_ACTIVATION */
        } else {
            cntr++;
            continue;
        }
        cntr = 0;
    } /* for */
}
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
