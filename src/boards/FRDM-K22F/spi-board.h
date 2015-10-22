/**
 * \file spi-board.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board SPI driver implementation
 *
 */

#ifndef __SPI_MCU_H__
#define __SPI_MCU_H__

#include "fsl_dspi_master_driver.h"
#include "fsl_dspi_slave_driver.h"

/*!
 * LoRaMac SPI typedef
 */
typedef SPI_Type SPI_TypeDef;

/*!
 *  SPI master driver configuration
 */
typedef struct {
    dspi_device_t device;
    dspi_master_state_t state;
    dspi_master_user_config_t userConfig;
} SPI_MasterDriverType;

/*!
 *  SPI slave driver configuration
 */
typedef struct {
    dspi_slave_state_t state;
    dspi_slave_user_config_t userConfig;
} SPI_SlaveDriverType;

/*!
 * SPI driver structure definition
 */
struct Spi_s {
    uint32_t instance;
    SPI_TypeDef *Spi;
    uint32_t *deviceDriver;
    bool isSlave;
    Gpio_t Mosi;
    Gpio_t Miso;
    Gpio_t Sclk;
    Gpio_t Nss;
};

#endif  // __SPI_MCU_H__
