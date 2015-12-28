/**
 * \file gps-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 02.11.2015
 * \brief Target board driver for GPS receiver
 *
 */

#include "board.h"

#define LOG_LEVEL_TRACE
#include "debug.h"

/*!
 * FIFO buffers size
 */
//#define GPS_FIFO_TX_SIZE                                128
#define GPS_FIFO_RX_SIZE                                128

/*!
 * FIFO buffers
 */
//uint8_t Gps_TxBuffer[GPS_FIFO_TX_SIZE];
uint8_t Gps_RxBuffer[GPS_FIFO_RX_SIZE];

/*!
 * Nmea string
 */
int8_t NmeaString[128];
uint8_t NmeaStringSize = 0;

void GpsMcuOnPpsSignal( void )
{
    bool parseData = false;

    GpsPpsHandler(&parseData);

    if ( parseData == true ) {
//        UartInit(&Uart0, UART_0, UART0_TX, UART0_RX);
//        UartConfig(&Uart0, RX_ONLY, 4800, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL);
    }
}

void GpsMcuInit( void )
{
    NmeaStringSize = 0;

    //FifoInit( &Uart0.FifoTx, Gps_TxBuffer, GPS_FIFO_TX_SIZE );
    FifoInit(&Uart0.FifoRx, Gps_RxBuffer, GPS_FIFO_RX_SIZE);
    Uart0.IrqNotify = GpsMcuIrqNotify;

    GpioInit(&GpsPps, PPS, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
    GpioSetInterrupt(&GpsPps, IRQ_FALLING_EDGE, IRQ_VERY_LOW_PRIORITY,
            &GpsMcuOnPpsSignal);

    UartInit(&Uart0, UART_0, UART0_TX, UART0_RX);
    UartConfig(&Uart0, RX_ONLY, 4800, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY,
            NO_FLOW_CTRL);
}

void GpsMcuIrqNotify( UartNotifyId_t id )
{
    uint8_t data;
    if ( id == UART_NOTIFY_RX ) {
        while (UartGetChar(&Uart0, &data) != ERR_RXEMPTY) {
            if ( (data == '$') || (NmeaStringSize >= 128) ) {
                NmeaStringSize = 0;
            }
            LOG_TRACE_BARE("%c", (char) data);
            NmeaString[NmeaStringSize++] = (int8_t) data;

            if ( data == '\n' ) {
                NmeaString[NmeaStringSize] = '\0';
                GpsParseGpsData(NmeaString, NmeaStringSize);
                LOG_TRACE_BARE("%s", NmeaString);
//                UartDeInit (&Uart0);
                BlockLowPowerDuringTask(false);
            }
        }
    }
}
