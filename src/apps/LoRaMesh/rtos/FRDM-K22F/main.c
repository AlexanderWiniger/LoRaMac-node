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
void main( void )
{
#if 1
    ForwardListNode_t* forwardList = forward_list_create((void*) 0);

    forward_list_push_front(forwardList, (void*) 1);
    forward_list_push_front(forwardList, (void*) 2);
    forward_list_push_front(forwardList, (void*) 3);
    forward_list_push_front(forwardList, (void*) 4);

#else
    BoardInitMcu();
    LOG_DEBUG("Mcu initialized.");
    OSA_Init();
    LOG_DEBUG("OS initialized.");
    BoardInitPeriph();
    LOG_DEBUG("Peripherals initialized.");

    LoRaMesh_AppInit();

    vTaskStartScheduler();

    for (;; ) {
        /* Should not be reached */
    }
#endif
}

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
