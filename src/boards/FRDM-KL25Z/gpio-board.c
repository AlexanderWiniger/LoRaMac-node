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

void GpioMcuInit(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type,
        uint32_t value)
{

}

void GpioMcuSetInterrupt(Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority,
        GpioIrqHandler *irqHandler)
{

}

void GpioMcuRemoveInterrupt(Gpio_t *obj)
{

}

void GpioMcuWrite(Gpio_t *obj, uint32_t value)
{

}

uint32_t GpioMcuRead(Gpio_t *obj)
{
    return 0;
}

