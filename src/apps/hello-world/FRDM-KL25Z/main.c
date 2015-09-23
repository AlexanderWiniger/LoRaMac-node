/**
 * \file main.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 17.09.2015
 * \brief Hello World application implementation
 *
 */

#include "board.h"
#include "uart.h"

/*------------------------- Local Defines --------------------------------*/
// Timer period: 500000uS
#define TMR_PERIOD         500000U

/*------------------------ Local Variables -------------------------------*/
/*!
 * FIFO buffers size
 */
#define FIFO_TX_SIZE                                128
#define FIFO_RX_SIZE                                128

uint8_t TxBuffer[FIFO_TX_SIZE];
uint8_t RxBuffer[FIFO_RX_SIZE];

static TimerEvent_t Led1Timer;
volatile bool Led1TimerEvent = false;

/*!
 * \brief Function executed on Led 1 Timeout event
 */
void OnLed1TimerEvent(void)
{
    Led1TimerEvent = true;
}

/*!
 * \brief Main application entry point.
 */
int main(void)
{
    // RX buffers
    //! @param receiveBuff Buffer used to hold received data
    uint8_t receiveBuff;

    // Target board initialisation
    BoardInitMcu();
    BoardInitPeriph();

//    TimerInit(&Led1Timer, OnLed1TimerEvent);
//    TimerSetValue(&Led1Timer, 90000);

    // Switch LED 1 ON
    GpioWrite(&Led1, 0);
//    TimerStart(&Led1Timer);

    FifoInit(&Uart0.FifoTx, TxBuffer, FIFO_TX_SIZE);
    FifoInit(&Uart0.FifoRx, RxBuffer, FIFO_RX_SIZE);

    // Print the initial banner
    UartPutBuffer(&Uart0, "Hello World!\n\r", sizeof("Hello World!\n\r"));

    while (1) {
        // Main routine that simply echoes received characters forever
        if (!UartGetChar(&Uart0, &receiveBuff)) {
            // Now echo the received character
            UartPutChar(&Uart0, receiveBuff);
        }
    }
}

