/**
 * \file utlities.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 28.09.2015
 * \brief Target mcu utilities
 *
 */

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#if defined(CPU_MKL26Z128VLH4) || defined(CPU_MK22FN512VLH12)
#include "fsl_i2c_master_driver.h"

/*!
 * LoRaMac specific ADC typedef
 */
typedef ADC_Type ADC_TypeDef;

/*!
 * LoRaMac specific I2C typedef
 */
typedef struct {
    uint32_t instance;
    i2c_device_t slave;
    i2c_master_state_t state;
} I2C_TypeDef;
#else
typedef uint32_t ADC_TypeDef;
typedef uint32_t I2C_TypeDef;
#endif

/*!
 * \brief Returns the minimum value betwen a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval minValue Minimum value
 */
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )

/*!
 * \brief Returns the maximum value betwen a and b
 *
 * \param [IN] a 1st value
 * \param [IN] b 2nd value
 * \retval maxValue Maximum value
 */
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )

/*!
 * \brief Returns 2 raised to the power of n
 *
 * \param [IN] n power value
 * \retval result of raising 2 to the power n
 */
#define POW2( n ) ( 1 << n )

/*!
 * \brief Initializes the pseudo ramdom generator initial value
 *
 * \param [IN] seed Pseudo ramdom generator initial value
 */
void srand1(uint32_t seed);

/*!
 * \brief Computes a random number between min and max
 *
 * \param [IN] min range minimum value
 * \param [IN] max range maximum value
 * \retval random random value in range min..max
 */
int32_t randr(int32_t min, int32_t max);

/*!
 * \brief Copies size elements of src array to dst array
 *
 * \remark STM32 Standard memcpy function only works on pointers that are aligned
 *
 * \param [OUT] dst  Destination array
 * \param [IN]  src  Source array
 * \param [IN]  size Number of bytes to be copied
 */
void memcpy1(uint8_t *dst, const uint8_t *src, uint16_t size);

/*!
 * \brief Set size elements of dst array with value
 *
 * \remark STM32 Standard memset function only works on pointers that are aligned
 *
 * \param [OUT] dst   Destination array
 * \param [IN]  value Default value
 * \param [IN]  size  Number of bytes to be copied
 */
void memset1(uint8_t *dst, uint8_t value, uint16_t size);

/*!
 * \brief Converts a nibble to an hexadecimal character
 *
 * \param [IN] a   Nibble to be converted
 * \retval hexChar Converted hexadecimal character
 */
int8_t Nibble2HexChar(uint8_t a);
#endif /* __UTILITIES_H__ */
