/**
 * \file gpio-board.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board GPIO driver implementation
 *
 */

#ifndef __GPIO_MCU_H__
#define __GPIO_MCU_H__

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Table of base addresses for GPIO instances. */
GPIO_Type * const g_gpioBase[GPIO_INSTANCE_COUNT] = GPIO_BASE_PTRS;

/* Table of base addresses for PORT instances. */
PORT_Type * const g_portBase[PORT_INSTANCE_COUNT] = PORT_BASE_PTRS;

/* Table to save port IRQ enum numbers defined in CMSIS files. */
const IRQn_Type g_portIrqId[PORT_INSTANCE_COUNT] = PORT_IRQS;

/*******************************************************************************
 * Alternate Pin Configuration
 ******************************************************************************/
typedef struct {
    PinNames pinName;
    port_mux_t muxConfig;
} gpio_alternate_fct_user_config_t;

extern gpio_alternate_fct_user_config_t alternateFctConfigs[];

/*!
 * \brief Initializes the given GPIO object
 *
 * \param [IN] obj    Pointer to the GPIO object to be initialized
 * \param [IN] pin    Pin name ( please look in pinName-board.h file )
 * \param [IN] mode   Pin mode [PIN_INPUT, PIN_OUTPUT,
 *                              PIN_ALTERNATE_FCT, PIN_ANALOGIC]
 * \param [IN] config Pin config [PIN_PUSH_PULL, PIN_OPEN_DRAIN]
 * \param [IN] type   Pin type [PIN_NO_PULL, PIN_PULL_UP, PIN_PULL_DOWN]
 * \param [IN] value  Default output value at initialisation
 *
 * \remark Open drain configuration isn't supported by KL25Z4
 */
void GpioMcuInit(Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type,
        uint32_t value);

/*!
 * \brief GPIO IRQ Initialization
 *
 * \param [IN] obj         Pointer to the GPIO object to be initialized
 * \param [IN] irqMode     IRQ mode [NO_IRQ, IRQ_RISING_EDGE,
 *                                  IRQ_FALLING_EDGE, IRQ_RISING_FALLING_EDGE]
 * \param [IN] irqPriority IRQ priority [IRQ_VERY_LOW_PRIORITY, IRQ_LOW_PRIORITY
 *                                       IRQ_MEDIUM_PRIORITY, IRQ_HIGH_PRIORITY
 *                                       IRQ_VERY_HIGH_PRIORITY]
 * \param [IN] irqHandler  Callback function pointer
 */
void GpioMcuSetInterrupt(Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority,
        GpioIrqHandler *irqHandler);

/*!
 * \brief GPIO IRQ DeInitialization
 *
 * \param [IN] obj         Pointer to the GPIO object to be Deinitialized
 */
void GpioMcuRemoveInterrupt(Gpio_t *obj);

/*!
 * \brief Writes the given value to the GPIO output
 *
 * \param [IN] obj    Pointer to the GPIO object
 * \param [IN] value  New GPIO output value
 */
void GpioMcuWrite(Gpio_t *obj, uint32_t value);

/*!
 * \brief Reads the current GPIO input value
 *
 * \param [IN] obj    Pointer to the GPIO object
 * \retval value  Current GPIO input value
 */
uint32_t GpioMcuRead(Gpio_t *obj);

#endif // __GPIO_MCU_H__
