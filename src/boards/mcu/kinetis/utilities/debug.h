/**
 * \file debug.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 13.11.2015
 * \brief Debug output helper
 *
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#if defined(USE_SEGGER_RTT)
#include "SEGGER_RTT.h"
#endif

/*******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/
/**
 * Define a global default log level (may be none), i.e. either
 * LOG_DEFAULT_LEVEL_TRACE, LOG_DEFAULT_LEVEL_DEBUG or LOG_DEFAULT_LEVEL_ERROR.
 *
 * Note:
 * In your application file, you may select a specific log level by defining
 * either LOG_LEVEL_TRACE, LOG_LEVEL_DEBUG or LOG_LEVEL_ERROR before including
 * this file.
 */
//#define LOG_DEFAULT_LEVEL_TRACE       // Everything gets printed
//#define LOG_DEFAULT_LEVEL_DEBUG       // Errors and debug data get printed
//#define LOG_DEFAULT_LEVEL_ERROR       // Only errors get printed
#define LOG_DEFAULT_LEVEL_NONE        // Nothing gets printed

/**
 * Undefine log enable flags
 */
#undef LOG_TRACE_IS_ENABLED
#undef LOG_DEBUG_IS_ENABLED
#undef LOG_ERROR_IS_ENABLED

/**
 * Color definitions
 */
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/*******************************************************************************
 * MACRO DEFINITIONS
 ******************************************************************************/
/**
 * Determine applicable log level
 */
#if !defined(LOG_LEVEL_TRACE) && !defined(LOG_LEVEL_DEBUG) && !defined(LOG_LEVEL_ERROR)
// No log level is defined -> set default log level
#if defined(LOG_DEFAULT_LEVEL_TRACE)
#define LOG_TRACE_IS_ENABLED
#define LOG_DEBUG_IS_ENABLED
#define LOG_ERROR_IS_ENABLED
#elif defined(LOG_DEFAULT_LEVEL_DEBUG)
#define LOG_DEBUG_IS_ENABLED
#define LOG_ERROR_IS_ENABLED
#elif defined(LOG_DEFAULT_LEVEL_ERROR)
#define LOG_ERROR_IS_ENABLED
#endif
#else
// At least one log level is defined
#if defined(LOG_LEVEL_TRACE)
#define LOG_TRACE_IS_ENABLED
#define LOG_DEBUG_IS_ENABLED
#define LOG_ERROR_IS_ENABLED
#elif defined(LOG_LEVEL_DEBUG)
#define LOG_DEBUG_IS_ENABLED
#define LOG_ERROR_IS_ENABLED
#elif defined(LOG_LEVEL_ERROR)
#define LOG_ERROR_IS_ENABLED
#endif
#endif

/**
 * Define log functions.
 */
#if defined(LOG_TRACE_IS_ENABLED)
#if defined(USE_SEGGER_RTT)
#define LOG_TRACE(fmt, ...)                 SEGGER_RTT_printf(0, "TRACE: " fmt "\r\n", ##__VA_ARGS__)
#define LOG_TRACE_BARE(fmt, ...)            SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__)
#define LOG_TRACE_IF(cond, fmt, ...)        if (cond) { SEGGER_RTT_printf(0, "TRACE: " fmt "\r\n", ##__VA_ARGS__); }
#define LOG_TRACE_BARE_IF(cond, fmt, ...)   if (cond) { SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__); }
#else
#define LOG_TRACE(fmt, ...)                 debug_printf("TRACE: " fmt "\r\n", ##__VA_ARGS__)
#define LOG_TRACE_BARE(fmt, ...)            debug_printf(fmt, ##__VA_ARGS__)
#define LOG_TRACE_IF(cond, fmt, ...)        if (cond) { debug_printf("TRACE: " fmt "\r\n", ##__VA_ARGS__); }
#define LOG_TRACE_BARE_IF(cond, fmt, ...)   if (cond) { debug_printf(fmt, ##__VA_ARGS__); }
#endif
#else
#define LOG_TRACE(fmt, ...)
#define LOG_TRACE_COLORED(fmt, ...)
#define LOG_TRACE_BARE(fmt, ...)
#define LOG_TRACE_IF(cond, fmt, ...)
#define LOG_TRACE_BARE_IF(cond, fmt, ...)
#endif

#if defined(LOG_DEBUG_IS_ENABLED)
#if defined(USE_SEGGER_RTT)
#define LOG_DEBUG(fmt, ...)                 SEGGER_RTT_printf(0, "DEBUG: " fmt "\r\n", ##__VA_ARGS__)
#define LOG_DEBUG_BARE(fmt, ...)            SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__)
#define LOG_DEBUG_IF(cond, fmt, ...)        if (cond) { SEGGER_RTT_printf(0, "DEBUG: " fmt "\r\n", ##__VA_ARGS__); }
#define LOG_DEBUG_BARE_IF(cond, fmt, ...)   if (cond) { SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__); }
#else
#define LOG_DEBUG(fmt, ...)                 debug_printf("DEBUG: " fmt "\r\n", ##__VA_ARGS__)
#define LOG_DEBUG_BARE(fmt, ...)            debug_printf(fmt, ##__VA_ARGS__)
#define LOG_DEBUG_IF(cond, fmt, ...)        if (cond) { debug_printf("DEBUG: " fmt "\r\n", ##__VA_ARGS__); }
#define LOG_DEBUG_BARE_IF(cond, fmt, ...)   if (cond) { debug_printf(fmt, ##__VA_ARGS__); }
#endif
#else
#define LOG_DEBUG(fmt, ...)
#define LOG_DEBUG_BARE(fmt, ...)
#define LOG_DEBUG_IF(cond, fmt, ...)
#define LOG_DEBUG_BARE_IF(cond, fmt, ...)
#endif

#if defined(LOG_ERROR_IS_ENABLED)
#if defined(USE_SEGGER_RTT)
#define LOG_ERROR(fmt, ...)                 SEGGER_RTT_printf(0, KRED "ERROR: " fmt "\x1b[0m\r\n", ##__VA_ARGS__)
#define LOG_ERROR_BARE(fmt, ...)            SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__)
#define LOG_ERROR_IF(cond, fmt, ...)        if (cond) { SEGGER_RTT_printf(0, KRED "ERROR: " fmt KNRM, ##__VA_ARGS__); }
#define LOG_ERROR_BARE_IF(cond, fmt, ...)   if (cond) { SEGGER_RTT_printf(0, fmt, ##__VA_ARGS__); }
#else
#define LOG_ERROR(fmt, ...)                 debug_printf(KRED "ERROR: " fmt "\x1b[0m\r\n", ##__VA_ARGS__)
#define LOG_ERROR_BARE(fmt, ...)            debug_printf(fmt, ##__VA_ARGS__)
#define LOG_ERROR_IF(cond, fmt, ...)        if (cond) { debug_printf(KRED "ERROR: " fmt KNRM, ##__VA_ARGS__); }
#define LOG_ERROR_BARE_IF(cond, fmt, ...)   if (cond) { debug_printf(fmt, ##__VA_ARGS__); }
#endif
#else
#define LOG_ERROR(fmt, ...)
#define LOG_ERROR_BARE(fmt, ...)
#define LOG_ERROR_IF(cond, fmt, ...)
#define LOG_ERROR_BARE_IF(cond, fmt, ...)
#endif

/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
#endif /* __DEBUG_H__ */
