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
static uint32_t LoRaDevAddr;

/*! LoRaNet ADR control status */
static bool AdrCtrlOn = false;

/*! Indicates if the node is connected to a private or public network */
static bool PublicNetwork = false;

/*! Indicates if the MAC layer has already joined a network. */
static bool IsNetworkJoined = false;

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
uint8_t PutPayload( void *fBuffer, uint16_t fBufferSize, uint8_t fPort, LoRaMessageType_t type,
        uint8_t nofRetries );

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

void LoRaNet_InitNwkIds( uint32_t netID, uint32_t devAddr, uint8_t *nwkSKey, uint8_t *appSKey )
{
    LoRaNetID = netID;
    LoRaDevAddr = devAddr;

    LoRaMac_SetSessionKeys(nwkSKey, appSKey);

    IsNetworkJoined = true;
}

uint8_t LoRaMacSendFrame( void *fPayload, uint16_t fPayloadSize, uint8_t fPort, bool confirmed,
        uint8_t nofRetries )
{
    uint8_t buf[LORANET_BUFFER_SIZE];

    int i;

    if ( fBufferSize > LORANET_PAYLOAD_SIZE ) {
        return ERR_OVERFLOW; /* block too large for payload */
    }

    i = 0;
    while (i < appPayloadSize) {
        RAPP_BUF_PAYLOAD_START(buf)[i] = *fPayload;
        fPayload++;
        i++;
    }
    return PutPayload(buf, sizeof(buf), fPayloadSize, fPort, confirmed, nofRetries);
}

/*******************************************************************************
 * PUBLIC SETUP FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
uint8_t PutPayload( void *buf, uint16_t bufSize, uint8_t payloadSize, uint8_t fPort,
        LoRaMessageType_t type )
{

}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
