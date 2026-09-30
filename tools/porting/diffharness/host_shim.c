#include "port_runtime.h"

#include <stdio.h>
#include <stdlib.h>

uint8_t port_framebuffer[PORT_VIDEO_MEMORY_BYTES];

uint8_t *port_video_pixels(void)
{
    return port_framebuffer;
}

void port_video_publish(const char *reason)
{
    (void)reason;
}

void port_guest_unwind(const char *reason)
{
    fprintf(stderr, "diffharness port abort: %s\n", reason ? reason : "unknown");
    abort();
}

void port_stub_fail(const char *symbol)
{
    port_guest_unwind(symbol);
}
