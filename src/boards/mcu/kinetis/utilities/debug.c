/**
 * \file debug.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 13.11.2015
 * \brief Debug output helper
 *
 */

/*******************************************************************************
 * Company:        LUCERNE UNIVERSITY OF APPLIED SCIENCES AND ARTS
 *
 * Project:        CEMIOS
 *
 * Target:         stm32f407vg
 *
 * Type:           module
 *
 * Description:    debug module
 *
 * Compiler:       ANSI-C
 *
 * Filename:       debug.c
 *
 * Version:        1.0
 *
 * Author:         Tobias Pluess
 *
 * Creation-Date:  21.05.2015
 *******************************************************************************
 Modification History:
 [1.0]    21.05.2015    Tobias Pluess
 - created
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/

#include <stdio.h>
#include "debug.h"

/*******************************************************************************
 * PRIVATE CONSTANT DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE MACRO DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE VARIABLES (STATIC)
 ******************************************************************************/
#ifdef DEBUG

void debug_assert(const char* file, int line)
{
    asm volatile("bkpt");
} /* end debug_assert */

#endif

/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
