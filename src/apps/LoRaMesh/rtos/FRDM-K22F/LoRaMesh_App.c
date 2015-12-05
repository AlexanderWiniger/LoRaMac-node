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

#include "LoRaMesh_App.h"
#include "LoRaMesh_AppConfig.h"
#include "LoRaNet.h"
#include "LoRaPhy.h"

#define LOG_LEVEL_TRACE
#include "debug.h"
/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
#define RADIO_PROCESS_INTERVAL      10      /* in [ms] = 10ms */
#define RADIO_RTOS_TICK_DELAY       (RADIO_PROCESS_INTERVAL/portTICK_RATE_MS)

/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
#define APP_CNTR_VALUE(interval)    (1000/RADIO_PROCESS_INTERVAL)

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

static LoRaNetCallbacks_t sLoRaNetCallbacks;

static uint8_t NwkSKey[] = LORAWAN_NWKSKEY;
static uint8_t AppSKey[] = LORAWAN_APPSKEY;

/*! Device address */
static uint32_t DevAddr;

/*! Application port */
static uint8_t AppPort = LORAMESH_APP_PORT;

/*! User application data size */
static uint8_t AppDataSize = LORAMESH_APP_DATA_SIZE;

/*! User application data */
static uint8_t AppData[LORAMESH_APP_DATA_MAX_SIZE];

/*! Indicates if the node is sending confirmed or unconfirmed messages */
static uint8_t IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/* Mesh app state machine function */
static void Process( void );

/* Send frame on configured app port */
static bool SendFrame( void );

/* RTOS task function */
static portTASK_FUNCTION(LoRaMeshTask, pvParameters);

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaMesh_AppInit( void )
{
    LoRaNet_Init(&sLoRaNetCallbacks);

    // NwkAddr
    DevAddr = 0x013D02AB;

    LoRaNet_InitNwkIds(LORAWAN_NETWORK_ID, DevAddr, NwkSKey, AppSKey);
    LOG_DEBUG("LoRaMesh network IDs initialized. Network ID: %u, DevAddr: 0x%08x.",
            LORAWAN_NETWORK_ID, DevAddr);

    LoRaNet_SetAdrOn (LORAWAN_ADR_ON);
    LoRaNet_SetPublicNetwork (LORAWAN_PUBLIC_NETWORK);
//    LoRaMac_SetDeviceClass (CLASS_C);
    LoRaMac_TestSetDutyCycleOn (LORAWAN_DUTYCYCLE_ON);

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
                break;
            case LORAMESH_LOWPOWER:
                break;
            case LORAMESH_TX_RX:
//                (void) LoRaMac_Process();
                break;
        }
    }
}

static bool SendFrame( void )
{
    uint8_t sendFrameStatus = 0;

    if ( IsTxConfirmed ) {
        sendFrameStatus = LoRaNet_SendConfirmedFrame(AppPort, AppData, AppDataSize,
                LORAMESH_NOF_RETRIES);
    } else {
        sendFrameStatus = LoRaNet_SendFrame(AppPort, AppData, AppDataSize);
    }

    if ( sendFrameStatus == LORA_ERR_NOTAVAIL ) return true;
    else return false;
}

static portTASK_FUNCTION(LoRaMeshTask, pvParameters)
{
    uint32_t cntr;
    (void)pvParameters; /* not used */

    cntr = 0; /* initialize LED counter */
    appState = LORAMESH_INITIAL; /* initialize state machine state */
    for(;;) {
        Process(); /* process state machine */
        cntr++;
        /* with an RTOS 10 ms/100 Hz tick rate, this is every second */
        if (cntr==APP_CNTR_VALUE(LORAMESH_APP_TX_INTERVAL)) {
            /* Send data packet */
            SendFrame();
        }
        vTaskDelay(RADIO_RTOS_TICK_DELAY);
    } /* for */
}
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
