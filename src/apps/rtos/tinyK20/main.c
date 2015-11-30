/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 28.11.2015
 * \brief FreeRTOS test implementation
 *
 */

#include "board.h"

#define LOG_LEVEL_DEBUG
#include "debug.h"

/*------------------------- Local Defines --------------------------------*/
/* task priority */
#define TASK_LED_RTOS_PRIO           7U
/* task stack size */
#define TASK_LED_RTOS_STACK_SIZE     0x200U

/*------------------------ Local Variables -------------------------------*/
static TimerEvent_t Led1Timer;
volatile bool Led1TimerEvent = false;

/*------------------------ Local Functions ------------------------------*/
/* task declare */
static void LedInitTask( void* pvArguments );
/*!
 * \brief Function executed on Led 1 Timeout event
 */
void OnLed1TimerEvent( TimerHandle_t xTimer );

int main( void )
{
    // Target board initialisation
    BoardInitMcu();
    LOG_DEBUG("Mcu initialized.");
    BoardInitPeriph();
    LOG_DEBUG("Peripherals initialized.");

    xTaskCreate(LedInitTask, "LedTask", TASK_LED_RTOS_STACK_SIZE, (void*) NULL, TASK_LED_RTOS_PRIO,
            (xTaskHandle*) NULL);

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
    TimerInit(&Led1Timer, "Led1Timer", 250, OnLed1TimerEvent, false);

    // Switch LED 1 ON
    GpioWrite(&Led1, 0);
    TimerStart(&Led1Timer);

    while (1) {
        if ( Led1TimerEvent == true ) {
            Led1TimerEvent = false;

            // Switch LED 1 OFF
            GpioWrite(&Led1, 1);
            TimerStart(&Led1Timer);
        }

        vTaskDelay(50);
    }
}

void OnLed1TimerEvent( TimerHandle_t xTimer )
{
    LOG_TRACE("%s expired.", pcTimerGetTimerName(xTimer));
    Led1TimerEvent = true;
}
