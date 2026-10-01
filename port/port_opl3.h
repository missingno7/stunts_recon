#ifndef STUNTS_PORT_OPL3_H
#define STUNTS_PORT_OPL3_H

#include <stddef.h>
#include <stdint.h>

/* Flat OPL2 register interface backed by the pinned Nuked-OPL3 core. */
void port_opl3_reset(unsigned sample_rate);
void port_opl3_write(uint8_t reg, uint8_t value);
size_t port_opl3_render_s16(int16_t *output, size_t frames,
                            unsigned sample_rate);

#endif
