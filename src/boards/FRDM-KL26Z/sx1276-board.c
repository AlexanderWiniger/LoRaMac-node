/**
 * \file sx1276-board.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.09.2015
 * \brief SX1276 driver specific target board functions implementation
 *
 */

#include "board.h"
#include "radio.h"
#include "sx1276/sx1276.h"
#include "sx1276-board.h"

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio = { SX1276Init, SX1276GetStatus, SX1276SetModem,
        SX1276SetChannel, SX1276IsChannelFree, SX1276Random, SX1276SetRxConfig,
        SX1276SetTxConfig, SX1276CheckRfFrequency, SX1276GetTimeOnAir,
        SX1276Send, SX1276SetSleep, SX1276SetStby, SX1276SetRx, SX1276StartCad,
        SX1276ReadRssi, SX1276Write, SX1276Read, SX1276WriteBuffer,
        SX1276ReadBuffer };

/*!
 * Antenna switch GPIO pins objects
 */
Gpio_t AntSwitchLf;
Gpio_t AntSwitchHf;

void SX1276IoInit(void)
{

}

void SX1276IoIrqInit(DioIrqHandler **irqHandlers)
{

}

void SX1276IoDeInit(void)
{

}

uint8_t SX1276GetPaSelect(uint32_t channel)
{

}

void SX1276SetAntSwLowPower(bool status)
{

}

void SX1276AntSwInit(void)
{

}

void SX1276AntSwDeInit(void)
{

}

void SX1276SetAntSw(uint8_t rxTx)
{

}

bool SX1276CheckRfFrequency(uint32_t frequency)
{
    // Implement check. Currently all frequencies are supported
    return true;
}
