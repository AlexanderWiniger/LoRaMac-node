/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 28.11.2015
 * \brief FreeRTOS test implementation
 *
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "board.h"

#define LOG_LEVEL_DEBUG
#include "debug.h"

/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
/* led task priority */
#define TASK_LED_RTOS_PRIO           7U
/* led task stack size */
#define TASK_LED_RTOS_STACK_SIZE     0x200U
/* gps task priority */
#define TASK_GPS_RTOS_PRIO           5U
/* gps task stack size */
#define TASK_GPS_RTOS_STACK_SIZE     0x200U

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
static bool IsLedActive = true;
/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/* led task declare */
static void LedInitTask( void* pvArguments );

/* gps task declare */
//static void GpsInitTask( void* pvArguments );
/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
int main( void )
{
    // Target board initialisation
    BoardInitMcu();
    LOG_DEBUG("Mcu initialized.");
    BoardInitPeriph();
    LOG_DEBUG("Peripherals initialized.");

    xTaskCreate(LedInitTask, "LedTask", TASK_LED_RTOS_STACK_SIZE, (void*) NULL,
    TASK_LED_RTOS_PRIO, (xTaskHandle*) NULL);

//    xTaskCreate(GpsInitTask, "GpsTask", TASK_GPS_RTOS_STACK_SIZE, (void*) NULL,
//            TASK_GPS_RTOS_PRIO, (xTaskHandle*) NULL);

// Print the initial banner
    LOG_DEBUG("Hello World!\r\n");

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    for ( ;; ) {
    }                    // Should not achieve here
}

/*!
 * task to blink led rtos between 1 seconds.
 */
void LedInitTask( void* pvArguments )
{
    // Switch LED 1 ON
    GpioWrite(&Led1, 0);

    while (1) {
        if ( IsLedActive ) {
            // Switch LED 1 OFF
            GpioWrite(&Led1, 1);
        } else {
            // Switch LED 1 ON
            GpioWrite(&Led1, 0);
        }

        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

/*!
 * task to blink led rtos between 1 seconds.
 */
void GpsInitTask( void* pvArguments )
{
    while (1) {
        LOG_DEBUG("GPS");
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
