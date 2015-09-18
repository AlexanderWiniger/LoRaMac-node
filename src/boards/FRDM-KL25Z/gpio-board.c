/**
 * \file gpio-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board GPIO driver implementation
 *
 */

#include "board.h"
#include "gpio-board.h"
#include "fsl_gpio_driver.h"
#include "fsl_interrupt_manager.h"

/* Table to save PORT IRQ enumeration numbers defined in CMSIS header file */
extern const IRQn_Type g_portIrqId[PORT_INSTANCE_COUNT];

void GpioMcuInit(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type,
        uint32_t value)
{
	if( pin == NC )
	{
		return;
	}

	obj->portIndex = (uint32_t) pin >> 8;

	obj->pin = pin;
	obj->pinIndex = (0x01 << (obj->pin & 0x1F));

	obj->port = (void*)((PORT_Type *)PORTA_BASE + (obj->portIndex * 0x1000));

	if( mode == PIN_INPUT ) {
		gpio_input_pin_user_config_t inputPinCfg;
		inputPinCfg.pinName = GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex);
		inputPinCfg.config.isPullEnable = (type == PIN_NO_PULL) ? false : true;
		inputPinCfg.config.isPassiveFilterEnabled = false;
		inputPinCfg.config.interrupt = kPortIntDisabled;
		GPIO_DRV_InputPinInit(&inputPinCfg);
	} else if ( mode == PIN_OUTPUT ) {
		gpio_output_pin_user_config_t outputPinCfg;
		outputPinCfg.pinName = GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex);
		outputPinCfg.config.driveStrength = kPortLowDriveStrength;
		outputPinCfg.config.slewRate = kPortSlowSlewRate;
		outputPinCfg.config.outputLogic = value;
		GPIO_DRV_OutputPinInit(&outputPinCfg);
	} else {
		/* Other modes are currently not supported */
		return;
	}
}

void GpioMcuSetInterrupt(Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority,
        GpioIrqHandler *irqHandler)
{
	port_interrupt_config_t interruptCfg;

	if( irqHandler == NULL ) {
		return;
	}
	/* Check if Gpio is configured as input */
	if( GPIO_DRV_GetPinDir(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex)) != 0 ) {
		return;
	}

	interruptCfg = (port_interrupt_config_t)irqMode | 0x08;
	PORT_HAL_SetPinIntMode(obj->port, obj->pinIndex, interruptCfg);

	/* Configure NVIC */
	if ((interruptCfg) && (g_portIrqId[obj->portIndex]))
	{
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
	if( ( obj == NULL ) || ( obj->port == NULL ) )
	{
		while( 1 );
	}
	// Check if pin is not connected
	if( obj->pin == NC )
	{
		return;
	}
	GPIO_DRV_WritePinOutput(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex), value);
}

uint32_t GpioMcuRead(Gpio_t *obj)
{
	if( obj == NULL )
	{
		while( 1 );
	}
	// Check if pin is not connected
	if( obj->pin == NC )
	{
		return 0;
	}
    return GPIO_DRV_ReadPinInput(GPIO_MAKE_PIN(obj->portIndex, obj->pinIndex));
}

void PORTA_IRQHandler(void) {
	// \todo Implement PORTA irq handler
}

void PORTD_IRQHandler(void) {
	// \todo Implement PORTD irq handler
}

