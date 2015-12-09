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

uint8_t LoRaFrm_OnPacketRx( LoRaPhy_PacketDesc *packet, LoRaMessageType_t type )
{
    LoRaFrmCtrl_t fCtrl;

    uint8_t pktHeaderLen = 0;
    uint32_t address = 0;
    uint8_t appPayloadStartIndex = 0;
    uint8_t port = 0xFF;
    uint8_t frameLen = 0;
    uint32_t mic = 0;
    uint32_t micRx = 0;

    uint16_t sequenceCounter = 0;
    uint16_t sequenceCounterPrev = 0;
    uint16_t sequenceCounterDiff = 0;
    uint32_t linkCounter = 0;

    MulticastGroupInfo_t *curMulticastGrp = NULL;
    ChildNodeInfo_t *curChildNode = NULL;
    uint8_t *nwkSKey = LoRaMacNwkSKey;
    uint8_t *appSKey = LoRaMacAppSKey;

    bool isMicOk = false;
    FrameDirection_t direction;

    if ( type == MSG_TYPE_DATA_UNCONFIRMED_UP || type == MSG_TYPE_DATA_CONFIRMED_UP ) {
        direction = UP_LINK;
    } else if ( type == MSG_TYPE_DATA_UNCONFIRMED_DOWN || type == MSG_TYPE_DATA_CONFIRMED_DOWN ) {
        direction = DOWN_LINK;
    } else {
        return ERR_FAILED;
    }

    address = payload[pktHeaderLen++];
    address |= ((uint32_t) payload[pktHeaderLen++] << 8);
    address |= ((uint32_t) payload[pktHeaderLen++] << 16);
    address |= ((uint32_t) payload[pktHeaderLen++] << 24);

    if ( direction == UP_LINK ) {
        /* Up link, therefore child node */
        curChildNode = ChildNodeFind(address);
        if ( curChildNode != NULL ) {
            nwkSKey = curChildNode->NwkSKey;
            appSKey = curChildNode->AppSKey;
            linkCounter = curChildNode->UpLinkCounter;
        } else {
            /* We are not the destination of this frame. */
            LOG_ERROR("Address didn't match any child node");
            LoRaMacEventFlags.Bits.Tx = 1;
            LoRaMacEventInfo.Status = LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL;
            LoRaMacState &= ~MAC_TX_RUNNING;
            return;
        }
    } else {
        /* Down link, therefore either from parent node or multicast group */
        if ( address != LoRaMacDevAddr ) {
            curMulticastGrp = MulticastGroupFind(address);
            if ( curMulticastGrp != NULL ) {
                LoRaMacEventFlags.Bits.Multicast = 1;
                nwkSKey = curMulticastGrp->NwkSKey;
                appSKey = curMulticastGrp->AppSKey;
                linkCounter = curMulticastGrp->DownLinkCounter;
            } else {
                /* We are not the destination of this frame. */
                LoRaMacEventFlags.Bits.Tx = 1;
                LoRaMacEventInfo.Status = LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL;
                LoRaMacState &= ~MAC_TX_RUNNING;
                return;
            }
        } else {
            LoRaMacEventFlags.Bits.Multicast = 0;
            nwkSKey = LoRaMacNwkSKey;
            appSKey = LoRaMacAppSKey;
            linkCounter = DownLinkCounter;
        }
    }

    fCtrl.Value = payload[pktHeaderLen++];

    sequenceCounter = (uint16_t) payload[pktHeaderLen++];
    sequenceCounter |= (uint16_t) payload[pktHeaderLen++] << 8;

    appPayloadStartIndex = 8 + fCtrl.Bits.FOptsLen;

    micRx |= (uint32_t) payload[size - LORAMAC_MFR_LEN];
    micRx |= ((uint32_t) payload[size - LORAMAC_MFR_LEN + 1] << 8);
    micRx |= ((uint32_t) payload[size - LORAMAC_MFR_LEN + 2] << 16);
    micRx |= ((uint32_t) payload[size - LORAMAC_MFR_LEN + 3] << 24);

    sequenceCounterPrev = (uint16_t) linkCounter;
    sequenceCounterDiff = (sequenceCounter - sequenceCounterPrev);

    if ( sequenceCounterDiff < (1 << 15) ) {
        linkCounter += sequenceCounterDiff;
        LoRaMacComputeMic(payload, size - LORAMAC_MFR_LEN, nwkSKey, address, direction, linkCounter,
                &mic);
        if ( micRx == mic ) {
            isMicOk = true;
        }
    } else {
        // check for link counter roll-over
        uint32_t linkCounterTmp = linkCounter + 0x10000 + (int16_t) sequenceCounterDiff;
        LoRaMacComputeMic(payload, size - LORAMAC_MFR_LEN, nwkSKey, address, direction,
                linkCounterTmp, &mic);
        if ( micRx == mic ) {
            isMicOk = true;
            linkCounterTmp = linkCounterTmp;
        }
    }

    if ( isMicOk ) {
        LoRaMacEventFlags.Bits.Rx = 1;
        LoRaMacEventInfo.RxSnr = snr;
        LoRaMacEventInfo.RxRssi = rssi;
        LoRaMacEventInfo.RxBufferSize = 0;
        AdrAckCounter = 0;

        // Update 32 bits downlink counter
        if ( LoRaMacEventFlags.Bits.Multicast == 1 ) {
            curMulticastGrp->DownLinkCounter = linkCounter;
        } else {
            DownLinkCounter = linkCounter;
        }

        if ( macHdr.Bits.MType == FRAME_TYPE_DATA_CONFIRMED_DOWN ) {
            SrvAckRequested = true;
        } else {
            SrvAckRequested = false;
        }

        // Check if the frame is an acknowledgement
        if ( fCtrl.Bits.Ack == 1 ) {
            LoRaMacEventInfo.TxAckReceived = true;

            // Stop the AckTimeout timer as no more retransmissions
            // are needed.
            TimerStop (&AckTimeoutTimer);
        } else {
            LoRaMacEventInfo.TxAckReceived = false;
            if ( AckTimeoutRetriesCounter > AckTimeoutRetries ) {
                // Stop the AckTimeout timer as no more retransmissions
                // are needed.
                TimerStop (&AckTimeoutTimer);
            }
        }

        if ( (fCtrl.Bits.FOptsLen > 0) && (LoRaMacEventFlags.Bits.Multicast == 0) ) {
            // Decode Options field MAC commands
            ProcessMacCommands(payload, 8, appPayloadStartIndex);
        }

        if ( ((size - 4) - appPayloadStartIndex) > 0 ) {
            port = payload[appPayloadStartIndex++];
            frameLen = (size - 4) - appPayloadStartIndex;

            if ( port == 0 ) {
                LoRaMacPayloadDecrypt(payload + appPayloadStartIndex, frameLen, nwkSKey, address,
                        direction, linkCounter, LoRaMacRxPayload);

                // Decode frame payload MAC commands
                ProcessMacCommands(LoRaMacRxPayload, 0, frameLen);
            } else {
                LoRaMacPayloadDecrypt(payload + appPayloadStartIndex, frameLen, appSKey, address,
                        direction, linkCounter, LoRaMacRxPayload);

                LoRaMacEventFlags.Bits.RxData = 1;
                LoRaMacEventInfo.RxPort = port;
                LoRaMacEventInfo.RxBuffer = LoRaMacRxPayload;
                LoRaMacEventInfo.RxBufferSize = frameLen;
            }
        }

        LoRaMacEventFlags.Bits.Tx = 1;
        LoRaMacEventInfo.Status = LORAMAC_EVENT_INFO_STATUS_OK;
    } else {
        LoRaMacEventInfo.TxAckReceived = false;

        LoRaMacEventFlags.Bits.Tx = 1;
        LoRaMacEventInfo.Status = LORAMAC_EVENT_INFO_STATUS_MIC_FAIL;
        LoRaMacState &= ~MAC_TX_RUNNING;
    }

    return LoRaMesh_OnPacketRx(packet); /* Pass message up the stack */
}

uint8_t LoRaFrm_PutPayload( uint8_t* buf, uint16_t bufSize, uint8_t payloadSize, uint8_t fPort,
        uint8_t* fOpts, uint8_t fOptsLen, LoRaMessageType_t type, bool encrypted )
{
    switch (type) {
        case MSG_TYPE_JOIN_REQ:
            break;
        case MSG_TYPE_JOIN_ACCEPT:
            break;
        case MSG_TYPE_DATA_CONFIRMED_DOWN:
        case MSG_TYPE_DATA_UNCONFIRMED_DOWN:
        case MSG_TYPE_DATA_CONFIRMED_UP:
        case MSG_TYPE_DATA_UNCONFIRMED_UP:
        {
            uint8_t pktHdrSize = 0, fBuffer[LORAFRM_BUFFER_SIZE];
            LoRaFrmCtrl_t fCtrl;

            /* Initialize buffer */
            memset1(fBuffer, 0U, LORAFRM_BUFFER_SIZE);

            fCtrl.Value = 0;

            fCtrl.Bits.FOptsLen = fOptsLen;
            fCtrl.Bits.FPending = 0;
            fCtrl.Bits.Ack = 0;
            fCtrl.Bits.AdrAckReq = 0;
            fCtrl.Bits.Adr = pLoRaDevice->ctrlFlags.Bits.adrCtrlOn;

            /* Payload encryption */
            if ( (payloadSize > 0) && (fPort == 0) ) { /* Encrypt frame payload with NwkSKey */
                LoRaMacPayloadEncrypt(buf, payloadSize, pLoRaDevice->nwkSKey, pLoRaDevice->devAddr,
                        UP_LINK, pLoRaDevice->upLinkCounter,
                        (LORAFRM_BUF_PAYLOAD_START_WPORT(fBuffer) + pLoRaDevice->macCmdBufferSize));
            } else if ( (payloadSize > 0) && (!encrypted) ) { /* Encrypt frame payload with AppSKey */
                LoRaMacPayloadEncrypt(buf, payloadSize, pLoRaDevice->appSKey, pLoRaDevice->devAddr,
                        UP_LINK, pLoRaDevice->upLinkCounter,
                        (LORAFRM_BUF_PAYLOAD_START_WPORT(fBuffer) + pLoRaDevice->macCmdBufferSize));
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

            if ( payloadSize > 0 ) {
                fBuffer[LORAFRM_BUF_IDX_PORT(fOptsLen)] = fPort;
                pktHdrSize++;
            }

            payloadSize += pktHdrSize;
        }
            break;
        case MSG_TYPE_PROPRIETARY:
            //Intentional falltrough
        default:
            return ERR_INVALID_TYPE;
            break;
    }

    LOG_TRACE("%s - Size %d", __FUNCTION__, payloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < (payloadSize + 3); i++ )
        LOG_TRACE_BARE("0x%02x ", fBuffer[i]);
    LOG_TRACE_BARE("\r\n");

    return LoRaMac_PutPayload(fBuffer, sizeof(fBuffer), payloadSize, type);
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
