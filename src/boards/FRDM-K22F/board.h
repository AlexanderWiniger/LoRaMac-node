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
#include "fsl_port_hal.h"
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
#include "rtc-board.h"
#include "timer-board.h"
#include "sx1276-board.h"
#include "uart-board.h"

#if defined( USE_USB_CDC )
#include "usb-cdc-board.h"
#endif

/*!
 * NULL definition
 */
#ifndef NULL
#define NULL                           ( ( void * )0 )
#endif

/*!
 * Generic definition
 */
#ifndef SUCCESS
#define SUCCESS                        1
#endif

#ifndef FAIL
#define FAIL                           0
#endif

/*!
 * Unique Devices IDs register set
 */
#define         ID1                    ( 0x1FF80050 )
#define         ID2                    ( 0x1FF80054 )
#define         ID3                    ( 0x1FF80064 )

/*!
 * Random seed generated using the MCU Unique ID
 */
#define RAND_SEED                      ( ( *( uint32_t* )ID1 ) ^ \
                                         ( *( uint32_t* )ID2 ) ^ \
                                         ( *( uint32_t* )ID3 ) )

/*!
 * Board MCU pins definitions
 */
#define LED_1                          PA_1
#define LED_2                          PA_2

#define RADIO_RESET                    PB_0

#define RADIO_SPI_DEVICE               SPI0
#define RADIO_MOSI                     PD_6
#define RADIO_MISO                     PD_7
#define RADIO_SCLK                     PD_5
#define RADIO_NSS                      PD_4

#define RADIO_DIO_0                    PB_16
#define RADIO_DIO_1                    PA_2
#define RADIO_DIO_2                    PA_4
#define RADIO_DIO_3                    PB_18
#define RADIO_DIO_4_A                  PB_19
#define RADIO_DIO_4_B                  PC_2
#define RADIO_DIO_5                    PA_1

#define RADIO_ANT_SWITCH_RX_TX         PB_3

#define USB_DM                         USB0_DM
#define USB_DP                         USB0_DP

#define FXOS8700CQ_I2C_DEVICE          I2C0
#define I2C_SCL                        PB_2
#define I2C_SDA                        PB_3

#define IRQ_1_FXOS8700CQ               PD_0
#define IRQ_2_FXOS8700CQ               PD_1

#define UART1_RX                       PE_1
#define UART1_TX                       PE_0

/*!
 * LED GPIO pins objects
 */
extern Gpio_t Led1;
extern Gpio_t Led2;

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
extern Uart_t Lpuart;
extern Uart_t Uart0;
extern Uart_t Uart1;
extern Uart_t Uart2;
#if defined( USE_USB_CDC )
extern Uart_t UartUsb;
#endif

/*!
 * MCU clock configuration
 */

#define CLOCK_VLPR 1U
#define CLOCK_RUN  2U
#define CLOCK_NUMBER_OF_CONFIGURATIONS 3U

#ifndef CLOCK_INIT_CONFIG
#define CLOCK_INIT_CONFIG CLOCK_RUN
#endif

/* OSC0 configuration. */
#define OSC0_XTAL_FREQ                 8000000U
#define OSC0_SC2P_ENABLE_CONFIG        false
#define OSC0_SC4P_ENABLE_CONFIG        false
#define OSC0_SC8P_ENABLE_CONFIG        false
#define OSC0_SC16P_ENABLE_CONFIG       false
#define MCG_HGO0                       kOscGainLow
#define MCG_RANGE0                     kOscRangeVeryHigh
#define MCG_EREFS0                     kOscSrcOsc

/* EXTAL0 PTA18 */
#define EXTAL0_PORT                    PORTA
#define EXTAL0_PIN                     18
#define EXTAL0_PINMUX                  kPortPinDisabled

/* XTAL0 PTA19 */
#define XTAL0_PORT                     PORTA
#define XTAL0_PIN                      19
#define XTAL0_PINMUX                   kPortPinDisabled

/* RTC external clock configuration. */
#define RTC_XTAL_FREQ                  32768U
#define RTC_SC2P_ENABLE_CONFIG         false
#define RTC_SC4P_ENABLE_CONFIG         false
#define RTC_SC8P_ENABLE_CONFIG         false
#define RTC_SC16P_ENABLE_CONFIG        false
#define RTC_OSC_ENABLE_CONFIG          true

#define BOARD_RTC_CLK_FREQUENCY        32768U;

/*!
 * \brief Initializes the target board peripherals.
 */
void BoardInitMcu( void );

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph( void );

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu( void );

/*!
 * \brief Gets the board 64 bits unique ID 
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId( uint8_t *id );

#endif // __BOARD_H__
