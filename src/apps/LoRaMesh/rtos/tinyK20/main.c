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

#include "Shell_App.h"
#include "LoRaMesh_AppConfig.h"
#include "LoRaMesh_App.h"

#define LOG_LEVEL_TRACE
#include "debug.h"

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
static uint32_t heartBeatCntr;
static bool heartBeatLedOn;

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
/*! Main application entry point. */
int main( void )
{
    BoardInitMcu();
    LOG_DEBUG("Mcu initialized.");
    BoardInitPeriph();
    LOG_DEBUG("Peripherals initialized.");
#if(LORAMESH_TEST_APP_ACTIVATED == 1)
    if ( xTaskCreate(LedTask, "Led", configMINIMAL_STACK_SIZE, (void*) NULL, tskIDLE_PRIORITY,
                    (xTaskHandle*) NULL) != pdPASS ) {
        /*lint -e527 */
        for (;; ) {
        }; /* error! probably out of memory */
        /*lint +e527 */
    }
#endif

#if defined( USE_SHELL )
    Shell_AppInit ();
#endif /* USE_SHELL */

    LoRaMesh_AppInit();

    /* Reset heartBeatCntr */
    heartBeatCntr = 0;
    heartBeatLedOn = false;

    vTaskStartScheduler();

    LOG_ERROR("Failed to create idle task. Probably out of memory.");

    for ( ;; ) {
        /* Should not be reached */
    }
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
void vApplicationIdleHook( void )
{
    if ( (heartBeatCntr++ % 100000) == 0 ) {
        if ( heartBeatLedOn ) GpioWrite(&Led1, 1);
        else GpioWrite(&Led1, 0);
        heartBeatLedOn = !heartBeatLedOn;
    }
}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
