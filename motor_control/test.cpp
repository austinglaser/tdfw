#include <iostream>
#include <glib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <stdio.h>

using namespace std;

static gboolean encoder_int(GIOChannel* channel, GIOCondition condition, gpointer user_data);
std::string exec(char* cmd);

int main(void)
{
	GMainLoop* loop = g_main_loop_new(0, 0);

    exec("echo 60 > /sys/class/gpio/export");
    exec("echo both > /sys/class/gpio/gpio60_pi10/edge");
    exec("echo in > /sys/class/gpio/gpio60_pi10/direction");

	int fd = open("/sys/class/gpio/gpio60_pi10/value", O_RDONLY | O_NONBLOCK);
    GIOChannel* channel = g_io_channel_unix_new(fd);
    GIOCondition cond = GIOCondition(G_IO_PRI);
    guint id = g_io_add_watch(channel, cond, encoder_int, 0);

    g_main_loop_run(loop);
}

static gboolean encoder_int(GIOChannel* channel, GIOCondition condition, gpointer user_data)
{
    (void) condition;
    (void) user_data;

    cout << "Encoder!" << endl;
    gchar buf[5];
    gsize buf_sz = 5;
    GError *error = 0;
    gsize bytes_read = 0; 
    g_io_channel_seek_position( channel, 0, G_SEEK_SET, 0 );
    GIOStatus rc = g_io_channel_read_chars(channel,
                                           buf, buf_sz - 1,
                                           &bytes_read,
                                           &error);
    cerr << "rc:" << rc << "  data:" << buf << endl;

    return true;
}


// From http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c
std::string exec(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}
