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

// Standard
#include <math.h>

// Chibios
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

// Project
#include "mds.h"

static WORKING_AREA(drive_thread_wa, 256);
static msg_t drive_thread_f(void * context);

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

static inline float string_to_float(uint8_t * buf);

/*
 * Application entry point.
 */
int main(void)
{
    uint8_t c;
    uint8_t buf[128];
    uint32_t i;
    float kp = 0.0;
    float ki = 0.0;
    float kd = 0.0;
    float sat = 5.0;

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    /*
     * Activates the serial driver 1 using the driver default configuration.
     * PA9(TX) and PA10(RX) are routed to USART1.
     */
    sdStart(&SD1, NULL);
    palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(7));

    // Init mds
    mds_init();

    chThdCreateStatic(drive_thread_wa, sizeof(drive_thread_wa),
                      NORMALPRIO,
                      drive_thread_f, NULL);

    while (TRUE) {
        // Get and echo
        c = sdGet(&SD1);

        // Interpret
        switch(c) {
            case 'c':
                mds_start_calibration();
                chprintf((BaseSequentialStream*) &SD1, "calibrating...\r\n");
                break;

            case 's':
                mds_stop_calibration();
                chprintf((BaseSequentialStream*) &SD1, "done calibrating...\r\n");
                break;

            case 'g':
                mds_start();
                chprintf((BaseSequentialStream*) &SD1, "starting...\r\n");
                break;

            case 'o':
                mds_stop();
                chprintf((BaseSequentialStream*) &SD1, "stopping...\r\n");
                break;

            case 'p':
                mds_stop();
                for (i = 0; i < 128; i++) {
                    buf[i] = sdGet(&SD1);
                    if (buf[i] == '\r' || buf[i] == '\n') {
                        buf[i] = '\0';
                        sdPut(&SD1, '\r');
                        sdPut(&SD1, '\n');
                        break;
                    }
                    else {
                        sdPut(&SD1, buf[i]);
                    }
                }

                kp = string_to_float(buf);
                chprintf((BaseSequentialStream*) &SD1, "updating kp=%f\r\n",kp);
                mds_set_pid_x(kp, ki, kd, sat, 0);
                break;

            case 'i':
                mds_stop();

                for (i = 0; i < 128; i++) {
                    buf[i] = sdGet(&SD1);
                    if (buf[i] == '\r' || buf[i] == '\n') {
                        buf[i] = '\0';
                        sdPut(&SD1, '\r');
                        sdPut(&SD1, '\n');
                        break;
                    }
                    else {
                        sdPut(&SD1, buf[i]);
                    }
                }

                ki = string_to_float(buf);
                chprintf((BaseSequentialStream*) &SD1, "updating ki=%f\r\n",ki);
                mds_set_pid_x(kp, ki, kd, sat, 0);
                break;

            case 'd':
                mds_stop();
                for (i = 0; i < 128; i++) {
                    buf[i] = sdGet(&SD1);
                    if (buf[i] == '\r' || buf[i] == '\n') {
                        buf[i] = '\0';
                        sdPut(&SD1, '\r');
                        sdPut(&SD1, '\n');
                        break;
                    }
                    else {
                        sdPut(&SD1, buf[i]);
                    }
                }

                kd = string_to_float(buf);
                chprintf((BaseSequentialStream*) &SD1, "updating kd=%f\r\n",kd);
                mds_set_pid_x(kp, ki, kd, sat, 0);
                break;

            case 'a':
                mds_stop();
                for (i = 0; i < 128; i++) {
                    buf[i] = sdGet(&SD1);
                    if (buf[i] == '\r' || buf[i] == '\n') {
                        buf[i] = '\0';
                        sdPut(&SD1, '\r');
                        sdPut(&SD1, '\n');
                        break;
                    }
                    else {
                        sdPut(&SD1, buf[i]);
                    }
                }

                sat = string_to_float(buf);
                chprintf((BaseSequentialStream*) &SD1, "updating sat=%f\r\n",sat);
                mds_set_pid_x(kp, ki, kd, sat, 0);
                break;

            default:
                // do nothing
                break;
        }
    }
}

static msg_t drive_thread_f(void * context)
{
    (void) context;

    while (TRUE) {
        mds_set_location(400.0, 50.0);
        chThdSleepMilliseconds(1000);
        mds_set_location(200.0, 50.0);
        chThdSleepMilliseconds(1000);
    }

    // Pedantic
    return 0;
}

static inline float string_to_float(uint8_t * buf)
{
    float number = 0.0;
    uint32_t digit = 1;
    uint32_t n;
    int32_t i;

    for (i = 0; buf[i]; i++);
    for (i--; i >= 0; i--) {
        n = (uint32_t) (buf[i] - '0');
        if (n > 9) return 0.0;
        
        number += ((float) n) * digit;
        digit *= 10;
    }

    number /= 1000000.0;

    return number;
}
