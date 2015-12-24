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
#define MAX_NOF_MULTICAST_GROUPS            LORAMESH_CONFIG_MAX_NOF_MULTICAST_GROUPS
#define MAX_NOF_CHILD_NODES                 LORAMESH_CONFIG_MAX_NOF_CHILD_NODES

#define ADVERTISING_RESERVED_TIME           2000
#define UPLINK_RESERVED_TIME                1000
#define MULTICAST_RESERVED_TIME             3000
#define RECEPTION_RESERVED_TIME             3000
/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/
typedef struct LoRaMeshEvent_s {
    uint32_t slot;
    TimerEvent_t *timer;
    TimerTime_t reservedTime;
    TimerTime_t nextOccurenceTime;
    LoRaSchedulerEventCallback_t callback;
    void *param;
    struct LoRaMeshEvent_s *nextEvent;
} LoRaMeshEvent_t;

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

/*! Scheduler */
static LoRaMeshEvent_t eventSchedulerList[MAX_NOF_SCHEDULER_EVENTS];
static LoRaMeshEvent_t *pEventScheduler;
static LoRaMeshEvent_t *pNextFreeSchedulerEvent;

/*! Multicast groups */
static MulticastGroupInfo_t multicastGrpList[MAX_NOF_MULTICAST_GROUPS];
static MulticastGroupInfo_t *pNextFreeMulticastGrp;

/*! Child nodes */
static ChildNodeInfo_t childNodeList[MAX_NOF_CHILD_NODES];
static ChildNodeInfo_t *pNextFreeChildNode;

/*! Rx message handlers */
static PortHandler_t portHandlers[LORAFRM_NOF_USABLE_FPORT];
static PortHandler_t *pPortHandlers;
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*! \brief Evaluates probability of a node to nominate itself as coordinator. */
static bool EvaluateNominationProbability( uint8_t nodeRank );

/*! \brief Calculate the nodes rank. */
static uint8_t CalculateNodeRank( void );

/*!  */
static uint8_t ScheduleEvent( TimerTime_t interval, LoRaSchedulerEventType_t type,
        LoRaSchedulerEventCallback_t callback, void* param );

/*! */
static TimerTime_t FindFreeSlot( TimerTime_t interval, LoRaSchedulerEventType_t type );

/*!  */
static void RemoveEvent( void );

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
MulticastGroupInfo_t* CreateMulticastGroup( uint32_t grpAddr, uint8_t* nwkSKey,
        uint8_t* appSKey, uint32_t channel, uint32_t interval );

/*! \brief Add mutlicast group. */
void MulticastGroupAdd( MulticastGroupInfo_t *multicastGrp );

/*! \brief Remove multicast group. */
void MulticastGroupRemove( MulticastGroupInfo_t *multicastGrp );

/*! \brief Find multicast group from the list. */
MulticastGroupInfo_t *MulticastGroupFind( uint32_t grpAddr );

/*!  */
static uint8_t PrintStatus( Shell_ConstStdIO_t *io );

/*!  */
static uint8_t PrintHelp( Shell_ConstStdIO_t *io );

/*! \brief Print out child node information. */
static uint8_t PrintChildNodes( Shell_ConstStdIO_t *io );

/*! \brief Print out multicast group information. */
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
byte LoRaMesh_ParseCommand( const unsigned char *cmd, bool *handled,
        Shell_ConstStdIO_t *io )
{
    if ( strcmp((char*) cmd, SHELL_CMD_HELP) == 0
            || strcmp((char*) cmd, "LoRaMesh help") == 0 ) {
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
    uint8_t i;

    LoRaMeshCallbacks = callbacks;

    /* Assign LoRa device structure pointer */
    pLoRaDevice = (LoRaDevice_t*) &LoRaDevice;

    /* Create a list with rx message handler */
    pPortHandlers = portHandlers;

    /* Init scheduler list */
    pEventScheduler = NULL;
    pNextFreeSchedulerEvent = eventSchedulerList;
    for ( i = 0; i < MAX_NOF_SCHEDULER_EVENTS - 1; i++ ) {
        eventSchedulerList[i].nextEvent = &eventSchedulerList[i + 1];
    }
    /* Schedule advertising */
    ScheduleEvent(30000, EVENT_TYPE_ADVERTISING, NULL, NULL);

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
}

void LoRaMesh_InitNwkIds( uint32_t netID, uint32_t devAddr, uint8_t *nwkSKey,
        uint8_t *appSKey )
{
    pLoRaDevice->netId = netID;
    pLoRaDevice->devAddr = devAddr;

    for ( uint8_t i = 0; i < 16; i++ ) {
        pLoRaDevice->upLinkSlot.AppSKey[i] = appSKey[i];
        pLoRaDevice->upLinkSlot.NwkSKey[i] = nwkSKey[i];
    }

    pLoRaDevice->ctrlFlags.Bits.nwkJoined = 1;
}

uint8_t LoRaMesh_RegisterApplicationPort( PortHandlerFunction_t fHandler, uint8_t fPort )
{
    if ( fPort < 1 || fPort > 223 ) return ERR_RANGE;

    if ( pPortHandlers[fPort - 1].Handler != NULL ) {
        pPortHandlers[fPort - 1].Handler = fHandler;
        return ERR_OK;
    }
    return ERR_FAILED;
}

uint8_t LoRaMesh_RemoveApplicationPort( PortHandlerFunction_t fHandler, uint8_t fPort )
{
    if ( fPort < 1 || fPort > 223 ) return ERR_RANGE;

    if ( pPortHandlers[fPort - 1].Handler == fHandler ) {
        pPortHandlers[fPort - 1].Handler = NULL;
        return ERR_OK;
    }

    return ERR_FAILED;
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
    while (i < appPayloadSize) {
        buf[LORAMESH_BUF_PAYLOAD_START(buf) + i] = *appPayload;
        appPayload++;
        i++;
    }

    return LoRaMesh_PutPayload(buf, sizeof(buf), appPayloadSize, fPort, FRM_TYPE_REGULAR);

}

uint8_t LoRaMesh_OnPacketRx( uint8_t *buf, uint8_t payloadSize, uint8_t fPort,
        LoRaFrm_Type_t fType )
{
    switch (fType) {
        case FRM_TYPE_REGULAR:
            if ( fPort > 0 ) {
                if ( pPortHandlers[fPort - 1].Handler != NULL ) {
                    pPortHandlers[fPort - 1].Handler(buf, payloadSize, fPort);
                }
            }
            break;
        case FRM_TYPE_MULTICAST:
            break;
        case FRM_TYPE_ADVERTISING:
            break;
        default:
            break;
    }
    return ERR_FAILED;
}

uint8_t LoRaMesh_PutPayload( uint8_t* buf, uint16_t bufSize, uint8_t payloadSize,
        uint8_t fPort, LoRaFrm_Type_t fType )
{
    /* Add app information */
#if(LORA_DEBUG_OUTPUT_PAYLOAD == 1)
    LOG_TRACE("%s - Size %d", __FUNCTION__, payloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < payloadSize; i++ )
    LOG_TRACE_BARE("0x%02x ", buf[i]);
    LOG_TRACE_BARE("\r\n");
#endif
    return LoRaFrm_PutPayload(buf, bufSize, payloadSize, fPort, fType, UP_LINK, false,
            false);
}

uint8_t LoRaMesh_ProcessAdvertising( uint8_t *aPayload, uint8_t aPayloadSize )
{
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
    LORAMAC_BUF_PAYLOAD_START(mPayload)[mPayloadSize++] = (pLoRaDevice->devNonce >> 8)
            & 0xFF;

    return LoRaMac_PutPayload((uint8_t*) &mPayload, sizeof(mPayload), mPayloadSize,
            MSG_TYPE_JOIN_REQ);
}

uint8_t LoRaMesh_JoinReqAdHoc( int32_t binLat, int32_t binLong, bool isRebind )
{
    return ERR_OK;
}

bool LoRaMesh_IsNetworkJoined( void )
{
    if ( pLoRaDevice != NULL ) {
        return (pLoRaDevice->ctrlFlags.Bits.nwkJoined == 1U);
    } else {
        return false;
    }
}

ChildNodeInfo_t* LoRaMesh_FindChildNode( uint32_t devAddr )
{
    if ( pLoRaDevice->childNodes != NULL ) {
        ChildNodeInfo_t *childNode = pLoRaDevice->childNodes;

        while (childNode != NULL) {
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

        while (grp != NULL) {
            if ( grp->Connection.Address == grpAddr ) return grp;
            grp = grp->nextSlot;
        }
    }

    return NULL;
}

/*******************************************************************************
 * PUBLIC SETUP FUNCTIONS
 ******************************************************************************/
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
/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
/*!
 *
 */
static uint8_t ScheduleEvent( TimerTime_t interval, LoRaSchedulerEventType_t type,
        LoRaSchedulerEventCallback_t callback, void* param )
{
    TimerTime_t eventTime = FindFreeSlot(interval, type);

    if ( eventTime > 0 ) {

    }
}

static TimerTime_t FindFreeSlot( TimerTime_t interval, LoRaSchedulerEventType_t type )
{
    TimerTime_t reserved;
    switch (type) {
        case EVENT_TYPE_ADVERTISING:
            reserved = ADVERTISING_RESERVED_TIME;
            break;
        case EVENT_TYPE_UPLINK:
            reserved = UPLINK_RESERVED_TIME;
            break;
        case EVENT_TYPE_MULTICAST:
            reserved = MULTICAST_RESERVED_TIME;
            break;
        case EVENT_TYPE_RECEPTION:
            reserved = RECEPTION_RESERVED_TIME;
            break;
        default:
            break;
    }
}

/*!
 *
 */
static void RemoveEvent( void )
{

}

/*!
 * \brief Evaluates probability of a node to nominate itself as coordinator and
 * returns true, if the calculated probability is bigger then a hardcoded threshold.
 *
 * \param nodeRank Calculated node rank
 * \return bool True if the node nominates itself
 */
bool EvaluateNominationProbability( uint8_t nodeRank )
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
 * \brief Create new child node with given data
 *
 * \param devAddr Device address of the child node
 * \param nwkSKey Network session key
 * \param appSKey Application session key
 * \param channel Channel frequency
 * \param interval Rx window interval
 */
ChildNodeInfo_t* CreateChildNode( uint32_t devAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        uint32_t channel, uint32_t interval )
{
    ChildNodeInfo_t* newNode = pNextFreeChildNode;

    if ( newNode != NULL ) {
        pNextFreeChildNode = newNode->nextSlot;
        newNode->Connection.Address = devAddr;
        memcpy1(newNode->Connection.AppSKey, appSKey, 16);
        memcpy1(newNode->Connection.NwkSKey, nwkSKey, 16);
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
 * \param devAddr Device address of child node to remove.
 * \return LoRaMacChildNodeInfo_t Removed child node.
 */
void ChildNodeRemove( ChildNodeInfo_t* childNode )
{
    if ( pLoRaDevice->childNodes != NULL ) {
        ChildNodeInfo_t* iterNode = pLoRaDevice->childNodes;

        while (iterNode->nextSlot != NULL) {
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
 *
 */
ChildNodeInfo_t *ChildNodeFind( uint32_t childAddr )
{
    ChildNodeInfo_t* childNode = pLoRaDevice->childNodes;

    while (childNode != NULL) {
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
MulticastGroupInfo_t* CreateMulticastGroup( uint32_t grpAddr, uint8_t* nwkSKey,
        uint8_t* appSKey, uint32_t channel, uint32_t interval )
{
    MulticastGroupInfo_t* newGrp = pNextFreeMulticastGrp;

    if ( newGrp != NULL ) {
        pNextFreeMulticastGrp = newGrp->nextSlot;
        newGrp->Connection.Address = grpAddr;
        memcpy1(newGrp->Connection.AppSKey, appSKey, 16);
        memcpy1(newGrp->Connection.NwkSKey, nwkSKey, 16);
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

        while (iterGrp->nextSlot != NULL) {
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
 *
 */
MulticastGroupInfo_t *MulticastGroupFind( uint32_t grpAddr )
{
    MulticastGroupInfo_t* grp = pLoRaDevice->childNodes;

    while (grp != NULL) {
        if ( grp->Connection.Address == grpAddr ) return grp;
        grp = grp->nextSlot;
    }
    return NULL;
}

/*******************************************************************************
 * SHELL FUNCTIONS (STATIC)
 ******************************************************************************/
static uint8_t PrintStatus( Shell_ConstStdIO_t *io )
{
    byte buf[16];

    Shell_SendStatusStr((unsigned char*) "LoRaMesh", (unsigned char*) "\r\n", io->stdOut);

    return ERR_OK;
}

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

static uint8_t PrintChildNodes( Shell_ConstStdIO_t *io )
{
    uint8_t j;
    ChildNodeInfo_t* childNode = pLoRaDevice->childNodes;

    while (childNode != NULL) {
        Shell_SendStr((unsigned char*) SHELL_DASH_LINE, io->stdOut);
        LOG_DEBUG_BARE("%-15s: 0x%08x\r\n", "Address", childNode->Connection.Address);
        LOG_DEBUG_BARE("%-15s: ", "NwkSKey");
        for ( j = 0; j < 16; j++ )
            LOG_DEBUG_BARE("0x%02x ", childNode->Connection.NwkSKey[j]);
        LOG_DEBUG_BARE("\r\n%-15s: ", "AppSKey");
        for ( j = 0; j < 16; j++ )
            LOG_DEBUG_BARE("0x%02x ", childNode->Connection.AppSKey[j]);
        LOG_DEBUG_BARE("%-15s: 0x%08x\r\n", "UpLinkCounter",
                childNode->Connection.UpLinkCounter);
        Shell_SendStr((unsigned char*) "\r\n---- Uplink Slot Info ----\r\n", io->stdOut);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Frequency", childNode->Connection.ChannelIndex);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Periodicity", childNode->Periodicity);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Duration", childNode->Duration);

        childNode = childNode->nextSlot;
    }
    return ERR_OK;
}

/*!
 * \brief Print out multicast group information.
 *
 * \param multicastGrp Pointer to multicast group information to print out.
 */
static uint8_t PrintMulticastGroups( Shell_ConstStdIO_t *io )
{
    uint8_t j;
    MulticastGroupInfo_t* multicastGrp = pLoRaDevice->multicastGroups;

    while (multicastGrp != NULL) {
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
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Frequency",
                multicastGrp->Connection.ChannelIndex);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Periodicity", multicastGrp->Periodicity);
        LOG_DEBUG_BARE("%-15s: %u\r\n", "Duration", multicastGrp->Duration);

        multicastGrp = multicastGrp->nextSlot;
    }
    return ERR_OK;
}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
