#include "ad15_driver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Write {
    uint8_t reg;
    uint8_t value;
} Write;

typedef struct Capture {
    Write writes[1024];
    size_t count;
} Capture;

static void capture_write(void *userdata, uint8_t reg, uint8_t value)
{
    Capture *capture = (Capture *)userdata;
    if (capture->count < sizeof(capture->writes) / sizeof(capture->writes[0])) {
        capture->writes[capture->count].reg = reg;
        capture->writes[capture->count].value = value;
        ++capture->count;
    }
}

static uint8_t *read_file(const char *path, size_t *size_out)
{
    FILE *file = fopen(path, "rb");
    long size;
    uint8_t *bytes;
    if (file == NULL || fseek(file, 0, SEEK_END) != 0 ||
        (size = ftell(file)) < 0 || fseek(file, 0, SEEK_SET) != 0) {
        if (file != NULL)
            fclose(file);
        return NULL;
    }
    bytes = (uint8_t *)malloc((size_t)size);
    if (bytes == NULL || fread(bytes, 1, (size_t)size, file) != (size_t)size) {
        free(bytes);
        fclose(file);
        return NULL;
    }
    fclose(file);
    *size_out = (size_t)size;
    return bytes;
}

static int expect_writes(const char *name, const Capture *capture,
                         const Write *expected, size_t count)
{
    size_t i;
    if (capture->count != count) {
        fprintf(stderr, "%s: got %lu writes, expected %lu\n", name,
                (unsigned long)capture->count, (unsigned long)count);
        return 0;
    }
    for (i = 0; i < count; ++i) {
        if (capture->writes[i].reg != expected[i].reg ||
            capture->writes[i].value != expected[i].value) {
            fprintf(stderr,
                    "%s write %lu: got %02X=%02X expected %02X=%02X\n",
                    name, (unsigned long)i, capture->writes[i].reg,
                    capture->writes[i].value, expected[i].reg,
                    expected[i].value);
            return 0;
        }
    }
    return 1;
}

static void clear_capture(Capture *capture)
{
    capture->count = 0;
}

static void put_sample_pointer(uint8_t *voice, const uint8_t *sample)
{
    uint32_t address = (uint32_t)(uintptr_t)sample;
    memcpy(voice + 0x10u, &address, sizeof(address));
}

static int expect_parameter_selector(Ad15Driver *driver, Capture *capture,
                                     const uint8_t *image,
                                     size_t image_size,
                                     const uint8_t *source_sample,
                                     uint8_t selector_offset,
                                     uint8_t selector,
                                     uint8_t chunk_offset,
                                     uint8_t register_number,
                                     uint8_t register_value)
{
    uint8_t sample[0x64];
    uint8_t voice[0x2e] = {0};
    uint8_t chunk[0x4c] = {0};
    const Write expected[] = {{register_number, register_value}};

    memcpy(sample, source_sample, sizeof(sample));
    ad15_driver_bind(driver, image, image_size, capture_write, capture);
    clear_capture(capture);
    if (ad15_driver_00(driver) != 10u)
        return 0;
    ad15_driver_21(driver, 1, voice, chunk, sample);
    sample[selector_offset] = selector;
    chunk[chunk_offset] = 0x38u;
    clear_capture(capture);
    ad15_driver_21(driver, 1, voice, chunk, sample);
    return expect_writes("+21 parameter selector", capture, expected,
                         sizeof(expected) / sizeof(expected[0]));
}

int main(int argc, char **argv)
{
    static const Write expected_elpi_bind[] = {
        {0xc0, 0x01}, {0x20, 0x62}, {0x40, 0x00}, {0x60, 0xb9},
        {0x80, 0x53}, {0x23, 0x64}, {0x43, 0x00}, {0x63, 0xf6},
        {0x83, 0x64}, {0xe3, 0x01},
    };
    static const Write expected_snar_bind[] = {
        {0xc8, 0x0e}, {0x32, 0x0f}, {0x52, 0x00}, {0x72, 0xf0},
        {0x92, 0x00}, {0x55, 0x00}, {0x75, 0xf7}, {0x95, 0x27},
    };
    static const Write expected_note[] = {{0xa0, 0x56}, {0xb0, 0x34}};
    static const Write expected_tenth_slot_note[] = {
        {0xa8, 0x56}, {0xb8, 0x30}, {0x52, 0x00},
    };
    static const Write expected_volume[] = {{0x40, 0x2c}, {0x43, 0x2c}};
    static const Write expected_fm_volume[] = {{0x55, 0x25}};
    static const Write expected_reset[] = {
        {0x80, 0x0a}, {0x83, 0x0a}, {0xb0, 0x01},
    };
    static const Write expected_slot_stop[] = {{0x40, 0x3f}, {0x43, 0x3f}};
    static const Write expected_parameter_volume[] = {
        {0x40, 0x2d}, {0x43, 0x2d},
    };
    static const Write expected_positive_bend[] = {{0xa0, 0x60}};
    static const Write expected_negative_bend[] = {{0xa0, 0x4c}};
    static const struct {
        uint8_t selector_offset;
        uint8_t selector;
        uint8_t chunk_offset;
        uint8_t reg;
        uint8_t value;
    } parameter_selectors[] = {
        {0x16u, 0x81u, 0x29u, 0x20u, 0x67u},
        {0x17u, 0x82u, 0x2bu, 0x23u, 0x67u},
        {0x18u, 0x83u, 0x2cu, 0x40u, 0x23u},
        {0x16u, 0x84u, 0x29u, 0x43u, 0x23u},
        {0x17u, 0x85u, 0x2bu, 0xc0u, 0x07u},
    };
    Ad15Driver driver;
    Capture capture = {{{0, 0}}, 0};
    size_t driver_size, elpi_size, snar_size, star_size;
    uint8_t *image;
    uint8_t *elpi;
    uint8_t *snar;
    uint8_t *star;
    uint8_t voice[0x2e];
    uint8_t chunk[0x4c];
    size_t selector_index;

    if (sizeof(void *) != 4u)
        return 77;
    if (argc != 5)
        return 2;
    image = read_file(argv[1], &driver_size);
    elpi = read_file(argv[2], &elpi_size);
    snar = read_file(argv[3], &snar_size);
    star = read_file(argv[4], &star_size);
    if (image == NULL || elpi == NULL || snar == NULL || star == NULL ||
        driver_size < 0x882u || elpi_size < 0x64u || snar_size < 0x64u ||
        star_size < 0x64u) {
        fprintf(stderr, "could not read AD15 oracle fixtures\n");
        return 2;
    }

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    if (ad15_driver_00(&driver) != 10u) {
        fprintf(stderr, "AD15 +00 did not report ten logical slots\n");
        return 1;
    }
    if (capture.count != 307u || capture.writes[0].reg != 4u ||
        capture.writes[0].value != 0x60u ||
        capture.writes[5].reg != 4u || capture.writes[5].value != 0x80u) {
        fprintf(stderr, "AD15 +00 register initialization differs from oracle\n");
        return 1;
    }

    clear_capture(&capture);
    memset(chunk, 0, sizeof(chunk));
    ad15_driver_21(&driver, 1, voice, chunk, elpi);
    if (!expect_writes("+21 ELPI", &capture, expected_elpi_bind,
                       sizeof(expected_elpi_bind) / sizeof(expected_elpi_bind[0])))
        return 1;

    clear_capture(&capture);
    ad15_driver_21(&driver, 9, voice, chunk, snar);
    if (!expect_writes("+21 SNAR", &capture, expected_snar_bind,
                       sizeof(expected_snar_bind) / sizeof(expected_snar_bind[0])))
        return 1;

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    ad15_driver_00(&driver);
    clear_capture(&capture);
    memset(voice, 0, sizeof(voice));
    memset(chunk, 0, sizeof(chunk));
    ad15_driver_09(&driver, 1, voice, chunk, 60, 96, elpi);
    if (!expect_writes("+09 ELPI", &capture, expected_note,
                       sizeof(expected_note) / sizeof(expected_note[0])) ||
        voice[3] != 60u || voice[4] != 0x56u || voice[5] != 0x14u ||
        voice[6] != 0x56u || voice[7] != 0x14u)
        return 1;

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    ad15_driver_00(&driver);
    clear_capture(&capture);
    memset(voice, 0, sizeof(voice));
    memset(chunk, 0, sizeof(chunk));
    ad15_driver_09(&driver, 9, voice, chunk, 48, 96, snar);
    if (!expect_writes("+09 tenth logical slot", &capture,
                       expected_tenth_slot_note,
                       sizeof(expected_tenth_slot_note) /
                           sizeof(expected_tenth_slot_note[0])))
        return 1;

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    ad15_driver_00(&driver);
    clear_capture(&capture);
    memset(voice, 0, sizeof(voice));
    memset(chunk, 0, sizeof(chunk));
    put_sample_pointer(voice, elpi);
    voice[1] = 1;
    ad15_driver_09(&driver, 1, voice, chunk, 60, 96, elpi);
    clear_capture(&capture);
    ad15_driver_12(&driver, 1, voice, 0x1234);
    if (!expect_writes("+12 ELPI volume", &capture, expected_volume,
                       sizeof(expected_volume) / sizeof(expected_volume[0])))
        return 1;

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    ad15_driver_00(&driver);
    clear_capture(&capture);
    memset(voice, 0, sizeof(voice));
    memset(chunk, 0, sizeof(chunk));
    put_sample_pointer(voice, snar);
    voice[1] = 1;
    ad15_driver_21(&driver, 9, voice, chunk, snar);
    ad15_driver_09(&driver, 9, voice, chunk, 48, 96, snar);
    clear_capture(&capture);
    ad15_driver_12(&driver, 9, voice, 0x1234);
    if (!expect_writes("+12 SNAR FM volume", &capture,
                       expected_fm_volume,
                       sizeof(expected_fm_volume) /
                           sizeof(expected_fm_volume[0])))
        return 1;

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    ad15_driver_00(&driver);
    clear_capture(&capture);
    memset(voice, 0, sizeof(voice));
    memset(chunk, 0, sizeof(chunk));
    ad15_driver_21(&driver, 1, voice, chunk, elpi);
    ad15_driver_09(&driver, 1, voice, chunk, 60, 96, elpi);
    clear_capture(&capture);
    ad15_driver_06(&driver);
    if (!expect_writes("+06 reset", &capture, expected_reset,
                       sizeof(expected_reset) / sizeof(expected_reset[0])))
        return 1;

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    ad15_driver_00(&driver);
    clear_capture(&capture);
    memset(voice, 0, sizeof(voice));
    memset(chunk, 0, sizeof(chunk));
    ad15_driver_21(&driver, 1, voice, chunk, elpi);
    ad15_driver_09(&driver, 1, voice, chunk, 60, 96, elpi);
    clear_capture(&capture);
    ad15_driver_1e(&driver, 1);
    if (!expect_writes("+1E stop logical slot 1", &capture, expected_reset,
                       sizeof(expected_reset) / sizeof(expected_reset[0])))
        return 1;

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    ad15_driver_00(&driver);
    clear_capture(&capture);
    memset(voice, 0, sizeof(voice));
    memset(chunk, 0, sizeof(chunk));
    put_sample_pointer(voice, elpi);
    voice[1] = 1;
    ad15_driver_21(&driver, 1, voice, chunk, elpi);
    ad15_driver_09(&driver, 1, voice, chunk, 60, 96, elpi);
    clear_capture(&capture);
    ad15_driver_15(&driver, 1, voice, 7, 48);
    if (!expect_writes("+15 type 7", &capture, expected_parameter_volume,
                       sizeof(expected_parameter_volume) /
                           sizeof(expected_parameter_volume[0])))
        return 1;
    clear_capture(&capture);
    ad15_driver_0f(&driver, 1, voice);
    if (!expect_writes("+0F slot stop", &capture, expected_slot_stop,
                       sizeof(expected_slot_stop) /
                           sizeof(expected_slot_stop[0])))
        return 1;

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    ad15_driver_00(&driver);
    clear_capture(&capture);
    memset(voice, 0, sizeof(voice));
    memset(chunk, 0, sizeof(chunk));
    put_sample_pointer(voice, star);
    voice[1] = 1;
    voice[3] = 60;
    voice[4] = 0x56;
    voice[5] = 0x14;
    ad15_driver_21(&driver, 1, voice, chunk, star);
    ad15_driver_09(&driver, 1, voice, chunk, 60, 96, star);
    clear_capture(&capture);
    chunk[0x26] = 0x00;
    chunk[0x27] = 0x20;
    ad15_driver_27(&driver, 1, voice, chunk, star);
    if (!expect_writes("+27 STAR positive bend", &capture,
                       expected_positive_bend,
                       sizeof(expected_positive_bend) /
                           sizeof(expected_positive_bend[0])) ||
        voice[6] != 0x60u || voice[7] != 0x14u)
        return 1;
    clear_capture(&capture);
    chunk[0x26] = 0x00;
    chunk[0x27] = 0xe0;
    ad15_driver_27(&driver, 1, voice, chunk, star);
    if (!expect_writes("+27 STAR negative bend", &capture,
                       expected_negative_bend,
                       sizeof(expected_negative_bend) /
                           sizeof(expected_negative_bend[0])) ||
        voice[6] != 0x4cu || voice[7] != 0x14u)
        return 1;

    ad15_driver_bind(&driver, image, driver_size, capture_write, &capture);
    ad15_driver_00(&driver);
    clear_capture(&capture);
    memset(voice, 0, sizeof(voice));
    ad15_driver_24(&driver, 1, voice, 0x1234u);
    if (capture.count != 0u || voice[4] != 0x7du || voice[5] != 0x11u ||
        voice[6] != 0x7du || voice[7] != 0x11u) {
        fprintf(stderr, "+24 scalar conversion differs from oracle\n");
        return 1;
    }

    for (selector_index = 0;
         selector_index < sizeof(parameter_selectors) /
                              sizeof(parameter_selectors[0]);
         ++selector_index) {
        const size_t record_size = elpi_size < 0x64u ? elpi_size : 0x64u;
        if (record_size != 0x64u ||
            !expect_parameter_selector(
                &driver, &capture, image, driver_size, elpi,
                parameter_selectors[selector_index].selector_offset,
                parameter_selectors[selector_index].selector,
                parameter_selectors[selector_index].chunk_offset,
                parameter_selectors[selector_index].reg,
                parameter_selectors[selector_index].value)) {
            fprintf(stderr, "+21 selector case %lu differs from oracle\n",
                    (unsigned long)selector_index);
            return 1;
        }
    }

    free(star);
    free(snar);
    free(elpi);
    free(image);
    puts("AD15 register and voice-state traces match locked Unicorn cases");
    return 0;
}
