/**
 * \file uart-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board UART driver implementation
 *
 */

#include "board.h"
#include "uart-board.h"

void UartMcuInit(Uart_t *obj, uint8_t uartId, PinNames tx, PinNames rx)
{
    obj->UartId = uartId;

    GpioInit(&obj->Tx, tx, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    GpioInit(&obj->Rx, rx, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
}

void UartMcuConfig(Uart_t *obj, UartMode_t mode, uint32_t baudrate, WordLength_t wordLength,
        StopBits_t stopBits, Parity_t parity, FlowCtrl_t flowCtrl)
{

}

void UartMcuDeInit(Uart_t *obj)
{

}

uint8_t UartMcuPutChar(Uart_t *obj, uint8_t data)
{

    return 1; // Busy
}

uint8_t UartMcuGetChar(Uart_t *obj, uint8_t *data)
{

    return 1;
}

void UartInterruptHandler(Uart_t *obj)
{

}

/*!
 * UART0 IRQ handler
 */
void UART0_RX_TX_IRQHandler(void)
{
    UartInterruptHandler (&Uart0);
}

/*!
 * UART1 IRQ handler
 */
void UART1_RX_TX_IRQHandler(void)
{
    UartInterruptHandler (&Uart1);
}

/*!
 * UART2 IRQ handler
 */
void UART2_RX_TX_IRQHandler(void)
{
    UartInterruptHandler (&Uart2);
}
