/**
 * \file board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief Target board general functions implementation
 *
 */
#include "board.h"
#include "adc-board.h"

/*!
 * IO Extander pins objects
 */

/*
 * MCU objects
 */

/*!
 * Initializes the unused GPIO to a know status
 */
static void BoardUnusedIoInit(void);

/*!
 * Flag to indicate if the MCU is Initialized
 */
static bool McuInitialized = false;

void BoardInitPeriph(void)
{

}

void BoardInitMcu(void)
{

}

void BoardDeInitMcu(void)
{

}

void BoardGetUniqueId(uint8_t *id)
{

}

/*!
 * Factory power supply
 */
#define FACTORY_POWER_SUPPLY                        3.0L

/*!
 * VREF calibration value
 */
#define VREFINT_CAL                                 ( *( uint16_t* )0x1FF80078 )

/*!
 * ADC maximum value
 */
#define ADC_MAX_VALUE                               4096

/*!                                                 
 * Battery thresholds                               
 */
#define BATTERY_MAX_LEVEL                           4150 // mV
#define BATTERY_MIN_LEVEL                           3200 // mV
#define BATTERY_SHUTDOWN_LEVEL                      3100 // mV

uint16_t BoardGetPowerSupply(void)
{
    return 0;
}

uint8_t BoardMeasureBatterieLevel(void)
{
    return 0;
}

static void BoardUnusedIoInit(void)
{

}
