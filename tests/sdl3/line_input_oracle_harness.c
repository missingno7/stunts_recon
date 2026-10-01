/* Host driver for the separately compiled fresh obj_seg032_group overlay. */
#include "../../port/port_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/fardata_11039.c"
#include "../../port/legacy_views.c"
#include "../../port/sprite.c"
#include "../../port/font.c"
#include "../../port/transition_work.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#define VIDEO_BYTES 65536u
#define FONT_CAPACITY 4096u
#define TEXT_CAPACITY 512u
#define CUSTOM_FONTS 3u

extern unsigned char fontdef_default[1408];
extern void set_fontdefseg(const void *font);
extern void font_setup_unknown(unsigned short foreground,
                               unsigned short background);
extern void port_sprite_init(void);
extern unsigned char *port_video_pixels(void);
extern void read_line_helper(void);
extern void read_line_helper2(void);
extern void port_line_input_set_state(short x, short y, short cursor_height,
                                      short max_width, short cursor_slot,
                                      unsigned char *text);
extern unsigned short port_line_input_get_slot(void);

static unsigned char test_video[VIDEO_BYTES];
static unsigned char custom_fonts[CUSTOM_FONTS][FONT_CAPACITY];
static size_t custom_font_lengths[CUSTOM_FONTS];
static unsigned char line_text[TEXT_CAPACITY];

/* The editor's implementation state is file-static in the included source. */
uint8_t *port_video_pixels(void) { return test_video; }

void port_guest_unwind(const char *symbol)
{
    fprintf(stderr, "unexpected line-input harness unwind: %s\n", symbol);
    exit(20);
}

void port_video_publish(const char *reason)
{
    (void)reason;
}

int port_memory_extent(const void *pointer, size_t *remaining_out)
{
    unsigned index;
    for (index = 0; index < CUSTOM_FONTS; ++index) {
        if (pointer == custom_fonts[index]) {
            if (remaining_out != NULL)
                *remaining_out = custom_font_lengths[index];
            return 1;
        }
    }
    return 0;
}

int16_t call_read_line(char *buffer, int16_t max_length, int16_t x,
                       int16_t y, int16_t timeout_low,
                       int16_t timeout_high)
{
    (void)buffer; (void)max_length; (void)x; (void)y;
    (void)timeout_low; (void)timeout_high;
    port_guest_unwind("line-input probe unexpectedly entered read_line");
    return 0;
}

void port_timer_copy_counter_words(unsigned short ticks_low,
                                   unsigned short ticks_high)
{
    (void)ticks_low; (void)ticks_high;
}

void *mmgr_alloc_pages(const char *name, uint16_t paragraphs)
{
    (void)name;
    return calloc((size_t)paragraphs, 16u);
}

void mmgr_free(void *pointer) { free(pointer); }

int16_t mulscl(int16_t left, int16_t right)
{
    (void)left; (void)right;
    return 0;
}

int port_far_from_host(const void *pointer, PortFarPtr *address_out,
                       size_t *remaining_out)
{
    (void)pointer; (void)address_out; (void)remaining_out;
    return 0;
}

void *port_far_resolve(PortFarPtr pointer, size_t extent)
{
    (void)pointer; (void)extent;
    return NULL;
}

void port_video_transition_begin(PortVideoTransition *transition)
{
    memset(transition, 0, sizeof(*transition));
}

void port_video_transition_advance(PortVideoTransition *transition,
                                   uint32_t work_units)
{
    (void)transition; (void)work_units;
}

void set_add_value(int32_t ticks) { (void)ticks; }

int16_t kb_call_readchar_callback(void) { return 0; }
int16_t poll_input_abort(void) { return 0; }
void timer_copy_counter(int16_t offset, int16_t segment)
{
    (void)offset; (void)segment;
}
int16_t timer_compare_dx(void) { return 0; }

static int read_exact(void *destination, size_t bytes)
{
    return fread(destination, 1u, bytes, stdin) == bytes;
}

static unsigned short get_u16(const unsigned char *bytes)
{
    return (unsigned short)(bytes[0] | ((unsigned short)bytes[1] << 8));
}

static void seed_frame(void)
{
    size_t index;
    for (index = 0; index < sizeof(test_video); ++index)
        test_video[index] = (unsigned char)(index * 17u +
                                            (index >> 8) * 3u + 11u);
}

static unsigned char *font_for_id(unsigned short font_id)
{
    if (font_id == 0u)
        return fontdef_default;
    if (font_id <= CUSTOM_FONTS)
        return custom_fonts[font_id - 1u];
    return NULL;
}

static size_t font_length_for_id(unsigned short font_id)
{
    if (font_id == 0u)
        return sizeof(fontdef_default);
    if (font_id <= CUSTOM_FONTS)
        return custom_font_lengths[font_id - 1u];
    return 0u;
}

int main(void)
{
    unsigned char magic[4];
    unsigned char encoded_step[24];
    unsigned char text_input[TEXT_CAPACITY];
    unsigned short font_id;
    unsigned short step_count;
    unsigned short step_index;
    unsigned index;

#ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    if (!read_exact(magic, sizeof(magic)) || memcmp(magic, "LIO1", 4u) != 0)
        return 2;
    for (index = 0; index < CUSTOM_FONTS; ++index) {
        unsigned char length_bytes[2];
        if (!read_exact(length_bytes, sizeof(length_bytes)))
            return 3;
        custom_font_lengths[index] = get_u16(length_bytes);
        if (custom_font_lengths[index] < 534u ||
            custom_font_lengths[index] > FONT_CAPACITY ||
            !read_exact(custom_fonts[index], custom_font_lengths[index]))
            return 4;
    }
    {
        unsigned char count_bytes[2];
        if (!read_exact(count_bytes, sizeof(count_bytes)))
            return 5;
        step_count = get_u16(count_bytes);
    }
    if (step_count == 0u || step_count > 32u)
        return 6;

    port_sprite_init();
    seed_frame();
    for (step_index = 0; step_index < step_count; ++step_index) {
        unsigned short select_font;
        unsigned short action;
        unsigned short foreground;
        unsigned short background;
        unsigned short line_height;
        unsigned short x;
        unsigned short y;
        unsigned short cursor_height;
        unsigned short max_width;
        unsigned short cursor_slot;
        unsigned short text_length;
        unsigned char *font;
        size_t font_length;

        if (!read_exact(encoded_step, sizeof(encoded_step)))
            return 7;
        font_id = get_u16(encoded_step + 0u);
        select_font = get_u16(encoded_step + 2u);
        action = get_u16(encoded_step + 4u);
        foreground = get_u16(encoded_step + 6u);
        background = get_u16(encoded_step + 8u);
        line_height = get_u16(encoded_step + 10u);
        x = get_u16(encoded_step + 12u);
        y = get_u16(encoded_step + 14u);
        cursor_height = get_u16(encoded_step + 16u);
        max_width = get_u16(encoded_step + 18u);
        cursor_slot = get_u16(encoded_step + 20u);
        text_length = get_u16(encoded_step + 22u);
        font = font_for_id(font_id);
        font_length = font_length_for_id(font_id);
        if (font == NULL || font_length < 534u || text_length == 0u ||
            text_length > TEXT_CAPACITY || action > 2u ||
            !read_exact(text_input, text_length) ||
            text_input[text_length - 1u] != 0u)
            return 8;

        if (select_font != 0u)
            set_fontdefseg(font);
        font_setup_unknown(foreground, background);
        if (line_height != 0xFFFFu) {
            font[18] = (unsigned char)line_height;
            font[19] = (unsigned char)(line_height >> 8);
        }
        memset(line_text, 0, sizeof(line_text));
        memcpy(line_text, text_input, text_length);
        port_line_input_set_state((short)x, (short)y, (short)cursor_height,
                                  (short)max_width, (short)cursor_slot,
                                  line_text);
        if (action == 0u) {
            read_line_helper();
        } else if (action == 1u) {
            read_line_helper2();
        } else {
            read_line_helper();
            read_line_helper();
        }

        if (fwrite(test_video, 1u, sizeof(test_video), stdout) !=
                sizeof(test_video) ||
            fwrite(font, 1u, font_length, stdout) != font_length ||
            fwrite(line_text, 1u, sizeof(line_text), stdout) !=
                sizeof(line_text))
            return 9;
        {
            unsigned char slot_bytes[2] = {
                (unsigned char)port_line_input_get_slot(),
                (unsigned char)(port_line_input_get_slot() >> 8)
            };
            if (fwrite(slot_bytes, 1u, sizeof(slot_bytes), stdout) !=
                sizeof(slot_bytes))
                return 10;
        }
    }
    return 0;
}
