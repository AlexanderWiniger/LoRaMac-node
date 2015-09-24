/**
 * \file i2c-board.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board I2C driver implementation
 *
 */

#ifndef __I2C_MCU_H__
#define __I2C_MCU_H__

/*!
 * LoRaMac I2C typedef
 */
typedef I2C_Type I2C_TypeDef;

/*!
 * Operation Mode for the I2C
 */
typedef enum {
    MODE_I2C = 0, MODE_SMBUS_DEVICE, MODE_SMBUS_HOST
} I2cMode;

/*!
 * I2C signal duty cycle
 */
typedef enum {
    I2C_DUTY_CYCLE_2 = 0, I2C_DUTY_CYCLE_16_9
} I2cDutyCycle;

/*!
 * I2C select if the acknowledge in after the 7th or 10th bit
 */
typedef enum {
    I2C_ACK_ADD_7_BIT = 0, I2C_ACK_ADD_10_BIT
} I2cAckAddrMode;

/*!
 * Internal device address size
 */
typedef enum {
    I2C_ADDR_SIZE_7 = 0, I2C_ADDR_SIZE_10,
} I2cAddrSize;

/*!
 * \brief Initializes the I2C object and MCU peripheral
 *
 * \param [IN] obj  I2C object
 * \param [IN] scl  I2C Scl pin name to be used
 * \param [IN] sda  I2C Sda pin name to be used
 */
void I2cMcuInit(I2c_t *obj, PinNames scl, PinNames sda);

/*!
 * \brief Initializes the I2C object and MCU peripheral
 *
 * \param [IN] obj              I2C object
 * \param [IN] mode             Mode of operation for the I2C Bus 
 * \param [IN] dutyCycle        Signal duty cycle
 * \param [IN] I2cAckEnable     Enable or Disable to ack
 * \param [IN] AckAddrMode      7bit or 10 bit addressing
 * \param [IN] I2cFrequency     I2C bus clock frequency
 */
void I2cMcuFormat(I2c_t *obj, I2cMode mode, I2cDutyCycle dutyCycle, bool I2cAckEnable,
        I2cAckAddrMode AckAddrMode, uint32_t I2cFrequency);

/*!
 * \brief DeInitializes the I2C object and MCU peripheral
 *
 * \param [IN] obj  I2C object
 */
void I2cMcuDeInit(I2c_t *obj);

/*!
 * \brief Write several data to the I2C device
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 * \param [IN] addr             register address
 * \param [IN] buffer           data buffer to write
 * \param [IN] size             number of data byte to write
 */
uint8_t I2cMcuWriteBuffer(I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer,
        uint16_t size);

/*!
 * \brief Read several data byte from the I2C device
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 * \param [IN] addr             register address
 * \param [IN] buffer           data buffer used to store the data read
 * \param [IN] size             number of data byte to read
 */
uint8_t I2cMcuReadBuffer(I2c_t *obj, uint8_t deviceAddr, uint16_t addr, uint8_t *buffer,
        uint16_t size);

/*!
 * \brief Waits until the given device is in standby mode
 *
 * \param [IN] obj              I2C object
 * \param [IN] deviceAddr       device address
 */
uint8_t I2cMcuWaitStandbyState(I2c_t *obj, uint8_t deviceAddr);

/*!
 * \brief Sets the internal device address size
 *
 * \param [IN] obj              I2C object
 * \param [IN] addrSize         Internal address size
 */
void I2cSetAddrSize(I2c_t *obj, I2cAddrSize addrSize);

#endif // __I2C_MCU_H__
