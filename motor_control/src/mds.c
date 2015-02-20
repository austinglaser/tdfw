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

/* --- PRIVATE DEFINITIONS -------------------------------------------------- */

#define MDS_KP_DEFAULT          (1)     /**< Default proportional loop constant */
#define MDS_KI_DEFAULT          (1)     /**< Default integral loop constant */
#define MDS_KD_DEFAULT          (1)     /**< Default differential loop constant */

#define MDS_MM_PER_REV          (78)
#define MDS_X_COUNTS_PER_REV    (1024*2)
#define MDS_Y_COUNTS_PER_REV    (13*4)
#define MDS_X_COUNTS_PER_MM     (MDS_X_COUNTS_PER_REV / MDS_MM_PER_REV)
#define MDS_Y_COUNTS_PER_MM     (MDS_Y_COUNTS_PER_REV / MDS_MM_PER_REV)

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   Modes for the mallet drive system
 */
typedef enum {
    MDS_MODE_UNINIT     = 0x00, /**< MDS has not yet been initialized */
    MDS_MODE_OFF,               /**< MDS is not controlling mallet */
    MDS_MODE_ON,                /**< MDS is controlling mallet */
    MDS_MODE_CALIBRATING,       /**< MDS is calibrating */
    MAX_MDS_MODE,               /**< Total number of MDS modes */
    MDS_MODE_INVALID,           /**< Bad MDS mode */
} mds_mode_t;

/**
 *  @brief  Info for the mallet drive system
 */
typedef struct {
    mds_mode_t mode;            /**< Current system mode */
    uint32_t is_calibrated;     /**< Whether or not the MDS is calibrated */

    uint32_t x_command;         /**< Current commanded x location (in mm) */
    uint32_t y_command;         /**< Current commanded y location (in mm) */

    uint32_t x_count;           /**< X encoder count */
    uint32_t y_count;           /**< Y encoder count */
    int32_t x_under;            /**< Number of times x count has over/under-flowed */
    int32_t y_under;            /**< Number of times y count has over/under-flowed */

    uint32_t kp;                /**< Proportional feedback constant */
    uint32_t ki;                /**< Integral feedback constant */
    uint32_t kd;                /**< Differential feedback constant */

    uint32_t x_lower;           /**< The lower bound of the play field in the x axis (in mm) */
    uint32_t x_upper;           /**< The upper bound of the play field in the x axis (in mm) */
    uint32_t y_lower;           /**< The lower bound of the play field in the y axis (in mm) */
    uint32_t y_upper;           /**< The upper bound of the play field in the y axis (in mm) */

    int32_t x_offset;           /**< Encoder count offset */
    int32_t y_offset;           /**< Encoder count offset */
} mds_info_t;

/* --- PRIVATE VARIABLES ---------------------------------------------------- */

static mds_info_t mds_info = {  /**< MDS settings */
    .mode               = MDS_MODE_UNINIT,
    .is_calibrated      = 0,

    .x_command          = 0,
    .y_command          = 0,

    .x_count            = 0,
    .y_count            = 0,
    .x_overflows        = 0,
    .y_overflows        = 0,

    .kp                 = MDS_KP_DEFAULT,
    .ki                 = MDS_KI_DEFAULT,
    .kd                 = MDS_KD_DEFAULT,

    .x_lower            = 0,
    .x_upper            = 0,
    .y_lower            = 0,
    .y_upper            = 0,

    .x_offset           = 0,
    .y_offset           = 0,
};

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Uses values in mds_info to calculate a new conversion factor
 */
static void mds_calculate_conversion(void);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

void mds_init(void)
{
    // Initialize counters

    // Initialize loop callback

    // Set initial mode
    mds_info.mode = MDS_MODE_OFF;
}

mds_err_t mds_set_pid(uint32_t kp, uint32_t ki, uint32_t kd)
{
    mds_err_t err;

    // Check that we're in the correct mode to change loop parameters
    switch (mds_info.mode) {
        case MDS_MODE_UNINIT:
        case MDS_MODE_OFF:
        case MDS_MODE_CALIBRATING:
            // Record new loop parameters
            mds_info.kp = kp;
            mds_info.ki = ki;
            mds_info.kd = kd;

            // Indicate success
            err = MDS_SUCCESS;
            break;

        default:
            // Indicate failure
            err = MDS_INVALID_MODE;
            break;
    }

    return err;
}

mds_err_t mds_start_calibration(void)
{
    // Stop movement
    mds_stop();

    // Check the mode
    if (mds_info.mode != MDS_MODE_CALIBRATING) {

        // Record current location
        mds_info.x_cal_min          = x_count;
        mds_info.x_cal_max          = x_count;
        mds_info.y_cal_min          = y_count;
        mds_info.y_cal_max          = y_count;
        mds_info.x_cal_overflows    = x_overflows;
        mds_info.y_cal_overflows    = y_overflows;

        // Set calibration mode
        mds_info.mode = MDS_MODE_CALIBRATING;
    }

    // Indicate success
    return MDS_SUCCESS;
}

mds_err_t mds_stop_calibration(void)
{
    // Leave calibration mode
    mds_info.mode           = MDS_MODE_OFF;

    // Calculate conversion factor
    mds_calculate_conversion();

    mds_info.is_calibrated  = 1;
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

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

/**
 * @} addtogroup MDS
 */
