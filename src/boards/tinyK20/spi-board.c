/**
 * \file spi-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board SPI driver implementation
 *
 */

#include "board.h"
#include "spi-board.h"
#include <stdlib.h>

/*----------------------- Local Definitions ------------------------------*/
/*!
 * MCU SPI peripherals enumeration
 */
typedef enum {
    SPI_0 = (uint32_t) SPI0_BASE_PTR
} SPIName;

/*! Number of bytes to be transmitted per transaction */
#define TRANSFER_SIZE   2

void SpiInit(Spi_t *obj, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss)
{

}

void SpiDeInit(Spi_t *obj)
{
    GpioInit(&obj->Mosi, obj->Mosi.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&obj->Miso, obj->Miso.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Sclk, obj->Sclk.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&obj->Nss, obj->Nss.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
}

void SpiFormat(Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave)
{
}

void SpiFrequency(Spi_t *obj, uint32_t hz)
{

}

uint16_t SpiInOut(Spi_t *obj, uint16_t outData)
{
    uint16_t data = 0x00;

    if ((obj == NULL) || (obj->Spi) == NULL) {
        while (1)
            ;
    }

    return data;
}

