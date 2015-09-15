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

/*!
 * Hardware Time base in us
 */
#define HW_TIMER_TIME_BASE                              100 //us 

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
 * Retunr the value of the counter used for a Delay
 */
uint32_t TimerHwGetDelayValue(void);

/*!
 * Increment the value of TimerDelayCounter
 */
void TimerIncrementDelayCounter(void);

void TimerHwInit(void)
{

}

void TimerHwDeInit(void)
{

}

uint32_t TimerHwGetMinimumTimeout(void)
{
    return 0;
}

void TimerHwStart(uint32_t val)
{

}

void TimerHwStop(void)
{

}

void TimerHwDelayMs(uint32_t delay)
{

}

TimerTime_t TimerHwGetElapsedTime(void)
{
    return 0;
}

TimerTime_t TimerHwGetTimerValue(void)
{

}

TimerTime_t TimerHwGetTime(void)
{

    return 0;
}

uint32_t TimerHwGetDelayValue(void)
{
    return 0;
}

void TimerIncrementTickCounter(void)
{

}

void TimerIncrementDelayCounter(void)
{

}

/*!
 * Timer IRQ handler
 */

/*!
 * Timer IRQ handler
 */

void TimerHwEnterLowPowerStopMode(void)
{
#ifndef USE_DEBUGGER
    __WFI();
#endif
}
