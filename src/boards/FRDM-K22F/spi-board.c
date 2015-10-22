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
#include "fsl_dspi_master_driver.h"
#include "fsl_dspi_slave_driver.h"
#include "fsl_interrupt_manager.h"
#include <stdlib.h>

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
    if (g_dspiBase[obj->instance] == NULL) return;

    obj->Spi = g_dspiBase[obj->instance];

    GpioInit(&obj->Mosi, mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Miso, miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Sclk, sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Nss, nss, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_UP, 1);

    if (!obj->isSlave) {
        obj->deviceDriver = malloc(sizeof(SPI_MasterDriverType));
        // 8 bits, CPOL = 0, CPHA = 0, MASTER
        SpiFormat(obj, 8, 0, 0, 0);
        SPI_MasterDriverType * master = ((SPI_MasterDriverType *) obj->deviceDriver);
        master->userConfig.isChipSelectContinuous = false;
        master->userConfig.isSckContinuous = false;
        master->userConfig.pcsPolarity = kDspiPcs_ActiveLow;
        master->userConfig.whichCtar = kDspiCtar0;
        master->userConfig.whichPcs = kDspiPcs0;
        DSPI_DRV_MasterInit(obj->instance, &master->state, &master->userConfig);
    } else {
        obj->Spi = malloc(sizeof(SPI_SlaveDriverType));
        // 8 bits, CPOL = 0, CPHA = 0, SLAVE
        SpiFormat(obj, 8, 0, 0, 1);
        DSPI_DRV_SlaveInit(obj->instance, &((SPI_SlaveDriverType*) obj->deviceDriver)->state,
                &((SPI_SlaveDriverType*) obj->deviceDriver)->userConfig);
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

void SpiFormat(Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave)
{
    dspi_data_format_config_t dataConfig;

    dataConfig.clkPhase = ((cpha != 1) ? kDspiClockPhase_FirstEdge : kDspiClockPhase_SecondEdge);
    dataConfig.clkPolarity = (
            (cpol != 1) ? kDspiClockPolarity_ActiveHigh : kDspiClockPolarity_ActiveLow);
    dataConfig.direction = kDspiMsbFirst;
    dataConfig.bitsPerFrame = bits;

    if (obj->isSlave) {
        ((SPI_SlaveDriverType *) obj->deviceDriver)->userConfig.dataConfig = dataConfig;
    } else {
        SPI_MasterDriverType * master = ((SPI_MasterDriverType *) obj->deviceDriver);
        master->device.dataBusConfig = dataConfig;
    }
}

void SpiFrequency(Spi_t *obj, uint32_t hz)
{
    uint32_t calculatedBaudRate;

    if (!(obj->isSlave)) {
        SPI_MasterDriverType * master = ((SPI_MasterDriverType *) obj->deviceDriver);
        master->device.bitsPerSec = hz;
        DSPI_DRV_MasterConfigureBus(obj->instance,
                &((SPI_MasterDriverType*) obj->deviceDriver)->device, &calculatedBaudRate);
    }
}

uint16_t SpiInOut(Spi_t *obj, uint16_t outData)
{
    dspi_status_t dspiResult;
    uint16_t data;
    uint8_t receiveBuffer[TRANSFER_SIZE];
    uint32_t wordsTransfered;

    if ((obj == NULL) || (obj->Spi) == NULL) {
        while (1)
            ;
    }

    // Reset the receive buffer.
    for (uint8_t i = 0; i < TRANSFER_SIZE; i++) {
        receiveBuffer[i] = 0;
    }

    if (obj->isSlave) {
        // Receive the data.
        dspiResult = DSPI_DRV_SlaveTransfer(obj->instance, NULL, receiveBuffer,
        TRANSFER_SIZE);
        if (dspiResult != kStatus_DSPI_Success) {
            PRINTF("ERROR: slave receives error \r\n");
            return -1;
        }
        // Wait until the transfer is complete.
        while (DSPI_DRV_SlaveGetTransferStatus(obj->instance, &wordsTransfered) == kStatus_DSPI_Busy) {
        }

        // Transfer the data back to master.
        dspiResult = DSPI_DRV_SlaveTransfer(obj->instance, receiveBuffer, NULL,
        TRANSFER_SIZE);
        if (dspiResult != kStatus_DSPI_Success) {
            PRINTF("ERROR: slave sends error \r\n");
            return -1;
        }
        // Wait until the transfer is complete.
        while (DSPI_DRV_SlaveGetTransferStatus(obj->instance, &wordsTransfered) == kStatus_DSPI_Busy) {
        }
    } else {
        // Send the data.
        dspiResult = DSPI_DRV_MasterTransfer(obj->instance, NULL, (uint8_t*) &outData, NULL,
        TRANSFER_SIZE);
        if (dspiResult != kStatus_DSPI_Success) {
            PRINTF("ERROR: send data error \r\n");
            return -1;
        }
        // Wait until the transfer is complete.
        while (DSPI_DRV_MasterGetTransferStatus(obj->instance, &wordsTransfered)
                == kStatus_DSPI_Busy) {
        }

        // Wait until slave is ready to send.
        DelayMs(50);

        // Receive the data.
        dspiResult = DSPI_DRV_MasterTransfer(obj->instance, NULL, NULL, receiveBuffer,
        TRANSFER_SIZE);
        if (dspiResult != kStatus_DSPI_Success) {
            PRINTF("\r\nERROR: receive data error \r\n");
            return -1;
        }
        // Wait until the transfer is complete.
        while (DSPI_DRV_MasterGetTransferStatus(obj->instance, &wordsTransfered)
                == kStatus_DSPI_Busy) {
        }
    }

    data = (uint16_t) receiveBuffer[0];
    data |= (uint16_t)(receiveBuffer[1] << 8);
    return data;
}

/*!
 * @brief This function is the implementation of SPI0 handler named in startup code.
 *
 * It passes the instance to the shared DSPI IRQ handler.
 */
void SPI0_IRQHandler(void)
{
    DSPI_DRV_IRQHandler (SPI0_IDX);
}

/*!
 * @brief This function is the implementation of SPI1 handler named in startup code.
 *
 * It passes the instance to the shared DSPI IRQ handler.
 */
void SPI1_IRQHandler(void)
{
    DSPI_DRV_IRQHandler (SPI1_IDX);
}

