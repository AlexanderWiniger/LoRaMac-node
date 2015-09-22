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

/* Table to save PORT IRQ enumeration numbers defined in CMSIS header file */
extern const IRQn_Type g_portIrqId[PORT_INSTANCE_COUNT];

void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type,
        uint32_t value ) {

    if ( pin == NC ) {
        return;
    }

    obj->pin = pin;
    obj->pinIndex = (obj->pin & 0x1F);
    obj->portIndex = (uint32_t) pin >> 5; // Divide by 32
    PORT_Type * portBase = PORT_BASE_ADDRESS(obj->portIndex);
    GPIO_Type * gpioBase = GPIO_BASE_ADDRESS(obj->portIndex);
    obj->port = (void*) portBase;

    /* Un-gate port clock*/
    CLOCK_SYS_EnablePortClock(obj->portIndex);

    /* Set current pin as gpio.*/
    PORT_HAL_SetMuxMode(portBase, obj->pinIndex, kPortMuxAsGpio);

    switch (mode) {
    case PIN_INPUT:
        GPIO_HAL_SetPinDir(gpioBase, obj->pinIndex, kGpioDigitalInput);
        PORT_HAL_SetPullCmd(portBase, obj->pinIndex, ((type == PIN_NO_PULL) ? false : true));
        PORT_HAL_SetPassiveFilterCmd(portBase, obj->pinIndex, false); /* Passive filer disabled by default */
        PORT_HAL_SetPinIntMode(portBase, obj->pinIndex, kPortIntDisabled); /* Interrupt disabled by default */
        break;
    case PIN_OUTPUT:
        GPIO_HAL_SetPinDir(gpioBase, obj->pinIndex, kGpioDigitalOutput);
        GPIO_HAL_WritePinOutput(gpioBase, obj->pinIndex, value);
        PORT_HAL_SetSlewRateMode(portBase, obj->pinIndex, kPortSlowSlewRate); /* Slow slew rate by default */
        PORT_HAL_SetDriveStrengthMode(portBase, obj->pinIndex, kPortLowDriveStrength); /* Low drive strength by default*/
        break;
    case PIN_ALTERNATE_FCT:
        /* Pin is used by peripheral device */
        break;
    default:
        /* PIN_ANALOGIC isn't supported by the MCU */
        break;
    }
//    if ( mode == PIN_INPUT ) {
//        gpio_input_pin_user_config_t inputPinCfg;
//        inputPinCfg.pinName = GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex);
//        inputPinCfg.config.isPullEnable = (type == PIN_NO_PULL) ? false : true;
//        inputPinCfg.config.isPassiveFilterEnabled = false;
//        inputPinCfg.config.interrupt = kPortIntDisabled;
//        GPIO_DRV_InputPinInit(&inputPinCfg);
//    } else if ( mode == PIN_OUTPUT ) {
//        /* Open drain configuration isn't supported by KL25Z4 */
//        gpio_output_pin_user_config_t outputPinCfg;
//        outputPinCfg.pinName = GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex);
//        outputPinCfg.config.driveStrength = kPortLowDriveStrength;
//        outputPinCfg.config.slewRate = kPortSlowSlewRate;
//        outputPinCfg.config.outputLogic = value;
//        GPIO_DRV_OutputPinInit(&outputPinCfg);
//    } else {
//        /* Other modes are currently not supported */
//        return;
//    }
}

void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority,
        GpioIrqHandler *irqHandler ) {
    port_interrupt_config_t interruptCfg;

    if ( irqHandler == NULL ) {
        return;
    }
    /* Check if Gpio is configured as input */
    GPIO_Type * gpioBase = GPIO_BASE_ADDRESS(obj->portIndex);
    if ( GPIO_HAL_GetPinDir(gpioBase, obj->pinIndex) != kGpioDigitalInput ) {
//    if ( GPIO_DRV_GetPinDir( GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex)) != 0 ) {
        return;
    }

    interruptCfg = (port_interrupt_config_t) irqMode | 0x08;
    PORT_HAL_SetPinIntMode(obj->port, obj->pinIndex, interruptCfg);

    /* Configure NVIC */
    if ( (interruptCfg) && (g_portIrqId[obj->portIndex]) ) {
        /* Enable GPIO interrupt.*/
        INT_SYS_EnableIRQ(g_portIrqId[obj->portIndex]);
    }

    // \todo Add functionality to define custom GpioIrqHandlers
}

void GpioMcuRemoveInterrupt( Gpio_t *obj ) {
    // \todo Function to remove interrupt
}

void GpioMcuWrite( Gpio_t *obj, uint32_t value ) {
    if ( (obj == NULL) || (obj->port == NULL) ) {
        while (1)
            ;
    }
    // Check if pin is not connected
    if ( obj->pin == NC ) {
        return;
    }
    GPIO_Type * gpioBase = GPIO_BASE_ADDRESS(obj->portIndex);
    GPIO_HAL_WritePinOutput(gpioBase, obj->pinIndex, value);
//    GPIO_DRV_WritePinOutput(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex), value);
}

uint32_t GpioMcuRead( Gpio_t *obj ) {
    if ( obj == NULL ) {
        while (1)
            ;
    }
    // Check if pin is not connected
    if ( obj->pin == NC ) {
        return 0;
    }
    GPIO_Type * gpioBase = GPIO_BASE_ADDRESS(obj->portIndex);
    return GPIO_HAL_ReadPinOutput(gpioBase, obj->pinIndex);
//    return GPIO_DRV_ReadPinInput(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex));
}

void PORTA_IRQHandler( void ) {
    // \todo Implement PORTA irq handler
}

void PORTD_IRQHandler( void ) {
    // \todo Implement PORTD irq handler
}

