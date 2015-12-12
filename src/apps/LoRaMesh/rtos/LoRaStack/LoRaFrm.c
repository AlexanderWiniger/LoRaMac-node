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

uint8_t LoRaFrm_OnPacketRx( LoRaPhy_PacketDesc *packet, uint32_t devAddr, LoRaFrmDir_t fDir,
        uint32_t fCnt )
{
    uint8_t fPayloadSize = 0, fBuffer[LORAFRM_BUFFER_SIZE];
    LoRaFrmType_t fType;
    LoRaFrmCtrl_t fCtrl;
    uint8_t fPort;

    fType = (LoRaFrmType_t)(packet->flags & LORAPHY_PACKET_FLAGS_FRM_MASK);
    fCtrl.Value = LORAFRM_BUF_CTRL(packet->phyData);
    fPort = packet->phyData[LORAFRM_BUF_IDX_PORT(fCtrl.Bits.FOptsLen)];

    if ( packet->flags & LORAPHY_PACKET_FLAGS_ACK_REQ ) {
        pLoRaDevice->ctrlFlags.Bits.ackRequested = 1;
    }

    switch (fType) {
        case FRM_TYPE_REGULAR:
            if ( fDir == DOWN_LINK ) {
                fPayloadSize = LORAPHY_BUF_SIZE(packet->phyData) - LORAFRM_HEADER_SIZE_MIN
                        - LORAFRM_PORT_SIZE - LORAMAC_HEADER_SIZE - LORAMAC_MIC_SIZE
                        - fCtrl.Bits.FOptsLen;
                if ( fPort == 0 ) {
                    /* Decrypt with encrypt function */
                    LoRaMacPayloadEncrypt(LORAFRM_BUF_PAYLOAD_START_WPORT(packet->phyData),
                            fPayloadSize, pLoRaDevice->upLinkSlot.NwkSKey, devAddr, fDir, fCnt,
                            fBuffer);

                    // Decode frame payload MAC commands
                    LoRaMac_ProcessCommands(fBuffer, 0, fPayloadSize);
                } else {
                    /* Decrypt with encrypt function */
                    LoRaMacPayloadEncrypt(LORAFRM_BUF_PAYLOAD_START_WPORT(packet->phyData),
                            fPayloadSize, pLoRaDevice->upLinkSlot.AppSKey, devAddr, fDir, fCnt,
                            fBuffer);
                }
                // Decode frame options MAC commands
                if ( fCtrl.Bits.FOptsLen > 0 )
                    LoRaMac_ProcessCommands(fBuffer, LORAFRM_HEADER_SIZE_MIN + LORAMAC_HEADER_SIZE,
                            fPayloadSize);
            }
            break;
        case FRM_TYPE_ADVERTISING:
            break;
        case FRM_TYPE_MULTICAST:
            break;
    }

    LOG_TRACE("%s - Size %d", __FUNCTION__, fPayloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < fPayloadSize; i++ )
        LOG_TRACE_BARE("0x%02x ", fBuffer[i]);
    LOG_TRACE_BARE("\r\n");

    return LoRaMesh_OnPacketRx((uint8_t*) &fBuffer, fPayloadSize, fPort, fType); /* Pass message up the stack */
}

uint8_t LoRaFrm_PutPayload( uint8_t* buf, uint16_t bufSize, uint8_t payloadSize, uint8_t fPort,
        LoRaFrmType_t fType, LoRaFrmDir_t fDir, bool confirmed, bool encrypted )
{
    uint8_t pktHdrSize = 0, fBuffer[LORAFRM_BUFFER_SIZE];
    LoRaFrmCtrl_t fCtrl;

    /* Initialize buffer */
    memset1(fBuffer, 0U, LORAFRM_BUFFER_SIZE);

    fCtrl.Value = 0;

    switch (fType) {
        case FRM_TYPE_REGULAR:
        {
            fCtrl.Bits.FOptsLen = 0;
            fCtrl.Bits.FPending = 0;
            fCtrl.Bits.Ack = (confirmed ? 1 : 0);
            fCtrl.Bits.AdrAckReq = 0;
            fCtrl.Bits.Adr = pLoRaDevice->ctrlFlags.Bits.adrCtrlOn;

            /* Payload encryption */
            if ( (payloadSize > 0) && (fPort == 0) ) { /* Encrypt frame payload with NwkSKey */
                LoRaMacPayloadEncrypt(buf, payloadSize, pLoRaDevice->upLinkSlot.NwkSKey,
                        pLoRaDevice->devAddr, fDir, pLoRaDevice->upLinkSlot.UpLinkCounter,
                        (LORAFRM_BUF_PAYLOAD_START_WPORT(fBuffer) + pLoRaDevice->macCmdBufferSize));
            } else if ( (payloadSize > 0) && (!encrypted) ) { /* Encrypt frame payload with AppSKey */
                LoRaMacPayloadEncrypt(buf, payloadSize, pLoRaDevice->upLinkSlot.AppSKey,
                        pLoRaDevice->devAddr, fDir, pLoRaDevice->upLinkSlot.UpLinkCounter,
                        (LORAFRM_BUF_PAYLOAD_START_WPORT(fBuffer) + pLoRaDevice->macCmdBufferSize));
            }

            /* Send ACK if pending */
            if ( pLoRaDevice->ctrlFlags.Bits.ackRequested == 1U ) {
                pLoRaDevice->ctrlFlags.Bits.ackRequested = 0;
                fCtrl.Bits.Ack = 1;
            }

            /* Adjust datarate if adr is enabled */
            if ( fCtrl.Bits.Adr == true ) {
                if ( pLoRaDevice->upLinkSlot.DataRateIndex == LORAMAC_MIN_DATARATE ) {
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
                        if ( pLoRaDevice->upLinkSlot.DataRateIndex > LORAMAC_MIN_DATARATE ) {
                            pLoRaDevice->upLinkSlot.DataRateIndex--;
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

            fBuffer[LORAFRM_BUF_IDX_CNTR] = pLoRaDevice->upLinkSlot.UpLinkCounter & 0xFF;
            fBuffer[LORAFRM_BUF_IDX_CNTR + 1] = (pLoRaDevice->upLinkSlot.UpLinkCounter >> 8) & 0xFF;

            pktHdrSize += LORAFRM_HEADER_SIZE_MIN;

//            if ( fCtrl.Bits.FOptsLen > 0 ) {
//                LoRaMac_AddCommands(&buf[LORAFRM_BUF_IDX_OPTS], 15);
//                pktHdrSize += fCtrl.Bits.FOptsLen;
//            }

            if ( payloadSize > 0 ) {
                fBuffer[LORAFRM_BUF_IDX_PORT(/*fOptsLen*/0)] = fPort;
                pktHdrSize++;
            }

            payloadSize += pktHdrSize;
        }
            break;
            break;
        case FRM_TYPE_ADVERTISING:
            break;
        case FRM_TYPE_MULTICAST:
            break;
        default:
            return ERR_INVALID_TYPE;
            break;
    }

    LoRaMacMsgType_t msgType;
    if ( fDir == UP_LINK ) {
        msgType = (confirmed ? MSG_TYPE_DATA_CONFIRMED_UP : MSG_TYPE_DATA_UNCONFIRMED_UP);
    } else {
        msgType = (confirmed ? MSG_TYPE_DATA_CONFIRMED_DOWN : MSG_TYPE_DATA_UNCONFIRMED_DOWN);
    }

    LOG_TRACE("%s - Size %d", __FUNCTION__, payloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < (payloadSize + 3); i++ )
        LOG_TRACE_BARE("0x%02x ", fBuffer[i]);
    LOG_TRACE_BARE("\r\n");

    return LoRaMac_PutPayload(fBuffer, sizeof(fBuffer), payloadSize, msgType);
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
