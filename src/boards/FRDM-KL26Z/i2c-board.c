/**
 * \file i2c-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board I2C driver implementation
 *
 */

#include "board.h"
#include "i2c-board.h"

/*!
 *  The value of the maximal timeout for I2C waiting loops 
 */
#define TIMEOUT_MAX                                 0x8000 

I2cAddrSize I2cInternalAddrSize = I2C_ADDR_SIZE_8;

/*!
 * MCU I2C peripherals enumeration
 */

void I2cMcuInit(I2c_t *obj, PinNames scl, PinNames sda)
{

}

void I2cMcuFormat(I2c_t *obj, I2cMode mode, I2cDutyCycle dutyCycle,
        bool I2cAckEnable, I2cAckAddrMode AckAddrMode, uint32_t I2cFrequency)
{

}

void I2cMcuDeInit(I2c_t *obj)
{

}

void I2cSetAddrSize(I2c_t *obj, I2cAddrSize addrSize)
{

}

uint8_t I2cMcuWriteBuffer(I2c_t *obj, uint8_t deviceAddr, uint16_t addr,
        uint8_t *buffer, uint16_t size)
{
    return 0;
}

uint8_t I2cMcuReadBuffer(I2c_t *obj, uint8_t deviceAddr, uint16_t addr,
        uint8_t *buffer, uint16_t size)
{
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

uint8_t I2cMcuWaitStandbyState(I2c_t *obj, uint8_t deviceAddr)
{
    return 0;
}
