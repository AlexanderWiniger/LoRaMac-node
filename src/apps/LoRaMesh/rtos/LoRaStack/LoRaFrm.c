/**
 * \file LoRaFrm.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 03.12.2015
 * \version 1.0
 *
 * \brief LoRa stack network layer implementation
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
/*! Maximum network layer payload size in absence of FOpts field */
#define ADR_ACK_LIMIT                       LORAMESH_CONFIG_ADR_ACK_LIMIT
#define ADR_ACK_DELAY                       LORAMESH_CONFIG_ADR_ACK_DELAY

/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * API FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaFrm_Init( void )
{

}

uint8_t LoRaFrm_OnPacketRx( LoRaPhy_PacketDesc *packet )
{
    return ERR_OK;
}

uint8_t LoRaFrm_PutPayload( uint8_t* fBuffer, uint16_t fBufferSize, uint8_t fPayloadSize,
        uint8_t fPort, uint8_t* fOpts, uint8_t fOptsLen, LoRaMessageType_t type )
{
    uint8_t pktHdrSize = 0;
    LoRaFrmCtrl_t fCtrl;

    fCtrl.Value = 0;

    fCtrl.Bits.FOptsLen = fOptsLen;
    fCtrl.Bits.FPending = 0;
    fCtrl.Bits.Ack = 0;
    fCtrl.Bits.AdrAckReq = 0;
    fCtrl.Bits.Adr = pLoRaDevice->ctrlFlags.Bits.adrCtrlOn;

    /* Payload encryption */
    if ( fPort == 0 ) { /* Encrypt frame payload with NwkSKey */
        LoRaMacPayloadEncrypt(fPayload, fPayloadSize, pLoRaDevice->nwkSKey, pLoRaDevice->devAddr,
                UP_LINK, pLoRaDevice->upLinkCounter,
                LORAFRM_BUF_PAYLOAD_START(fBuffer, pLoRaDevice->macCmdBufferSize));
    } else { /* Encrypt frame payload with AppSKey */
        LoRaMacPayloadEncrypt(fPayload, fPayloadSize, pLoRaDevice->appSKey, pLoRaDevice->devAddr,
                UP_LINK, pLoRaDevice->upLinkCounter,
                LORAFRM_BUF_PAYLOAD_START(fBuffer, pLoRaDevice->macCmdBufferSize));
    }

    /* Send ACK if pending */
    if ( pLoRaDevice->ctrlFlags.Bits.ackRequested == 1U ) {
        pLoRaDevice->ctrlFlags.Bits.ackRequested = 0;
        fCtrl.Bits.Ack = 1;
    }

    /* Adjust datarate if adr is enabled */
    if ( fCtrl.Bits.Adr == true ) {
        if ( pLoRaDevice->curDatarateIdx == LORAMAC_MIN_DATARATE ) {
            pLoRaDevice->adrAckCounter = 0;
            fCtrl.Bits.AdrAckReq = false;
        } else {
            if ( pLoRaDevice->adrAckCounter > ADR_ACK_LIMIT ) {
                fCtrl.Bits.AdrAckReq = true;
            } else {
                fCtrl.Bits.AdrAckReq = false;
            }
            if ( pLoRaDevice->adrAckCounter > (ADR_ACK_LIMIT + ADR_ACK_DELAY) ) {
                pLoRaDevice->adrAckCounter = 0;
                if ( pLoRaDevice->curDatarateIdx > LORAMAC_MIN_DATARATE ) {
                    pLoRaDevice->curDatarateIdx--;
                } else {
                    // Re-enable default channels LC1, LC2, LC3
                    pLoRaDevice->channelsMask[0] |= (LC(1) + LC(2) + LC(3));
                }
            }
        }
    }

    fBuffer[LORAFRM_BUF_IDX_DEVADDR] = (pLoRaDevice->devAddr) & 0xFF;
    fBuffer[LORAFRM_BUF_IDX_DEVADDR + 1] = (pLoRaDevice->devAddr >> 8) & 0xFF;
    fBuffer[LORAFRM_BUF_IDX_DEVADDR + 2] = (pLoRaDevice->devAddr >> 16) & 0xFF;
    fBuffer[LORAFRM_BUF_IDX_DEVADDR + 3] = (pLoRaDevice->devAddr >> 24) & 0xFF;

    fBuffer[LORAFRM_BUF_IDX_CTRL] = fCtrl.Value;

    fBuffer[LORAFRM_BUF_IDX_CNTR] = pLoRaDevice->upLinkCounter & 0xFF;
    fBuffer[LORAFRM_BUF_IDX_CNTR + 1] = (pLoRaDevice->upLinkCounter >> 8) & 0xFF;

    pktHdrSize += LORAFRM_HEADER_SIZE_MIN;

    if ( fOptsLen > 0 ) {
        for ( uint8_t i = 0; i < fOptsLen; i++ ) {
            fBuffer[LORAFRM_BUF_IDX_OPTS + i] = fOpts[i];
        }
        pktHdrSize += fOptsLen;
    }

    if ( fPayloadSize > 0 ) {
        fBuffer[LORAFRM_BUF_IDX_PORT(fOptsLen)] = fPort;
        pktHdrSize++;
    }

    fPayloadSize += pktHdrSize;

    return LoRaMac_PutPayload(fBuffer, fBufferSize, fPayloadSize, type);
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
