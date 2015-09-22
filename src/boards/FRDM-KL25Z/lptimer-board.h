/**
 * \file lptimer-board.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief MCU low-power timer and low power modes management
 *
 */

#ifndef __LPTIMER_BOARD_H__
#define __LPTIMER_BOARD_H__

/*!
 * \brief Timer time variable definition
 */
#ifndef TimerTime_t
typedef uint64_t TimerTime_t;
#endif

/*!
 * \brief Initializes the low-power timer
 *
 */
void LptmrInit( void );

/*!
 * \brief Stop the low-power timer
 */
void LptmrStopTimer( void );

/*!
 * \brief Return the minimum timeout the low-power timer is able to handle
 *
 * \retval minimum value for a timeout
 */
uint32_t LptmrGetMinimumTimeout( void );

/*!
 * \brief Start the low-power timer
 *
 * \param[IN] timeout       Duration of the Timer
 */
void LptmrSetTimeout( uint32_t timeout );

/*!
 * \brief Get the low-power timer value
 *
 * \retval low-power timer value
 */
TimerTime_t LptmrGetTimerValue( void );

/*!
 * \brief Get the low-power timer elapsed time since the last Alarm was set
 *
 * \retval Low-power timer Elapsed time since the last alarm
 */
uint32_t LptmrGetTimerElapsedTime( void );

/*!
 * \brief This function block the MCU from going into Low Power mode
 *
 * \param [IN] Status enable or disable
 */
void BlockLowPowerDuringTask( bool Status );

/*!
 * \brief Sets the MCU in low power STOP mode
 */
void LptmrEnterLowPowerStopMode( void );

/*!
 * \brief Restore the MCU to its normal operation mode
 */
void LptmrRecoverMcuStatus( void );

/*!
 * \brief Perfoms a standard blocking delay in the code execution
 *
 * \param [IN] delay Delay value in ms
 */
void LptmrDelayMs( uint32_t delay );

#endif // __LPTIMER_BOARD_H__
