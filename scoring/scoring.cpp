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

#define TIME_MS(tv) ((((tv).tv_sec)*1000.0) + (((tv).tv_usec)/1000.0))

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

static gboolean encoder_int_x(GIOChannel* channel, GIOCondition condition, gpointer user_data);
static gboolean encoder_int_y(GIOChannel* channel, GIOCondition condition, gpointer user_data);

/* --- PRIVATE VARIABLES ---------------------------------------------------- */

static int enc_x_val_fd;
static int enc_y_val_fd;

struct timeval last_aha;
struct timeval last_player;

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

int main(void)
{
	GMainLoop* loop = g_main_loop_new(0, 0);

	enc_x_val_fd    = open("/sys/class/gpio/gpio58_pi10/value",     O_RDONLY | O_NONBLOCK);
	enc_y_val_fd    = open("/sys/class/gpio/gpio59_pi11/value",     O_RDONLY | O_NONBLOCK);

    GIOChannel* enc_y_val   = g_io_channel_unix_new(enc_y_val_fd);
    GIOChannel* enc_x_val   = g_io_channel_unix_new(enc_x_val_fd);

    // Configure the interrupts
    GIOCondition cond = GIOCondition(G_IO_PRI);
    g_io_add_watch(enc_x_val, cond, encoder_int_x, 0);
    g_io_add_watch(enc_y_val, cond, encoder_int_y, 0);

    // Start main loop
    g_main_loop_run(loop);
}

/* --- PRIVATE FUNCTION DEFINITIONS ----------------------------------------- */

static gboolean encoder_int_x(GIOChannel* channel, GIOCondition condition, gpointer user_data)
{
    (void) condition;
    (void) user_data;

    gchar buf[5];
    gsize buf_sz = 5;
    GError* error = 0;
    gsize bytes_read = 0; 
    static bool first = true;
    struct timeval now;

    // Read channel again to clear the interrupt
    g_io_channel_seek_position(channel, 0, G_SEEK_SET, 0);
    g_io_channel_read_chars(channel, buf, buf_sz - 1, &bytes_read, &error);

    if (first) {
        first = false;
        gettimeofday(&last_player, NULL);
    }
    else {
        gettimeofday(&now, NULL);
        if ((TIME_MS(now) - TIME_MS(last_player)) > 1000) {
            cout << 'u' << endl;
            last_player = now;
        }
    }

    return true;
}

static gboolean encoder_int_y(GIOChannel* channel, GIOCondition condition, gpointer user_data)
{
    (void) condition;
    (void) user_data;

    gchar buf[5];
    gsize buf_sz = 5;
    GError* error = 0;
    gsize bytes_read = 0; 
    static bool first = true;
    struct timeval now;

    // Read channel again to clear the interrupt
    g_io_channel_seek_position(channel, 0, G_SEEK_SET, 0);
    g_io_channel_read_chars(channel, buf, buf_sz - 1, &bytes_read, &error);

    if (first) {
        first = false;
        gettimeofday(&last_aha, NULL);
    }
    else {
        gettimeofday(&now, NULL);
        if ((TIME_MS(now) - TIME_MS(last_aha)) > 1000) {
            cout << 'a' << endl;
            last_aha = now;
        }
    }

    return true;
}
