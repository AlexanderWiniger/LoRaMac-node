/**
 * \file spi-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board SPI driver implementation
 *
 */

#include "board.h"
#include "spi-board.h"

/*!
 * MCU SPI peripherals enumeration
 */

SPI_InitTypeDef SPI_InitStructure;

void SpiInit(Spi_t *obj, PinNames mosi, PinNames miso, PinNames sclk,
        PinNames nss)
{

}

void SpiDeInit(Spi_t *obj)
{

}

void SpiFormat(Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave)
{

}

void SpiFrequency(Spi_t *obj, uint32_t hz)
{

}

uint16_t SpiInOut(Spi_t *obj, uint16_t outData)
{
    return 0;
}

