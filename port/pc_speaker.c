#include "pc_speaker.h"

#include <string.h>

static uint32_t effective_reload(const PortPcSpeaker *speaker)
{
    return speaker->pit_reload == 0 ? 65536u : speaker->pit_reload;
}

static uint32_t effective_current_reload(const PortPcSpeaker *speaker)
{
    return speaker->pit_current_reload == 0
               ? 65536u
               : speaker->pit_current_reload;
}

static uint32_t mode3_half_period(const PortPcSpeaker *speaker)
{
    uint32_t reload = effective_current_reload(speaker);
    if (speaker->pit_mode3_out_high)
        return (reload + 1u) >> 1;
    return reload >> 1;
}

static void mode3_start_reload(PortPcSpeaker *speaker, uint16_t reload)
{
    speaker->pit_current_reload = reload;
    speaker->pit_mode3_running = 1;
    speaker->pit_pending_reload = 0;
    speaker->pit_mode3_reload_pending = 0;
    speaker->phase_ticks = 0;
    speaker->pit_mode3_out_high = 1;
}

static void mode3_commit_count(PortPcSpeaker *speaker, uint16_t reload)
{
    speaker->pit_reload = reload;
    if (speaker->pit_mode3_running == 0 ||
        (speaker->ppi_port61 & 1u) == 0) {
        /* A first load, or a load while GATE is low, is ready for the next
           rising GATE edge. An already running mode-3 count keeps its current
           half-cycle and transfers this count at the next half-cycle edge. */
        mode3_start_reload(speaker, reload);
    } else {
        speaker->pit_pending_reload = reload;
        speaker->pit_mode3_reload_pending = 1;
    }
}

void port_pc_speaker_advance_ticks(PortPcSpeaker *speaker, uint32_t ticks)
{
    if (speaker == NULL || ticks == 0)
        return;
    if (speaker->pit_mode == 1u) {
        if (speaker->one_shot_active) {
            if (ticks >= speaker->one_shot_ticks_remaining) {
                speaker->one_shot_ticks_remaining = 0;
                speaker->one_shot_active = 0;
            } else {
                speaker->one_shot_ticks_remaining -= ticks;
            }
        }
    } else if (speaker->pit_mode == 3u) {
        if ((speaker->ppi_port61 & 1u) == 0) {
            speaker->pit_mode3_out_high = 1;
            return;
        }
        while (ticks != 0) {
            uint32_t half_period = mode3_half_period(speaker);
            uint32_t remaining;
            if (half_period == 0)
                half_period = 1;
            remaining = half_period - speaker->phase_ticks;
            if (ticks < remaining) {
                speaker->phase_ticks += ticks;
                return;
            }
            ticks -= remaining;
            speaker->phase_ticks = 0;
            speaker->pit_mode3_out_high =
                (uint8_t)!speaker->pit_mode3_out_high;
            if (speaker->pit_mode3_reload_pending) {
                speaker->pit_current_reload = speaker->pit_pending_reload;
                speaker->pit_mode3_reload_pending = 0;
            }
        }
    }
}

int port_pc_speaker_output_high(const PortPcSpeaker *speaker)
{
    if (speaker == NULL || (speaker->ppi_port61 & 3u) != 3u)
        return 0;
    if (speaker->pit_mode == 1u)
        return speaker->one_shot_active == 0;
    if (speaker->pit_mode != 3u)
        return 0;
    return speaker->pit_mode3_out_high != 0;
}

int port_pc_speaker_output_level(const PortPcSpeaker *speaker)
{
    if (speaker == NULL || (speaker->ppi_port61 & 3u) != 3u ||
        (speaker->pit_mode != 1u && speaker->pit_mode != 3u))
        return -1;
    return port_pc_speaker_output_high(speaker);
}

void port_pc_speaker_reset(PortPcSpeaker *speaker)
{
    if (speaker != NULL)
        memset(speaker, 0, sizeof(*speaker));
}

void port_pc_speaker_write(PortPcSpeaker *speaker, uint16_t port,
                           uint8_t value)
{
    unsigned selected_channel;
    if (speaker == NULL)
        return;
    if (port == 0x43u) {
        selected_channel = (unsigned)((value >> 6) & 3u);
        speaker->pit_control = value;
        if (selected_channel != 2u)
            return;
        speaker->pit_access = (uint8_t)((value >> 4) & 3u);
        speaker->pit_mode = (uint8_t)((value >> 1) & 7u);
        if (speaker->pit_mode >= 6u)
            speaker->pit_mode = (uint8_t)(speaker->pit_mode - 4u);
        if (speaker->pit_access == 3u) {
            speaker->pit_write_low_next = 1;
        } else if (speaker->pit_access == 1u) {
            speaker->pit_write_low_next = 0;
        } else if (speaker->pit_access == 2u) {
            speaker->pit_write_low_next = 0;
        }
        speaker->phase_ticks = 0;
        speaker->one_shot_ticks_remaining = 0;
        speaker->one_shot_active = 0;
        speaker->pit_current_reload = 0;
        speaker->pit_mode3_running = 0;
        speaker->pit_pending_reload = 0;
        speaker->pit_mode3_reload_pending = 0;
        speaker->pit_mode3_out_high = 1;
        return;
    }

    if (port == 0x42u) {
        if (speaker->pit_access == 1u) {
            uint16_t reload = (uint16_t)((speaker->pit_reload & 0xff00u) | value);
            if (speaker->pit_mode == 3u)
                mode3_commit_count(speaker, reload);
            else {
                speaker->pit_reload = reload;
                speaker->phase_ticks = 0;
            }
        } else if (speaker->pit_access == 2u) {
            uint16_t reload = (uint16_t)((speaker->pit_reload & 0x00ffu) |
                                         ((uint16_t)value << 8));
            if (speaker->pit_mode == 3u)
                mode3_commit_count(speaker, reload);
            else {
                speaker->pit_reload = reload;
                speaker->phase_ticks = 0;
            }
        } else if (speaker->pit_access == 3u) {
            if (speaker->pit_write_low_next) {
                speaker->pit_lsb = value;
                speaker->pit_write_low_next = 0;
            } else {
                uint16_t reload = (uint16_t)(((uint16_t)value << 8) |
                                             speaker->pit_lsb);
                speaker->pit_write_low_next = 1;
                if (speaker->pit_mode == 3u)
                    mode3_commit_count(speaker, reload);
                else {
                    speaker->pit_reload = reload;
                    speaker->phase_ticks = 0;
                }
            }
        }
        return;
    }

    if (port == 0x61u) {
        uint8_t old_gate = speaker->ppi_port61;
        speaker->ppi_port61 = value;
        if ((old_gate & 1u) == 0 && (value & 1u) != 0) {
            if (speaker->pit_mode == 1u) {
                speaker->phase_ticks = 0;
                speaker->one_shot_ticks_remaining = effective_reload(speaker);
                speaker->one_shot_active = 1;
            } else if (speaker->pit_mode == 3u) {
                mode3_start_reload(speaker, speaker->pit_reload);
            }
        } else if ((value & 1u) == 0 && speaker->pit_mode == 1u) {
            speaker->one_shot_ticks_remaining = 0;
            speaker->one_shot_active = 0;
        } else if ((value & 1u) == 0 && speaker->pit_mode == 3u) {
            speaker->pit_mode3_out_high = 1;
        }
    }
}

void port_pc_speaker_write_divisor(PortPcSpeaker *speaker, uint16_t divisor)
{
    if (speaker == NULL)
        return;
    /* PC15 has already selected channel 2, low/high access, mode 3 at +00.
       The +30 path writes only the channel-2 reload and opens the speaker. */
    port_pc_speaker_write(speaker, 0x42u, (uint8_t)divisor);
    port_pc_speaker_write(speaker, 0x42u, (uint8_t)(divisor >> 8));
    port_pc_speaker_write(speaker, 0x61u,
                          (uint8_t)(speaker->ppi_port61 | 3u));
}

size_t port_pc_speaker_render_s16(PortPcSpeaker *speaker, int16_t *output,
                                  size_t frames, unsigned sample_rate,
                                  int16_t gain)
{
    size_t i;
    if (output == NULL)
        return 0;
    if (speaker == NULL || sample_rate == 0) {
        memset(output, 0, frames * sizeof(*output));
        return frames;
    }
    if (speaker->output_rate != sample_rate) {
        speaker->output_rate = sample_rate;
        speaker->sample_clock_remainder = 0;
    }
    for (i = 0; i < frames; ++i) {
        uint64_t clock_numerator;
        uint32_t clocks;
        clock_numerator = speaker->sample_clock_remainder + PORT_PC_PIT_HZ;
        clocks = (uint32_t)(clock_numerator / sample_rate);
        speaker->sample_clock_remainder = clock_numerator % sample_rate;
        port_pc_speaker_advance_ticks(speaker, clocks);
        {
            int level = port_pc_speaker_output_level(speaker);
            output[i] = level < 0 ? 0 : level ? gain : (int16_t)-gain;
        }
    }
    return frames;
}

uint16_t port_pc_speaker_divisor(const PortPcSpeaker *speaker)
{
    return speaker != NULL ? speaker->pit_reload : 0;
}

uint8_t port_pc_speaker_gate(const PortPcSpeaker *speaker)
{
    return speaker != NULL ? (uint8_t)(speaker->ppi_port61 & 3u) : 0;
}
