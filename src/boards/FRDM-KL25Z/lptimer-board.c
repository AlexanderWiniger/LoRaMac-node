/**
 * \file lptimer-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief MCU RTC timer and low power modes management
 *
 */

#include <time.h>
#include "board.h"
#include "lptimer-board.h"
#include "fsl_lptmr_driver.h"

/*!
 * Low-power timer tmie base in us
 */
#define LPTMR_TIME_BASE         100U

/*!
 * \brief Low-power timer interrupt call back function.
 */
void lptmr_call_back( void );

/*!
 * Low-power timer configuration
 */
const lptmr_user_config_t lptmrConfig =
{
    .timerMode = kLptmrTimerModeTimeCounter,
    .freeRunningEnable = false,
    .prescalerEnable = true,
    .prescalerClockSource = kClockLptmrSrcLpoClk,
    .prescalerValue = kLptmrPrescalerDivide2,
    .isInterruptEnabled = true,
};

/*!
 * \brief Indicates if the RTC is already Initalized or not
 */
static bool LptmrInitalized = false;

void LptmrInit( void ) {
    if ( LptmrInitalized == false ) {
        lptmr_state_t lptmrState;
        LPTMR_DRV_Init(LPTMR0_IDX, &lptmrState, &lptmrConfig);
        // Set timer period for TMR_PERIOD seconds
        LPTMR_DRV_SetTimerPeriodUs(LPTMR0_IDX, LPTMR_TIME_BASE);
        // Install interrupt call back function for LPTMR
        LPTMR_DRV_InstallCallback(LPTMR0_IDX, lptmr_call_back);
        // Start LPTMR
        LPTMR_DRV_Start (LPTMR0_IDX);
    }
}

void LptmrStopTimer( void ) {
    LPTMR_DRV_Stop (LPTMR0_IDX);
}

uint32_t LptmrGetMinimumTimeout( void ) {
    return 0;
}

void LptmrSetTimeout( uint32_t timeout ) {

}

uint32_t LptmrGetTimerElapsedTime( void ) {
    return 0;
}

TimerTime_t LptmrGetTimerValue( void ) {
    return 0;
}

void LptmrEnterLowPowerStopMode( void ) {

}

void LptmrRecoverMcuStatus( void ) {

}

void lptmr_call_back( void ) {
    TimerIrqHandler();
}

void BlockLowPowerDuringTask( bool status ) {

}

void LptmrDelayMs( uint32_t delay ) {

}
