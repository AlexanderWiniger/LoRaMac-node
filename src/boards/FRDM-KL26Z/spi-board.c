/**
 * \file spi-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board SPI driver implementation
 *
 */

#include "board.h"
#include "spi-board.h"

/*----------------------- Local Definitions ------------------------------*/
/*!
 * MCU SPI peripherals enumeration
 */
typedef enum {
    SPI_0 = (uint32_t) SPI0_BASE, SPI_1 = (uint32_t) SPI1_BASE,
} SPIName;

/*------------------------ Local Variables -------------------------------*/
/*! Table of base addresses for PORT instances. */
SPI_Type * const g_spiBase[I2C_INSTANCE_COUNT] = SPI_BASE_PTRS;

/*! Table to save port IRQ enum numbers defined in CMSIS files. */
const IRQn_Type g_spiIrqId[I2C_INSTANCE_COUNT] = SPI_IRQS;

void SpiInit(Spi_t *obj, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss)
{
    /* Check if a proper channel was selected */
    if (obj->Spi == NULL) return;

    GpioInit(&obj->Mosi, mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Miso, miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Sclk, sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);

    if (nss != NC) {
        GpioInit(&obj->Nss, nss, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    } else {
        SPI_HAL_SetSlaveSelectOutputMode(obj->Spi, kSpiSlaveSelect_AutomaticOutput);
    }

    /* Disable clock for SPI.*/
    if (obj->Spi == g_spiBase[0]) CLOCK_SYS_EnableSpiClock(0);
    else CLOCK_SYS_EnableSpiClock(1);

    if (nss == NC) {
        // 8 bits, CPOL = 0, CPHA = 0, MASTER
        SpiFormat(obj, 8, 0, 0, 0);
    } else {
        // 8 bits, CPOL = 0, CPHA = 0, SLAVE
        SpiFormat(obj, 8, 0, 0, 1);
    }
    SpiFrequency(obj, 10000000);

    /* Enable Spi module */
    SPI_HAL_Enable(obj->Spi);
}

void SpiDeInit(Spi_t *obj)
{
    /* Disable Spi module */
    SPI_HAL_Disable(obj->Spi);

    /* Disable clock for SPI.*/
    if (obj->Spi == g_spiBase[0]) CLOCK_SYS_DisableSpiClock(0);
    else CLOCK_SYS_DisableSpiClock(1);

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
    return 0;
}

