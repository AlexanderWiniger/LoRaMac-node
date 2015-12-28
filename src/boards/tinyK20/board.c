/**
 * \file board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 17.11.2015
 * \brief Target board general functions implementation
 *
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "board.h"
#include "debug.h"

/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
/*! FIFO buffers size */
//#define SHELL_FIFO_TX_SIZE                                128
#define SHELL_FIFO_RX_SIZE                                128

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/
/*! LED GPIO pin objects */
Gpio_t Led1;

/*! GPIO pin objects */
Gpio_t GpsPps;

/*! MCU objects */
Adc_t Adc;
I2c_t I2c;
Uart_t Uart0;
Uart_t Uart1;
Uart_t Uart2;
#if defined( USE_USB_CDC )
Uart_t UartUsb;
#endif

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
/*! Flag to indicate if the MCU is Initialized */
static bool McuInitialized = false;

/*! FIFO buffers */
//uint8_t Shell_TxBuffer[SHELL_FIFO_TX_SIZE];
uint8_t Shell_RxBuffer[SHELL_FIFO_RX_SIZE];

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*! Initializes the unused GPIO to a known status */
static void BoardUnusedIoInit( void );

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
void BoardInitPeriph( void )
{
    /* Init GPIO pins */
    GpioInit(&Led1, LED_1, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);

    /* Init GPS */
    //GpsInit();
    // Switch LED 1 OFF
    GpioWrite(&Led1, 1);
}

void BoardInitMcu( void )
{
    if ( McuInitialized == false ) {
        /* Initialize low level components */
        low_level_init();

        /*! SPI channel to be used by Semtech SX1276 */
#if defined(SX1276_BOARD_EMBED)
        SpiInit(&SX1276.Spi, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, NC);
        SX1276IoInit();
#endif

#if defined( USE_SHELL )
#if defined (USE_USB_CDC)
        UartInit( &UartUsb, UART_USB_CDC, NC, NC );
        UartConfig( &UartUsb, RX_TX, 115200, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );
        TimerSetLowPowerEnable(false);
#else
        //FifoInit( &Uart1.FifoTx, Shell_TxBuffer, SHELL_FIFO_TX_SIZE );
        FifoInit(&Uart1.FifoRx, Shell_RxBuffer, SHELL_FIFO_RX_SIZE);
        UartInit(&Uart1, UART_1, UART1_TX, UART1_RX);
        UartConfig(&Uart1, RX_TX, 115200, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY,
                NO_FLOW_CTRL);
#if defined(DEBUG)
        DbgConsole_Init(&Uart1);
#endif
        TimerSetLowPowerEnable(false);
#endif /* USE_USB_CDC */
#elif defined(DEBUG)
        UartInit(&Uart1, UART_1, UART1_TX, UART1_RX);
        UartConfig(&Uart1, RX_TX, 115200, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY,
                NO_FLOW_CTRL);
        DbgConsole_Init(&Uart1);
        TimerSetLowPowerEnable(false);
#elif( LOW_POWER_MODE_ENABLE )
        TimerSetLowPowerEnable(true);
#else
        TimerSetLowPowerEnable(false);
#endif
        BoardUnusedIoInit();

#if !defined(USE_FREE_RTOS)
        if ( TimerGetLowPowerEnable() == true ) {
            RtcInit();
        } else {
            TimerHwInit();
        }
#endif /* USE_FREE_RTOS */

        McuInitialized = true;
    }
}

void BoardDeInitMcu( void )
{
#if defined(SX1276_BOARD_EMBED)
    SpiDeInit(&SX1276.Spi);
    SX1276IoDeInit();
#endif

    McuInitialized = false;
}

uint8_t BoardGetBatteryLevel( void )
{
    /* Device is connected to an external power source*/
    return 0;
}

uint32_t BoardGetRandomSeed( void )
{
    int32_t randout = 0;

    return randout;
}

void BoardGetUniqueId( uint8_t *id )
{
    id[0] = (ID1 + ID3) >> 24;
    id[1] = (ID1 + ID3) >> 16;
    id[2] = (ID1 + ID3) >> 8;
    id[3] = (ID1 + ID3);
    id[4] = (ID2 + ID4) >> 24;
    id[5] = (ID2 + ID4) >> 16;
    id[6] = (ID2 + ID4) >> 8;
    id[7] = (ID2 + ID4);
}

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
static void BoardUnusedIoInit( void )
{
// \todo Initialize unused gpio to knwon state
}
