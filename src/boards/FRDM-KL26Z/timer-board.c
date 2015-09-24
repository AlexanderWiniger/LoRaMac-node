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
#include "fsl_hwtimer.h"
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
 * Hardware Timer device struct
 */
extern const hwtimer_devif_t kSystickDevif;

/*!
 * Hardware Timer struct
 */
hwtimer_t hwtimer;

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

/*!
 * HWTimer callback function (see Kinetis SDK v.1.2 API Reference Manual)
 * \param data Arbitrary pointer passed as parameter to the callback function.
 */
void hwtimer_callback( void* data );

void TimerHwInit( void ) {
    /*!
     *  Hwtimer initialization
     *  \remark Uses ARM SysTick
     */
    HWTIMER_SYS_Init(&hwtimer, &HWTIMER_LL_DEVIF, HWTIMER_LL_ID, NULL);
//    NVIC_SetPriority(SysTick_IRQn, 1);
    HWTIMER_SYS_SetPeriod(&hwtimer, HW_TIMER_TIME_BASE);
    HWTIMER_SYS_RegisterCallback(&hwtimer, hwtimer_callback, NULL);
    HWTIMER_SYS_Start(&hwtimer);

    /*!
     * TPM0 initialization
     * \remark Only used for delay
     */

}

void TimerHwDeInit( void ) {
    HWTIMER_SYS_Deinit(&hwtimer);
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

void hwtimer_callback( void* data ) {

}

void TimerHwEnterLowPowerStopMode( void ) {
#ifndef USE_DEBUGGER
    __WFI();
#endif
}
