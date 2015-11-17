/**
 * \file timer-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board specific timer functions implementation
 *
 */

#include <math.h>
#include "board.h"
#include "timer-board.h"

/*------------------------- Local Defines --------------------------------*/
#define HWTIMER_PERIOD                                  100 //us
/*------------------------ Local Variables -------------------------------*/

/*!
 * Hardware Timer tick counter
 */
volatile TimerTime_t TimerTickCounter = 1;

/*!
 * Saved value of the Tick counter at the start of the next event
 */
static TimerTime_t TimerTickCounterContext = 0;

/*!
 * Value trigging the IRQ
 */
volatile TimerTime_t TimeoutCntValue = 0;

/*!
 * Increment the Hardware Timer tick counter
 */
void TimerIncrementTickCounter(void);

/*!
 * Counter used for the Delay operations
 */
volatile uint32_t TimerDelayCounter = 0;

/*!
 * @brief Hardware timer callback function
 */
void hwtimer_callback(void* data);

extern void HWTIMER_SYS_SystickIsrAction(void);

void TimerHwInit(void)
{

}

void TimerHwDeInit(void)
{

}

uint32_t TimerHwGetMinimumTimeout(void)
{
    return (ceil(2 * HWTIMER_PERIOD));
}

void TimerHwStart(uint32_t val)
{
    TimerTickCounterContext = TimerHwGetTimerValue();

    if (val <= HWTIMER_PERIOD + 1) {
        TimeoutCntValue = TimerTickCounterContext + 1;
    } else {
        TimeoutCntValue = TimerTickCounterContext + ((val - 1) / HWTIMER_PERIOD);
    }
}

void TimerHwStop(void)
{

}

void TimerHwDelayMs(uint32_t delay)
{

}

TimerTime_t TimerHwGetElapsedTime(void)
{
    return (((TimerHwGetTimerValue() - TimerTickCounterContext) + 1) * HWTIMER_PERIOD);
}

TimerTime_t TimerHwGetTimerValue(void)
{
    TimerTime_t val = 0;

    return (val);
}

TimerTime_t TimerHwGetTime(void)
{

    return TimerHwGetTimerValue() * HWTIMER_PERIOD;
}

void TimerIncrementTickCounter(void)
{

}

void hwtimer_callback(void* data)
{
    TimerIncrementTickCounter();

    if (TimerTickCounter == TimeoutCntValue) {
        TimerIrqHandler();
    }
}
/*!
 * @brief Interrupt service routine.
 */
void SysTick_Handler(void)
{

}

void TimerHwEnterLowPowerStopMode(void)
{
#ifndef USE_DEBUGGER
    __WFI();
#endif
}
