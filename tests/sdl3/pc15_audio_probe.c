#include "audio_backend.h"
#include "pc_speaker.h"
#include "port_runtime.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *audiodriverbinary;
unsigned char g_audiodrvvoices_count;
unsigned char audio_driver_mode;
unsigned char audio_driver_extension_mode;
unsigned char audio_pause_in_progress;
unsigned char audioflag2;
unsigned char audio_song_ready;
unsigned char audioflag6;
unsigned char g_musicvolumesetting;
unsigned char sfx_audio_vol;
char g_drvaudiocode[14];

static int s_timer_registered;
static int s_timer_removed;
static void *s_sample_record;
static size_t s_sample_record_size;
static void *s_sample_storage;
static size_t s_sample_storage_size;

void *port_fs_load(const char *path, size_t *length_out,
                   PortFarPtr *address_out)
{
    FILE *file;
    long length;
    void *bytes;
    (void)address_out;
    if (path == NULL || strcmp(path, "pc15.drv") != 0)
        return NULL;
    file = fopen("assets/PC15.DRV", "rb");
    if (file == NULL || fseek(file, 0, SEEK_END) != 0 ||
        (length = ftell(file)) < 0 || fseek(file, 0, SEEK_SET) != 0) {
        if (file != NULL)
            fclose(file);
        return NULL;
    }
    bytes = malloc((size_t)length + 1u);
    if (bytes == NULL || fread(bytes, 1, (size_t)length, file) != (size_t)length) {
        free(bytes);
        fclose(file);
        return NULL;
    }
    ((uint8_t *)bytes)[length] = 0;
    fclose(file);
    *length_out = (size_t)length;
    return bytes;
}

int port_memory_extent(const void *pointer, size_t *remaining_out)
{
    uintptr_t value = (uintptr_t)pointer;
    uintptr_t record = (uintptr_t)s_sample_record;
    uintptr_t storage = (uintptr_t)s_sample_storage;
    if (pointer == NULL)
        return 0;
    if (s_sample_record != NULL && value >= record &&
        value - record < s_sample_record_size) {
        if (remaining_out != NULL)
            *remaining_out = s_sample_record_size - (size_t)(value - record);
        return 1;
    }
    if (s_sample_storage != NULL && value >= storage &&
        value - storage < s_sample_storage_size) {
        if (remaining_out != NULL)
            *remaining_out = s_sample_storage_size - (size_t)(value - storage);
        return 1;
    }
    return 0;
}

void mmgr_release(void *pointer) { free(pointer); }
void timer_reg_callback(void (*callback)(void)) { (void)callback; ++s_timer_registered; }
void timer_remove_callback(void (*callback)(void)) { (void)callback; ++s_timer_removed; }
void audiodriver_timer(void) {}
void reset_audio_driver_state(void) {}
int port_audio_sdl_start(void) { return 0; }
void port_audio_sdl_stop(void) {}

static int check(int condition, const char *message)
{
    if (!condition)
        fprintf(stderr, "FAIL: %s\n", message);
    return condition;
}

static void write_u32(uint8_t *destination, uint32_t value)
{
    destination[0] = (uint8_t)value;
    destination[1] = (uint8_t)(value >> 8);
    destination[2] = (uint8_t)(value >> 16);
    destination[3] = (uint8_t)(value >> 24);
}

static uint16_t read_asset_word(unsigned offset)
{
    FILE *file = fopen("assets/PC15.DRV", "rb");
    uint8_t bytes[2];
    if (file == NULL || fseek(file, (long)offset, SEEK_SET) != 0 ||
        fread(bytes, 1, sizeof(bytes), file) != sizeof(bytes)) {
        if (file != NULL)
            fclose(file);
        return 0;
    }
    fclose(file);
    return (uint16_t)(bytes[0] | ((uint16_t)bytes[1] << 8));
}

static int test_pit2_mode1_pulse(void)
{
    PortPcSpeaker speaker;
    port_pc_speaker_reset(&speaker);
    port_pc_speaker_write(&speaker, 0x43u, 0xB2u);
    port_pc_speaker_write(&speaker, 0x42u, 7u);
    port_pc_speaker_write(&speaker, 0x42u, 0u);
    port_pc_speaker_write(&speaker, 0x61u, 2u);
    port_pc_speaker_write(&speaker, 0x61u, 3u);
    if (!check(port_pc_speaker_output_level(&speaker) == 0,
               "PIT2 mode 1 starts low on the PPI gate edge"))
        return 0;
    port_pc_speaker_advance_ticks(&speaker, 6u);
    if (!check(port_pc_speaker_output_level(&speaker) == 0,
               "mode 1 pulse remains low through count minus one"))
        return 0;
    port_pc_speaker_advance_ticks(&speaker, 1u);
    return check(port_pc_speaker_output_level(&speaker) == 1,
                 "mode 1 returns high at the programmed count");
}

static int test_pit2_mode3_reload_latching(void)
{
    PortPcSpeaker speaker;
    int16_t samples[480];
    size_t i;
    int positive = 0;
    int negative = 0;
    static const uint16_t engine_reloads[] = {65417u, 65298u, 0u};

    port_pc_speaker_reset(&speaker);
    port_pc_speaker_write(&speaker, 0x43u, 0xB6u);
    port_pc_speaker_write_divisor(&speaker, engine_reloads[0]);
    if (!check(port_pc_speaker_output_level(&speaker) == 1,
               "mode 3 starts with OUT high"))
        return 0;

    /* PC15 +30 rewrites PIT2 about once per 10 ms while the live engine is
       changing its reload among 65417, 65298 and zero (65536). The 8254 keeps
       the current half-cycle and loads the newest count at the next edge. */
    for (i = 0; i < 120u; ++i) {
        port_pc_speaker_write_divisor(
            &speaker, engine_reloads[i %
                                     (sizeof(engine_reloads) /
                                      sizeof(engine_reloads[0]))]);
        port_pc_speaker_render_s16(&speaker, samples,
                                   sizeof(samples) / sizeof(samples[0]),
                                   48000u, 4096);
        {
            size_t j;
            for (j = 0; j < sizeof(samples) / sizeof(samples[0]); ++j) {
                if (samples[j] > 0)
                    positive = 1;
                else if (samples[j] < 0)
                    negative = 1;
            }
        }
    }
    return check(positive && negative,
                 "mode 3 reload writes preserve the live engine square wave");
}

static int test_pc15_sample_isr_path(void)
{
    uint8_t voice[0x30] = {0};
    uint8_t chunk[0x4c] = {0};
    uint8_t sample_record[0x5d] = {0};
    uint8_t *stream = (uint8_t *)calloc(1u, 0x32u + 64u);
    int16_t output[1024];
    uint16_t rate;
    size_t i;
    int positive = 0;
    int negative = 0;

    if (!check(stream != NULL, "allocate sample test stream"))
        return 0;
    s_sample_record = sample_record;
    s_sample_record_size = sizeof(sample_record);
    s_sample_storage = stream;
    s_sample_storage_size = 0x32u + 64u;
    sample_record[5] = 3;
    write_u32(sample_record + 6u, (uint32_t)(uintptr_t)stream);
    stream[8] = 64u;
    stream[9] = 0u;
    for (i = 0; i < 64u; ++i)
        stream[0x32u + i] = (uint8_t)(i * 4u);

    if (!check(port_audio_backend_load("pc15") == 1, "select PC15"))
        return 0;
    if (!check(g_audiodrvvoices_count == 7u && audio_driver_mode == 0u,
               "PC15 advertises its seven software voices"))
        return 0;

    /* Match the live PCENG1.VCE ENGI case: +24 has left the base pitch at
       zero, note FF is retained, and the +28==2 record mode adds voice+1C.
       The original PC15 +27 oracle returns FF89 for this captured state. */
    sample_record[0x28] = 2u;
    voice[1] = 1u;
    voice[3] = 0xffu;
    voice[0x1c] = 0x89u;
    voice[0x1d] = 0xffu;
    port_audio_driver_27(2u, voice, chunk, sample_record);
    if (!check((uint16_t)(voice[6] | ((uint16_t)voice[7] << 8)) == 0xff89u,
               "live PCENG1 note-FF update matches original +27 oracle"))
        return 0;

    memset(voice, 0, sizeof(voice));
    voice[1] = 1;
    port_audio_driver_09(0, voice, chunk, 35, 0, sample_record);
    rate = read_asset_word(0x12fu + 35u * 2u);
    if (!check((uint16_t)(voice[4] | ((uint16_t)voice[5] << 8)) == rate,
               "channel zero obtains the PC15 sample rate table entry"))
        return 0;
    if (!check(voice[3] == 35u &&
               (uint16_t)(voice[6] | ((uint16_t)voice[7] << 8)) == rate,
               "channel-zero note and both divisor words match PC15 +09"))
        return 0;
    if (!check(port_audio_pc15_unsupported_sample_count() == 0,
               "host-rebased sample buffer is accepted"))
        return 0;

    port_audio_render_s16(output, 1024u, 48000u);
    for (i = 0; i < 1024u; ++i) {
        if (output[i] > 0)
            positive = 1;
        if (output[i] < 0)
            negative = 1;
    }
    if (!check(port_audio_pc15_sample_irq_count() == 138u,
               "INT 8 sample cursor stops at the one-pass stream end"))
        return 0;
    if (!check(port_audio_pc15_sample_byte_count() == 137u,
               "stream reads each in-range byte before the terminal IRQ"))
        return 0;
    if (!check(port_audio_pc15_sample_reload_hash() == 0x110acd13u,
               "sample-to-PIT2 reload sequence matches the PC15 oracle trace"))
        return 0;
    if (!check(positive && negative,
               "PIT2 mode 1 sample pulses reach the 48 kHz renderer"))
        return 0;

    /* PCENG1.VCE's CRA2 record stores the literal name "cras" at +6 instead
       of a resolved host pointer. It must not be dereferenced as a far span. */
    sample_record[6] = 'c';
    sample_record[7] = 'r';
    sample_record[8] = 'a';
    sample_record[9] = 's';
    port_audio_driver_09(0, voice, chunk, 35, 0, sample_record);
    port_audio_render_s16(output, 16u, 48000u);
    if (!check(port_audio_pc15_unsupported_sample_count() == 1u,
               "unresolved resource names remain explicitly unsupported"))
        return 0;
    for (i = 0; i < 16u; ++i)
        if (!check(output[i] == 0,
                   "unresolved far pointer is safely silenced"))
            return 0;

    audio_stop_unknown();
    if (!check(port_audio_driver_36(0) == 0,
               "audio_stop_unknown preserves voice state for the following unload"))
        return 0;
    port_audio_shutdown();
    free(stream);
    s_sample_record = NULL;
    s_sample_storage = NULL;
    return check(s_timer_registered == 1 && s_timer_removed == 1,
                 "driver cleanup unregisters its callback exactly once");
}

int main(void)
{
    if (!test_pit2_mode1_pulse() || !test_pit2_mode3_reload_latching() ||
        !test_pc15_sample_isr_path())
        return 1;
    puts("PC15 sample ISR and PIT2 mode-1/mode-3 checks passed");
    return 0;
}
