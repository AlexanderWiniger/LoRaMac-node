/**
 * \file spi-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board SPI driver implementation
 *
 */

#include "board.h"
#include "spi-board.h"
#include "fsl_spi_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"

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

    /* Enable clock for SPI.*/
    if (obj->Spi == g_spiBase[0]) CLOCK_SYS_EnableSpiClock(0);
    else CLOCK_SYS_EnableSpiClock(1);

    // Initialize the SPI module registers to default value, which disables the module
    SPI_HAL_Init(obj->Spi);

    if (nss != NC) {
        GpioInit(&obj->Nss, nss, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
        SPI_HAL_SetSlaveSelectOutputMode(obj->Spi, kSpiSlaveSelect_AutomaticOutput);
    } else {
        SPI_HAL_SetSlaveSelectOutputMode(obj->Spi, kSpiSlaveSelect_AsGpio);
    }

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
    spi_master_slave_mode_t slaveMode;
    spi_clock_polarity_t clockPolarity;
    spi_clock_phase_t clockPhase;
    spi_data_bitcount_mode_t bitCount;

    /* Disable Spi module */
    SPI_HAL_Disable(obj->Spi);

    slaveMode = ((slave != 1) ? kSpiMaster : kSpiSlave);
    clockPolarity = ((cpol != 1) ? kSpiClockPolarity_ActiveHigh : kSpiClockPolarity_ActiveLow);
    clockPhase = ((cpha != 1) ? kSpiClockPhase_FirstEdge : kSpiClockPhase_SecondEdge);
    bitCount = ((bits != 8) ? kSpi16BitMode : kSpi8BitMode);

    /* Set SPI to master mode */
    SPI_HAL_SetMasterSlave(obj->Spi, slaveMode);

    SPI_HAL_SetDataFormat(obj->Spi, clockPolarity, clockPhase, kSpiMsbFirst);
    SPI_HAL_Set8or16BitMode(obj->Spi, bitCount);

    /* Enable Spi module */
    SPI_HAL_Enable(obj->Spi);
}

void SpiFrequency(Spi_t *obj, uint32_t hz)
{
    uint32_t spiSourceClock;

    if (obj->Spi == g_spiBase[0]) spiSourceClock = CLOCK_SYS_GetSpiFreq(0);
    else spiSourceClock = CLOCK_SYS_GetSpiFreq(1);

    /* Disable Spi module */
    SPI_HAL_Disable(obj->Spi);

    SPI_HAL_SetBaud(obj->Spi, hz, spiSourceClock);

    /* Enable Spi module */
    SPI_HAL_Enable(obj->Spi);
}

uint16_t SpiInOut(Spi_t *obj, uint16_t outData)
{
    uint16_t data;
    if ((obj == NULL) || (obj->Spi) == NULL) {
        while (1)
            ;
    }

    while (SPI_HAL_IsTxBuffEmptyPending(obj->Spi))
        ;
    SPI_HAL_WriteDataHigh(obj->Spi, ((outData & 0xFF00) >> 8));
    SPI_HAL_WriteDataLow(obj->Spi, (outData & 0xFF));
    while (SPI_HAL_IsReadBuffFullPending(obj->Spi))
        ;
    data = SPI_HAL_ReadDataHigh(obj->Spi) << 8;
    data |= SPI_HAL_ReadDataLow(obj->Spi);
    while (SPI_HAL_IsMatchPending(obj->Spi))
        ;
    SPI_HAL_ClearMatchFlag(obj->Spi);
    return data;
}

