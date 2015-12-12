/**
 * \file LoRaTest_App.c
 * \author Alexander Winiger alexander.winiger@hslu.ch
 * \date Dec 11, 2015
 * \version 1.0
 *
 * \brief
 *
 *******************************************************************************
 *  Change log:
 *      [1.0]   Dec 11, 2015      	Alexander Winiger
 *          - created
 *******************************************************************************
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "LoRaMesh.h"
#include "LoRaMacCrypto.h"
#include "LoRaTest_App.h"

#define LOG_LEVEL_TRACE
#include "debug.h"
/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
const uint8_t frmPayload[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0' };
/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
static uint8_t msgBuffer[LORAMESH_BUFFER_SIZE];
static uint8_t phyFlags;
static bool addMessage;
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/* RTOS task function */
static portTASK_FUNCTION(LoRaTestTask, pvParameters);
/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
void LoRaTest_AppInit( void )
{
    /* Initialize msg buffer */
    for ( uint16_t i = 0; i < LORAMESH_BUFFER_SIZE; i++ ) {
        msgBuffer[i] = 0u;
    }

    phyFlags = LORAPHY_PACKET_FLAGS_NONE;
    addMessage = false;

    if ( xTaskCreate(LoRaTestTask, "LoRaTest", configMINIMAL_STACK_SIZE, (void*) NULL,
            tskIDLE_PRIORITY, (xTaskHandle*) NULL) != pdPASS ) {
        /*lint -e527 */
        for ( ;; ) {
        }; /* error! probably out of memory */
        /*lint +e527 */
    }
}

void LoRaTest_AddFrame( void )
{
    LoRaFrmCtrl_t fCtrl;
    LoRaMacHdr_t mHdr;
    uint32_t mic, payloadSize;

    mHdr.Value = 0u;
    mHdr.Bits.MType = MSG_TYPE_DATA_UNCONFIRMED_DOWN;

    fCtrl.Value = 0u;
    fCtrl.Bits.Adr = pLoRaDevice->ctrlFlags.Bits.adrCtrlOn;

    LOG_TRACE("%s - Size %d", __FUNCTION__, sizeof(frmPayload));
    LOG_TRACE_BARE("\t");
    for ( uint8_t i = 0; i < sizeof(frmPayload); i++ )
        LOG_TRACE_BARE("0x%02x ", frmPayload[i]);
    LOG_TRACE_BARE("\r\n");

    /* Encrypt with decrypt */
    LoRaMacPayloadDecrypt(frmPayload, sizeof(frmPayload), pLoRaDevice->upLinkSlot.AppSKey,
            pLoRaDevice->devAddr, DOWN_LINK, pLoRaDevice->upLinkSlot.DownLinkCounter,
            LORAFRM_BUF_PAYLOAD_START_WPORT(msgBuffer));

    msgBuffer[LORAPHY_BUF_IDX_FLAGS] = LORAPHY_PACKET_FLAGS_NONE;
    msgBuffer[LORAMAC_BUF_IDX_HDR] = mHdr.Value;
    msgBuffer[LORAFRM_BUF_IDX_DEVADDR] = (pLoRaDevice->devAddr) & 0xFF;
    msgBuffer[LORAFRM_BUF_IDX_DEVADDR + 1] = (pLoRaDevice->devAddr >> 8) & 0xFF;
    msgBuffer[LORAFRM_BUF_IDX_DEVADDR + 2] = (pLoRaDevice->devAddr >> 16) & 0xFF;
    msgBuffer[LORAFRM_BUF_IDX_DEVADDR + 3] = (pLoRaDevice->devAddr >> 24) & 0xFF;
    msgBuffer[LORAFRM_BUF_IDX_CTRL] = fCtrl.Value;
    msgBuffer[LORAFRM_BUF_IDX_CNTR] = pLoRaDevice->upLinkSlot.DownLinkCounter & 0xFF;
    msgBuffer[LORAFRM_BUF_IDX_CNTR + 1] = (pLoRaDevice->upLinkSlot.DownLinkCounter >> 8) & 0xFF;
    msgBuffer[LORAFRM_BUF_IDX_PORT(0)] = 2;

    payloadSize = sizeof(frmPayload) + LORAFRM_HEADER_SIZE_MIN + LORAFRM_PORT_SIZE
            + LORAMAC_HEADER_SIZE;

    LoRaMacComputeMic((uint8_t*) &msgBuffer[LORAMAC_BUF_IDX_HDR], payloadSize,
            pLoRaDevice->upLinkSlot.NwkSKey, pLoRaDevice->devAddr, DOWN_LINK,
            pLoRaDevice->upLinkSlot.DownLinkCounter, &mic);

    *LORAMAC_BUF_MIC_START(msgBuffer, payloadSize++) = mic & 0xFF;
    *LORAMAC_BUF_MIC_START(msgBuffer, payloadSize++) = (mic >> 8) & 0xFF;
    *LORAMAC_BUF_MIC_START(msgBuffer, payloadSize++) = (mic >> 16) & 0xFF;
    *LORAMAC_BUF_MIC_START(msgBuffer, payloadSize++) = (mic >> 24) & 0xFF;

    msgBuffer[LORAPHY_BUF_IDX_SIZE] = payloadSize;

    addMessage = true;
}
/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
static portTASK_FUNCTION(LoRaTestTask, pvParameters)
{
    (void)pvParameters; /* not used */

    for(;;) {
        if(addMessage) {
            addMessage = false;
            LoRaPhy_QueueRxMessage((uint8_t*)&msgBuffer,
                    LORAPHY_BUF_SIZE(msgBuffer), false, phyFlags);
        }
        vTaskDelay(1000/portTICK_RATE_MS);
    } /* for */
}
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
