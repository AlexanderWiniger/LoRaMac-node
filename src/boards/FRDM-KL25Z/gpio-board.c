/**
 * \file gpio-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board GPIO driver implementation
 *
 */

#include "board.h"
#include "gpio-board.h"
#include "fsl_gpio_hal.h"
#include "fsl_port_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * \remark  For chip-specific alternate function see KL25 sub-family data sheet
 *          p. 44 - KL25 Signal Multiplexing and Pin Assignments.
 */
gpio_alternate_fct_user_config_t alternateFctConfigs[] = {
    {
        .pinName = PA_1,
        .muxConfig = kPortMuxAlt2, ///> UART0_RX
    },
    {
        .pinName = PA_2,
        .muxConfig = kPortMuxAlt2, ///> UART0_TX
    },
    {
        .pinName = PD_0,
        .muxConfig = kPortMuxAlt2, ///> SPI0_PCS0
    },
    {
        .pinName = PD_1,
        .muxConfig = kPortMuxAlt2, ///> SPI0_SCK
    },
    {
        .pinName = PD_2,
        .muxConfig = kPortMuxAlt2, ///> SPI0_MOSI
    },
    {
        .pinName = PD_3,
        .muxConfig = kPortMuxAlt2, ///> SPI0_MISO
    },
    {
        .pinName = PE_0,
        .muxConfig = kPortMuxAlt4, ///> RTC_CLKOUT
    },
    {
        .pinName = PE_24,
        .muxConfig = kPortMuxAlt5, ///> I2C0_SCL
    },
    {
        .pinName = PE_25,
        .muxConfig = kPortMuxAlt5, ///> I2C0_SDA
    },
    {
        .pinName = PIN_OUT_OF_RANGE,
    }
};

/* Table to save PORT IRQ enumeration numbers defined in CMSIS header file */
extern const IRQn_Type g_portIrqId[PORT_INSTANCE_COUNT];

void GpioMcuInit(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type,
        uint32_t value)
{

    if (pin == NC) {
        return;
    }

    obj->pin = pin;
    obj->pinIndex = (obj->pin & 0x1F);
    obj->portIndex = (uint32_t) pin >> 5;          // Divide by 32
    PORT_Type * portBase = PORT_BASE_ADDRESS(obj->portIndex);
    GPIO_Type * gpioBase = GPIO_BASE_ADDRESS(obj->portIndex);
    obj->port = (void*) portBase;

    /* Un-gate port clock*/
    CLOCK_SYS_EnablePortClock(obj->portIndex);

    switch (mode) {
        case PIN_INPUT:
            /* Set current pin as gpio.*/
            PORT_HAL_SetMuxMode(portBase, obj->pinIndex, kPortMuxAsGpio); // Configure pin muxing to gpio
            GPIO_HAL_SetPinDir(gpioBase, obj->pinIndex, kGpioDigitalInput); // Set pin direction to input
            PORT_HAL_SetPullCmd(portBase, obj->pinIndex, ((type == PIN_NO_PULL) ? false : true)); // Enable/Disable internal pull resistor
            break;
        case PIN_OUTPUT:
            PORT_HAL_SetMuxMode(portBase, obj->pinIndex, kPortMuxAsGpio); // Configure pin muxing to gpio
            GPIO_HAL_WritePinOutput(gpioBase, obj->pinIndex, value);     // Set default output level
            GPIO_HAL_SetPinDir(gpioBase, obj->pinIndex, kGpioDigitalOutput); // Set pin direction to output
            break;
        case PIN_ALTERNATE_FCT:
        {
            int i = 0;
            while (alternateFctConfigs[i].pinName != PIN_OUT_OF_RANGE) {
                if (alternateFctConfigs[i].pinName == pin) {
                    PORT_HAL_SetMuxMode(portBase, obj->pinIndex, alternateFctConfigs[i].muxConfig); // Configure alternate function muxing to gpio
                    break;
                }
                i++;
            }
            break;
        }
        case PIN_ANALOGIC:
            PORT_HAL_SetMuxMode(portBase, obj->pinIndex, kPortPinDisabled); // Set pin as disabled, but is used as an analog pin
            break;
        default:
            /* Nothing to do */
            break;
    }
}

void GpioMcuSetInterrupt(Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority,
        GpioIrqHandler *irqHandler)
{
    port_interrupt_config_t interruptCfg;

    if (irqHandler == NULL) {
        return;
    }
    /* Check if Gpio is configured as input */
    GPIO_Type * gpioBase = GPIO_BASE_ADDRESS(obj->portIndex);
    if (GPIO_HAL_GetPinDir(gpioBase, obj->pinIndex) != kGpioDigitalInput) {
//    if ( GPIO_DRV_GetPinDir( GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex)) != 0 ) {
        return;
    }

    interruptCfg = (port_interrupt_config_t) irqMode | 0x08;
    PORT_HAL_SetPinIntMode(obj->port, obj->pinIndex, interruptCfg);

    /* Configure NVIC */
    if ((interruptCfg) && (g_portIrqId[obj->portIndex])) {
        /* Enable GPIO interrupt.*/
        INT_SYS_EnableIRQ(g_portIrqId[obj->portIndex]);
    }

    // \todo Add functionality to define custom GpioIrqHandlers
}

void GpioMcuRemoveInterrupt(Gpio_t *obj)
{
    // \todo Function to remove interrupt
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
    GPIO_Type * gpioBase = GPIO_BASE_ADDRESS(obj->portIndex);
    GPIO_HAL_WritePinOutput(gpioBase, obj->pinIndex, value);
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
    GPIO_Type * gpioBase = GPIO_BASE_ADDRESS(obj->portIndex);
    return GPIO_HAL_ReadPinOutput(gpioBase, obj->pinIndex);
}

void PORTA_IRQHandler(void)
{
    // \todo Implement PORTA irq handler
}

void PORTD_IRQHandler(void)
{
    // \todo Implement PORTD irq handler
}

