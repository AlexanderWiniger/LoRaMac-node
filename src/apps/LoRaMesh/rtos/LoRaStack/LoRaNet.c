/**
 * \file LoRaNet.c
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
#include "LoRaNet.h"
#include "LoRaMac.h"
#include "LoRaPhy.h"

#define LOG_LEVEL_TRACE
#include "debug.h"
/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
/*! Maximum network layer payload size in absence of FOpts field */
#define LORA_NET_MAXPAYLOAD                      242

/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
/*! Network ID ( 3 bytes ) */
static uint32_t LoRaNetID;

/*! Mote Address */
static uint32_t LoRaNetDevAddr;

/*!
 * LoRaWAN frame counter. Each time a packet is sent the counter is incremented.
 * Only the 16 LSB bits are sent
 */
static uint32_t UpLinkCounter = 1;

/*!
 * LoRaWAN frame counter. Each time a packet is received the counter is incremented.
 * Only the 16 LSB bits are received
 */
static uint32_t DownLinkCounter = 0;

/*! LoRaNet ADR control status */
static bool AdrCtrlOn = false;

/*! Indicates if the node is connected to a private or public network */
static bool IsPublicNetwork = false;

/*! Indicates if the MAC layer has already joined a network. */
static bool IsNetworkJoined = false;

/*! IsPacketCounterFixed enables the MIC field tests by fixing the UpLinkCounter value */
static bool IsUpLinkCounterFixed = false;

/*! Used for test purposes. Disables the opening of the reception windows. */
static bool IsRxWindowsEnabled = true;

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
uint8_t PutPayload( uint8_t* fBuffer, uint16_t fBufferSize, uint8_t payloadSize,
        uint8_t fPort, uint8_t* fOpts, uint8_t fOptsLen,
        LoRaMessageType_t type );

/*******************************************************************************
 * API FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaNet_Init( LoRaNetCallbacks_t *callabcks )
{

}

void LoRaNet_SetAdrOn( bool enable )
{
    AdrCtrlOn = enable;
}

void LoRaNet_InitNwkIds( uint32_t netID, uint32_t devAddr, uint8_t *nwkSKey,
        uint8_t *appSKey )
{
    LoRaNetID = netID;
    LoRaNetDevAddr = devAddr;

    LoRaMac_SetSessionKeys(nwkSKey, appSKey);

    IsNetworkJoined = true;
}

uint8_t LoRaNet_SendFrame( uint8_t *fPayload, uint16_t fPayloadSize,
        uint8_t fPort, bool confirmed )
{
    uint8_t i = 0, buf[LORANET_BUFFER_SIZE];
    LoRaMessageType_t msgType;

    if ( fPayloadSize > LORANET_PAYLOAD_SIZE ) {
        return LORA_ERR_OVERFLOW; /* block too large for payload */
    }

    if ( confirmed ) {
        msgType = MSG_TYPE_DATA_CONFIRMED_UP;
    } else {
        msgType = MSG_TYPE_DATA_UNCONFIRMED_UP;
    }

    while (i < fPayloadSize) {
        LORANET_BUF_PAYLOAD_START(buf, 0)[i] = *fPayload;
        fPayload++;
        i++;
    }

    return PutPayload(buf, sizeof(buf), fPayloadSize, fPort, NULL, 0, msgType);
}

/*******************************************************************************
 * PUBLIC SETUP FUNCTIONS
 ******************************************************************************/
void LoRaNet_SetPublicNetwork( bool enable )
{
    IsPublicNetwork = enable;
}
/*******************************************************************************
 * TEST FUNCTIONS (PUBLIC) (FOR DEBUG PURPOSES ONLY)
 ******************************************************************************/
void LoRaNet_TestSetDutyCycleOn( bool enable )
{
//    DutyCycleOn = enable;
}

void LoRaNet_TestRxWindowsOn( bool enable )
{
//    IsRxWindowsEnabled = enable;
}

void LoRaNet_TestSetMic( uint16_t upLinkCounter )
{
    UpLinkCounter = upLinkCounter;
    IsUpLinkCounterFixed = true;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
uint8_t PutPayload( uint8_t* fBuffer, uint16_t fBufferSize, uint8_t payloadSize,
        uint8_t fPort, uint8_t* fOpts, uint8_t fOptsLen,
        LoRaMessageType_t type )
{
    uint8_t pktHdrSize = 0;
    LoRaFrameCtrl_t fCtrl;

    fCtrl.Value = 0;

    fCtrl.Bits.FOptsLen = fOptsLen;
    fCtrl.Bits.FPending = 0;
    fCtrl.Bits.Ack = false;
    fCtrl.Bits.AdrAckReq = false;
    fCtrl.Bits.Adr = AdrCtrlOn;

    fBuffer[LORANET_BUF_IDX_DEVADDR] = (LoRaNetDevAddr) & 0xFF;
    fBuffer[LORANET_BUF_IDX_DEVADDR + 1] = (LoRaNetDevAddr >> 8) & 0xFF;
    fBuffer[LORANET_BUF_IDX_DEVADDR + 2] = (LoRaNetDevAddr >> 16) & 0xFF;
    fBuffer[LORANET_BUF_IDX_DEVADDR + 3] = (LoRaNetDevAddr >> 24) & 0xFF;

    fBuffer[LORANET_BUF_IDX_CTRL] = fCtrl.Value;

    fBuffer[LORANET_BUF_IDX_CNTR] = UpLinkCounter & 0xFF;
    fBuffer[LORANET_BUF_IDX_CNTR + 1] = (UpLinkCounter >> 8) & 0xFF;

    pktHdrSize += LORANET_HEADER_SIZE_MIN;

    if ( fOptsLen > 0 ) {
        for ( uint8_t i = 0; i < fOptsLen; i++ ) {
            fBuffer[LORANET_BUF_IDX_OPTS + i] = fOpts[i];
        }
        pktHdrSize += fOptsLen;
    }

    if ( payloadSize > 0 ) {
        fBuffer[LORANET_BUF_IDX_PORT(fOptsLen)] = fPort;
    }

    return LoRaMac_PutPayload(fBuffer, fBufferSize, payloadSize, pktHdrSize,
            type);
}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
