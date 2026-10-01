#include "ad15_driver.h"
#include "port_opl3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned register_write_count;

static void write_register(void *userdata, uint8_t reg, uint8_t value)
{
    (void)userdata;
    ++register_write_count;
    port_opl3_write(reg, value);
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

int main(int argc, char **argv)
{
    enum { FRAMES = 8192 };
    Ad15Driver driver;
    uint8_t voice[0x2e] = {0};
    uint8_t chunk[0x4c] = {0};
    int16_t pcm[FRAMES];
    size_t driver_size, sample_size, i;
    uint8_t *driver_image;
    uint8_t *sample;
    unsigned nonzero = 0;
    int positive = 0;
    int negative = 0;

    if (sizeof(void *) != 4u)
        return 77;
    if (argc != 3)
        return 2;
    driver_image = read_file(argv[1], &driver_size);
    sample = read_file(argv[2], &sample_size);
    if (driver_image == NULL || sample == NULL ||
        driver_size < 0x882u || sample_size < 0x64u) {
        fprintf(stderr, "could not read AD15/ELPI fixtures\n");
        free(sample);
        free(driver_image);
        return 2;
    }

    port_opl3_reset(48000u);
    ad15_driver_bind(&driver, driver_image, driver_size, write_register, NULL);
    if (ad15_driver_00(&driver) != 10u) {
        fprintf(stderr, "AD15 did not initialize its OPL backend\n");
        free(sample);
        free(driver_image);
        return 1;
    }
    voice[1] = 1u;
    {
        uint32_t sample_address = (uint32_t)(uintptr_t)sample;
        memcpy(voice + 0x10u, &sample_address, sizeof(sample_address));
    }
    chunk[0x28] = 0x7fu;
    ad15_driver_21(&driver, 1, voice, chunk, sample);
    ad15_driver_09(&driver, 1, voice, chunk, 60, 96, sample);
    if (register_write_count < 20u ||
        port_opl3_render_s16(pcm, FRAMES, 48000u) != FRAMES) {
        fprintf(stderr, "AD15 did not send a complete OPL note to the renderer\n");
        free(sample);
        free(driver_image);
        return 1;
    }
    for (i = 0; i < FRAMES; ++i) {
        if (pcm[i] != 0)
            ++nonzero;
        if (pcm[i] > 0)
            positive = 1;
        if (pcm[i] < 0)
            negative = 1;
    }
    free(sample);
    free(driver_image);
    if (nonzero == 0u || !positive || !negative) {
        fprintf(stderr,
                "shipped ELPI note was silent or not bipolar (%u nonzero frames)\n",
                nonzero);
        return 1;
    }
    printf("AD15 ELPI note rendered %u nonzero bipolar PCM frames from %u register writes\n",
           nonzero, register_write_count);
    return 0;
}
