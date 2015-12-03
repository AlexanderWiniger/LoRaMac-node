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

#include "LoRaNet.h"
#include "LoRaPhy.h"

#define LOG_LEVEL_TRACE
#include "debug.h"

/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
/*! Defines the application data transmission duty cycle */
#define APP_TX_DUTYCYCLE              1000000  // 5 [s] value in us
/* task priority */
#define TASK_MESH_RTOS_PRIO           7U
/* task stack size */
#define TASK_MESH_RTOS_STACK_SIZE     0x200U

/*******************************************************************************
 * PRIVATE MACRO DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*!
 * task declare
 */
void task_mesh_rtos( task_param_t param );

/*
 * task define
 */
OSA_TASK_DEFINE(task_mesh_rtos, TASK_MESH_RTOS_STACK_SIZE);

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
/**
 * Main application entry point.
 */
int main( void )
{
    static osa_status_t result = kStatus_OSA_Error;

    BoardInitMcu();
    LOG_DEBUG("Mcu initialized.");
    OSA_Init();
    LOG_DEBUG("OS initialized.");
    BoardInitPeriph();
    LOG_DEBUG("Peripherals initialized.");

    LoRaNet_Init();

    result = OSA_TaskCreate(task_mesh_rtos, (uint8_t *) "led_rtos", TASK_MESH_RTOS_STACK_SIZE,
            task_mesh_rtos_stack, TASK_MESH_RTOS_PRIO, (task_param_t) 0, false,
            &task_mesh_rtos_task_handler);
    if ( result != kStatus_OSA_Success ) {
        LOG_ERROR("Failed to create led_rtos task");
    }

    LOG_DEBUG("Starting LoRa Mesh application...");

    OSA_Start();

    for ( ;; ) {

    }
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
void task_mesh_rtos( task_param_t param )
{
#if( OVER_THE_AIR_ACTIVATION != 0 )
    uint8_t sendFrameStatus = 0;
#endif

    LOG_TRACE("Starting mesh app task...");

    while ( 1 ) {
        LOG_TRACE("Trying to send frame...");
//        PrepareTxFrame (AppPort);
//        SendFrame();

        TimerLowPowerHandler();
        OSA_TimeDelay(APP_TX_DUTYCYCLE);
    }
}
