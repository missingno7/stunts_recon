#include "port_runtime.h"
#include "audio_backend.h"
#include "ad15_driver.h"
#include "pc_speaker.h"
#include "port_opl3.h"

#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

enum PortAudioBackend {
    PORT_AUDIO_NONE = 0,
    PORT_AUDIO_PC15 = 1,
    PORT_AUDIO_AD15 = 2
};

typedef struct PortPc15State {
    uint8_t active[7];
    uint8_t mixed[7];
    uint8_t gain[7];
    uint16_t divisor[7];
    uint8_t mix_phase;
    uint8_t sample_stream_active;
    uint8_t sample_timer_owned;
    uint8_t sample_repeat_count;
    uint16_t sample_offset;
    uint16_t sample_end_offset;
    uint16_t sample_rate_integer;
    uint16_t sample_rate_fraction;
    uint16_t sample_rate_remainder;
    uint16_t sample_irq_ticks_remaining;
    const uint8_t *sample_data;
    uint32_t sample_irq_count;
    uint32_t sample_byte_count;
    uint32_t sample_reload_hash;
    uint32_t unsupported_samples;
} PortPc15State;

static atomic_flag s_audio_lock = ATOMIC_FLAG_INIT;
static enum PortAudioBackend s_backend;
static uint8_t *s_driver_image;
static size_t s_driver_image_size;
static PortPc15State s_pc15;
static Ad15Driver s_ad15;
static PortPcSpeaker s_speaker;
static int s_timer_registered;
static int s_audio_output_active;

extern void *audiodriverbinary;
extern unsigned char g_audiodrvvoices_count;
extern unsigned char audio_driver_mode;
extern unsigned char audio_driver_extension_mode;
extern unsigned char audio_pause_in_progress;
extern unsigned char audioflag2;
extern unsigned char audio_song_ready;
extern unsigned char audioflag6;
extern unsigned char g_musicvolumesetting;
extern unsigned char sfx_audio_vol;
extern void audiodriver_timer(void);
extern void reset_audio_driver_state(void);
extern void audiodrv_atexit(void);
extern void timer_reg_callback(void (*callback)(void));
extern void timer_remove_callback(void (*callback)(void));
extern char g_drvaudiocode[14];

static void audio_lock(void)
{
    while (atomic_flag_test_and_set_explicit(&s_audio_lock,
                                              memory_order_acquire)) {
    }
}

static void audio_unlock(void)
{
    atomic_flag_clear_explicit(&s_audio_lock, memory_order_release);
}

static uint16_t read_u16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static int16_t read_i16(const uint8_t *p)
{
    return (int16_t)read_u16(p);
}

static void write_u16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static uint16_t pc15_word(unsigned offset)
{
    if (s_driver_image == NULL || offset > s_driver_image_size ||
        s_driver_image_size - offset < 2u)
        return 0;
    return read_u16(s_driver_image + offset);
}

static int has_extension(const char *path)
{
    const char *base;
    const char *slash;
    const char *backslash;
    const char *dot;
    if (path == NULL)
        return 0;
    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    base = slash;
    if (backslash != NULL && (base == NULL || backslash > base))
        base = backslash;
    base = base != NULL ? base + 1 : path;
    dot = strrchr(base, '.');
    return dot != NULL && dot[1] != '\0';
}

static int make_driver_path(const char *name, char *path, size_t capacity)
{
    int written;
    if (name == NULL || name[0] == '\0' || path == NULL || capacity == 0)
        return 0;
    if (has_extension(name))
        written = snprintf(path, capacity, "%s", name);
    else
        written = snprintf(path, capacity, "%s.drv", name);
    return written >= 0 && (size_t)written < capacity;
}

static int is_pc15_name(const char *name)
{
    const char *base;
    const char *slash;
    const char *backslash;
    if (name == NULL)
        return 0;
    slash = strrchr(name, '/');
    backslash = strrchr(name, '\\');
    base = slash;
    if (backslash != NULL && (base == NULL || backslash > base))
        base = backslash;
    base = base != NULL ? base + 1 : name;
    return (base[0] == 'p' || base[0] == 'P') &&
           (base[1] == 'c' || base[1] == 'C');
}

static int is_ad15_name(const char *name)
{
    const char *base;
    const char *slash;
    const char *backslash;
    if (name == NULL)
        return 0;
    slash = strrchr(name, '/');
    backslash = strrchr(name, '\\');
    base = slash;
    if (backslash != NULL && (base == NULL || backslash > base))
        base = backslash;
    base = base != NULL ? base + 1 : name;
    return (base[0] == 'a' || base[0] == 'A') &&
           (base[1] == 'd' || base[1] == 'D');
}

static void ad15_write_register(void *userdata, uint8_t reg, uint8_t value)
{
    (void)userdata;
    port_opl3_write(reg, value);
}

static void pc15_silence_speaker(void)
{
    uint8_t gate = (uint8_t)(s_speaker.ppi_port61 & 0xfeu);
    port_pc_speaker_write(&s_speaker, 0x61u, gate);
}

static void pc15_entry_06_unlocked(void);

static void pc15_sample_restore_timer(void)
{
    /* PC15 +33's private restore helper (06A8) restores the channel-2 mode
       word to B6h and releases the temporary INT 8 stream. The host never
       changes a DOS vector; the SDL clock is owned by this adapter. */
    if (s_pc15.sample_timer_owned != 0) {
        s_pc15.sample_stream_active = 0;
        s_pc15.sample_timer_owned = 0;
        s_pc15.sample_data = NULL;
        s_pc15.sample_irq_ticks_remaining = 0;
        port_pc_speaker_write(&s_speaker, 0x43u, 0xB6u);
    }
}

static void pc15_reset_sample_timer(void)
{
    pc15_sample_restore_timer();
}

static int pc15_sample_stream_from_record(const uint8_t *sample_record,
                                          const uint8_t **data_out,
                                          uint16_t *length_out)
{
    uint32_t host_pointer;
    uintptr_t pointer_value;
    const uint8_t *stream;
    size_t available;
    uint16_t length;

    if (sample_record == NULL || data_out == NULL || length_out == NULL ||
        !port_memory_extent(sample_record, &available) || available < 10u)
        return 0;

    /* The game writes this four-byte far-pointer field when a linked audio
       shape is present. Raw PC VCE files leave it zero. On the SDL build the
       field contains a flat i686 host pointer, never a DOS 16:16 address. */
    memcpy(&host_pointer, sample_record + 6u, sizeof(host_pointer));
    pointer_value = (uintptr_t)host_pointer;
    if (host_pointer == 0 || pointer_value != (uintptr_t)(const void *)pointer_value)
        return 0;
    stream = (const uint8_t *)pointer_value;
    if (!port_memory_extent(stream, &available) || available < 0x32u + 2u)
        return 0;
    length = read_u16(stream + 8u);
    if ((size_t)length > available - 0x32u ||
        (uint32_t)length + 0x32u > 0xffffu)
        return 0;
    *data_out = stream + 0x32u;
    *length_out = length;
    return 1;
}

static void pc15_start_sample_stream(const uint8_t *sample_record,
                                     uint16_t rate)
{
    const uint8_t *data;
    uint16_t length;
    uint32_t rate_remainder;
    uint8_t gate;

    pc15_sample_restore_timer();
    if (!pc15_sample_stream_from_record(sample_record, &data, &length)) {
        ++s_pc15.unsupported_samples;
        pc15_silence_speaker();
        return;
    }

    /* +09 enters the private +33 path with mode zero, a one-pass count, the
       sample-rate word, and the far sample header. +33 uses PIT0 divisor 70
       (1193180 / 17000), retriggers PIT2 mode 1 once per IRQ, and starts data
       at header+32h. */
    port_pc_speaker_write(&s_speaker, 0x43u, 0xB2u);
    gate = (uint8_t)(s_speaker.ppi_port61 | 3u);
    port_pc_speaker_write(&s_speaker, 0x61u, gate);
    s_pc15.sample_timer_owned = 1;
    s_pc15.sample_data = data;
    s_pc15.sample_offset = 0x32u;
    s_pc15.sample_end_offset = (uint16_t)(0x32u + length);
    s_pc15.sample_repeat_count = 1;
    s_pc15.sample_rate_integer = (uint16_t)(rate / 17000u);
    rate_remainder = (uint32_t)rate % 17000u;
    s_pc15.sample_rate_fraction = (uint16_t)((rate_remainder << 16) / 17000u);
    s_pc15.sample_rate_remainder = 0;
    s_pc15.sample_irq_ticks_remaining = 70u;
    s_pc15.sample_reload_hash = 2166136261u;
    if (length == 0) {
        /* The original helper enables mode 1 before its zero-length return.
           It does not install a timer in this case, so expose it as unsupported
           rather than pretending the SDL stream contains audio samples. */
        s_pc15.sample_data = NULL;
        s_pc15.sample_irq_ticks_remaining = 0;
        ++s_pc15.unsupported_samples;
        return;
    }
    s_pc15.sample_stream_active = 1;
}

static uint8_t pc15_sample_reload(uint8_t sample_byte)
{
    /* 08A7-0888 constructs the byte lookup using the 70-count PIT0 reload. */
    return (uint8_t)(5u + ((uint32_t)sample_byte * 65u) / 255u);
}

static void pc15_sample_irq(void)
{
    uint32_t fraction;
    uint32_t next_offset;
    uint8_t value;
    uint8_t gate;

    ++s_pc15.sample_irq_count;
    fraction = (uint32_t)s_pc15.sample_rate_remainder +
               s_pc15.sample_rate_fraction;
    s_pc15.sample_rate_remainder = (uint16_t)fraction;
    next_offset = (uint32_t)s_pc15.sample_offset +
                  s_pc15.sample_rate_integer + (fraction >> 16);
    s_pc15.sample_offset = (uint16_t)next_offset;
    if (s_pc15.sample_offset >= s_pc15.sample_end_offset) {
        s_pc15.sample_offset = 0x32u;
        s_pc15.sample_rate_remainder = 0;
        if (s_pc15.sample_repeat_count != 0) {
            --s_pc15.sample_repeat_count;
            if (s_pc15.sample_repeat_count == 0) {
                pc15_sample_restore_timer();
                return;
            }
        }
    }

    value = s_pc15.sample_data[s_pc15.sample_offset - 0x32u];
    ++s_pc15.sample_byte_count;
    {
        uint8_t reload = pc15_sample_reload(value);
        s_pc15.sample_reload_hash =
            (s_pc15.sample_reload_hash ^ reload) * 16777619u;
        port_pc_speaker_write(&s_speaker, 0x42u, reload);
    }
    port_pc_speaker_write(&s_speaker, 0x42u, 0);
    gate = (uint8_t)(s_speaker.ppi_port61 & 0xfeu);
    port_pc_speaker_write(&s_speaker, 0x61u, gate);
    port_pc_speaker_write(&s_speaker, 0x61u, (uint8_t)(gate | 1u));
}

static uint8_t pc15_entry_00_unlocked(void)
{
    unsigned i;
    if (s_backend != PORT_AUDIO_PC15 || s_driver_image == NULL)
        return 0;
    port_pc_speaker_write(&s_speaker, 0x43u, 0xB6u);
    /* +00 calls +06 after selecting PIT channel 2, clears the driver's two
       saved timer-vector words, then writes five consecutive 7Fh gains. */
    pc15_entry_06_unlocked();
    s_pc15.sample_stream_active = 0;
    for (i = 0; i < 5; ++i)
        s_pc15.gain[i] = 0x7f;
    return 7;
}

static void pc15_entry_06_unlocked(void)
{
    uint8_t gate = (uint8_t)(s_speaker.ppi_port61 & 0xfcu);
    port_pc_speaker_write(&s_speaker, 0x61u, gate);
    s_pc15.active[1] = 0;
    s_pc15.active[2] = 0;
    s_pc15.active[3] = 0;
    s_pc15.active[4] = 0;
}

static void pc15_entry_03_unlocked(void)
{
    pc15_reset_sample_timer();
    pc15_entry_06_unlocked();
}

static uint16_t pc15_divisor_table(uint8_t index)
{
    return pc15_word(0x003fu + (unsigned)index * 2u);
}

static uint16_t pc15_frequency_table(uint8_t index)
{
    return pc15_word(0x012fu + (unsigned)index * 2u);
}

static uint16_t pc15_scaled_delta(uint16_t left, uint16_t right)
{
    uint32_t product = (uint32_t)left * (uint32_t)right;
    int32_t signed_product = (int32_t)product;
    return (uint16_t)((uint32_t)(signed_product >> 13) & 0xffffu);
}

static void pc15_entry_09_unlocked(int16_t channel, void *voice_pointer,
                                   int16_t note, const void *sample_pointer)
{
    uint8_t *voice = (uint8_t *)voice_pointer;
    uint8_t note_byte = (uint8_t)note;
    uint16_t base;
    if (s_backend != PORT_AUDIO_PC15 || voice == NULL)
        return;

    voice[3] = note_byte;
    if (channel == 0) {
        base = pc15_frequency_table(note_byte);
        write_u16(voice + 4, base);
        write_u16(voice + 6, base);
        s_pc15.active[0] = 0xff;
        pc15_start_sample_stream((const uint8_t *)sample_pointer, base);
        return;
    }
    if (channel < 0 || channel >= 7)
        return;
    if (note_byte != 0xffu) {
        base = pc15_divisor_table(note_byte);
        write_u16(voice + 4, base);
        write_u16(voice + 6, base);
    }
    s_pc15.active[channel] = 0xff;
}

static void pc15_entry_0f_unlocked(int16_t channel)
{
    if (s_backend != PORT_AUDIO_PC15)
        return;
    if (channel == 0)
        pc15_reset_sample_timer();
    if (channel >= 0 && channel < 7)
        s_pc15.active[channel] = 0;
}

static void pc15_entry_12_unlocked(int16_t channel, int16_t value)
{
    if (s_backend != PORT_AUDIO_PC15 || channel < 0 || channel >= 7)
        return;
    s_pc15.gain[channel] = (uint8_t)value;
}

static void pc15_entry_24_unlocked(int16_t channel, void *voice_pointer,
                                   uint16_t value)
{
    uint16_t divisor;
    uint8_t *voice = (uint8_t *)voice_pointer;
    if (s_backend != PORT_AUDIO_PC15 || channel == 0 || voice == NULL ||
        channel < 0 || channel >= 7)
        return;
    divisor = value <= 0x0130u ? 0 : (uint16_t)(0x4dc0u / value);
    write_u16(voice + 4, divisor);
    write_u16(voice + 6, divisor);
}

static void pc15_entry_27_unlocked(uint16_t channel, void *voice_pointer,
                                   void *chunk_pointer,
                                   const void *sample_pointer)
{
    uint8_t *voice = (uint8_t *)voice_pointer;
    uint8_t *chunk = (uint8_t *)chunk_pointer;
    const uint8_t *sample = (const uint8_t *)sample_pointer;
    uint8_t table_index;
    uint16_t cx;
    int16_t bend;
    uint16_t ax;
    uint16_t dx;
    if (s_backend != PORT_AUDIO_PC15 || channel >= 7 || voice == NULL ||
        chunk == NULL || sample == NULL || voice[1] == 0)
        return;

    if (sample[0x35] == 1) {
        table_index = (uint8_t)(voice[0x22] + voice[3]);
        cx = pc15_divisor_table(table_index);
    } else {
        cx = read_u16(voice + 4);
        table_index = voice[3];
    }
    cx = (uint16_t)(cx - (int16_t)(int8_t)sample[0x11]);
    bend = read_i16(chunk + 0x26);
    if (bend > 0) {
        table_index = (uint8_t)(table_index + sample[0x12]);
        ax = (uint16_t)(0u - pc15_divisor_table(table_index));
        ax = (uint16_t)(ax + cx);
        dx = (uint16_t)bend;
        if (dx >= 0x1000u)
            dx = (uint16_t)(dx + 0x80u);
        cx = (uint16_t)(cx - pc15_scaled_delta(ax, dx));
    } else if (bend < 0) {
        table_index = (uint8_t)(table_index - sample[0x12]);
        ax = (uint16_t)(pc15_divisor_table(table_index) - cx);
        dx = (uint16_t)(0u - (uint16_t)bend);
        cx = (uint16_t)(cx + pc15_scaled_delta(ax, dx));
    }
    if (sample[0x28] == 2)
        cx = (uint16_t)(cx + read_u16(voice + 0x1c));
    if (sample[0x19] == 2)
        cx = (uint16_t)(cx + read_u16(chunk + 0x14));

    s_pc15.divisor[channel] = cx;
    write_u16(voice + 6, cx);
}

static void pc15_entry_30_unlocked(void)
{
    int selected = -1;
    unsigned i;
    uint8_t phase;
    if (s_backend != PORT_AUDIO_PC15)
        return;
    s_pc15.mixed[0] = s_pc15.active[0];
    for (i = 1; i < 7; ++i)
        s_pc15.mixed[i] = s_pc15.gain[i] != 0 ? s_pc15.active[i] : 0;

    /* The PC15 +30 selector is translated from 05DF-06A7. Voice zero owns
       the driver-private stream path; other voices follow its literal
       priority/phase branches. */
    if (s_pc15.mixed[0] == 0) {
        if (s_pc15.mixed[6] != 0)
            selected = 6;
        else if (s_pc15.mixed[5] != 0)
            selected = 5;
        else if (s_pc15.mixed[3] != 0 || s_pc15.mixed[4] != 0) {
            phase = s_pc15.mix_phase;
            if ((phase & 2u) != 0 &&
                (s_pc15.mixed[1] != 0 || s_pc15.mixed[2] != 0)) {
                if (s_pc15.mixed[1] != 0)
                    selected = 1;
                else if (s_pc15.mixed[2] != 0)
                    selected = 2;
            } else if (s_pc15.mixed[3] != 0) {
                selected = 3;
            } else {
                selected = 4;
            }
        } else if (s_pc15.mixed[1] != 0) {
            selected = 1;
        } else if (s_pc15.mixed[2] != 0) {
            selected = 2;
        }
        if (selected >= 0)
            port_pc_speaker_write_divisor(&s_speaker,
                                          s_pc15.divisor[selected]);
        else
            pc15_silence_speaker();
    }
    s_pc15.mix_phase = (uint8_t)(s_pc15.mix_phase + 1u);
}

int port_audio_backend_load(const char *driver_name)
{
    char driver_path[256];
    size_t image_size = 0;
    PortFarPtr image_address;
    uint8_t *image = NULL;
    int pc15_selected;
    int ad15_selected;
    int output_active;

    port_audio_shutdown();
    pc15_selected = is_pc15_name(driver_name);
    ad15_selected = is_ad15_name(driver_name);
    if ((pc15_selected || ad15_selected) &&
        make_driver_path(driver_name, driver_path, sizeof(driver_path))) {
        image = (uint8_t *)port_fs_load(driver_path, &image_size,
                                        &image_address);
    }
    if (pc15_selected && image != NULL && image_size >= 0x08b3u &&
        image[0] == 0xe9u) {
        audio_lock();
        s_driver_image = image;
        s_driver_image_size = image_size;
        s_backend = PORT_AUDIO_PC15;
        memset(&s_pc15, 0, sizeof(s_pc15));
        port_pc_speaker_reset(&s_speaker);
        audiodriverbinary = image;
        audio_driver_mode = 0;
        audio_driver_extension_mode = 0;
        g_audiodrvvoices_count = 7;
        s_audio_output_active = 0;
        audio_unlock();

        (void)port_audio_driver_00();
        g_musicvolumesetting = 0x7f;
        sfx_audio_vol = 0x7f;
        audio_pause_in_progress = 0;
        audioflag2 = 1;
        audio_song_ready = 0;
        audioflag6 = 1;
        reset_audio_driver_state();
        timer_reg_callback(audiodriver_timer);
        s_timer_registered = 1;
        output_active = port_audio_sdl_start();
        audio_lock();
        s_audio_output_active = output_active;
        audio_unlock();
        if (output_active)
            fprintf(stderr, "PORT audio: PC15 PIT speaker output opened through SDL3\n");
        else
            fprintf(stderr,
                    "PORT audio: PC15 call model selected; SDL3 playback unavailable\n");
        return 1;
    }

    if (ad15_selected && image != NULL && image_size >= 0x0882u &&
        image[0] == 0xe9u) {
        port_opl3_reset(48000u);
        audio_lock();
        s_driver_image = image;
        s_driver_image_size = image_size;
        s_backend = PORT_AUDIO_AD15;
        memset(&s_pc15, 0, sizeof(s_pc15));
        port_pc_speaker_reset(&s_speaker);
        ad15_driver_bind(&s_ad15, image, image_size,
                         ad15_write_register, NULL);
        audiodriverbinary = image;
        audio_driver_mode = 0;
        audio_driver_extension_mode = 0;
        g_audiodrvvoices_count = 10;
        s_audio_output_active = 0;
        audio_unlock();

        if (port_audio_driver_00() == 0u) {
            port_audio_shutdown();
            return 0;
        }
        g_musicvolumesetting = 0x7f;
        sfx_audio_vol = 0x7f;
        audio_pause_in_progress = 0;
        audioflag2 = 1;
        audio_song_ready = 0;
        audioflag6 = 1;
        reset_audio_driver_state();
        timer_reg_callback(audiodriver_timer);
        s_timer_registered = 1;
        output_active = port_audio_sdl_start();
        audio_lock();
        s_audio_output_active = output_active;
        audio_unlock();
        if (output_active)
            fprintf(stderr, "PORT audio: AD15 OPL2 output opened through SDL3\n");
        else
            fprintf(stderr,
                    "PORT audio: AD15 OPL2 call model selected; SDL3 playback unavailable\n");
        return 1;
    }

    if (image != NULL)
        mmgr_release(image);
    audio_lock();
    s_driver_image = NULL;
    s_driver_image_size = 0;
    s_backend = PORT_AUDIO_NONE;
    s_audio_output_active = 0;
    memset(&s_pc15, 0, sizeof(s_pc15));
    memset(&s_ad15, 0, sizeof(s_ad15));
    port_pc_speaker_reset(&s_speaker);
    audiodriverbinary = NULL;
    g_audiodrvvoices_count = 0;
    audio_driver_mode = 0;
    audio_driver_extension_mode = 0;
    audio_unlock();
    if (driver_name == NULL)
        driver_name = "default";
    fprintf(stderr,
            "PORT audio: no translated driver for %s; audio safely disabled\n",
            driver_name);
    return 0;
}

int port_audio_silent_load(const char *driver_name)
{
    (void)port_audio_backend_load(driver_name);
    return 0;
}

int port_audio_is_active(void)
{
    int active;
    audio_lock();
    active = s_backend != PORT_AUDIO_NONE;
    audio_unlock();
    return active;
}

int port_audio_is_silent(void)
{
    return !port_audio_is_active();
}

const char *port_audio_backend_name(void)
{
    const char *name;
    audio_lock();
    if (s_backend == PORT_AUDIO_NONE)
        name = "silent";
    else if (s_backend == PORT_AUDIO_AD15)
        name = !s_audio_output_active ? "ad15-opl2-unavailable" :
               s_ad15.unsupported_sample_calls != 0 ?
                   "ad15-opl2-partial" : "ad15-opl2";
    else
        name = s_audio_output_active ?
                   (s_pc15.unsupported_samples != 0 ?
                        "pc15-pit-partial" : "pc15-pit") :
                   "pc15-pit-unavailable";
    audio_unlock();
    return name;
}

void port_audio_shutdown(void)
{
    uint8_t *image;
    void *binary;
    if (s_timer_registered) {
        timer_remove_callback(audiodriver_timer);
        s_timer_registered = 0;
    }
    /* Stop/join the SDL callback before taking the state lock it uses. */
    port_audio_sdl_stop();
    audio_lock();
    if (s_backend == PORT_AUDIO_PC15)
        pc15_entry_03_unlocked();
    else if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_03(&s_ad15);
    image = s_driver_image;
    binary = audiodriverbinary;
    s_driver_image = NULL;
    s_driver_image_size = 0;
    s_backend = PORT_AUDIO_NONE;
    s_audio_output_active = 0;
    memset(&s_pc15, 0, sizeof(s_pc15));
    memset(&s_ad15, 0, sizeof(s_ad15));
    port_pc_speaker_reset(&s_speaker);
    audiodriverbinary = NULL;
    g_audiodrvvoices_count = 0;
    audio_driver_mode = 0;
    audio_driver_extension_mode = 0;
    audio_unlock();
    if (image != NULL)
        mmgr_release(image);
    else if (binary != NULL)
        mmgr_release(binary);
}

/* Original audio_stop_unknown only restores the DOS INT 8 vector when that
   vector still belongs to its temporary handler at CS:1909. SDL owns no guest
   interrupt vector, so the corresponding host action is limited to releasing
   the PC15 stream timer state (the following audiodrv_atexit unloads the
   driver and closes the SDL stream). */
void audio_stop_unknown(void)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_PC15)
        pc15_sample_restore_timer();
    else if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_stop_unknown(&s_ad15);
    audio_unlock();
}

size_t port_audio_render_s16(int16_t *output, size_t frames,
                             unsigned sample_rate)
{
    size_t i;
    audio_lock();
    if (output == NULL) {
        audio_unlock();
        return 0;
    }
    if (sample_rate == 0) {
        memset(output, 0, frames * sizeof(*output));
        audio_unlock();
        return frames;
    }
    if (s_backend == PORT_AUDIO_AD15) {
        size_t rendered = port_opl3_render_s16(output, frames, sample_rate);
        audio_unlock();
        return rendered;
    }
    if (s_speaker.output_rate != sample_rate) {
        s_speaker.output_rate = sample_rate;
        s_speaker.sample_clock_remainder = 0;
    }
    for (i = 0; i < frames; ++i) {
        uint64_t numerator = s_speaker.sample_clock_remainder +
                             PORT_PC_PIT_HZ;
        uint32_t clocks = (uint32_t)(numerator / sample_rate);
        s_speaker.sample_clock_remainder = numerator % sample_rate;
        while (clocks != 0) {
            uint32_t step = clocks;
            if (s_pc15.sample_stream_active != 0 &&
                step > s_pc15.sample_irq_ticks_remaining)
                step = s_pc15.sample_irq_ticks_remaining;
            port_pc_speaker_advance_ticks(&s_speaker, step);
            clocks -= step;
            if (s_pc15.sample_stream_active != 0) {
                s_pc15.sample_irq_ticks_remaining =
                    (uint16_t)(s_pc15.sample_irq_ticks_remaining - step);
                if (s_pc15.sample_irq_ticks_remaining == 0) {
                    s_pc15.sample_irq_ticks_remaining = 70u;
                    pc15_sample_irq();
                }
            }
        }
        {
            int level = port_pc_speaker_output_level(&s_speaker);
            output[i] = level < 0 ? 0 : level ? 4096 : (int16_t)-4096;
        }
    }
    audio_unlock();
    return frames;
}

uint8_t port_audio_driver_00(void)
{
    uint8_t voices;
    audio_lock();
    voices = s_backend == PORT_AUDIO_AD15 ? ad15_driver_00(&s_ad15) :
             pc15_entry_00_unlocked();
    audio_unlock();
    return voices;
}

void port_audio_driver_03(void)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_03(&s_ad15);
    else
        pc15_entry_03_unlocked();
    audio_unlock();
}

void port_audio_driver_06(void)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_06(&s_ad15);
    else
        pc15_entry_06_unlocked();
    audio_unlock();
}

void port_audio_driver_09(int16_t channel, void *voice, void *chunk,
                          int16_t note, int16_t value,
                          const void *sample)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_09(&s_ad15, channel, voice, chunk, note, value, sample);
    else
        pc15_entry_09_unlocked(channel, voice, note, sample);
    audio_unlock();
}

void port_audio_driver_0c(int16_t channel, void *voice)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_0c(&s_ad15, channel, voice);
    audio_unlock();
}

void port_audio_driver_0f(int16_t channel, void *voice)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_0f(&s_ad15, channel, voice);
    else
        pc15_entry_0f_unlocked(channel);
    audio_unlock();
}

void port_audio_driver_12(int16_t channel, void *voice, int16_t value)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_12(&s_ad15, channel, voice, value);
    else
        pc15_entry_12_unlocked(channel, value);
    audio_unlock();
}

void port_audio_driver_15(int16_t channel, void *voice,
                          int16_t type, int16_t value)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_15(&s_ad15, channel, voice, type, value);
    audio_unlock();
}

void port_audio_driver_18(void)
{
}

void port_audio_driver_1b(void *chunk, int16_t value, int16_t channel)
{
    (void)chunk;
    (void)value;
    (void)channel;
}

void port_audio_driver_1e(int16_t channel)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_1e(&s_ad15, channel);
    else
        pc15_entry_0f_unlocked(channel);
    audio_unlock();
}

void port_audio_driver_21(int16_t channel, void *voice, void *chunk,
                          const void *sample)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_21(&s_ad15, channel, voice, chunk, sample);
    audio_unlock();
}

void port_audio_driver_24(int16_t channel, void *voice, uint16_t value)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_24(&s_ad15, channel, voice, value);
    else
        pc15_entry_24_unlocked(channel, voice, value);
    audio_unlock();
}

void port_audio_driver_27(uint16_t channel, void *voice, void *chunk,
                          const void *sample)
{
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        ad15_driver_27(&s_ad15, channel, voice, chunk, sample);
    else
        pc15_entry_27_unlocked(channel, voice, chunk, sample);
    audio_unlock();
}

void port_audio_driver_30(void *voices)
{
    (void)voices;
    audio_lock();
    pc15_entry_30_unlocked();
    audio_unlock();
}

void port_audio_driver_39(int16_t count, const uint8_t *bytes)
{
    (void)count;
    (void)bytes;
}

void port_audio_driver_3f(int16_t count, const void *bytes)
{
    (void)count;
    (void)bytes;
}

void port_audio_driver_42(const void *patches)
{
    (void)patches;
}

int port_audio_driver_entry_33_supported(void)
{
    return 0;
}

uint32_t port_audio_pc15_unsupported_sample_count(void)
{
    uint32_t count;
    audio_lock();
    count = s_pc15.unsupported_samples;
    audio_unlock();
    return count;
}

uint32_t port_audio_pc15_sample_irq_count(void)
{
    uint32_t count;
    audio_lock();
    count = s_pc15.sample_irq_count;
    audio_unlock();
    return count;
}

uint32_t port_audio_pc15_sample_byte_count(void)
{
    uint32_t count;
    audio_lock();
    count = s_pc15.sample_byte_count;
    audio_unlock();
    return count;
}

uint32_t port_audio_pc15_sample_reload_hash(void)
{
    uint32_t hash;
    audio_lock();
    hash = s_pc15.sample_reload_hash;
    audio_unlock();
    return hash;
}

uint16_t port_audio_driver_36(int16_t channel)
{
    uint16_t result = 1;
    audio_lock();
    if (s_backend == PORT_AUDIO_AD15)
        result = ad15_driver_36(&s_ad15, channel);
    else if (s_backend == PORT_AUDIO_PC15 && channel == 0 && s_pc15.active[0] != 0)
        result = 0;
    audio_unlock();
    return result;
}

uint16_t port_audio_driver_3c(void)
{
    /* PC15's +3C vector is `mov ax,0ffffh; retf`. */
    return 0xffffu;
}

/* Keep the C caller's filename-prefix contract. The bytes loaded for PC15 are
   retained as inert data for lookup tables; their DOS instruction stream is
   never executed or cast to a host function. */
int16_t audio_load_driver(char *filename, int16_t unused, int16_t signature)
{
    const char *base = filename;
    const char *backslash;
    const char *slash;
    (void)unused;
    (void)signature;
    if (filename != NULL) {
        backslash = strrchr(filename, '\\');
        slash = strrchr(filename, '/');
        if (backslash != NULL && (slash == NULL || backslash > slash))
            base = backslash + 1;
        else if (slash != NULL)
            base = slash + 1;
        if (base[0] != '\0' && base[1] != '\0') {
            g_drvaudiocode[0] = base[0];
            g_drvaudiocode[1] = base[1];
            g_drvaudiocode[2] = '\0';
        }
    }
    (void)port_audio_backend_load(filename);
    return 0;
}
