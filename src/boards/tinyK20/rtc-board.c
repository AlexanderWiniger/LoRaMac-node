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

/*------------------------ Local Variables -------------------------------*/
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

void RtcInit(void)
{
    if (RtcInitalized == false) {

        RtcInitalized = true;
    }
}

void RtcStopTimer(void)
{
    RtcClearStatus();
}

uint32_t RtcGetMinimumTimeout(void)
{
    return (ceil(3 * RTC_ALARM_TIME_BASE));
}

void RtcSetTimeout(uint32_t timeout)
{

}

uint32_t RtcGetTimerElapsedTime(void)
{
    uint32_t CalendarValue = 0;
    return ((uint32_t)(ceil(((CalendarValue - RtcTimerContext) + 2) * RTC_ALARM_TIME_BASE)));
}

TimerTime_t RtcGetTimerValue(void)
{
    TimerTime_t CalendarValue = 0;

    CalendarValue = RtcGetCalendarValue();

    return ((CalendarValue + 2) * RTC_ALARM_TIME_BASE);
}

static void RtcClearStatus(void)
{

}

static void RtcStartWakeUpAlarm(uint32_t timeoutValue)
{

}

void RtcEnterLowPowerStopMode(void)
{
    if ((LowPowerDisableDuringTask == false) && (RtcTimerEventAllowsLowPower == true)) {
        // Disable IRQ while the MCU is being deinitialized to prevent race issues

//      \todo Implement RtcEnterLowPowerStopMode
//        BoardDeInitMcu();

    }
}

void RtcRecoverMcuStatus(void)
{
    if (TimerGetLowPowerEnable() == true) {
        if ((LowPowerDisableDuringTask == false) && (RtcTimerEventAllowsLowPower == true)) {
            // Disable IRQ while the MCU is not running on HSE

//          \todo Implement RtcRecoverMcuStatus
//            BoardInitMcu();

        }
    }
}

/*!
 * \brief RTC IRQ Handler on the RTC Alarm
 */
void RTC_IRQHandler(void)
{

}

/*!
 * \brief RTC IRQ Handler on the RTC Seconds interrupt
 */
void RTC_Seconds_IRQHandler(void)
{

}

void BlockLowPowerDuringTask(bool status)
{
    if (status == true) {
        RtcRecoverMcuStatus();
    }
    LowPowerDisableDuringTask = status;
}

void RtcDelayMs(uint32_t delay)
{
    TimerTime_t delayValue = 0;
    TimerTime_t timeout = 0;

    delayValue = (TimerTime_t)(delay * 1000);

    // Wait delay ms
    timeout = RtcGetTimerValue();
    while (((RtcGetTimerValue() - timeout)) < delayValue) {
//        __NOP();
    }
}

TimerTime_t RtcGetCalendarValue(void)
{
    uint32_t seconds = 0;
    uint32_t srcClock = 0;
    uint64_t tmp = 0;

    return (TimerTime_t) seconds;
}
