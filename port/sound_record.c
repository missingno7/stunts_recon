#include "sound_record.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef PORT_BOUNDARY_RECORD
#include "port_runtime.h"
#include "boundary_record.h"
#endif

/* Guest-owned observer. No timer pump, SDL call, device callback, or game-state
   write. Pointer values are used only to resolve stable table/resource views. */
#define RESOURCE_CAP 2048u
typedef struct SoundResource {
    const void *base;
    size_t size;
    uint64_t hash;
    char name[256];
} SoundResource;
static SoundResource s_resources[RESOURCE_CAP];
static FILE *s_output;
static PortSoundClockObserver s_observer;
static const void *s_chunks, *s_voices;
static size_t s_chunk_stride, s_voice_stride;
static uint32_t s_chunk_generation[24], s_voice_generation[16];
static int s_voice_chunk[16];
static const void *s_voice_resource[16];
static uint64_t s_previous_tick;
static uint32_t s_sequence;
static const char *s_driver = "none";
static int s_failed;

static void string(const char *value)
{
    const unsigned char *p = (const unsigned char *)value;
    fputc('"', s_output);
    for (; *p; ++p) {
        if (*p == '"' || *p == '\\') fputc('\\', s_output);
        if (*p < 32) fprintf(s_output, "\\u%04x", (unsigned)*p);
        else fputc(*p, s_output);
    }
    fputc('"', s_output);
}

int port_sound_record_open(const char *path, PortSoundClockObserver observer)
{
    if (!port_sound_record_close()) return 0;
    s_failed = 0;
    if (path == NULL || *path == '\0') return 1;
    s_output = fopen(path, "wb");
    if (s_output == NULL) return 0;
    s_observer = observer;
    s_previous_tick = UINT64_MAX;
    s_sequence = 0;
    s_driver = "none";
    s_chunks = s_voices = NULL;
    s_chunk_stride = s_voice_stride = 0;
    memset(s_resources, 0, sizeof(s_resources));
    memset(s_chunk_generation, 0, sizeof(s_chunk_generation));
    memset(s_voice_generation, 0, sizeof(s_voice_generation));
    memset(s_voice_chunk, 0xff, sizeof(s_voice_chunk));
    memset(s_voice_resource, 0, sizeof(s_voice_resource));
    fputs("{\"schema\":\"stunts-sound-requests-v1\",\"kind\":\"header\","
          "\"tick_source\":\"guest_dispatched_raw_tick\","
          "\"resource_hash\":\"fnv1a64_at_load\"}\n", s_output);
    return 1;
}

int port_sound_record_close(void)
{
    if (s_output != NULL) {
        if (ferror(s_output)) s_failed = 1;
        if (fclose(s_output) != 0) s_failed = 1;
        s_output = NULL;
    }
    return !s_failed;
}

int port_sound_record_enabled(void) { return s_output != NULL; }

void port_sound_record_tables(const void *chunks, size_t chunk_stride,
                              const void *voices, size_t voice_stride)
{
    if (!s_output) return;
    s_chunks = chunks; s_chunk_stride = chunk_stride;
    s_voices = voices; s_voice_stride = voice_stride;
}

void port_sound_record_forget(const void *base)
{
    unsigned i;
    if (!s_output) return;
    for (i = 0; i < RESOURCE_CAP; ++i)
        if (s_resources[i].base == base) s_resources[i].base = NULL;
}

void port_sound_record_resource(const char *name, const void *base, size_t size)
{
    unsigned i;
    const char *leaf = name, *p;
    uint64_t hash = UINT64_C(14695981039346656037);
    size_t j;
    SoundResource *entry = NULL;
    if (!s_output || !base) return;
    port_sound_record_forget(base);
    for (i = 0; i < RESOURCE_CAP; ++i)
        if (s_resources[i].base == NULL) { entry = &s_resources[i]; break; }
    if (!entry) { s_failed = 1; return; }
    for (j = 0; j < size; ++j)
        hash = (hash ^ ((const uint8_t *)base)[j]) * UINT64_C(1099511628211);
    for (p = name; *p; ++p) if (*p == '/' || *p == '\\') leaf = p + 1;
    for (j = 0; leaf[j] && j + 1 < sizeof(entry->name); ++j)
        entry->name[j] = (char)toupper((unsigned char)leaf[j]);
    entry->name[j] = '\0';
    entry->base = base; entry->size = size; entry->hash = hash;
}

static int index_of(const void *pointer, const void *base, size_t stride, unsigned count)
{
    uintptr_t p = (uintptr_t)pointer, b = (uintptr_t)base;
    if (!pointer || !base || !stride || p < b || (p - b) / stride >= count ||
        (p - b) % stride != 0) return -1;
    return (int)((p - b) / stride);
}

static void resource(const void *pointer)
{
    unsigned i;
    uintptr_t p = (uintptr_t)pointer;
    if (!pointer) { fputs("null", s_output); return; }
    for (i = 0; i < RESOURCE_CAP; ++i) {
        SoundResource *entry = &s_resources[i];
        uintptr_t b = (uintptr_t)entry->base;
        if (entry->base && p >= b && p - b < entry->size) {
            fputs("{\"name\":", s_output); string(entry->name);
            fprintf(s_output, ",\"fnv1a64\":\"%016llx\",\"offset\":%llu}",
                (unsigned long long)entry->hash, (unsigned long long)(p - b));
            return;
        }
    }
    /* Unknown spans stay unknown; never invent identity from an address. */
    fputs("{\"resolved\":false}", s_output);
}

static void begin(const char *kind, int chunk, int voice, const void *sample)
{
    uint64_t tick = 0;
    int slot = -1;
    if (s_observer) s_observer(&tick, &slot);
    if (tick != s_previous_tick) { s_sequence = 0; s_previous_tick = tick; }
    fprintf(s_output, "{\"kind\":"); string(kind);
#ifdef PORT_BOUNDARY_RECORD
    fprintf(s_output,",\"game_frame\":%u,\"scope\":%u,\"game_inputmode\":%u",
        (unsigned)port_game_frame_snapshot(),port_boundary_scope(),
        (unsigned)port_game_inputmode_snapshot());
#endif
    fprintf(s_output, ",\"raw_tick\":%llu,\"sequence\":%u,\"source\":\"%s\","
        "\"callback_slot\":%d,\"driver\":", (unsigned long long)tick,
        (unsigned)s_sequence++, slot < 0 ? "foreground" : "callback", slot);
    string(s_driver);
    fprintf(s_output, ",\"chunk\":%d,\"chunk_generation\":%u,\"voice\":%d,"
        "\"voice_generation\":%u,\"resource\":", chunk,
        chunk >= 0 && chunk < 24 ? s_chunk_generation[chunk] : 0,
        voice, voice >= 0 && voice < 16 ? s_voice_generation[voice] : 0);
    resource(sample);
}

void port_sound_record_driver(const char *driver, unsigned entry,
    const void *voice, const void *chunk, const void *sample,
    const uint16_t *arguments, size_t count, int has_result, uint16_t result,
    const void *payload, size_t payload_size)
{
    size_t i;
    int vi, ci;
    if (!s_output) return;
    s_driver = driver;
    vi = entry == 0x30 ? -1 : index_of(voice, s_voices, s_voice_stride, 16);
    ci = index_of(chunk, s_chunks, s_chunk_stride, 24);
    if (vi >= 0) {
        if (ci >= 0) s_voice_chunk[vi] = ci;
        else ci = s_voice_chunk[vi];
        if (sample) s_voice_resource[vi] = sample;
        else sample = s_voice_resource[vi];
    }
    if (entry == 0x09 && vi >= 0) ++s_voice_generation[vi];
    begin("driver", ci, vi, sample);
    fprintf(s_output, ",\"entry\":%u,\"voice_array\":%s,\"arguments_u16\":[",
        entry, entry == 0x30 ? "true" : "false");
    for (i = 0; i < count; ++i) fprintf(s_output, "%s%u", i ? "," : "", arguments[i]);
    fputs("],\"return_u16\":", s_output);
    if (has_result) fprintf(s_output, "%u", result); else fputs("null", s_output);
    fputs(",\"payload_hex\":\"", s_output);
    if (payload) for (i = 0; i < payload_size; ++i)
        fprintf(s_output, "%02x", (unsigned)((const uint8_t *)payload)[i]);
    fputs("\"}\n", s_output);
}

void port_sound_record_logical(const char *kind, int chunk, int voice,
    const void *sample, const uint32_t *values, size_t count)
{
    size_t i;
    if (!s_output) return;
    if (strcmp(kind, "chunk_init") == 0 && chunk >= 0 && chunk < 24)
        ++s_chunk_generation[chunk];
    begin(kind, chunk, voice, sample);
    fputs(",\"values_u32\":[", s_output);
    for (i = 0; i < count; ++i)
        fprintf(s_output, "%s%lu", i ? "," : "", (unsigned long)values[i]);
    fputs("]}\n", s_output);
}

void port_sound_record_event(int chunk, const void *stream, unsigned command,
    unsigned param, uint32_t value, uint32_t delta, unsigned length, int value_kind)
{
    if (!s_output) return;
    begin("chunk_event", chunk, -1, stream);
    fprintf(s_output, ",\"command\":%u,\"param\":%u,\"delta\":%lu,\"length\":%u,"
        "\"value_kind\":\"%s\",\"value_u32\":", command, param, (unsigned long)delta,
        length, value_kind == 2 ? "resource" : value_kind == 1 ? "scalar" : "unused");
    if (value_kind == 1) fprintf(s_output, "%lu", (unsigned long)value);
    else fputs("null", s_output);
    fputs(",\"target_resource\":", s_output);
    resource(value_kind == 2 ? (const void *)(uintptr_t)value : NULL);
    fputs("}\n", s_output);
}
