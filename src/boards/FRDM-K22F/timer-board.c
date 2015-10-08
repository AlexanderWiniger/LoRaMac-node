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
void TimerIncrementTickCounter( void );

/*!
 * Counter used for the Delay operations
 */
volatile uint32_t TimerDelayCounter = 0;

/*------------------------ Local Functions -------------------------------*/
/*!
 * Return the value of the counter used for a Delay
 */
uint32_t TimerHwGetDelayValue( void );

/*!
 * Increment the value of TimerDelayCounter
 */
void TimerIncrementDelayCounter( void );

void TimerHwInit( void ) {
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

    /* Calculate the count value, assign it to timer counter register.*/
    uint32_t count = (uint32_t)(HW_TIMER_TIME_BASE * pitSourceClock / 1000000U - 1U);

    /*!
     * PIT timer 0 initialization
     */
    /* Set timer period.*/
    PIT_HAL_SetTimerPeriodByCount(PIT_BASE_PTR, 0, count);

    /* Enable or disable interrupt.*/
    PIT_HAL_SetIntCmd(PIT_BASE_PTR, 0, true);

    /* Enable PIT interrupt.*/
    INT_SYS_EnableIRQ (PIT0_IRQn);

    /* Start timer 0 */
    PIT_HAL_StartTimer(PIT_BASE_PTR, 0);

    /* Calculate the count value, assign it to timer counter register.*/
    count = (uint32_t)(1 * pitSourceClock / 1000000U - 1U);

    /*!
     * PIT timer 1 initialization
     */
    /* Set timer period.*/
    PIT_HAL_SetTimerPeriodByCount(PIT_BASE_PTR, 1, count);

    /* Enable or disable interrupt.*/
    PIT_HAL_SetIntCmd(PIT_BASE_PTR, 1, true);

    /* Enable PIT interrupt.*/
    INT_SYS_EnableIRQ (PIT1_IRQn);

    /* Start timer 1 */
    PIT_HAL_StartTimer(PIT_BASE_PTR, 1);

}

void TimerHwDeInit( void ) {
    /* Disable PIT timer 0 interrupt. Clear the chain bit if available */
    PIT_HAL_SetIntCmd(PIT_BASE_PTR, 0, false);
    INT_SYS_DisableIRQ (PIT0_IRQn);
#if FSL_FEATURE_PIT_HAS_CHAIN_MODE
    PIT_HAL_SetTimerChainCmd(PIT_BASE_PTR, 0, false);
#endif

    /* Disable PIT timer 1 interrupt. Clear the chain bit if available */
    PIT_HAL_SetIntCmd(PIT_BASE_PTR, 1, false);
    INT_SYS_DisableIRQ (PIT1_IRQn);
#if FSL_FEATURE_PIT_HAS_CHAIN_MODE
    PIT_HAL_SetTimerChainCmd(PIT_BASE_PTR, 1, false);
#endif

    /* Disable PIT module clock*/
    PIT_HAL_Disable (PIT_BASE_PTR);

    /* Gate PIT clock control*/
    CLOCK_SYS_DisablePitClock(0);
}

uint32_t TimerHwGetMinimumTimeout( void ) {
    return 0;
}

void TimerHwStart( uint32_t val ) {

}

void TimerHwStop( void ) {

}

void TimerHwDelayMs( uint32_t delay ) {

}

TimerTime_t TimerHwGetElapsedTime( void ) {
    return 0;
}

TimerTime_t TimerHwGetTimerValue( void ) {
    TimerTime_t val = 0;

    INT_SYS_DisableIRQGlobal();

    val = TimerTickCounter;

    INT_SYS_EnableIRQGlobal();

    return (val);
}

TimerTime_t TimerHwGetTime( void ) {

    return TimerHwGetTimerValue() * HW_TIMER_TIME_BASE;
}

uint32_t TimerHwGetDelayValue( void ) {
    uint32_t val = 0;

    INT_SYS_DisableIRQGlobal();

    val = TimerDelayCounter;

    INT_SYS_EnableIRQGlobal();

    return (val);
}

void TimerIncrementTickCounter( void ) {
    INT_SYS_DisableIRQGlobal();

    TimerTickCounter++;

    INT_SYS_EnableIRQGlobal();
}

void TimerIncrementDelayCounter( void ) {
    INT_SYS_DisableIRQGlobal();

    TimerDelayCounter++;

    INT_SYS_EnableIRQGlobal();
}

/*!
 * PIT timer 0 irq handler
 */
void PIT0_IRQHandler( void ) {
    if ( PIT_HAL_IsIntPending(PIT_BASE_PTR, 0U) ) {
        /* Clear interrupt flag.*/
        PIT_HAL_ClearIntFlag(PIT_BASE_PTR, 0U);
        TimerIncrementTickCounter();

        if ( TimerTickCounter == TimeoutCntValue ) {
            TimerIrqHandler();
        }
    }
}

/*!
 * PIT timer 1 irq handler
 */
void PIT1_IRQHandler( void ) {
    if ( PIT_HAL_IsIntPending(PIT_BASE_PTR, 1U) ) {
        /* Clear interrupt flag.*/
        PIT_HAL_ClearIntFlag(PIT_BASE_PTR, 1U);
        TimerIncrementDelayCounter();
    }
}

void TimerHwEnterLowPowerStopMode( void ) {
#ifndef USE_DEBUGGER
    __WFI();
#endif
}
