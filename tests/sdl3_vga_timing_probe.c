#include "../port/vga_timing.h"

#include <stdio.h>
#include <stdlib.h>

static uint64_t time_at_dot(uint64_t dot)
{
    return (dot * 1000000000ull + PORT_VGA_DOT_CLOCK_HZ - 1u) /
           PORT_VGA_DOT_CLOCK_HZ;
}

static int expect_status(const char *name, uint64_t time_ns, uint8_t expected)
{
    const uint8_t actual = port_vga_input_status_1(time_ns);
    if (actual != expected) {
        fprintf(stderr, "%s: expected 0x%02x, got 0x%02x at %llu ns\n",
                name, expected, actual, (unsigned long long)time_ns);
        return 0;
    }
    return 1;
}

int main(void)
{
    const uint64_t line = PORT_VGA_HORIZONTAL_TOTAL_DOTS;
    const uint64_t frame = PORT_VGA_FRAME_DOTS;
    int ok = 1;

    ok &= expect_status("active display", time_at_dot(0), 0x00u);
    ok &= expect_status("horizontal blank", time_at_dot(640), 0x01u);
    ok &= expect_status("last displayed line", time_at_dot(399 * line), 0x00u);
    ok &= expect_status("vertical blank before retrace", time_at_dot(400 * line), 0x01u);
    ok &= expect_status("retrace starts", time_at_dot(412 * line), 0x09u);
    ok &= expect_status("second retrace line", time_at_dot(413 * line), 0x09u);
    ok &= expect_status("retrace ends", time_at_dot(414 * line), 0x01u);
    ok &= expect_status("one frame repeats", time_at_dot(frame), 0x00u);
    ok &= expect_status("same-time reads are stable", time_at_dot(412 * line),
                        port_vga_input_status_1(time_at_dot(412 * line)));
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
