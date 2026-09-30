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
