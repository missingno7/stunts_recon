#ifndef STUNTS_PORT_AUDIO_BACKEND_H
#define STUNTS_PORT_AUDIO_BACKEND_H

#include <stddef.h>
#include <stdint.h>

/* Fixed host packet for the legacy port_audio_dispatch entry. Callers use the
   named typed wrappers below when a return value or an exact call signature
   matters. */
typedef struct PortAudioDispatchPacket {
    int16_t words[4];
    uint16_t unsigned_word;
    void *pointers[3];
} PortAudioDispatchPacket;

/* Host-side replacement for the direct far-call vectors in the raw PC15.DRV
   asset. Pointer arguments are host views over the accepted game records;
   they are never treated as pointers into executable driver code. */
int port_audio_backend_load(const char *driver_name);
int port_audio_is_active(void);
const char *port_audio_backend_name(void);
size_t port_audio_render_s16(int16_t *output, size_t frames,
                             unsigned sample_rate);
int port_audio_sdl_start(void);
void port_audio_sdl_stop(void);
void audio_stop_unknown(void);

uint8_t port_audio_driver_00(void);
void port_audio_driver_03(void);
void port_audio_driver_06(void);
void port_audio_driver_09(int16_t channel, void *voice, void *chunk,
                          int16_t note, int16_t value,
                          const void *sample);
void port_audio_driver_0c(int16_t channel, void *voice);
void port_audio_driver_0f(int16_t channel, void *voice);
void port_audio_driver_12(int16_t channel, void *voice, int16_t value);
void port_audio_driver_15(int16_t channel, void *voice,
                          int16_t type, int16_t value);
void port_audio_driver_18(void);
void port_audio_driver_1b(void *chunk, int16_t value, int16_t channel);
void port_audio_driver_1e(int16_t channel);
void port_audio_driver_21(int16_t channel, void *voice, void *chunk,
                          const void *sample);
void port_audio_driver_24(int16_t channel, void *voice, uint16_t value);
void port_audio_driver_27(uint16_t channel, void *voice, void *chunk,
                          const void *sample);
void port_audio_driver_30(void *voices);
uint16_t port_audio_driver_36(int16_t channel);
void port_audio_driver_39(int16_t count, const uint8_t *bytes);
uint16_t port_audio_driver_3c(void);
void port_audio_driver_3f(int16_t count, const void *bytes);
void port_audio_driver_42(const void *patches);

/* Offset+33 is only statically reached inside PC15's +09 path in the inspected
   driver. The game's common +33 meaning remains unresolved, so this wrapper
   intentionally reports unsupported instead of guessing its ABI. */
int port_audio_driver_entry_33_supported(void);

/* Explicit output coverage counters help keep the translated subset visible. */
uint32_t port_audio_pc15_unsupported_sample_count(void);
uint32_t port_audio_pc15_sample_irq_count(void);
uint32_t port_audio_pc15_sample_byte_count(void);
uint32_t port_audio_pc15_sample_reload_hash(void);

#endif
