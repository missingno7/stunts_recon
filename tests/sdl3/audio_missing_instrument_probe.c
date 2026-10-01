/* Executes selected functions directly from the generated SDL host overlay. */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stunts_types.h"
#include "audio_backend.h"

#if !defined(PORT_AUDIO_PROBE_STRUCTS)
#error "the Python regression supplies generated overlay record layouts"
#endif

/* The layouts are extracted verbatim from generated obj_seg028.c. */
/* GENERATED_AUDIO_TYPES */

struct AudioChunk audiochunktable[24];
struct AudioVoice snd_voices_tbl[16];
U8 audioblock[24];
U8 audio_driver_mode;
I8 FAR *kick_res;
I8 FAR *g_snaresnd;
I8 FAR *tommsampleresource;
I8 FAR *ride_audio_sound_res;
I8 FAR *audio_opp_res;
I8 FAR *chhtsample;
I8 FAR *resource_sound_hit;

static unsigned selection_calls;
static unsigned driver_calls;

int port_audio_is_active(void) { return 1; }

I16 select_audio_voice_slot(I8 FAR *sample, struct AudioChunk *chunk)
{
    (void)sample;
    (void)chunk;
    ++selection_calls;
    return 0;
}

void port_audio_driver_21(int16_t channel, void *voice, void *chunk,
                          const void *sample)
{
    (void)channel; (void)voice; (void)chunk; (void)sample;
    ++driver_calls;
}

void port_audio_driver_09(int16_t channel, void *voice, void *chunk,
                          int16_t note, int16_t value, const void *sample)
{
    (void)channel; (void)voice; (void)chunk; (void)note; (void)value; (void)sample;
    ++driver_calls;
}

void port_audio_driver_24(int16_t channel, void *voice, uint16_t value)
{
    (void)channel; (void)voice; (void)value;
    ++driver_calls;
}

/* The original helper reads a little-endian dword from the resource directory. */
U32 audioresource_get_dword(I8 FAR *address)
{
    const U8 FAR *p = (const U8 FAR *)address;
    return (U32)p[0] | ((U32)p[1] << 8) | ((U32)p[2] << 16) | ((U32)p[3] << 24);
}

I16 FAR audioresource_compare_chunknames(I16 caseSensitive, U8 FAR *chunkName,
                                         U8 FAR *foundName, I16 count);
I16 FAR audioresource_get_chunk_index(I16 stride, I16 numChunks, U8 *chunkName,
                                      I8 FAR *chunkNames);
I8 FAR * FAR audioresource_find(I8 HUGE *resource, U8 *chunkName);
void FAR audioresource_copy_4_bytes(U8 FAR *dst, U8 FAR *src);
void FAR audio_map_song_instruments(void FAR *song, void FAR *voice);
I16 FAR _loadds process_audio_event(struct AudioEvent *event, I16 chunkIndex);

/* GENERATED_AUDIO_FUNCTIONS */

static U8 *read_all(const char *path, size_t *length)
{
    FILE *file = fopen(path, "rb");
    long size;
    U8 *bytes;
    if (file == NULL || fseek(file, 0, SEEK_END) != 0 ||
        (size = ftell(file)) < 0 || fseek(file, 0, SEEK_SET) != 0)
        return NULL;
    bytes = (U8 *)malloc((size_t)size);
    if (bytes == NULL || fread(bytes, 1, (size_t)size, file) != (size_t)size) {
        free(bytes);
        fclose(file);
        return NULL;
    }
    fclose(file);
    *length = (size_t)size;
    return bytes;
}

int main(int argc, char **argv)
{
    U8 *kms, *vce, *song, *header;
    size_t kms_size = 0, vce_size = 0;
    U8 before_instrument[4];
    U8 voice_snapshot[sizeof(snd_voices_tbl)];
    U8 block_snapshot[sizeof(audioblock)];
    struct AudioEvent event;
    I16 result;

    if (argc != 3 || sizeof(void *) != 4) {
        fprintf(stderr, "probe needs two asset paths and a 32-bit compiler\n");
        return 2;
    }
    kms = read_all(argv[1], &kms_size);
    vce = read_all(argv[2], &vce_size);
    if (kms == NULL || vce == NULL || kms_size < 6 || vce_size < 6) {
        fprintf(stderr, "could not load audio bank assets\n");
        return 2;
    }

    song = (U8 *)audioresource_find((I8 HUGE *)kms, (U8 *)"over");
    if (song == NULL) {
        fprintf(stderr, "SKIDOVER.KMS has no OVER resource\n");
        return 1;
    }
    header = (U8 *)audioresource_find((I8 HUGE *)song, (U8 *)"hdr1");
    if (header == NULL || header[6] < 3 || memcmp(header + 7 + 2 * 4, "KEYS", 4) != 0) {
        fprintf(stderr, "OVER instrument slot 2 is not KEYS before mapping\n");
        return 1;
    }
    memcpy(before_instrument, header + 7 + 2 * 4, sizeof(before_instrument));
    audio_map_song_instruments(song, vce);
    if (audioresource_find((I8 HUGE *)vce, (U8 *)"KEYS") != NULL ||
        memcmp(header + 7 + 2 * 4, "\0\0\0\0", 4) != 0 ||
        memcmp(before_instrument, "KEYS", 4) != 0) {
        fprintf(stderr, "missing KEYS did not map to a null resource pointer\n");
        return 1;
    }

    memset(audiochunktable, 0xA5, sizeof(audiochunktable));
    audiochunktable[4].data = NULL;
    memset(snd_voices_tbl, 0x5A, sizeof(snd_voices_tbl));
    memcpy(voice_snapshot, snd_voices_tbl, sizeof(voice_snapshot));
    memset(audioblock, 0xC3, sizeof(audioblock));
    memcpy(block_snapshot, audioblock, sizeof(block_snapshot));
    memset(&event, 0xD7, sizeof(event));
    event.command = 0x4A;
    event.param = 0x6C;
    event.value = 12;
    selection_calls = 0;
    driver_calls = 0;
    result = process_audio_event(&event, 4);
    if (result != -1 || memcmp(voice_snapshot, snd_voices_tbl, sizeof(voice_snapshot)) != 0 ||
        memcmp(block_snapshot, audioblock, sizeof(block_snapshot)) != 0 ||
        selection_calls != 0 || driver_calls != 0) {
        fprintf(stderr, "null instrument changed voice/audio state: result=%d select=%u drivers=%u\n",
                result, selection_calls, driver_calls);
        return 1;
    }

    free(kms);
    free(vce);
    puts("missing KEYS instrument safely rejected without voice mutation");
    return 0;
}
