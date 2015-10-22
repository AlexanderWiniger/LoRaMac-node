/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 17.09.2015
 * \brief Hello World application implementation
 *
 */

#include "board.h"
#include "uart.h"

/*------------------------- Local Defines --------------------------------*/

/*------------------------ Local Variables -------------------------------*/
static TimerEvent_t Led1Timer;
volatile bool Led1TimerEvent = false;

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
    // LED state
    bool Led1On;

    // Target board initialisation
    BoardInitMcu();
    BoardInitPeriph();

    TimerInit(&Led1Timer, OnLed1TimerEvent);
    TimerSetValue(&Led1Timer, 500000);

    // Switch LED 1 ON
    GpioWrite(&Led1, 0);
    Led1On = true;
    TimerStart(&Led1Timer);

    // Print the initial banner
    PRINTF("\r\nHello World!\r\n\r\n");

    while (1) {
        if (Led1TimerEvent == true) {
            Led1TimerEvent = false;

            if (Led1On) GpioWrite(&Led1, 1);
            else GpioWrite(&Led1, 0);
            Led1On = !Led1On;
            TimerStart(&Led1Timer);
        }
    }
}

