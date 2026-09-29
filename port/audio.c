#include "port_runtime.h"

#include <stdio.h>

static int s_audio_loaded;

int port_audio_silent_load(const char *driver_name)
{
    s_audio_loaded = 1;
    fprintf(stderr, "PORT audio: silent driver adapter selected (%s)\n",
            driver_name != NULL ? driver_name : "default");
    return 0;
}

void port_audio_shutdown(void)
{
    s_audio_loaded = 0;
}

/* Keep the C caller's startup contract. DOS .DRV bytes are never executed by
   the 32-bit host; later audio work routes every observed entry through a
   typed port dispatch table. */
int16_t audio_load_driver(char *filename, int16_t unused, int16_t signature)
{
    (void)unused;
    (void)signature;
    return (int16_t)port_audio_silent_load(filename);
}
