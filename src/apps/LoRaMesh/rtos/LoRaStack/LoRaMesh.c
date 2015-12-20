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

/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/
typedef struct {
    TimerEvent_t *timer;
    TimerTime_t reservedTime;
    TimerTime_t nextEventTime;
    LoRaSchedulerEventCallback_t callback;
    void *param;
} LoRaMeshEvent_t;

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
/*! LoRaMesh upper layer event functions */
static LoRaMeshCallbacks_t *LoRaMeshCallbacks;

/*! Application Session Key */
static uint8_t LoRaAppSKey[16] = { 0 };

/*! Network Session Key */
static uint8_t LoRaNwkSKey[16] = { 0 };

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
        .AppSKey = (uint8_t*)&LoRaAppSKey,
        .NwkSKey = (uint8_t*)&LoRaNwkSKey,
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
static ListPointer_t pEventScheduler;

/*! Rx message handlers */
static ListPointer_t pRxMsgHandlers;

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*! \brief Evaluates probability of a node to nominate itself as coordinator. */
static bool EvaluateNominationProbability( uint8_t nodeRank );

/*! \brief Calculate the nodes rank. */
static uint8_t CalculateNodeRank( void );

/*!  */
static void ScheduleEvent( LoRaSchedulerEventType_t type,
        LoRaSchedulerEventCallback_t callback, void* param );

/*!  */
static void RemoveEvent( void );

/*! \brief Create new child node with given data. */
ChildNodeInfo_t* CreateChildNode( uint32_t devAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        uint32_t channel, uint32_t interval );

/*! \brief Add child node at the tail of the list. */
void ChildNodeAdd( ChildNodeInfo_t* childNode );

/*! \brief Remove child node from the list. */
void ChildNodeRemove( ChildNodeInfo_t* childNode );

/*! \brief Print out child node information. */
void ChildNodePrint( ChildNodeInfo_t* childNode );

/*! \brief Create new child node with given data. */
MulticastGroupInfo_t* CreateMulticastGroup( uint32_t grpAddr, uint8_t* nwkSKey,
        uint8_t* appSKey, uint32_t channel, uint32_t interval );

/*! \brief Add mutlicast group. */
void MulticastGroupAdd( MulticastGroupInfo_t *multicastGrp );

/*! \brief Remove multicast group. */
void MulticastGroupRemove( MulticastGroupInfo_t *multicastGrp );

/*! \brief Print out multicast group information. */
void MulticastGroupPrint( MulticastGroupInfo_t* multicastGrp );

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
void LoRaMesh_Init( LoRaMeshCallbacks_t *callbacks,
        LoRaMac_BatteryLevelCallback_t batteryLevelCb )
{
    LoRaMeshCallbacks = callbacks;

    /* Assign LoRa device structure pointer */
    pLoRaDevice = (LoRaDevice_t*) &LoRaDevice;

    /* Create a list with rx message handler */
    pRxMsgHandlers = ListCreate();

    /* Create scheduler list */
    pEventScheduler = ListCreate();

    /* Initialize stack */
    LoRaFrm_Init();
    LoRaMac_Init(batteryLevelCb);
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

uint8_t LoRaMesh_RegisterApplicationPort( RxMsgHandler fHandler, uint8_t fPort )
{
    PortHandler_t *tempPortHandler;

    if ( fPort < 1 || fPort > 223 ) return ERR_RANGE;

    tempPortHandler = (PortHandler_t*) custom_malloc(sizeof(PortHandler_t));
    tempPortHandler->Handler = fHandler;
    tempPortHandler->Port = fPort;

    if ( pRxMsgHandlers->head == NULL ) {
        /* Add rx message handler */
        ListPushBack(pRxMsgHandlers, tempPortHandler);
    } else {
        ListNodePointer_t currNode = pRxMsgHandlers->head;

        /* Make sure port is not already registered */
        while (currNode->next != NULL) {
            if ( ((PortHandler_t*) currNode->data)->Port == fPort ) {
                return ERR_NOTAVAIL;
            }
        }
        /* Add rx message handler */
        ListPushBack(pRxMsgHandlers, tempPortHandler);
    }

    return ERR_OK;
}

uint8_t LoRaMesh_RemoveApplicationPort( RxMsgHandler fHandler, uint8_t fPort )
{
    PortHandler_t *tempPortHandler;
    ListNodePointer_t currNode;

    currNode = pRxMsgHandlers->head;

    while (currNode != NULL) {
        tempPortHandler = (PortHandler_t*) currNode->data;
        if ( tempPortHandler->Port == fPort ) {
            ListRemove(pRxMsgHandlers, currNode->data);
            return ERR_OK;
        }
        currNode = currNode->next;
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
                ListNodePointer_t currNode = pRxMsgHandlers->head;
                while (currNode != NULL) {
                    if ( ((PortHandler_t*) currNode->data)->Port == fPort ) {
                        return ((PortHandler_t*) currNode->data)->Handler(buf,
                                payloadSize, fPort);
                    }
                    currNode = currNode->next;
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
    ChildNodeInfo_t* tempNode;
    ListNodePointer_t listNode;

    if ( pLoRaDevice->childNodes != NULL ) {
        listNode = pLoRaDevice->childNodes->head;

        while (listNode != NULL) {
            tempNode = (ChildNodeInfo_t*) listNode->data;
            if ( tempNode->Connection.Address == devAddr ) return tempNode;
            listNode = listNode->next;
        }
    }

    return NULL;
}

MulticastGroupInfo_t* LoRaMesh_FindMulticastGroup( uint32_t grpAddr )
{
    MulticastGroupInfo_t* tempGrp;
    ListNodePointer_t listNode;

    if ( pLoRaDevice->multicastGroups != NULL ) {
        listNode = pLoRaDevice->multicastGroups->head;

        while (listNode != NULL) {
            tempGrp = (MulticastGroupInfo_t*) listNode->data;
            if ( tempGrp->Connection.Address == grpAddr ) return tempGrp;
            listNode = listNode->next;
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
 * PRINT OUT FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaMesh_PrintChildNodes( bool reverseOrder )
{
    ListNodePointer_t tempNode;
    uint8_t i;
    if ( reverseOrder ) tempNode = (ListNodePointer_t) pLoRaDevice->childNodes->tail;
    else tempNode = (ListNodePointer_t) pLoRaDevice->childNodes->head;

    for ( i = 0; i < pLoRaDevice->childNodes->count; i++ ) {
        LOG_DEBUG_BARE("%u. ---------------------------------------------\r\n", (i + 1));
        ChildNodePrint((ChildNodeInfo_t*) tempNode->data);
        if ( reverseOrder ) tempNode = (ListNodePointer_t) tempNode->prev;
        else tempNode = (ListNodePointer_t) tempNode->next;
    }
}

void LoRaMesh_PrintMulticastGroups( bool reverseOrder )
{
    ListNodePointer_t tempGrp;
    uint8_t i;
    if ( reverseOrder ) tempGrp = (ListNodePointer_t) pLoRaDevice->multicastGroups->tail;
    else tempGrp = (ListNodePointer_t) pLoRaDevice->multicastGroups->head;

    for ( i = 0; i < pLoRaDevice->multicastGroups->count; i++ ) {
        LOG_DEBUG_BARE("%u. ---------------------------------------------\r\n", (i + 1));
        MulticastGroupPrint((MulticastGroupInfo_t*) tempGrp->data);
        if ( reverseOrder ) tempGrp = (ListNodePointer_t) tempGrp->prev;
        else tempGrp = (ListNodePointer_t) tempGrp->next;
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
static void ScheduleEvent( LoRaSchedulerEventType_t type,
        LoRaSchedulerEventCallback_t callback, void* param )
{

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
    ChildNodeInfo_t* newNode = (ChildNodeInfo_t*) custom_malloc(sizeof(ChildNodeInfo_t));
    newNode->Connection.Address = devAddr;
    newNode->Connection.NwkSKey = nwkSKey;
    newNode->Connection.AppSKey = appSKey;
    newNode->Connection.UpLinkCounter = 0;
    newNode->Connection.ChannelIndex = channel;
    newNode->Connection.DataRateIndex = LORAMAC_DEFAULT_DATARATE;
    newNode->Connection.TxPowerIndex = LORAMAC_DEFAULT_TX_POWER;
    newNode->Periodicity = interval;
    newNode->Duration = 0u;

    return newNode;
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
        pLoRaDevice->childNodes = ListCreate();
    }
    ListPushBack(pLoRaDevice->childNodes, (void*) childNode);
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
        ListRemove(pLoRaDevice->childNodes, (void*) childNode);
        custom_free(childNode);
    }
}

/*!
 * \brief Print out child node information.
 *
 * \param childNode Pointer to child node information to print out.
 */
void ChildNodePrint( ChildNodeInfo_t* childNode )
{
    uint8_t j;
    LOG_DEBUG_BARE("%-15s: 0x%08x\r\n", "Address", childNode->Connection.Address);
    LOG_DEBUG_BARE("%-15s: ", "NwkSKey");
    for ( j = 0; j < 16; j++ )
        LOG_DEBUG_BARE("0x%02x ", childNode->Connection.NwkSKey[j]);
    LOG_DEBUG_BARE("\r\n%-15s: ", "AppSKey");
    for ( j = 0; j < 16; j++ )
        LOG_DEBUG_BARE("0x%02x ", childNode->Connection.AppSKey[j]);
    LOG_DEBUG_BARE("%-15s: 0x%08x\r\n", "UpLinkCounter",
            childNode->Connection.UpLinkCounter);
    LOG_DEBUG_BARE("\r\n---- %-15s ----\r\n", "Uplink Slot Info");
    LOG_DEBUG_BARE("%-15s: %u\r\n", "Frequency", childNode->Connection.ChannelIndex);
    LOG_DEBUG_BARE("%-15s: %u\r\n", "Periodicity", childNode->Periodicity);
    LOG_DEBUG_BARE("%-15s: %u\r\n", "Duration", childNode->Duration);
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
    MulticastGroupInfo_t* newGrp = (MulticastGroupInfo_t*) custom_malloc(
            sizeof(MulticastGroupInfo_t));
    newGrp->Connection.Address = grpAddr;
    newGrp->Connection.NwkSKey = nwkSKey;
    newGrp->Connection.AppSKey = appSKey;
    newGrp->Connection.DownLinkCounter = 0;
    newGrp->Connection.ChannelIndex = channel;
    newGrp->Connection.DataRateIndex = LORAMAC_DEFAULT_DATARATE;
    newGrp->Connection.TxPowerIndex = LORAMAC_DEFAULT_TX_POWER;
    newGrp->Periodicity = interval;
    newGrp->Duration = 0u;

    return newGrp;
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
        pLoRaDevice->multicastGroups = ListCreate();
    }

    ListPushBack(pLoRaDevice->multicastGroups, (void*) multicastGrp);
}

/*!
 * \brief Remove multicast group.
 *
 * \param multicastGrp Pointer to information structure of the multicast group to be removed.
 */
void MulticastGroupRemove( MulticastGroupInfo_t *multicastGrp )
{
    if ( pLoRaDevice->multicastGroups != NULL ) {
        ListRemove(pLoRaDevice->multicastGroups, (void*) multicastGrp);
        custom_free(multicastGrp);
    }
}

/*!
 * \brief Print out multicast group information.
 *
 * \param multicastGrp Pointer to multicast group information to print out.
 */
void MulticastGroupPrint( MulticastGroupInfo_t* multicastGrp )
{
    uint8_t j;
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
}
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
