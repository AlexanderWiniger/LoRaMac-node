/**
 * \file board.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board general functions implementation
 *
 */
#ifndef __BOARD_H__
#define __BOARD_H__

#include "timer.h"
#include "delay.h"
#include "gpio.h"
#include "adc.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "radio.h"
#include "sx1276/sx1276.h"
#include "rtc-board.h"
#include "timer-board.h"
#include "sx1276-board.h"
#include "uart-board.h"

#if defined( USE_USB_CDC )
#include "usb-cdc-board.h"
#endif

/*!
 * Define indicating if an external IO expander is to be used
 */
#define BOARD_IOE_EXT

/*!
 * NULL definition
 */
#ifndef NULL
#define NULL                                    ( ( void * )0 )
#endif

/*!
 * Generic definition
 */
#ifndef SUCCESS
#define SUCCESS                                     1
#endif

#ifndef FAIL
#define FAIL                                        0  
#endif

/*!
 * Unique Devices IDs register set ( STM32L1xxx )
 */
#define         ID1                                 ( 0x1FF80050 )
#define         ID2                                 ( 0x1FF80054 )
#define         ID3                                 ( 0x1FF80064 )

/*!
 * Random seed generated using the MCU Unique ID
 */
#define RAND_SEED                                   ( ( *( uint32_t* )ID1 ) ^ \
                                                      ( *( uint32_t* )ID2 ) ^ \
                                                      ( *( uint32_t* )ID3 ) )

/*!
 * Board IO Extender pins definitions
 */

/*!
 * Board MCU pins definitions
 */

/*!
 * LED GPIO pins objects
 */

/*!
 * MCU objects
 */

/*!
 * \brief Initializes the target board peripherals.
 */
void BoardInitMcu(void);

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph(void);

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu(void);

/*!
 * \brief Measure the Battery level
 *
 * \retval value  battery level ( 0: very low, 254: fully charged )
 */
uint8_t BoardMeasureBatterieLevel(void);

/*!
 * \brief Gets the board 64 bits unique ID 
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId(uint8_t *id);

#endif // __BOARD_H__
