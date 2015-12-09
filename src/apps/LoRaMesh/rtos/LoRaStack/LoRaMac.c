/**
 * \file LoRaMac.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 03.12.2015
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
/*! Maximum MAC layer payload size */
#define LORA_MAC_MAXPAYLOAD                      250

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
void LoRaMac_Init( void )
{

}

uint8_t LoRaMac_OnPacketRx( LoRaPhy_PacketDesc *packet )
{
    uint8_t *payload, payloadSize, macRxBuffer[LORAMAC_BUFFER_SIZE];
    uint32_t micRx = 0, mic = 0, frameCntr = 0, devAddr, rxAddr;
    uint16_t sequenceCntr = 0, sequenceCntrDiff = 0;
    MulticastGroupInfo_t *curMulticastGrp = NULL;
    ChildNodeInfo_t *curChildNode = NULL;
    LoRaMacHeader_t macHdr;

    payload = packet->phyData;
    payloadSize = LORAPHY_BUF_SIZE(packet->phyData);

    /* Check if incoming packet is an advertising beacon */
    if ( packet->flags & LORAPHY_PACKET_FLAGS_TX_ADVERTISING ) {
        LOG_TRACE("Received advertising beacon.");
        return LoRaMesh_ProcessAdvertising(payload, payloadSize);
    }

    macHdr.Value = LORAMAC_BUF_HDR(packet->phyData);

    /* Check if message version matches*/
    if ( macHdr.Bits.Major != LORAMESH_CONFIG_MAJOR_VERSION ) return ERR_FAILED;

    switch (macHdr.Bits.MType) {
        case MSG_TYPE_JOIN_REQ:
            LOG_TRACE("Received join request.");
            break;
        case MSG_TYPE_JOIN_ACCEPT:
        {
            if ( pLoRaDevice->ctrlFlags.Bits.nwkJoined == 1 ) {
                return ERR_FAILED;
            }
            LoRaMacJoinDecrypt(LORAMAC_BUF_PAYLOAD_START(payload), payloadSize - 1,
                    pLoRaDevice->appKey, macRxBuffer + 1);

            macRxBuffer[0] = macHdr.Value;

            LoRaMacJoinComputeMic(macRxBuffer, payloadSize - LORAMAC_MIC_SIZE, pLoRaDevice->appKey,
                    &mic);

            micRx |= ((uint32_t) macRxBuffer[payloadSize - LORAMAC_MIC_SIZE]);
            micRx |= ((uint32_t) macRxBuffer[payloadSize - LORAMAC_MIC_SIZE + 1] << 8);
            micRx |= ((uint32_t) macRxBuffer[payloadSize - LORAMAC_MIC_SIZE + 2] << 16);
            micRx |= ((uint32_t) macRxBuffer[payloadSize - LORAMAC_MIC_SIZE + 3] << 24);

            if ( micRx == mic ) {
                uint8_t receiveDelay1, receiveDelay2;

                LoRaMacJoinComputeSKeys(pLoRaDevice->appKey, (macRxBuffer + 1),
                        pLoRaDevice->devNonce, pLoRaDevice->nwkSKey, pLoRaDevice->appSKey);

                pLoRaDevice->netId = (uint32_t) macRxBuffer[4];
                pLoRaDevice->netId |= ((uint32_t) macRxBuffer[5] << 8);
                pLoRaDevice->netId |= ((uint32_t) macRxBuffer[6] << 16);

                pLoRaDevice->devAddr = (uint32_t) macRxBuffer[7];
                pLoRaDevice->devAddr |= ((uint32_t) macRxBuffer[8] << 8);
                pLoRaDevice->devAddr |= ((uint32_t) macRxBuffer[9] << 16);
                pLoRaDevice->devAddr |= ((uint32_t) macRxBuffer[10] << 24);

                // DLSettings
                LoRaPhy_SetDownLinkSettings((macRxBuffer[11] >> 4) & 0x07, macRxBuffer[11] & 0x0F);
                // RxDelay
                receiveDelay1 = (macRxBuffer[12] & 0x0F);
                if ( receiveDelay1 == 0 ) {
                    receiveDelay1 = 1;
                }
                receiveDelay1 *= 1e3;
                receiveDelay2 = receiveDelay1 + 1e3;
                /* Set Rx1 and Rx2 delays */
                LoRaPhy_SetReceiveDelay1(receiveDelay1);
                LoRaPhy_SetReceiveDelay2(receiveDelay2);

                //CFList
                if ( (payloadSize - 1) > 16 ) {
                    ChannelParams_t param;
                    param.DrRange.Value = (DR_5 << 4) | DR_0;

                    for ( uint8_t i = 3, j = 0; i < (5 + 3); i++, j += 3 ) {
                        param.Frequency = ((uint32_t) macRxBuffer[13 + j]
                                | ((uint32_t) macRxBuffer[14 + j] << 8)
                                | ((uint32_t) macRxBuffer[15 + j] << 16)) * 100;
                        LoRaPhy_SetChannel(i, param);
                    }
                }
                pLoRaDevice->ctrlFlags.Bits.nwkJoined = 1;
                return ERR_OK;
            } else {
                return ERR_FAILED;
            }
            break;
        }
        case MSG_TYPE_DATA_CONFIRMED_DOWN: /* Intentional fall through */
        case MSG_TYPE_DATA_UNCONFIRMED_DOWN:
        {
            rxAddr = macRxBuffer[LORAFRM_BUF_IDX_DEVADDR];
            rxAddr |= ((uint32_t) macRxBuffer[LORAFRM_BUF_IDX_DEVADDR + 1] << 8);
            rxAddr |= ((uint32_t) macRxBuffer[LORAFRM_BUF_IDX_DEVADDR + 2] << 16);
            rxAddr |= ((uint32_t) macRxBuffer[LORAFRM_BUF_IDX_DEVADDR + 3] << 24);

            if ( packet->flags & LORAPHY_PACKET_FLAGS_TX_MULTICAST ) {
                curMulticastGrp = LoRaMesh_FindMulticastGroup(rxAddr);
                if ( curMulticastGrp != NULL ) {
                    frameCntr = curMulticastGrp->DownLinkCounter;
                    devAddr = curMulticastGrp->Address;
                } else return ERR_FAILED;
            } else {
                frameCntr = pLoRaDevice->downLinkCounter;
                devAddr = pLoRaDevice->devAddr;
            }
            break;
        }
        case MSG_TYPE_DATA_CONFIRMED_UP: /* Intentional fall through */
        case MSG_TYPE_DATA_UNCONFIRMED_UP:
        {
            rxAddr = macRxBuffer[LORAFRM_BUF_IDX_DEVADDR];
            rxAddr |= ((uint32_t) macRxBuffer[LORAFRM_BUF_IDX_DEVADDR + 1] << 8);
            rxAddr |= ((uint32_t) macRxBuffer[LORAFRM_BUF_IDX_DEVADDR + 2] << 16);
            rxAddr |= ((uint32_t) macRxBuffer[LORAFRM_BUF_IDX_DEVADDR + 3] << 24);

            curChildNode = LoRaMesh_FindChildNode(rxAddr);
            if ( curChildNode != NULL ) {
                frameCntr = curChildNode->UpLinkCounter;
                devAddr = curChildNode->Address;
                if ( rxAddr != devAddr ) return ERR_FAILED;
            }
            break;
        }
        case MSG_TYPE_PROPRIETARY:
            //Intentional falltrough
        default:
            return ERR_INVALID_TYPE;
            break;
    }

    micRx |= ((uint32_t) macRxBuffer[payloadSize - LORAMAC_MIC_SIZE]);
    micRx |= ((uint32_t) macRxBuffer[payloadSize - LORAMAC_MIC_SIZE + 1] << 8);
    micRx |= ((uint32_t) macRxBuffer[payloadSize - LORAMAC_MIC_SIZE + 2] << 16);
    micRx |= ((uint32_t) macRxBuffer[payloadSize - LORAMAC_MIC_SIZE + 3] << 24);

    sequenceCntr = (uint16_t) macRxBuffer[LORAFRM_BUF_IDX_CNTR];
    sequenceCntr |= (uint16_t) macRxBuffer[LORAFRM_BUF_IDX_CNTR + 1] << 8;
    sequenceCntrDiff = (sequenceCntr - ((uint16_t)(frameCntr & 0xFFFF)));

    if ( sequenceCntrDiff < (1 << 15) ) frameCntr += sequenceCntrDiff;
    else frameCntr += 0x10000 + (int16_t) sequenceCntrDiff;

    LoRaMacComputeMic(macRxBuffer, payloadSize - LORAMAC_MIC_SIZE, pLoRaDevice->nwkSKey, devAddr,
            direction, frameCntr, &mic);

    if ( mic == micRx ) return LoRaFrm_OnPacketRx(packet); /* Pass message up the stack */

    return ERR_FAILED;
}

uint8_t LoRaMac_PutPayload( uint8_t* buf, size_t bufSize, size_t payloadSize,
        LoRaMessageType_t type )
{
    LoRaMacHeader_t macHdr;
    uint8_t flags = LORAPHY_PACKET_FLAGS_NONE;
    uint32_t mic = 0;

    macHdr.Value = 0;

    macHdr.Bits.MType = type;
    macHdr.Bits.Major = LORAMESH_CONFIG_MAJOR_VERSION;

    buf[LORAMAC_BUF_IDX_HDR] = macHdr.Value;
    payloadSize++;

    switch (macHdr.Bits.MType) {
        case MSG_TYPE_JOIN_REQ:
            LoRaMacJoinComputeMic(buf, payloadSize & 0xFF, pLoRaDevice->appKey, &mic);
            break;
        case MSG_TYPE_JOIN_ACCEPT:
            break;
        case MSG_TYPE_DATA_UNCONFIRMED_UP:
        case MSG_TYPE_DATA_CONFIRMED_UP:
        {
            LoRaMacComputeMic(buf, payloadSize, pLoRaDevice->nwkSKey, pLoRaDevice->devAddr, UP_LINK,
                    pLoRaDevice->downLinkCounter, &mic);

            if ( (payloadSize + LORAMAC_MIC_SIZE) > LORAMAC_PAYLOAD_SIZE ) {
                return ERR_OVERFLOW;
            }

            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = mic & 0xFF;
            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = (mic >> 8) & 0xFF;
            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = (mic >> 16) & 0xFF;
            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = (mic >> 24) & 0xFF;

            flags = LORAPHY_PACKET_FLAGS_TX_REGULAR;
            break;
        }
        case MSG_TYPE_DATA_UNCONFIRMED_DOWN:
        case MSG_TYPE_DATA_CONFIRMED_DOWN:
            LOG_ERROR("Message type data down not yet implemented.");
            break;
        default:
            return ERR_INVALID_TYPE;
    }

    LOG_TRACE("%s - Size %d", __FUNCTION__, payloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < (payloadSize + 2); i++ )
        LOG_TRACE_BARE("0x%02x ", buf[i]);
    LOG_TRACE_BARE("\r\n");

    return LoRaPhy_PutPayload(buf, bufSize, payloadSize, flags);
}
/*******************************************************************************
 * PUBLIC SETUP FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
