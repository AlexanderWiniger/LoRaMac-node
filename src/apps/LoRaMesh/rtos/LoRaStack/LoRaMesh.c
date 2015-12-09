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
LoRaDevice_t* pLoRaDevice;

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*! LoRaMesh upper layer event functions */
static LoRaMeshCallbacks_t *LoRaMeshCallbacks;
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
    pLoRaDevice->nwkSKey = NULL;
    pLoRaDevice->appSKey = NULL;
    pLoRaDevice->upLinkCounter = 0x01;
    pLoRaDevice->downLinkCounter = 0x00;
    pLoRaDevice->adrAckCounter = 0x00;
    pLoRaDevice->nbRep = 1;
    pLoRaDevice->nbRepCounter = 0;
    for ( i = 0; i < 6; i++ )
        pLoRaDevice->channelsMask[i] = 0U;
    pLoRaDevice->curChannelIdx = 0U;
    pLoRaDevice->curDatarateIdx = LORAMAC_DEFAULT_DATARATE;
    pLoRaDevice->curTxPowerIdx = LORAMAC_DEFAULT_TX_POWER;
    for ( i = 0; i < 15; i++ )
        pLoRaDevice->macCmdBuffer[i] = 0U;
    pLoRaDevice->macCmdBufferSize = 0U;

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

    pLoRaDevice->appSKey = malloc(16);
    memcpy(pLoRaDevice->appSKey, appSKey, 16);

    pLoRaDevice->nwkSKey = malloc(16);
    memcpy(pLoRaDevice->nwkSKey, nwkSKey, 16);

    pLoRaDevice->ctrlFlags.Bits.nwkJoined = 1;
}

uint8_t LoRaMesh_SendFrame( uint8_t *appPayload, size_t appPayloadSize, uint8_t fPort,
        bool isUpLink, bool isConfirmed )
{
    uint8_t i, buf[LORAMESH_BUFFER_SIZE];
    LoRaMessageType_t msgType;

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

    if ( isUpLink ) {
        msgType = ((isConfirmed) ? MSG_TYPE_DATA_CONFIRMED_UP : MSG_TYPE_DATA_UNCONFIRMED_UP);
    } else {
        msgType = ((isConfirmed) ? MSG_TYPE_DATA_CONFIRMED_DOWN : MSG_TYPE_DATA_UNCONFIRMED_DOWN);
    }

    return LoRaMesh_PutPayload(buf, sizeof(buf), appPayloadSize, fPort, NULL, 0, msgType);

}

uint8_t LoRaMesh_OnPacketRx( LoRaPhy_PacketDesc* packet )
{
    return ERR_OK;
}

uint8_t LoRaMesh_PutPayload( uint8_t* buf, uint16_t bufSize, uint8_t payloadSize, uint8_t fPort,
        uint8_t* fOpts, uint8_t fOptsLen, LoRaMessageType_t type )
{
    /* Add app information */

    LOG_TRACE("%s - Size %d", __FUNCTION__, payloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < payloadSize; i++ )
        LOG_TRACE_BARE("0x%02x ", buf[i]);
    LOG_TRACE_BARE("\r\n");

    return LoRaFrm_PutPayload(buf, bufSize, payloadSize, fPort, fOpts, fOptsLen, type, false);
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

    return LoRaFrm_PutPayload(fPayload, LORAFRM_PAYLOAD_SIZE, fPayloadSize, 0, NULL, 0,
            MSG_TYPE_JOIN_REQ, false);
}

bool LoRaMesh_IsNetworkJoined( void )
{
    if ( pLoRaDevice != NULL ) {
        return (pLoRaDevice->ctrlFlags.Bits.nwkJoined == 1U);
    } else {
        return false;
    }
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
        pLoRaDevice->upLinkCounter = upLinkCounter;
        pLoRaDevice->dbgFlags.Bits.upLinkCounterFixed = 1;
    }
}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
