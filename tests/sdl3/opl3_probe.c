#include "port_opl3.h"
#include "opl3.h"

#include <stdio.h>

int main(void)
{
    static const uint8_t writes[][2] = {
        {0x20, 0x20}, {0x23, 0x20}, {0x40, 0x00}, {0x43, 0x00},
        {0x60, 0xF0}, {0x63, 0xF0}, {0x80, 0x0C}, {0x83, 0x0C},
        {0xC0, 0x01}, {0xA0, 0xAE}, {0xB0, 0x30},
    };
    opl3_chip reference;
    int16_t actual[128];
    int16_t expected[128];
    int16_t pair[2];
    size_t i;

    port_opl3_reset(48000u);
    OPL3_Reset(&reference, 48000u);
    for (i = 0; i < sizeof(writes) / sizeof(writes[0]); ++i) {
        port_opl3_write(writes[i][0], writes[i][1]);
        OPL3_WriteRegBuffered(&reference, writes[i][0], writes[i][1]);
    }
    for (i = 0; i < 128; ++i) {
        OPL3_GenerateResampled(&reference, pair);
        expected[i] = pair[0];
    }
    if (port_opl3_render_s16(actual, 64, 48000u) != 64 ||
        port_opl3_render_s16(actual + 64, 64, 48000u) != 64)
        return 1;
    for (i = 0; i < 128; ++i) {
        if (actual[i] != expected[i]) {
            fprintf(stderr, "OPL sample %zu: got %d expected %d\n",
                    i, (int)actual[i], (int)expected[i]);
            return 1;
        }
    }
    puts("OPL2 wrapper matches direct Nuked buffered PCM across split blocks");
    return 0;
}
