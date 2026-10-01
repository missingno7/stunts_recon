#include "../../port/port_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "../../src/fardata_11039.c"
#include "../../port/legacy_views.c"
#include "../../port/sprite.c"
#include "../../port/font.c"
#include "../../port/transition_work.h"

#define FONT_CAPACITY 4096u
#define TEXT_CAPACITY 512u

static uint8_t test_video[PORT_VIDEO_MEMORY_BYTES];
static uint8_t custom_font[FONT_CAPACITY];
static size_t custom_font_length;

uint8_t *port_video_pixels(void)
{
    return test_video;
}

void port_guest_unwind(const char *symbol)
{
    fprintf(stderr, "unexpected font harness unwind: %s\n", symbol);
    exit(20);
}

int16_t call_read_line(char *buffer, int16_t length, int16_t x, int16_t y,
                       int16_t timeout_low, int16_t timeout_high)
{
    (void)buffer; (void)length; (void)x; (void)y;
    (void)timeout_low; (void)timeout_high;
    port_guest_unwind("font probe unexpectedly entered the line editor");
    return 0;
}

int port_memory_extent(const void *pointer, size_t *remaining_out)
{
    if (pointer != custom_font)
        return 0;
    if (remaining_out != NULL)
        *remaining_out = custom_font_length;
    return 1;
}

void *mmgr_alloc_pages(const char *name, uint16_t paragraphs)
{
    (void)name;
    return calloc((size_t)paragraphs, 16u);
}

void mmgr_free(void *pointer)
{
    free(pointer);
}

void port_video_publish(const char *reason)
{
    (void)reason;
}

void port_video_transition_begin(PortVideoTransition *transition)
{
    memset(transition, 0, sizeof(*transition));
}

void port_video_transition_advance(PortVideoTransition *transition,
                                   uint32_t work_units)
{
    (void)transition;
    (void)work_units;
}

int port_far_from_host(const void *pointer, PortFarPtr *address_out,
                       size_t *remaining_out)
{
    (void)pointer;
    (void)address_out;
    (void)remaining_out;
    return 0;
}

void *port_far_resolve(PortFarPtr pointer, size_t extent)
{
    (void)pointer;
    (void)extent;
    return NULL;
}

int16_t mulscl(int16_t left, int16_t right)
{
    (void)left;
    (void)right;
    return 0;
}

static int read_exact(void *destination, size_t bytes)
{
    return fread(destination, 1u, bytes, stdin) == bytes;
}

static uint16_t get_u16(const uint8_t *bytes)
{
    return (uint16_t)(bytes[0] | ((uint16_t)bytes[1] << 8));
}

static void seed_frame(void)
{
    size_t i;
    for (i = 0; i < sizeof(test_video); ++i)
        test_video[i] = (uint8_t)(i * 17u + (i >> 8) * 3u + 11u);
}

int main(void)
{
    uint8_t header[26];
    uint8_t text[TEXT_CAPACITY];
    uint8_t *font;
    uint16_t operation;
    uint16_t font_kind;
    uint16_t font_length;
    uint16_t foreground;
    uint16_t background;
    uint16_t x;
    uint16_t y;
    uint16_t clip_left;
    uint16_t clip_right;
    uint16_t clip_top;
    uint16_t clip_bottom;
    uint16_t count;
    uint16_t text_length;
    int16_t result = 0;

#ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    if (!read_exact(header, sizeof(header)))
        return 2;
    operation = get_u16(header);
    font_kind = get_u16(header + 2u);
    font_length = get_u16(header + 4u);
    foreground = get_u16(header + 6u);
    background = get_u16(header + 8u);
    x = get_u16(header + 10u);
    y = get_u16(header + 12u);
    clip_left = get_u16(header + 14u);
    clip_right = get_u16(header + 16u);
    clip_top = get_u16(header + 18u);
    clip_bottom = get_u16(header + 20u);
    count = get_u16(header + 22u);
    text_length = get_u16(header + 24u);
    if (font_length < 534u || font_length > FONT_CAPACITY ||
        text_length == 0u || text_length > TEXT_CAPACITY)
        return 3;

    seed_frame();
    if (font_kind == 0u) {
        if (font_length != sizeof(fontdef_default) ||
            !read_exact(custom_font, font_length))
            return 4;
        if (memcmp(custom_font, fontdef_default, sizeof(fontdef_default)) != 0)
            return 5;
        font = fontdef_default;
    } else if (font_kind == 1u) {
        if (!read_exact(custom_font, font_length))
            return 6;
        custom_font_length = font_length;
        font = custom_font;
    } else {
        return 7;
    }
    if (!read_exact(text, text_length) ||
        !read_exact(test_video, sizeof(test_video)))
        return 8;
    if (text[text_length - 1u] != 0u)
        return 9;

    port_sprite_init();
    sprset1size((int16_t)clip_left, (int16_t)clip_right,
                (int16_t)clip_top, (int16_t)clip_bottom);
    set_fontdefseg(font);
    font_setup_unknown(foreground, background);
    switch (operation) {
    case 0u:
        font_draw_text((const char *)text, (int16_t)x, (int16_t)y);
        break;
    case 1u:
        draw_text_at((const char *)text, (int16_t)x, (int16_t)y);
        break;
    case 2u:
        break;
    case 3u:
        result = font_op((const char *)text, count);
        break;
    case 4u:
        result = font_op2((const char *)text);
        break;
    default:
        return 10;
    }

    if (fwrite(test_video, 1u, sizeof(test_video), stdout) != sizeof(test_video) ||
        fwrite(font, 1u, font_length, stdout) != font_length)
        return 11;
    {
        uint8_t encoded_result[2] = {
            (uint8_t)result, (uint8_t)((uint16_t)result >> 8)
        };
        if (fwrite(encoded_result, 1u, sizeof(encoded_result), stdout) !=
            sizeof(encoded_result))
            return 12;
    }
    return 0;
}
