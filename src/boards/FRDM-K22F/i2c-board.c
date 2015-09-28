/**
 * \file i2c-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board I2C driver implementation
 *
 */

#include "board.h"
#include "i2c-board.h"
#include "fsl_i2c_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"

/*----------------------- Local Definitions ------------------------------*/
/*!
 *  The value of the maximal timeout for I2C waiting loops 
 */
#define TIMEOUT_MAX                                 0x8000 

/*!
 * MCU I2C peripherals enumeration
 */
typedef enum {
    I2C_0 = (uint32_t) I2C0_BASE, I2C_1 = (uint32_t) I2C1_BASE
} I2cName;

/*------------------------ Local Variables -------------------------------*/
/*! Table of base addresses for PORT instances. */
I2C_Type * const g_i2cBase[I2C_INSTANCE_COUNT] = I2C_BASE_PTRS;

/*! Table to save port IRQ enum numbers defined in CMSIS files. */
const IRQn_Type g_i2cIrqId[I2C_INSTANCE_COUNT] = I2C_IRQS;

I2cAddrSize I2cInternalAddrSize = I2C_ADDR_SIZE_8;

void I2cMcuInit( I2c_t *obj, PinNames scl, PinNames sda ) {
    /* Check if a proper channel was selected */
    if ( obj->I2c == NULL ) return;

    /* Enable clock for I2C.*/
    if ( obj->I2c == g_i2cBase[0] ) CLOCK_SYS_EnableI2cClock(0);
    else CLOCK_SYS_EnableI2cClock(1);

    /* Initialize peripheral to known state.*/
    I2C_HAL_Init(obj->I2c);

    GpioInit(&obj->Scl, scl, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
    GpioInit(&obj->Sda, sda, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
}

void I2cMcuFormat( I2c_t *obj, I2cMode mode, I2cDutyCycle dutyCycle, bool I2cAckEnable,
        I2cAckAddrMode AckAddrMode, uint32_t I2cFrequency ) {
    uint32_t i2cClockFreq, baudRate_kbps;

    if ( I2cFrequency > 400000 ) {
        baudRate_kbps = 400;
    } else {
        baudRate_kbps = I2cFrequency / 1000;
    }

    /* Get the current bus clock.*/
    if ( obj->I2c == g_i2cBase[0] ) i2cClockFreq = CLOCK_SYS_GetI2cFreq(0);
    else i2cClockFreq = CLOCK_SYS_GetI2cFreq(1);

    I2C_HAL_SetBaudRate(obj->I2c, i2cClockFreq, baudRate_kbps, NULL);

    /* I2C Peripheral Enable */
    I2C_HAL_Enable(obj->I2c);
}

void I2cMcuDeInit( I2c_t *obj ) {
    /* Disable module.*/
    I2C_HAL_Disable(obj->I2c);

    /* Disable clock for I2C.*/
    if ( obj->I2c == g_i2cBase[0] ) CLOCK_SYS_DisableI2cClock(0);
    else CLOCK_SYS_DisableI2cClock(1);

    GpioInit(&obj->Scl, obj->Scl.pin, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&obj->Sda, obj->Sda.pin, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
}

void I2cSetAddrSize( I2c_t *obj, I2cAddrSize addrSize ) {
    I2cInternalAddrSize = addrSize;
}

uint8_t I2cMcuWriteBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer,
        uint16_t size ) {
    return 0;
}

uint8_t I2cMcuReadBuffer( I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer,
        uint16_t size ) {
    return 0;
}

/* Maximum Timeout values for flags and events waiting loops. These timeouts are
 not based on accurate values, they just guarantee that the application will 
 not remain stuck if the I2C communication is corrupted.
 You may modify these timeout values depending on CPU frequency and application
 conditions (interrupts routines ...). */
#define EE_FLAG_TIMEOUT         ( ( uint32_t )0x1000 )
#define EE_LONG_TIMEOUT         ( ( uint32_t )( 10 * EE_FLAG_TIMEOUT ) )

/* Maximum number of trials for I2cMcuWaitStandbyState( ) function */
#define EE_MAX_TRIALS_NUMBER     300

uint8_t I2cMcuWaitStandbyState( I2c_t *obj, uint8_t deviceAddr ) {
    return 0;
}
