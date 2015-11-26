/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 17.09.2015
 * \brief Hello World application implementation
 *
 */

#include "board.h"
#include "uart.h"

#define LOG_LEVEL_DEBUG
#include "debug.h"

/*------------------------- Local Defines --------------------------------*/

/*------------------------ Local Variables -------------------------------*/
static TimerEvent_t Led1Timer;
volatile bool Led1TimerEvent = false;

static TimerEvent_t Led2Timer;
volatile bool Led2TimerEvent = false;

static TimerEvent_t Led3Timer;
volatile bool Led3TimerEvent = false;

bool SwitchAPushEvent = false;
bool SwitchBPushEvent = false;

/*!
 * \brief Switch A IRQ callback
 */
void SwitchAIrq(void);

/*!
 * \brief Switch B IRQ callback
 */
void SwitchBIrq(void);

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
    LOG_DEBUG("Mcu initialized.");
    BoardInitPeriph();
    LOG_DEBUG("Peripherals initialized.");

    /* Switch A & B */
    GpioSetInterrupt(&SwitchA, IRQ_FALLING_EDGE, IRQ_LOW_PRIORITY, SwitchAIrq);
    GpioSetInterrupt(&SwitchB, IRQ_FALLING_EDGE, IRQ_LOW_PRIORITY, SwitchBIrq);

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
    LOG_DEBUG("\r\nHello World!\r\n");

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

        if (SwitchAPushEvent) {
            accel_sensor_data_t sensorData;
            SwitchAPushEvent = false;

            if (FxosReadSensorData(&sensorData) != FAIL) {
                LOG_DEBUG_BARE("DATA: Accelerometer (X/Y/Z):\t%d \t%d \t%d \r\n", sensorData.accelX,
                        sensorData.accelY, sensorData.accelZ);
//                LOG_DEBUG_BARE("DATA: Magnetometer (X/Y/Z):\t%d \t%d \t%d \r\n", sensorData.magX, sensorData.magY,
//                        sensorData.magZ);
            } else {
                LOG_ERROR("Couldn't retrieve sensor data!");
            }
        }

        if (SwitchBPushEvent) {
            SwitchBPushEvent = false;
            LOG_TRACE("Button B pushed!");
        }
    }
}

void SwitchAIrq(void)
{
    DelayMs(20);    // Software debouncing
    SwitchAPushEvent = true;
}

void SwitchBIrq(void)
{
    DelayMs(20);    // Software debouncing
    SwitchBPushEvent = true;
}
