#include "ad15_driver.h"

#include <string.h>

enum {
    AD15_REGISTER_OFFSETS = 0x0870,
    AD15_NOTE_TABLE = 0x08b2,
    AD15_SAMPLE_RATE_TABLE = 0x08fa,
    AD15_MIN_IMAGE_SIZE = 0x0882
};

static uint16_t read_u16(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static void write_u16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static uint8_t driver_byte(const Ad15Driver *driver, size_t offset)
{
    return driver != NULL && driver->image != NULL &&
           offset < driver->image_size ? driver->image[offset] : 0;
}

static uint16_t driver_word(const Ad15Driver *driver, size_t offset)
{
    if (driver == NULL || driver->image == NULL || offset > driver->image_size ||
        driver->image_size - offset < 2u)
        return 0;
    return read_u16(driver->image + offset);
}

static void write_register(Ad15Driver *driver, uint8_t reg, uint8_t value)
{
    if (driver == NULL || driver->write_register == NULL ||
        driver->register_cache[reg] == value)
        return;
    driver->register_cache[reg] = value;
    driver->write_register(driver->write_userdata, reg, value);
}

static uint8_t operator_offset(const Ad15Driver *driver, unsigned index)
{
    return driver_byte(driver, AD15_REGISTER_OFFSETS + index);
}

static void reset_channel(Ad15Driver *driver, unsigned channel)
{
    unsigned pair = channel * 2u;
    uint8_t modulator = operator_offset(driver, pair);
    uint8_t carrier = operator_offset(driver, pair + 1u);

    write_register(driver, (uint8_t)(0x40u + modulator), 0x3fu);
    write_register(driver, (uint8_t)(0x40u + carrier), 0x3fu);
    write_register(driver, (uint8_t)(0x80u + modulator), 0x0au);
    write_register(driver, (uint8_t)(0x80u + carrier), 0x0au);
    write_register(driver, (uint8_t)(0xb0u + channel), 0x01u);
}

/* AD15's 06F9 helper: note/12 selects the block, the remainder plus a signed
   VCE bias indexes the driver's own f-number table. The assembly does not
   renormalize a biased index, so this deliberately preserves that behavior. */
static uint16_t note_pitch(const Ad15Driver *driver, uint8_t note,
                           int8_t bias)
{
    int8_t table_index = (int8_t)(uint8_t)((note % 12u) + (uint8_t)bias);
    unsigned block = note / 12u;
    int32_t signed_offset = (int32_t)AD15_NOTE_TABLE +
                            (int32_t)table_index * 2;
    uint16_t f_number = signed_offset >= 0
                            ? driver_word(driver, (size_t)signed_offset)
                            : 0;
    return (uint16_t)(f_number | (uint16_t)(block << 10));
}

static const uint8_t *voice_sample(const uint8_t *voice)
{
    uint32_t address;
    if (voice == NULL)
        return NULL;
    memcpy(&address, voice + 0x10u, sizeof(address));
    return (const uint8_t *)(uintptr_t)address;
}

static void set_operator_volume(Ad15Driver *driver, unsigned channel,
                                const uint8_t *sample, uint8_t chunk_volume,
                                uint8_t velocity)
{
    unsigned pair = channel * 2u;
    unsigned op;
    for (op = 0; op != 2; ++op) {
        unsigned field = op == 0 ? 0x4au : 0x56u;
        unsigned high_field = field + 1u;
        unsigned table_index = pair + op;
        uint8_t level = sample[field];
        uint8_t result = (uint8_t)(sample[high_field] << 6);

        /* 0730 scales both operators in additive connection mode. In FM
           connection mode it preserves the modulator's VCE level and scales
           only the carrier; the branch surrounds only the first operator's
           calculation in the original routine. */
        if (op != 0u || sample[0x44] == 1u) {
            uint32_t scaled = (uint32_t)chunk_volume * velocity;
            uint32_t attenuation;
            scaled = ((scaled >> 6) + 1u) >> 1;
            attenuation = (uint32_t)(0x3fu - sample[field]) * scaled;
            attenuation = ((attenuation >> 6) + 1u) >> 1;
            attenuation &= 0x3fu;
            level = (uint8_t)(0x3fu - attenuation);
        }
        result = (uint8_t)(result | level);
        write_register(driver,
                       (uint8_t)(0x40u + operator_offset(driver, table_index)),
                       result);
    }
}

static void set_chunk_volume(Ad15Driver *driver, unsigned channel,
                             const uint8_t *sample, uint8_t chunk_volume,
                             uint8_t velocity)
{
    set_operator_volume(driver, channel, sample, chunk_volume, velocity);
}

static void set_parameter(Ad15Driver *driver, unsigned channel,
                          const uint8_t *sample, uint8_t parameter,
                          uint8_t value)
{
    unsigned pair = channel * 2u;
    uint8_t modulator = operator_offset(driver, pair);
    uint8_t carrier = operator_offset(driver, pair + 1u);
    uint8_t reg;
    uint8_t data;
    unsigned base;

    switch (parameter) {
    case 0x81u:
        base = 0x46u;
        reg = (uint8_t)(0x20u + modulator);
        data = (uint8_t)((((((sample[base + 10u] << 1) |
                              sample[base + 9u]) << 1 |
                             sample[base + 8u]) << 1 |
                            sample[base + 7u]) << 4) |
                          ((value >> 3) & 7u));
        write_register(driver, reg, data);
        break;
    case 0x82u:
        base = 0x52u;
        reg = (uint8_t)(0x20u + carrier);
        data = (uint8_t)((((((sample[base + 10u] << 1) |
                              sample[base + 9u]) << 1 |
                             sample[base + 8u]) << 1 |
                            sample[base + 7u]) << 4) |
                          ((value >> 3) & 7u));
        write_register(driver, reg, data);
        break;
    case 0x83u:
    case 0x84u: {
        unsigned offset = parameter == 0x83u ? modulator : carrier;
        unsigned high_offset = parameter == 0x83u ? 0x4bu : 0x57u;
        uint8_t attenuation = (uint8_t)(((0x7fu - value) >> 1) & 0x3fu);
        data = (uint8_t)((sample[high_offset] << 6) | attenuation);
        write_register(driver, (uint8_t)(0x40u + offset), data);
        break;
    }
    case 0x85u:
        data = (uint8_t)((((value >> 4) & 7u) << 1) | sample[0x44]);
        write_register(driver, (uint8_t)(0xc0u + channel), data);
        break;
    default:
        break;
    }
}

static void update_pitch(Ad15Driver *driver, unsigned channel,
                         uint8_t *voice, const uint8_t *chunk,
                         const uint8_t *sample)
{
    uint16_t current;
    uint16_t target;
    int16_t bend;
    int32_t delta;
    int32_t product;
    int16_t pitch_bias;
    uint8_t note;
    unsigned register_channel = channel - 1u;

    if (voice == NULL || chunk == NULL || sample == NULL ||
        voice[1] == 0u)
        return;

    current = read_u16(voice + 4u);
    if (sample[0x35] == 0x91u) {
        note = (uint8_t)(voice[3] + voice[0x22]);
        target = note_pitch(driver, note, 0);
        current = target;
    }
    if (sample[0x28] == 0x90u)
        current = (uint16_t)(current + read_u16(voice + 0x1cu));
    if (sample[0x19] == 0x90u)
        current = (uint16_t)(current + read_u16(voice + 0x14u));

    bend = (int16_t)read_u16(chunk + 0x26u);
    if (bend != 0) {
        pitch_bias = (int16_t)(int8_t)sample[0x12];
        if (bend < 0)
            pitch_bias = (int16_t)-pitch_bias;
        target = note_pitch(driver, voice[3], (int8_t)pitch_bias);
        product = ((int32_t)(int16_t)(target - current)) *
                  (bend < 0 ? -(int32_t)bend : (int32_t)bend);
        /* The original shifts the signed 32-bit product thirteen places,
           using arithmetic shifts, before adding it to the current pitch. */
        delta = product >= 0 ? product / 8192 :
                -(((-product) + 8191) / 8192);
        current = (uint16_t)(current + delta);
    }
    current = (uint16_t)(current + (int16_t)(int8_t)sample[0x11]);
    write_u16(voice + 6u, current);
    write_register(driver, (uint8_t)(0xa0u + register_channel),
                   (uint8_t)current);
    write_register(driver, (uint8_t)(0xb0u + register_channel),
                   (uint8_t)((uint8_t)(2u - voice[1]) << 5 |
                             (uint8_t)(current >> 8)));

    set_parameter(driver, register_channel, sample, sample[0x35],
                  voice[0x22]);
    set_parameter(driver, register_channel, sample, sample[0x28],
                  (uint8_t)read_u16(voice + 0x1cu));
    set_parameter(driver, register_channel, sample, sample[0x19],
                  (uint8_t)read_u16(voice + 0x14u));
}

void ad15_driver_bind(Ad15Driver *driver, const uint8_t *image,
                      size_t image_size, Ad15WriteRegister write_fn,
                      void *write_userdata)
{
    if (driver == NULL)
        return;
    memset(driver, 0, sizeof(*driver));
    driver->image = image;
    driver->image_size = image_size;
    driver->write_register = write_fn;
    driver->write_userdata = write_userdata;
}

uint8_t ad15_driver_00(Ad15Driver *driver)
{
    unsigned reg;
    unsigned channel;
    static const uint8_t detection[][2] = {
        {0x04u, 0x60u}, {0x04u, 0x80u}, {0x02u, 0xffu},
        {0x04u, 0x21u}, {0x04u, 0x60u}, {0x04u, 0x80u}
    };
    size_t i;
    if (driver == NULL || driver->image == NULL ||
        driver->image_size < AD15_MIN_IMAGE_SIZE ||
        driver->write_register == NULL)
        return 0;

    /* OPL hardware discovery is replaced by the pinned emulated OPL core.
       AD15's post-detection register initialization and cache semantics are
       preserved here. Its 03F routine seeds every cache entry to 1, then
       writes zero to all 256 first-bank registers. */
    /* Successful 0388h timer probe: these are the register/data pairs in
       AD15's +0810 detection helper before it seeds the 03F cache. */
    for (i = 0; i < sizeof(detection) / sizeof(detection[0]); ++i)
        write_register(driver, detection[i][0], detection[i][1]);
    memset(driver->velocity, 0, sizeof(driver->velocity));
    driver->sample_timer_active = 0;
    driver->initialized = 1;
    /* The machine loop uses CX=00FFh and therefore clears indices 0..254;
       byte 255 remains whatever the loaded driver data held there. */
    for (reg = 0; reg != 255u; ++reg) {
        driver->register_cache[reg] = 1u;
        write_register(driver, (uint8_t)reg, 0);
    }

    write_register(driver, 0x01u, 0x20u);
    write_register(driver, 0x08u, 0x00u);
    write_register(driver, 0xbdu, 0x00u);
    for (channel = 0; channel != 9u; ++channel)
        reset_channel(driver, channel);
    return 10u;
}

void ad15_driver_03(Ad15Driver *driver)
{
    unsigned channel;
    if (driver == NULL || !driver->initialized)
        return;
    for (channel = 0; channel != 9u; ++channel)
        reset_channel(driver, channel);
    driver->sample_timer_active = 0;
}

void ad15_driver_06(Ad15Driver *driver)
{
    ad15_driver_03(driver);
}

void ad15_driver_09(Ad15Driver *driver, int16_t channel, void *voice_pointer,
                    void *chunk_pointer, int16_t note_value,
                    int16_t velocity_value, const void *sample_pointer)
{
    uint8_t *voice = (uint8_t *)voice_pointer;
    uint8_t *chunk = (uint8_t *)chunk_pointer;
    const uint8_t *sample = (const uint8_t *)sample_pointer;
    unsigned index;
    uint8_t note;
    uint16_t pitch;
    uint8_t velocity;
    uint8_t modulator;
    uint8_t carrier;

    if (driver == NULL || !driver->initialized || channel < 0 ||
        channel > 9 || voice == NULL || sample == NULL)
        return;
    if (channel == 0) {
        /* All shipped AD15 VCE records have +0A == 0, so this branch is not
           used by their music/effects. The driver requires a nested sample
           descriptor and an INT 8 stream; that case remains unsupported. */
        if (sample[0x0a] != 0u)
            ++driver->unsupported_sample_calls;
        return;
    }
    if (chunk == NULL)
        return;
    index = (unsigned)channel - 1u;
    note = (uint8_t)note_value;
    voice[3] = note;
    pitch = read_u16(voice + 4u);
    if (note != 0xffu) {
        pitch = note_pitch(driver, note, 0);
        write_u16(voice + 4u, pitch);
        pitch = (uint16_t)(pitch + (int16_t)(int8_t)sample[0x11]);
        write_u16(voice + 6u, pitch);
    }

    modulator = operator_offset(driver, index * 2u);
    carrier = operator_offset(driver, index * 2u + 1u);
    write_register(driver, (uint8_t)(0x40u + modulator), 0x3fu);
    write_register(driver, (uint8_t)(0x40u + carrier), 0x3fu);
    write_register(driver, (uint8_t)(0xa0u + index), (uint8_t)pitch);
    write_register(driver, (uint8_t)(0xb0u + index),
                   (uint8_t)(0x20u | (uint8_t)(pitch >> 8)));

    velocity = sample[0x15] != 0u ? (uint8_t)velocity_value : 0x7fu;
    driver->velocity[index] = velocity;
    set_chunk_volume(driver, index, sample, chunk[0x28], velocity);
}

void ad15_driver_0c(Ad15Driver *driver, int16_t channel, void *voice_pointer)
{
    const uint8_t *voice = (const uint8_t *)voice_pointer;
    uint16_t pitch;
    unsigned index;
    if (driver == NULL || !driver->initialized || channel <= 0 ||
        channel > 9 || voice == NULL)
        return;
    index = (unsigned)channel - 1u;
    pitch = read_u16(voice + 6u);
    write_register(driver, (uint8_t)(0xb0u + index),
                   (uint8_t)(pitch >> 8));
}

void ad15_driver_0f(Ad15Driver *driver, int16_t channel, void *voice)
{
    unsigned index;
    if (driver == NULL || !driver->initialized || channel < 0 || channel > 9)
        return;
    (void)voice;
    if (channel == 0) {
        ad15_driver_stop_unknown(driver);
        return;
    }
    index = (unsigned)channel - 1u;
    write_register(driver,
                   (uint8_t)(0x40u + operator_offset(driver, index * 2u)),
                   0x3fu);
    write_register(driver,
                   (uint8_t)(0x40u + operator_offset(driver, index * 2u + 1u)),
                   0x3fu);
}

void ad15_driver_12(Ad15Driver *driver, int16_t channel, void *voice_pointer,
                    int16_t value)
{
    uint8_t *voice = (uint8_t *)voice_pointer;
    const uint8_t *sample;
    unsigned index;
    if (driver == NULL || !driver->initialized || channel < 0 ||
        channel > 9)
        return;
    if (channel == 0) {
        ad15_driver_stop_unknown(driver);
        return;
    }
    if (voice == NULL)
        return;
    index = (unsigned)channel - 1u;
    sample = voice_sample(voice);
    if (sample == NULL)
        return;
    set_chunk_volume(driver, index, sample, (uint8_t)value,
                     driver->velocity[index]);
}

void ad15_driver_15(Ad15Driver *driver, int16_t channel, void *voice_pointer,
                    int16_t type, int16_t value)
{
    uint8_t *voice = (uint8_t *)voice_pointer;
    const uint8_t *sample;
    unsigned index;
    uint8_t parameter;

    if (driver == NULL || !driver->initialized || channel <= 0 ||
        channel > 9 || voice == NULL || voice[1] == 0u)
        return;
    index = (unsigned)channel - 1u;
    sample = voice_sample(voice);
    if (sample == NULL)
        return;
    switch ((uint16_t)type) {
    case 1u:
        parameter = sample[0x16];
        set_parameter(driver, index, sample, parameter, (uint8_t)value);
        break;
    case 7u:
        set_chunk_volume(driver, index, sample, (uint8_t)value,
                         driver->velocity[index]);
        break;
    case 11u:
        parameter = sample[0x17];
        set_parameter(driver, index, sample, parameter, (uint8_t)value);
        break;
    case 12u:
        parameter = sample[0x18];
        set_parameter(driver, index, sample, parameter, (uint8_t)value);
        break;
    default:
        break;
    }
}

void ad15_driver_21(Ad15Driver *driver, int16_t channel, void *voice,
                    void *chunk_pointer, const void *sample_pointer)
{
    const uint8_t *sample = (const uint8_t *)sample_pointer;
    uint8_t *chunk = (uint8_t *)chunk_pointer;
    unsigned index;
    unsigned pair;
    uint8_t modulator;
    uint8_t carrier;
    unsigned base;
    unsigned op;

    (void)voice;
    if (driver == NULL || !driver->initialized || channel <= 0 ||
        channel > 9 || chunk == NULL || sample == NULL)
        return;
    index = (unsigned)channel - 1u;
    pair = index * 2u;
    modulator = operator_offset(driver, pair);
    carrier = operator_offset(driver, pair + 1u);

    write_register(driver, (uint8_t)(0xc0u + index),
                   (uint8_t)((sample[0x45] << 1) | sample[0x44]));

    for (op = 0; op != 2u; ++op) {
        base = op == 0 ? 0x46u : 0x52u;
        {
            uint8_t offset = op == 0 ? modulator : carrier;
            uint8_t value = (uint8_t)((((((sample[base + 10u] << 1) |
                                          sample[base + 9u]) << 1 |
                                         sample[base + 8u]) << 1 |
                                        sample[base + 7u]) << 4) |
                                      sample[base + 6u]);
            write_register(driver, (uint8_t)(0x20u + offset), value);
            write_register(driver, (uint8_t)(0x40u + offset),
                           (uint8_t)((sample[base + 5u] << 6) |
                                     sample[base + 4u]));
            write_register(driver, (uint8_t)(0x60u + offset),
                           (uint8_t)((sample[base] << 4) |
                                     sample[base + 1u]));
            write_register(driver, (uint8_t)(0x80u + offset),
                           (uint8_t)((sample[base + 2u] << 4) |
                                     sample[base + 3u]));
            write_register(driver, (uint8_t)(0xe0u + offset),
                           sample[base + 11u]);
        }
    }

    if (chunk[0x29] != 0u)
        set_parameter(driver, index, sample, sample[0x16], chunk[0x29]);
    if (chunk[0x2b] != 0u)
        set_parameter(driver, index, sample, sample[0x17], chunk[0x2b]);
    if (chunk[0x2c] != 0u)
        set_parameter(driver, index, sample, sample[0x18], chunk[0x2c]);
}

void ad15_driver_24(Ad15Driver *driver, int16_t channel, void *voice_pointer,
                    uint16_t value)
{
    uint8_t *voice = (uint8_t *)voice_pointer;
    uint32_t scaled;
    uint32_t exponent = 0;
    uint32_t high;
    uint32_t low;
    uint32_t quotient;
    uint16_t result;
    if (driver == NULL || !driver->initialized || channel <= 0 ||
        channel > 9 || voice == NULL)
        return;

    /* AD15 normalizes the value in DX:AX until DX <= 0185h (with a
       secondary AX bound at equality), divides by 50000, then ORs the scale
       exponent into AH. */
    scaled = (uint32_t)value << 16;
    for (;;) {
        high = scaled >> 16;
        low = scaled & 0xffffu;
        if (high < 0x185u ||
            (high == 0x185u && low <= 0xdcb0u))
            break;
        scaled >>= 1;
        ++exponent;
    }
    quotient = scaled / 50000u;
    result = (uint16_t)quotient;
    result = (uint16_t)(result | (uint16_t)((exponent << 10) & 0xff00u));
    write_u16(voice + 4u, result);
    write_u16(voice + 6u, result);
}

void ad15_driver_27(Ad15Driver *driver, uint16_t channel,
                    void *voice_pointer, void *chunk_pointer,
                    const void *sample_pointer)
{
    uint8_t *voice = (uint8_t *)voice_pointer;
    const uint8_t *chunk = (const uint8_t *)chunk_pointer;
    const uint8_t *sample;
    (void)sample_pointer;
    if (driver == NULL || !driver->initialized || channel == 0 ||
        channel > 9 || voice == NULL || chunk == NULL)
        return;
    sample = voice_sample(voice);
    if (sample == NULL)
        return;
    update_pitch(driver, channel, voice, chunk, sample);
}

void ad15_driver_1e(Ad15Driver *driver, int16_t channel)
{
    unsigned index;
    if (channel == 0) {
        ad15_driver_stop_unknown(driver);
        return;
    }
    if (driver == NULL || !driver->initialized || channel < 1 || channel > 9)
        return;
    index = (unsigned)channel - 1u;
    reset_channel(driver, index);
}

uint16_t ad15_driver_36(const Ad15Driver *driver, int16_t channel)
{
    if (channel != 0)
        return 1u;
    return driver != NULL && driver->sample_timer_active != 0 ? 1u : 0u;
}

void ad15_driver_stop_unknown(Ad15Driver *driver)
{
    if (driver != NULL)
        driver->sample_timer_active = 0;
}
