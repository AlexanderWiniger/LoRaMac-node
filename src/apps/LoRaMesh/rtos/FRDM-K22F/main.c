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
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
/*! Main application entry point. */
int main( void )
{
    BoardInitMcu();
    LOG_DEBUG("Mcu initialized.");
    OSA_Init();
    LOG_DEBUG("OS initialized.");
    BoardInitPeriph();
    LOG_DEBUG("Peripherals initialized.");

    LoRaMesh_AppInit();

    vTaskStartScheduler();

    for ( ;; ) {
        /* Should not be reached */
    }
}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
