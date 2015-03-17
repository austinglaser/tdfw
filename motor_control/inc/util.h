/**
 * @file    util.h
 * @author  Austin Glaser <austin.glaser@colorado.edu>
 *
 * @brief   Utility functions
 */

/* --- PUBLIC MACROS -------------------------------------------------------- */

/**
 * @brief   Macro which can be used similar to the built-in printf, without
 *          the need to include the stream
 */
#define PRINTF(...)           chprintf((BaseSequentialStream*) &SD1, __VA_ARGS__)
