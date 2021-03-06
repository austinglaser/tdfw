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
#include <math.h>

// Standard Peripheral
#include "stm32f30x_tim.h"
#include "stm32f30x_rcc.h"

// Chibios
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

// Project
#include "mds.h"
#include "util.h"

/* --- PRIVATE DEFINITIONS -------------------------------------------------- */

// X encoder
#define ENC_A_X_PORT    GPIOA
#define ENC_A_X_PIN     (15)
#define ENC_A_X_AF      (1)

#define ENC_B_X_PORT    GPIOB
#define ENC_B_X_PIN     (3)
#define ENC_B_X_AF      (1)

// Y Encoder
#define ENC_A_Y_PORT    GPIOC
#define ENC_A_Y_PIN     (6)
#define ENC_A_Y_AF      (2)

#define ENC_B_Y_PORT    GPIOC
#define ENC_B_Y_PIN     (7)
#define ENC_B_Y_AF      (2)

// X Drive
#define DRIVE_X_CHANNEL (0)
#define DRIVE_X_PORT    GPIOE
#define DRIVE_X_PIN     (9)
#define DRIVE_X_AF      (2)

#define DIR_X_PORT      GPIOE
#define DIR_X_PIN       (10)

#define EN_X_PORT       GPIOE
#define EN_X_PIN        (13)

// Y Drive
#define DRIVE_Y_CHANNEL (1)
#define DRIVE_Y_PORT    GPIOE
#define DRIVE_Y_PIN     (11)
#define DRIVE_Y_AF      (2)

#define DIR_Y_PORT      GPIOE
#define DIR_Y_PIN       (12)

#define EN_Y_PORT       GPIOE
#define EN_Y_PIN        (14)

#define MDS_LOOP_TIME_MS        (1)

#define MDS_KP_DEFAULT_X        (0.15)      /**< Default proportional loop constant */
#define MDS_KI_DEFAULT_X        (0.000005)  /**< Default integral loop constant */
#define MDS_KD_DEFAULT_X        (5.0)       /**< Default differential loop constant */
#define MDS_SAT_DEFAULT_X       (10.0)      /**< Default x saturation value in volts */

#define MDS_KP_DEFAULT_Y        (0.15)      /**< Default proportional loop constant */
#define MDS_KI_DEFAULT_Y        (0.000005)  /**< Default integral loop constant */
#define MDS_KD_DEFAULT_Y        (10.0)      /**< Default differential loop constant */
#define MDS_SAT_DEFAULT_Y       (15.0)      /**< Default y saturation value in volts */

#define MDS_VOLT_THRESH         (0.1)
#define MDS_ERR_THRESH_X        (5.0)
#define MDS_ERR_THRESH_Y        (5.0)

#define MDS_SAFETY_ZONE_MM_X    (40.0) 
#define MDS_SAFETY_ZONE_MM_Y    (30.0)

#define MDS_SUPPLY_VOLTS        (20.0)

#define MDS_MM_PER_REV          (78.2)
#define MDS_COUNTS_PER_REV_X    (1024.0*2.0)
#define MDS_COUNTS_PER_REV_Y    (13.0*4.0)
#define MDS_COUNTS_PER_MM_X     (MDS_COUNTS_PER_REV_X / MDS_MM_PER_REV)
#define MDS_COUNTS_PER_MM_Y     (MDS_COUNTS_PER_REV_Y / MDS_MM_PER_REV)

#define MDS_MAX_COUNT_VALUE_X   UINT32_MAX
#define MDS_MAX_COUNT_VALUE_Y   UINT16_MAX

#define ST2MS(st) (((st) * 1000L) / CH_FREQUENCY)

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

    float setpoint_x;           /**< Current setpointed x location (in mm) */
    float setpoint_y;           /**< Current setpointed y location (in mm) */

    int32_t count_x;           /**< X encoder count */
    int16_t count_y;           /**< Y encoder count */

    float kp_x;                 /**< Proportional feedback constant for x axis */
    float ki_x;                 /**< Integral feedback constant for x axis */
    float kd_x;                 /**< Differential feedback constant for x axis */
    float kp_y;                 /**< Proportional feedback constant for y axis */
    float ki_y;                 /**< Integral feedback constant for y axis */
    float kd_y;                 /**< Differential feedback constant for y axis */

    float sat_x;                /**< Saturation for the x axis in volts */
    float sat_y;                /**< Saturation for the y axis in volts */

    float integral_x;           /**< Running integral in x */
    float integral_y;           /**< Running integral in y */
    float last_error_x;         /**< Last x error value (used for differential) */
    float last_error_y;         /**< Last y error value (used for differential) */

    float lower_x;              /**< The lower bound of the play field in the x axis (in mm) */
    float lower_y;              /**< The lower bound of the play field in the y axis (in mm) */
    float upper_x;              /**< The upper bound of the play field in the x axis (in mm) */
    float upper_y;              /**< The upper bound of the play field in the y axis (in mm) */

    int32_t offset_x;          /**< Encoder count offset in x axis*/
    int16_t offset_y;          /**< Encoder count offset in y axis*/

    int32_t cal_max_x;         /**< Maximum encoder count value encountered in x axis */
    int32_t cal_min_x;         /**< Maximum encoder count value encountered in x axis */
    int16_t cal_max_y;         /**< Maximum encoder count value encountered in y axis */
    int16_t cal_min_y;         /**< Maximum encoder count value encountered in y axis */
} mds_info_t;

/* --- PRIVATE VARIABLES ---------------------------------------------------- */

static mds_info_t mds_info = {              /**< MDS settings */
    .mode               = MDS_MODE_UNINIT,
    .is_calibrated      = 0,

    .setpoint_x         = 0,
    .setpoint_y         = 0,

    .count_x            = 0,
    .count_y            = 0,

    .kp_x               = MDS_KP_DEFAULT_X,
    .ki_x               = MDS_KI_DEFAULT_X,
    .kd_x               = MDS_KD_DEFAULT_X,
    .kp_y               = MDS_KP_DEFAULT_Y,
    .ki_y               = MDS_KI_DEFAULT_Y,
    .kd_y               = MDS_KD_DEFAULT_Y,

    .sat_x              = MDS_SAT_DEFAULT_X,
    .sat_y              = MDS_SAT_DEFAULT_Y,

    .integral_x         = 0.0,
    .integral_y         = 0.0,
    .last_error_x       = 0.0,
    .last_error_y       = 0.0,

    .lower_x            = 0.0,
    .lower_y            = 0.0,
    .upper_x            = 0.0,
    .upper_y            = 0.0,

    .offset_x           = 0,
    .offset_y           = 0,

    .cal_max_x          = 0,
    .cal_min_x          = 0,
    .cal_max_y          = 0,
    .cal_min_y          = 0,
};

static WORKING_AREA(mds_update_thread_wa, 1024);    /**< Stack area for update thread */
static Thread* mds_update_thread;                   /**< Update thread variable */
static Semaphore mds_lock_sem;                      /**< Semaphore for mutual exclusion */

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Thread to periodically update the PID
 *
 * @param context:          Unused
 */
static msg_t mds_update_thread_f(void * context);

/**
 * @brief Convert encoder counts to mm in the x dimension
 *
 * @param[in] counts:       Current encoder counter value
 *
 * @return:                 The converted x coordinate
 */
static inline float mds_counts_to_mm_x(int32_t counts);

/**
 * @brief   Convert encoder counts to mm in the x dimension
 *
 * @param[in] counts:       Current encoder counter value
 *
 * @return:                 The converted y coordinate
 */
static inline float mds_counts_to_mm_y(int16_t counts);

/**
 * @brief   Set the output setpoint on the x axis to <volts>
 *
 * @note    Setting to 0.0 will turn off the motor
 *
 * @param[in] volts:        The value to set. Converted to PWM value
 */
static inline void mds_set_output_x(float volts);

/**
 * @brief   Set the output setpoint on the x axis to <volts>
 *
 * @note    Setting to 0.0 will turn off the motor
 * 
 * @param[in] volts:        The value to set. Converted to PWM value
 */
static inline void mds_set_output_y(float volts);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

void mds_init(void)
{
    TIM_TimeBaseInitTypeDef TIM2_CFG;
    TIM_TimeBaseInitTypeDef TIM3_CFG;
    PWMConfig pwm_config;

    // Start semaphore locked
    chSemInit(&mds_lock_sem, 0);

    // Configure counter pins
    palSetPadMode(ENC_A_X_PORT, ENC_A_X_PIN, PAL_MODE_ALTERNATE(ENC_A_X_AF));
    palSetPadMode(ENC_B_X_PORT, ENC_B_X_PIN, PAL_MODE_ALTERNATE(ENC_B_X_AF));
    palSetPadMode(ENC_A_Y_PORT, ENC_A_Y_PIN, PAL_MODE_ALTERNATE(ENC_A_Y_AF));
    palSetPadMode(ENC_B_Y_PORT, ENC_B_Y_PIN, PAL_MODE_ALTERNATE(ENC_B_Y_AF));

    // Configure PWM pins
    palSetPadMode(DRIVE_X_PORT, DRIVE_X_PIN, PAL_MODE_ALTERNATE(DRIVE_X_AF));
    palSetPadMode(DRIVE_Y_PORT, DRIVE_Y_PIN, PAL_MODE_ALTERNATE(DRIVE_Y_AF));

    // Configure direction and enable pins
    palSetPadMode(EN_X_PORT, EN_X_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(EN_Y_PORT, EN_Y_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(DIR_X_PORT, DIR_X_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(DIR_X_PORT, DIR_X_PIN, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(EN_X_PORT, EN_X_PIN);
    palClearPad(EN_Y_PORT, EN_Y_PIN);
    palClearPad(DIR_X_PORT, DIR_X_PIN);
    palClearPad(DIR_Y_PORT, DIR_Y_PIN);


    // Initialize encoder counters
    // Clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // X
    TIM2_CFG.TIM_Prescaler = 0;
    TIM2_CFG.TIM_Period = UINT32_MAX;
    TIM2_CFG.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM2_CFG.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM2_CFG);
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_Cmd(TIM2, ENABLE);

    // Y
    TIM3_CFG.TIM_Prescaler = 0;
    TIM3_CFG.TIM_Period = UINT16_MAX;
    TIM3_CFG.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM3_CFG.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM3_CFG);
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_Cmd(TIM3, ENABLE);

    // Enable PWM
    pwm_config.frequency                            = 500000;
    pwm_config.period                               = 2000;
    pwm_config.callback                             = NULL;
    pwm_config.cr2                                  = 0x00000000;
    pwm_config.dier                                 = 0x00000000;

    pwm_config.channels[DRIVE_X_CHANNEL].mode       = PWM_OUTPUT_ACTIVE_LOW;
    pwm_config.channels[DRIVE_X_CHANNEL].callback   = NULL;
    pwm_config.channels[DRIVE_Y_CHANNEL].mode       = PWM_OUTPUT_ACTIVE_LOW;
    pwm_config.channels[DRIVE_Y_CHANNEL].callback   = NULL;

    pwmStart(&PWMD1, &pwm_config);
    pwmEnableChannel(&PWMD1, DRIVE_X_CHANNEL, 0); // Start both with 0% duty cycle
    pwmEnableChannel(&PWMD1, DRIVE_Y_CHANNEL, 0);
    
    // Set initial mode
    mds_info.mode = MDS_MODE_OFF;

    // Start loop
    mds_update_thread = chThdCreateStatic(mds_update_thread_wa, sizeof(mds_update_thread_wa),
                                          HIGHPRIO,
                                          mds_update_thread_f, NULL);
    // Release lock
    chSemSignal(&mds_lock_sem);
}

mds_err_t mds_set_pid_x(float kp_x, float ki_x, float kd_x, float sat_x, uint8_t reverse_x)
{
    mds_err_t err;

    // Lock
    chSemWait(&mds_lock_sem);

    // Check that we're in the correct mode to change loop parameters
    switch (mds_info.mode) {
        case MDS_MODE_UNINIT:
        case MDS_MODE_OFF:
        case MDS_MODE_CALIBRATING:
            // Record new loop parameters
            if (reverse_x) {
                mds_info.kp_x = 0.0 - kp_x;
                mds_info.ki_x = 0.0 - ki_x;
                mds_info.kd_x = 0.0 - kd_x;
            }
            else {
                mds_info.kp_x = kp_x;
                mds_info.ki_x = ki_x;
                mds_info.kd_x = kd_x;
            }

            // Record saturation
            mds_info.sat_x = sat_x;

            // Indicate success
            err = MDS_SUCCESS;
            break;

        default:
            // Indicate failure
            err = MDS_INVALID_MODE;
            break;
    }

    // Release lock
    chSemSignal(&mds_lock_sem);

    return err;
}

mds_err_t mds_set_pid_y(float kp_y, float ki_y, float kd_y, float sat_y, uint8_t reverse_y)
{
    mds_err_t err;

    // Lock
    chSemWait(&mds_lock_sem);

    // Check that we're in the correct mode to change loop parameters
    switch (mds_info.mode) {
        case MDS_MODE_UNINIT:
        case MDS_MODE_OFF:
        case MDS_MODE_CALIBRATING:
            // Record new loop parameters
            if (reverse_y) {
                mds_info.kp_y = 0.0 - kp_y;
                mds_info.ki_y = 0.0 - ki_y;
                mds_info.kd_y = 0.0 - kd_y;
            }
            else {
                mds_info.kp_y = kp_y;
                mds_info.ki_y = ki_y;
                mds_info.kd_y = kd_y;
            }

            // Record saturation
            mds_info.sat_y = sat_y;

            // Indicate success
            err = MDS_SUCCESS;
            break;

        default:
            // Indicate failure
            err = MDS_INVALID_MODE;
            break;
    }

    // Release lock
    chSemSignal(&mds_lock_sem);

    return err;
}

mds_err_t mds_start_calibration(void)
{
    // Stop movement
    mds_stop();

    // Lock
    chSemWait(&mds_lock_sem);

    // Check the mode
    if (mds_info.mode != MDS_MODE_CALIBRATING) {
        // Not calibrated!
        mds_info.is_calibrated = 0;

        // Record current location
        mds_info.cal_min_x = mds_info.count_x;
        mds_info.cal_max_x = mds_info.count_x;
        mds_info.cal_min_y = mds_info.count_y;
        mds_info.cal_max_y = mds_info.count_y;

        // Set calibration mode
        mds_info.mode = MDS_MODE_CALIBRATING;
    }

    // Release lock
    chSemSignal(&mds_lock_sem);

    // Indicate success
    return MDS_SUCCESS;
}

mds_err_t mds_stop_calibration(float* lower_x, float* upper_x, float* lower_y, float* upper_y)
{
    mds_err_t err;

    // Lock
    chSemWait(&mds_lock_sem);

    if (!lower_x || !upper_x || !lower_y || !upper_y) err = MDS_INVALID_PARAM;

    else {
        // Initialize calibration values
        *lower_x = 0.0; 
        *upper_x = 0.0; 
        *lower_y = 0.0; 
        *upper_y = 0.0; 

        switch (mds_info.mode) {
            case MDS_MODE_CALIBRATING:
                // Leave calibration mode
                mds_info.mode = MDS_MODE_OFF;

                // Set offset
                mds_info.offset_x = mds_info.cal_min_x;
                mds_info.offset_y = mds_info.cal_min_y;

                // Check that we've done a valid calibration
                if (mds_counts_to_mm_x(mds_info.cal_max_x) >= MDS_SAFETY_ZONE_MM_X*2 && 
                    mds_counts_to_mm_y(mds_info.cal_max_y) >= MDS_SAFETY_ZONE_MM_Y*2)
                {
                    // Calculate playfield
                    mds_info.lower_x = MDS_SAFETY_ZONE_MM_X;
                    mds_info.lower_y = MDS_SAFETY_ZONE_MM_Y;
                    mds_info.upper_x = mds_counts_to_mm_x(mds_info.cal_max_x) - MDS_SAFETY_ZONE_MM_X;
                    mds_info.upper_y = mds_counts_to_mm_y(mds_info.cal_max_y) - MDS_SAFETY_ZONE_MM_Y;

                    // Record that we're calibrated
                    mds_info.is_calibrated  = 1;

                    // Record calibration values
                    *upper_x = mds_counts_to_mm_x(mds_info.cal_max_x);
                    *upper_y = mds_counts_to_mm_y(mds_info.cal_max_y);

                    // Indicate success
                    err = MDS_SUCCESS;
                }
                else {
                    err = MDS_BAD_CALIBRATION;
                }
                break;

            default:
                // Wrong mode
                err = MDS_INVALID_MODE;
                break;
        }
    }

    // Release lock
    chSemSignal(&mds_lock_sem);

    return err;
}

mds_err_t mds_start(void)
{
    // Lock for critical section
    chSemWait(&mds_lock_sem);

    mds_err_t err;
    float location_x;
    float location_y;

    switch (mds_info.mode) {
        case MDS_MODE_OFF:
            // Make sure calibrated
            if (mds_info.is_calibrated) {
                // Find location
                location_x = mds_counts_to_mm_x(mds_info.count_x);
                location_y = mds_counts_to_mm_y(mds_info.count_y);

                // Make setpoint current location to prevent bad things
                mds_info.setpoint_x = location_x;
                mds_info.setpoint_y = location_y;

                // Clear integral
                mds_info.integral_x = 0.0;
                mds_info.integral_y = 0.0;

                // Calculate initial error to avoid impulse on first timestep
                mds_info.last_error_x = mds_info.setpoint_x - location_x;
                mds_info.last_error_y = mds_info.setpoint_y - location_y;

                // Turn on MDS
                mds_info.mode = MDS_MODE_ON;

                // Indicate success
                err = MDS_SUCCESS;
            }
            else {
                err = MDS_UNCALIBRATED;
            }
            break;

        default:
            err = MDS_INVALID_MODE;
            break;
    }

    // Release lock
    chSemSignal(&mds_lock_sem);

    return err;
}

mds_err_t mds_stop(void)
{
    // Lock for critical section
    chSemWait(&mds_lock_sem);

    // Record that we're off
    mds_info.mode = MDS_MODE_OFF;

    // Turn off both axes
    mds_set_output_x(0.0);
    mds_set_output_y(0.0);

    // Release lock
    chSemSignal(&mds_lock_sem);

    return MDS_SUCCESS;
}

mds_err_t mds_set_location(float setpoint_x, float setpoint_y)
{
    mds_err_t err;

    // Lock 
    chSemWait(&mds_lock_sem);

    switch (mds_info.mode) {
        case MDS_MODE_ON:
            err = MDS_SUCCESS;

            // Check x limits and record new setpoint
            if (setpoint_x > mds_info.upper_x) {
                mds_info.setpoint_x = mds_info.upper_x;
                err = MDS_OUT_OF_BOUNDS;
            }
            else if (setpoint_x < mds_info.lower_x) {
                mds_info.setpoint_x = mds_info.lower_x;
                err = MDS_OUT_OF_BOUNDS;
            }
            else {
                mds_info.setpoint_x = setpoint_x;
            }

            // Check y limits and record new setpoint
            if (setpoint_y > mds_info.upper_y) {
                mds_info.setpoint_y = mds_info.upper_y;
                err = MDS_OUT_OF_BOUNDS;
            }
            else if (setpoint_y < mds_info.lower_y) {
                mds_info.setpoint_y = mds_info.lower_y;
                err = MDS_OUT_OF_BOUNDS;
            }
            else {
                mds_info.setpoint_y = setpoint_y;
            }

            break;

        default:
            // Wrong mode
            err = MDS_INVALID_MODE;
            break;
    }

    // Release lock
    chSemSignal(&mds_lock_sem);

    return err;
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static msg_t mds_update_thread_f(void * context)
{
    (void) context;

    float location_x;
    float location_y;
    float error_x;
    float error_y;
    float differential_x;
    float differential_y;
    float command_x;
    float command_y;
    systime_t next_time;

    // Record start time
    next_time = chTimeNow() + MS2ST(MDS_LOOP_TIME_MS);

    while (TRUE) {
        // Sleep until next time
        chThdSleepUntil(next_time);

        // Figure out the next time to wake up
        next_time += MS2ST(MDS_LOOP_TIME_MS);

        // Lock 
        chSemWait(&mds_lock_sem);

        // Get latest location
        mds_info.count_x = (int32_t) TIM2->CNT;
        mds_info.count_y = (int16_t) TIM3->CNT;

        switch (mds_info.mode) {
            case MDS_MODE_ON:
                // Convert location to mm
                location_x = mds_counts_to_mm_x(mds_info.count_x);
                location_y = mds_counts_to_mm_y(mds_info.count_y);

                // Calculate errors
                error_x = mds_info.setpoint_x - location_x;
                error_y = mds_info.setpoint_y - location_y;

                // Only calculate PID if we're over a threshold, to avoid windup
                if (fabs(error_x) > MDS_ERR_THRESH_X) {
                    // Calculate x values
                    mds_info.integral_x += error_x * MDS_LOOP_TIME_MS;
                    differential_x = (error_x - mds_info.last_error_x) / MDS_LOOP_TIME_MS;
                    mds_info.last_error_x = error_x;

                    // Calculate x command
                    command_x = mds_info.kp_x * error_x             +
                                mds_info.ki_x * mds_info.integral_x + 
                                mds_info.kd_x * differential_x;
                }
                else {
                    command_x = 0.0;
                }

                // Only calculate PID if we're over a threshold, to avoid windup
                if (fabs(error_y) > MDS_ERR_THRESH_Y) {
                    // Calculate y values
                    mds_info.integral_y += error_y * MDS_LOOP_TIME_MS;
                    differential_y = (error_y - mds_info.last_error_y) / MDS_LOOP_TIME_MS;
                    mds_info.last_error_y = error_y;

                    // Calculate y command
                    command_y = mds_info.kp_y * error_y             +
                                mds_info.ki_y * mds_info.integral_y + 
                                mds_info.kd_y * differential_y;
                }
                else {
                    command_y = 0.0;
                }

                // Set command
                mds_set_output_x(command_x);
                mds_set_output_y(command_y);

                //time,x,y,set_x,set_y,command_x,command_y;
                DEBUG_PRINTF("%u,%f,%f,%f,%f,%f,%f;\r\n",
                             ST2MS(chTimeNow()),
                             location_x,
                             location_y,
                             mds_info.setpoint_x,
                             mds_info.setpoint_y,
                             command_x,
                             command_y);
                break;

            case MDS_MODE_CALIBRATING:
                // Check if we have an extreme value in the x axis. If so, record it
                if      (mds_info.count_x > mds_info.cal_max_x) mds_info.cal_max_x = mds_info.count_x;
                else if (mds_info.count_x < mds_info.cal_min_x) mds_info.cal_min_x = mds_info.count_x;

                // Check if we have an extreme value in the y axis. If so, record it
                if      (mds_info.count_y > mds_info.cal_max_y) mds_info.cal_max_y = mds_info.count_y;
                else if (mds_info.count_y < mds_info.cal_min_y) mds_info.cal_min_y = mds_info.count_y;

                // Ensure the motors are off
                mds_set_output_x(0.0);
                mds_set_output_y(0.0);

                // Tell us where we are
                DEBUG_PRINTF("%u,%u,%u;\r\n",
                             ST2MS(chTimeNow()),
                             mds_info.count_x,
                             mds_info.count_y);
                break;

            default:
                // Ensure the motors are off
                mds_set_output_x(0.0);
                mds_set_output_y(0.0);
                break;
        }

        // Don't sleep forever!
        if (chTimeNow() >= next_time) {
            DEBUG_PRINTF("Uh oh\r\n");
            next_time = chTimeNow() + MS2ST(MDS_LOOP_TIME_MS);
        }

        // Release lock
        chSemSignal(&mds_lock_sem);
    }

    // Pedantic; unreachable
    return 0;
}

static inline float mds_counts_to_mm_x(int32_t counts)
{
    // Convert counts to mm
    return (counts - mds_info.offset_x)/MDS_COUNTS_PER_MM_X;
}

static inline float mds_counts_to_mm_y(int16_t counts)
{
    // Convert counts to mm
    return (counts - mds_info.offset_y)/MDS_COUNTS_PER_MM_Y;
}

static inline void mds_set_output_x(float volts)
{
    // If we're close to zero, just turn off the channel
    if (-MDS_VOLT_THRESH <= volts && volts <= MDS_VOLT_THRESH) {
        // Turn off enable
        palClearPad(EN_X_PORT, EN_X_PIN);

        // Turn off channel
        pwmEnableChannel(&PWMD1, DRIVE_X_CHANNEL, 0);
    }
    else {
        // Check direction
        if (volts < 0.0) {
            // Backwards
            palSetPad(DIR_X_PORT, DIR_X_PIN);
            
            // Invert volts
            volts = -volts;
        }
        else {
            // Forwards
            palClearPad(DIR_X_PORT, DIR_X_PIN);
        }

        // Apply saturation
        if (volts > mds_info.sat_x) volts = mds_info.sat_x;

        // Calculate duty cycle percentage (in 100ths of percent)
        uint32_t percentage = (uint32_t) lroundf((volts * 10000.0) / MDS_SUPPLY_VOLTS);
        pwmEnableChannel(&PWMD1, DRIVE_X_CHANNEL, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, percentage));

        // Enable channel
        palSetPad(EN_X_PORT, EN_X_PIN);
    }
}

static inline void mds_set_output_y(float volts)
{
    // If we're close to zero, just turn off the channel
    if (-MDS_VOLT_THRESH <= volts && volts <= MDS_VOLT_THRESH) {
        // Turn off enable
        palClearPad(EN_Y_PORT, EN_Y_PIN);

        // Turn off channel
        pwmEnableChannel(&PWMD1, DRIVE_Y_CHANNEL, 0);
    }
    else {
        // Check direction
        if (volts < 0.0) {
            // Backwards
            palSetPad(DIR_Y_PORT, DIR_Y_PIN);
            
            // Invert volts
            volts = -volts;
        }
        else {
            // Forwards
            palClearPad(DIR_Y_PORT, DIR_Y_PIN);
        }

        // Apply saturation
        if (volts > mds_info.sat_y) volts = mds_info.sat_y;

        // Calculate duty cycle percentage (in 100ths of percent)
        uint32_t percentage = (uint32_t) lroundf((volts * 10000.0) / MDS_SUPPLY_VOLTS);
        pwmEnableChannel(&PWMD1, DRIVE_Y_CHANNEL, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, percentage));

        // Enable channel
        palSetPad(EN_Y_PORT, EN_Y_PIN);
    }
}

/**
 * @} addtogroup MDS
 */
