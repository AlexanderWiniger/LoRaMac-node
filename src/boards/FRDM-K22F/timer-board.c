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
#include "fsl_interrupt_manager.h"

/*------------------------- Local Defines --------------------------------*/
/*!
 * Hardware Timer device
 */
#define HWTIMER_LL_DEVIF        kSystickDevif

/*!
 * Hardware Timer device ID
 */
#define HWTIMER_LL_ID           0
/*!
 * Hardware Timer time base in us
 */
#define HW_TIMER_TIME_BASE                              100 //us 

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

/*------------------------ Local Functions -------------------------------*/
/*!
 * Return the value of the counter used for a Delay
 */
uint32_t TimerHwGetDelayValue(void);

/*!
 * Increment the value of TimerDelayCounter
 */
void TimerIncrementDelayCounter(void);

void TimerHwInit(void)
{

    /*!
     * TPM0 initialization
     * \remark Only used for delay
     */

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
    TimerTime_t val = 0;

    INT_SYS_DisableIRQGlobal();

    val = TimerTickCounter;

    INT_SYS_EnableIRQGlobal();

    return (val);
}

TimerTime_t TimerHwGetTime(void)
{

    return TimerHwGetTimerValue() * HW_TIMER_TIME_BASE;
}

uint32_t TimerHwGetDelayValue(void)
{
    uint32_t val = 0;

    INT_SYS_DisableIRQGlobal();

    val = TimerDelayCounter;

    INT_SYS_EnableIRQGlobal();

    return (val);
}

void TimerIncrementTickCounter(void)
{
    INT_SYS_DisableIRQGlobal();

    TimerTickCounter++;

    INT_SYS_EnableIRQGlobal();
}

void TimerIncrementDelayCounter(void)
{
    INT_SYS_DisableIRQGlobal();

    TimerDelayCounter++;

    INT_SYS_EnableIRQGlobal();
}

void TimerHwEnterLowPowerStopMode(void)
{
#ifndef USE_DEBUGGER
    __WFI();
#endif
}
