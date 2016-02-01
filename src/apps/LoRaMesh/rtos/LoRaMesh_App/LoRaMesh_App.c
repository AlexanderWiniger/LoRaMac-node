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

#define LOG_LEVEL_DEBUG
#include "debug.h"
/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
#define RADIO_PROCESS_INTERVAL                      100 /* in [ms] = 10ms */
#define RADIO_RTOS_TICK_DELAY                       (RADIO_PROCESS_INTERVAL/portTICK_RATE_MS)

#define NODE_C

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

/*! Application multicast group address */
static const uint32_t AppAddr = 0x00A5EF01;

/*! Application port */
static uint8_t AppPort = LORAMESH_APP_PORT;

/*! User application data size */
static uint8_t AppDataSize = LORAMESH_APP_DATA_SIZE;

/*! User application data */
static uint8_t AppData[LORAMESH_APP_DATA_MAX_SIZE];

/*! Indicates if the node is sending confirmed or unconfirmed messages */
static uint8_t IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;

/*! Data entries */
static DataEntry_t dataEntries[LORAMESH_APP_NOF_DATA_ENTRIES];
static DataEntry_t *pDataEntries;
static DataEntry_t *pFreeDataEntries;
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/* Mesh app state machine function */
static void Process( void );

/* Send frame on configured app port */
static void SendDataFrame( void *param );

/* Send frame on configured app port */
static void SendMulticastDataFrame( void *param );

/**/
static void ReceiveDataFrame( void *param );

/* Process data frame */
static uint8_t ProcessDataFrame( uint8_t *buf, uint8_t payloadSize, uint32_t devAddr,
        uint8_t fPort );

/**/
static DataEntry_t *FindEntry( uint32_t devAddr );

/**/
static DataEntry_t *AllocateEntry( void );

/**/
static void FreeEntry( DataEntry_t *entry );

/* RTOS task function */
static void LoRaMeshTask( void *pvParameters );

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaMesh_AppInit( void )
{
    LoRaMesh_Init(&sLoRaMeshCallbacks);

    /* Initialize data entries structure */
    pDataEntries = NULL;
    pFreeDataEntries = dataEntries;
    for ( uint8_t i = 0; i < LORAMESH_APP_NOF_DATA_ENTRIES - 1; i++ ) {
        dataEntries[i].next = (DataEntry_t*) &dataEntries[i + 1];
    }
#if(LORAMESH_TEST_APP_ACTIVATED == 1)
    LoRaTest_AppInit();
#endif /* LORAMESH_TEST_APP_ACTIVATED */

#if( OVER_THE_AIR_ACTIVATION == 0 )
    // NwkAddr
#if defined(NODE_A)
    DevAddr = 0x013AE27A;
#elif defined(NODE_B)
    DevAddr = 0x013A1024;
#elif defined(NODE_C)
    DevAddr = 0x013AD5F1;
#endif

    LoRaMesh_SetNwkIds(LORAWAN_NETWORK_ID, DevAddr, NwkSKey, AppSKey);
    LOG_DEBUG("LoRaMesh network IDs initialized. Network ID: %u, DevAddr: 0x%08x.",
            LORAWAN_NETWORK_ID, DevAddr);
#else
    // Initialize LoRaMac device unique ID
    BoardGetUniqueId (DevEui);
#endif /* OVER_THE_AIR_ACTIVATION */

    LoRaMesh_SetAdrOn (LORAWAN_ADR_ON);
    LoRaMesh_SetPublicNetwork (LORAWAN_PUBLIC_NETWORK);
    LoRaMesh_TestSetDutyCycleCtrlOff (LORAWAN_DUTYCYCLE_OFF);
#if defined(NODE_A)
    LoRaMesh_SetDeviceRole (COORDINATOR);
#endif

    LoRaMesh_RegisterApplication((PortHandlerFunction_t) & ProcessDataFrame, AppPort);

    if ( pLoRaDevice->devRole == NODE ) {
        /* Multicast group */
#if defined(NODE_B)
        LoRaMesh_RegisterTransmission(0, 5000000, EVENT_TYPE_UPLINK,
                LORAMESH_APP_DATA_SIZE, &SendDataFrame, (void*) NULL);
#elif defined(NODE_C)
        LoRaMesh_RegisterTransmission(6, 4000000, EVENT_TYPE_UPLINK,
                LORAMESH_APP_DATA_SIZE, &SendDataFrame, (void*) NULL);
#endif
        LoRaMesh_TestCreateMulticastGroup(AppAddr, LORAMESH_APP_TX_INTERVAL, 868100000,
                NwkSKey, AppSKey, false);
        LoRaMesh_RegisterReceptionWindow(2, LORAMESH_APP_TX_INTERVAL, &ReceiveDataFrame,
                (void*) AppAddr);
    } else if ( pLoRaDevice->devRole == COORDINATOR ) {
        /* Test child node */
        LoRaMesh_TestCreateChildNode(0x013A1024, 5000000, 868300000, NwkSKey, AppSKey);
        LoRaMesh_RegisterReceptionWindow(0, 5000000, &ReceiveDataFrame,
                (void*) 0x013A1024);
        /* Multicast group */
        LoRaMesh_TestCreateMulticastGroup(AppAddr, LORAMESH_APP_TX_INTERVAL, 868100000,
                NwkSKey, AppSKey, true);
        LoRaMesh_RegisterTransmission(2, LORAMESH_APP_TX_INTERVAL, EVENT_TYPE_MULTICAST,
                (3 * LORAMESH_APP_DATA_SIZE), &SendMulticastDataFrame, (void*) NULL);
        /* Test child node */
        LoRaMesh_TestCreateChildNode(0x013AD5F1, 4000000, 868500000, NwkSKey, AppSKey);
        LoRaMesh_RegisterReceptionWindow(6, 4000000, &ReceiveDataFrame,
                (void*) 0x013AD5F1);
    }

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

static uint8_t ProcessDataFrame( uint8_t *buf, uint8_t payloadSize, uint32_t devAddr,
        uint8_t fPort )
{
    uint8_t payloadIndex = 0;
    DataEntry_t *entry;

    LOG_TRACE("Received %u bytes from 0x%08x on port %u.", payloadSize, devAddr, fPort);

    if ( fPort != AppPort ) return ERR_NOTAVAIL;

#if defined(NODE_A)
    entry = FindEntry(devAddr);

    if ( entry == NULL ) {
        entry = AllocateEntry();
        if ( entry == NULL ) return ERR_FAILED;
    }

    entry->Timestamp = GpsGetCurrentUnixTime();
    entry->EntryInfo.Value = (uint8_t) LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++];
    /* Latitude */
    entry->LatitudeBinary = LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++];
    entry->LatitudeBinary |= ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++] & 0xFF)
            << 8);
    entry->LatitudeBinary |= ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++] & 0xFF)
            << 16);
    entry->LatitudeBinary |= ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++] & 0xFF)
            << 24);
    /* Longitude */
    entry->LongitudeBinary = LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++];
    entry->LongitudeBinary |= ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++] & 0xFF)
            << 8);
    entry->LongitudeBinary |= ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++] & 0xFF)
            << 16);
    entry->LongitudeBinary |= ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++] & 0xFF)
            << 24);
    /* Store barometric altitude if present */
    if ( entry->EntryInfo.Bits.AltitudeBar == 1 ) {
        entry->Altitude.Barometric = LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++];
        entry->Altitude.Barometric |= ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++]
                        & 0xFF) << 8);
    } else {
        entry->Altitude.Barometric = 0x00;
    }
    /* Store gps altitude if present */
    if ( entry->EntryInfo.Bits.AltitudeGPS == 1 ) {
        entry->Altitude.GPS = LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++];
        entry->Altitude.GPS |= ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++] & 0xFF)
                << 8);
    } else {
        entry->Altitude.GPS = 0x00;
    }
    /* Store barometric altitude if present */
    if ( entry->EntryInfo.Bits.VectorTrack == 1 ) {
        entry->VectorTrack.GroundSpeed = LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++];
        entry->VectorTrack.GroundSpeed |=
        ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++] & 0xFF) << 8);
        entry->VectorTrack.Track = LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++];
        entry->VectorTrack.Track |= ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++]
                        & 0xFF) << 8);
    } else {
        entry->VectorTrack.GroundSpeed = 0x00;
        entry->VectorTrack.Track = 0x00;
    }
    /* Store gps altitude if present */
    if ( entry->EntryInfo.Bits.WindSpeed == 1 ) {
        entry->WindSpeed = LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++];
        entry->WindSpeed |=
        ((LORAMESH_BUF_PAYLOAD_START(buf)[payloadIndex++] & 0xFF) << 8);
    } else {
        entry->WindSpeed = 0x00;
    }
#else
#endif
    return ERR_OK;
}

static void SendDataFrame( void* param )
{
    int32_t latiBin, longiBin;
    uint8_t dataSize = 0;

    latiBin = 0x42DEC4;
    longiBin = 0x05E868;

    AppData[dataSize++] = 0x0F;
    AppData[dataSize++] = ((latiBin) & 0xFF);
    AppData[dataSize++] = ((latiBin >> 8) & 0xFF);
    AppData[dataSize++] = ((latiBin >> 16) & 0xFF);
    AppData[dataSize++] = ((latiBin >> 24) & 0xFF);
    AppData[dataSize++] = ((longiBin) & 0xFF);
    AppData[dataSize++] = ((longiBin >> 8) & 0xFF);
    AppData[dataSize++] = ((longiBin >> 16) & 0xFF);
    AppData[dataSize++] = ((longiBin >> 24) & 0xFF);
    AppData[dataSize++] = 0xB9; /* 441 müM*/
    AppData[dataSize++] = 0x01;
    AppData[dataSize++] = 0xBC; /* 443 müM */
    AppData[dataSize++] = 0x01;
    AppData[dataSize++] = 0x32; /* 50 dm/s */
    AppData[dataSize++] = 0x00;
    AppData[dataSize++] = 0x1F; /* 287° */
    AppData[dataSize++] = 0x01;
    AppData[dataSize++] = 0x0C; /* 12 km/h */
    AppData[dataSize++] = 0x00;

    LOG_TRACE("Sending data frame at %u ms.",
            (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS));
    LoRaMesh_SendFrame(AppData, LORAMESH_APP_DATA_SIZE, AppPort, true, IsTxConfirmed);
}

static void SendMulticastDataFrame( void* param )
{
    int32_t latiBin, longiBin;
    uint32_t timestamp;
    uint8_t dataSize = 0, i;
    DataEntry_t *iterEntry;

    latiBin = 0x42DEC4;
    longiBin = 0x05E868;
    timestamp = GpsGetCurrentUnixTime();

    AppData[dataSize++] = 0x00; /* Reset SDS Header */

    /* First add this nodes data */
    AppData[dataSize++] = ((pLoRaDevice->devAddr) & 0xFF); /* Nwk addr */
    AppData[dataSize++] = ((pLoRaDevice->devAddr >> 8) & 0xFF);
    AppData[dataSize++] = ((pLoRaDevice->devAddr >> 16) & 0xFF);
    AppData[dataSize++] = ((timestamp) & 0xFF); /* Timestamp */
    AppData[dataSize++] = ((timestamp >> 8) & 0xFF);
    AppData[dataSize++] = ((timestamp >> 16) & 0xFF);
    AppData[dataSize++] = ((timestamp >> 24) & 0xFF);
    AppData[dataSize++] = 0x0F; /* Entry Info */
    AppData[dataSize++] = ((latiBin) & 0xFF); /* Latitude */
    AppData[dataSize++] = ((latiBin >> 8) & 0xFF);
    AppData[dataSize++] = ((latiBin >> 16) & 0xFF);
    AppData[dataSize++] = ((latiBin >> 24) & 0xFF);
    AppData[dataSize++] = ((longiBin) & 0xFF); /* Longitude */
    AppData[dataSize++] = ((longiBin >> 8) & 0xFF);
    AppData[dataSize++] = ((longiBin >> 16) & 0xFF);
    AppData[dataSize++] = ((longiBin >> 24) & 0xFF);
    AppData[dataSize++] = 0xB9; /* 441 müM*/
    AppData[dataSize++] = 0x01;
    AppData[dataSize++] = 0xBC; /* 443 müM */
    AppData[dataSize++] = 0x01;
    AppData[dataSize++] = 0x32; /* 50 dm/s */
    AppData[dataSize++] = 0x00;
    AppData[dataSize++] = 0x1F; /* 287° */
    AppData[dataSize++] = 0x01;
    AppData[dataSize++] = 0x0C; /* 12 km/h */
    AppData[dataSize++] = 0x00;

    /* Second, add child nodes data */
    i = 0;
    iterEntry = pDataEntries;
    while (iterEntry != NULL || iterEntry->DevAddr == 0x00) {
        AppData[dataSize++] = ((iterEntry->DevAddr) & 0xFF); /* Nwk addr */
        AppData[dataSize++] = ((iterEntry->DevAddr >> 8) & 0xFF);
        AppData[dataSize++] = ((iterEntry->DevAddr >> 16) & 0xFF);
        AppData[dataSize++] = ((iterEntry->Timestamp) & 0xFF); /* Timestamp */
        AppData[dataSize++] = ((iterEntry->Timestamp >> 8) & 0xFF);
        AppData[dataSize++] = ((iterEntry->Timestamp >> 16) & 0xFF);
        AppData[dataSize++] = ((iterEntry->Timestamp >> 24) & 0xFF);
        AppData[dataSize++] = ((iterEntry->EntryInfo.Value) & 0xFF); /* Entry Info */
        AppData[dataSize++] = ((iterEntry->LatitudeBinary) & 0xFF); /* Latitude */
        AppData[dataSize++] = ((iterEntry->LatitudeBinary >> 8) & 0xFF);
        AppData[dataSize++] = ((iterEntry->LatitudeBinary >> 16) & 0xFF);
        AppData[dataSize++] = ((iterEntry->LatitudeBinary >> 24) & 0xFF);
        AppData[dataSize++] = ((iterEntry->LongitudeBinary) & 0xFF); /* Longitude */
        AppData[dataSize++] = ((iterEntry->LongitudeBinary >> 8) & 0xFF);
        AppData[dataSize++] = ((iterEntry->LongitudeBinary >> 16) & 0xFF);
        AppData[dataSize++] = ((iterEntry->LongitudeBinary >> 24) & 0xFF);
        if ( iterEntry->EntryInfo.Bits.AltitudeGPS == 1 ) {
            AppData[dataSize++] = ((iterEntry->Altitude.GPS) & 0xFF);
            AppData[dataSize++] = ((iterEntry->Altitude.GPS >> 8) & 0xFF);
        }
        if ( iterEntry->EntryInfo.Bits.AltitudeBar == 1 ) {
            AppData[dataSize++] = ((iterEntry->Altitude.Barometric) & 0xFF);
            AppData[dataSize++] = ((iterEntry->Altitude.Barometric >> 8) & 0xFF);
        }
        if ( iterEntry->EntryInfo.Bits.VectorTrack == 1 ) {
            AppData[dataSize++] = ((iterEntry->VectorTrack.GroundSpeed) & 0xFF);
            AppData[dataSize++] = ((iterEntry->VectorTrack.GroundSpeed >> 8) & 0xFF);
            AppData[dataSize++] = ((iterEntry->VectorTrack.Track) & 0xFF);
            AppData[dataSize++] = ((iterEntry->VectorTrack.Track >> 8) & 0xFF);
        }
        if ( iterEntry->EntryInfo.Bits.WindSpeed == 1 ) {
            AppData[dataSize++] = ((iterEntry->WindSpeed) & 0xFF);
            AppData[dataSize++] = ((iterEntry->WindSpeed >> 8) & 0xFF);
        }
        i++;
        iterEntry = iterEntry->next;
    }
    AppData[0] = i + 1;

    LOG_TRACE("Sending multicast data frame at %u ms.",
            (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS));
    LoRaMesh_SendMulticast(AppData, AppDataSize, AppPort);
}

static void ReceiveDataFrame( void *param )
{
#if(LORAMESH_TEST_APP_ACTIVATED == 1)
    if(LoRaMesh_IsNetworkJoined()) {
        LoRaTest_AddDataFrame((uint32_t)param);
    }
#else
    uint32_t addr = (uint32_t) param;
    uint8_t ch, dr;
    ChildNodeInfo_t* childNode;
    MulticastGroupInfo_t* multicastGrp;

    if ( (childNode = LoRaMesh_FindChildNode(addr)) != NULL ) {
        ch = childNode->Connection.ChannelIndex;
        dr = childNode->Connection.DataRateIndex;
        LOG_TRACE("Open child node reception window at %u ms.",
                (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS));
    } else if ( (multicastGrp = LoRaMesh_FindMulticastGroup(addr)) != NULL ) {
        ch = multicastGrp->Connection.ChannelIndex;
        dr = multicastGrp->Connection.DataRateIndex;
        LOG_TRACE("Open multicast group reception window at %u ms.",
                (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS));
    } else {
        return;
    }

    LoRaMesh_TestOpenReceptionWindow(ch, dr);
#endif
}

static DataEntry_t *FindEntry( uint32_t devAddr )
{
    DataEntry_t *iterEntry = pDataEntries;

    while (iterEntry->next) {
        if ( iterEntry->DevAddr == devAddr ) {
            return iterEntry;
        }
        iterEntry = iterEntry->next;
    }

    return NULL;
}

static DataEntry_t *AllocateEntry( void )
{
    DataEntry_t *entry = pFreeDataEntries;

    if ( entry != NULL ) {
        pFreeDataEntries = entry->next;
        entry->DevAddr = 0x00;
        entry->Timestamp = 0x00;
        entry->EntryInfo.Value = 0x00;
        entry->LatitudeBinary = 0x00;
        entry->LongitudeBinary = 0x00;
        entry->Altitude.Barometric = 0x00;
        entry->Altitude.GPS = 0x00;
        entry->VectorTrack.GroundSpeed = 0x00;
        entry->VectorTrack.Track = 0x00;
        entry->WindSpeed = 0x00;
        entry->next = pDataEntries;
        pDataEntries = entry;
        return pDataEntries;
    }

    return NULL;
}

static void FreeEntry( DataEntry_t *entry )
{
    if ( pDataEntries != NULL ) {
        DataEntry_t* iterEntry, *prevEntry;

        prevEntry = NULL;
        iterEntry = pDataEntries;

        while (iterEntry != NULL) {
            if ( iterEntry == entry ) {
                if ( prevEntry == NULL ) {
                    pDataEntries = iterEntry->next;
                } else {
                    prevEntry->next = iterEntry->next;
                }
                iterEntry->next = pFreeDataEntries;
                pFreeDataEntries = iterEntry;
                break;
            }
            prevEntry = iterEntry;
            iterEntry = iterEntry->next;
        }
    }
}

static void LoRaMeshTask( void *pvParameters )
{
    uint32_t cntr = 0; /* initialize send counter */
    bool meshAppActive = true;
    appState = LORAMESH_INITIAL; /* initialize state machine state */
    (void) pvParameters; /* not used */

    LOG_DEBUG_BARE("Starting LoRaMesh application...\r\n");

    while (meshAppActive) {
        Process(); /* process state machine */
#if( OVER_THE_AIR_ACTIVATION != 0 )
        if(cntr > APP_CNTR_VALUE(LORAWAN_OTAA_INTERVAL)) {
            /* Send join request */
            LoRaMesh_JoinReq((uint8_t*)&DevEui, (uint8_t*)&AppEui, (uint8_t*)&AppKey);
#if(LORAMESH_TEST_APP_ACTIVATED == 1)
            vTaskDelay(500/portTICK_RATE_MS);
            LoRaTest_AddJoinAcc((uint8_t*)&DevEui, (uint8_t*)&AppEui, (uint8_t*)&AppKey, false);
#endif /* LORAMESH_TEST_APP_ACTIVATED */
        }
#endif /* OVER_THE_AIR_ACTIVATION */
        cntr++;
        /* Task interval of 10 ms */
        vTaskDelay(10 / portTICK_RATE_MS);
    } /* while */

    DataEntry_t *iterEntry, *prevEntry;
    iterEntry = pDataEntries;
    while (iterEntry != NULL) {
        prevEntry = iterEntry;
        FreeEntry(prevEntry);
        iterEntry = iterEntry->next;
    }
}
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
