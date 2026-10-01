#include "port_opl3.h"

#include "opl3.h"

#include <string.h>

static opl3_chip s_chip;
static unsigned s_sample_rate;
static int s_initialized;

void port_opl3_reset(unsigned sample_rate)
{
    if (sample_rate == 0)
        sample_rate = 48000u;
    OPL3_Reset(&s_chip, sample_rate);
    s_sample_rate = sample_rate;
    s_initialized = 1;
}

void port_opl3_write(uint8_t reg, uint8_t value)
{
    if (!s_initialized)
        port_opl3_reset(48000u);
    /* AD15 addresses an OPL2 register bank only. Nuked maps 0..FF directly
       to the first bank; writes keep the chip's original bus delay. */
    OPL3_WriteRegBuffered(&s_chip, reg, value);
}

size_t port_opl3_render_s16(int16_t *output, size_t frames,
                            unsigned sample_rate)
{
    size_t i;
    int16_t pair[2];
    if (output == NULL)
        return 0;
    if (sample_rate == 0)
        sample_rate = 48000u;
    if (!s_initialized || s_sample_rate != sample_rate)
        port_opl3_reset(sample_rate);
    for (i = 0; i < frames; ++i) {
        OPL3_GenerateResampled(&s_chip, pair);
        /* The historical two-port OPL path has no stereo pan controls. The
           proven Nuked boundary uses its first channel sample as mono. */
        output[i] = pair[0];
    }
    return frames;
}
