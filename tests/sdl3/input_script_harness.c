#include "../../port/port_runtime.h"
#include <stdio.h>
#include "../../port/input_script.c"

static int16_t mouse_x, mouse_y;

void port_input_mouse_set(int16_t x, int16_t y)
{
    mouse_x = x;
    mouse_y = y;
}

void port_input_mouse_set_buttons(uint16_t buttons) { (void)buttons; }
void port_input_apply_dos_scancode(uint8_t code) { (void)code; }

void port_trace_input_mouse(uint64_t sequence, uint64_t scheduled_ns,
                            uint64_t actual_ns, double u, double v,
                            uint16_t buttons)
{
    (void)scheduled_ns; (void)actual_ns; (void)u; (void)v;
    printf("M %llu %d %d %u\n", (unsigned long long)sequence,
           mouse_x, mouse_y, buttons);
}

void port_trace_input_keyboard(uint64_t sequence, uint64_t scheduled_ns,
                               uint64_t actual_ns, const uint8_t *codes,
                               size_t count)
{
    size_t i;
    (void)scheduled_ns; (void)actual_ns;
    printf("K %llu", (unsigned long long)sequence);
    for (i = 0; i < count; ++i)
        printf(" %u", codes[i]);
    putchar('\n');
}

int main(int argc, char **argv)
{
    if (argc != 2 || !port_input_script_load(argv[1]))
        return 1;
    port_input_script_pump(UINT64_MAX);
    return 0;
}
