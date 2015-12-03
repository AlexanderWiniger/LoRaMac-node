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
/*******************************************************************************
 * PUBLIC SETUP FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
