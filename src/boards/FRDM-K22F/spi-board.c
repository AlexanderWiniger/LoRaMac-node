/**
 * \file spi-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board SPI driver implementation
 *
 */

#include "board.h"
#include "spi-board.h"
#include "fsl_dspi_hal.h"
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
SPI_Type * const g_spiBase[SPI_INSTANCE_COUNT] = SPI_BASE_PTRS;

/*! Table to save port IRQ enum numbers defined in CMSIS files. */
const IRQn_Type g_spiIrqId[SPI_INSTANCE_COUNT] = SPI_IRQS;

void SpiInit(Spi_t *obj, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss)
{
    /* Check if a proper channel was selected */
    if (obj->Spi == NULL) return;

    GpioInit(&obj->Mosi, mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Miso, miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Sclk, sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);

    if (nss != NC) {
        GpioInit(&obj->Nss, nss, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    }

    /* Disable clock for SPI.*/
    if (obj->Spi == g_spiBase[0]) CLOCK_SYS_EnableSpiClock(0);
    else CLOCK_SYS_EnableSpiClock(1);

    // Initialize the DSPI module registers to default value, which disables the module
    DSPI_HAL_Init(obj->Spi);

    if (nss == NC) {
        // 8 bits, CPOL = 0, CPHA = 0, MASTER
        SpiFormat(obj, 8, 0, 0, 0);
    } else {
        // 8 bits, CPOL = 0, CPHA = 0, SLAVE
        SpiFormat(obj, 8, 0, 0, 1);
    }
    SpiFrequency(obj, 10000000);

    /* Enable Spi module */
    DSPI_HAL_Enable(obj->Spi);
}

void SpiDeInit(Spi_t *obj)
{
    /* Disable Spi module */
    DSPI_HAL_Disable(obj->Spi);

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
    dspi_data_format_config_t dataFormat;
    dspi_master_slave_mode_t slaveMode;

    /* Disable Spi module */
    DSPI_HAL_Disable(obj->Spi);

    dataFormat.clkPhase = ((cpha != 1) ? kDspiClockPhase_FirstEdge : kDspiClockPhase_SecondEdge);
    dataFormat.clkPolarity = (
            (cpol != 1) ? kDspiClockPolarity_ActiveHigh : kDspiClockPolarity_ActiveLow);
    dataFormat.direction = kDspiMsbFirst;
    dataFormat.bitsPerFrame = bits;
    slaveMode = ((slave != 1) ? kDspiMaster : kDspiSlave);

    /* Set SPI to master mode */
    DSPI_HAL_SetMasterSlaveMode(obj->Spi, slaveMode);

    // Configure for continuous SCK operation
    DSPI_HAL_SetContinuousSckCmd(obj->Spi, false);
    // Configure for peripheral chip select polarity
    DSPI_HAL_SetPcsPolarityMode(obj->Spi, kDspiPcs0, kDspiPcs_ActiveLow);
    // Disable FIFO operation.
    DSPI_HAL_SetFifoCmd(obj->Spi, false, false);
    // Initialize the configurable delays: PCS-to-SCK, prescaler = 0, scaler = 1
    DSPI_HAL_SetDelay(obj->Spi, kDspiCtar0, 0, 1, kDspiPcsToSck);

    DSPI_HAL_SetDataFormat(obj->Spi, kDspiCtar0, &dataFormat);

    /* Enable Spi module */
    DSPI_HAL_Enable(obj->Spi);
}

void SpiFrequency(Spi_t *obj, uint32_t hz)
{
    uint32_t spiSourceClock;
//    dspi_ctar_selection_t whichCtar;

    if (obj->Spi == g_spiBase[0]) spiSourceClock = CLOCK_SYS_GetSpiFreq(0);
    else spiSourceClock = CLOCK_SYS_GetSpiFreq(1);

    /* Disable Spi module */
    DSPI_HAL_Disable(obj->Spi);

    DSPI_HAL_SetBaudRate(obj->Spi, kDspiCtar0, hz, spiSourceClock);

    /* Enable Spi module */
    DSPI_HAL_Enable(obj->Spi);
}

uint16_t SpiInOut(Spi_t *obj, uint16_t outData)
{
    uint16_t data;
    uint32_t cmd;
    dspi_command_config_t command;

    if ((obj == NULL) || (obj->Spi) == NULL) {
        while (1)
            ;
    }

    /* Before sending the data, we first need to initialize the data command struct
     * Configure the data command attributes for the desired PCS, CTAR, and continuous PCS
     * which are derived from the run-time state struct
     */
    command.whichPcs = kDspiPcs0;
    command.whichCtar = kDspiCtar0;
    command.isChipSelectContinuous = true;
    command.isEndOfQueue = 0;
    command.clearTransferCount = 0;

    /* "Build" the command word. Only do this once since the commad word members don't
     * change until the last word sent (which is when the end of queue flag gets set).
     */
    cmd = DSPI_HAL_GetFormattedCommand(obj->Spi, &command);

    while (SPI_HAL_IsTxBuffEmptyPending(obj->Spi))
        ;
    DSPI_HAL_WriteCmdDataMastermode(obj->Spi, cmd | data);

    while (DSPI_HAL_GetStatusFlag(obj->Spi, kDspiRxFifoDrainRequest) == false) {
    }
    data = DSPI_HAL_ReadData(obj->Spi);
    // Clear RFDR flag
    DSPI_HAL_ClearStatusFlag(obj->Spi, kDspiRxFifoDrainRequest);
    return data;
}

