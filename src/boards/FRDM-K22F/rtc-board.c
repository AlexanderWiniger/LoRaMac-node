/**
 * \file rtc-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 21.09.2015
 * \brief MCU RTC timer and low power modes management
 *
 */
#include <math.h>
#include <time.h>
#include "board.h"
#include "rtc-board.h"
#include "fsl_rtc_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"

/*----------------------- Local Definitions ------------------------------*/
/*!
 * RTC Time base in us
 */
#define RTC_ALARM_TIME_BASE                             122.07

/*!
 * MCU Wake Up Time
 */
#define MCU_WAKE_UP_TIME                                3400

/*----------------------- Local Functions ------------------------------*/
/*!
 * \brief Start the Rtc Alarm (time base 1s)
 */
static void RtcStartWakeUpAlarm( uint32_t timeoutValue );

/*!
 * \brief Read the MCU internal Calendar value
 *
 * \retval Calendar value
 */
static TimerTime_t RtcGetCalendarValue( void );

/*!
 * \brief Clear the RTC flags and Stop all IRQs
 */
static void RtcClearStatus( void );

/*------------------------ Local Variables -------------------------------*/
/* Table of base addresses for RTC instances. */
RTC_Type * const g_rtcBase[RTC_INSTANCE_COUNT] = RTC_BASE_PTRS;

/*!
 * \brief Indicates if the RTC is already Initalized or not
 */
static bool RtcInitalized = false;

/*!
 * \brief Flag to indicate if the timestamps until the next event is long enough 
 * to set the MCU into low power mode
 */
static bool RtcTimerEventAllowsLowPower = false;

/*!
 * \brief Flag to disable the LowPower Mode even if the timestamps until the
 * next event is long enough to allow Low Power mode 
 */
static bool LowPowerDisableDuringTask = false;

/*!
 * Keep the value of the RTC timer when the RTC alarm is set
 */
static TimerTime_t RtcTimerContext = 0;

/*!
 * Number of seconds in a minute
 */
static const uint8_t SecondsInMinute = 60;

/*!
 * Number of seconds in an hour
 */
static const uint16_t SecondsInHour = 3600;

/*!
 * Number of seconds in a day
 */
static const uint32_t SecondsInDay = 86400;

/*!
 * Number of hours in a day
 */
static const uint8_t HoursInDay = 24;

/*!
 * Number of days in a standard year
 */
static const uint16_t DaysInYear = 365;

/*!
 * Number of days in a leap year
 */
static const uint16_t DaysInLeapYear = 366;

/*!
 * Number of days in a century
 */
static const double DaysInCentury = 36524.219;

/*!
 * Number of days in each month on a normal year
 */
static const uint8_t DaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*!
 * Number of days in each month on a leap year
 */
static const uint8_t DaysInMonthLeapYear[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*!
 * Hold the previous year value to detect the turn of a century
 */
static uint8_t PreviousYear = 0;

/*!
 * Century counter
 */
static uint8_t Century = 0;

/*!
 * RTC external clock configuration
 */
rtc_osc_user_config_t rtcOscConfig =
{
    .freq = RTC_XTAL_FREQ,
    .enableCapacitor2p = RTC_SC2P_ENABLE_CONFIG,
    .enableCapacitor4p = RTC_SC4P_ENABLE_CONFIG,
    .enableCapacitor8p = RTC_SC8P_ENABLE_CONFIG,
    .enableCapacitor16p = RTC_SC16P_ENABLE_CONFIG,
    .enableOsc = RTC_OSC_ENABLE_CONFIG,
};

void RtcInit( void ) {
    if ( RtcInitalized == false ) {
        /* Enable clock gate to RTC module */
        CLOCK_SYS_EnableRtcClock(0U);

        /* Initialize the general configuration for RTC module.*/
        RTC_HAL_Init (RTC_BASE_PTR);

        NVIC_ClearPendingIRQ (RTC_IRQn);
        NVIC_ClearPendingIRQ (RTC_Seconds_IRQn);
        INT_SYS_EnableIRQ(RTC_IRQn);
        INT_SYS_EnableIRQ(RTC_Seconds_IRQn);

        /* Enable the RTC Clock output */
        RTC_HAL_SetClockOutCmd(RTC_BASE_PTR, true);

        /* Configure RTC external clock */
        CLOCK_SYS_RtcOscInit(0U, &rtcOscConfig);
        RtcInitalized = true;
    }
}

void RtcStopTimer( void ) {
    RtcClearStatus();
}

uint32_t RtcGetMinimumTimeout( void ) {
    return (ceil(3 * RTC_ALARM_TIME_BASE));
}

void RtcSetTimeout( uint32_t timeout ) {

}

uint32_t RtcGetTimerElapsedTime( void ) {
    return 0;
}

TimerTime_t RtcGetTimerValue( void ) {
    return 0;
}

static void RtcClearStatus( void ) {

}

static void RtcStartWakeUpAlarm( uint32_t timeoutValue ) {

}

void RtcEnterLowPowerStopMode( void ) {

}

void RtcRecoverMcuStatus( void ) {

}

/*!
 * \brief RTC IRQ Handler on the RTC Alarm
 */
void RTC_Alarm_IRQHandler( void ) {

}

void BlockLowPowerDuringTask( bool status ) {

}

void RtcDelayMs( uint32_t delay ) {

}

TimerTime_t RtcGetCalendarValue( void ) {
    return (TimerTime_t) 0;
}
