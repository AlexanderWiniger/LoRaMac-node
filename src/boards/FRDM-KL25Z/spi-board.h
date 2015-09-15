/**
 * \file spi-board.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board SPI driver implementation
 *
 */

#ifndef __SPI_MCU_H__
#define __SPI_MCU_H__

/*!
 * SPI driver structure definition
 */
struct Spi_s {
    SPI_TypeDef *Spi;
    Gpio_t Mosi;
    Gpio_t Miso;
    Gpio_t Sclk;
    Gpio_t Nss;
};

#endif  // __SPI_MCU_H__
