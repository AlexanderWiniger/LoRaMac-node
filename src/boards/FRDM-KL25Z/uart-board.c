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

}

void UartMcuConfig(Uart_t *obj, UartMode_t mode, uint32_t baudrate,
        WordLength_t wordLength, StopBits_t stopBits, Parity_t parity,
        FlowCtrl_t flowCtrl)
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

/*!
 * UART IRQ handler
 */