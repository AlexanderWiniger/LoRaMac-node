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
#if 0
    LoRaMacHeader_t macHdr;
    uint32_t mic = 0;
    uint32_t micRx = 0;

    if ( LoRaMacDeviceClass != CLASS_C ) {
        Radio.Sleep();
    }
    TimerStop (&RxWindowTimer2);

    /* Check if incoming packet is an advertising beacon */
    if ( (LoRaMacDeviceClass == CLASS_D)
            && (LoRaMacEventFlags.Bits.RxSlot == 0) ) {
        LOG_TRACE("Received advertising beacon.");
        ProcessAdvertisingBeacon(payload, size);
        return;
    }

    macHdr.Value = (uint8_t) * payload;

    switch (macHdr.Bits.MType) {
        case FRAME_TYPE_JOIN_REQ:
        LOG_TRACE("Received join request.");
        break;
        case FRAME_TYPE_JOIN_ACCEPT:
        if ( IsLoRaMacNetworkJoined == true ) {
            break;
        }
        LoRaMacJoinDecrypt(payload + 1, size - 1, LoRaMacAppKey,
                LoRaMacRxPayload + 1);

        LoRaMacRxPayload[0] = macHdr.Value;

        LoRaMacJoinComputeMic(LoRaMacRxPayload, size - LORAMAC_MFR_LEN,
                LoRaMacAppKey, &mic);

        micRx |= (uint32_t) LoRaMacRxPayload[size - LORAMAC_MFR_LEN];
        micRx |= ((uint32_t) LoRaMacRxPayload[size - LORAMAC_MFR_LEN + 1]
                << 8);
        micRx |= ((uint32_t) LoRaMacRxPayload[size - LORAMAC_MFR_LEN + 2]
                << 16);
        micRx |= ((uint32_t) LoRaMacRxPayload[size - LORAMAC_MFR_LEN + 3]
                << 24);

        if ( micRx == mic ) {
            LoRaMacEventFlags.Bits.Rx = 1;
            LoRaMacEventInfo.RxSnr = snr;
            LoRaMacEventInfo.RxRssi = rssi;

            LoRaMacJoinComputeSKeys(LoRaMacAppKey, LoRaMacRxPayload + 1,
                    LoRaMacDevNonce, LoRaMacNwkSKey, LoRaMacAppSKey);

            LoRaMacNetID = (uint32_t) LoRaMacRxPayload[4];
            LoRaMacNetID |= ((uint32_t) LoRaMacRxPayload[5] << 8);
            LoRaMacNetID |= ((uint32_t) LoRaMacRxPayload[6] << 16);

            LoRaMacDevAddr = (uint32_t) LoRaMacRxPayload[7];
            LoRaMacDevAddr |= ((uint32_t) LoRaMacRxPayload[8] << 8);
            LoRaMacDevAddr |= ((uint32_t) LoRaMacRxPayload[9] << 16);
            LoRaMacDevAddr |= ((uint32_t) LoRaMacRxPayload[10] << 24);

            // DLSettings
            Rx1DrOffset = (LoRaMacRxPayload[11] >> 4) & 0x07;
            Rx2Channel.Datarate = LoRaMacRxPayload[11] & 0x0F;
            // RxDelay
            ReceiveDelay1 = (LoRaMacRxPayload[12] & 0x0F);
            if ( ReceiveDelay1 == 0 ) {
                ReceiveDelay1 = 1;
            }
            ReceiveDelay1 *= 1e6;
            ReceiveDelay2 = ReceiveDelay1 + 1e6;

            //CFList
            if ( (size - 1) > 16 ) {
                ChannelParams_t param;
                param.DrRange.Value = (DR_5 << 4) | DR_0;

                for ( uint8_t i = 3, j = 0; i < (5 + 3); i++, j += 3 ) {
                    param.Frequency = ((uint32_t) LoRaMacRxPayload[13 + j]
                            | ((uint32_t) LoRaMacRxPayload[14 + j] << 8)
                            | ((uint32_t) LoRaMacRxPayload[15 + j] << 16))
                    * 100;
                    LoRaMacSetChannel(i, param);
                }
            }

            LoRaMacEventFlags.Bits.JoinAccept = 1;
            IsLoRaMacNetworkJoined = true;
            ChannelsDatarate = ChannelsDefaultDatarate;
            LoRaMacEventInfo.Status = LORAMAC_EVENT_INFO_STATUS_OK;
        } else {
            LoRaMacEventInfo.Status = LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL;
        }

        LoRaMacEventFlags.Bits.Tx = 1;
        break;
        case FRAME_TYPE_DATA_CONFIRMED_DOWN:
        LOG_TRACE_IF(macHdr.Bits.MType == FRAME_TYPE_DATA_CONFIRMED_DOWN,
                "Received confirmed downlink.");
        case FRAME_TYPE_DATA_UNCONFIRMED_DOWN:
        LOG_TRACE_IF(macHdr.Bits.MType == FRAME_TYPE_DATA_UNCONFIRMED_DOWN,
                "Received unconfirmed downlink.");
        case FRAME_TYPE_DATA_CONFIRMED_UP:
        LOG_TRACE_IF(macHdr.Bits.MType == FRAME_TYPE_DATA_CONFIRMED_UP,
                "Received unconfirmed uplink.");
        case FRAME_TYPE_DATA_UNCONFIRMED_UP:
        LOG_TRACE_IF(macHdr.Bits.MType == FRAME_TYPE_DATA_UNCONFIRMED_UP,
                "Received unconfirmed uplink.");
        ProcessDataFrame(payload, size, rssi, snr);
        break;
        case FRAME_TYPE_PROPRIETARY:
        //Intentional falltrough
        default:
        LoRaMacEventFlags.Bits.Tx = 1;
        LoRaMacEventInfo.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
        LoRaMacState &= ~MAC_TX_RUNNING;
        break;
    }

    if ( (LoRaMacDeviceClass == CLASS_C)
            && (LoRaMacEventFlags.Bits.RxSlot == 1) ) {
        OnRxWindow2TimerEvent();
    }
#endif
    return LoRaFrm_OnPacketRx(packet);
}

uint8_t LoRaMac_PutPayload( uint8_t* fBuffer, size_t fBufferSize,
        size_t fPayloadSize, LoRaMessageType_t type )
{
    LoRaMacHeader_t macHdr;
    uint8_t flags = LORAPHY_PACKET_FLAGS_NONE;
    uint32_t mic = 0;

    macHdr.Value = 0;

    macHdr.Bits.MType = type;
    macHdr.Bits.Major = LORAMESH_CONFIG_MAJOR_VERSION;

    fBuffer[LORAMAC_BUF_IDX_HDR] = macHdr.Value;

    switch (macHdr.Bits.MType) {
        case MSG_TYPE_JOIN_REQ:
            LoRaMacJoinComputeMic(fBuffer, fPayloadSize & 0xFF,
                    pLoRaDevice->appKey, &mic);
            break;
        case MSG_TYPE_JOIN_ACCEPT:
            break;
        case MSG_TYPE_DATA_UNCONFIRMED_UP:
        case MSG_TYPE_DATA_CONFIRMED_UP:
        {
            LoRaMacComputeMic(fBuffer, fPayloadSize, pLoRaDevice->nwkSKey,
                    pLoRaDevice->devAddr, UP_LINK, pLoRaDevice->downLinkCounter,
                    &mic);

            if ( (fPayloadSize + LORAMAC_MIC_SIZE) > LORAMAC_PAYLOAD_SIZE ) {
                return ERR_OVERFLOW;
            }

            fBuffer[fPayloadSize++] = mic & 0xFF;
            fBuffer[fPayloadSize++] = (mic >> 8) & 0xFF;
            fBuffer[fPayloadSize++] = (mic >> 16) & 0xFF;
            fBuffer[fPayloadSize++] = (mic >> 24) & 0xFF;

            flags = LORAPHY_PACKET_FLAGS_TX_REGULAR;
            break;
        }
        case MSG_TYPE_DATA_UNCONFIRMED_DOWN:
        case MSG_TYPE_DATA_CONFIRMED_DOWN:
            break;
        default:
            return ERR_INVALID_TYPE;
    }

    return LoRaPhy_PutPayload(fBuffer, fBufferSize, fPayloadSize, flags);
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
