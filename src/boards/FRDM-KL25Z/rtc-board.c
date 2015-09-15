/**
 * \file rtc-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief MCU RTC timer and low power modes management
 *
 */

#include <math.h>
#include <time.h>
#include "board.h"
#include "rtc-board.h"

/*!
 * RTC Time base in us
 */
#define RTC_ALARM_TIME_BASE                             122.07

/*!
 * MCU Wake Up Time
 */
#define MCU_WAKE_UP_TIME                                3400

/*!
 * \brief Configure the Rtc hardware
 */
static void RtcSetConfig(void);

/*!
 * \brief Configure the Rtc Alarm
 */
static void RtcSetAlarmConfig(void);

/*!
 * \brief Start the Rtc Alarm (time base 1s)
 */
static void RtcStartWakeUpAlarm(uint32_t timeoutValue);

/*!
 * \brief Read the MCU internal Calendar value
 *
 * \retval Calendar value
 */
static TimerTime_t RtcGetCalendarValue(void);

/*!
 * \brief Clear the RTC flags and Stop all IRQs
 */
static void RtcClearStatus(void);

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
static const uint8_t DaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
        30, 31 };

/*!
 * Number of days in each month on a leap year
 */
static const uint8_t DaysInMonthLeapYear[] = { 31, 29, 31, 30, 31, 30, 31, 31,
        30, 31, 30, 31 };

/*!
 * Hold the previous year value to detect the turn of a century
 */
static uint8_t PreviousYear = 0;

/*!
 * Century counter
 */
static uint8_t Century = 0;

void RtcInit(void)
{

}

static void RtcSetConfig(void)
{

}

static void RtcSetAlarmConfig(void)
{

}

void RtcStopTimer(void)
{
}

uint32_t RtcGetMinimumTimeout(void)
{
    return 0;
}

void RtcSetTimeout(uint32_t timeout)
{

}

uint32_t RtcGetTimerElapsedTime(void)
{
    return 0;
}

TimerTime_t RtcGetTimerValue(void)
{
    return 0;
}

static void RtcClearStatus(void)
{

}

static void RtcStartWakeUpAlarm(uint32_t timeoutValue)
{

}

void RtcEnterLowPowerStopMode(void)
{

}

void RtcRecoverMcuStatus(void)
{

}

/*!
 * \brief RTC IRQ Handler on the RTC Alarm
 */
void RTC_Alarm_IRQHandler(void)
{

}

void BlockLowPowerDuringTask(bool status)
{

}

void RtcDelayMs(uint32_t delay)
{

}

TimerTime_t RtcGetCalendarValue(void)
{
    return 0;
}
