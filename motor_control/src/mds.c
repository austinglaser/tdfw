/**
 * @file    mds.c
 * @author  Autin Glaser <austin.glaser@colorado.edu>
 * 
 * @brief   Interface code for driving MDS on the AHA system
 *
 * @addtogroup MDS
 * @{
 */

/* --- INCLUDE FILES -------------------------------------------------------- */

// Standard
#include <stdint.h>

// Project
#include "mds.h"

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

void mds_init(void)
{
}

mds_err_t mds_set_pid(uint32_t kp, uint32_t ki, uint32_t kd)
{
    (void) kp;
    (void) ki;
    (void) kd;

    return MDS_FAIL;
}

mds_err_t mds_set_playfield(uint32_t x_lower, uint32_t x_upper, uint32_t y_lower, uint32_t y_upper)
{
    (void) x_lower;
    (void) x_upper;
    (void) y_lower;
    (void) y_upper;

    return MDS_FAIL;
}

mds_err_t mds_start_calibration(void)
{
    return MDS_FAIL;
}

mds_err_t mds_stop_calibration(void)
{
    return MDS_FAIL;
}

mds_err_t mds_start(void)
{
    return MDS_FAIL;
}

mds_err_t mds_stop(void)
{
    return MDS_FAIL;
}

mds_err_t mds_set_location(uint32_t x_location, uint32_t y_location)
{
    (void) x_location;
    (void) y_location;

    return MDS_FAIL;
}

/**
 * @} addtogroup MDS
 */
