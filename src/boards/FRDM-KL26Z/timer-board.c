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
#include "fsl_pit_hal.h"
#include "fsl_interrupt_manager.h"
#include "fsl_clock_manager.h"

/*------------------------- Local Defines --------------------------------*/
/*!
 * Hardware Timer time base in us
 */
#define HW_TIMER_TIME_BASE                              100 //us 

/*------------------------ Local Variables -------------------------------*/
/* Table of base addresses for pit instances. */
PIT_Type * const g_pitBase[] = PIT_BASE_PTRS;

/* Table to save PIT IRQ enum numbers defined in CMSIS files. */
const IRQn_Type g_pitIrqId[] = PIT_IRQS;

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

/*------------------------ Local Functions -------------------------------*/
void TimerHwInit(void)
{
    uint64_t pitSourceClock;
    uint32_t count;

    /*!
     * PIT initialization
     */
    /* Un-gate pit clock*/
    CLOCK_SYS_EnablePitClock(0);

    /* Enable PIT module clock*/
    PIT_HAL_Enable (PIT_BASE_PTR);

    /* Set timer run or stop in debug mode*/
    PIT_HAL_SetTimerRunInDebugCmd(PIT_BASE_PTR, true);

    /* Finally, update pit source clock frequency.*/
    pitSourceClock = CLOCK_SYS_GetPitFreq(0);

    /*!
     * PIT timer HWTIMER_TIMER_CHANNEL initialization
     */
    /* Calculate the count value, assign it to timer counter register.*/
    count = (uint32_t)(HW_TIMER_TIME_BASE * pitSourceClock / 1000000U - 1U);

    /* Set timer period.*/
    PIT_HAL_SetTimerPeriodByCount(PIT_BASE_PTR, HWTIMER_TIMER_CHANNEL, count);

    /* Enable or disable interrupt.*/
    PIT_HAL_SetIntCmd(PIT_BASE_PTR, HWTIMER_TIMER_CHANNEL, true);

    /* Enable PIT interrupt.*/
    INT_SYS_EnableIRQ(g_pitIrqId[HWTIMER_TIMER_CHANNEL]);

    /* Start timer HWTIMER_TIMER_CHANNEL */
    PIT_HAL_StartTimer(PIT_BASE_PTR, HWTIMER_TIMER_CHANNEL);

    /*!
     * PIT timer HWTIMER_DELAY_CHANNEL initialization
     */
    /* Calculate the count value, assign it to timer counter register.*/
    count = (uint32_t)(10 * pitSourceClock / 1000000U - 1U);

    /* Set timer period.*/
    PIT_HAL_SetTimerPeriodByCount(PIT_BASE_PTR, HWTIMER_DELAY_CHANNEL, count);

    /* Enable PIT interrupt.*/
    INT_SYS_EnableIRQ(g_pitIrqId[HWTIMER_DELAY_CHANNEL]);
}

void TimerHwDeInit(void)
{
    /* Disable PIT timer HWTIMER_TIMER_CHANNEL interrupt. Clear the chain bit if available */
    PIT_HAL_SetIntCmd(PIT_BASE_PTR, HWTIMER_TIMER_CHANNEL, false);
    INT_SYS_DisableIRQ(g_pitIrqId[HWTIMER_TIMER_CHANNEL]);
#if FSL_FEATURE_PIT_HAS_CHAIN_MODE
    PIT_HAL_SetTimerChainCmd(PIT_BASE_PTR, HWTIMER_TIMER_CHANNEL, false);
#endif

    /* Disable PIT timer 1 interrupt. Clear the chain bit if available */
    PIT_HAL_SetIntCmd(PIT_BASE_PTR, HWTIMER_DELAY_CHANNEL, false);
    INT_SYS_DisableIRQ(g_pitIrqId[HWTIMER_DELAY_CHANNEL]);
#if FSL_FEATURE_PIT_HAS_CHAIN_MODE
    PIT_HAL_SetTimerChainCmd(PIT_BASE_PTR, HWTIMER_DELAY_CHANNEL, false);
#endif

    /* Disable PIT module clock*/
    PIT_HAL_Disable (PIT_BASE_PTR);

    /* Gate PIT clock control*/
    CLOCK_SYS_DisablePitClock(0);
}

uint32_t TimerHwGetMinimumTimeout(void)
{
    return (ceil(2 * HW_TIMER_TIME_BASE));
}

void TimerHwStart(uint32_t val)
{
    TimerTickCounterContext = TimerHwGetTimerValue();

    if (val <= HW_TIMER_TIME_BASE + 1) {
        TimeoutCntValue = TimerTickCounterContext + 1;
    } else {
        TimeoutCntValue = TimerTickCounterContext + ((val - 1) / HW_TIMER_TIME_BASE);
    }
}

void TimerHwStop(void)
{
    PIT_HAL_SetIntCmd(PIT_BASE_PTR, HWTIMER_TIMER_CHANNEL, false);
    PIT_HAL_StopTimer(PIT_BASE_PTR, HWTIMER_TIMER_CHANNEL);
}

void TimerHwDelayMs(uint32_t delay)
{
    uint32_t delayValue = 0;

    delayValue = delay;

    TimerDelayCounter = 0;

    PIT_HAL_SetIntCmd(PIT_BASE_PTR, HWTIMER_DELAY_CHANNEL, true);
    PIT_HAL_StartTimer(PIT_BASE_PTR, HWTIMER_DELAY_CHANNEL);

    while (TimerHwGetDelayValue() < delayValue) {
    }

    PIT_HAL_SetIntCmd(PIT_BASE_PTR, HWTIMER_DELAY_CHANNEL, false);
    PIT_HAL_StopTimer(PIT_BASE_PTR, HWTIMER_DELAY_CHANNEL);
}

TimerTime_t TimerHwGetElapsedTime(void)
{
    return (((TimerHwGetTimerValue() - TimerTickCounterContext) + 1) * HW_TIMER_TIME_BASE);
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

/*!
 * PIT timer irq handler
 */
void PIT_IRQHandler(void)
{
    if (PIT_HAL_IsIntPending(PIT_BASE_PTR, HWTIMER_TIMER_CHANNEL)) {
        /* Clear interrupt flag.*/
        PIT_HAL_ClearIntFlag(PIT_BASE_PTR, HWTIMER_TIMER_CHANNEL);
        TimerIncrementTickCounter();

        if (TimerTickCounter == TimeoutCntValue) {
            TimerIrqHandler();
        }
    } else if (PIT_HAL_IsIntPending(PIT_BASE_PTR, HWTIMER_DELAY_CHANNEL)) {
        /* Clear interrupt flag.*/
        PIT_HAL_ClearIntFlag(PIT_BASE_PTR, HWTIMER_DELAY_CHANNEL);
        TimerIncrementDelayCounter();
    }
}

void TimerHwEnterLowPowerStopMode(void)
{
#ifndef USE_DEBUGGER
    __WFI();
#endif
}
