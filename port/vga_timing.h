#ifndef STUNTS_PORT_VGA_TIMING_H
#define STUNTS_PORT_VGA_TIMING_H

#include <stdint.h>

/* IBM-compatible VGA Mode 13h raster timing, expressed as dot-clock counts.
   The 3DAh status value is sampled from this clock and never changes merely
   because software reads the register. */
#define PORT_VGA_DOT_CLOCK_HZ 25175000ull
#define PORT_VGA_HORIZONTAL_TOTAL_DOTS 800ull
#define PORT_VGA_HORIZONTAL_DISPLAY_DOTS 640ull
#define PORT_VGA_VERTICAL_TOTAL_LINES 449ull
#define PORT_VGA_VERTICAL_DISPLAY_LINES 400ull
#define PORT_VGA_VERTICAL_RETRACE_START_LINE 412ull
#define PORT_VGA_VERTICAL_RETRACE_END_LINE 414ull
#define PORT_VGA_FRAME_DOTS \
    (PORT_VGA_HORIZONTAL_TOTAL_DOTS * PORT_VGA_VERTICAL_TOTAL_LINES)

static inline uint64_t port_vga_phase_dots(uint64_t machine_time_ns)
{
    /* Split whole seconds and nanoseconds so uptime cannot overflow when
       converted to dot clocks. The modulus reduction is exact. */
    const uint64_t whole_seconds = machine_time_ns / 1000000000ull;
    const uint64_t subsecond_ns = machine_time_ns % 1000000000ull;
    const uint64_t whole_second_dots =
        ((whole_seconds % PORT_VGA_FRAME_DOTS) * PORT_VGA_DOT_CLOCK_HZ) %
        PORT_VGA_FRAME_DOTS;
    const uint64_t subsecond_dots =
        (subsecond_ns * PORT_VGA_DOT_CLOCK_HZ) / 1000000000ull;
    return (whole_second_dots + subsecond_dots) % PORT_VGA_FRAME_DOTS;
}

static inline uint8_t port_vga_input_status_1(uint64_t machine_time_ns)
{
    const uint64_t dots = port_vga_phase_dots(machine_time_ns);
    const uint64_t line = dots / PORT_VGA_HORIZONTAL_TOTAL_DOTS;
    const uint64_t dot_in_line = dots % PORT_VGA_HORIZONTAL_TOTAL_DOTS;
    uint8_t status = 0;

    /* Input Status #1 bit 0 is active-low display enable: it is set during
       horizontal or vertical blanking. Mode 13h double-scans 200 rows into
       400 displayed raster lines. */
    if (line >= PORT_VGA_VERTICAL_DISPLAY_LINES ||
        dot_in_line >= PORT_VGA_HORIZONTAL_DISPLAY_DOTS)
        status |= 0x01u;
    if (line >= PORT_VGA_VERTICAL_RETRACE_START_LINE &&
        line < PORT_VGA_VERTICAL_RETRACE_END_LINE)
        status |= 0x08u;
    return status;
}

#endif
