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

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
/*! LoRaMesh upper layer event functions */
static LoRaMeshCallbacks_t *LoRaMeshCallbacks;

/*! */
LoRaDevice_t* pLoRaDevice;

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*! \brief Evaluates probability of a node to nominate itself as coordinator. */
static bool EvaluateNominationProbability( uint8_t nodeRank );

/*! \brief Calculate the nodes rank. */
static uint8_t CalculateNodeRank( void );

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
MulticastGroupInfo_t* CreateMulticastGroup( uint32_t grpAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        uint32_t channel, uint32_t interval );

/*! \brief Add mutlicast group. */
void MulticastGroupAdd( MulticastGroupInfo_t *multicastGrp );

/*! \brief Remove multicast group. */
void MulticastGroupRemove( MulticastGroupInfo_t *multicastGrp );

/*! \brief Print out multicast group information. */
void MulticastGroupPrint( MulticastGroupInfo_t* multicastGrp );
/*******************************************************************************
 * API FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaMesh_Init( LoRaMeshCallbacks_t *callbacks )
{
    uint8_t i;

    LoRaMeshCallbacks = callbacks;

    /* Initialized LoRa device structure */
    pLoRaDevice = (LoRaDevice_t*) malloc(sizeof(LoRaDevice_t));

    pLoRaDevice->devAddr = 0x00;

    pLoRaDevice->netId = 0x00;
    pLoRaDevice->devAddr = 0x00;
    pLoRaDevice->devEui = NULL;
    pLoRaDevice->devNonce = 0x00;
    pLoRaDevice->devClass = CLASS_A;
    pLoRaDevice->appEui = NULL;
    pLoRaDevice->appKey = NULL;
    pLoRaDevice->childNodes = NULL;
    pLoRaDevice->multicastGroups = NULL;
    for ( i = 0; i < 6; i++ )
        pLoRaDevice->channelsMask[i] = 0U;
    pLoRaDevice->adrAckCounter = 0x00;
    pLoRaDevice->nbRep = 1;
    pLoRaDevice->nbRepCounter = 0;

    /* Mac commands buffer */
    for ( i = 0; i < 15; i++ )
        pLoRaDevice->macCmdBuffer[i] = 0U;
    pLoRaDevice->macCmdBufferSize = 0U;

    /* Up link slot information */
    pLoRaDevice->upLinkSlot.Address = 0x00;
    pLoRaDevice->upLinkSlot.AppSKey = NULL;
    pLoRaDevice->upLinkSlot.NwkSKey = NULL;
    pLoRaDevice->upLinkSlot.UpLinkCounter = 0x01u;
    pLoRaDevice->upLinkSlot.DownLinkCounter = 0x00u;
    pLoRaDevice->upLinkSlot.ChannelIndex = 0u;
    pLoRaDevice->upLinkSlot.DataRateIndex = LORAMAC_DEFAULT_DATARATE;
    pLoRaDevice->upLinkSlot.TxPowerIndex = LORAMAC_DEFAULT_TX_POWER;

    /* Advertising slot information */
    pLoRaDevice->advertisingSlot.Time = 0u;
    pLoRaDevice->advertisingSlot.Interval = 0u;
    pLoRaDevice->advertisingSlot.Duration = 0u;

    /* Device flags */
    pLoRaDevice->ctrlFlags.Value = 0U;

    /* Debug flags */
    pLoRaDevice->dbgFlags.Value = 0U;

    /* Initialize stack */
    LoRaFrm_Init();
    LoRaMac_Init();
    LoRaPhy_Init();
}

void LoRaMesh_InitNwkIds( uint32_t netID, uint32_t devAddr, uint8_t *nwkSKey, uint8_t *appSKey )
{
    pLoRaDevice->netId = netID;
    pLoRaDevice->devAddr = devAddr;

    pLoRaDevice->upLinkSlot.AppSKey = malloc(16);
    memcpy(pLoRaDevice->upLinkSlot.AppSKey, appSKey, 16);

    pLoRaDevice->upLinkSlot.NwkSKey = malloc(16);
    memcpy(pLoRaDevice->upLinkSlot.NwkSKey, nwkSKey, 16);

    pLoRaDevice->ctrlFlags.Bits.nwkJoined = 1;
}

uint8_t LoRaMesh_SendFrame( uint8_t *appPayload, size_t appPayloadSize, uint8_t fPort,
        bool isUpLink, bool isConfirmed )
{
    uint8_t i, buf[LORAMESH_BUFFER_SIZE];

    if ( !LoRaMesh_IsNetworkJoined() ) {
        return ERR_NOTAVAIL; // No network has been joined yet
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

uint8_t LoRaMesh_OnPacketRx( uint8_t *buf, uint8_t payloadSize, uint8_t fPort, LoRaFrmType_t fType )
{
    return ERR_OK;
}

uint8_t LoRaMesh_PutPayload( uint8_t* buf, uint16_t bufSize, uint8_t payloadSize, uint8_t fPort,
        LoRaFrmType_t fType )
{
    /* Add app information */

    LOG_TRACE("%s - Size %d", __FUNCTION__, payloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < payloadSize; i++ )
        LOG_TRACE_BARE("0x%02x ", buf[i]);
    LOG_TRACE_BARE("\r\n");

    return LoRaFrm_PutPayload(buf, bufSize, payloadSize, fPort, fType, UP_LINK, false, false);
}

uint8_t LoRaMesh_ProcessAdvertising( uint8_t *aPayload, uint8_t aPayloadSize )
{
    return ERR_OK;
}

uint8_t LoRaMesh_JoinReq( uint8_t * devEui, uint8_t * appEui, uint8_t * appKey )
{
    uint8_t fPayloadSize = 0, fPayload[LORAFRM_BUFFER_SIZE];
    uint16_t devNonce;

    memcpy(&LORAFRM_BUF_PAYLOAD_START(fPayload)[fPayloadSize], appEui, 8);
    fPayloadSize += 8;
    memcpy(&LORAFRM_BUF_PAYLOAD_START(fPayload)[fPayloadSize], devEui, 8);
    fPayloadSize += 8;

    devNonce = LoRaPhy_GenerateNonce();
    pLoRaDevice->devNonce = devNonce;
    /* Save */
    memcpy(pLoRaDevice->appEui, appEui, 8);
    memcpy(pLoRaDevice->devEui, devEui, 8);
    memcpy(pLoRaDevice->appKey, appKey, 16);

    LORAFRM_BUF_PAYLOAD_START(fPayload)[fPayloadSize++] = devNonce & 0xFF;
    LORAFRM_BUF_PAYLOAD_START(fPayload)[fPayloadSize++] = (devNonce >> 8) & 0xFF;

    return ERR_FAILED;
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
    ChildNodeInfo_t* newNode = (ChildNodeInfo_t*) malloc(sizeof(ChildNodeInfo_t));
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
        free(childNode);
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
    LOG_DEBUG_BARE("%-15s: 0x%08x\r\n", "UpLinkCounter", childNode->Connection.UpLinkCounter);
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
MulticastGroupInfo_t* CreateMulticastGroup( uint32_t grpAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        uint32_t channel, uint32_t interval )
{
    MulticastGroupInfo_t* newGrp = (MulticastGroupInfo_t*) malloc(sizeof(MulticastGroupInfo_t));
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
        free(multicastGrp);
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
