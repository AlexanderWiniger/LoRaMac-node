/**
 * \file gpio-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board GPIO driver implementation
 *
 */

#include "board.h"
#include "gpio-board.h"

static GpioIrqHandler *GpioIrq[16];

void GpioMcuInit(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config,
        PinTypes type, uint32_t value)
{

}

void GpioMcuSetInterrupt(Gpio_t *obj, IrqModes irqMode,
        IrqPriorities irqPriority, GpioIrqHandler *irqHandler)
{

}

void GpioMcuRemoveInterrupt(Gpio_t *obj)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    GpioIrq[obj->pin & 0x0F] = NULL;

    EXTI_InitStructure.EXTI_Line = (0x01 << (obj->pin & 0x0F));
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void GpioMcuWrite(Gpio_t *obj, uint32_t value)
{

}

uint32_t GpioMcuRead(Gpio_t *obj)
{
    return 0;
}

