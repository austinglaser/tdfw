/* --- INCLUDES ------------------------------------------------------------- */

#include <iostream>
#include <glib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

using namespace std;

/* --- PRIVATE MACROS ------------------------------------------------------- */

/**
 * @brief   Extracts milliseconds from a timeval struct
 *  
 * Gets the current time (in milliseconds) from a timeval struct (defined in
 * sys/time.h)
 *
 * @param[in] tv:   An instance of <struct timeval>
 *
 * @return:         The system time (in milliseconds)
 */
#define TIME_MS(tv) ((((tv).tv_sec)*1000.0) + (((tv).tv_usec)/1000.0))

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

/**
 * @brief Interrupt used when the player scores
 *
 * @param[in]   channel:    The channel the interrupt occurred on
 * @param       condition:  Unused
 * @param       user_data:  Unused
 *
 * @return:                 Success (true) or failure (false)
 */
static gboolean player_score_interrupt(GIOChannel* channel, GIOCondition condition, gpointer user_data);

/**
 * @brief Interrupt used when the AHA scores
 *
 * @param[in]   channel:    The channel the interrupt occurred on
 * @param       condition:  Unused
 * @param       user_data:  Unused
 *
 * @return:                 Success (true) or failure (false)
 */
static gboolean aha_score_interrupt(GIOChannel* channel, GIOCondition condition, gpointer user_data);

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

/**
 * @brief   Main application entry point
 *
 * @note    This application will only work correctly if setup.sh has been previously
 *          run to configure the pins as inputs with interrupts on the rising edge
 */
int main(void)
{
    int player_switch_val_fd;
    int aha_switch_val_fd;
    GIOChannel* aha_switch_val;
    GIOChannel* player_switch_val;
    GIOCondition cond;
    GMainLoop* loop;

    // Create main loop
	loop = g_main_loop_new(0, 0);

    // Open value file descriptors
	player_switch_val_fd    = open("/sys/class/gpio/gpio58_pi10/value",     O_RDONLY | O_NONBLOCK);
	aha_switch_val_fd       = open("/sys/class/gpio/gpio59_pi11/value",     O_RDONLY | O_NONBLOCK);

    // Get the corresponding IO channels
    aha_switch_val      = g_io_channel_unix_new(aha_switch_val_fd);
    player_switch_val   = g_io_channel_unix_new(player_switch_val_fd);

    // Configure the interrupts
    cond = GIOCondition(G_IO_PRI);
    g_io_add_watch(player_switch_val, cond, player_score_interrupt, 0);
    g_io_add_watch(aha_switch_val, cond, aha_score_interrupt, 0);

    // Start main loop
    g_main_loop_run(loop);
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static gboolean player_score_interrupt(GIOChannel* channel, GIOCondition condition, gpointer user_data)
{
    (void) condition;
    (void) user_data;

    gchar buf[5];
    gsize buf_sz = 5;
    GError* error = 0;
    gsize bytes_read = 0; 
    static bool first = true;
    static struct timeval last_player;
    struct timeval now;

    // Read channel again to clear the interrupt
    g_io_channel_seek_position(channel, 0, G_SEEK_SET, 0);
    g_io_channel_read_chars(channel, buf, buf_sz - 1, &bytes_read, &error);

    // If this is the first interrupt, don't record a score
    if (first) {
        first = false;
        gettimeofday(&last_player, NULL);
    }
    else {
        // Check the time of the last interrupt to provide some debouncing
        gettimeofday(&now, NULL);
        if ((TIME_MS(now) - TIME_MS(last_player)) > 1000) {
            cout << 'u' << endl;
            last_player = now;
        }
    }

    // Success
    return true;
}

static gboolean aha_score_interrupt(GIOChannel* channel, GIOCondition condition, gpointer user_data)
{
    (void) condition;
    (void) user_data;

    gchar buf[5];
    gsize buf_sz = 5;
    GError* error = 0;
    gsize bytes_read = 0; 
    static bool first = true;
    struct timeval now;
    static struct timeval last_aha;

    // Read channel again to clear the interrupt
    g_io_channel_seek_position(channel, 0, G_SEEK_SET, 0);
    g_io_channel_read_chars(channel, buf, buf_sz - 1, &bytes_read, &error);

    // If this is the first interrupt, don't record a score
    if (first) {
        first = false;
        gettimeofday(&last_aha, NULL);
    }
    else {
        // Check the time of the last interrupt to provide some debouncing
        gettimeofday(&now, NULL);
        if ((TIME_MS(now) - TIME_MS(last_aha)) > 1000) {
            cout << 'a' << endl;
            last_aha = now;
        }
    }

    // Success
    return true;
}
