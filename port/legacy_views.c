#include "port_runtime.h"

/* The game stores this SCREEN_RECT behind the far pointer at word_405FE.
   The pointer targets load-image offset 0x2B1F0. These fields preserve the
   target's 20-byte SCREEN_RECT view consumed by read_line_helper*. */
typedef struct PortLegacyScreenRect {
    int16_t width;
    int16_t height;
    int16_t reserved[7];
    int16_t bottom;
} PortLegacyScreenRect;

static PortLegacyScreenRect s_line_input_screen_rect = {
    3, 0, {0, 0, 0, 1, 8, 8, 8}, 8
};

/* This typed host view replaces the zero-filled unresolved data placeholder. */
PortLegacyScreenRect *line_input_screen_rect = &s_line_input_screen_rect;

/* Medium-model callers pass the final 32-bit timeout as two stack words.
   Native cdecl needs six arguments explicitly rather than relying on adjacent
   words of a single host argument. */
extern int16_t call_read_line(char *, int16_t, int16_t, int16_t, int16_t, int16_t);
int16_t port_call_read_line(char *buffer, int16_t max_length, int16_t x,
                            int16_t y, int32_t timeout)
{
    return call_read_line(buffer, max_length, x, y,
                          (int16_t)(uint16_t)timeout,
                          (int16_t)(uint16_t)((uint32_t)timeout >> 16));
}
