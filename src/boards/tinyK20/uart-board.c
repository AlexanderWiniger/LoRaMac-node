/**
 * \file uart-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board UART driver implementation
 *
 */

#include "board.h"
#include "uart-board.h"

#define LOG_LEVEL_TRACE
#include "debug.h"

static UART_MemMapPtr g_uartBase[] = UART_BASE_PTRS;
static IRQInterruptIndex g_uartIrq[] = { INT_UART0_RX_TX, INT_UART1_RX_TX, INT_UART2_RX_TX };

void UartMcuInit(Uart_t *obj, uint8_t uartId, PinNames tx, PinNames rx)
{
    obj->UartId = uartId;

    GpioInit(&obj->Tx, tx, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    GpioInit(&obj->Rx, rx, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 1);

    /* Enable clock for UARTx */
    SIM_BASE_PTR->SCGC4 |= (SIM_SCGC4_UART0_MASK << uartId);

    /* Initialize UART */
    g_uartBase[uartId]->BDH = 0U;
    g_uartBase[uartId]->BDL = 4U;
    g_uartBase[uartId]->C1 = 0U;
    g_uartBase[uartId]->C2 = 0U;
    g_uartBase[uartId]->S2 = 0U;
    g_uartBase[uartId]->C3 = 0U;
    g_uartBase[uartId]->C4 = 0U;
    g_uartBase[uartId]->C5 = 0U;
    g_uartBase[uartId]->MODEM = 0U;
    g_uartBase[uartId]->IR = 0U;
    g_uartBase[uartId]->PFIFO = 0U;
    g_uartBase[uartId]->CFIFO = 0U;
    g_uartBase[uartId]->SFIFO = 0xC0U;
    g_uartBase[uartId]->TWFIFO = 0U;
    g_uartBase[uartId]->RWFIFO = 1U;
}

void UartMcuConfig(Uart_t *obj, UartMode_t mode, uint32_t baudrate, WordLength_t wordLength,
        StopBits_t stopBits, Parity_t parity, FlowCtrl_t flowCtrl)
{
    uint16_t sbr, brfa;
    uint32_t uartSourceClock;

    /* UART clock source is either system or bus clock depending on instance */
    uartSourceClock = ((obj->UartId < 2) ? CPU_CORE_CLK_HZ : CPU_BUS_CLK_HZ);

    /* Disable receiver & transmitter before config procedure */
    g_uartBase[obj->UartId]->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

    /*!
     *  Initialize UART baud rate, bit count, parity and stop bit.
     */
    if (parity == NO_PARITY) {
        g_uartBase[obj->UartId]->C1 &= ~(UART_C1_PE_MASK);
    } else {
        if (parity == EVEN_PARITY)
            g_uartBase[obj->UartId]->C1 = (((g_uartBase[obj->UartId]->C1) & ~(UART_C1_PT_MASK))
                    | (UART_C1_PE_MASK));
        else if (parity == ODD_PARITY)
            g_uartBase[obj->UartId]->C1 |= (UART_C1_PE_MASK | UART_C1_PT_MASK);
    }

    /* stopBits configuration is not supported */
    (void) stopBits;

    /* calculate the baud rate modulo divisor, sbr*/
    sbr = uartSourceClock / (baudrate * 16);

    /* check to see if sbr is out of range of register bits */
    if ((sbr > 0x1FFF) || (sbr < 1)) {
        /* unsupported baud rate for given source clock input*/
        LOG_ERROR("Unsupported baud rate!");
        return;
    }

    /* write the sbr value to the BDH and BDL registers*/
    g_uartBase[obj->UartId]->BDH = (((g_uartBase[obj->UartId]->BDH) & (~(UART_BDH_SBR_MASK)))
            | (((uint8_t)(sbr >> 8)) & UART_BDH_SBR_MASK));
    g_uartBase[obj->UartId]->BDL = (((g_uartBase[obj->UartId]->BDL) & (~(UART_BDL_SBR_MASK)))
            | (((uint8_t) sbr) & UART_BDL_SBR_MASK));

    /* determine if a fractional divider is needed to fine tune closer to the
     * desired baud, each value of brfa is in 1/32 increments,
     * hence the multiply-by-32. */
    brfa = (32 * uartSourceClock / (baudrate * 16)) - 32 * sbr;

    /* write the brfa value to the register*/
    g_uartBase[obj->UartId]->C4 = (((g_uartBase[obj->UartId]->C4) & (~(UART_C4_BRFA_MASK)))
            | (((uint8_t) brfa) & UART_C4_BRFA_MASK));

    /* Set word length */
    g_uartBase[obj->UartId]->C1 = (((g_uartBase[obj->UartId]->C1) & (~(UART_C1_M_MASK)))
            | ((wordLength << UART_C1_M_SHIFT) & UART_C1_M_MASK));

    /* Finally, enable the UART transmitter and receiver*/
    if (mode == TX_ONLY) {
        g_uartBase[obj->UartId]->C2 |= UART_C2_TE_MASK; /* Enable transmitter */
    } else if (mode == RX_ONLY) {
        g_uartBase[obj->UartId]->C2 |= UART_C2_RE_MASK; /* Enable receiver interrupt */
        g_uartBase[obj->UartId]->C2 |= UART_C2_RE_MASK; /* Enable receiver */
    } else {
        g_uartBase[obj->UartId]->C2 |= UART_C2_RE_MASK; /* Enable receiver interrupt */
        g_uartBase[obj->UartId]->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK); /* Enable transmitter & receiver */
    }

    /* Enable interrupt for UARTx */
    NVIC_BASE_PTR->ISER[(((uint32_t) (int32_t) g_uartIrq[obj->UartId]) >> 5UL)] = (uint32_t)(
            1UL << (((uint32_t) (int32_t) g_uartIrq[obj->UartId]) & 0x1FUL));
}

void UartMcuDeInit(Uart_t *obj)
{
    /* Wait until the data is completely shifted out of shift register */
    while ((((g_uartBase[obj->UartId]->S1) & UART_S1_TC_MASK) >> UART_S1_TC_SHIFT) != 1) {
    }

    /* Disable the interrupt */
    NVIC_BASE_PTR->ICER[(((uint32_t) (int32_t) g_uartIrq[obj->UartId]) >> 5UL)] = (uint32_t)(
            1UL << (((uint32_t) (int32_t) g_uartIrq[obj->UartId]) & 0x1FUL));

    /* Disable TX and RX */
    g_uartBase[obj->UartId]->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

    /* Gate UART module clock */
    SIM_BASE_PTR->SCGC4 &= ~(SIM_SCGC4_UART0_MASK << obj->UartId);
}

uint8_t UartMcuPutChar(Uart_t *obj, uint8_t data)
{
    if (IsFifoFull(&obj->FifoTx) == false) {
        __disable_irq();
        FifoPush(&obj->FifoTx, data);
        __enable_irq();
        /* Enable the UART Transmit interrupt */
        g_uartBase[obj->UartId]->C2 |= UART_C2_TCIE_MASK;
        return 0; // OK
    }
    return 1; // Busy
}

uint8_t UartMcuGetChar(Uart_t *obj, uint8_t *data)
{
    if (IsFifoEmpty(&obj->FifoRx) == false) {
        __disable_irq();
        *data = FifoPop(&obj->FifoRx);
        __enable_irq();
        return 0; // OK
    }
    return 1; // Busy
}

void UartInterruptHandler(Uart_t *obj)
{
    uint8_t data;

    if (obj == NULL) return;

    /* Transmission finished */
    if (((((g_uartBase[obj->UartId]->S1) & UART_S1_TDRE_MASK) >> UART_S1_TDRE_SHIFT) == 1)
            && ((((g_uartBase[obj->UartId]->S1) & UART_S1_TC_MASK) >> UART_S1_TC_SHIFT) == 1)) {
        if (!IsFifoEmpty(&obj->FifoTx)) {
            data = FifoPop(&obj->FifoTx);
            /* Write one byte to the transmit data register */
            (void) (g_uartBase[obj->UartId]->S1); /* Reset TC flag by reading register S1 */
            g_uartBase[obj->UartId]->D = data;
        } else {
            /* Disable the UART0 Transmit interrupt */
            g_uartBase[obj->UartId]->C2 &= ~(UART_C2_TCIE_MASK);
        }
        if (obj->IrqNotify != NULL) {
            obj->IrqNotify(UART_NOTIFY_TX);
        }
    }

    /* Data received */
    if ((((g_uartBase[obj->UartId]->S1) & UART_S1_OR_MASK) >> UART_S1_OR_SHIFT) == 1) {
        data = g_uartBase[obj->UartId]->D;
    }

    if ((((g_uartBase[obj->UartId]->S1) & UART_S1_RDRF_MASK) >> UART_S1_RDRF_SHIFT) == 1) {
        (void) (g_uartBase[obj->UartId]->S1); /* Reset RDRF flag by reading register S1 */
        data = g_uartBase[obj->UartId]->D;
        if (IsFifoFull(&obj->FifoRx) == false) {
            // Read one byte from the receive data register
            FifoPush(&obj->FifoRx, data);
        }
        if (obj->IrqNotify != NULL) {
            obj->IrqNotify(UART_NOTIFY_RX);
        }
    }
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
