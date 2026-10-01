#include "port_runtime.h"
#include "transition_work.h"

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

/* Routine parity observes pixels and ABI without waiting on a host clock.
   Frozen instruction counts separately verify the transition work model. */
void port_video_transition_begin(PortVideoTransition *transition)
{
    transition->work_units = PORT_TRANSITION_FIXED_WORK;
}

void port_video_transition_advance(PortVideoTransition *transition, uint32_t work)
{
    transition->work_units += work;
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
