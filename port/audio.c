#include "port_runtime.h"

#include <stdio.h>
#include <string.h>

static int s_audio_loaded;
extern char g_drvaudiocode[14];

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

int port_audio_is_silent(void)
{
    return s_audio_loaded;
}

/* Keep the C caller's startup contract. DOS .DRV bytes are never executed by
   the 32-bit host; later audio work routes every observed entry through a
   typed port dispatch table. */
int16_t audio_load_driver(char *filename, int16_t unused, int16_t signature)
{
    const char *base = filename;
    const char *backslash;
    const char *slash;
    (void)unused;
    (void)signature;
    if (filename != NULL) {
        backslash = strrchr(filename, '\\');
        slash = strrchr(filename, '/');
        if (backslash != NULL && (slash == NULL || backslash > slash))
            base = backslash + 1;
        else if (slash != NULL)
            base = slash + 1;
        if (base[0] != '\0' && base[1] != '\0') {
            /* The game loader derives this two-character resource prefix from
               the selected driver filename before trying voice resources. */
            g_drvaudiocode[0] = base[0];
            g_drvaudiocode[1] = base[1];
            g_drvaudiocode[2] = '\0';
        }
    }
    return (int16_t)port_audio_silent_load(filename);
}
