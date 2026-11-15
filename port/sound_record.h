#ifndef STUNTS_SOUND_RECORD_H
#define STUNTS_SOUND_RECORD_H

#include <stddef.h>
#include <stdint.h>

typedef void (*PortSoundClockObserver)(uint64_t *tick, int *callback_slot);
int port_sound_record_open(const char *path, PortSoundClockObserver observer);
int port_sound_record_close(void);
int port_sound_record_enabled(void);
void port_sound_record_tables(const void *chunks, size_t chunk_stride,
                              const void *voices, size_t voice_stride);
void port_sound_record_resource(const char *name, const void *base, size_t size);
void port_sound_record_forget(const void *base);
void port_sound_record_driver(const char *driver, unsigned entry,
    const void *voice, const void *chunk, const void *resource,
    const uint16_t *arguments, size_t count, int has_result, uint16_t result,
    const void *payload, size_t payload_size);
void port_sound_record_logical(const char *kind, int chunk, int voice,
    const void *resource, const uint32_t *values, size_t count);
/* Only CALL uses a rebased pointer value; many other opcodes leave value stale.
   0 = unused, 1 = scalar bits, 2 = resource target (never serialize its address). */
void port_sound_record_event(int chunk, const void *stream, unsigned command,
    unsigned param, uint32_t value, uint32_t delta, unsigned length, int value_kind);

/* Old standalone production probes can compile without the observer. The
   executable build enables it; runtime recording is still off by default. */
#ifdef PORT_SOUND_RECORD
#define PORT_SOUND_RESOURCE(name, base, size) port_sound_record_resource(name, base, size)
#define PORT_SOUND_FORGET(base) port_sound_record_forget(base)
#else
#define PORT_SOUND_RESOURCE(name, base, size) ((void)0)
#define PORT_SOUND_FORGET(base) ((void)0)
#endif
#endif
