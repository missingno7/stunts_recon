#include "port_runtime.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

typedef void (*PortExitHandler)(void);

/* The original `_exitlistfuncs` occupies 44 bytes: eleven far-pointer words
   pairs. Registration searches ten entries; the final entry is the permanent
   zero-segment sentinel observed by reverse dispatch. */
enum { PORT_EXIT_HANDLER_CAPACITY = 10 };
static PortExitHandler s_exit_handlers[PORT_EXIT_HANDLER_CAPACITY + 1];

int16_t add_exit_handler(PortExitHandler callback)
{
    unsigned i;
    uint16_t original_offset;

    /* The DOS routine left AX holding the callback offset on success. Its
       value is not consumed by the indexed game callers, but retain the
       low-word return shape recorded by the port declarations. */
    original_offset = (uint16_t)(uintptr_t)callback;
    if (callback == NULL)
        return (int16_t)original_offset;

    for (i = 0; i < PORT_EXIT_HANDLER_CAPACITY; ++i) {
        if (s_exit_handlers[i] == callback)
            return (int16_t)original_offset;
        if (s_exit_handlers[i] == NULL) {
            s_exit_handlers[i] = callback;
            return (int16_t)original_offset;
        }
    }

    /* The original overflow branch calls the game's fatal service and does
       not return. Keep a return value only for host adapters whose fatal
       service unexpectedly returns. */
    fatal_error("EXIT LIST OVERFLOW");
    return -1;
}

void call_exitlist(void)
{
    int i;

    /* The original list is persistent: call_exitlist does not erase entries.
       A callback may remove its own timer/input resources exactly as before. */
    for (i = PORT_EXIT_HANDLER_CAPACITY; i >= 0; --i) {
        PortExitHandler callback = s_exit_handlers[i];
        if (callback != NULL)
            callback();
    }
}

void port_guest_exit(int status)
{
    char reason[64];

    call_exitlist();

    /* DOS `_exit(0)` terminates the process after invoking this table. The
       SDL host owns the process and asks the guest thread to unwind instead.
       This adapter also handles the game's direct CRT exit(status) call. */
    (void)snprintf(reason, sizeof(reason), "game requested exit (status %d)",
                   status);
    port_guest_set_exit_status(status);
    port_guest_request_stop(reason);
    port_guest_check_stop();
}

void call_exitlist2(void)
{
    port_guest_exit(0);
}

void debug_printf_text(const char *format, ...)
{
    va_list args;

    if (format == NULL)
        return;
    va_start(args, format);
    (void)vfprintf(stderr, format, args);
    va_end(args);
    (void)fflush(stderr);
}
