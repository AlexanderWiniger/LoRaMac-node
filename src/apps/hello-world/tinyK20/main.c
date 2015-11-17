/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 17.11.2015
 * \brief Hello World application implementation
 *
 */

#include "board.h"
#include "uart.h"

#define PRINTF

/*------------------------- Local Defines --------------------------------*/

/*------------------------ Local Variables -------------------------------*/
static TimerEvent_t Led1Timer;
volatile bool Led1TimerEvent = false;

/*!
 * \brief Switch A IRQ callback
 */
void SwitchAIrq(void);

/*!
 * \brief Function executed on Led 1 Timeout event
 */
void OnLed1TimerEvent(void)
{
    Led1TimerEvent = true;
}

/*!
 * \brief Main application entry point.
 */
int main(void)
{
    // Target board initialisation
    BoardInitMcu();
    PRINTF("TRACE: Mcu initialized.\r\n");
    BoardInitPeriph();
    PRINTF("TRACE: Peripherals initialized.\r\n");

    TimerInit(&Led1Timer, OnLed1TimerEvent);
    TimerSetValue(&Led1Timer, 250000);

    // Switch LED 1 ON
    GpioWrite(&Led1, 0);
    TimerStart(&Led1Timer);

    // Print the initial banner
    PRINTF("\r\nHello World!\r\n\r\n");

    while (1) {
        if (Led1TimerEvent == true) {
            Led1TimerEvent = false;

            // Switch LED 1 OFF
            GpioWrite(&Led1, 1);
            TimerStart(&Led1Timer);
        }
    }
}
