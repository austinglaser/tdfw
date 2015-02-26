/* --- INCLUDES ------------------------------------------------------------- */

#include <iostream>
#include <glib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

using namespace std;

/* --- PRIVATE FUNCTION PROTOTYPES ------------------------------------------ */

static gboolean encoder_int_x(GIOChannel* channel, GIOCondition condition, gpointer user_data);
static gboolean encoder_int_y(GIOChannel* channel, GIOCondition condition, gpointer user_data);
static size_t str_to_buf(gchar * buf, char * str);

/* --- PRIVATE VARIABLES ---------------------------------------------------- */

static int enc_x_edge_fd;
static int enc_x_dir_fd;
static int enc_x_val_fd;
static int enc_y_edge_fd;
static int enc_y_dir_fd;
static int enc_y_val_fd;

static volatile unsigned int player_score = 0;
static volatile unsigned int aha_score = 0;

/* --- PUBLIC FUNCTION DEFINITIONS ------------------------------------------ */

int main(void)
{
	GMainLoop* loop = g_main_loop_new(0, 0);
    gchar buf[16];
    gsize count;
    gsize n_bytes;
    GError* error = 0;

    // Open the general GPIO control files
    int gpio_export_fd  = open("/sys/class/gpio/export", O_WRONLY | O_NONBLOCK);
    GIOChannel* gpio_export = g_io_channel_unix_new(gpio_export_fd);

    // Export encoder pins
    count = str_to_buf(buf, "60\n");
    g_io_channel_write_chars(gpio_export, buf, count, &n_bytes, &error);
    count = str_to_buf(buf, "61\n");
    g_io_channel_write_chars(gpio_export, buf, count, &n_bytes, &error);

    // Open the pin control files
	enc_x_edge_fd   = open("/sys/class/gpio/gpio60_pi10/edge",      O_WRONLY | O_NONBLOCK);
	enc_x_dir_fd    = open("/sys/class/gpio/gpio60_pi10/direction", O_WRONLY | O_NONBLOCK);
	enc_x_val_fd    = open("/sys/class/gpio/gpio60_pi10/value",     O_RDONLY | O_NONBLOCK);
    GIOChannel* enc_x_edge  = g_io_channel_unix_new(enc_x_edge_fd);
    GIOChannel* enc_x_dir   = g_io_channel_unix_new(enc_x_dir_fd);
    GIOChannel* enc_x_val   = g_io_channel_unix_new(enc_x_val_fd);

	enc_y_edge_fd   = open("/sys/class/gpio/gpio61_pi11/edge",      O_WRONLY | O_NONBLOCK);
	enc_y_dir_fd    = open("/sys/class/gpio/gpio61_pi11/direction", O_WRONLY | O_NONBLOCK);
	enc_y_val_fd    = open("/sys/class/gpio/gpio61_pi11/value",     O_RDONLY | O_NONBLOCK);
    GIOChannel* enc_y_edge  = g_io_channel_unix_new(enc_y_edge_fd);
    GIOChannel* enc_y_dir   = g_io_channel_unix_new(enc_y_dir_fd);
    GIOChannel* enc_y_val   = g_io_channel_unix_new(enc_y_val_fd);

    // Configure the pins
    count = str_to_buf(buf, "rising\n");
    g_io_channel_write_chars(enc_x_edge, buf, count, &n_bytes, &error);
    g_io_channel_write_chars(enc_y_edge, buf, count, &n_bytes, &error);

    count = str_to_buf(buf, "in\n");
    g_io_channel_write_chars(enc_x_dir, buf, count, &n_bytes, &error);
    g_io_channel_write_chars(enc_y_dir, buf, count, &n_bytes, &error);
    
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

    // Read channel again to clear the interrupt
    g_io_channel_seek_position(channel, 0, G_SEEK_SET, 0);
    GIOStatus rc = g_io_channel_read_chars(channel, buf, buf_sz - 1, &bytes_read, &error);

    cout << "Player scored!" << endl;
    cout << "AHA: " << aha_score << " Player: " << player_score << endl;

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

    // Read channel again to clear the interrupt
    g_io_channel_seek_position(channel, 0, G_SEEK_SET, 0);
    GIOStatus rc = g_io_channel_read_chars(channel, buf, buf_sz - 1, &bytes_read, &error);

    cout << "AHA scored!" << endl;
    cout << "AHA: " << aha_score << " Player: " << player_score << endl;

    return true;
}

static size_t str_to_buf(gchar * buf, char * str)
{
    uint32_t i;
    for (i = 0; str[i]; i++) buf[i] = str[i];

    return strlen(str);
}
