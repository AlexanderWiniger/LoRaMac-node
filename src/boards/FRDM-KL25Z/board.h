/**
 * \file board.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board general functions implementation
 *
 */
#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdbool.h>
#include "fsl_device_registers.h"
#include "utilities.h"
#include "timer.h"
#include "delay.h"
#include "gpio.h"
#include "adc.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "radio.h"
#include "sx1276/sx1276.h"
#include "mma8451.h"
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
//#define BOARD_IOE_EXT
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
 * Unique Devices IDs register set
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
#define LED_1                                       PB_19
#define LED_2                                       PB_18
#define LED_3                                       PD_1

#define RADIO_RESET                                 PC_1

#define RADIO_MOSI                                  PD_2
#define RADIO_MISO                                  PD_3
#define RADIO_SCLK                                  PD_1
#define RADIO_NSS                                   PD_0

#define RADIO_DIO_0                                 PA_1
#define RADIO_DIO_1                                 PA_2
#define RADIO_DIO_2                                 PB_0
#define RADIO_DIO_3                                 PB_2
#define RADIO_DIO_4                                 PB_3
#define RADIO_DIO_5                                 PC_2

#define RADIO_ANT_SWITCH_HF                         PE_1
#define RADIO_ANT_SWITCH_LF                         PE_0

#define OSC_EXTAL0                                  PA_18
#define OSC_XTAL0                                   PA_19

#define USB_DM                                      USB0_DM
#define USB_DP                                      USB0_DP

#define I2C_SCL                                     PE_24
#define I2C_SDA                                     PE_25

#define IRQ_1_MMA8451                               PA_14
#define IRQ_2_MMA8451                               PA_15

#define UART_TX                                     PA_1
#define UART_RX                                     PA_2

#define TSIO_CH9                                    PB_16
#define TSIO_CH10                                   PB_17

#define SX_GPIO0                                    PB_1

#define JTAG_RX_TGTMCU                              PA_1
#define JTAG_TX_TGTMCU                              PA_2
#define JTAG_SWD_CLK                                PA_0
#define JTAG_SWD_DIO_TGTMCU                         PA_3
#define JTAG_NRST_TGTMCU                            PA_20

/*!
 * LED GPIO pins objects
 */
extern Gpio_t Led1;
extern Gpio_t Led2;
extern Gpio_t Led3;

/*!
 * IRQ GPIO pins objects
 */
extern Gpio_t Irq1Mma8451;
extern Gpio_t Irq2Mma8451;

/*!
 * MCU objects
 */
extern Adc_t Adc;
extern I2c_t I2c;
extern Uart_t Uart1;
#if defined( USE_USB_CDC )
extern Uart_t UartUsb;
#endif

/*!
 * MCU clock configuration
 */
/* OSC0 configuration. */
#define OSC0_XTAL_FREQ                 8000000U
#define OSC0_SC2P_ENABLE_CONFIG        false
#define OSC0_SC4P_ENABLE_CONFIG        false
#define OSC0_SC8P_ENABLE_CONFIG        false
#define OSC0_SC16P_ENABLE_CONFIG       false
#define MCG_HGO0                       kOscGainLow
#define MCG_RANGE0                     kOscRangeVeryHigh
#define MCG_EREFS0                     kOscSrcOsc

/* RTC external clock configuration. */
#define RTC_XTAL_FREQ   0U
#define RTC_SC2P_ENABLE_CONFIG         false
#define RTC_SC4P_ENABLE_CONFIG         false
#define RTC_SC8P_ENABLE_CONFIG         false
#define RTC_SC16P_ENABLE_CONFIG        false
#define RTC_OSC_ENABLE_CONFIG          false
#define RTC_CLK_OUTPUT_ENABLE_CONFIG   false

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
 * \brief Gets the board 64 bits unique ID 
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId(uint8_t *id);

#endif // __BOARD_H__
