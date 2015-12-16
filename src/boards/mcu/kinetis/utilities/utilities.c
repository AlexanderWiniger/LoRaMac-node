/**
 * \file utlities.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 28.09.2015
 * \brief Target mcu utilities
 *
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "utilities.h"

/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/
// Standard random functions redefinition start
#define RAND_LOCAL_MAX 2147483647L
/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
static uint32_t next = 1;

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
int32_t rand1( void )
{
    return ((next = next * 1103515245L + 12345L) % RAND_LOCAL_MAX);
}

void srand1( uint32_t seed )
{
    next = seed;
}

/*! Standard random functions redefinition end */

int32_t randr( int32_t min, int32_t max )
{
    return (int32_t)((uint32_t) rand1() % (max - min + 1) + min);
}

void *custom_malloc( size_t size )
{
#if defined(FSL_RTOS_FREE_RTOS) || defined(USE_FREE_RTOS)
    return pvPortMalloc(size);
#else
    return malloc(size);
#endif
}

void custom_free( void *p )
{
#if defined(FSL_RTOS_FREE_RTOS) || defined(USE_FREE_RTOS)
    vPortFree(p);
#else
    free(p);
#endif
}

void memcpy1( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    while (size--) {
        *dst++ = *src++;
    }
}

void memset1( uint8_t *dst, uint8_t value, uint16_t size )
{
    while (size--) {
        *dst++ = value;
    }
}

int8_t Nibble2HexChar( uint8_t a )
{
    if ( a < 10 ) {
        return '0' + a;
    } else if ( a < 16 ) {
        return 'A' + (a - 10);
    } else {
        return '?';
    }
}
