/**
 * @file    util.h
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Utility functions
 */

/* --- HEADERS -------------------------------------------------------------- */

// Chibios
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

/* --- PUBLIC MACROS -------------------------------------------------------- */

#define DEBUG FALSE

/**
 * @brief   Macro which can be used similar to the built-in printf, without
 *          the need to include the stream
 */
#define     PRINTF(...)         chprintf((BaseSequentialStream*) &SD1, __VA_ARGS__)

#define     ECHO(c) \
    do { \
        if ((c) == '\r' || c == '\n') { \
            sdPut(&SD1, '\r'); \
            sdPut(&SD1, '\n'); \
        } \
        else { \
            sdPut(&SD1, c); \
        } \
    } while (0)


#if DEBUG == TRUE
    #define DEBUG_PRINTF(...)   PRINTF(__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif
