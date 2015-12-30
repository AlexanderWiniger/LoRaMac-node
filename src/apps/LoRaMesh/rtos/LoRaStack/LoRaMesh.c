/**
 * \file LoRaMesh.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 08.12.2015
 * \version 1.0
 *
 * \brief LoRa stack medium access layer implementation
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "board.h"

#include "LoRaMacCrypto.h"
#include "LoRaMesh.h"

#define LOG_LEVEL_TRACE
#include "debug.h"
/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
#define MAX_NOF_SCHEDULER_EVENTS            LORAMESH_CONFIG_MAX_NOF_SCHEDULER_EVENTS
#define MAX_NOF_SCHEDULER_EVENT_HANDLERS    LORAMESH_CONFIG_MAX_NOF_SCHEDULER_EVENT_HANDLERS
#define MAX_NOF_MULTICAST_GROUPS            LORAMESH_CONFIG_MAX_NOF_MULTICAST_GROUPS
#define MAX_NOF_CHILD_NODES                 LORAMESH_CONFIG_MAX_NOF_CHILD_NODES
#define MAX_NOF_PORT_HANDLERS               LORAMESH_CONFIG_MAX_NOF_PORT_HANDLERS

#define ADVERTISING_INTERVAL                (30000000)
#define ADVERTISING_GUARD_TIME              (2280000)
#define ADVERTISING_RESERVED_TIME           (2120000)
#define AVAILABLE_SLOT_TIME                 (ADVERTISING_INTERVAL-ADVERTISING_GUARD_TIME-ADVERTISING_RESERVED_TIME)
#define TIME_PER_SLOT                       (50000)
#define NOF_AVAILABLE_SLOTS                 (AVAILABLE_SLOT_TIME / TIME_PER_SLOT)

#define UPLINK_RESERVED_TIME                (50000)
#define MULTICAST_RESERVED_TIME             (250000)
#define RECEPTION_RESERVED_TIME             (50000)
/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
/*! LoRaMesh upper layer event functions */
static LoRaMeshCallbacks_t *LoRaMeshCallbacks;

/*! LoRaWAN device pointer */
static LoRaDevice_t LoRaDevice = {
    .devAddr = 0x00,
    .netId = 0x00,
    .devAddr = 0x00,
    .devEui = NULL,
    .devNonce = 0x00,
    .devClass = CLASS_A,
    .devRole = NODE,
    .coordinatorAddr = 0x00,
    .appEui = NULL,
    .appKey = NULL,
    .childNodes = NULL,
    .multicastGroups = NULL,
    .channelsMask = {0},
    .currChannelIndex = 0u,
    .currDataRateIndex = LORAMAC_DEFAULT_DATARATE,
    .currTxPowerIndex = LORAMAC_DEFAULT_TX_POWER,
    .adrAckCounter = 0u,
    .nbRep = 1u,
    .nbRepCounter = 0u,
    .macCmdBuffer = {0},
    .macCmdBufferIndex = 0u,
    .upLinkSlot = {
        .Address = 0x00,
        .AppSKey = {0},
        .NwkSKey = {0},
        .UpLinkCounter = 1u,
        .DownLinkCounter = 0u,
        .ChannelIndex = 0u,
        .DataRateIndex = LORAMAC_DEFAULT_DATARATE,
        .TxPowerIndex = LORAMAC_DEFAULT_TX_POWER
    },
    .advertisingSlot = {
        .Time = 0u,
        .Interval = 0u,
        .Duration = 0u,
    },
    .ctrlFlags.Value = 0u, /* Device flags */
    .dbgFlags.Value = 0u /* Debug flags */
};

/*!
 * LoRaMesh Event Scheduler
 */
/*! Event scheduler events list */
static LoRaSchedulerEvent_t eventSchedulerList[MAX_NOF_SCHEDULER_EVENTS];
static LoRaSchedulerEvent_t *pEventScheduler;
static LoRaSchedulerEvent_t *pNextSchedulerEvent;
static LoRaSchedulerEvent_t *pNextFreeSchedulerEvent;
/*! Event scheduler event handler list */
static LoRaSchedulerEventHandler_t eventHandlerList[MAX_NOF_SCHEDULER_EVENT_HANDLERS];
static LoRaSchedulerEventHandler_t *pEventHandlers;
static LoRaSchedulerEventHandler_t *pNextFreeEventHandler;
/*! Event scheduler timer*/
static TimerEvent_t EventSchedulerTimer;

/*! Advertising timer */
static TimerEvent_t AdvertisingTimer;

/*! Multicast groups */
static MulticastGroupInfo_t multicastGrpList[MAX_NOF_MULTICAST_GROUPS];
static MulticastGroupInfo_t *pNextFreeMulticastGrp;

/*! Child nodes */
static ChildNodeInfo_t childNodeList[MAX_NOF_CHILD_NODES];
static ChildNodeInfo_t *pNextFreeChildNode;

/*! Rx message handlers */
static PortHandler_t portHandlers[MAX_NOF_PORT_HANDLERS];
static PortHandler_t *pPortHandlers;
static PortHandler_t *pNextFreePortHandler;
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*! \brief Evaluates probability of a node to nominate itself as coordinator. */
static bool EvaluateAcceptanceProbability( int32_t latiBin, int32_t longiBin );

/*! \brief Evaluates probability of a node to nominate itself as coordinator. */
static bool EvaluateNominationProbability( uint8_t nodeRank, DeviceClass_t nodeClass );

/*! \brief Calculate the nodes rank. */
static uint8_t CalculateNodeRank( void );

/*!  */
static void OnMulticastSchedulerEvent( void *param );

/*!  */
static void OnReceptionSchedulerEvent( void *param );

/*!  */
static void OnUplinkSchedulerEvent( void *param );

/*! \brief Reception window 1 scheduler event */
static void OnRx1SchedulerEvent( void *param );

/*! \brief Reception window 2 scheduler event */
static void OnRx2SchedulerEvent( void *param );

/*! \brief Function executed on advertising timer event */
static void OnAdvertisingTimerEvent( TimerHandle_t xTimer );

/*! brief Function executed on event scheduler timer event */
static void OnEventSchedulerTimerEvent( TimerHandle_t xTimer );

/*! \brief Schedule new event */
static uint8_t ScheduleEvent( LoRaSchedulerEventHandler_t *evtHandler, TimerTime_t interval );

/*! \brief Remove scheduler event */
static uint8_t RemoveEvent( LoRaSchedulerEventHandler_t *evtHandler );

/*! \brief Find free slots for given event */
static uint8_t FindFreeSlots( TimerTime_t interval, uint16_t durationInSlots,
        uint8_t *nofAllocatedSlots, uint16_t *allocatedSlots, bool scheduleRxWindows );

/*! \brief Allocate new scheduler event */
static LoRaSchedulerEvent_t *AllocateEvent( void );

/*! \brief Free allocated scheduler event */
static void FreeEvent( LoRaSchedulerEvent_t *evt );

/*! \brief Allocate scheduler event handler */
static LoRaSchedulerEventHandler_t * AllocateEventHandler( void );

/*! \brief Free allocated scheduler event handler */
static void FreeEventHandler( LoRaSchedulerEventHandler_t *evtHandler );

/*! \brief Create new child node with given data. */
ChildNodeInfo_t* CreateChildNode( uint32_t devAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        uint32_t channel, uint32_t interval );

/*! \brief Add child node at the tail of the list. */
void ChildNodeAdd( ChildNodeInfo_t* childNode );

/*! \brief Remove child node from the list. */
void ChildNodeRemove( ChildNodeInfo_t* childNode );

/*! \brief Find child node from the list. */
ChildNodeInfo_t *ChildNodeFind( uint32_t childAddr );

/*! \brief Create new child node with given data. */
MulticastGroupInfo_t* CreateMulticastGroup( uint32_t grpAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        uint32_t channel, uint32_t interval );

/*! \brief Add mutlicast group. */
void MulticastGroupAdd( MulticastGroupInfo_t *multicastGrp );

/*! \brief Remove multicast group. */
void MulticastGroupRemove( MulticastGroupInfo_t *multicastGrp );

/*! \brief Find multicast group from the list. */
MulticastGroupInfo_t *MulticastGroupFind( uint32_t grpAddr );

/*! \brief Print status */
static uint8_t PrintStatus( Shell_ConstStdIO_t *io );

/*! \brief Print help */
static uint8_t PrintHelp( Shell_ConstStdIO_t *io );

/*! \brief Print event scheduler list */
static uint8_t PrintEventSchedulerList( Shell_ConstStdIO_t *io );

/*! \brief Print child node information. */
static uint8_t PrintChildNodes( Shell_ConstStdIO_t *io );

/*! \brief Print multicast group information. */
static uint8_t PrintMulticastGroups( Shell_ConstStdIO_t *io );

/*******************************************************************************
 * MODULE VARIABLES (PUBLIC)
 ******************************************************************************/
/*! LoRaWAN device pointer */
LoRaDevice_t* pLoRaDevice;

/*! Data rates table definition */
const uint8_t Datarates[] = { 12, 11, 10, 9, 8, 7, 7, 50 };

/*! Maximum payload with respect to the datarate index. Cannot operate with repeater. */
const uint8_t MaxPayloadByDatarate[] = { 51, 51, 51, 115, 242, 242, 242, 242 };

/*! Tx output powers table definition */
const uint8_t TxPowers[] = { 20, 14, 11, 8, 5, 2 };

/*! LoRa device used throughout the stack */
extern LoRaDevice_t* pLoRaDevice;

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
byte LoRaMesh_ParseCommand( const unsigned char *cmd, bool *handled, Shell_ConstStdIO_t *io )
{
    if ( strcmp((char*) cmd, SHELL_CMD_HELP) == 0 || strcmp((char*) cmd, "LoRaMesh help") == 0 ) {
        *handled = true;
        return PrintHelp(io);
    } else if ( (strcmp((char*) cmd, SHELL_CMD_STATUS) == 0)
            || (strcmp((char*) cmd, "LoRaMesh status") == 0) ) {
        *handled = true;
        return PrintStatus(io);
    } else if ( (strcmp((char*) cmd, "LoRaMesh childnodes") == 0) ) {
        *handled = true;
        return PrintChildNodes(io);
    } else if ( (strcmp((char*) cmd, "LoRaMesh multicastgroups") == 0) ) {
        *handled = true;
        return PrintMulticastGroups(io);
    }
    return ERR_OK;
}

void LoRaMesh_Init( LoRaMeshCallbacks_t *callbacks )
{
    LoRaSchedulerEventHandler_t *handler;
    uint8_t i;

    LoRaMeshCallbacks = callbacks;

    /* Assign LoRa device structure pointer */
    pLoRaDevice = (LoRaDevice_t*) &LoRaDevice;

    /* Create a list with rx message handler */
    pPortHandlers = NULL;
    pNextFreePortHandler = portHandlers;
    for ( i = 0; i < MAX_NOF_PORT_HANDLERS - 1; i++ ) {
        portHandlers[i].nextPortHandler = &portHandlers[i + 1];
    }

    /* Init scheduler list */
    pEventScheduler = NULL;
    pNextSchedulerEvent = NULL;
    pNextFreeSchedulerEvent = eventSchedulerList;
    for ( i = 0; i < MAX_NOF_SCHEDULER_EVENTS - 1; i++ ) {
        eventSchedulerList[i].nextSchedulerEvent = &eventSchedulerList[i + 1];
    }
    /* Init event handlers list */
    pEventHandlers = NULL;
    pNextFreeEventHandler = eventHandlerList;
    for ( i = 0; i < MAX_NOF_SCHEDULER_EVENT_HANDLERS - 1; i++ ) {
        eventHandlerList[i].nextEventHandler = &eventHandlerList[i + 1];
    }
    /* Allocate rx1 window handler */
    handler = AllocateEventHandler();
    handler->eventType = EVENT_TYPE_RX1_WINDOW;
    handler->callback = OnRx1SchedulerEvent;
    handler->param = (void*) NULL;
    /* Allocate rx2 window handler */
    handler = AllocateEventHandler();
    handler->eventType = EVENT_TYPE_RX2_WINDOW;
    handler->callback = OnRx2SchedulerEvent;
    handler->param = (void*) NULL;

    /* Event scheduler timer */
    TimerInit(&EventSchedulerTimer, "EventSchedulerTimer", (void*) NULL, OnEventSchedulerTimerEvent,
            false);

    /* Advertising timer */
    TimerInit(&AdvertisingTimer, "AdvertisingTimer", (void*) NULL, OnAdvertisingTimerEvent, true);
    TimerSetValue(&AdvertisingTimer, LORAMESH_CONFIG_ADV_INTERVAL);
    TimerStart(&AdvertisingTimer);

    /* Init child node list */
    pNextFreeChildNode = childNodeList;
    for ( i = 0; i < MAX_NOF_CHILD_NODES - 1; i++ ) {
        childNodeList[i].nextSlot = &childNodeList[i + 1];
    }

    /* Init multicast group list */
    pNextFreeMulticastGrp = multicastGrpList;
    for ( i = 0; i < MAX_NOF_MULTICAST_GROUPS - 1; i++ ) {
        multicastGrpList[i].nextSlot = &multicastGrpList[i + 1];
    }

    /* Initialize stack */
    LoRaFrm_Init();
    LoRaMac_Init(LoRaMeshCallbacks->GetBatteryLevel);
    LoRaPhy_Init();
#if 0
    /* Reception window */
    handler = AllocateEventHandler();
    handler->callback = OnReceptionSchedulerEvent;
    handler->eventType = EVENT_TYPE_SYNCH_RX_WINDOW;
    ScheduleEvent(handler, 4e6);
    /* Multicast window */
    handler = AllocateEventHandler();
    handler->callback = OnMulticastSchedulerEvent;
    handler->eventType = EVENT_TYPE_MULTICAST;
    ScheduleEvent(handler, 8e6);
    /* Uplink window */
    handler = AllocateEventHandler();
    handler->callback = OnUplinkSchedulerEvent;
    handler->eventType = EVENT_TYPE_UPLINK;
    ScheduleEvent(handler, 6e6);
    /* Reception window */
    handler = AllocateEventHandler();
    handler->callback = OnReceptionSchedulerEvent;
    handler->eventType = EVENT_TYPE_SYNCH_RX_WINDOW;
    ScheduleEvent(handler, 5e6);
#endif
}

uint8_t LoRaMesh_RegisterApplication( PortHandlerFunction_t fHandler, uint8_t fPort )
{
    PortHandler_t *handler;

    if ( fPort < 1 || fPort > 223 ) return ERR_RANGE;

    handler = pPortHandlers;

    /* Check if port is already registered */
    while ( handler != NULL ) {
        if ( handler->fPort == fPort ) return ERR_NOTAVAIL;
        handler = handler->nextPortHandler;
    }

    /* Get next free port handler */
    handler = pNextFreePortHandler;
    if ( handler == NULL ) return ERR_FAILED;

    /* Populate port handler */
    handler->fPort = fPort;
    handler->fHandler = fHandler;

    /* Add to port handler list */
    handler->nextPortHandler = pPortHandlers;
    pPortHandlers = handler;

    return ERR_OK;
}

uint8_t LoRaMesh_RemoveApplication( uint8_t fPort )
{
    PortHandler_t *handler;

    if ( fPort < 1 || fPort > 223 ) return ERR_RANGE;

    handler = pPortHandlers;

    /* Find port handler is already registered */
    while ( handler != NULL ) {
        if ( handler->fPort == fPort ) {
            /* Remove port handler */
            handler->fPort = 0;
            handler->fHandler = NULL;
            handler->nextPortHandler = pNextFreePortHandler;
            pNextFreePortHandler = handler;
            return ERR_OK;
        }
        handler = handler->nextPortHandler;
    }

    return ERR_FAILED;
}

uint8_t LoRaMesh_RegisterTransmission( uint32_t interval, void (*callback)( void *param ),
        void* param )
{
    LoRaSchedulerEventHandler_t * evtHandler;
    uint8_t result;

    evtHandler = AllocateEventHandler();
    if ( evtHandler == NULL ) {
        LOG_ERROR("Unable to allocate event handlers.");
        return ERR_NOTAVAIL;
    }
    /* Initialize event handler */
    evtHandler->eventInterval = interval;
    evtHandler->eventType = EVENT_TYPE_UPLINK;
    evtHandler->callback = callback;
    evtHandler->param = param;

    if ( (result = ScheduleEvent(evtHandler, (TimerTime_t) interval)) != ERR_OK )
        LOG_ERROR("Unable to schedule events.");

    (PrintEventSchedulerList(Shell_GetStdio()));
    return result;
}

uint8_t LoRaMesh_RemoveTransmission( uint32_t interval, void (*callback)( void *param ) )
{
    LoRaSchedulerEventHandler_t * evtHandler;
    uint8_t result;

    while ( evtHandler != NULL ) {
        if ( (evtHandler->eventInterval == interval) && (evtHandler->callback == callback) ) {
            if ( (result = RemoveEvent(evtHandler)) == ERR_OK ) FreeEventHandler(evtHandler);
        }
        evtHandler = evtHandler->nextEventHandler;
    }

    return result;
}

uint8_t LoRaMesh_SendFrame( uint8_t *appPayload, size_t appPayloadSize, uint8_t fPort,
        bool isUpLink, bool isConfirmed )
{
    uint8_t i, buf[LORAMESH_BUFFER_SIZE];

    if ( !LoRaMesh_IsNetworkJoined() ) {
        return ERR_NOTAVAIL;   // No network has been joined yet
    }

    if ( appPayloadSize > LORAMESH_PAYLOAD_SIZE ) {
        return ERR_OVERFLOW; /* block too large for payload */
    }

    i = 0;
    while ( i < appPayloadSize ) {
        buf[LORAMESH_BUF_PAYLOAD_START(buf) + i] = *appPayload;
        appPayload++;
        i++;
    }

    return LoRaMesh_PutPayload(buf, sizeof(buf), appPayloadSize, fPort);
}

uint8_t LoRaMesh_SendMulticast( uint8_t *appPayload, size_t appPayloadSize, uint8_t fPort )
{
    uint8_t i, buf[LORAMESH_BUFFER_SIZE];

    if ( !LoRaMesh_IsNetworkJoined() ) {
        return ERR_NOTAVAIL;   // No network has been joined yet
    }

    if ( appPayloadSize > LORAMESH_PAYLOAD_SIZE ) {
        return ERR_OVERFLOW; /* block too large for payload */
    }

    i = 0;
    while ( i < appPayloadSize ) {
        buf[LORAMESH_BUF_PAYLOAD_START(buf) + i] = *appPayload;
        appPayload++;
        i++;
    }

    return LoRaMesh_PutPayload(buf, sizeof(buf), appPayloadSize, fPort);
}

uint8_t LoRaMesh_SendAdvertising( void )
{
    uint8_t buf[LORAMESH_ADVERTISING_MSG_LENGTH + LORAPHY_HEADER_SIZE], payloadSize, phyFlags,
            devRank;
    int32_t latiBin, longiBin;

    payloadSize = 0;
    phyFlags = LORAPHY_PACKET_FLAGS_ADVERTISING;

    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->devAddr) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->devAddr >> 8) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->devAddr >> 16) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->devAddr >> 24) & 0xFF;

    /* Device info */
    devRank = CalculateNodeRank();
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = ((((pLoRaDevice->devRole) & 0xF) << 4)
            | (devRank & 0xF));

    GpsGetLatestGpsPositionBinary(&latiBin, &longiBin);
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (latiBin) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (latiBin >> 8) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (latiBin >> 16) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (latiBin >> 24) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (longiBin) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (longiBin >> 8) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (longiBin >> 16) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (longiBin >> 24) & 0xFF;

    /* Coordinator address */
    if ( pLoRaDevice->coordinatorAddr == 0x00
            && EvaluateNominationProbability(0, (DeviceClass_t) 0) ) {
        buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->devAddr) & 0xFF;
        buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->devAddr >> 8) & 0xFF;
        buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->devAddr >> 16) & 0xFF;
        buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->devAddr >> 24) & 0xFF;
    } else {
        buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->coordinatorAddr) & 0xFF;
        buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->coordinatorAddr >> 8) & 0xFF;
        buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->coordinatorAddr >> 16)
                & 0xFF;
        buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->coordinatorAddr >> 24)
                & 0xFF;
    }

    /* Advertising slot info */
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->advertisingSlot.Time) & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->advertisingSlot.Time >> 8)
            & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->advertisingSlot.Time >> 16)
            & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (pLoRaDevice->advertisingSlot.Time >> 24)
            & 0xFF;
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (uint8_t)(
            pLoRaDevice->advertisingSlot.Interval / 1e6);
    buf[LORAPHY_BUF_IDX_PAYLOAD + (payloadSize++)] = (uint8_t)(
            pLoRaDevice->advertisingSlot.Duration / 1e6);

    return LoRaPhy_PutPayload(buf, sizeof(buf), payloadSize, phyFlags);
}

uint8_t LoRaMesh_ProcessAdvertising( uint8_t *aPayload, uint8_t aPayloadSize )
{
    uint32_t coordAddr = 0x00, devAddr = 0x00;
    uint8_t rank, role;

    devAddr |= (aPayload[LORAMESH_ADVERITSING_DEV_ADR_IDX]);
    devAddr |= (aPayload[LORAMESH_ADVERITSING_DEV_ADR_IDX + 1] << 8);
    devAddr |= (aPayload[LORAMESH_ADVERITSING_DEV_ADR_IDX + 2] << 16);
    devAddr |= (aPayload[LORAMESH_ADVERITSING_DEV_ADR_IDX + 3] << 24);

    rank = (aPayload[LORAMESH_ADVERITSING_ROLE_RANK_IDX] & 0x0F);
    role = ((aPayload[LORAMESH_ADVERITSING_ROLE_RANK_IDX] & 0xF0) >> 4);

    coordAddr |= (aPayload[LORAMESH_ADVERITSING_COORD_ADR_IDX]);
    coordAddr |= (aPayload[LORAMESH_ADVERITSING_COORD_ADR_IDX + 1] << 8);
    coordAddr |= (aPayload[LORAMESH_ADVERITSING_COORD_ADR_IDX + 2] << 16);
    coordAddr |= (aPayload[LORAMESH_ADVERITSING_COORD_ADR_IDX + 3] << 24);

    if ( pLoRaDevice->coordinatorAddr != coordAddr ) {
        if ( coordAddr != 0x00 ) {
            if ( coordAddr != devAddr
                    && EvaluateNominationProbability(rank, (DeviceClass_t) role) ) {
                /* Node will nominate itself */
                pLoRaDevice->coordinatorAddr = pLoRaDevice->devAddr;
            } else {
                /* Elect propagated coordinator */
                pLoRaDevice->coordinatorAddr = coordAddr;
            }
        } else {

        }
    } else {
        if ( coordAddr == 0x00 ) {
            if ( EvaluateNominationProbability(rank, (DeviceClass_t) role) ) {
                /* Node will nominate itself */
                pLoRaDevice->coordinatorAddr = pLoRaDevice->devAddr;
            } else {
                /* Propagate loss of former coordinator */
                pLoRaDevice->coordinatorAddr = 0x00;
            }
        }
    }
    return ERR_OK;
}

uint8_t LoRaMesh_JoinReq( uint8_t * devEui, uint8_t * appEui, uint8_t * appKey )
{
    uint8_t mPayloadSize = 0, mPayload[LORAMAC_BUFFER_SIZE];

    /* Store values for key generation */
    pLoRaDevice->devNonce = LoRaPhy_GenerateNonce();
    pLoRaDevice->appEui = appEui;
    pLoRaDevice->devEui = devEui;
    pLoRaDevice->appKey = appKey;

    memcpy(&LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize], appEui, 8);
    mPayloadSize += 8;
    memcpy(&LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize], devEui, 8);
    mPayloadSize += 8;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = pLoRaDevice->devNonce & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (pLoRaDevice->devNonce >> 8) & 0xFF;

    return LoRaMac_PutPayload((uint8_t*) &mPayload, sizeof(mPayload), mPayloadSize,
            MSG_TYPE_JOIN_REQ);
}

uint8_t LoRaMesh_JoinMeshReq( uint8_t * devEui, uint8_t * appEui, uint8_t * appKey )
{
    uint8_t mPayloadSize = 0, mPayload[LORAMAC_BUFFER_SIZE];
    int32_t latiBin, longiBin;

    /* Store values for key generation */
    pLoRaDevice->devNonce = LoRaPhy_GenerateNonce();
    pLoRaDevice->appEui = appEui;
    pLoRaDevice->devEui = devEui;
    pLoRaDevice->appKey = appKey;

    memcpy(&LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize], appEui, 8);
    mPayloadSize += 8;
    memcpy(&LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize], devEui, 8);
    mPayloadSize += 8;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = pLoRaDevice->devNonce & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (pLoRaDevice->devNonce >> 8) & 0xFF;

    GpsGetLatestGpsPositionBinary(&latiBin, &longiBin);
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (latiBin) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (latiBin >> 8) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (latiBin >> 16) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (latiBin >> 24) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (longiBin) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (longiBin >> 8) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (longiBin >> 16) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (longiBin >> 24) & 0xFF;

    return LoRaMac_PutPayload((uint8_t*) &mPayload, sizeof(mPayload), mPayloadSize,
            MSG_TYPE_JOIN_REQ);
}

uint8_t LoRaMesh_ProcessJoinMeshReq( uint8_t *payload, uint8_t payloadSize )
{
    uint8_t appEui[8], devEui[8], *nwkSKey, *appSKey;
    int32_t latiBin, longiBin;
    uint16_t devNonce = 0x00;

    memcpy(appEui, payload[LORAMAC_BUF_IDX_PAYLOAD], 8);
    memcpy(devEui, payload[LORAMAC_BUF_IDX_PAYLOAD + 8], 8);

    devNonce |= (payload[LORAMAC_BUF_IDX_PAYLOAD + 16] & 0xFF);
    devNonce |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 17] >> 8) & 0xFF);

    latiBin = (payload[LORAMAC_BUF_IDX_PAYLOAD + 18] & 0xFF);
    latiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 19] >> 8) & 0xFF);
    latiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 20] >> 16) & 0xFF);
    latiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 21] >> 24) & 0xFF);
    longiBin = (payload[LORAMAC_BUF_IDX_PAYLOAD + 22] & 0xFF);
    longiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 23] >> 8) & 0xFF);
    longiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 24] >> 16) & 0xFF);
    longiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 25] >> 24) & 0xFF);

    if ( EvaluateAcceptanceProbability(latiBin, longiBin) ) {
        ChildNodeInfo_t* newChild;
        newChild = CreateChildNode(LoRaMesh_GenerateDeviceAddress(devNonce), nwkSKey, appSKey, 0,
                0);
        if ( newChild == NULL ) return ERR_FAILED;
        ChildNodeAdd(newChild);
        if ( pLoRaDevice->devRole == NODE ) pLoRaDevice->devRole = ROUTER;
    }

    return ERR_OK;
}

uint8_t LoRaMesh_RebindMeshReq( void )
{
    uint8_t mPayloadSize = 0, mPayload[LORAMAC_BUFFER_SIZE];
    int32_t latiBin, longiBin;

    /* Store values for key generation */
    pLoRaDevice->devNonce = LoRaPhy_GenerateNonce();

    memcpy(&LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize], pLoRaDevice->appEui, 8);
    mPayloadSize += 8;
    memcpy(&LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize], pLoRaDevice->devEui, 8);
    mPayloadSize += 8;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = pLoRaDevice->devNonce & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (pLoRaDevice->devNonce >> 8) & 0xFF;

    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (pLoRaDevice->devAddr) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (pLoRaDevice->devAddr >> 8) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (pLoRaDevice->devAddr >> 16) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (pLoRaDevice->devAddr >> 24) & 0xFF;

    GpsGetLatestGpsPositionBinary(&latiBin, &longiBin);
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (latiBin) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (latiBin >> 8) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (latiBin >> 16) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (latiBin >> 24) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (longiBin) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (longiBin >> 8) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (longiBin >> 16) & 0xFF;
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (longiBin >> 24) & 0xFF;

    return LoRaMac_PutPayload((uint8_t*) &mPayload, sizeof(mPayload), mPayloadSize,
            MSG_TYPE_JOIN_REQ);
}

uint8_t LoRaMesh_ProcessRebindMeshReq( uint8_t *payload, uint8_t payloadSize )
{
    uint8_t appEui[8], devEui[8];
    int32_t latiBin, longiBin;
    uint32_t devAddr;
    uint16_t devNonce;

    memcpy(appEui, payload[LORAMAC_BUF_IDX_PAYLOAD], 8);
    memcpy(devEui, payload[LORAMAC_BUF_IDX_PAYLOAD + 8], 8);

    devNonce = (payload[LORAMAC_BUF_IDX_PAYLOAD + 16] & 0xFF);
    devNonce |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 17] >> 8) & 0xFF);

    latiBin = (payload[LORAMAC_BUF_IDX_PAYLOAD + 18] & 0xFF);
    latiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 19] >> 8) & 0xFF);
    latiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 20] >> 16) & 0xFF);
    latiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 21] >> 24) & 0xFF);
    longiBin = (payload[LORAMAC_BUF_IDX_PAYLOAD + 22] & 0xFF);
    longiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 23] >> 8) & 0xFF);
    longiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 24] >> 16) & 0xFF);
    longiBin |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 25] >> 24) & 0xFF);

    devAddr = (payload[LORAMAC_BUF_IDX_PAYLOAD + 26] & 0xFF);
    devAddr |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 27] >> 8) & 0xFF);
    devAddr |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 28] >> 16) & 0xFF);
    devAddr |= ((payload[LORAMAC_BUF_IDX_PAYLOAD + 29] >> 24) & 0xFF);

    if ( EvaluateAcceptanceProbability(latiBin, longiBin) ) {

    }

    return ERR_OK;
}

uint8_t LoRaMesh_LinkCheckReq( void )
{
    return ERR_OK;
}

uint8_t LoRaMesh_OnPacketRx( uint8_t *buf, uint8_t payloadSize, uint8_t fPort )
{
    if ( fPort >= LORAFRM_LOWEST_FPORT && fPort <= LORAFRM_HIGHEST_FPORT ) {
        PortHandler_t *iterHandler = pPortHandlers;
        while ( iterHandler != NULL ) {
            if ( iterHandler->fPort == fPort && iterHandler->fHandler != NULL ) {
                return iterHandler->fHandler(buf, payloadSize, fPort);
            }
            iterHandler = iterHandler->nextPortHandler;
        }
    }

    return ERR_FAILED;
}

uint8_t LoRaMesh_PutPayload( uint8_t* buf, uint16_t bufSize, uint8_t payloadSize, uint8_t fPort )
{
    /* Add app information */
#if(LORA_DEBUG_OUTPUT_PAYLOAD == 1)
    LOG_TRACE("%s - Size %d", __FUNCTION__, payloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < payloadSize; i++ )
    LOG_TRACE_BARE("0x%02x ", buf[i]);
    LOG_TRACE_BARE("\r\n");
#endif
    return LoRaFrm_PutPayload(buf, bufSize, payloadSize, fPort, FRM_TYPE_REGULAR, UP_LINK, false,
            false);
}

bool LoRaMesh_IsNetworkJoined( void )
{
    if ( pLoRaDevice != NULL ) {
        return (pLoRaDevice->ctrlFlags.Bits.nwkJoined == 1U);
    } else {
        return false;
    }
}

uint32_t LoRaMesh_GenerateDeviceAddress( uint16_t nonce )
{
    return (uint32_t)(0x13 | nonce);
}

ChildNodeInfo_t* LoRaMesh_FindChildNode( uint32_t devAddr )
{
    if ( pLoRaDevice->childNodes != NULL ) {
        ChildNodeInfo_t *childNode = pLoRaDevice->childNodes;

        while ( childNode != NULL ) {
            if ( childNode->Connection.Address == devAddr ) return childNode;
            childNode = childNode->nextSlot;
        }
    }

    return NULL;
}

MulticastGroupInfo_t* LoRaMesh_FindMulticastGroup( uint32_t grpAddr )
{
    if ( pLoRaDevice->multicastGroups != NULL ) {
        MulticastGroupInfo_t *grp = pLoRaDevice->multicastGroups;

        while ( grp != NULL ) {
            if ( grp->Connection.Address == grpAddr ) return grp;
            grp = grp->nextSlot;
        }
    }

    return NULL;
}

/*******************************************************************************
 * PUBLIC SETUP FUNCTIONS
 ******************************************************************************/
void LoRaMesh_SetNwkIds( uint32_t netID, uint32_t devAddr, uint8_t *nwkSKey, uint8_t *appSKey )
{
    pLoRaDevice->netId = netID;
    pLoRaDevice->devAddr = devAddr;

    for ( uint8_t i = 0; i < 16; i++ ) {
        pLoRaDevice->upLinkSlot.AppSKey[i] = appSKey[i];
        pLoRaDevice->upLinkSlot.NwkSKey[i] = nwkSKey[i];
    }

    pLoRaDevice->ctrlFlags.Bits.nwkJoined = 1;
}

void LoRaMesh_SetDeviceClass( DeviceClass_t devClass )
{
    pLoRaDevice->devClass = devClass;
}

void LoRaMesh_SetDeviceRole( DeviceRole_t devRole )
{
    pLoRaDevice->devRole = devRole;
}

void LoRaMesh_SetPublicNetwork( bool enable )
{
    if ( pLoRaDevice != NULL ) {
        pLoRaDevice->ctrlFlags.Bits.nwkPublic = (enable ? 1U : 0U);
    }
}

void LoRaMesh_SetAdrOn( bool enable )
{
    if ( pLoRaDevice != NULL ) {
        pLoRaDevice->ctrlFlags.Bits.adrCtrlOn = (enable ? 1U : 0U);
    }
}

/*******************************************************************************
 * TEST FUNCTIONS (PUBLIC) (FOR DEBUG PURPOSES ONLY)
 ******************************************************************************/
void LoRaMesh_TestSetDutyCycleCtrlOff( bool enable )
{
    if ( pLoRaDevice != NULL ) {
        pLoRaDevice->dbgFlags.Bits.dutyCycleCtrlOff = (enable ? 1U : 0U);
    }
}

void LoRaMesh_TestSetRxWindowsOn( bool enable )
{
    if ( pLoRaDevice != NULL ) {
        pLoRaDevice->dbgFlags.Bits.rxWindowsDisabled = (enable ? 0U : 1U);
    }
}

void LoRaMesh_TestSetMic( uint16_t upLinkCounter )
{
    if ( pLoRaDevice != NULL ) {
        pLoRaDevice->upLinkSlot.UpLinkCounter = upLinkCounter;
        pLoRaDevice->dbgFlags.Bits.upLinkCounterFixed = 1;
    }
}

void LoRaMesh_TestCreateChildNode( uint32_t devAddr, uint32_t interval, uint32_t freqChannel,
        uint8_t *nwkSKey, uint8_t *appSKey )
{
    ChildNodeInfo_t* newChildNode = CreateChildNode(devAddr, interval, freqChannel, nwkSKey,
            appSKey);

    if ( newChildNode != NULL ) {
        ChildNodeAdd(newChildNode);
    }
}

void LoRaMesh_TestCreateMulticastGroup( uint32_t grpAddr, uint32_t interval, uint32_t freqChannel,
        uint8_t *nwkSKey, uint8_t *appSKey )
{
    MulticastGroupInfo_t newGrp = CreateMulticastGroup(grpAddr, interval, freqChannel, nwkSKey,
            appSKey);

    if ( newChildNode != NULL ) {
        MulticastGroupAdd(newGrp);
        ScheduleEvent()
    }
}
/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
/*!
 * Schedule a LoRaMesh event
 *
 * \param[OUT] eHandler Allocated event handler
 * \param[IN] interval Event interval
 *
 * \retval status ERR_OK Event was successfully scheduled
 */
static uint8_t ScheduleEvent( LoRaSchedulerEventHandler_t *evtHandler, TimerTime_t interval )
{
    LoRaSchedulerEvent_t *evt, *prevEvt = NULL;
    uint16_t durationInSlots;
    uint16_t allocatedSlots[32];
    bool receptionWindows = false;
    uint8_t i, j, nofAllocatedSlots;

    switch ( evtHandler->eventType ) {
        case EVENT_TYPE_UPLINK:
            durationInSlots = UPLINK_RESERVED_TIME / TIME_PER_SLOT;
            receptionWindows = true;
            break;
        case EVENT_TYPE_MULTICAST:
            durationInSlots = MULTICAST_RESERVED_TIME / TIME_PER_SLOT;
            break;
        case EVENT_TYPE_SYNCH_RX_WINDOW:
            durationInSlots = RECEPTION_RESERVED_TIME / TIME_PER_SLOT;
            break;
        default:
            break;
    }

    if ( FindFreeSlots(interval, durationInSlots, (uint8_t*) &nofAllocatedSlots,
            (uint16_t*) &allocatedSlots, receptionWindows) != ERR_OK ) return ERR_FAILED;

    for ( i = 0; i < nofAllocatedSlots; i++ ) {
        evt = AllocateEvent();

        if ( evt == NULL ) {
            LOG_ERROR("Unable to allocate event.");
            return ERR_NOTAVAIL;
        }

        evt->eventHandler = evtHandler;
        evt->startSlot = allocatedSlots[i];
        evt->endSlot = evt->startSlot + durationInSlots;

        if ( pEventScheduler == NULL ) {
            evt->nextSchedulerEvent = NULL;
            pNextSchedulerEvent = evt;
            pEventScheduler = evt;

            TimerSetValue(&EventSchedulerTimer, ADVERTISING_RESERVED_TIME);
            TimerStart(&EventSchedulerTimer);
        } else {
            LoRaSchedulerEvent_t *iterEvt = pEventScheduler;
            while ( iterEvt != NULL ) {
                if ( ((iterEvt->endSlot < evt->startSlot) && (iterEvt->nextSchedulerEvent != NULL)
                        && (iterEvt->nextSchedulerEvent->startSlot > evt->endSlot))
                        || ((iterEvt->nextSchedulerEvent == NULL)
                                && (iterEvt->endSlot < evt->startSlot)) ) {
                    evt->nextSchedulerEvent = iterEvt->nextSchedulerEvent;
                    iterEvt->nextSchedulerEvent = evt;
                    break;
                }
                iterEvt = iterEvt->nextSchedulerEvent;
            }
        }

        if ( receptionWindows ) {
            LoRaSchedulerEvent_t *rxEvt;
            for ( j = 0; j < 2; j++ ) {
                rxEvt = AllocateEvent();
                if ( rxEvt == NULL ) {
                    LOG_ERROR("Coulnd't allocate reception window %u", j);
                    return ERR_NOTAVAIL;
                }

                rxEvt->eventHandler = &eventHandlerList[j];
                rxEvt->startSlot = allocatedSlots[(i + 1) + j];
                rxEvt->endSlot = rxEvt->startSlot + (RECEPTION_RESERVED_TIME / TIME_PER_SLOT);

                /* Place in scheduler event list */
                LoRaSchedulerEvent_t *iterEvt = pEventScheduler;
                while ( iterEvt != NULL ) {
                    if ( ((iterEvt->endSlot < rxEvt->startSlot)
                            && (iterEvt->nextSchedulerEvent != NULL)
                            && (iterEvt->nextSchedulerEvent->startSlot > rxEvt->endSlot))
                            || ((iterEvt->nextSchedulerEvent == NULL)
                                    && (iterEvt->endSlot < rxEvt->startSlot)) ) {
                        rxEvt->nextSchedulerEvent = iterEvt->nextSchedulerEvent;
                        iterEvt->nextSchedulerEvent = rxEvt;
                        break;
                    }
                    iterEvt = iterEvt->nextSchedulerEvent;
                }
            }
            i += 2;
        }

        if ( evtHandler->firstScheduledEvent == NULL ) evtHandler->firstScheduledEvent = evt;

        if ( prevEvt == NULL ) {
            prevEvt = evt;
        } else {
            prevEvt->nextRecurringEvent = evt;
            prevEvt = evt;
        }
    }

    return ERR_OK;
}

/*!
 * Remove event
 *
 * \param[IN] evtHandler Pointer to scheduler event handler to remove
 *
 * \retval status ERR_OK if removed successfully
 */
static uint8_t RemoveEvent( LoRaSchedulerEventHandler_t *evtHandler )
{
    LoRaSchedulerEvent_t *nextEvt, *iterEvt;

    /* Find and remove all related scheduler events */
    iterEvt = evtHandler->firstScheduledEvent;
    while ( iterEvt != NULL ) {
        nextEvt = iterEvt->nextRecurringEvent;
        /* In case it's an uplink event we also have to remove related rx windows */
        if ( evtHandler->eventType == EVENT_TYPE_UPLINK ) {
            LoRaSchedulerEvent_t *iterRxEvt = pEventScheduler;
            /* Remove rx1 window */
            while ( iterRxEvt != NULL ) {
                if ( iterRxEvt->startSlot
                        == (iterEvt->startSlot + (pLoRaDevice->rxWindow1Delay / TIME_PER_SLOT)) ) {
                    /* Corresponding rx1 window found */
                    FreeEvent(iterRxEvt);
                    break;
                }
                iterRxEvt = iterRxEvt->nextSchedulerEvent;
            }
            /* Remove rx2 window */
            iterRxEvt = pEventScheduler;
            while ( iterRxEvt != NULL ) {
                if ( iterRxEvt->startSlot
                        == (iterEvt->startSlot + (pLoRaDevice->rxWindow2Delay / TIME_PER_SLOT)) ) {
                    /* Corresponding rx2 window found */
                    FreeEvent(iterRxEvt);
                    break;
                }
                iterRxEvt = iterRxEvt->nextSchedulerEvent;
            }
        }
        FreeEvent(iterEvt);
        iterEvt = nextEvt;
    }

    /* Check if scheduler list is empty */
    if ( pEventScheduler == NULL ) {
        TimerStop(&EventSchedulerTimer);
        pNextSchedulerEvent = NULL;
    }
    /* Free event handler */
    FreeEventHandler(evtHandler);
    return ERR_OK;
}

/*!
 * Find a specified number of free slots within the advertising window
 *
 * \param[IN] interval Time between slots
 * \param[IN] durationInSlots Duration of a single allocation
 * \param[IN] nofSlots  Number of slots to allocate
 * \param[OUT] allocatedSlots Allocated slot numbers
 * \param[IN] scheduleRxWindows Schedule rx1 and rx2 windows (only for uplinks)
 *
 * \retval status ERR_OK if slot allocation was successful
 */
static uint8_t FindFreeSlots( TimerTime_t interval, uint16_t durationInSlots,
        uint8_t *nofAllocatedSlots, uint16_t *allocatedSlots, bool scheduleRxWindows )
{
    uint8_t nofAllocationTries = 10, maxNofSlots;
    int16_t slots[32];
    LoRaSchedulerEvent_t *evt;
    uint16_t i = 0;

    *(nofAllocatedSlots) = 0;

    /* Calculate max number of slot allocations */
    maxNofSlots = (AVAILABLE_SLOT_TIME / interval);
    if ( (AVAILABLE_SLOT_TIME % interval) > 0 ) maxNofSlots += 1;

    if ( pEventScheduler == NULL ) {
        uint16_t slot = 0;
        for ( i = 0; i < maxNofSlots; i++ ) {
            slot = ((interval / TIME_PER_SLOT) * i);
            if ( scheduleRxWindows ) {
                uint16_t slotRx1, slotRx2;
                slotRx1 = slot + (pLoRaDevice->rxWindow1Delay / TIME_PER_SLOT);
                slotRx2 = slot + (pLoRaDevice->rxWindow2Delay / TIME_PER_SLOT);
                /* Make sure Rx windows fit in as well */
                if ( (slotRx2 + durationInSlots) < NOF_AVAILABLE_SLOTS ) {
                    *(allocatedSlots++) = slot;
                    *(allocatedSlots++) = slotRx1;
                    *(allocatedSlots++) = slotRx2;
                    *(nofAllocatedSlots) += 3;
                    continue;
                }
                break;
            } else {
                if ( (slot + durationInSlots) < NOF_AVAILABLE_SLOTS ) {
                    *(allocatedSlots++) = slot;
                    *(nofAllocatedSlots) += 1;
                }
            }
        }
    } else {
        bool allocationDone = false;
        uint16_t nextSlot = 0, offSet = 0;
        uint32_t timeFrame = 0;

        if ( scheduleRxWindows ) maxNofSlots *= 3;

        while ( !allocationDone && nofAllocationTries > 0 ) {
            /* Find first free slot */
            evt = pEventScheduler;
            while ( evt != NULL ) {
                if ( evt->nextSchedulerEvent != NULL ) {
                    timeFrame = (evt->nextSchedulerEvent->startSlot - evt->endSlot);
                } else {
                    timeFrame = NOF_AVAILABLE_SLOTS - evt->endSlot - 1;
                }

                if ( timeFrame > durationInSlots ) {
                    /* Potential first slot found */
                    slots[0] = (evt->endSlot + 1 + offSet);
                    *(nofAllocatedSlots) += 1;
                    break;
                }
                evt = evt->nextSchedulerEvent;
            }

            /* Allocate recurring events */
            for ( i = 1; i < maxNofSlots; i++ ) {
                if ( scheduleRxWindows ) {
                    if ( (i % 3) == 1 ) {
                        nextSlot = slots[0] + ((interval / TIME_PER_SLOT) * (i / 3))
                                + (pLoRaDevice->rxWindow1Delay / TIME_PER_SLOT);
                    } else if ( (i % 3) == 2 ) {
                        nextSlot = slots[0] + ((interval / TIME_PER_SLOT) * (i / 3))
                                + (pLoRaDevice->rxWindow2Delay / TIME_PER_SLOT);
                    } else {
                        nextSlot = slots[0] + ((interval / TIME_PER_SLOT) * (i / 3));
                        /* Check if rx2 window fits within available slots */
                        if ( (nextSlot + (pLoRaDevice->rxWindow2Delay / TIME_PER_SLOT))
                                > NOF_AVAILABLE_SLOTS ) {
                            allocationDone = true;
                            break;
                        }
                    }
                } else {
                    nextSlot = slots[0] + ((interval / TIME_PER_SLOT) * i);
                }
                slots[i] = -1;
                while ( evt != NULL ) {
                    if ( evt->endSlot < nextSlot ) {
                        if ( ((evt->nextSchedulerEvent != NULL)
                                && (evt->nextSchedulerEvent->startSlot > nextSlot))
                                || ((evt->nextSchedulerEvent == NULL)
                                        && ((NOF_AVAILABLE_SLOTS - durationInSlots) > nextSlot)) ) {
                            slots[i] = nextSlot;
                            *(nofAllocatedSlots) += 1;
                            break;
                        }
                    } else {
                        offSet +=
                                ((evt->startSlot + (evt->endSlot - evt->startSlot)) - nextSlot + 1);
                        break;
                    }
                    evt = evt->nextSchedulerEvent;
                }
                /* Allocation not possible */
                if ( slots[i] == -1 ) {
                    *(nofAllocatedSlots) = 0; /* Reset */
                    nofAllocationTries--;
                    break;
                }
            }
            if ( i == (maxNofSlots) ) allocationDone = true;
        }

        if ( nofAllocationTries == 0 ) {
            return ERR_FAILED;
        } else {
            for ( i = 0; i < (*nofAllocatedSlots); i++ ) {
                *(allocatedSlots++) = slots[i];
            }
        }
    }
    return ERR_OK;
}

static void OnMulticastSchedulerEvent( void *param )
{
    uint32_t currTime;

    currTime = (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS);

    LOG_TRACE("Multicast event at %u ms.", currTime);
}

static void OnReceptionSchedulerEvent( void *param )
{
    uint32_t currTime;

    currTime = (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS);

    LOG_TRACE("Reception window event at %u ms.", currTime);
}

static void OnUplinkSchedulerEvent( void *param )
{
    uint32_t currTime;

    currTime = (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS);

    LOG_TRACE("Up link event at %u ms.", currTime);
}

static void OnRx1SchedulerEvent( void *param )
{
    uint32_t currTime;

    currTime = (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS);

    LOG_TRACE("Reception window 1 event at %u ms.", currTime);
}

static void OnRx2SchedulerEvent( void *param )
{
    uint32_t currTime;

    currTime = (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS);

    LOG_TRACE("Reception window 2 event at %u ms.", currTime);
}

/*!
 * Function executed when advertising FreeRTOS software timer expires.
 *
 * \param xTimer FreeRTOS timer handle
 */
static void OnAdvertisingTimerEvent( TimerHandle_t xTimer )
{
    LOG_TRACE("Advertising timer event at %u (ms).",
            (uint32_t)(TimerGetCurrentTime() * portTICK_PERIOD_MS));

    /* Add packet to message queue */
    LoRaMesh_SendAdvertising();
}

/*!
 * Function executed when event schedule FreeRTOS software timer expires.
 *
 * \param xTimer FreeRTOS timer handle
 */
static void OnEventSchedulerTimerEvent( TimerHandle_t xTimer )
{
    LoRaSchedulerEvent_t* nextEvt;
    uint32_t nextEvtTime;
    uint16_t slot;

    TimerStop(&EventSchedulerTimer);

    if ( pNextSchedulerEvent == NULL ) return;

    slot = (((TimerGetCurrentTime() * portTICK_PERIOD_MS) * 1e3) - ADVERTISING_RESERVED_TIME)
            / TIME_PER_SLOT;
    slot = slot % (ADVERTISING_INTERVAL / TIME_PER_SLOT);

    if ( pNextSchedulerEvent->nextSchedulerEvent == NULL ) {
        /* Calculate next event time */
        nextEvtTime = (((NOF_AVAILABLE_SLOTS - pNextSchedulerEvent->startSlot)
                + pEventScheduler->startSlot) * TIME_PER_SLOT) + ADVERTISING_GUARD_TIME
                + ADVERTISING_RESERVED_TIME;
        /* Restart scheduler */
        nextEvt = pEventScheduler;
    } else if ( pNextSchedulerEvent->startSlot == slot ) {
        /* Start scheduler timer */
        nextEvtTime = ((pNextSchedulerEvent->nextSchedulerEvent->startSlot
                - pNextSchedulerEvent->startSlot) * TIME_PER_SLOT);
        /* Move pointer forward */
        nextEvt = pNextSchedulerEvent->nextSchedulerEvent;
    } else {
        LOG_ERROR("Drift occurred.");
        return;
    }

    /* Start scheduler timer */
    TimerSetValue(&EventSchedulerTimer, nextEvtTime);
    TimerStart(&EventSchedulerTimer);
    /* Invoke callback function */
    if ( pNextSchedulerEvent->eventHandler != NULL ) {
        pNextSchedulerEvent->eventHandler->callback(pNextSchedulerEvent->eventHandler->param);
    }
    /* Move pointer forward */
    pNextSchedulerEvent = nextEvt;
}

/*!
 * Evaluates probability of a node to accept a join mesh or rebind
 * mesh request depending on distance to the node and the current
 * role
 *
 * \param[IN] latiBin Latitude in binary form
 * \param[IN] longiBin Longitude in binary form
 *
 * \retval bool True if request should be accpted
 */
bool EvaluateAcceptanceProbability( int32_t latiBin, int32_t longiBin )
{
    uint32_t distance;

    GpsGetDistanceToLatestGpsPositionBinary(latiBin, longiBin, &distance);
    return true;
}

/*!
 * Evaluates probability of a node to nominate itself as coordinator and
 * returns true, if the calculated probability is bigger then a hard coded threshold.
 *
 * \param[IN] nodeRank Remote nodes rank
 * \param[IN] nodeClass Remote nodes role
 *
 * \retval bool True if the node nominates itself
 */
bool EvaluateNominationProbability( uint8_t nodeRank, DeviceClass_t nodeClass )
{
    (void) nodeRank;
    return true;
}

/*!
 * Calculate the nodes rank.
 *
 * \return uint8_t Rank of the node
 */
uint8_t CalculateNodeRank( void )
{
    return 1;
}

/*!
 * Allocate LoRa scheduler event
 *
 * \retval Pointer to allocated scheduler event
 */
static LoRaSchedulerEvent_t *AllocateEvent( void )
{
    LoRaSchedulerEvent_t *evt = pNextFreeSchedulerEvent;

    if ( evt != NULL ) {
        pNextFreeSchedulerEvent = evt->nextSchedulerEvent;
        evt->startSlot = 0;
        evt->startSlot = 0;
        evt->nextRecurringEvent = NULL;
        evt->nextSchedulerEvent = NULL;
        evt->eventHandler = NULL;
    }

    return evt;
}

/*!
 * Free allocated LoRa scheduler event
 *
 * \param[IN] evt LoRa scheduler event to be freed
 */
static void FreeEvent( LoRaSchedulerEvent_t *evt )
{
    if ( pEventScheduler != NULL ) {
        LoRaSchedulerEvent_t *prevEvt, *iterEvt;
        prevEvt = NULL;
        iterEvt = pEventScheduler;

        while ( iterEvt != NULL ) {
            if ( iterEvt == evt ) {
                if ( prevEvt == NULL ) {
                    pEventScheduler = iterEvt->nextSchedulerEvent;
                } else {
                    prevEvt->nextSchedulerEvent = iterEvt->nextSchedulerEvent;
                }
                iterEvt->nextSchedulerEvent = pNextFreeSchedulerEvent;
                iterEvt->startSlot = 0;
                iterEvt->endSlot = 0;
                iterEvt->eventHandler = NULL;
                iterEvt->nextRecurringEvent = NULL;
                pNextFreeSchedulerEvent = iterEvt;
                break;
            }
            prevEvt = iterEvt;
            iterEvt = iterEvt->nextSchedulerEvent;
        }
    }
}

/*!
 * Allocate LoRa scheduler event handler
 *
 * \retval Pointer to allocated scheduler event handler
 */
static LoRaSchedulerEventHandler_t *AllocateEventHandler( void )
{
    LoRaSchedulerEventHandler_t *handler = pNextFreeEventHandler;

    if ( handler != NULL ) {
        pNextFreeEventHandler = handler->nextEventHandler;
        handler->eventInterval = 0;
        handler->eventType = EVENT_TYPE_UPLINK;
        handler->callback = NULL;
        handler->param = (void*) NULL;
        handler->nextEventHandler = pEventHandlers;
        handler->firstScheduledEvent = NULL;
        pEventHandlers = handler;
        return pEventHandlers;
    }

    return NULL;
}

/*!
 * Free allocated LoRa scheduler event handler
 *
 * \param[IN] evt LoRa scheduler event handler to be freed
 */
static void FreeEventHandler( LoRaSchedulerEventHandler_t *handler )
{
    if ( pEventHandlers != NULL ) {
        LoRaSchedulerEventHandler_t* iterHandler, *prevHandler;

        prevHandler = NULL;
        iterHandler = pEventHandlers;

        while ( iterHandler != NULL ) {
            if ( iterHandler == handler ) {
                if ( prevHandler == NULL ) {
                    pEventHandlers = iterHandler->nextEventHandler;
                } else {
                    prevHandler->nextEventHandler = iterHandler->nextEventHandler;
                }
                iterHandler->nextEventHandler = pNextFreeEventHandler;
                iterHandler->callback = NULL;
                iterHandler->param = (void*) NULL;
                iterHandler->eventType = 0;
                iterHandler->firstScheduledEvent = NULL;
                pNextFreeEventHandler = iterHandler;
                break;
            }
            prevHandler = iterHandler;
            iterHandler = iterHandler->nextEventHandler;
        }
    }
}

/*!
 * \brief Create new child node with given data
 *
 * \param devAddr Device address of the child node
 * \param nwkSKey Network session key
 * \param appSKey Application session key
 * \param channel Channel frequency
 * \param interval Rx window interval
 *
 * \retval
 */
ChildNodeInfo_t* CreateChildNode( uint32_t devAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        uint32_t channel, uint32_t interval )
{
    ChildNodeInfo_t* newNode = pNextFreeChildNode;

    if ( newNode != NULL ) {
        pNextFreeChildNode = newNode->nextSlot;
        newNode->Connection.Address = devAddr;
        memcpy(newNode->Connection.AppSKey, appSKey, 16);
        memcpy(newNode->Connection.NwkSKey, nwkSKey, 16);
        newNode->Connection.UpLinkCounter = 0;
        newNode->Connection.ChannelIndex = channel;
        newNode->Connection.DataRateIndex = LORAMAC_DEFAULT_DATARATE;
        newNode->Connection.TxPowerIndex = LORAMAC_DEFAULT_TX_POWER;
        newNode->Periodicity = interval;
        newNode->Duration = 0u;

        return newNode;
    }
    return NULL;
}

/*!
 * \brief Add child node at the tail of the list.
 *
 * \param childNode Child Node to append
 */
void ChildNodeAdd( ChildNodeInfo_t* childNode )
{
    // Reset uplink counter
    childNode->Connection.UpLinkCounter = 0;

    if ( pLoRaDevice->childNodes == NULL ) {
        childNode->nextSlot = NULL;
        pLoRaDevice->childNodes = childNode;
    } else {
        childNode->nextSlot = pLoRaDevice->childNodes;
        pLoRaDevice->childNodes = childNode;
    }
}

/*!
 * \brief Remove child node from the list.
 *
 * \param childNode Pointer to child node that should be removed.
 */
void ChildNodeRemove( ChildNodeInfo_t* childNode )
{
    if ( pLoRaDevice->childNodes != NULL ) {
        ChildNodeInfo_t* iterNode = pLoRaDevice->childNodes;

        while ( iterNode->nextSlot != NULL ) {
            if ( iterNode->nextSlot == childNode ) {
                iterNode->nextSlot = childNode->nextSlot;
                childNode->nextSlot = pNextFreeChildNode;
                pNextFreeChildNode = childNode;
            }
            iterNode = iterNode->nextSlot;
        }
    }
}

/*!
 * \brief Find child node from specified address.
 *
 * \param[IN] childAddr Child node address
 * \retval Pointer to the child node if found. NULL otherwise.
 */
ChildNodeInfo_t *ChildNodeFind( uint32_t childAddr )
{
    ChildNodeInfo_t* childNode = pLoRaDevice->childNodes;

    while ( childNode != NULL ) {
        if ( childNode->Connection.Address == childAddr ) return childNode;
        childNode = childNode->nextSlot;
    }
    return NULL;
}

/*!
 * \brief Create new child node with given data
 *
 * \param devAddr Device address of the child node
 * \param nwkSKey Network session key
 * \param appSKey Application session key
 * \param channel Channel frequency
 * \param interval Rx window interval
 */
MulticastGroupInfo_t* CreateMulticastGroup( uint32_t grpAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        uint32_t channel, uint32_t interval )
{
    MulticastGroupInfo_t* newGrp = pNextFreeMulticastGrp;

    if ( newGrp != NULL ) {
        pNextFreeMulticastGrp = newGrp->nextSlot;
        newGrp->Connection.Address = grpAddr;
        memcpy(newGrp->Connection.AppSKey, appSKey, 16);
        memcpy(newGrp->Connection.NwkSKey, nwkSKey, 16);
        newGrp->Connection.DownLinkCounter = 0;
        newGrp->Connection.ChannelIndex = channel;
        newGrp->Connection.DataRateIndex = LORAMAC_DEFAULT_DATARATE;
        newGrp->Connection.TxPowerIndex = LORAMAC_DEFAULT_TX_POWER;
        newGrp->Periodicity = interval;
        newGrp->Duration = 0u;

        return newGrp;
    }
    return NULL;
}

/*!
 * \brief Add mutlicast group.
 *
 * \param multicastGrp Pointer to information structure of the multicast group to be added.
 */
void MulticastGroupAdd( MulticastGroupInfo_t *multicastGrp )
{
    // Reset downlink counter
    multicastGrp->Connection.DownLinkCounter = 0;

    if ( pLoRaDevice->multicastGroups == NULL ) {
        multicastGrp->nextSlot = NULL;
        pLoRaDevice->multicastGroups = multicastGrp;
    } else {
        multicastGrp->nextSlot = pLoRaDevice->multicastGroups;
        pLoRaDevice->multicastGroups = multicastGrp;
    }
}

/*!
 * \brief Remove multicast group.
 *
 * \param multicastGrp Pointer to information structure of the multicast group to be removed.
 */
void MulticastGroupRemove( MulticastGroupInfo_t *multicastGrp )
{
    if ( pLoRaDevice->multicastGroups != NULL ) {
        ChildNodeInfo_t* iterGrp = pLoRaDevice->childNodes;

        while ( iterGrp->nextSlot != NULL ) {
            if ( iterGrp->nextSlot == multicastGrp ) {
                iterGrp->nextSlot = multicastGrp->nextSlot;
                multicastGrp->nextSlot = pNextFreeMulticastGrp;
                pNextFreeMulticastGrp = multicastGrp;
            }
            iterGrp = iterGrp->nextSlot;
        }
    }
}

/*!
 * \brief Find multicast group through group address.
 *
 * \param[IN] grpAddr Multicast group address
 * \retval Pointer to multicast group if found
 */
MulticastGroupInfo_t *MulticastGroupFind( uint32_t grpAddr )
{
    MulticastGroupInfo_t* grp = pLoRaDevice->childNodes;

    while ( grp != NULL ) {
        if ( grp->Connection.Address == grpAddr ) return grp;
        grp = grp->nextSlot;
    }
    return NULL;
}

/*******************************************************************************
 * SHELL FUNCTIONS (STATIC)
 ******************************************************************************/
/*!
 * \brief Print out LoRaMesh status.
 *
 * \param io Std io to be used for print out.
 */
static uint8_t PrintStatus( Shell_ConstStdIO_t *io )
{
    byte buf[16];

    Shell_SendStatusStr((unsigned char*) "LoRaMesh", (unsigned char*) "\r\n", io->stdOut);

    return ERR_OK;
}

/*!
 * \brief Print out LoRaMesh command help.
 *
 * \param io Std io to be used for print out.
 */
static uint8_t PrintHelp( Shell_ConstStdIO_t *io )
{
    Shell_SendHelpStr((unsigned char*) "LoRaMesh",
            (unsigned char*) "Group of LoRaMesh commands\r\n", io->stdOut);
    Shell_SendHelpStr((unsigned char*) "  childnodes",
            (unsigned char*) "Print child nodes list\r\n", io->stdOut);
    Shell_SendHelpStr((unsigned char*) "  multicastgroups",
            (unsigned char*) "Print multicast groups list\r\n", io->stdOut);
    Shell_SendHelpStr((unsigned char*) "  help|status",
            (unsigned char*) "Print help or status information\r\n", io->stdOut);

    return ERR_OK;
}

/*!
 * \brief Print out scheduler list.
 *
 * \param io Std io to be used for print out.
 */
static uint8_t PrintEventSchedulerList( Shell_ConstStdIO_t *io )
{
    LoRaSchedulerEvent_t* evt = pEventScheduler;
    byte buf[32];
    uint32_t i = 0;

    while ( evt != NULL ) {
        switch ( evt->eventHandler->eventType ) {
            case EVENT_TYPE_UPLINK:
                memcpy(buf, (byte*) "Uplink event", sizeof("Uplink event"));
                break;
            case EVENT_TYPE_MULTICAST:
                memcpy(buf, (byte*) "Multicast event", sizeof("Multicast event"));
                break;
            case EVENT_TYPE_SYNCH_RX_WINDOW:
                memcpy(buf, (byte*) "Synchronized rx window event",
                        sizeof("Synchronized rx window event"));
                break;
            case EVENT_TYPE_RX1_WINDOW:
                memcpy(buf, (byte*) "Rx1 window event", sizeof("Rx1 window event"));
                break;
            case EVENT_TYPE_RX2_WINDOW:
                memcpy(buf, (byte*) "Rx2 window event", sizeof("Rx2 window event"));
                break;
        }
        LOG_DEBUG("%u. %s at slot %u.", (i + 1), (unsigned char*) buf, evt->startSlot);
        i++;
        evt = evt->nextSchedulerEvent;
    }

    return ERR_OK;
}

/*!
 * \brief Print out child node list.
 *
 * \param io Std io to be used for print out.
 */
static uint8_t PrintChildNodes( Shell_ConstStdIO_t *io )
{
    uint8_t j;
    ChildNodeInfo_t* childNode = pLoRaDevice->childNodes;

    while ( childNode != NULL ) {
        Shell_SendStr((unsigned char*) SHELL_DASH_LINE, io->stdOut);
        LOG_DEBUG_BARE("%-15s: 0x%08x\r\n", "Address", childNode->Connection.Address);
        LOG_DEBUG_BARE("%-15s: ", "NwkSKey");
        for ( j = 0; j < 16; j++ )
            LOG_DEBUG_BARE("0x%02x ", childNode->Connection.NwkSKey[j]);
        LOG_DEBUG_BARE("\r\n%-15s: ", "AppSKey");
        for ( j = 0; j < 16; j++ )
            LOG_DEBUG_BARE("0x%02x ", childNode->Connection.AppSKey[j]);
        LOG_DEBUG_BARE("%-15s: 0x%08x\r\n", "UpLinkCounter", childNode->Connection.UpLinkCounter);
        Shell_SendStr((unsigned char*) "\r\n---- Uplink Slot Info ----\r\n", io->stdOut);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Frequency", childNode->Connection.ChannelIndex);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Periodicity", childNode->Periodicity);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Duration", childNode->Duration);

        childNode = childNode->nextSlot;
    }
    return ERR_OK;
}

/*!
 * \brief Print out multicast group list.
 *
 * \param io Std io to be used for print out.
 */
static uint8_t PrintMulticastGroups( Shell_ConstStdIO_t *io )
{
    uint8_t j;
    MulticastGroupInfo_t* multicastGrp = pLoRaDevice->multicastGroups;

    while ( multicastGrp != NULL ) {
        Shell_SendStr((unsigned char*) SHELL_DASH_LINE, io->stdOut);
        LOG_DEBUG_BARE("%-15s: 0x%08x\r\n", "Address", multicastGrp->Connection.Address);
        LOG_DEBUG_BARE("%-15s: ", "NwkSKey");
        for ( j = 0; j < 16; j++ )
            LOG_DEBUG_BARE("0x%02x ", multicastGrp->Connection.NwkSKey[j]);
        LOG_DEBUG_BARE("\r\n%-15s: ", "AppSKey");
        for ( j = 0; j < 16; j++ )
            LOG_DEBUG_BARE("0x%02x ", multicastGrp->Connection.AppSKey[j]);
        LOG_DEBUG_BARE("%-15s: 0x%08x\r\n", "DownLinkCounter",
                multicastGrp->Connection.DownLinkCounter);
        LOG_DEBUG_BARE("\r\n---- %-15s ----\r\n", "Downlink Slot Info");
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Frequency", multicastGrp->Connection.ChannelIndex);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Periodicity", multicastGrp->Periodicity);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Duration", multicastGrp->Duration);

        multicastGrp = multicastGrp->nextSlot;
    }
    return ERR_OK;
}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
