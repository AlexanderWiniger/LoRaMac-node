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
#include "LoRaMac.h"

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
/*! AES encryption/decryption cipher network session key */
static uint8_t LoRaMacNwkSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*! AES encryption/decryption cipher application session key */
static uint8_t LoRaMacAppSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * API FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaMac_Init( void )
{

}

uint8_t LoRaMac_PutPayload( uint8_t* fBuffer, uint8_t fBufferSize,
        uint8_t payloadSize, uint8_t pktHdrSize, LoRaMessageType_t type )
{
    LoRaMacHeader_t macHdr;
    uint32_t mic = 0;

    if ( fBuffer == NULL ) {
        fBufferSize = 0;
    } else {
        if ( ValidatePayloadLength(fBufferSize, ChannelsDatarate) == false ) {
            return 3;
        }
    }

    macHdr.Value = 0;

    macHdr.Bits.MType = type;
    macHdr.Bits.Major = LORAMESH_CONFIG_MAJOR_VERSION;

    fBuffer[LORAMAC_BUF_IDX_HDR] = macHdr.Value;

    switch (macHdr->Bits.MType) {
        case MSG_TYPE_JOIN_REQ:
            RxWindow1Delay = JoinAcceptDelay1 - RADIO_WAKEUP_TIME;
            RxWindow2Delay = JoinAcceptDelay2 - RADIO_WAKEUP_TIME;

            LoRaMacBufferPktLen = pktHeaderLen;

            LoRaMacMemCpy(LoRaMacAppEui, LoRaMacBuffer + LoRaMacBufferPktLen,
                    8);
            LoRaMacBufferPktLen += 8;
            LoRaMacMemCpy(LoRaMacDevEui, LoRaMacBuffer + LoRaMacBufferPktLen,
                    8);
            LoRaMacBufferPktLen += 8;

            LoRaMacDevNonce = Radio.Random();

            LoRaMacBuffer[LoRaMacBufferPktLen++] = LoRaMacDevNonce & 0xFF;
            LoRaMacBuffer[LoRaMacBufferPktLen++] = (LoRaMacDevNonce >> 8)
                    & 0xFF;

            LoRaMacJoinComputeMic(LoRaMacBuffer, LoRaMacBufferPktLen & 0xFF,
                    LoRaMacAppKey, &mic);

            LoRaMacBuffer[LoRaMacBufferPktLen++] = mic & 0xFF;
            LoRaMacBuffer[LoRaMacBufferPktLen++] = (mic >> 8) & 0xFF;
            LoRaMacBuffer[LoRaMacBufferPktLen++] = (mic >> 16) & 0xFF;
            LoRaMacBuffer[LoRaMacBufferPktLen++] = (mic >> 24) & 0xFF;
            break;
        case MSG_TYPE_JOIN_ACCEPT:
            break;
        case MSG_TYPE_DATA_UNCONFIRMED_UP:
            break;
        case MSG_TYPE_DATA_UNCONFIRMED_DOWN:
            break;
        case MSG_TYPE_DATA_CONFIRMED_UP:
            break;
        case MSG_TYPE_DATA_CONFIRMED_DOWN:
            break;
        default:
            return 4;
    }

    return LoRaPhy_PutPayload(fBuffer, fBufferSize, payloadSize);
}

void LoRaMac_SetSessionKeys( uint8_t *nwkSKey, uint8_t *appSKey )
{
    LoRaMacMemCpy(nwkSKey, LoRaMacNwkSKey, 16);
    LoRaMacMemCpy(appSKey, LoRaMacAppSKey, 16);
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
