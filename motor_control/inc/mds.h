/**
 * @file    mds.h
 * @author  Autin Glaser <austin.glaser@colorado.edu>
 * 
 * @brief   Interface code for driving MDS on the AHA system
 */

#ifndef MDS_H_
#define MDS_H_

/**
 * @defgroup MDS
 * @{
 */

/* --- INCLUDE FILES -------------------------------------------------------- */

// Standard
#include <stdint.h>

/* --- PUBLIC DATA TYPES ---------------------------------------------------- */

/**
 * @brief   Possible error codes from MDS functions. Don't even trip dawg
 */
typedef enum {
    MDS_SUCCESS         = 0x00, /**< Everything went better than expected! */
    MDS_INIT,                   /**< Start the engine, dammit! */
    MDS_UNCALIBRATED,           /**< We don't know WHERE we are */
    MDS_INVALID_MODE,           /**< I can't do that Dave */
    MDS_INVALID_PARAM,          /**< one dem params not right dawg */
    MDS_OUT_OF_BOUNDS,          /**< TOO BIG! TOO SMALL! AAAAAH */
    MDS_FAIL,                   /**< Sometink vent wrong. Ve don't know vat */
} mds_err_t;

/* --- PUBLIC FUNCTION PROTOTYPES ------------------------------------------- */

/**
 * @brief   Initializes the MDS system. Does not start motion or initiat calibration
 */
void mds_init(void);

/**
 * @brief   Sets the controller's loop parameters
 *
 * @note    At initialization, these are set to a conservative default value
 *
 * @param[in] kp:               The proportional feedback constant
 * @param[in] ki:               The integral feedback constant
 * @param[in] kd:               The differential feedback constant
 *
 * @retval MDS_SUCCESS:         The new loop parameters are set
 * @retval MDS_INVALID_MODE:    The MDS is moving, can't set these on the fly
 * @retval MDS_INVALID_PARAM:   One or more of the constants is too big
 */
mds_err_t mds_set_pid(uint32_t kp, uint32_t ki, uint32_t kd);

/**
 * @brief   Start the mallet calibration process
 * 
 * @details During the calibration process, the user must manually move the
 *          mallet to the extreme limits of its motion. The MDS driver will
 *          record the maximum and minimum values, and designate valid locations
 *          within these hard limits
 *
 *          This function stops all mallet movement when it is called, if any was
 *          in progress
 *
 * @retval MDS_SUCCESS:         Calibration is in progress
 */
mds_err_t mds_start_calibration(void);

/**
 * @brief   Stop the mallet calibration process, and record the bounds
 *
 * @note    The MDS uses the provided size of the playfield to calculate
 *          the conversion from pixel values
 *
 * @retval MDS_SUCCESS:         Calibration has completed successfuly
 */
mds_err_t mds_stop_calibration(void);

/**
 * @brief   Puts the mallet under MDS control
 *
 * @retval MDS_SUCCESS:         The mallet is now under MDS control
 * @retval MDS_INVALID_MODE:    MDS calibrating
 * @retval MDS_UNCALIBRATED:    MDS is uncalibrated, can't initiate movement
 */
mds_err_t mds_start(void);

/**
 * @brief   Releases MDS controll of the mallet
 *
 * @retval MDS_SUCCESS:         The mallet will not move
 * @retval MDS_INVALID_MODE:    MDS calibrating
 * @retval MDS_UNCALIBRATED:    MDS is uncalibrated, can't initiate movement
 */
mds_err_t mds_stop(void);

/**
 * @brief   Command the mallet to a certain position
 * 
 * @param[in] x_location:       The target x location in mm
 * @param[in] y_location:       The target y location in mm
 *
 * @retval MDS_SUCCESS:         New target locations successfuly recorded
 * @retval MDS_OUT_OF_BOUNDS:   Either or both target locations are outside the boundaries
 * @retval MDS_INVALID_MODE:    Movement has not been initiated
 */
mds_err_t mds_set_location(uint32_t x_location, uint32_t y_location);

/**
 * @} defgroup MDS
 */

#endif  //MDS_H_
