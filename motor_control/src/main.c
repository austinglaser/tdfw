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
#include <stdio.h>

// Chibios
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

// Project
#include "mds.h"

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

/*
 * Application entry point.
 */
int main(void)
{
    uint8_t c;

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

    while (TRUE) {
        // Get and echo
        c = sdGet(&SD1);

        // Interpret
        switch(c) {
            case 'c':
            case 'C':
                chprintf((BaseSequentialStream*) &SD1, "calibrating...\r\n");
                mds_start_calibration();
                break;

            case 's':
            case 'S':
                mds_stop_calibration();
                chprintf((BaseSequentialStream*) &SD1, "done calibrating...\r\n");
                break;

            default:
                // do nothing
                break;
        }
    }
}
