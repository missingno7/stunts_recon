#include "../../port/sprite.c"

#include <stdio.h>
#include <stdlib.h>

uint8_t port_framebuffer[PORT_VIDEO_MEMORY_BYTES];

static uint8_t *shape_data;
static size_t shape_size;

uint8_t *port_video_pixels(void)
{
    return port_framebuffer;
}

int port_memory_extent(const void *pointer, size_t *remaining_out)
{
    if (pointer != shape_data || shape_data == NULL)
        return 0;
    if (remaining_out != NULL)
        *remaining_out = shape_size;
    return 1;
}

int port_far_from_host(const void *pointer, PortFarPtr *address_out,
                       size_t *remaining_out)
{
    uintptr_t value = (uintptr_t)pointer;
    uintptr_t source = (uintptr_t)shape_data;
    uintptr_t target = (uintptr_t)port_framebuffer;
    if (shape_data != NULL && value >= source &&
        value < source + shape_size) {
        address_out->segment = 0x2000;
        address_out->offset = (uint16_t)(value - source);
    } else if (value >= target &&
               value < target + sizeof(port_framebuffer)) {
        address_out->segment = 0x3000;
        address_out->offset = (uint16_t)(value - target);
    } else {
        return 0;
    }
    address_out->space = PORT_FAR_REAL;
    if (remaining_out != NULL)
        *remaining_out = 0x10000u - address_out->offset;
    return 1;
}

void *port_far_resolve(PortFarPtr pointer, size_t extent)
{
    uint8_t *base;
    size_t available;
    if (pointer.segment == 0x2000) {
        base = shape_data;
        available = shape_size;
    } else if (pointer.segment == 0x3000) {
        base = port_framebuffer;
        available = sizeof(port_framebuffer);
    } else {
        return NULL;
    }
    if ((size_t)pointer.offset > available ||
        extent > available - pointer.offset)
        return NULL;
    return base + pointer.offset;
}

void port_guest_unwind(const char *reason)
{
    fprintf(stderr, "%s\n", reason);
    exit(2);
}

void port_video_publish(const char *reason)
{
    (void)reason;
}

void port_video_transition_begin(PortVideoTransition *transition)
{
    transition->work_units = PORT_TRANSITION_FIXED_WORK;
}

void port_video_transition_advance(PortVideoTransition *transition, uint32_t work)
{
    transition->work_units += work;
}

void *mmgr_alloc_pages(const char *name, uint16_t paragraphs)
{
    (void)name;
    (void)paragraphs;
    return NULL;
}

void mmgr_free(void *pointer)
{
    (void)pointer;
}

int main(int argc, char **argv)
{
    FILE *input;
    FILE *output;
    long length;
    int phase;
    if (argc != 3) {
        fprintf(stderr, "usage: sprite_unk3_harness SHAPE OUTPUT\n");
        return 2;
    }
    input = fopen(argv[1], "rb");
    if (input == NULL || fseek(input, 0, SEEK_END) != 0 ||
        (length = ftell(input)) < 16 || fseek(input, 0, SEEK_SET) != 0)
        return 2;
    shape_size = (size_t)length;
    shape_data = (uint8_t *)malloc(shape_size);
    if (shape_data == NULL ||
        fread(shape_data, 1, shape_size, input) != shape_size) {
        fclose(input);
        return 2;
    }
    fclose(input);
    if ((size_t)((PortShape2D *)shape_data)->width *
        ((PortShape2D *)shape_data)->height > shape_size - 16u)
        return 2;

    memset(port_framebuffer, 0, sizeof(port_framebuffer));
    port_sprite_init();
    for (phase = 0; phase < 4; ++phase)
        sprite_1_unk3((const PortShape2D *)shape_data, (int16_t)phase);

    output = fopen(argv[2], "wb");
    if (output == NULL ||
        fwrite(port_framebuffer, 1, PORT_FRAMEBUFFER_BYTES, output) !=
            PORT_FRAMEBUFFER_BYTES) {
        if (output != NULL)
            fclose(output);
        return 2;
    }
    fclose(output);
    free(shape_data);
    return 0;
}
