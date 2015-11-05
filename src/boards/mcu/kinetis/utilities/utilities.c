/**
 * \file utlities.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 28.09.2015
 * \brief Target mcu utilities
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "utilities.h"
#include "fsl_rnga_driver.h"

/*! Standard random functions redefinition start */
static bool rngaInitialized = false;

int rand(void)
{
    int32_t randout;

    if (!rngaInitialized) {
        rnga_user_config_t
        rngaConfig = {
            .isIntMasked = true,
            .highAssuranceEnable = true
        };
        RNGA_DRV_Init(0, &rngaConfig);
        rngaInitialized = true;
    }

    RNGA_DRV_GetRandomData(0, &randout, sizeof(int32_t));
    return randout;
}

void srand(unsigned int seed)
{
    RNGA_DRV_Seed(0, seed);
}
/*! Standard random functions redefinition end */

int32_t randr(int32_t min, int32_t max)
{
    return (int32_t)((uint32_t) rand() % (max - min + 1) + min);
}

void memcpy1(uint8_t *dst, uint8_t *src, uint16_t size)
{
    while (size--) {
        *dst++ = *src++;
    }
}

void memset1(uint8_t *dst, uint8_t value, uint16_t size)
{
    while (size--) {
        *dst++ = value;
    }
}

int8_t Nibble2HexChar(uint8_t a)
{
    if (a < 10) {
        return '0' + a;
    } else if (a < 16) {
        return 'A' + (a - 10);
    } else {
        return '?';
    }
}
