/**
 * \file types.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 17.11.2015
 * \brief Type definitions
 *
 */

#ifndef __TYPES_H__
#define __TYPES_H__

/* Standard ANSI C types */
#include <stdint.h>

#ifndef FALSE
  #define  FALSE  0x00u                /* Boolean value FALSE. FALSE is defined always as a zero value. */
#endif
#ifndef TRUE
  #define  TRUE   0x01u                /* Boolean value TRUE. TRUE is defined always as a non zero value. */
#endif

#ifndef NULL
  #define  NULL   0x00u
#endif

/* PE types definition */
#ifndef __cplusplus
  #ifndef bool
typedef unsigned char           bool;
  #endif
#endif
typedef unsigned char           byte;
typedef unsigned short          word;
typedef unsigned long           dword;
typedef unsigned long long      dlong;
typedef unsigned char           TPE_ErrCode;
#ifndef TPE_Float
typedef float                   TPE_Float;
#endif
#ifndef char_t
typedef char                    char_t;
#endif

/* Other basic data types */
typedef signed char             int8;
typedef signed short int        int16;
typedef signed long int         int32;

typedef unsigned char           uint8;
typedef unsigned short int      uint16;
typedef unsigned long int       uint32;


/**********************************************************/
/* Uniform multiplatform 8-bits peripheral access macros */
/**********************************************************/

/* Enable maskable interrupts */
#define __enable_irq(void)\
 do {\
  /*lint -save  -e950 Disable MISRA rule (1.1) checking. */\
     __asm("CPSIE f");\
  /*lint -restore Enable MISRA rule (1.1) checking. */\
 } while(0)

/* Disable maskable interrupts */
#define __disable_irq() \
 do {\
  /*lint -save  -e950 Disable MISRA rule (1.1) checking. */\
     __asm ("CPSID f");\
  /*lint -restore Enable MISRA rule (1.1) checking. */\
 } while(0)



/* Save status register and disable interrupts */
#define EnterCritical() \
 do {\
  uint8_t SR_reg_local;\
  /*lint -save  -e586 -e950 Disable MISRA rule (2.1,1.1) checking. */\
   __asm ( \
   "MRS R0, FAULTMASK\n\t" \
   "CPSID f\n\t"            \
   "STRB R0, %[output]"  \
   : [output] "=m" (SR_reg_local)\
   :: "r0");\
  /*lint -restore Enable MISRA rule (2.1,1.1) checking. */\
   if (++SR_lock == 1u) {\
     SR_reg = SR_reg_local;\
   }\
 } while(0)


/* Restore status register  */
#define ExitCritical() \
 do {\
   if (--SR_lock == 0u) { \
  /*lint -save  -e586 -e950 Disable MISRA rule (2.1,1.1) checking. */\
     __asm (                 \
       "ldrb r0, %[input]\n\t"\
       "msr FAULTMASK,r0;\n\t" \
       ::[input] "m" (SR_reg)  \
       : "r0");                \
  /*lint -restore Enable MISRA rule (2.1,1.1) checking. */\
   }\
 } while(0)


#define _DEBUGHALT() \
  /*lint -save  -e586 -e950 Disable MISRA rule (2.1,1.1) checking. */\
  __asm( "BKPT 255") \
  /*lint -restore Enable MISRA rule (2.1,1.1) checking. */

#define _NOP() \
  /*lint -save  -e586 -e950 Disable MISRA rule (2.1,1.1) checking. */\
  __asm( "NOP") \
  /*lint -restore Enable MISRA rule (2.1,1.1) checking. */

#define _WFI() \
  /*lint -save  -e586 -e950 Disable MISRA rule (2.1,1.1) checking. */\
  __asm("WFI") \
  /*lint -restore Enable MISRA rule (2.1,1.1) checking. */

/* Interrupt definition template */
#if !defined(Default_ISR)
  #define Default_ISR(ISR_name) void __attribute__ ((weak, interrupt)) ISR_name(void)
#endif

#endif /* __TYPES_H__ */
