#include "port_runtime.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Recovered seg012:6397 compares SS with DS and returns one only when equal.
   The i686 Windows host uses a flat data/stack selector model, so observing
   the real selectors preserves that adapter behavior without introducing a
   game-specific success path. */
int16_t compare_ds_ss(void)
{
#if defined(__i386__) && defined(__GNUC__)
    uint16_t data_segment;
    uint16_t stack_segment;
    __asm__ volatile ("movw %%ds, %0" : "=r" (data_segment));
    __asm__ volatile ("movw %%ss, %0" : "=r" (stack_segment));
    return data_segment == stack_segment ? 1 : 0;
#else
    return 1;
#endif
}

typedef struct PortIntRegisters {
    uint16_t ax, bx, cx, dx, si, di, cflag;
} PortIntRegisters;

extern uint16_t mousehorscale;

int int86(int interrupt_number, void *input, void *output)
{
    PortIntRegisters regs;
    memset(&regs, 0, sizeof(regs));
    if (input != NULL)
        memcpy(&regs, input, sizeof(regs));

    if (interrupt_number == 0x15 && regs.ax == 0xC201) {
        /* BIOS mouse initialization selector used by src/seg017_mouse_whole.c. */
    } else if (interrupt_number == 0x33) {
        switch (regs.ax) {
        case 0: /* Reset: AX=ffffh if present, BX=button count. */
            regs.ax = 0xFFFF;
            regs.bx = 3;
            break;
        case 1: /* Show cursor; the host renderer owns the cursor presentation. */
        case 2: /* Hide cursor. */
        case 15: /* Store-compatible pixel-ratio call; SDL coordinates are pixels. */
            break;
        case 3: {
            int16_t x, y;
            uint16_t buttons;
            port_input_mouse_get(&x, &y, &buttons);
            regs.bx = buttons;
            /* Mode 13h exposes 640 DOS mouse columns for the game's 320-wide
               logical screen when mousehorscale is set. SDL events stay in
               logical pixels; convert only at the INT 33h boundary. */
            regs.cx = (uint16_t)((uint16_t)x << mousehorscale);
            regs.dx = (uint16_t)y;
            break;
        }
        case 4:
            port_input_mouse_set((int16_t)(regs.cx >> mousehorscale),
                                 (int16_t)regs.dx);
            break;
        case 7:
            port_input_mouse_set_x_bounds(
                (int16_t)(regs.cx >> mousehorscale),
                (int16_t)(regs.dx >> mousehorscale));
            break;
        case 8:
            port_input_mouse_set_y_bounds((int16_t)regs.cx, (int16_t)regs.dx);
            break;
        default: {
            char reason[96];
            snprintf(reason, sizeof(reason),
                     "unsupported INT 33h mouse function AX=%04x", regs.ax);
            port_stub_fail(reason);
            break;
        }
        }
    } else {
        char reason[96];
        snprintf(reason, sizeof(reason), "unsupported interrupt %02xh AX=%04x",
                 interrupt_number & 0xFF, regs.ax);
        port_stub_fail(reason);
    }

    if (output != NULL)
        memcpy(output, &regs, sizeof(regs));
    return 0;
}

int16_t set_criterr_handler(int16_t (*handler)(void))
{
    (void)handler;
    return 0;
}

void initialize_div0(void)
{
    /* Host arithmetic faults are handled by the C runtime; DOS vector setup
       is not carried over as a guest interrupt vector. */
}

void port_audio_dispatch(uint8_t entry_offset, const void *packet, size_t bytes)
{
    (void)entry_offset;
    (void)packet;
    (void)bytes;
    /* M0's contract-only audio adapter intentionally produces no sound. */
}

void fatal_error(const char *format, ...)
{
    char message[256];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format != NULL ? format : "fatal error", args);
    va_end(args);
    port_stub_fail(message);
}
