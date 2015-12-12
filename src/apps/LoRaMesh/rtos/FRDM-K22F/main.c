/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 12.11.2015
 * \brief LoRaMesh implementation
 *
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include <string.h>
#include <math.h>
#include "board.h"

#include "LoRaMesh_App.h"
#include "LoRaMacCrypto.h"

#define LOG_LEVEL_TRACE
#include "debug.h"

/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE MACRO DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
/*! AES encryption/decryption cipher network session key */
static uint8_t NwkSKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88,
        0x09, 0xCF, 0x4F, 0x3C };
/*! AES encryption/decryption cipher application session key */
static uint8_t AppSKey[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88,
        0x09, 0xCF, 0x4F, 0x3C };
/*! Device address */
static uint32_t devAddr = 0x0132C9AF;
/*! Frame direction */
static uint8_t fDir = 0x01u;
/*! Sequence counter */
static uint32_t fCnt = 0x01;
/*! App payload */
static uint8_t appPayload[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0' };
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
/**
 * Main application entry point.
 */
int main( void )
{
    uint8_t appPayloadEncrypted[sizeof(appPayload)];
    uint8_t appPayloadDecrypted[sizeof(appPayload)];

    for ( uint8_t i = 0; i < sizeof(appPayload); i++ ) {
        appPayloadEncrypted[i] = 0x00u;
        appPayloadDecrypted[i] = 0x00u;
    }

    BoardInitMcu();
    LOG_DEBUG("Mcu initialized.");
    OSA_Init();
    LOG_DEBUG("OS initialized.");
    BoardInitPeriph();
    LOG_DEBUG("Peripherals initialized.");

#if 0
    /* void LoRaMacPayloadEncrypt( const uint8_t *buffer, uint16_t size, const uint8_t *key,
     uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *encBuffer ) */
    LoRaMacPayloadEncrypt((uint8_t*) &appPayload, sizeof(appPayload), (uint8_t*) &NwkSKey, devAddr,
            fDir, fCnt, (uint8_t*) &appPayloadEncrypted);

    /* void LoRaMacPayloadDecrypt( const uint8_t *buffer, uint16_t size, const uint8_t *key,
     uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *decBuffer ) */
    LoRaMacPayloadEncrypt((uint8_t*) &appPayloadEncrypted, sizeof(appPayload), (uint8_t*) &NwkSKey,
            devAddr, fDir, fCnt, (uint8_t*) &appPayloadDecrypted);
#else
    LoRaMesh_AppInit();

    vTaskStartScheduler();
#endif

    for ( ;; ) {
        /* Should not be reached */
    }
}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
