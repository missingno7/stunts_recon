#ifndef STUNTS_PORT_PC_SPEAKER_H
#define STUNTS_PORT_PC_SPEAKER_H

#include <stddef.h>
#include <stdint.h>

/* Small PIT channel-2/PPI 0x61 model for the register sequence emitted by
   PC15.DRV. It models register writes, not game-side note heuristics. */
typedef struct PortPcSpeaker {
    uint8_t pit_control;
    uint8_t pit_mode;
    uint8_t pit_access;
    uint8_t pit_write_low_next;
    uint8_t pit_lsb;
    uint8_t ppi_port61;
    uint16_t pit_reload;
    uint16_t pit_current_reload;
    uint16_t pit_pending_reload;
    uint32_t phase_ticks;
    uint32_t one_shot_ticks_remaining;
    uint8_t one_shot_active;
    uint8_t pit_mode3_out_high;
    uint8_t pit_mode3_running;
    uint8_t pit_mode3_reload_pending;
    uint64_t sample_clock_remainder;
    unsigned output_rate;
} PortPcSpeaker;

#define PORT_PC_PIT_HZ 1193182u

void port_pc_speaker_reset(PortPcSpeaker *speaker);
void port_pc_speaker_write(PortPcSpeaker *speaker, uint16_t port,
                           uint8_t value);
void port_pc_speaker_write_divisor(PortPcSpeaker *speaker, uint16_t divisor);
void port_pc_speaker_advance_ticks(PortPcSpeaker *speaker, uint32_t ticks);
int port_pc_speaker_output_high(const PortPcSpeaker *speaker);
int port_pc_speaker_output_level(const PortPcSpeaker *speaker);
size_t port_pc_speaker_render_s16(PortPcSpeaker *speaker, int16_t *output,
                                  size_t frames, unsigned sample_rate,
                                  int16_t gain);
uint16_t port_pc_speaker_divisor(const PortPcSpeaker *speaker);
uint8_t port_pc_speaker_gate(const PortPcSpeaker *speaker);

#endif
