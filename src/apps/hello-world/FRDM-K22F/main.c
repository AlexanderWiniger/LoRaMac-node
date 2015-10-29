/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 17.09.2015
 * \brief Hello World application implementation
 *
 */

#include "board.h"
#include "uart.h"

#include "fsl_port_hal.h"   /* \todo Debug purpose only */
#include "fsl_gpio_driver.h"   /* \todo Debug purpose only */

/*------------------------- Local Defines --------------------------------*/

/*------------------------ Local Variables -------------------------------*/
/* Declare Output GPIO pins */
gpio_output_pin_user_config_t dbgPin = {
    .pinName = GPIO_MAKE_PIN(GPIOD_IDX, 2),
    .config.outputLogic = 1,
    .config.slewRate = kPortSlowSlewRate,
    .config.driveStrength = kPortLowDriveStrength,
}; /* \todo Debug purpose only */

static TimerEvent_t Led1Timer;
volatile bool Led1TimerEvent = false;

static TimerEvent_t Led2Timer;
volatile bool Led2TimerEvent = false;

static TimerEvent_t Led3Timer;
volatile bool Led3TimerEvent = false;

/*!
 * \brief Function executed on Led 1 Timeout event
 */
void OnLed1TimerEvent(void)
{
    Led1TimerEvent = true;
}

/*!
 * \brief Function executed on Led 2 Timeout event
 */
void OnLed2TimerEvent(void)
{
    Led2TimerEvent = true;
}

/*!
 * \brief Function executed on Led 3 Timeout event
 */
void OnLed3TimerEvent(void)
{
    Led3TimerEvent = true;
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

    PORT_HAL_SetMuxMode(PORTD, 2u, kPortMuxAsGpio); /* \todo Debug purpose only */
    GPIO_DRV_OutputPinInit (&dbgPin); /* \todo Debug purpose only */

    TimerInit(&Led1Timer, OnLed1TimerEvent);
    TimerSetValue(&Led1Timer, 250000);

    TimerInit(&Led2Timer, OnLed2TimerEvent);
    TimerSetValue(&Led2Timer, 250000);

    TimerInit(&Led3Timer, OnLed3TimerEvent);
    TimerSetValue(&Led3Timer, 250000);

    // Switch LED 1 ON
    GpioWrite(&Led1, 0);
    TimerStart(&Led1Timer);

    // Print the initial banner
    PRINTF("\r\nHello World!\r\n\r\n");

#if 1
    for (;;) {
        GPIO_DRV_TogglePinOutput(dbgPin.pinName); /* \todo Debug purpose only */
        DelayMs(10);
    }
#else
    while (1) {
        if (Led1TimerEvent == true) {
            Led1TimerEvent = false;

            // Switch LED 1 OFF
            GpioWrite(&Led1, 1);
            // Switch LED 2 ON
            GpioWrite(&Led2, 0);
            TimerStart(&Led2Timer);
        }

        if (Led2TimerEvent == true) {
            Led2TimerEvent = false;

            // Switch LED 2 OFF
            GpioWrite(&Led2, 1);
            // Switch LED 3 ON
            GpioWrite(&Led3, 0);
            TimerStart(&Led3Timer);
        }

        if (Led3TimerEvent == true) {
            Led3TimerEvent = false;

            // Switch LED 3 OFF
            GpioWrite(&Led3, 1);
            // Switch LED 1 ON
            GpioWrite(&Led1, 0);
            TimerStart(&Led1Timer);
        }
    }
#endif
}

