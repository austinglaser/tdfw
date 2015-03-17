/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/* --- HEADERS -------------------------------------------------------------- */

// Standard
#include <math.h>

// Chibios
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

// Project
#include "mds.h"
#include "util.h"

/* --- PRIVATE MACROS ------------------------------------------------------- */

#define BUF_LEN     (64)            /**< Size of serial buffer */

/* --- PRIVATE DATA TYPES --------------------------------------------------- */

/**
 * @brief   States used for the serial state machine
 */
typedef enum {
    SERIAL_MODE_IDLE        = 0x00, /**< No transaction in process */
    SERIAL_MODE_STARTED,            /**< Start byte recieved */
    SERIAL_MODE_PARAM,              /**< Recieving loop params */
    SERIAL_MODE_PARAM_X,            /**< Recieving x loop params */
    SERIAL_MODE_PARAM_X_P,          /**< Recieving x proportional constant */
    SERIAL_MODE_PARAM_X_I,          /**< Recieving x integral constant */
    SERIAL_MODE_PARAM_X_D,          /**< Recieving x derivative constant */
    SERIAL_MODE_PARAM_X_S,          /**< Recieving x saturation constant */
    SERIAL_MODE_PARAM_Y,            /**< Recieving y loop params */
    SERIAL_MODE_PARAM_Y_P,          /**< Recieving y proportional constant */
    SERIAL_MODE_PARAM_Y_I,          /**< Recieving y integral constant */
    SERIAL_MODE_PARAM_Y_D,          /**< Recieving y derivative constant */
    SERIAL_MODE_PARAM_Y_S,          /**< Recieving y saturation constant */
    SERIAL_MODE_LOCATION,           /**< Recieving location command */
    SERIAL_MODE_LOCATION_X,         /**< Recieving x location */
    SERIAL_MODE_LOCATION_Y,         /**< Recieving y location */
    SERIAL_MODE_RUN,                /**< Recieved run command */
    SERIAL_MODE_STOP,               /**< Recieved stop command */
    SERIAL_MODE_CALIBRATE,          /**< Recieved calibrate command */
    SERIAL_MODE_CALIBRATE_DONE,     /**< Recieved calibration done command */
    SERIAL_MODE_ERROR,              /**< Error state */
    MAX_SERIAL_MODE,                /**< Total number of serial modes */
    SERIAL_MODE_INVALID,            /**< Invalid serial mode */
} serial_mode_t;

/**
 * @brief   Errors the serial state machine can encounter
 */
typedef enum {
    SERIAL_ERROR_NONE       = 0x00, /**< No error */

    SERIAL_ERROR_MDS_INIT,          /**< MDS uninitialized */
    SERIAL_ERROR_MDS_UNCALIBRATED,  /**< MDS not calibrated */
    SERIAL_ERROR_MDS_INVALID_MODE,  /**< MDS in wrong mode */
    SERIAL_ERROR_MDS_INVALID_PARAM, /**< Invalid parameter supplied to mds function */
    SERIAL_ERROR_MDS_OUT_OF_BOUNDS, /**< Location supplied outside of safety boundaries */
    SERIAL_ERROR_MDS_BAD_CALIBRATION, /**< Bad calibration */
    SERIAL_ERROR_MDS_FAIL,          /**< Generic MDS failure */

    SERIAL_ERROR_BAD_START,         /**< Unexpected character encountered when starting */
    SERIAL_ERROR_BAD_MSG_TYPE,      /**< Bad message type character */
    SERIAL_ERROR_BAD_SEPARATOR,     /**< Bad separator character */
    SERIAL_ERROR_PARSE_VALUE,       /**< Trouble parsing value */
    SERIAL_ERROR_TRAILING_CHARS,    /**< Trailing characters at the end of the message */

    MAX_SERIAL_ERROR,               /**< Total number of serial errors */
    SERIAL_ERROR_INVALID,           /**< Invalid serial error */
} serial_error_t;

typedef enum {
    SERIAL_START            = 'G',  /**< Start character for all transactions */
    SERIAL_ON               = 'O',  /**< Board is powered and ready to recieve commands */
    SERIAL_PARAM            = 'R',  /**< Character to set parameters */
    SERIAL_PARAM_X          = 'X',  /**< Character to set x parameters */
    SERIAL_PARAM_Y          = 'Y',  /**< Character to set y parameters */
    SERIAL_LOCATION         = 'L',  /**< Character to set location */
    SERIAL_RUN              = 'S',  /**< Character to turn on mds */
    SERIAL_STOP             = 'P',  /**< character to turn off mds */
    SERIAL_CALIBRATE        = 'C',  /**< Character to calibrate mds */
    SERIAL_CALIBRATE_DONE   = 'D',  /**< Character to finish calibrating */
    SERIAL_ACK              = 'A',  /**< Ack character */
    SERIAL_ERROR            = 'E',  /**< Error character */
} serial_char_t;

typedef void (*serial_mode_handler_t)(uint8_t);

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief   Handle a character in idle mode
 */
static void serial_mode_idle_handler(uint8_t c);

/**
 * @brief   Handle a character in started mode
 */
static void serial_mode_started_handler(uint8_t c);

/**
 * @brief   Handle a character in parameter mode
 */
static void serial_mode_param_handler(uint8_t c);

/**
 * @brief   Handle a character in x parameter mode
 */
static void serial_mode_param_x_handler(uint8_t c);

/**
 * @brief   Handle a character in x proportional parameter mode
 */
static void serial_mode_param_x_p_handler(uint8_t c);

/**
 * @brief   Handle a character in x integral parameter mode
 */
static void serial_mode_param_x_i_handler(uint8_t c);

/**
 * @brief   Handle a character in x derivative parameter mode
 */
static void serial_mode_param_x_d_handler(uint8_t c);

/**
 * @brief   Handle a character in x saturation parameter mode
 */
static void serial_mode_param_x_s_handler(uint8_t c);

/**
 * @brief   Handle a character in y parameter mode
 */
static void serial_mode_param_y_handler(uint8_t c);

/**
 * @brief   Handle a character in y proportional parameter mode
 */
static void serial_mode_param_y_p_handler(uint8_t c);

/**
 * @brief   Handle a character in y integral parameter mode
 */
static void serial_mode_param_y_i_handler(uint8_t c);

/**
 * @brief   Handle a character in y derivative parameter mode
 */
static void serial_mode_param_y_d_handler(uint8_t c);

/**
 * @brief   Handle a character in y saturation parameter mode
 */
static void serial_mode_param_y_s_handler(uint8_t c);

/**
 * @brief   Handle a character in location mode
 */
static void serial_mode_location_x_handler(uint8_t c);

/**
 * @brief   Handle a character in location mode
 */
static void serial_mode_location_y_handler(uint8_t c);

/**
 * @brief   Handle a character in location mode
 */
static void serial_mode_location_handler(uint8_t c);

/**
 * @brief   Handle a character in start mode
 */
static void serial_mode_run_handler(uint8_t c);

/**
 * @brief   Handle a character in stop mode
 */
static void serial_mode_stop_handler(uint8_t c);

/**
 * @brief   Handle a character in calibrate mode
 */
static void serial_mode_calibrate_handler(uint8_t c);

/**
 * @brief   Handle a character in calibration done mode
 */
static void serial_mode_calibrate_done_handler(uint8_t c);

/**
 * @brief   Handle a character in error mode
 */
static void serial_mode_error_handler(uint8_t c);

/**
 * @brief   Respond with an ack
 */
static inline void serial_send_ack(void);

/**
 * @brief   Respond with an error
 *
 * @param[in] c:    Determines whether the error is to be sent immediately or not
 */
static inline void serial_send_error(void);

/**
 * @brief   Sets an error code
 *
 * @param[in] err:  The new error code to be set
 * @param[in] c:    The last character recieved. Determines whether it's printed immediately or not
 */
static inline void serial_set_error(serial_error_t err, uint8_t c);

/**
 * @brief   Tries to interpret the string in buf as a float
 */
static int string_to_float(uint8_t* buf, float* number);

/* --- PRIVATE VARIABLES ---------------------------------------------------- */

static serial_mode_t serial_mode;   /**< Mode variable for serial state machine */
static serial_error_t serial_error; /**< Error which occurred during serial transaction */

static uint8_t serial_buf[BUF_LEN]; /**< Buffer for multi-character values */
static uint32_t serial_buf_index;   /**< Index of next open slot in buffer */

static float kp_x = 0.0;            /**< X proportional constant */
static float ki_x = 0.0;            /**< X integral constant */
static float kd_x = 0.0;            /**< X derivative constant */
static float sat_x = 0.0;           /**< X saturation constant */

static float kp_y = 0.0;            /**< Y proportional constant */
static float ki_y = 0.0;            /**< Y integral constant */
static float kd_y = 0.0;            /**< Y derivative constant */
static float sat_y = 0.0;           /**< Y saturation constant */

static float location_x = 0.0;      /**< X location */
static float location_y = 0.0;      /**< Y location */

/**
 * @brief   Array of character handlers
 */
static serial_mode_handler_t serial_mode_handler[MAX_SERIAL_MODE] = {
    serial_mode_idle_handler,
    serial_mode_started_handler,

    serial_mode_param_handler,
    serial_mode_param_x_handler,
    serial_mode_param_x_p_handler,
    serial_mode_param_x_i_handler,
    serial_mode_param_x_d_handler,
    serial_mode_param_x_s_handler,
    serial_mode_param_y_handler,
    serial_mode_param_y_p_handler,
    serial_mode_param_y_i_handler,
    serial_mode_param_y_d_handler,
    serial_mode_param_y_s_handler,

    serial_mode_location_handler,
    serial_mode_location_x_handler,
    serial_mode_location_y_handler,

    serial_mode_run_handler,
    serial_mode_stop_handler,

    serial_mode_calibrate_handler,
    serial_mode_calibrate_done_handler,

    serial_mode_error_handler,
};

/**
 * @brief   Error strings
 */
static char * serial_error_string[MAX_SERIAL_ERROR] = {
    "No error",

    "MDS uninitialized",
    "MDS uncalibrated",
    "MDS in invalid mode",
    "MDS recieved invalid paramter",
    "MDS recieved out-of-bounds command",
    "MDS failed to calibrate",
    "MDS error",

    "Bad start character",
    "Bad message type",
    "Bad separator",
    "Unable to parse value",
    "Trailing characters",
};

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

/*
 * Application entry point.
 */
int main(void)
{
    uint8_t c;

    // System initializations.
    halInit();
    chSysInit();

    // Activates the serial driver 1 using the driver default configuration.
    // PA9(TX) and PA10(RX) are routed to USART1.
    sdStart(&SD1, NULL);
    palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));

    // Initialize MDS
    mds_init();

    // Initialize serial state machine
    serial_mode = SERIAL_MODE_IDLE;
    serial_error = SERIAL_ERROR_NONE;

    // Flush buffer
    while (!sdGetWouldBlock(&SD1)) sdGet(&SD1);

    // Send power on signal
    PRINTF("%c%c\r\n", SERIAL_START, SERIAL_ON);

    while (TRUE) {
        // Get a character
        c = sdGet(&SD1);

        // TODO: Take out echo?
        if (c == '\r' || c == '\n') {
            sdPut(&SD1, '\r');
            sdPut(&SD1, '\n');
        }
        else sdPut(&SD1, c);

        // Call the appropriate handler
        serial_mode_handler[serial_mode](c);
    }
}

static void serial_mode_idle_handler(uint8_t c)
{
    DEBUG_PRINTF("idle\r\n");

    // Handle start character
    if (c == SERIAL_START) {
        serial_mode = SERIAL_MODE_STARTED;
        return;
    }

    // Handle other characters
    else if (c != '\r' && c != '\n') {
        serial_set_error(SERIAL_ERROR_BAD_START, c);
        return;
    }
}

static void serial_mode_started_handler(uint8_t c)
{
    DEBUG_PRINTF("started\r\n");

    // Handle message type character
    switch (c) {
        case SERIAL_PARAM:
            serial_mode = SERIAL_MODE_PARAM;
            break;

        case SERIAL_LOCATION:
            serial_mode = SERIAL_MODE_LOCATION;
            break;

        case SERIAL_RUN:
            serial_mode = SERIAL_MODE_RUN;
            break;

        case SERIAL_STOP:
            serial_mode = SERIAL_MODE_STOP;
            break;

        case SERIAL_CALIBRATE:
            serial_mode = SERIAL_MODE_CALIBRATE;
            break;
            
        case SERIAL_CALIBRATE_DONE:
            serial_mode = SERIAL_MODE_CALIBRATE_DONE;
            break;

        default:
            serial_set_error(SERIAL_ERROR_BAD_MSG_TYPE, c);
            break;
    }
}

static void serial_mode_param_handler(uint8_t c)
{
    DEBUG_PRINTF("param\r\n");

    // Handle message sub-type
    switch (c) {
        case SERIAL_PARAM_X:
            serial_mode = SERIAL_MODE_PARAM_X;
            break;

        case SERIAL_PARAM_Y:
            serial_mode = SERIAL_MODE_PARAM_Y;
            break;

        default:
            serial_set_error(SERIAL_ERROR_BAD_MSG_TYPE, c);
            break;
    }
}

static void serial_mode_param_x_handler(uint8_t c)
{
    DEBUG_PRINTF("param_x\r\n");

    if (c == ':') {
        // Zero out buffer
        serial_buf_index = 0;

        // Go to next mode
        serial_mode = SERIAL_MODE_PARAM_X_P;
        return;
    }
    else {
        // Go to error mode
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
        return;
    }
}

static void serial_mode_param_x_p_handler(uint8_t c)
{
    DEBUG_PRINTF("param_x_p\r\n");

    // Record numerical characters
    if (('0' <= c && c <= '9') || c == '.') {
        serial_buf[serial_buf_index] = c;
        serial_buf_index++;
    }

    // Separator
    else if (c == ',') {
        // Add terminator
        serial_buf[serial_buf_index] = '\0';

        // Interpret value
        kp_x = 0.0;
        if (string_to_float(serial_buf, &kp_x)) {
            // Bad value
            serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
            return;
        }

        // Next value
        serial_buf_index = 0;
        serial_mode = SERIAL_MODE_PARAM_X_I;
        return;
    }

    // Bad character
    else {
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
        return;
    }
}

static void serial_mode_param_x_i_handler(uint8_t c)
{
    DEBUG_PRINTF("param_x_i\r\n");

    // Record numerical characters
    if (('0' <= c && c <= '9') || c == '.') {
        serial_buf[serial_buf_index] = c;
        serial_buf_index++;
    }
    
    // Separator
    else if (c == ',') {
        // Add terminator
        serial_buf[serial_buf_index] = '\0';

        // Interpret value
        ki_x = 0.0;
        if (string_to_float(serial_buf, &ki_x)) {
            // Bad value
            serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
            return;
        }

        // Next value
        serial_buf_index = 0;
        serial_mode = SERIAL_MODE_PARAM_X_D;
        return;
    }

    else {
        // Bad character
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
        return;
    }
}

static void serial_mode_param_x_d_handler(uint8_t c)
{
    DEBUG_PRINTF("param_x_d\r\n");

    // Record numerical characters
    if (('0' <= c && c <= '9') || c == '.') {
        serial_buf[serial_buf_index] = c;
        serial_buf_index++;
    }
    
    // Separator
    else if (c == ',') {
        // Add terminator
        serial_buf[serial_buf_index] = '\0';

        // Interpret value
        kd_x = 0.0;
        if (string_to_float(serial_buf, &kd_x)) {
            // Bad value
            serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
            return;
        }

        // Next value
        serial_buf_index = 0;
        serial_mode = SERIAL_MODE_PARAM_X_S;
        return;
    }

    else {
        // Bad character
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
        return;
    }
}

static void serial_mode_param_x_s_handler(uint8_t c)
{
    DEBUG_PRINTF("param_x_s\r\n");

    // Static initialization only happens once
    static uint32_t got_number = 0;

    if (!got_number) {
        // Numerical character
        if (('0' <= c && c <= '9') || c == '.') {
            serial_buf[serial_buf_index] = c;
            serial_buf_index++;
        }

        // Separator
        else if (c == ',') {
            // Add terminator
            serial_buf[serial_buf_index] = '\0';

            // Interpret value
            sat_x = 0.0;
            if (string_to_float(serial_buf, &sat_x)) {
                // Bad value
                serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
                return;
            }

            // We're done
            got_number = 1;
        }

        // Bad character
        else {
            serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
            return;
        }
    }

    else if (c == '\n' || c == '\r') {
        if (!got_number) {
            // Add terminator
            serial_buf[serial_buf_index] = '\0';

            // Interpret value
            sat_x = 0.0;
            if (string_to_float(serial_buf, &sat_x)) {
                // Bad value
                serial_mode = SERIAL_MODE_ERROR;
                serial_error = SERIAL_ERROR_PARSE_VALUE;
                return;
            }
        }

        // Reset buffer index
        serial_buf_index = 0;

        // Reset got_number
        got_number = 0;

        // Set loop params
        serial_error = mds_set_pid_x(kp_x, ki_x, kd_x, sat_x, 0);

        if (serial_error != SERIAL_ERROR_NONE) serial_send_error();
        else                                   serial_send_ack();

        // Reset state machine
        serial_error = SERIAL_ERROR_NONE;
        serial_mode = SERIAL_MODE_IDLE;
        return;
    }

    else {
        // Reset got_number
        got_number = 0;

        // Record error
        serial_error = SERIAL_ERROR_TRAILING_CHARS;
        serial_mode = SERIAL_MODE_ERROR;
        return;
    }
}

static void serial_mode_param_y_handler(uint8_t c)
{
    DEBUG_PRINTF("param_y\r\n");

    if (c == ':') {
        // Zero out buffer
        serial_buf_index = 0;

        // Go to next mode
        serial_mode = SERIAL_MODE_PARAM_X_P;
        return;
    }
    else {
        // Go to error mode
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
        return;
    }
}

static void serial_mode_param_y_p_handler(uint8_t c)
{
    DEBUG_PRINTF("param_y_p\r\n");

    // Record numerical characters
    if (('0' <= c && c <= '9') || c == '.') {
        serial_buf[serial_buf_index] = c;
        serial_buf_index++;
    }
    
    // Separator
    else if (c == ',') {
        // Add terminator
        serial_buf[serial_buf_index] = '\0';

        // Interpret value
        kp_y = 0.0;
        if (string_to_float(serial_buf, &kp_y)) {
            // Bad value
            serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
            return;
        }

        // Next value
        serial_buf_index = 0;
        serial_mode = SERIAL_MODE_PARAM_X_I;
        return;
    }

    else {
        // Bad character
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
        return;
    }
}

static void serial_mode_param_y_i_handler(uint8_t c)
{
    DEBUG_PRINTF("param_y_i\r\n");

    // Record numerical characters
    if (('0' <= c && c <= '9') || c == '.') {
        serial_buf[serial_buf_index] = c;
        serial_buf_index++;
    }
    
    // Separator
    else if (c == ',') {
        // Add terminator
        serial_buf[serial_buf_index] = '\0';

        // Interpret value
        ki_y = 0.0;
        if (string_to_float(serial_buf, &ki_y)) {
            // Bad value
            serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
            return;
        }

        // Next value
        serial_buf_index = 0;
        serial_mode = SERIAL_MODE_PARAM_X_D;
        return;
    }

    else {
        // Bad character
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
    }
}

static void serial_mode_param_y_d_handler(uint8_t c)
{
    DEBUG_PRINTF("param_y_d\r\n");

    // Record numerical characters
    if (('0' <= c && c <= '9') || c == '.') {
        serial_buf[serial_buf_index] = c;
        serial_buf_index++;
    }
    
    // Separator
    else if (c == ',') {
        // Add terminator
        serial_buf[serial_buf_index] = '\0';

        // Interpret value
        kd_y = 0.0;
        if (string_to_float(serial_buf, &kd_y)) {
            // Bad value
            serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
            return;
        }

        // Next value
        serial_buf_index = 0;
        serial_mode = SERIAL_MODE_PARAM_X_S;
        return;
    }

    else {
        // Bad character
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
    }
}

static void serial_mode_param_y_s_handler(uint8_t c)
{
    DEBUG_PRINTF("param_y_s\r\n");

    // Static initialization only happens once
    static uint32_t got_number = 0;

    if (!got_number && c != '\r' && c != '\n') {
        // Numerical character
        if (('0' <= c && c <= '9') || c == '.') {
            serial_buf[serial_buf_index] = c;
            serial_buf_index++;
        }

        // Separator
        else if (c == ',') {
            // Add terminator
            serial_buf[serial_buf_index] = '\0';

            // Interpret value
            sat_y = 0.0;
            if (string_to_float(serial_buf, &sat_y)) {
                // Bad value
                serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
                return;
            }

            // We're done
            got_number = 1;
        }

        // Bad character
        else {
            serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
            return;
        }
    }

    else if (c == '\n' || c == '\r') {
        if (!got_number) {
            // Add terminator
            serial_buf[serial_buf_index] = '\0';

            // Interpret value
            sat_y = 0.0;
            if (string_to_float(serial_buf, &sat_y)) {
                // Bad value
                serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
                return;
            }
        }

        // Reset buffer index
        serial_buf_index = 0;

        // Reset got_number
        got_number = 0;

        // Set loop params
        serial_error = mds_set_pid_y(kp_y, ki_y, kd_y, sat_y, 0);

        if (serial_error != SERIAL_ERROR_NONE) serial_send_error();
        else                                   serial_send_ack();

        // Reset state machine
        serial_error = SERIAL_ERROR_NONE;
        serial_mode = SERIAL_MODE_IDLE;
        return;
    }

    else {
        // Reset got_number
        got_number = 0;

        // Record error
        serial_error = SERIAL_ERROR_TRAILING_CHARS;
        serial_mode = SERIAL_MODE_ERROR;
        return;
    }
}

static void serial_mode_location_handler(uint8_t c)
{
    DEBUG_PRINTF("location\r\n");

    if (c == ':') {
        // Zero out buffer
        serial_buf_index = 0;

        // Go to next mode
        serial_mode = SERIAL_MODE_LOCATION_X;
        return;
    }

    else {
        // Go to error mode
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
        return;
    }
}

static void serial_mode_location_x_handler(uint8_t c)
{
    DEBUG_PRINTF("location_x\r\n");

    // Record numerical characters
    if (('0' <= c && c <= '9') || c == '.') {
        serial_buf[serial_buf_index] = c;
        serial_buf_index++;
    }
    
    // Separator
    else if (c == ',') {
        // Add terminator
        serial_buf[serial_buf_index] = '\0';

        // Interpret value
        location_x = 0.0;
        if (string_to_float(serial_buf, &location_x)) {
            // Bad value
            serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
            return;
        }

        // Next value
        serial_buf_index = 0;
        serial_mode = SERIAL_MODE_LOCATION_Y;
        return;
    }

    else {
        // Bad character
        serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
    }
}

static void serial_mode_location_y_handler(uint8_t c)
{
    DEBUG_PRINTF("location_y\r\n");

    // Static initialization only happens once
    static uint32_t got_number = 0;

    if (!got_number && c != '\r' && c != '\n') {
        // Numerical character
        if (('0' <= c && c <= '9') || c == '.') {
            serial_buf[serial_buf_index] = c;
            serial_buf_index++;
        }

        // Separator
        else if (c == ',') {
            // Add terminator
            serial_buf[serial_buf_index] = '\0';

            // Interpret value
            location_y = 0.0;
            if (string_to_float(serial_buf, &location_y)) {
                // Bad value
                serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
                return;
            }

            // We're done
            got_number = 1;
        }

        // Bad character
        else {
            serial_set_error(SERIAL_ERROR_BAD_SEPARATOR, c);
            return;
        }
    }

    else if (c == '\n' || c == '\r') {
        if (!got_number) {
            // Add terminator
            serial_buf[serial_buf_index] = '\0';

            // Interpret value
            sat_y = 0.0;
            if (string_to_float(serial_buf, &sat_y)) {
                // Bad value
                serial_set_error(SERIAL_ERROR_PARSE_VALUE, c);
                return;
            }
        }

        // Reset buffer index
        serial_buf_index = 0;

        // Reset got_number
        got_number = 0;

        // Set location
        serial_error = mds_set_location(location_x, location_y);

        // Check error value
        if (serial_error != SERIAL_ERROR_NONE) serial_send_error();
        else                                   serial_send_ack();

        // Reset state machine
        serial_error = SERIAL_ERROR_NONE;
        serial_mode = SERIAL_MODE_IDLE;
        return;
    }

    else {
        // Reset got_number
        got_number = 0;

        // Record error
        serial_set_error(SERIAL_ERROR_TRAILING_CHARS, c);
        return;
    }
}

static void serial_mode_run_handler(uint8_t c)
{
    DEBUG_PRINTF("run\r\n");

    if (c == '\r' || c == '\n') {
        // Start the MDS
        serial_error = mds_start();

        // Print an error if we've recieved one, otherwise ack
        if (serial_error != SERIAL_ERROR_NONE) serial_send_error();
        else                                   serial_send_ack();

        // Reset state machine
        serial_mode = SERIAL_MODE_IDLE;
        serial_error = SERIAL_ERROR_NONE;
    }

    else {
        // Record the error
        serial_set_error(SERIAL_ERROR_TRAILING_CHARS, c);
    }
}

static void serial_mode_stop_handler(uint8_t c)
{
    DEBUG_PRINTF("stop\r\n");

    if (c == '\r' || c == '\n') {
        // Start the MDS
        serial_error = mds_stop();

        // Print an error if we've recieved one, otherwise ack
        if (serial_error != SERIAL_ERROR_NONE) serial_send_error();
        else                                   serial_send_ack();

        // Reset state machine
        serial_mode = SERIAL_MODE_IDLE;
        serial_error = SERIAL_ERROR_NONE;
    }

    else {
        // Record the error
        serial_set_error(SERIAL_ERROR_TRAILING_CHARS, c);
    }
}

static void serial_mode_calibrate_handler(uint8_t c)
{
    DEBUG_PRINTF("calibrate\r\n");

    if (c == '\r' || c == '\n') {
        // Start the MDS
        serial_error = mds_start_calibration();

        // Print an error if we've recieved one, otherwise ack
        if (serial_error != SERIAL_ERROR_NONE) serial_send_error();
        else                                   serial_send_ack();

        // Reset state machine
        serial_mode = SERIAL_MODE_IDLE;
        serial_error = SERIAL_ERROR_NONE;
    }

    else {
        // Record the error
        serial_set_error(SERIAL_ERROR_TRAILING_CHARS, c);
    }
}

static void serial_mode_calibrate_done_handler(uint8_t c)
{
    DEBUG_PRINTF("calibrate_done\r\n");

    float lower_x;
    float upper_x;
    float lower_y;
    float upper_y;

    if (c == '\r' || c == '\n') {
        // Start the MDS
        serial_error = mds_stop_calibration(&lower_x, &upper_x, &lower_y, &upper_y);

        // Print an error if we've recieved one
        if (serial_error != SERIAL_ERROR_NONE) serial_send_error();

        // Otherwise report back calibration values
        else {
            DEBUG_PRINTF("\r\n");

            PRINTF("%c%c:%f,%f,%f,%f\r\n", SERIAL_START, SERIAL_CALIBRATE, lower_x, upper_x, lower_y, upper_y);
        }

        // Reset state machine
        serial_mode = SERIAL_MODE_IDLE;
        serial_error = SERIAL_ERROR_NONE;
    }

    else {
        // Record the error
        serial_set_error(SERIAL_ERROR_TRAILING_CHARS, c);
    }
}

static void serial_mode_error_handler(uint8_t c)
{
    DEBUG_PRINTF("error\r\n");

    // Check for a finished transaction
    if (c == '\r' || c == '\n') {
        // Print error message
        serial_send_error();

        // Reset state machine
        serial_mode = SERIAL_MODE_IDLE;
        serial_error = SERIAL_ERROR_NONE;
    }
}

static inline void serial_send_ack(void)
{
    DEBUG_PRINTF("\r\n");

    PRINTF("%c%c\r\n", SERIAL_START, SERIAL_ACK);
}

static inline void serial_send_error(void)
{
    DEBUG_PRINTF("\r\n");

    PRINTF("%c%c:%s\r\n", SERIAL_START, SERIAL_ERROR, serial_error_string[serial_error]);
}

static inline void serial_set_error(serial_error_t err, uint8_t c)
{
    if (err != SERIAL_ERROR_NONE) {
        // If we got a stop, send error immediately
        if (c == '\r' || c == '\n') {
            // Set and print error
            serial_error = err;
            serial_send_error();

            // Reset state machine
            serial_mode = SERIAL_MODE_IDLE;
            serial_error = SERIAL_ERROR_NONE;
            return;
        }

        // Otherwise, got to error mode
        else {
            serial_mode = SERIAL_MODE_ERROR;
            serial_error = err;
            return;
        }
    }

    else {
        // Reset state machine
        serial_mode = SERIAL_MODE_IDLE;
        serial_error = SERIAL_ERROR_NONE;
    }
}

static int string_to_float(uint8_t* buf, float* number)
{
    uint32_t digit = 1;
    uint32_t exp = 0;
    uint32_t neg = 0;
    uint32_t n;
    int32_t i;
    uint32_t len;

    // Check pointers
    if (!number || !buf) return 1;

    // Initialize
    *number = 0.0;

    // Check if negative
    if (buf[0] == '-') {
        neg = 1;
        buf++;
    }

    // Find the end
    for (i = 0; buf[i]; i++);
    len = i;

    // Read back from the end
    for (i--; i >= 0; i--) {
        if (buf[i] == '.') {
            // Check if not the first decimal point
            if (exp ||
                (buf[len-1] = '.' && ((uint32_t) i) != len - 1)){
                return 1;
            }

            // Record decimal point location
            exp = len - i - 1;
        }

        else {
            // Record digit
            n = (uint32_t) (buf[i] - '0');

            // Check whether valid digit
            if (n > 9) return 1;

            // Add to accumulator
            *number += ((float) n) * digit;
            digit *= 10;
        }
    }

    // Scale appropriately
    *number /= pow(10.0, exp);

    if (neg) *number = -(*number);

    return 0;
}

/* --- INTERRUPT HANDLERS --------------------------------------------------- */

void HardFaultVector(void)
{
    palSetPad(GPIOE, GPIOE_LED6_GREEN);
    while(1);
}

void BusFaultVector(void)
{
    palSetPad(GPIOE, GPIOE_LED6_GREEN);
    palSetPad(GPIOE, GPIOE_LED4_BLUE);
    while(1);
}

void UsageFaultVector(void)
{
    palSetPad(GPIOE, GPIOE_LED4_BLUE);
    while(1);
}

