/**
 * \file uart-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board UART driver implementation
 *
 */

#include "board.h"
#include "uart-board.h"
#include "fsl_uart_hal.h"
#include "fsl_lpsci_hal.h"
#include "fsl_clock_manager.h"

void UartMcuInit(Uart_t *obj, uint8_t uartId, PinNames tx, PinNames rx)
{
	obj->UartId = uartId;

	PORT_HAL_SetMuxMode(PORTA, 1, kPortMuxAlt2);
	PORT_HAL_SetMuxMode(PORTA, 2, kPortMuxAlt2);

	// Select different clock source for LPSCI. */
#if (CLOCK_INIT_CONFIG == CLOCK_VLPR)
	CLOCK_SYS_SetLpsciSrc(uartId, kClockLpsciSrcMcgIrClk);
#else
	CLOCK_SYS_SetLpsciSrc(uartId, kClockLpsciSrcPllFllSel);
#endif
}

void UartMcuConfig(Uart_t *obj, UartMode_t mode, uint32_t baudrate,
        WordLength_t wordLength, StopBits_t stopBits, Parity_t parity,
        FlowCtrl_t flowCtrl)
{
	UART_Type * g_Base[UART_INSTANCE_COUNT] = UART_BASE_PTRS;
	UART_Type * base = g_Base[obj->UartId];
	uint32_t uartSourceClock;

	if( obj->UartId == UART_0 )
    {
        CLOCK_SYS_EnableLpsciClock(obj->UartId);

        uartSourceClock = CLOCK_SYS_GetLpsciFreq(obj->UartId);

        lpsci_parity_mode_t parityMode;
        if( parity == NO_PARITY ) parityMode = kLpsciParityDisabled;
        else if( parity == EVEN_PARITY ) parityMode = kLpsciParityEven;
        else if( parity == ODD_PARITY ) parityMode = kLpsciParityOdd;
        else parityMode = kLpsciParityDisabled;

        lpsci_stop_bit_count_t stopBitCnt;
        if( stopBits == UART_1_STOP_BIT ) stopBitCnt = kLpsciOneStopBit;
		else if( stopBits == UART_2_STOP_BIT ) stopBitCnt = kLpsciTwoStopBit;
		else stopBitCnt = kLpsciOneStopBit;

        /* Initialize LPSCI baud rate, bit count, parity and stop bit. */
        LPSCI_HAL_SetBaudRate(base, uartSourceClock, baudrate);
        LPSCI_HAL_SetBitCountPerChar(base, (lpsci_bit_count_per_char_t)wordLength);
        LPSCI_HAL_SetParityMode(base, parityMode);
#if FSL_FEATURE_LPSCI_HAS_STOP_BIT_CONFIG_SUPPORT
        LPSCI_HAL_SetStopBitCount(base, stopBitCnt);
#endif

        /* Finally, enable the LPSCI transmitter and receiver*/
        if( mode == TX_ONLY ) {
        	LPSCI_HAL_EnableTransmitter(base);
        } else if( mode == RX_ONLY ) {
        	LPSCI_HAL_EnableReceiver(base);
        } else {
        	LPSCI_HAL_EnableTransmitter(base);
        	LPSCI_HAL_EnableReceiver(base);
        }
    } else {
		CLOCK_SYS_EnableUartClock(obj->UartId);

		/* UART clock source is either system or bus clock depending on instance */
		uartSourceClock = CLOCK_SYS_GetUartFreq(obj->UartId);

		uart_parity_mode_t parityMode;
		if( parity == NO_PARITY ) parityMode = kUartParityDisabled;
		else if( parity == EVEN_PARITY ) parityMode = kUartParityEven;
		else if( parity == ODD_PARITY ) parityMode = kUartParityOdd;
		else parityMode = kUartParityDisabled;

		uart_stop_bit_count_t stopBitCnt;
		if( stopBits == UART_1_STOP_BIT ) stopBitCnt = kUartOneStopBit;
		else if( stopBits == UART_2_STOP_BIT ) stopBitCnt = kUartTwoStopBit;
		else stopBitCnt = kUartOneStopBit;

		/* Initialize UART baud rate, bit count, parity and stop bit. */
		UART_HAL_SetBaudRate(base, uartSourceClock, baudrate);
		UART_HAL_SetBitCountPerChar(base, (uart_bit_count_per_char_t)wordLength);
		UART_HAL_SetParityMode(base, parityMode);
#if FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT
		UART_HAL_SetStopBitCount(base, stopBitCnt);
#endif

		/* Finally, enable the UART transmitter and receiver*/
		if( mode == TX_ONLY ) {
			UART_HAL_EnableTransmitter(base);
		} else if( mode == RX_ONLY ) {
			UART_HAL_EnableReceiver(base);
		} else {
			UART_HAL_EnableTransmitter(base);
			UART_HAL_EnableReceiver(base);
		}
    }
}

void UartMcuDeInit(Uart_t *obj)
{
	/* Declare config sturcuture to initialize a uart instance. */
	UART_Type * g_Base[UART_INSTANCE_COUNT] = UART_BASE_PTRS;
	UART_Type * base = g_Base[obj->UartId];

	if( obj->UartId == UART_0 )
	{
		CLOCK_SYS_DisableLpsciClock(obj->UartId);

		LPSCI_HAL_DisableTransmitter(base);
		LPSCI_HAL_DisableReceiver(base);
	} else {
		CLOCK_SYS_DisableUartClock(obj->UartId);

		UART_HAL_DisableTransmitter(base);
		UART_HAL_DisableReceiver(base);
	}
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
