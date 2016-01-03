/**
 * \file gps-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 02.11.2015
 * \brief Target board driver for GPS receiver
 *
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "board.h"
#include <string.h>

#define LOG_LEVEL_NONE
#include "debug.h"

/*! FIFO buffers size */
#define GPS_FIFO_RX_SIZE                                128

/*! Maximum NMEA message length */
#define NMEA_MAX_MESSAGE_LENGTH                         256

/*! FIFO buffers */
uint8_t Gps_RxBuffer[GPS_FIFO_RX_SIZE];

/*! Nmea string */
char NmeaString[256];
uint8_t NmeaStringLength = 0;

/*! Nmea configuration strings */
//const char psrf_100[] = "$PSRF100,1,19200,8,1,0*38"; /* Set Serial Port */
//const char psrf_103_gga[] = "$PSRF103,00,00,00,01*24"; /* Deactivate GGA messages */
//const char psrf_103_gsa[] = "$PSRF103,02,00,05,01*23"; /* Deactivate GSA messages */
//const char psrf_103_gsv[] = "$PSRF103,03,00,00,01*27"; /* Deactivate GSV messages */
//const char psrf_104[] = "$PSRF104,47.0167,8.3167,441,0,566417,1877,12,2*24"; /* LLA Navigation Initialization */
//const char psrf_117[] = "$PSRF117,16*0B"; /* Shutdown message */
void GpsMcuOnPpsSignal( void )
{
    bool parseData = false;

    GpsPpsHandler(&parseData);

    if ( parseData == true ) {
        UartInit(&Uart0, UART_0, UART0_TX, UART0_RX);
        UartConfig(&Uart0, RX_ONLY, 4800, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY,
                NO_FLOW_CTRL);
    }
}

void GpsMcuInit( void )
{
    NmeaStringLength = 0;

    FifoInit(&Uart0.FifoRx, Gps_RxBuffer, GPS_FIFO_RX_SIZE);
    Uart0.IrqNotify = GpsMcuIrqNotify;
#if 0
    UartInit(&Uart0, UART_0, UART0_TX, UART0_RX);
    UartConfig(&Uart0, RX_ONLY, 4800, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY,
            NO_FLOW_CTRL);
    /* Configure GPS module */
//    UartPutBuffer(&Uart0, (uint8_t *) psrf_100, sizeof(psrf_100));
//    UartPutBuffer(&Uart0, (uint8_t *) psrf_103_gga, sizeof(psrf_103_gga));
//    UartPutBuffer(&Uart0, (uint8_t *) psrf_103_gsa, sizeof(psrf_103_gsa));
//    UartPutBuffer(&Uart0, (uint8_t *) psrf_103_gsv, sizeof(psrf_103_gsv));
//    UartPutBuffer(&Uart0, (uint8_t *) psrf_104, sizeof(psrf_104));
//    UartPutBuffer(&Uart0, (uint8_t *) psrf_117, sizeof(psrf_117));
#endif
    GpioInit(&GpsPps, PPS, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
    GpioSetInterrupt(&GpsPps, IRQ_FALLING_EDGE, IRQ_VERY_LOW_PRIORITY,
            &GpsMcuOnPpsSignal);
}

void GpsMcuIrqNotify( UartNotifyId_t id )
{
    uint8_t data;
    if ( id == UART_NOTIFY_RX ) {
        while (UartGetChar(&Uart0, &data) != ERR_RXEMPTY) {
            if ( (data == '$') || (NmeaStringLength >= NMEA_MAX_MESSAGE_LENGTH) ) {
                NmeaStringLength = 0;
            }
            LOG_TRACE_BARE("%c", (char) data);
            NmeaString[NmeaStringLength++] = (int8_t) data;

            if ( data == '\n' ) {
                NmeaString[NmeaStringLength] = '\0';
                if ( strncmp(NmeaString, (const char*) "$GPRMC", 6) == 0 ) {
                    if ( GpsParseGpsData(NmeaString, NmeaStringLength) == SUCCESS ) {
                        LOG_TRACE_BARE("NMEA: %s", NmeaString);
                        UartDeInit (&Uart0);
                        BlockLowPowerDuringTask(false);
                    }
                }
            }
        }
    }
}
