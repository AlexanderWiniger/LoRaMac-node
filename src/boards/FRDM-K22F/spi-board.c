/**
 * \file spi-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board SPI driver implementation
 *
 */

#include "board.h"
#include "spi-board.h"
#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"

/*----------------------- Local Definitions ------------------------------*/
/*!
 * MCU SPI peripherals enumeration
 */
typedef enum {
    SPI_0 = (uint32_t) SPI0_BASE, SPI_1 = (uint32_t) SPI1_BASE,
} SPIName;

/*! Number of bytes to be transmitted per transaction */
#define TRANSFER_SIZE   2

void SpiInit(Spi_t *obj, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss)
{
    /* Check if a proper channel was selected */
    if (obj->instance < 0 || obj->instance > SPI_INSTANCE_COUNT) return;

    GpioInit(&obj->Mosi, mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Miso, miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Sclk, sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);

    if (nss != NC) {
        GpioInit(&obj->Nss, nss, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
        obj->isSlave = false;
    } else {
        obj->isSlave = true;
    }

    if (nss == NC) {
        // 8 bits, CPOL = 0, CPHA = 0, MASTER
        SpiFormat(obj, 8, 0, 0, 0);
        ((SPI_MasterType*) obj->Spi)->userConfig.isChipSelectContinuous = false;
        ((SPI_MasterType*) obj->Spi)->userConfig.isSckContinuous = false;
        ((SPI_MasterType*) obj->Spi)->userConfig.pcsPolarity = kDspiPcs_ActiveLow;
        ((SPI_MasterType*) obj->Spi)->userConfig.whichCtar = kDspiCtar0;
        ((SPI_MasterType*) obj->Spi)->userConfig.whichPcs = kDspiPcs0;
        DSPI_DRV_MasterInit(obj->instance, &((SPI_MasterType*) obj->Spi)->state,
                &((SPI_MasterType*) obj->Spi)->userConfig);
    } else {
        // 8 bits, CPOL = 0, CPHA = 0, SLAVE
        SpiFormat(obj, 8, 0, 0, 1);
        DSPI_DRV_SlaveInit(obj->instance, &((SPI_SlaveType*) obj->Spi)->state,
                &((SPI_SlaveType*) obj->Spi)->userConfig);
    }
    SpiFrequency(obj, 10000000);
}

void SpiDeInit(Spi_t *obj)
{
    if (obj->isSlave) DSPI_DRV_SlaveDeinit(obj->instance);
    else DSPI_DRV_MasterDeinit(obj->instance);

    GpioInit(&obj->Mosi, obj->Mosi.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&obj->Miso, obj->Miso.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Sclk, obj->Sclk.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioInit(&obj->Nss, obj->Nss.pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);
}

dspi_status_t SpiInitAsMaster(void)
{
    dspi_master_state_t masterState;
    dspi_master_user_config_t
    masterUserConfig = {
        .isChipSelectContinuous = false,
        .isSckContinuous = false,
        .pcsPolarity = kDspiPcs_ActiveLow,
        .whichCtar = kDspiCtar0,
        .whichPcs = kDspiPcs0
    };

    return DSPI_DRV_MasterInit(RADIO_SPI_INSTANCE, &masterState, &masterUserConfig);
}

dspi_status_t SpiInitAsSlave(void)
{
    return kStatus_DSPI_Success;
}

void SpiFormat(Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave)
{
    dspi_data_format_config_t dataConfig;

    dataConfig.clkPhase = ((cpha != 1) ? kDspiClockPhase_FirstEdge : kDspiClockPhase_SecondEdge);
    dataConfig.clkPolarity = (
            (cpol != 1) ? kDspiClockPolarity_ActiveHigh : kDspiClockPolarity_ActiveLow);
    dataConfig.direction = kDspiMsbFirst;
    dataConfig.bitsPerFrame = bits;

    if (obj->isSlave) {
        ((SPI_SlaveType *) obj->Spi)->userConfig.dataConfig = dataConfig;
    } else {
        ((SPI_MasterType *) obj->Spi)->device.dataBusConfig = dataConfig;
    }
}

void SpiFrequency(Spi_t *obj, uint32_t hz)
{
    uint32_t calculatedBaudRate;

    if (!(obj->isSlave)) {
        ((SPI_MasterType *) obj->Spi)->device.bitsPerSec = hz;
        DSPI_DRV_MasterConfigureBus(obj->instance, &((SPI_MasterType*) obj->Spi)->device,
                &calculatedBaudRate);
    }
}

uint16_t SpiInOut(Spi_t *obj, uint16_t outData)
{
    uint16_t data;
    uint8_t receiveBuffer[TRANSFER_SIZE];
    uint8_t i;
    uint32_t wordsTransfered;

    if ((obj == NULL) || (obj->Spi) == NULL) {
        while (1)
            ;
    }

    // Reset the receive buffer.
    for (i = 0; i < TRANSFER_SIZE; i++) {
        receiveBuffer[i] = 0;
    }

    if (obj->isSlave) {
        // Receive the data.
        DSPI_DRV_SlaveTransfer(obj->instance, NULL, receiveBuffer,
        TRANSFER_SIZE);

        // Wait until the transfer is complete.
        while (DSPI_DRV_SlaveGetTransferStatus(obj->instance, &wordsTransfered) == kStatus_DSPI_Busy) {
        }

        // Transfer the data back to master.
        DSPI_DRV_SlaveTransfer(obj->instance, receiveBuffer, NULL,
        TRANSFER_SIZE);
        // Wait until the transfer is complete.
        while (DSPI_DRV_SlaveGetTransferStatus(obj->instance, &wordsTransfered) == kStatus_DSPI_Busy) {
        }
    } else {
        DSPI_DRV_MasterTransfer(obj->instance, NULL, (uint8_t*) &outData, NULL, TRANSFER_SIZE);
        // Wait until the transfer is complete.
        while (DSPI_DRV_MasterGetTransferStatus(obj->instance, &wordsTransfered)
                == kStatus_DSPI_Busy) {
        }

        DSPI_DRV_MasterTransfer(obj->instance, NULL, NULL, receiveBuffer, TRANSFER_SIZE);
        // Wait until the transfer is complete.
        while (DSPI_DRV_MasterGetTransferStatus(obj->instance, &wordsTransfered)
                == kStatus_DSPI_Busy) {
        }
    }

    data = receiveBuffer[0] || (receiveBuffer[1] << 8);
    return data;
}

