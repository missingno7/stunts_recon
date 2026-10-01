#ifndef PORT_RENDER_PATTERN_H
#define PORT_RENDER_PATTERN_H
#include <stdint.h>

/* Absolute DOS screen phase: high byte on even y, low byte on odd y;
   the span rotates left before sampling, so x=0 selects bit 7. */
static inline int port_render_pattern_bit(uint16_t word, int x, int y)
{
    unsigned row = (unsigned)y & 1u;
    unsigned byte = row ? word & 255u : word >> 8;
    return (int)((byte >> (7u - ((unsigned)x & 7u))) & 1u);
}
#endif
