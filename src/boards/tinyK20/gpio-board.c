/**
 * \file gpio-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board GPIO driver implementation
 *
 */

#include "board.h"
#include "gpio-board.h"

/*----------------------- Local Definitions ------------------------------*/
/*!
 * Number of Pins per Port
 */
#define NR_OF_PINS_PORTA        5
#define NR_OF_PINS_PORTB        6
#define NR_OF_PINS_PORTAB       (NR_OF_PINS_PORTA+NR_OF_PINS_PORTB)
#define NR_OF_PINS_PORTC        8
#define NR_OF_PINS_PORTABC      (NR_OF_PINS_PORTA+NR_OF_PINS_PORTB+NR_OF_PINS_PORTC)
#define NR_OF_PINS_PORTD        8
#define NR_OF_PINS_PORTABCD     (NR_OF_PINS_PORTA+NR_OF_PINS_PORTB+NR_OF_PINS_PORTC+NR_OF_PINS_PORTD)
/*------------------------ Local Variables -------------------------------*/
/*! Available pins for port A */
uint8_t pinsPortA[] = { 0, 1, 2, 3, 4 };
/*! Available pins for port B */
uint8_t pinsPortB[] = { 0, 1, 2, 3, 16, 17 };
/*! Available pins for port C */
uint8_t pinsPortC[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
/*! Available pins for port D */
uint8_t pinsPortD[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

/*! GPIO irq handler port A */
static GpioIrqHandler *GpioAIrq[5];

/*! GPIO irq handler port B */
static GpioIrqHandler *GpioBIrq[18];

/*! GPIO irq handler port C */
static GpioIrqHandler *GpioCIrq[8];

/*! GPIO irq handler port D */
static GpioIrqHandler *GpioDIrq[8];

void GpioMcuInit(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type,
        uint32_t value)
{

    if (pin == NC) {
        return;
    }

    if (pin < NR_OF_PINS_PORTA) {
        obj->pinIndex = pinsPortA[pin];
        obj->portIndex = 0;
    } else if (pin < NR_OF_PINS_PORTAB) {
        obj->pinIndex = pinsPortB[(pin - NR_OF_PINS_PORTA)];
        obj->portIndex = 1;
    } else if (pin < NR_OF_PINS_PORTABC) {
        obj->pinIndex = pinsPortC[(pin - NR_OF_PINS_PORTAB)];
        obj->portIndex = 2;
    } else if (pin < NR_OF_PINS_PORTABCD) {
        obj->pinIndex = pinsPortD[(pin - NR_OF_PINS_PORTABC)];
        obj->portIndex = 3;
    } else {
        return;
    }

    obj->pin = pin;
//    obj->port = (void*) g_portBase[obj->portIndex];

    switch (mode) {
        case PIN_INPUT:

            break;
        case PIN_OUTPUT:

            break;
        case PIN_ALTERNATE_FCT:
        {

            break;
        }
        case PIN_ANALOGIC:

            break;
        default:
            /* Nothing to do */
            break;
    }
}

void GpioMcuSetInterrupt(Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority,
        GpioIrqHandler *irqHandler)
{
    if (irqHandler == NULL) {
        return;
    }

}

void GpioMcuRemoveInterrupt(Gpio_t *obj)
{
    if (obj->portIndex == 0)
        GpioAIrq[obj->pinIndex & 0x1F] = NULL;
    else if (obj->portIndex == 1)
        GpioBIrq[obj->pinIndex & 0x1F] = NULL;
    else if (obj->portIndex == 2)
        GpioCIrq[obj->pinIndex & 0x0F] = NULL;
    else if (obj->portIndex == 3)
        GpioDIrq[obj->pinIndex & 0x07] = NULL;
    else
        return;

}

void GpioMcuWrite(Gpio_t *obj, uint32_t value)
{
    if ((obj == NULL) || (obj->port == NULL)) {
        while (1)
            ;
    }
    // Check if pin is not connected
    if (obj->pin == NC) {
        return;
    }

}

uint32_t GpioMcuRead(Gpio_t *obj)
{
    if (obj == NULL) {
        while (1)
            ;
    }
    // Check if pin is not connected
    if (obj->pin == NC) {
        return 0;
    }
    return 0;
}

void PORTA_IRQHandler(void)
{
    uint32_t pendingInt = PORTA_ISFR;

    /* Clear interrupt flag.*/
    PORTA_ISFR = ~0u;

    if (pendingInt & 0x1)
        GpioAIrq[0]();
    else if ((pendingInt & (1U << 1)) >> 1)
        GpioAIrq[1]();
    else if ((pendingInt & (1U << 2)) >> 2)
        GpioAIrq[2]();
    else if ((pendingInt & (1U << 3)) >> 3)
        GpioAIrq[3]();
    else if ((pendingInt & (1U << 4)) >> 4) GpioAIrq[4]();
    /* PTA5 to PTA31 can't be used as GPIO */
}

void PORTB_IRQHandler(void)
{
    uint32_t pendingInt = PORTB_ISFR;

    /* Clear interrupt flag.*/
    PORTB_ISFR = ~0u;

    if (pendingInt & 0x1)
        GpioBIrq[0]();
    else if ((pendingInt & (1U << 1)) >> 1)
        GpioBIrq[1]();
    else if ((pendingInt & (1U << 2)) >> 2)
        GpioBIrq[2]();
    else if ((pendingInt & (1U << 3)) >> 3)
        GpioBIrq[3]();
    /* PTB4 to PTB15 can't be used as GPIO */
    else if ((pendingInt & (1U << 16)) >> 16)
        GpioBIrq[16]();
    else if ((pendingInt & (1U << 17)) >> 17) GpioBIrq[17]();
    /* PTB18 to PTB31 can't be used as GPIO */
}

void PORTC_IRQHandler(void)
{
    uint32_t pendingInt = PORTC_ISFR;

    /* Clear interrupt flag.*/
    PORTC_ISFR = ~0u;

    if (pendingInt & 0x1)
        GpioCIrq[0]();
    else if ((pendingInt & (1U << 1)) >> 1)
        GpioCIrq[1]();
    else if ((pendingInt & (1U << 2)) >> 2)
        GpioCIrq[2]();
    else if ((pendingInt & (1U << 3)) >> 3)
        GpioCIrq[3]();
    else if ((pendingInt & (1U << 4)) >> 4)
        GpioCIrq[4]();
    else if ((pendingInt & (1U << 5)) >> 5)
        GpioCIrq[5]();
    else if ((pendingInt & (1U << 6)) >> 6)
        GpioCIrq[6]();
    else if ((pendingInt & (1U << 7)) >> 7) GpioCIrq[7]();
    /* PTC8 to PTC31 can't be used as GPIO */
}

void PORTD_IRQHandler(void)
{
    uint32_t pendingInt = PORTD_ISFR;

    /* Clear interrupt flag.*/
    PORTD_ISFR = ~0u;

    if (pendingInt & 0x1)
        GpioDIrq[1]();
    else if ((pendingInt & (1U << 0)) >> 0)
        GpioDIrq[0]();
    else if ((pendingInt & (1U << 1)) >> 1)
        GpioDIrq[1]();
    else if ((pendingInt & (1U << 2)) >> 2)
        GpioDIrq[2]();
    else if ((pendingInt & (1U << 3)) >> 3)
        GpioDIrq[3]();
    else if ((pendingInt & (1U << 4)) >> 4)
        GpioDIrq[4]();
    else if ((pendingInt & (1U << 5)) >> 5)
        GpioDIrq[5]();
    else if ((pendingInt & (1U << 6)) >> 6)
        GpioDIrq[6]();
    else if ((pendingInt & (1U << 7)) >> 7) GpioDIrq[7]();
    /* PTD8 to PTD31 can't be used as GPIO */
}

