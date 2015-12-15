/*
 / _____)             _              | |
 ( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
 (______/|_____)_|_|_| \__)_____)\____)_| |_|
 (C)2013 Semtech

 Description: Timer objects and scheduling management

 License: Revised BSD License, see LICENSE.TXT file include in the project

 Maintainer: Miguel Luis and Gregory Cristian
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#if defined(FSL_RTOS_FREE_RTOS) || defined(USE_FREE_RTOS)
#include "FreeRTOS.h"
#include "timers.h"
#endif

/*!
 * \brief Timer time variable definition
 */
#ifndef TimerTime_t
typedef uint64_t TimerTime_t;
#endif

#if defined(FSL_RTOS_FREE_RTOS) || defined(USE_FREE_RTOS)

/*! \brief Timer object description */
typedef struct TimerEvent_s {
    TimerHandle_t Handle;
    uint32_t PeriodInMs;   //! Timer period value
    bool HasChanged;//! Period of the timer has changed
    bool AutoReload;//! Is auto reload enabled
    bool IsRunning;//! Is Timer running
    TimerCallbackFunction_t Callback;//! Timer callback function
}TimerEvent_t;

/*!
 * \brief Initializes the timer object
 *
 * \remark TimerSetValue function must be called before starting the timer.
 *         this function initializes timestamp and reload value at 0.
 *
 * \param [IN] obj          Structure containing the timer object parameters
 * \param [IN] callback     Function callback called at the end of the timeout
 */
void TimerInit( TimerEvent_t *obj, const char* name, uint32_t id, uint32_t periodInUs, TimerCallbackFunction_t callback, bool autoReload);

#else
/*!
 * \brief Timer object description
 */
typedef struct TimerEvent_s {
    uint32_t Timestamp;         //! Current timer value
    uint32_t ReloadValue;       //! Timer delay value
    bool IsRunning;             //! Is the timer currently running
    void (*Callback)( void );   //! Timer IRQ callback function
    struct TimerEvent_s *Next;   //! Pointer to the next Timer object.
} TimerEvent_t;

/*!
 * \brief Initializes the timer object
 *
 * \remark TimerSetValue function must be called before starting the timer.
 *         this function initializes timestamp and reload value at 0.
 *
 * \param [IN] obj          Structure containing the timer object parameters
 * \param [IN] callback     Function callback called at the end of the timeout
 */
void TimerInit( TimerEvent_t *obj, void (*callback)( void ) );

/*!
 * Timer IRQ event handler
 */
void TimerIrqHandler( void );

#endif /* FSL_RTOS_FREE_RTOS */

/*!
 * \brief Starts and adds the timer object to the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStart( TimerEvent_t *obj );

/*!
 * \brief Stops and removes the timer object from the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerStop( TimerEvent_t *obj );

/*!
 * \brief Resets the timer object
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
void TimerReset( TimerEvent_t *obj );

/*!
 * \brief Set timer new timeout value
 *
 * \param [IN] obj   Structure containing the timer object parameters
 * \param [IN] value New timer timeout value
 */
void TimerSetValue( TimerEvent_t *obj, uint32_t periodInUs );

/*!
 * \brief Read the current time
 *
 * \retval time returns current time
 */
TimerTime_t TimerGetCurrentTime( void );

/*!
 * \brief Manages the entry into ARM cortex deep-sleep mode
 */
void TimerLowPowerHandler( void );

/*!
 * \brief Enables/Disables low power timers usage
 *
 * \param [IN] enable [true]RTC timer used, [false]Normal timer used
 */
void TimerSetLowPowerEnable( bool enable );

/*!
 * \brief Initializes the timer object
 *
 * \retval enable [true]RTC timer used, [false]Normal timer used
 */
bool TimerGetLowPowerEnable( void );
#endif  // __TIMER_H__
