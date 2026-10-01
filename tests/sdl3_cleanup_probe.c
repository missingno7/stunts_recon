#include "../port/port_runtime.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern int16_t add_exit_handler(void (*callback)(void));
extern void call_exitlist(void);
extern void call_exitlist2(void);
extern void port_guest_exit(int status);
extern void debug_printf_text(const char *format, ...);

static char s_sequence[64];
static unsigned s_used;
static unsigned s_overflow_count;
static unsigned s_stop_request_count;
static unsigned s_stop_check_count;
static unsigned s_exit_status_count;
static char s_stop_reasons[2][64];
static int s_exit_statuses[2];

void fatal_error(const char *format, ...)
{
    if (format == NULL || strcmp(format, "EXIT LIST OVERFLOW") != 0)
        s_overflow_count = 1000;
    else
        ++s_overflow_count;
}

void port_guest_request_stop(const char *reason)
{
    static const char *const expected_reasons[2] = {
        "game requested exit (status 0)",
        "game requested exit (status 1)"
    };
    if (reason == NULL || s_stop_request_count >= 2u ||
        s_exit_status_count != s_stop_request_count + 1u ||
        strcmp(reason, expected_reasons[s_stop_request_count]) != 0) {
        s_stop_request_count = 1000;
    } else {
        (void)snprintf(s_stop_reasons[s_stop_request_count],
                       sizeof(s_stop_reasons[0]), "%s", reason);
        ++s_stop_request_count;
    }
}

void port_guest_set_exit_status(int status)
{
    if (s_exit_status_count >= 2u) {
        s_exit_status_count = 1000;
        return;
    }
    s_exit_statuses[s_exit_status_count++] = status;
}

void port_guest_check_stop(void)
{
    ++s_stop_check_count;
}

#define HANDLER(number) \
    static void handler##number(void) \
    { \
        s_sequence[s_used++] = (char)('0' + (number)); \
    }

HANDLER(0)
HANDLER(1)
HANDLER(2)
HANDLER(3)
HANDLER(4)
HANDLER(5)
HANDLER(6)
HANDLER(7)
HANDLER(8)
HANDLER(9)
HANDLER(10)

static int check(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAILED: %s\n", message);
        return 0;
    }
    return 1;
}

int main(void)
{
    static void (*const callbacks[10])(void) = {
        handler0, handler1, handler2, handler3, handler4,
        handler5, handler6, handler7, handler8, handler9
    };
    unsigned i;

    for (i = 0; i < 10; ++i) {
        (void)add_exit_handler(callbacks[i]);
        if (i == 0)
            (void)add_exit_handler(callbacks[i]);
    }
    if (!check(add_exit_handler(NULL) == 0,
               "null callback should not consume an exit slot"))
        return 1;
    if (!check(add_exit_handler(handler10) == -1 && s_overflow_count == 1,
               "the eleventh distinct callback should overflow"))
        return 2;

    call_exitlist();
    if (!check(s_used == 10 && memcmp(s_sequence, "9876543210", 10) == 0,
               "callbacks should run once in reverse registration order"))
        return 3;
    call_exitlist();
    if (!check(s_used == 20 &&
               memcmp(s_sequence + 10, "9876543210", 10) == 0,
               "dispatch should preserve registrations for a later call"))
        return 4;

    call_exitlist2();
    if (!check(s_used == 30 &&
               memcmp(s_sequence + 20, "9876543210", 10) == 0 &&
               s_stop_request_count == 1 && s_stop_check_count == 1 &&
               s_exit_status_count == 1 && s_exit_statuses[0] == 0 &&
               strcmp(s_stop_reasons[0], "game requested exit (status 0)") == 0,
               "call_exitlist2 should clean up then request guest unwind"))
        return 5;

    port_guest_exit(1);
    if (!check(s_used == 40 &&
               memcmp(s_sequence + 30, "9876543210", 10) == 0 &&
               s_stop_request_count == 2 && s_stop_check_count == 2 &&
               s_exit_status_count == 2 && s_exit_statuses[1] == 1 &&
               strcmp(s_stop_reasons[1], "game requested exit (status 1)") == 0,
               "the game CRT-exit adapter should reuse the cleanup boundary"))
        return 6;

    debug_printf_text("cleanup-test:%d/%s\n", 7, "ok");
    puts("SDL3 cleanup adapter checks passed");
    return 0;
}
