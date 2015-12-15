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
    LoRaFrmDir_t frameDir;
    LoRaMacHdr_t macHdr;

    payload = packet->phyData;
    payloadSize = LORAPHY_BUF_SIZE(packet->phyData);

    /* Check if incoming packet is an advertising beacon */
    if ( (packet->flags & LORAPHY_PACKET_FLAGS_FRM_MASK) == LORAPHY_PACKET_FLAGS_FRM_ADVERTISING ) {
        LOG_TRACE("Received advertising beacon.");
        return LoRaMesh_ProcessAdvertising(payload, payloadSize);
    }

    macHdr.Value = LORAMAC_BUF_HDR(packet->phyData);

    /* Check if message version matches*/
    if ( macHdr.Bits.Major != LORAMESH_CONFIG_MAJOR_VERSION ) return ERR_FAILED;

    switch ( macHdr.Bits.MType ) {
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
                        pLoRaDevice->devNonce, pLoRaDevice->upLinkSlot.NwkSKey,
                        pLoRaDevice->upLinkSlot.AppSKey);

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
            packet->flags |= LORAPHY_PACKET_FLAGS_ACK_REQ;
        case MSG_TYPE_DATA_UNCONFIRMED_DOWN:
        {
            rxAddr = payload[LORAFRM_BUF_IDX_DEVADDR];
            rxAddr |= ((uint32_t) payload[LORAFRM_BUF_IDX_DEVADDR + 1] << 8);
            rxAddr |= ((uint32_t) payload[LORAFRM_BUF_IDX_DEVADDR + 2] << 16);
            rxAddr |= ((uint32_t) payload[LORAFRM_BUF_IDX_DEVADDR + 3] << 24);

            if ( (packet->flags & LORAPHY_PACKET_FLAGS_FRM_MASK)
                    == LORAPHY_PACKET_FLAGS_FRM_MULTICAST ) {
                curMulticastGrp = LoRaMesh_FindMulticastGroup(rxAddr);
                if ( curMulticastGrp != NULL ) {
                    frameCntr = curMulticastGrp->Connection.DownLinkCounter;
                    devAddr = curMulticastGrp->Connection.Address;
                } else
                    return ERR_FAILED;
            } else {
                frameCntr = pLoRaDevice->upLinkSlot.DownLinkCounter;
                devAddr = pLoRaDevice->devAddr;
            }

            frameDir = DOWN_LINK;
            break;
        }
        case MSG_TYPE_DATA_CONFIRMED_UP: /* Intentional fall through */
            packet->flags |= LORAPHY_PACKET_FLAGS_ACK_REQ;
        case MSG_TYPE_DATA_UNCONFIRMED_UP:
        {
            rxAddr = payload[LORAFRM_BUF_IDX_DEVADDR];
            rxAddr |= ((uint32_t) payload[LORAFRM_BUF_IDX_DEVADDR + 1] << 8);
            rxAddr |= ((uint32_t) payload[LORAFRM_BUF_IDX_DEVADDR + 2] << 16);
            rxAddr |= ((uint32_t) payload[LORAFRM_BUF_IDX_DEVADDR + 3] << 24);

            curChildNode = LoRaMesh_FindChildNode(rxAddr);
            if ( curChildNode != NULL ) {
                frameCntr = curChildNode->Connection.UpLinkCounter;
                devAddr = curChildNode->Connection.Address;
                if ( rxAddr != devAddr ) return ERR_FAILED;
            }

            frameDir = UP_LINK;
            break;
        }
        case MSG_TYPE_PROPRIETARY:
            //Intentional falltrough
        default:
            return ERR_INVALID_TYPE;
            break;
    }

    micRx |= ((uint32_t) payload[LORAMAC_BUF_IDX_HDR + payloadSize - LORAMAC_MIC_SIZE]);
    micRx |= ((uint32_t) payload[LORAMAC_BUF_IDX_HDR + payloadSize - LORAMAC_MIC_SIZE + 1] << 8);
    micRx |= ((uint32_t) payload[LORAMAC_BUF_IDX_HDR + payloadSize - LORAMAC_MIC_SIZE + 2] << 16);
    micRx |= ((uint32_t) payload[LORAMAC_BUF_IDX_HDR + payloadSize - LORAMAC_MIC_SIZE + 3] << 24);

    sequenceCntr = (uint16_t) payload[LORAFRM_BUF_IDX_CNTR];
    sequenceCntr |= (uint16_t) payload[LORAFRM_BUF_IDX_CNTR + 1] << 8;
    sequenceCntrDiff = (sequenceCntr - ((uint16_t)(frameCntr & 0xFFFF)));

    if ( sequenceCntrDiff < (1 << 15) )
        frameCntr += sequenceCntrDiff;
    else
        frameCntr += 0x10000 + (int16_t) sequenceCntrDiff;

    LoRaMacComputeMic((uint8_t*) &payload[LORAMAC_BUF_IDX_HDR], payloadSize - LORAMAC_MIC_SIZE,
            pLoRaDevice->upLinkSlot.NwkSKey, devAddr, frameDir, frameCntr, &mic);

    if ( mic == micRx ) {
#if(LORA_DEBUG_OUTPUT_PAYLOAD == 1)
        LOG_TRACE("%s - Size %d", __FUNCTION__, payloadSize - LORAMAC_MIC_SIZE);
        LOG_TRACE_BARE("\t");
        for ( uint8_t i = 0; i < ((payloadSize - LORAMAC_MIC_SIZE) + 2); i++ )
        LOG_TRACE_BARE("0x%02x ", payload[i]);
        LOG_TRACE_BARE("\r\n");
#endif
        return LoRaFrm_OnPacketRx(packet, devAddr, frameDir, frameCntr);
    }

    return ERR_FAILED;
}

uint8_t LoRaMac_PutPayload( uint8_t* buf, size_t bufSize, size_t payloadSize,
        LoRaMacMsgType_t type )
{
    LoRaMacHdr_t macHdr;
    uint8_t flags = LORAPHY_PACKET_FLAGS_NONE;
    uint32_t mic = 0;

    macHdr.Value = 0;

    macHdr.Bits.MType = type;
    macHdr.Bits.Major = LORAMESH_CONFIG_MAJOR_VERSION;

    buf[LORAMAC_BUF_IDX_HDR] = macHdr.Value;
    payloadSize++;

    switch ( macHdr.Bits.MType ) {
        case MSG_TYPE_JOIN_REQ:
            LoRaMacJoinComputeMic(buf, payloadSize & 0xFF, pLoRaDevice->appKey, &mic);

            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = mic & 0xFF;
            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = (mic >> 8) & 0xFF;
            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = (mic >> 16) & 0xFF;
            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = (mic >> 24) & 0xFF;
            break;
        case MSG_TYPE_JOIN_ACCEPT:
            break;
        case MSG_TYPE_DATA_UNCONFIRMED_UP:
        case MSG_TYPE_DATA_CONFIRMED_UP:
        {
            LoRaMacComputeMic(buf, payloadSize, pLoRaDevice->upLinkSlot.NwkSKey,
                    pLoRaDevice->devAddr, UP_LINK, pLoRaDevice->upLinkSlot.DownLinkCounter, &mic);

            if ( (payloadSize + LORAMAC_MIC_SIZE) > LORAMAC_PAYLOAD_SIZE ) {
                return ERR_OVERFLOW;
            }

            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = mic & 0xFF;
            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = (mic >> 8) & 0xFF;
            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = (mic >> 16) & 0xFF;
            *LORAMAC_BUF_MIC_START(buf, payloadSize++) = (mic >> 24) & 0xFF;

            flags |= LORAPHY_PACKET_FLAGS_FRM_REGULAR;
            break;
        }
        case MSG_TYPE_DATA_UNCONFIRMED_DOWN:
        case MSG_TYPE_DATA_CONFIRMED_DOWN:
            LOG_ERROR("Message type data down not yet implemented.");
            break;
        default:
            return ERR_INVALID_TYPE;
    }
#if(LORA_DEBUG_OUTPUT_PAYLOAD == 1)
    LOG_TRACE("%s - Size %d", __FUNCTION__, payloadSize);
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < (payloadSize + 2); i++ )
    LOG_TRACE_BARE("0x%02x ", buf[i]);
    LOG_TRACE_BARE("\r\n");
#endif
    return LoRaPhy_PutPayload(buf, bufSize, payloadSize, flags);
}

void LoRaMac_ProcessCommands( uint8_t *payload, uint8_t macIndex, uint8_t commandsSize )
{
#if 0
    while (macIndex < commandsSize) {
        // Decode Frame MAC commands
        switch (payload[macIndex++]) {
            case SRV_MAC_LINK_CHECK_ANS:
            LoRaMacEventFlags.Bits.LinkCheck = 1;
            LoRaMacEventInfo.DemodMargin = payload[macIndex++];
            LoRaMacEventInfo.NbGateways = payload[macIndex++];
            break;
            case SRV_MAC_LINK_ADR_REQ:
            {
                uint8_t status = 0x07;
                uint16_t chMask;
                int8_t txPower = 0;
                int8_t datarate = 0;
                uint8_t nbRep = 0;
                uint8_t chMaskCntl = 0;
                uint16_t channelsMask[6] = {0, 0, 0, 0, 0, 0};

                // Initialize local copy of the channels mask array
                for ( uint8_t i = 0; i < 6; i++ ) {
                    channelsMask[i] = ChannelsMask[i];
                }
                datarate = payload[macIndex++];
                txPower = datarate & 0x0F;
                datarate = (datarate >> 4) & 0x0F;

                if ( (AdrCtrlOn == false)
                        && ((ChannelsDatarate != datarate) || (ChannelsTxPower != txPower)) ) {   // ADR disabled don't handle ADR requests if server tries to change datarate or txpower
                                                                                                             // Answer the server with fail status
                                                                                                             // Power ACK     = 0
                                                                                                             // Data rate ACK = 0
                                                                                                             // Channel mask  = 0
                    AddMacCommand(MOTE_MAC_LINK_ADR_ANS, 0, 0);
                    macIndex += 3;                                                                           // Skip over the remaining bytes of the request
                    break;
                }
                chMask = (uint16_t) payload[macIndex++];
                chMask |= (uint16_t) payload[macIndex++] << 8;

                nbRep = payload[macIndex++];
                chMaskCntl = (nbRep >> 4) & 0x07;
                nbRep &= 0x0F;
                if ( nbRep == 0 ) {
                    nbRep = 1;
                }
                if ( (chMaskCntl == 0) && (chMask == 0) ) {
                    status &= 0xFE;   // Channel mask KO
                } else if ( (chMaskCntl >= 1) && (chMaskCntl <= 5) ) {
                    // RFU
                    status &= 0xFE;// Channel mask KO
                } else {
                    for ( uint8_t i = 0; i < LORA_MAX_NB_CHANNELS; i++ ) {
                        if ( chMaskCntl == 6 ) {
                            if ( Channels[i].Frequency != 0 ) {
                                chMask |= 1 << i;
                            }
                        } else {
                            if ( ((chMask & (1 << i)) != 0) && (Channels[i].Frequency == 0) ) {   // Trying to enable an undefined channel
                                status &= 0xFE;// Channel mask KO
                            }
                        }
                    }
                    channelsMask[0] = chMask;
                }
                if ( ((datarate < LORAMAC_MIN_DATARATE) || (datarate > LORAMAC_MAX_DATARATE))
                        == true ) {
                    status &= 0xFD;   // Datarate KO
                }

                //
                // Remark MaxTxPower = 0 and MinTxPower = 5
                //
                if ( ((LORAMAC_MAX_TX_POWER <= txPower) && (txPower <= LORAMAC_MIN_TX_POWER))
                        == false ) {
                    status &= 0xFB;   // TxPower KO
                }
                if ( (status & 0x07) == 0x07 ) {
                    ChannelsDatarate = datarate;
                    ChannelsTxPower = txPower;
                    ChannelsMask[0] = channelsMask[0];
                    ChannelsMask[1] = channelsMask[1];
                    ChannelsMask[2] = channelsMask[2];
                    ChannelsMask[3] = channelsMask[3];
                    ChannelsMask[4] = channelsMask[4];
                    ChannelsMask[5] = channelsMask[5];
                    ChannelsNbRep = nbRep;
                }
                AddMacCommand(MOTE_MAC_LINK_ADR_ANS, status, 0);
            }
            break;
            case SRV_MAC_DUTY_CYCLE_REQ:
            MaxDCycle = payload[macIndex++];
            AggregatedDCycle = 1 << MaxDCycle;
            AddMacCommand(MOTE_MAC_DUTY_CYCLE_ANS, 0, 0);
            break;
            case SRV_MAC_RX_PARAM_SETUP_REQ:
            {
                uint8_t status = 0x07;
                int8_t datarate = 0;
                int8_t drOffset = 0;
                uint32_t freq = 0;

                drOffset = (payload[macIndex] >> 4) & 0x07;
                datarate = payload[macIndex] & 0x0F;
                macIndex++;
                freq = (uint32_t) payload[macIndex++];
                freq |= (uint32_t) payload[macIndex++] << 8;
                freq |= (uint32_t) payload[macIndex++] << 16;
                freq *= 100;

                if ( Radio.CheckRfFrequency(freq) == false ) {
                    status &= 0xFE;   // Channel frequency KO
                }

                if ( ((datarate < LORAMAC_MIN_DATARATE) || (datarate > LORAMAC_MAX_DATARATE))
                        == true ) {
                    status &= 0xFD;   // Datarate KO
                }

                if ( ((drOffset < LORAMAC_MIN_RX1_DR_OFFSET)
                                || (drOffset > LORAMAC_MAX_RX1_DR_OFFSET)) == true ) {
                    status &= 0xFB;   // Rx1DrOffset range KO
                }

                if ( (status & 0x07) == 0x07 ) {
                    Rx2Channel.Datarate = datarate;
                    Rx2Channel.Frequency = freq;
                    Rx1DrOffset = drOffset;
                }
                AddMacCommand(MOTE_MAC_RX_PARAM_SETUP_ANS, status, 0);
            }
            break;
            case SRV_MAC_DEV_STATUS_REQ:
            {
                uint8_t batteryLevel = BAT_LEVEL_NO_MEASURE;
                if ( (LoRaMacCallbacks != NULL) && (LoRaMacCallbacks->GetBatteryLevel != NULL) ) {
                    batteryLevel = LoRaMacCallbacks->GetBatteryLevel();
                }
                AddMacCommand(MOTE_MAC_DEV_STATUS_ANS, batteryLevel, LoRaMacEventInfo.RxSnr);
            }
            break;
            case SRV_MAC_NEW_CHANNEL_REQ:
            {
                uint8_t status = 0x03;
                int8_t channelIndex = 0;
                ChannelParams_t chParam;

                channelIndex = payload[macIndex++];
                chParam.Frequency = (uint32_t) payload[macIndex++];
                chParam.Frequency |= (uint32_t) payload[macIndex++] << 8;
                chParam.Frequency |= (uint32_t) payload[macIndex++] << 16;
                chParam.Frequency *= 100;
                chParam.DrRange.Value = payload[macIndex++];

                if ( (channelIndex < 3) || (channelIndex > LORA_MAX_NB_CHANNELS) ) {
                    status &= 0xFE;   // Channel frequency KO
                }

                if ( Radio.CheckRfFrequency(chParam.Frequency) == false ) {
                    status &= 0xFE;   // Channel frequency KO
                }

                if ( (chParam.DrRange.Fields.Min > chParam.DrRange.Fields.Max)
                        || (((LORAMAC_MIN_DATARATE <= chParam.DrRange.Fields.Min)
                                        && (chParam.DrRange.Fields.Min <= LORAMAC_MAX_DATARATE)) == false)
                        || (((LORAMAC_MIN_DATARATE <= chParam.DrRange.Fields.Max)
                                        && (chParam.DrRange.Fields.Max <= LORAMAC_MAX_DATARATE)) == false) ) {
                    status &= 0xFD;   // Datarate range KO
                }
                if ( (status & 0x03) == 0x03 ) {
                    LoRaMacSetChannel(channelIndex, chParam);
                }
                AddMacCommand(MOTE_MAC_NEW_CHANNEL_ANS, status, 0);
            }
            break;
            case SRV_MAC_RX_TIMING_SETUP_REQ:
            {
                uint8_t delay = payload[macIndex++] & 0x0F;

                if ( delay == 0 ) {
                    delay++;
                }
                ReceiveDelay1 = delay * 1e6;
                ReceiveDelay2 = ReceiveDelay1 + 1e6;
                AddMacCommand(MOTE_MAC_RX_TIMING_SETUP_ANS, 0, 0);
            }
            break;
            default:
            // Unknown command. ABORT MAC commands processing
            return;
        }
    }
#endif
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
