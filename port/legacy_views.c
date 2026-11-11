#include "port_runtime.h"

/* The recovered SCREEN_RECT is a view of the selected font header. The DOS
   far pointer at word_405FE consists of offset zero followed by fontdefseg;
   selecting a font therefore changes both text rendering and this view.
   Its width/height/bottom names actually address foreground/background/
   line height. Keep the alias live instead of copying the default values. */
typedef struct PortLegacyScreenRect {
    int16_t foreground;
    int16_t background;
    int16_t font_header_middle[7];
    int16_t line_height;
} PortLegacyScreenRect;

_Static_assert(sizeof(PortLegacyScreenRect) == 20, "DOS line-editor font view");
_Static_assert(offsetof(PortLegacyScreenRect, foreground) == 0, "font foreground");
_Static_assert(offsetof(PortLegacyScreenRect, background) == 2, "font background");
_Static_assert(offsetof(PortLegacyScreenRect, line_height) == 18, "font line height");

extern uint8_t fontdef_default[1408];
PortLegacyScreenRect *line_input_font_header =
    (PortLegacyScreenRect *)fontdef_default;

void port_line_input_select_font(const void *font_data)
{
    line_input_font_header = (PortLegacyScreenRect *)font_data;
}

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
