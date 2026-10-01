#include "port_runtime.h"
#include "transition_work.h"

#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/*
 * Faithful C translation of the DOS sprite-window descriptor and row-table
 * setup in asm/sprite_make_wnd.ASM and asm/patterned_lines_windows.ASM. The
 * 16-bit row-table offsets become host pointers in the PORT_BUILD overlay;
 * pixel storage retains the original 16-byte SHAPE2D header and row-major
 * indexed bytes.
 */
#define PORT_MAX_WINDOWS 32u

typedef struct PortWindow {
    PortSprite sprite;
    uint16_t *line_offsets;
    uint8_t live;
} PortWindow;

static PortWindow s_windows[PORT_MAX_WINDOWS];
static PortShape2D s_screen_shape;
static uint16_t s_screen_lines[PORT_SCREEN_HEIGHT];
static PortSprite s_sprite1;
static uint8_t s_sprite_ready;
static uint16_t s_line_pattern_bits;
static uint8_t s_line_auxiliary_color;

/* Exact byte profiles from graphics_resource_runtime.ASM's
   sphere_scanline_profile_00..38; profile N is stored as N+1 bytes. */
static const uint8_t s_sphere_scanline_profiles[] = {
    0x01, 0x02, 0x03, 0x03, 0x04, 0x04, 0x03, 0x04, 0x05, 0x05, 0x04, 0x05, 0x06, 0x06, 0x06, 0x04, 0x06, 0x06, 0x07, 0x07,
    0x08, 0x05, 0x06, 0x07, 0x08, 0x08, 0x09, 0x09, 0x05, 0x07, 0x08, 0x09, 0x09, 0x0a, 0x0a, 0x0a, 0x05, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0b, 0x0b, 0x0b, 0x05, 0x08, 0x09, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0d, 0x06, 0x08, 0x09, 0x0b, 0x0c,
    0x0c, 0x0d, 0x0d, 0x0e, 0x0e, 0x0e, 0x06, 0x08, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x0f, 0x0f, 0x06, 0x09,
    0x0a, 0x0c, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x10, 0x10, 0x10, 0x10, 0x06, 0x09, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x10,
    0x11, 0x11, 0x11, 0x11, 0x12, 0x07, 0x09, 0x0b, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13,
    0x07, 0x0a, 0x0c, 0x0d, 0x0f, 0x10, 0x11, 0x11, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x14, 0x07, 0x0a, 0x0c, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x13, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x15, 0x07, 0x0a, 0x0c, 0x0e, 0x10, 0x11, 0x12,
    0x13, 0x13, 0x14, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x16, 0x17, 0x08, 0x0b, 0x0d, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17, 0x18, 0x18, 0x18, 0x08, 0x0b, 0x0d, 0x0f, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x16, 0x17, 0x17, 0x18, 0x18, 0x18, 0x19, 0x19, 0x19, 0x19, 0x08, 0x0b, 0x0e, 0x0f, 0x11, 0x12, 0x14, 0x15, 0x16, 0x16,
    0x17, 0x18, 0x18, 0x19, 0x19, 0x19, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x08, 0x0b, 0x0e, 0x10, 0x11, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x18, 0x19, 0x1a, 0x1a, 0x1a, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1c, 0x08, 0x0c, 0x0e, 0x10, 0x12, 0x13, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x19, 0x1a, 0x1a, 0x1b, 0x1b, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1d, 0x09, 0x0c, 0x0f, 0x11,
    0x12, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1b, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
    0x09, 0x0c, 0x0f, 0x11, 0x13, 0x14, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1b, 0x1c, 0x1d, 0x1d, 0x1e, 0x1e, 0x1e, 0x1f,
    0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x09, 0x0d, 0x0f, 0x11, 0x13, 0x15, 0x16, 0x17, 0x19, 0x1a, 0x1b, 0x1b, 0x1c, 0x1d, 0x1d,
    0x1e, 0x1e, 0x1f, 0x1f, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x21, 0x09, 0x0d, 0x0f, 0x12, 0x14, 0x15, 0x17, 0x18, 0x19,
    0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1e, 0x1f, 0x1f, 0x20, 0x20, 0x21, 0x21, 0x21, 0x21, 0x22, 0x22, 0x22, 0x22, 0x09, 0x0d,
    0x10, 0x12, 0x14, 0x16, 0x17, 0x18, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1e, 0x1f, 0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x22,
    0x22, 0x23, 0x23, 0x23, 0x23, 0x23, 0x09, 0x0d, 0x10, 0x12, 0x14, 0x16, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x20, 0x21, 0x22, 0x22, 0x22, 0x23, 0x23, 0x23, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x0a, 0x0d, 0x10, 0x13, 0x15,
    0x17, 0x18, 0x19, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x20, 0x21, 0x22, 0x22, 0x23, 0x23, 0x24, 0x24, 0x24, 0x25, 0x25,
    0x25, 0x25, 0x25, 0x25, 0x26, 0x0a, 0x0e, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1a, 0x1b, 0x1d, 0x1e, 0x1f, 0x20, 0x20, 0x21,
    0x22, 0x23, 0x23, 0x24, 0x24, 0x25, 0x25, 0x25, 0x26, 0x26, 0x26, 0x26, 0x27, 0x27, 0x27, 0x27, 0x0a, 0x0e, 0x11, 0x13,
    0x15, 0x17, 0x19, 0x1a, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 0x25, 0x26, 0x26, 0x26, 0x27,
    0x27, 0x27, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x0a, 0x0e, 0x11, 0x14, 0x16, 0x18, 0x19, 0x1b, 0x1c, 0x1e, 0x1f, 0x20,
    0x21, 0x22, 0x23, 0x23, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28, 0x28, 0x28, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29,
    0x29, 0x0a, 0x0e, 0x11, 0x14, 0x16, 0x18, 0x1a, 0x1b, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x26,
    0x27, 0x27, 0x28, 0x28, 0x29, 0x29, 0x29, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2b, 0x0a, 0x0f, 0x12, 0x14, 0x17,
    0x18, 0x1a, 0x1c, 0x1d, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x26, 0x27, 0x28, 0x28, 0x29, 0x29, 0x2a, 0x2a,
    0x2a, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2c, 0x2c, 0x2c, 0x2c, 0x0b, 0x0f, 0x12, 0x15, 0x17, 0x19, 0x1b, 0x1c, 0x1e, 0x1f,
    0x20, 0x22, 0x23, 0x24, 0x25, 0x25, 0x26, 0x27, 0x28, 0x28, 0x29, 0x29, 0x2a, 0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2c, 0x2c,
    0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x0b, 0x0f, 0x12, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1e, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x25, 0x26, 0x27, 0x28, 0x28, 0x29, 0x2a, 0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2d, 0x2d, 0x2d, 0x2d, 0x2e, 0x2e, 0x2e, 0x2e,
    0x2e, 0x2e, 0x2e, 0x0b, 0x0f, 0x12, 0x15, 0x18, 0x1a, 0x1b, 0x1d, 0x1f, 0x20, 0x21, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x28, 0x29, 0x2a, 0x2a, 0x2b, 0x2c, 0x2c, 0x2d, 0x2d, 0x2d, 0x2e, 0x2e, 0x2e, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f,
    0x30, 0x0b, 0x0f, 0x13, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x1f, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a,
    0x2b, 0x2b, 0x2c, 0x2c, 0x2d, 0x2e, 0x2e, 0x2e, 0x2f, 0x2f, 0x2f, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31,
};

/* obj_seg008.c's PORT_BUILD descriptor widens the DOS lineofs word to a host
   pointer, preserving the backing shape and the remaining nine clip words. */
PortSprite sprite2;

static void set_sprite_bounds(PortSprite *sprite, uint16_t width,
                              uint16_t height)
{
    sprite->words[0] = 0;
    sprite->words[1] = 0;
    sprite->words[2] = 0;
    sprite->words2[0] = 0;
    sprite->words2[1] = width;
    sprite->words2[2] = 0;
    sprite->words2[3] = height;
    sprite->words2[4] = width;
    sprite->words2[5] = 0;
    sprite->words2[6] = width;
    sprite->words2[7] = 0;
    sprite->words2[8] = width;
}

void port_sprite_init(void)
{
    unsigned y;
    if (s_sprite_ready)
        return;
    memset(&s_screen_shape, 0, sizeof(s_screen_shape));
    s_screen_shape.width = PORT_SCREEN_WIDTH;
    s_screen_shape.height = PORT_SCREEN_HEIGHT;
    for (y = 0; y < PORT_SCREEN_HEIGHT; ++y)
        s_screen_lines[y] = (uint16_t)(y * PORT_SCREEN_WIDTH);
    memset(&sprite2, 0, sizeof(sprite2));
    sprite2.sprite_bitmapptr = &s_screen_shape;
    sprite2.lineofs = s_screen_lines;
    set_sprite_bounds(&sprite2, PORT_SCREEN_WIDTH, PORT_SCREEN_HEIGHT);
    s_sprite1 = sprite2;
    s_sprite_ready = 1;
}

static PortWindow *find_window(const PortSprite *sprite)
{
    unsigned i;
    if (sprite == NULL)
        return NULL;
    for (i = 0; i < PORT_MAX_WINDOWS; ++i)
        if (s_windows[i].live &&
            (&s_windows[i].sprite == sprite ||
             (s_windows[i].sprite.sprite_bitmapptr == sprite->sprite_bitmapptr &&
              s_windows[i].sprite.lineofs == sprite->lineofs)))
            return &s_windows[i];
    return NULL;
}

static uint8_t *sprite_pixels(const PortSprite *sprite, size_t *extent_out)
{
    PortWindow *window;
    size_t extent;
    if (sprite == NULL || sprite->sprite_bitmapptr == NULL)
        return NULL;
    if (sprite->sprite_bitmapptr == &s_screen_shape) {
        if (extent_out != NULL)
            *extent_out = PORT_VIDEO_MEMORY_BYTES;
        return port_video_pixels();
    }
    window = find_window(sprite);
    if (window == NULL || !port_memory_extent(sprite->sprite_bitmapptr,
                                               &extent) || extent < 16u)
        return NULL;
    if (extent_out != NULL)
        *extent_out = extent;
    return (uint8_t *)sprite->sprite_bitmapptr;
}

/* Give resource adapters the same active bitmap, bound, and row-table view
   that the renderer uses without exposing the file-local descriptor. */
int port_sprite_active_view(uint8_t **pixels_out, size_t *extent_out,
                            PortSprite **sprite_out)
{
    size_t extent;
    uint8_t *pixels;

    port_sprite_init();
    pixels = sprite_pixels(&s_sprite1, &extent);
    if (pixels == NULL)
        return 0;
    if (pixels_out != NULL)
        *pixels_out = pixels;
    if (extent_out != NULL)
        *extent_out = extent;
    if (sprite_out != NULL)
        *sprite_out = &s_sprite1;
    return 1;
}

static uint8_t *shape_pixels(const PortShape2D *shape, size_t *extent_out)
{
    size_t extent;
    if (shape == NULL || !port_memory_extent(shape, &extent) || extent < 16u)
        return NULL;
    if (extent_out != NULL)
        *extent_out = extent - 16u;
    return (uint8_t *)shape + 16u;
}

PortSprite *sprite_make_window(uint16_t width, uint16_t height, uint16_t color)
{
    PortWindow *window = NULL;
    PortShape2D *shape;
    uint32_t pixels;
    uint32_t paragraphs;
    unsigned i;
    (void)color; /* The accepted DOS routine accepts but does not consume it. */
    if (width == 0 || height == 0 || (uint32_t)width * height > 0xFFF0u)
        port_guest_unwind("invalid MCGA window dimensions");
    port_sprite_init();
    for (i = 0; i < PORT_MAX_WINDOWS; ++i) {
        if (!s_windows[i].live) {
            window = &s_windows[i];
            break;
        }
    }
    if (window == NULL)
        port_guest_unwind("MCGA window descriptor table exhausted");

    pixels = (uint32_t)width * height;
    /* asm/sprite_make_wnd.ASM: ((width*height + 16) >> 4) + 1 paragraphs. */
    paragraphs = ((pixels + 16u) >> 4) + 1u;
    shape = (PortShape2D *)mmgr_alloc_pages("MCGA WINDOW",
                                             (uint16_t)paragraphs);
    if (shape == NULL)
        port_guest_unwind("MCGA window allocation failed");
    memset(shape, 0, 16u + pixels);
    shape->width = width;
    shape->height = height;
    window->line_offsets = (uint16_t *)malloc((size_t)height * sizeof(uint16_t));
    if (window->line_offsets == NULL) {
        mmgr_free(shape);
        port_guest_unwind("MCGA window row table allocation failed");
    }
    for (i = 0; i < height; ++i)
        window->line_offsets[i] = (uint16_t)(16u + (uint32_t)i * width);
    memset(&window->sprite, 0, sizeof(window->sprite));
    window->sprite.sprite_bitmapptr = shape;
    window->sprite.lineofs = window->line_offsets;
    set_sprite_bounds(&window->sprite, width, height);
    window->live = 1;
    return &window->sprite;
}

void *sprite_make_wnd(uint16_t width, uint16_t height, uint16_t color)
{
    return sprite_make_window(width, height, color);
}

void sprite_free_window(void *pointer)
{
    PortWindow *window = find_window((const PortSprite *)pointer);
    if (window == NULL)
        return;
    mmgr_free(window->sprite.sprite_bitmapptr);
    free(window->line_offsets);
    memset(window, 0, sizeof(*window));
}

void sprite_setup1_from_arg_pointer(const PortSprite *sprite)
{
    if (sprite == NULL)
        port_guest_unwind("null sprite descriptor");
    port_sprite_init();
    s_sprite1 = *sprite;
}

void sprite_set_1_from_argptr(const PortSprite *sprite)
{
    sprite_setup1_from_arg_pointer(sprite);
}

void sprite_copy_2_to_1(void)
{
    sprite_setup1_from_arg_pointer(&sprite2);
}

void sprite_copy_2_to_1_2(void)
{
    /* Active C source sprite_copy_2_to_1_2 calls sprcopy2to12, whose accepted
       body selects sprite2 through sprite_setup1_from_arg_pointer. */
    sprite_copy_2_to_1();
}

void sprite_copy_arg_to_both(const PortSprite *sprites)
{
    if (sprites == NULL)
        port_guest_unwind("null paired sprite state");
    port_sprite_init();
    s_sprite1 = sprites[0];
    sprite2 = sprites[1];
}

void sprite_copy_both_to_arg(PortSprite *sprites)
{
    if (sprites == NULL)
        port_guest_unwind("null paired sprite destination");
    port_sprite_init();
    sprites[0] = s_sprite1;
    sprites[1] = sprite2;
}

void sprset1size(int16_t left, int16_t right, int16_t top, int16_t bottom)
{
    int width;
    int height;
    if (left < 0) left = 0;
    if (top < 0) top = 0;
    width = s_sprite1.sprite_bitmapptr != NULL
                ? s_sprite1.sprite_bitmapptr->width : 0;
    height = s_sprite1.sprite_bitmapptr != NULL
                 ? s_sprite1.sprite_bitmapptr->height : 0;
    if (right > width) right = (int16_t)width;
    if (bottom > height) bottom = (int16_t)height;
    if (right < left) right = left;
    if (bottom < top) bottom = top;
    s_sprite1.words2[0] = (uint16_t)left;
    s_sprite1.words2[1] = (uint16_t)right;
    s_sprite1.words2[2] = (uint16_t)top;
    s_sprite1.words2[3] = (uint16_t)bottom;
}

void sprite_clear_1_color(uint8_t color)
{
    size_t extent;
    uint8_t *pixels = sprite_pixels(&s_sprite1, &extent);
    uint16_t left = s_sprite1.words2[0];
    uint16_t right = s_sprite1.words2[1];
    uint16_t top = s_sprite1.words2[2];
    uint16_t bottom = s_sprite1.words2[3];
    uint16_t pitch = s_sprite1.words2[4];
    uint16_t y;
    if (pixels == NULL || s_sprite1.lineofs == NULL || pitch == 0)
        return;
    for (y = top; y < bottom; ++y) {
        uint32_t offset = (uint32_t)s_sprite1.lineofs[y] + left;
        uint16_t x;
        if (offset > extent || (size_t)(right - left) > extent - offset)
            port_guest_unwind("sprite clear exceeded backing extent");
        for (x = left; x < right; ++x)
            pixels[offset + x - left] = color;
    }
}

/* Translate the 94-byte context `load_2477e` copy loop (instruction
   boundaries verified; no strict recipe). It saves a rectangle from the
   active sprite into a shape bitmap using the shape's DOS x/y origin. */
void sprite_clear_shape(PortShape2D *shape)
{
    size_t source_extent;
    size_t destination_extent;
    size_t shape_pixels_count;
    uint8_t *source = sprite_pixels(&s_sprite1, &source_extent);
    uint8_t *destination = shape_pixels(shape, &destination_extent);
    uint16_t width;
    uint16_t height;
    uint16_t source_x;
    uint16_t source_y;
    uint16_t row;
    uint16_t pitch;
    uint16_t active_height;

    if (source == NULL || destination == NULL ||
        s_sprite1.lineofs == NULL || s_sprite1.sprite_bitmapptr == NULL)
        port_guest_unwind("invalid sprite save-under buffers");
    width = shape->width;
    height = shape->height;
    source_x = shape->pos_x;
    source_y = shape->pos_y;
    pitch = s_sprite1.words2[4];
    active_height = s_sprite1.sprite_bitmapptr->height;
    shape_pixels_count = (size_t)width * height;
    if ((size_t)width > destination_extent ||
        shape_pixels_count > destination_extent || source_x > pitch ||
        width > pitch - source_x || source_y > active_height ||
        height > active_height - source_y)
        port_guest_unwind("sprite save-under outside active bounds");

    for (row = 0; row < height; ++row) {
        size_t source_offset = (size_t)s_sprite1.lineofs[source_y + row] +
                               source_x;
        size_t destination_offset = (size_t)row * width;
        if (source_offset > source_extent ||
            (size_t)width > source_extent - source_offset ||
            destination_offset > destination_extent ||
            (size_t)width > destination_extent - destination_offset)
            port_guest_unwind("sprite save-under exceeded backing extent");
        memmove(destination + destination_offset, source + source_offset, width);
    }
}

void sprite1_unknown2(int16_t x, int16_t y, int16_t width,
                      int16_t height, int16_t color)
{
    size_t extent;
    uint8_t *pixels = sprite_pixels(&s_sprite1, &extent);
    int left = x;
    int top = y;
    int draw_width = width;
    int draw_height = height;
    int clip_left = s_sprite1.words2[0];
    int clip_right = s_sprite1.words2[1];
    int clip_top = s_sprite1.words2[2];
    int clip_bottom = s_sprite1.words2[3];
    uint16_t pitch = s_sprite1.words2[4];
    int row;
    /* Translate the four signed clipping branches in
       asm/seg012_sprite_1_unk_group.ASM before the shared color fill. */
    if (pixels == NULL || s_sprite1.lineofs == NULL || pitch == 0 ||
        draw_width <= 0 || draw_height <= 0)
        return;
    if (left < clip_left) {
        draw_width -= clip_left - left;
        left = clip_left;
    }
    if (left + draw_width > clip_right)
        draw_width = clip_right - left;
    if (top < clip_top) {
        draw_height -= clip_top - top;
        top = clip_top;
    }
    if (top + draw_height > clip_bottom)
        draw_height = clip_bottom - top;
    if (draw_width <= 0 || draw_height <= 0)
        return;
    if (left < 0 || top < 0 || left + draw_width > pitch ||
        top + draw_height > s_sprite1.sprite_bitmapptr->height)
        port_guest_unwind("rectangle fill outside active bounds");
    for (row = top; row < top + draw_height; ++row) {
        size_t offset = (size_t)s_sprite1.lineofs[row] + (size_t)left;
        if (offset > extent || (size_t)draw_width > extent - offset)
            port_guest_unwind("rectangle fill exceeded backing extent");
        memset(pixels + offset, (uint8_t)color, (size_t)draw_width);
    }
}

static int16_t signed_word(uint16_t bits)
{
    return bits <= INT16_MAX ? (int16_t)bits
                             : (int16_t)((int32_t)bits - 65536);
}

static int16_t add_signed_words(int16_t left, int16_t right)
{
    return signed_word((uint16_t)((uint16_t)left + (uint16_t)right));
}

static int16_t sub_signed_words(int16_t left, int16_t right)
{
    return signed_word((uint16_t)((uint16_t)left - (uint16_t)right));
}

/* C translation of the accepted `_draw_filled_rect` body in
   asm/sprite_rectangle_scaled_blitters.ASM:47-165 (see also the accepted
   sub_35B76 migration entry, docs/porting/asm-migration.md:368). Preserve its
   left/right/top/absolute-height clipping, row-table addressing and XOR
   color operation; the DOS ES:DI window maps to the active host sprite. */
void draw_filled_rect(int16_t x, int16_t y, int16_t width,
                      int16_t height, int16_t color)
{
    size_t extent;
    uint8_t *pixels = sprite_pixels(&s_sprite1, &extent);
    int16_t left = signed_word(s_sprite1.words2[0]);
    int16_t right = signed_word(s_sprite1.words2[1]);
    int16_t top = signed_word(s_sprite1.words2[2]);
    int16_t sprite_height = signed_word(s_sprite1.words2[3]);
    int16_t pitch = signed_word(s_sprite1.words2[4]);
    int16_t delta;
    int16_t row;
    int16_t col;

    if (pixels == NULL || s_sprite1.lineofs == NULL || pitch <= 0)
        return;

    /* Each ADD/SUB and signed branch here follows the original 16-bit word
       operation, including wrap at the register boundary. */
    delta = sub_signed_words(left, x);
    if (delta > 0) {
        x = left;
        width = sub_signed_words(width, delta);
        if (width <= 0)
            return;
    }
    delta = sub_signed_words(add_signed_words(x, width), right);
    if (delta > 0) {
        width = sub_signed_words(width, delta);
        if (width <= 0)
            return;
    }
    delta = sub_signed_words(top, y);
    if (delta > 0) {
        height = sub_signed_words(height, delta);
        if (height <= 0)
            return;
        y = top;
    }
    delta = sub_signed_words(add_signed_words(y, height), sprite_height);
    if (delta > 0) {
        height = sub_signed_words(height, delta);
        if (height <= 0)
            return;
    }
    if (width <= 0 || height <= 0)
        return;

    if (x < 0 || y < 0 || x >= pitch || width > pitch - x ||
        y >= s_sprite1.sprite_bitmapptr->height ||
        height > s_sprite1.sprite_bitmapptr->height - y)
        port_guest_unwind("filled rectangle outside active sprite bounds");

    for (row = 0; row < height; ++row) {
        size_t offset = (size_t)s_sprite1.lineofs[(uint16_t)(y + row)] +
                        (uint16_t)x;
        if (offset > extent || (size_t)width > extent - offset)
            port_guest_unwind("filled rectangle exceeded active sprite extent");
        for (col = 0; col < width; ++col)
            pixels[offset + (uint16_t)col] ^= (uint8_t)color;
    }
    if (s_sprite1.sprite_bitmapptr == &s_screen_shape)
        port_video_publish("draw_filled_rect");
}

static void draw_opaque(const PortShape2D *shape, int x, int y)
{
    size_t src_extent;
    size_t dst_extent;
    uint8_t *src = shape_pixels(shape, &src_extent);
    uint8_t *dst = sprite_pixels(&s_sprite1, &dst_extent);
    int left = s_sprite1.words2[0];
    int right = s_sprite1.words2[1];
    int top = s_sprite1.words2[2];
    int bottom = s_sprite1.words2[3];
    int width;
    int height;
    int draw_left;
    int draw_right;
    int draw_top;
    int draw_bottom;
    int row;
    if (src == NULL || dst == NULL || s_sprite1.lineofs == NULL)
        return;
    width = shape->width;
    height = shape->height;
    draw_left = x > left ? x : left;
    draw_right = x + width < right ? x + width : right;
    draw_top = y > top ? y : top;
    draw_bottom = y + height < bottom ? y + height : bottom;
    if (draw_right <= draw_left || draw_bottom <= draw_top)
        return;
    for (row = draw_top; row < draw_bottom; ++row) {
        size_t src_offset = (size_t)(row - y) * width + (draw_left - x);
        size_t dst_offset = (size_t)s_sprite1.lineofs[row] + draw_left;
        size_t count = (size_t)(draw_right - draw_left);
        if (src_offset > src_extent || count > src_extent - src_offset ||
            dst_offset > dst_extent || count > dst_extent - dst_offset)
            port_guest_unwind("sprite blit exceeded backing extent");
        memcpy(dst + dst_offset, src + src_offset, count);
    }
}

static void compose_boolean(const PortShape2D *shape, int x, int y, int use_and)
{
    size_t src_extent;
    size_t dst_extent;
    uint8_t *src = shape_pixels(shape, &src_extent);
    uint8_t *dst = sprite_pixels(&s_sprite1, &dst_extent);
    int left = s_sprite1.words2[0];
    int right = s_sprite1.words2[1];
    int top = s_sprite1.words2[2];
    int bottom = s_sprite1.words2[3];
    int draw_left;
    int draw_right;
    int draw_top;
    int draw_bottom;
    int row;
    if (src == NULL || dst == NULL || s_sprite1.lineofs == NULL)
        return;
    if ((size_t)shape->width * shape->height > src_extent)
        port_guest_unwind("boolean sprite source exceeded backing extent");
    draw_left = x > left ? x : left;
    draw_right = x + shape->width < right ? x + shape->width : right;
    draw_top = y > top ? y : top;
    draw_bottom = y + shape->height < bottom ? y + shape->height : bottom;
    if (draw_right <= draw_left || draw_bottom <= draw_top)
        return;
    for (row = draw_top; row < draw_bottom; ++row) {
        size_t src_offset = (size_t)(row - y) * shape->width +
                            (size_t)(draw_left - x);
        size_t dst_offset = (size_t)s_sprite1.lineofs[row] +
                            (size_t)draw_left;
        size_t count = (size_t)(draw_right - draw_left);
        size_t column;
        if (src_offset > src_extent || count > src_extent - src_offset ||
            dst_offset > dst_extent || count > dst_extent - dst_offset)
            port_guest_unwind("boolean sprite blit exceeded backing extent");
        for (column = 0; column < count; ++column) {
            if (use_and)
                dst[dst_offset + column] &= src[src_offset + column];
            else
                dst[dst_offset + column] |= src[src_offset + column];
        }
    }
}

void sprite_putimage_and(const PortShape2D *shape, int16_t x, int16_t y)
{
    if (shape != NULL)
        compose_boolean(shape, x, y, 1);
}

void sprite_putimage_and_alt(const PortShape2D *shape, int16_t x, int16_t y)
{
    /* Despite its recovered name, load_23BBC enters the opaque copy core
       (REP MOVSB/MOVSW), not the AND loop used by the other two entries. */
    draw_opaque(shape, x, y);
}

static int16_t sprite_header_relative(int16_t coordinate, uint16_t origin)
{
    /* These entrypoints SUB AX,[SI+4/6] before signed clipping. */
    uint16_t bits = (uint16_t)((uint16_t)coordinate - origin);
    return (int16_t)(bits < 0x8000u ? (int32_t)bits : (int32_t)bits - 0x10000);
}

void sprite_putimage_and_alt2(const PortShape2D *shape, int16_t x, int16_t y)
{
    if (shape != NULL)
        compose_boolean(shape, sprite_header_relative(x, shape->unknown1),
                        sprite_header_relative(y, shape->unknown2), 1);
}

void sprite_putimage_or(const PortShape2D *shape, int16_t x, int16_t y)
{
    if (shape != NULL)
        compose_boolean(shape, x, y, 0);
}

void sprite_putimage_or_alt(const PortShape2D *shape, int16_t x, int16_t y)
{
    if (shape != NULL)
        compose_boolean(shape, sprite_header_relative(x, shape->unknown1),
                        sprite_header_relative(y, shape->unknown2), 0);
}

void sprite_shape_to_1(const PortShape2D *shape, int16_t x, int16_t y)
{
    draw_opaque(shape, x, y);
}

void sprite_shape_to_1_alt(const PortShape2D *shape)
{
    if (shape != NULL)
        draw_opaque(shape, shape->pos_x, shape->pos_y);
}

/* Translation of seg012:0x23E00..0x23E90. Positive signed controls consume
   one color and repeat it with STOSB; negative controls copy literals with
   MOVSB. Runs continue across rows through the active line-offset table and
   terminate at a zero control byte. */
static void shape2d_draw_runs(const PortShape2D *shape,
                                     uint16_t origin_x, uint16_t origin_y,
                                     const char *publication_reason)
{
    size_t source_extent;
    size_t destination_extent;
    uint8_t *source = shape_pixels(shape, &source_extent);
    uint8_t *destination = sprite_pixels(&s_sprite1, &destination_extent);
    size_t source_index = 0;
    uint16_t row;
    uint16_t remaining_width;
    uint16_t destination_offset;
    uint16_t width;
    uint16_t height;
    if (shape == NULL || source == NULL || destination == NULL ||
        s_sprite1.lineofs == NULL || s_sprite1.sprite_bitmapptr == NULL)
        return;
    width = shape->width;
    height = s_sprite1.sprite_bitmapptr->height;
    row = origin_y;
    remaining_width = width;
    if (row >= height)
        port_guest_unwind("shape2d row started outside the active sprite");
    destination_offset = (uint16_t)(s_sprite1.lineofs[row] + origin_x);

    while (source_index < source_extent) {
        int8_t control = (int8_t)source[source_index++];
        unsigned run;
        unsigned item;
        uint8_t repeated = 0;
        if (control == 0)
            break;
        run = (unsigned)(control < 0 ? -(int)control : control);
        if (control > 0) {
            if (source_index >= source_extent)
                port_guest_unwind("shape2d repeat packet exceeded its source extent");
            repeated = source[source_index++];
        } else if (run > source_extent - source_index) {
            port_guest_unwind("shape2d literal run exceeded its source extent");
        }
        for (item = 0; item < run; ++item) {
            if (row >= height)
                port_guest_unwind("shape2d row advanced outside the active sprite");
            if ((size_t)destination_offset >= destination_extent)
                port_guest_unwind("shape2d destination exceeded its sprite extent");
            destination[destination_offset] = control > 0
                ? repeated : source[source_index++];
            --remaining_width;
            if ((int16_t)remaining_width <= 0) {
                ++row;
                /* The DOS loop resolves the next row after the final pixel,
                   before reading its zero terminator. Avoid reading beyond
                   the host row table when that next row is never drawn. */
                if (row < height)
                    destination_offset = (uint16_t)(s_sprite1.lineofs[row] +
                                                    origin_x);
                remaining_width = width;
            } else {
                ++destination_offset;
            }
        }
    }
    if (s_sprite1.sprite_bitmapptr == &s_screen_shape)
        port_video_publish(publication_reason);
}

void shape2d_op_unk(const PortShape2D *shape)
{
    if (shape != NULL)
        shape2d_draw_runs(shape, shape->pos_x, shape->pos_y,
                                  "shape2d_op_unk");
}

/* The 30-byte seg012:0x23DE2 entry loads explicit x/y arguments and jumps
   into the shared run decoder at 0x23E1B. */
void shape2d_op_unknown5(const PortShape2D *shape, int16_t x, int16_t y)
{
    if (shape != NULL)
        shape2d_draw_runs(shape, (uint16_t)x, (uint16_t)y,
                                  "shape2d_op_unknown5");
}

/* Translation of the clipped alternate decoder at seg012:0x23ED2. Its
   positive controls consume and repeat one colour; negative controls copy
   literal bytes. The sign selects the loop form. The
   shape's declared width/height establish source coordinates, while DOS
   clip bounds select destination writes. */
void shape2d_op_unk3(const PortShape2D *shape)
{
    size_t source_extent;
    size_t destination_extent;
    uint8_t *source = shape_pixels(shape, &source_extent);
    uint8_t *destination = sprite_pixels(&s_sprite1, &destination_extent);
    size_t pixel_count;
    size_t pixel_index = 0;
    size_t source_index = 0;
    int origin_x;
    int origin_y;
    int clip_left;
    int clip_right;
    int clip_top;
    int clip_bottom;
    if (shape == NULL || source == NULL || destination == NULL ||
        s_sprite1.lineofs == NULL || s_sprite1.sprite_bitmapptr == NULL ||
        shape->width == 0 || shape->height == 0)
        return;
    if ((size_t)shape->width > SIZE_MAX / (size_t)shape->height)
        port_guest_unwind("shape2d image dimensions overflowed");
    pixel_count = (size_t)shape->width * (size_t)shape->height;
    origin_x = signed_word(shape->pos_x);
    origin_y = signed_word(shape->pos_y);
    clip_left = signed_word(s_sprite1.words2[0]);
    clip_right = signed_word(s_sprite1.words2[1]);
    clip_top = signed_word(s_sprite1.words2[2]);
    clip_bottom = signed_word(s_sprite1.words2[3]);

    while (pixel_index < pixel_count && source_index < source_extent) {
        int8_t control = (int8_t)source[source_index++];
        size_t run;
        size_t count;
        size_t item;
        if (control == 0)
            break;
        run = (size_t)(control > 0 ? control : -(int)control);
        count = run < pixel_count - pixel_index
            ? run : pixel_count - pixel_index;
        if (control > 0 && source_index >= source_extent)
            port_guest_unwind("shape2d repeat packet exceeded its source extent");
        if (control < 0 && count > source_extent - source_index)
            port_guest_unwind("shape2d literal packet exceeded its source extent");
        {
            uint8_t repeated = control > 0 ? source[source_index++] : 0;
            for (item = 0; item < count; ++item, ++pixel_index) {
                int target_x = origin_x + (int)(pixel_index % shape->width);
                int target_y = origin_y + (int)(pixel_index / shape->width);
                uint8_t color = control > 0 ? repeated : source[source_index++];
                if (target_x < clip_left || target_x >= clip_right ||
                    target_y < clip_top || target_y >= clip_bottom ||
                    target_x < 0 || target_y < 0 ||
                    target_x >= s_sprite1.sprite_bitmapptr->width ||
                    target_y >= s_sprite1.sprite_bitmapptr->height)
                    continue;
                {
                    size_t offset = (size_t)s_sprite1.lineofs[target_y] +
                                    (size_t)target_x;
                    if (offset >= destination_extent)
                        port_guest_unwind("shape2d pixel exceeded its sprite extent");
                    destination[offset] = color;
                }
            }
        }
    }
    if (s_sprite1.sprite_bitmapptr == &s_screen_shape)
        port_video_publish("shape2d_op_unk3");
}

/* C translation of the shared clipped RLE draw path entered by the recovered
   shape2d_op_unk2 prologue at seg012:0x5494 and continued at
   shape2d_op_unk3 (seg012:0x54B2). Positive signed run counts repeat the
   following colour byte; negative counts copy the following literal bytes.
   Decode every source row even when clipping hides its left or right edge. */
void shape2d_op_unk2(const PortShape2D *shape, int16_t x, int16_t y)
{
    size_t source_extent;
    size_t destination_extent;
    uint8_t *source = shape_pixels(shape, &source_extent);
    uint8_t *destination = sprite_pixels(&s_sprite1, &destination_extent);
    int16_t clip_left = signed_word(s_sprite1.words2[0]);
    int16_t clip_right = signed_word(s_sprite1.words2[1]);
    int16_t clip_top = signed_word(s_sprite1.words2[2]);
    int16_t clip_bottom = signed_word(s_sprite1.words2[3]);
    size_t source_index = 0;
    size_t pixel_index = 0;
    size_t pixel_count;
    if (shape == NULL || source == NULL || destination == NULL ||
        s_sprite1.lineofs == NULL || s_sprite1.sprite_bitmapptr == NULL)
        return;
    if (shape->width == 0 || shape->height == 0)
        return;
    pixel_count = (size_t)shape->width * shape->height;
    while (pixel_index < pixel_count) {
        int8_t control;
        size_t run;
        size_t count;
        size_t item;
        if (source_index >= source_extent)
            port_guest_unwind("shape2d RLE ended before the declared image extent");
        control = (int8_t)source[source_index++];
        if (control == 0)
            return;
        run = (size_t)(control > 0 ? control : -(int)control);
        count = run < pixel_count - pixel_index ? run : pixel_count - pixel_index;
        if (control > 0) {
            uint8_t color;
            if (source_index >= source_extent)
                port_guest_unwind("shape2d RLE colour was outside the image extent");
            color = source[source_index++];
            for (item = 0; item < count; ++item, ++pixel_index) {
                int target_x = (int)x + (int)(pixel_index % shape->width);
                int target_y = (int)y + (int)(pixel_index / shape->width);
                if (target_x >= clip_left && target_x < clip_right &&
                    target_y >= clip_top && target_y < clip_bottom &&
                    target_x >= 0 && target_y >= 0 &&
                    target_x < s_sprite1.sprite_bitmapptr->width &&
                    target_y < s_sprite1.sprite_bitmapptr->height) {
                    size_t offset = (size_t)s_sprite1.lineofs[target_y] +
                                    (size_t)target_x;
                    if (offset >= destination_extent)
                        port_guest_unwind("shape2d destination exceeded its sprite extent");
                    destination[offset] = color;
                }
            }
        } else {
            if (count > source_extent - source_index)
                port_guest_unwind("shape2d literal run was outside the image extent");
            for (item = 0; item < count; ++item, ++pixel_index) {
                int target_x = (int)x + (int)(pixel_index % shape->width);
                int target_y = (int)y + (int)(pixel_index / shape->width);
                uint8_t color = source[source_index++];
                if (target_x >= clip_left && target_x < clip_right &&
                    target_y >= clip_top && target_y < clip_bottom &&
                    target_x >= 0 && target_y >= 0 &&
                    target_x < s_sprite1.sprite_bitmapptr->width &&
                    target_y < s_sprite1.sprite_bitmapptr->height) {
                    size_t offset = (size_t)s_sprite1.lineofs[target_y] +
                                    (size_t)target_x;
                    if (offset >= destination_extent)
                        port_guest_unwind("shape2d destination exceeded its sprite extent");
                    destination[offset] = color;
                }
            }
        }
    }
}

void sprite_clear_shape_alt(PortShape2D *shape, int16_t x, int16_t y)
{
    size_t source_extent;
    size_t destination_extent;
    uint8_t *source = sprite_pixels(&s_sprite1, &source_extent);
    uint8_t *destination;
    int clip_left;
    int clip_right;
    int clip_top;
    int clip_bottom;
    int left;
    int right;
    int top;
    int bottom;
    int row;
    if (shape == NULL || source == NULL || s_sprite1.lineofs == NULL)
        return;
    destination = shape_pixels(shape, &destination_extent);
    if (destination == NULL)
        return;
    shape->pos_x = (uint16_t)x;
    shape->pos_y = (uint16_t)y;
    if (source == destination)
        return;

    clip_left = s_sprite1.words2[0];
    clip_right = s_sprite1.words2[1];
    clip_top = s_sprite1.words2[2];
    clip_bottom = s_sprite1.words2[3];
    left = x > clip_left ? x : clip_left;
    right = x + shape->width < clip_right ? x + shape->width : clip_right;
    top = y > clip_top ? y : clip_top;
    bottom = y + shape->height < clip_bottom ? y + shape->height : clip_bottom;
    if (right <= left || bottom <= top)
        return;
    if ((size_t)shape->width * shape->height > destination_extent)
        port_guest_unwind("sprite save exceeded backing extent");
    for (row = top; row < bottom; ++row) {
        size_t source_offset;
        size_t destination_offset;
        size_t count = (size_t)(right - left);
        int source_y = row;
        int source_x = left;
        destination_offset = (size_t)(row - y) * shape->width +
                             (size_t)(left - x);
        if (source_y < 0 ||
            (uint32_t)source_y >= s_sprite1.sprite_bitmapptr->height || source_x < 0 ||
            (uint32_t)source_x + count > s_sprite1.sprite_bitmapptr->width)
            port_guest_unwind("sprite save outside active bounds");
        source_offset = (size_t)s_sprite1.lineofs[source_y] +
                        (size_t)source_x;
        if (source_offset > source_extent || count > source_extent - source_offset ||
            destination_offset > destination_extent ||
            count > destination_extent - destination_offset)
            port_guest_unwind("sprite save exceeded backing extent");
        memcpy(destination + destination_offset, source + source_offset, count);
    }
}

void sprputimage(const PortShape2D *shape)
{
    if (shape != NULL) {
        draw_opaque(shape, shape->pos_x, shape->pos_y);
        if (s_sprite1.sprite_bitmapptr == &s_screen_shape)
            port_video_publish("sprite_blit_to_video");
    }
}

/* Faithful C translation of asm/seg012_sprite_1_unk_group.ASM:
   _sprite_1_unk3 (lines 146-165, 166-291). The 12 table entries select
   interlaced source/destination rows; each lane advances by 12 rows, with a
   phase increment for each destination row. The routine writes directly to
   the active sprite segment, which is the MCGA video aperture for the intro
   transition. */
void sprite_1_unk3(const PortShape2D *shape, int16_t phase)
{
    static const uint8_t row_pattern[12] = {
        11, 5, 8, 2, 10, 4, 7, 1, 9, 3, 6, 0
    };
    static const uint8_t pre_skip[4] = { 1, 3, 0, 2 };
    static const uint8_t post_skip[4] = { 3, 1, 4, 2 };
    size_t src_extent;
    uint8_t *src_base;
    uint8_t *dst_base;
    PortFarPtr source_address;
    PortFarPtr target_address;
    uint16_t width;
    uint16_t height;
    uint16_t base_x;
    uint16_t base_y;
    uint16_t phase_word = (uint16_t)phase;
    PortVideoTransition transition;
    int visible;
    unsigned lane;

    if (shape == NULL || s_sprite1.sprite_bitmapptr == NULL ||
        s_sprite1.lineofs == NULL)
        return;
    src_base = shape_pixels(shape, &src_extent);
    dst_base = sprite_pixels(&s_sprite1, NULL);
    if (src_base == NULL || dst_base == NULL)
        return;
    if (!port_far_from_host(src_base, &source_address, NULL) ||
        !port_far_from_host(dst_base, &target_address, NULL))
        port_guest_unwind("sprite_1_unk3 surface has no guest segment address");
    width = shape->width;
    height = shape->height;
    if ((size_t)width * height > src_extent)
        port_guest_unwind("sprite_1_unk3 source exceeded backing extent");
    base_x = shape->pos_x;
    base_y = shape->pos_y;
    visible = s_sprite1.sprite_bitmapptr == &s_screen_shape;
    if (visible)
        port_video_transition_begin(&transition);

    /* Assembly visits the row pattern from index 11 down to 0. The actual
       row offset is pattern[index] + 12*k, bounded by the shape's height. */
    for (lane = 0; lane < 12; ++lane) {
        unsigned source_row = row_pattern[11u - lane];
        unsigned row_step = 0;

        if (visible)
            port_video_transition_advance(&transition, PORT_TRANSITION_LANE_WORK);

        for (;;) {
            uint32_t row_index = source_row + 12u * row_step;
            uint32_t target_row = (uint32_t)base_y + row_index;
            uint16_t source_offset;
            uint16_t target_offset;
            int remaining = width;
            uint16_t xphase = (uint16_t)(phase_word + lane + row_step);
            unsigned target_height = s_sprite1.sprite_bitmapptr->height;

            if (row_index >= height || target_row >= target_height)
                break;
            source_offset = (uint16_t)(row_index * width);
            if ((size_t)row_index * width > src_extent ||
                width > src_extent - (size_t)row_index * width)
                port_guest_unwind("sprite_1_unk3 row exceeded source extent");
            target_offset = (uint16_t)(s_sprite1.lineofs[target_row] + base_x);

            while (remaining > 0) {
                unsigned slot = xphase & 3u;
                unsigned before = pre_skip[slot];
                unsigned after;
                remaining -= (int)before;
                if (remaining <= 0)
                    break;
                source_offset = (uint16_t)(source_offset + before);
                target_offset = (uint16_t)(target_offset + before);
                {
                    PortFarPtr source_pixel = source_address;
                    PortFarPtr target_pixel = target_address;
                    uint8_t *source_host;
                    uint8_t *target_host;
                    source_pixel.offset = (uint16_t)(source_pixel.offset +
                                                     source_offset);
                    target_pixel.offset = (uint16_t)(target_pixel.offset +
                                                     target_offset);
                    source_host = (uint8_t *)port_far_resolve(source_pixel, 1u);
                    target_host = (uint8_t *)port_far_resolve(target_pixel, 1u);
                    if (source_host == NULL)
                        port_guest_unwind("sprite_1_unk3 source address is unmapped");
                    if (target_host == NULL)
                        port_guest_unwind("sprite_1_unk3 target address is unmapped");
                    *target_host = *source_host;
                }
                /* MOV AL,[SI] / MOV ES:[DI],AL advance neither register;
                   both offsets advance only by the phase post-skip below. */
                after = post_skip[slot];
                source_offset = (uint16_t)(source_offset + after);
                target_offset = (uint16_t)(target_offset + after);
                remaining -= (int)after;
                ++xphase;
            }
            if (visible)
                port_video_transition_advance(&transition,
                    port_transition_row_work(width,
                        (uint16_t)(phase_word + lane + row_step)));
            ++row_step;
        }
    }

    if (visible)
        port_video_publish("sprite_1_unk3");
}

void port_sprite_plot_active(int16_t x, int16_t y, uint8_t color)
{
    size_t extent;
    uint8_t *pixels = sprite_pixels(&s_sprite1, &extent);
    uint16_t left = s_sprite1.words2[0];
    uint16_t right = s_sprite1.words2[1];
    uint16_t top = s_sprite1.words2[2];
    uint16_t bottom = s_sprite1.words2[3];
    size_t offset;
    if (pixels == NULL || s_sprite1.lineofs == NULL ||
        x < left || x >= right || y < top || y >= bottom)
        return;
    offset = (size_t)s_sprite1.lineofs[(uint16_t)y] + (uint16_t)x;
    if (offset >= extent)
        port_guest_unwind("font pixel exceeded sprite backing extent");
    pixels[offset] = color;
}

/* The font entries access ES:DI directly and never inspect the sprite clip.
   Preserve their 16-bit row-table-plus-X address, including horizontal wrap. */
void port_sprite_plot_font(int16_t x, int16_t y, uint8_t color)
{
    size_t extent;
    uint8_t *pixels = sprite_pixels(&s_sprite1, &extent);
    uint16_t offset;
    if (pixels == NULL || s_sprite1.lineofs == NULL)
        return;
    if ((uint16_t)y >= s_sprite1.sprite_bitmapptr->height)
        port_guest_unwind("font row outside active sprite line table");
    offset = (uint16_t)(s_sprite1.lineofs[(uint16_t)y] + (uint16_t)x);
    if (offset >= extent)
        port_guest_unwind("font pixel exceeded sprite backing extent");
    pixels[offset] = color;
}

/* Host-view adapter for asm/putpixel_single_maybe.ASM. Its four signed
   half-open clip comparisons select the active sprite, then its lineofs[y]
   plus x address and low color byte are reproduced by the common plotter. */
void putpixel_single_maybe(int16_t x, int16_t y, int16_t color)
{
    port_sprite_plot_active(x, y, (uint8_t)color);
}

static uint32_t raster_line_slope(unsigned minor, unsigned major);
static void raster_line_draw_clipped(int16_t x1, int16_t y1,
                                     int16_t x2, int16_t y2,
                                     int16_t color);

/* Semantic host translation of the short preRender_line dispatcher at
   seg012:0x1FDDE plus putpixel_line1_maybe. The major-axis fixed-point walk
   follows the descriptor's half-pixel phase, and active-sprite clipping is
   applied to the resulting samples. */
void preRender_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
                    int16_t color)
{
    raster_line_draw_clipped(x1, y1, x2, y2, color);
    if (s_sprite1.sprite_bitmapptr == &s_screen_shape)
        port_video_publish("preRender_line");
}

typedef enum RasterFillMode {
    RASTER_FILL_SOLID,
    RASTER_FILL_PATTERN,
    RASTER_FILL_AUXILIARY
} RasterFillMode;

typedef struct RasterPoint {
    int16_t x;
    int16_t y;
} RasterPoint;

static uint16_t rotate_pattern_rows(uint16_t pattern)
{
    return (uint16_t)((pattern << 8) | (pattern >> 8));
}

static uint8_t rotate_pattern_pixel(uint8_t pattern)
{
    return (uint8_t)((pattern << 1) | (pattern >> 7));
}

static int sprite_raster_bounds(int *left, int *right, int *top, int *bottom)
{
    int width;
    int height;
    if (s_sprite1.sprite_bitmapptr == NULL || s_sprite1.lineofs == NULL)
        return 0;
    width = s_sprite1.sprite_bitmapptr->width;
    height = s_sprite1.sprite_bitmapptr->height;
    *left = s_sprite1.words2[0];
    *right = s_sprite1.words2[1];
    *top = s_sprite1.words2[2];
    *bottom = s_sprite1.words2[3];
    if (*left < 0) *left = 0;
    if (*top < 0) *top = 0;
    if (*right > width) *right = width;
    if (*bottom > height) *bottom = height;
    return *right > *left && *bottom > *top;
}

/* draw_patterned_lines rotates its 16-bit row pattern on every scanline,
   aligns the low byte to the left edge, then rotates once per destination
   pixel. The byte swap is intentionally retained between calls because the
   DOS routine mutates _line_pattern_bits in DGROUP. */
static void fill_raster_span(int left, int y, int right_inclusive,
                             uint8_t color, RasterFillMode mode,
                             uint16_t *pattern)
{
    int clip_left;
    int clip_right;
    int clip_top;
    int clip_bottom;
    size_t extent;
    uint8_t *pixels;
    uint8_t mask;
    size_t offset;
    int x;

    if (!sprite_raster_bounds(&clip_left, &clip_right, &clip_top,
                              &clip_bottom) || y < clip_top || y >= clip_bottom)
        return;
    if (left < clip_left) left = clip_left;
    if (right_inclusive >= clip_right) right_inclusive = clip_right - 1;
    if (right_inclusive < left)
        return;
    pixels = sprite_pixels(&s_sprite1, &extent);
    if (pixels == NULL)
        return;
    offset = (size_t)s_sprite1.lineofs[(uint16_t)y] + (size_t)left;
    if (offset > extent || (size_t)(right_inclusive - left + 1) > extent - offset)
        port_guest_unwind("polygon span exceeded active sprite extent");
    if (mode == RASTER_FILL_SOLID) {
        memset(pixels + offset, color, (size_t)(right_inclusive - left + 1));
        return;
    }

    mask = (uint8_t)*pattern;
    for (x = 0; x < (left & 7); ++x)
        mask = rotate_pattern_pixel(mask);
    for (x = left; x <= right_inclusive; ++x) {
        mask = rotate_pattern_pixel(mask);
        if ((mask & 1u) != 0)
            pixels[offset + (size_t)(x - left)] =
                mode == RASTER_FILL_AUXILIARY ? s_line_auxiliary_color : color;
        else if (mode == RASTER_FILL_AUXILIARY)
            pixels[offset + (size_t)(x - left)] = color;
    }
}

typedef struct RasterInterval {
    int left;
    int right;
} RasterInterval;

static uint32_t raster_line_slope(unsigned minor, unsigned major)
{
    uint64_t fixed = (uint64_t)minor << 16;
    uint32_t slope = (uint32_t)(fixed / major);
    if (major >= 50u && fixed % major > major / 2u)
        ++slope;
    return slope;
}

static int64_t raster_ceil_div(int64_t numerator, int64_t denominator)
{
    if (numerator >= 0)
        return (numerator + denominator - 1) / denominator;
    /* C integer division truncates toward zero, which is ceil for negatives. */
    return numerator / denominator;
}

/* The polygon helpers rasterize each edge with the line record's major-axis
   DDA. For an x-major edge, all x samples that land on one scan row form a
   boundary interval; using only the geometric intersection drops those DOS
   pixels (for example, the row starts at x=10 and ends at x=11 for a 4:1
   edge). */
/* helper3 mode8's right-side loop has a final store after LOOP expires.
   Its carry phase can place the endpoint on the row below the geometric end;
   helper2 and the left-side loop have different terminal stores. See
   slope_raster_loop_head_29/30 and branch_path_30 in the locked ASM. */
static int raster_has_terminal_spill(int dx, int dy)
{
    if (dx <= 0 || dy <= 0 || dx <= dy)
        return 0;
    uint32_t slope = raster_line_slope((unsigned)dy, (unsigned)dx);
    uint64_t final_y = 0x8000u + (uint64_t)((unsigned)dx + 1u) * slope;
    return (final_y >> 16) > (unsigned)dy;
}

typedef struct RasterLineRecord {
    uint16_t x_fraction_low;
    int16_t start_x;
    uint16_t y_fraction_low;
    int16_t start_y;
    int16_t end_x;
    int16_t end_y;
    uint16_t slope;
    uint16_t step_count;
    uint16_t color;
    uint8_t mode;
    uint8_t clip_flags;
    uint16_t top_clipped_rows;
    uint16_t bottom_clipped_rows;
    uint16_t left_clipped_rows;
    uint16_t right_clipped_rows;
} RasterLineRecord;

_Static_assert(sizeof(RasterLineRecord) == 28u,
               "line descriptor must match the 14-word DOS record");

static unsigned draw_line_record(unsigned x0_raw, unsigned y0_raw,
                                 unsigned x1_raw, unsigned y1_raw,
                                 void *record_pointer,
                                 int clipping_enabled);

static int raster_edge_interval(const RasterPoint *first,
                                const RasterPoint *second, int y,
                                int terminal_sample,
                                RasterInterval *interval)
{
    const RasterPoint *top = first;
    const RasterPoint *bottom = second;
    int start_x;
    int start_y;
    int delta_y;
    int dx;
    unsigned abs_dx;
    unsigned dy;
    uint32_t slope;
    uint64_t advance;
    int direction;

    if (top->y > bottom->y) {
        top = second;
        bottom = first;
    }
    start_x = top->x;
    start_y = top->y;
    dy = (unsigned)((int)bottom->y - start_y);
    delta_y = y - start_y;
    if (dy == 0u || delta_y < 0)
        return 0;
    dx = (int)bottom->x - start_x;
    direction = dx < 0 ? -1 : 1;
    abs_dx = (unsigned)(dx < 0 ? -dx : dx);
    if ((unsigned)delta_y > dy) {
        if (terminal_sample && (unsigned)delta_y == dy + 1u) {
            interval->left = interval->right = bottom->x;
            return 1;
        }
        return 0;
    }
    if (dy >= abs_dx) {
        if (abs_dx == 0u) {
            interval->left = start_x;
            interval->right = start_x;
            return 1;
        }
        slope = raster_line_slope(abs_dx, dy);
        advance = (uint64_t)(unsigned)delta_y * slope;
        advance += direction > 0 ? 0x8000u : 0x7fffu;
        advance >>= 16;
        interval->left = start_x + direction * (int)advance;
        interval->right = interval->left;
        return 1;
    }

    slope = raster_line_slope(dy, abs_dx);
    if (slope == 0u)
        return 0;
    {
        int64_t low = (int64_t)delta_y * 65536 - 0x8000;
        int64_t high = (int64_t)(delta_y + 1) * 65536 - 0x8000;
        int64_t first_step = raster_ceil_div(low, slope);
        int64_t last_step = raster_ceil_div(high, slope) - 1;
        int first_x;
        int last_x;
        if (first_step < 0)
            first_step = 0;
        if (last_step > (int64_t)abs_dx)
            last_step = abs_dx;
        if (first_step > last_step)
            return 0;
        first_x = start_x + direction * (int)first_step;
        last_x = start_x + direction * (int)last_step;
        interval->left = first_x < last_x ? first_x : last_x;
        interval->right = first_x > last_x ? first_x : last_x;
    }
    return 1;
}

static int raster_floor_fixed(int64_t value)
{
    if (value >= 0)
        return (int)(value >> 16);
    return -(int)(((-value) + 0xffff) >> 16);
}

/* Consume the exact descriptor that the frozen line initializer produces.
   The helper advances y-major modes by rows and x-major modes by columns. */
static int raster_record_interval(const RasterLineRecord *record, int y,
                                  RasterInterval *interval)
{
    unsigned mode = record->mode;
    if (record->step_count == 0u || mode < 2u || mode > 8u)
        return 0;
    if (mode <= 6u) {
        int delta = y - record->start_y;
        int x;
        int64_t fixed;
        if (delta < 0 || (unsigned)delta >= record->step_count)
            return 0;
        switch (mode) {
        case 2u: x = record->start_x; break;
        case 3u: x = record->start_x - delta; break;
        case 4u: x = record->start_x + delta; break;
        case 5u:
            fixed = (int64_t)record->x_fraction_low + 0x8000 -
                    (int64_t)(unsigned)delta * record->slope;
            x = record->start_x + raster_floor_fixed(fixed);
            break;
        case 6u:
            fixed = (int64_t)record->x_fraction_low + 0x8000 +
                    (int64_t)(unsigned)delta * record->slope;
            x = record->start_x + (int)(fixed >> 16);
            break;
        default: return 0;
        }
        interval->left = interval->right = x;
        return 1;
    }

    {
        int64_t phase = (int64_t)record->y_fraction_low + 0x8000;
        int64_t low = (int64_t)(y - record->start_y) * 65536 - phase;
        int64_t high = low + 65536;
        int64_t first_step;
        int64_t last_step;
        if (record->slope == 0u) {
            if (low > 0 || high <= 0) return 0;
            first_step = 0;
            last_step = record->step_count - 1u;
        } else {
            first_step = raster_ceil_div(low, record->slope);
            last_step = raster_ceil_div(high, record->slope) - 1;
            if (first_step < 0) first_step = 0;
            if (last_step >= record->step_count)
                last_step = record->step_count - 1u;
            if (last_step < first_step) return 0;
        }
        if (mode == 7u) {
            interval->left = record->start_x - (int)last_step;
            interval->right = record->start_x - (int)first_step;
        } else {
            interval->left = record->start_x + (int)first_step;
            interval->right = record->start_x + (int)last_step;
        }
    }
    return 1;
}

static int raster_record_prefill(const RasterLineRecord *record, int y,
                                 int clip_left, int clip_right,
                                 RasterInterval *interval)
{
    int rounded_start = record->start_y +
                        (record->y_fraction_low >= 0x8000u);
    int row;
    if (record->top_clipped_rows != 0u) {
        row = rounded_start - (int)record->top_clipped_rows;
        if (y >= row && y < rounded_start) {
            interval->left = clip_left;
            interval->right = clip_left - 1;
            return 1;
        }
    }
    if (record->left_clipped_rows != 0u) {
        row = rounded_start - (int)record->left_clipped_rows;
        if (y >= row && y < rounded_start) {
            interval->left = clip_right;
            interval->right = clip_right - 1;
            return 1;
        }
    }
    if (record->bottom_clipped_rows != 0u) {
        row = (int)record->end_y + 1;
        if (y >= row && y < row + (int)record->bottom_clipped_rows) {
            interval->left = clip_left;
            interval->right = clip_left - 1;
            return 1;
        }
    }
    if (record->right_clipped_rows != 0u) {
        row = (int)record->end_y + 1;
        if (y >= row && y < row + (int)record->right_clipped_rows) {
            interval->left = clip_right;
            interval->right = clip_right - 1;
            return 1;
        }
    }
    return 0;
}

static int raster_record_postfill(const RasterLineRecord *record, int y,
                                  int clip_left, int clip_right,
                                  int *left, int *right, int *seeded)
{
    int rounded_start = record->start_y +
                        (record->y_fraction_low >= 0x8000u);
    int row;
    int changed = 0;
    if (record->top_clipped_rows != 0u) {
        row = rounded_start - (int)record->top_clipped_rows;
        if (y >= row && y < rounded_start) {
            /* helper3's clip postamble writes only the left array here. */
            *left = clip_left;
            *seeded = 1;
            changed = 1;
        }
    }
    if (record->left_clipped_rows != 0u) {
        row = rounded_start - (int)record->left_clipped_rows;
        if (y >= row && y < rounded_start) {
            /* The left-clip postamble writes only the right array. */
            *right = clip_right - 1;
            *seeded = 1;
            changed = 1;
        }
    }
    if (record->bottom_clipped_rows != 0u) {
        row = (int)record->end_y + 1;
        if (y >= row && y < row + (int)record->bottom_clipped_rows) {
            /* Bottom-clipped rows receive the same left-array assignment. */
            *left = clip_left;
            *seeded = 1;
            changed = 1;
        }
    }
    if (record->right_clipped_rows != 0u) {
        row = (int)record->end_y + 1;
        if (y >= row && y < row + (int)record->right_clipped_rows) {
            /* Right-clipped rows receive the right-array assignment only. */
            *right = clip_right - 1;
            *seeded = 1;
            changed = 1;
        }
    }
    return changed;
}

enum { RASTER_BOUND_ROWS = 480 };

/* Translate helper3's alternate-polygon mode-7 loop (case13).  In this
   descriptor mode AX walks left while DX carries advance the row pointer.
   The branch order matters: the opposite boundary is checked before DX is
   advanced, and the terminal no-carry branch does not store AX. */
static void raster_mode13_alt(const RasterLineRecord *record,
                              RasterInterval *rows,
                              unsigned char *seeded)
{
    unsigned cx = record->step_count;
    int ax = record->start_x;
    int y = record->start_y;
    uint32_t initial = (uint32_t)record->y_fraction_low + 0x8000u;
    uint16_t dx = (uint16_t)initial;

    if ((initial >> 16) != 0u)
        ++y;
    while (cx != 0u) {
        uint32_t sum;
        int carry;

        if (y >= 0 && y < RASTER_BOUND_ROWS && rows[y].right < ax) {
            /* branch_path_24: DI switches to the right array; STOSW writes
               and advances DI to the following row. */
            if (y >= 0 && y < RASTER_BOUND_ROWS) {
                rows[y].right = ax;
                seeded[y] = 1u;
            }
            ++y;
            for (;;) {
                --ax;
                sum = (uint32_t)dx + record->slope;
                carry = (int)(sum >> 16);
                dx = (uint16_t)sum;
                --cx;
                if (cx == 0u)
                    return;
                if (carry) {
                    /* Carry falls through to LOOP loop_head_25, whose
                       STOSW stores this sample before the next update. */
                    if (y >= 0 && y < RASTER_BOUND_ROWS) {
                        rows[y].right = ax;
                        seeded[y] = 1u;
                    }
                    ++y;
                }
                /* JNB on no-carry follows branch_path_25 back to
                   loop_head_26 and intentionally skips a store. */
            }
        }

        sum = (uint32_t)dx + record->slope;
        carry = (int)(sum >> 16);
        dx = (uint16_t)sum;
        if (!carry) {
            --ax;
            --cx;
            continue;
        }

        if (y >= 0 && y < RASTER_BOUND_ROWS && rows[y].left > ax) {
            /* branch_path_26 starts STOSW on the left array. */
            rows[y].left = ax;
            seeded[y] = 1u;
            ++y;
            --ax;
            --cx;
            while (cx != 0u) {
                sum = (uint32_t)dx + record->slope;
                carry = (int)(sum >> 16);
                dx = (uint16_t)sum;
                if (carry) {
                    if (y >= 0 && y < RASTER_BOUND_ROWS) {
                        rows[y].left = ax;
                        seeded[y] = 1u;
                    }
                    ++y;
                    --ax;
                    --cx;
                } else {
                    --ax;
                    --cx;
                    if (cx == 0u) {
                        /* branch_path_27's terminal INC AX / MOV [DI],AX. */
                        ++ax;
                        if (y >= 0 && y < RASTER_BOUND_ROWS) {
                            rows[y].left = ax;
                            seeded[y] = 1u;
                        }
                    }
                }
            }
            return;
        }

        /* branch_path_23: the carry advanced DI by one row; its terminal
           LOOP exit has no store. */
        ++y;
        --ax;
        --cx;
    }
}

/* Symmetric alternate-polygon mode-8 path (case14).  It walks AX rightward;
   the first-side comparison and its carry-controlled post-loop stores differ
   from case13, so it remains a separate transcription. */
static void raster_mode14_alt(const RasterLineRecord *record,
                              RasterInterval *rows,
                              unsigned char *seeded)
{
    unsigned cx = record->step_count;
    int ax = record->start_x;
    int y = record->start_y;
    uint32_t initial = (uint32_t)record->y_fraction_low + 0x8000u;
    uint16_t dx = (uint16_t)initial;

    if ((initial >> 16) != 0u)
        ++y;
    while (cx != 0u) {
        uint32_t sum;
        int carry;

        if (y >= 0 && y < RASTER_BOUND_ROWS && rows[y].left > ax) {
            /* The first cmp jumps directly to branch_path_31/STOSW. */
            rows[y].left = ax;
            seeded[y] = 1u;
            ++y;
            ++ax;
            --cx;
            while (cx != 0u) {
                sum = (uint32_t)dx + record->slope;
                carry = (int)(sum >> 16);
                dx = (uint16_t)sum;
                if (carry) {
                    if (y >= 0 && y < RASTER_BOUND_ROWS) {
                        rows[y].left = ax;
                        seeded[y] = 1u;
                    }
                    ++y;
                }
                ++ax;
                --cx;
            }
            return;
        }

        sum = (uint32_t)dx + record->slope;
        carry = (int)(sum >> 16);
        dx = (uint16_t)sum;
        if (!carry) {
            ++ax;
            --cx;
            continue;
        }
        if (y >= 0 && y < RASTER_BOUND_ROWS && rows[y].right < ax) {
            /* branch_path_29: switch to right array and MOV [DI],AX. */
            rows[y].right = ax;
            seeded[y] = 1u;
            while (cx != 0u) {
                ++ax;
                sum = (uint32_t)dx + record->slope;
                carry = (int)(sum >> 16);
                dx = (uint16_t)sum;
                if (carry) {
                    ++y;
                    --cx;
                    if (cx != 0u) {
                        if (y >= 0 && y < RASTER_BOUND_ROWS) {
                            rows[y].right = ax;
                            seeded[y] = 1u;
                        }
                    } else {
                        --ax;
                        if (y >= 0 && y < RASTER_BOUND_ROWS) {
                            rows[y].right = ax;
                            seeded[y] = 1u;
                        }
                    }
                } else {
                    --cx;
                    if (cx == 0u) {
                        ++y;
                        --ax;
                        if (y >= 0 && y < RASTER_BOUND_ROWS) {
                            rows[y].right = ax;
                            seeded[y] = 1u;
                        }
                    }
                }
            }
            return;
        }
        ++y;
        ++ax;
        --cx;
    }
}

static void raster_polygon(int16_t color, int16_t point_count,
                           const int16_t *points, RasterFillMode mode,
                           int merge_each_row)
{
    RasterPoint vertices[256];
    struct RasterPolyEdge {
        int vertex;
        int next;
        RasterLineRecord record;
    } edges[256];
    int edge_list[2][256];
    int edge_count = 0;
    int chain_count[2] = {0, 0};
    RasterInterval row_bounds[RASTER_BOUND_ROWS] = {{0, 0}};
    unsigned char row_seeded[RASTER_BOUND_ROWS] = {0};
    int8_t reverse_side[256] = {0};
    int min_y = INT_MAX, max_y = INT_MIN;
    int min_x = INT_MAX, max_x = INT_MIN;
    int min_vertex = 0, max_vertex = 0;
    int clip_left, clip_right, clip_top, clip_bottom;
    int clipped;
    int16_t index;
    uint16_t pattern = s_line_pattern_bits;
    int drew = 0;

    if (points == NULL || point_count <= 0 || point_count > 256 ||
        !sprite_raster_bounds(&clip_left, &clip_right, &clip_top, &clip_bottom))
        return;
    for (index = 0; index < point_count; ++index) {
        vertices[index].x = points[(size_t)index * 2u];
        vertices[index].y = points[(size_t)index * 2u + 1u];
        if (vertices[index].x < min_x) min_x = vertices[index].x;
        if (vertices[index].x > max_x) max_x = vertices[index].x;
        if (vertices[index].y <= min_y) { min_y = vertices[index].y; min_vertex = index; }
        if (vertices[index].y > max_y) { max_y = vertices[index].y; max_vertex = index; }
    }
    if (point_count == 1) {
        preRender_line(vertices[0].x, vertices[0].y, vertices[0].x,
                       vertices[0].y, color);
        return;
    }
    if (point_count == 2) {
        preRender_line((int16_t)vertices[0].x, (int16_t)vertices[0].y,
                       (int16_t)vertices[1].x, (int16_t)vertices[1].y, color);
        return;
    }

    /* Shared setup culls against rightmost-1, then selects clipped line
       descriptors globally when any input vertex exceeds that box. */
    if (max_x < clip_left || min_x >= clip_right - 1 ||
        max_y < clip_top || min_y >= clip_bottom)
        return;
    clipped = (max_x > clip_right - 1 || min_x < clip_left ||
               max_y >= clip_bottom || min_y < clip_top);
    {
        int first_y = min_y > clip_top ? min_y : clip_top;
        int last_y = max_y < clip_bottom - 1 ? max_y : clip_bottom - 1;
        int y;
        int chain;
        if (first_y > last_y) return;
        if (min_y == max_y || min_x == max_x) {
            preRender_line((int16_t)min_x, (int16_t)min_y,
                           (int16_t)max_x, (int16_t)max_y, color);
            return;
        }

        /* The DOS helper builds one descriptor per ascending edge, then
           processes every forward edge before entering the reverse helper. */
        for (chain = 0; chain < 2; ++chain) {
            int vertex = min_vertex;
            while (vertex != max_vertex) {
                int next = chain == 0 ? vertex + 1 : vertex - 1;
                if (next == point_count) next = 0;
                if (next < 0) next = point_count - 1;
                if (vertices[next].y > vertices[vertex].y) {
                    struct RasterPolyEdge *edge = &edges[edge_count];
                    edge->vertex = vertex;
                    edge->next = next;
                    memset(&edge->record, 0, sizeof(edge->record));
                    edge->record.color = (uint16_t)color;
                    (void)draw_line_record((uint16_t)vertices[vertex].x,
                                           (uint16_t)vertices[vertex].y,
                                           (uint16_t)vertices[next].x,
                                           (uint16_t)vertices[next].y,
                                           &edge->record, clipped);
                    edge_list[chain][chain_count[chain]++] = edge_count++;
                }
                vertex = next;
            }
        }

        if (mode != RASTER_FILL_SOLID && (first_y & 1) == 0)
            pattern = rotate_pattern_rows(pattern);

        /* helper2: collect the forward boundary for every scan row first. */
        for (y = first_y; y <= last_y; ++y) {
            int i;
            for (i = 0; i < chain_count[0]; ++i) {
                struct RasterPolyEdge *item = &edges[edge_list[0][i]];
                RasterInterval edge;
                int has_edge;
                int vertex = item->vertex;
                int next = item->next;
                if (clipped) {
                    has_edge = raster_record_interval(&item->record, y, &edge);
                    if (!has_edge)
                        has_edge = raster_record_prefill(&item->record, y,
                                          clip_left, clip_right, &edge);
                } else {
                    has_edge = raster_edge_interval(&vertices[vertex],
                                      &vertices[next], y, 0, &edge);
                }
                if (has_edge) {
                    row_bounds[y] = edge;
                    row_seeded[y] = 1u;
                }
            }
        }

        /* helper3: process reverse edges after all helper2 bounds exist.
           Alternate x-major modes use the original case13/case14 state
           machines, including their distinct initial and terminal stores. */
        for (chain = 0; chain < chain_count[1]; ++chain) {
            struct RasterPolyEdge *item = &edges[edge_list[1][chain]];
            RasterLineRecord *record = &item->record;
            int vertex = item->vertex;
            int next = item->next;
            if (!merge_each_row &&
                (record->mode == 7u || record->mode == 8u)) {
                if (record->mode == 7u)
                    raster_mode13_alt(record, row_bounds, row_seeded);
                else
                    raster_mode14_alt(record, row_bounds, row_seeded);
                if (clipped) {
                    for (y = first_y; y <= last_y; ++y) {
                        int seeded = row_seeded[y] != 0u;
                        (void)raster_record_postfill(record, y,
                            clip_left, clip_right, &row_bounds[y].left,
                            &row_bounds[y].right, &seeded);
                        row_seeded[y] = (unsigned char)seeded;
                    }
                }
                continue;
            }
            for (y = first_y; y <= last_y; ++y) {
                RasterInterval edge;
                int has_edge;
                if (clipped) {
                    has_edge = raster_record_interval(record, y, &edge);
                    if (!has_edge && !merge_each_row) {
                        int dx = vertices[next].x - vertices[vertex].x;
                        int dy = vertices[next].y - vertices[vertex].y;
                        if (record->mode == 8u && dx > dy &&
                            vertices[next].y < clip_bottom &&
                            raster_has_terminal_spill(dx, dy) &&
                            y == (int)record->end_y + 1) {
                            edge.left = edge.right = record->end_x;
                            has_edge = 1;
                        }
                    }
                } else {
                    int dx = vertices[next].x - vertices[vertex].x;
                    int dy = vertices[next].y - vertices[vertex].y;
                    has_edge = raster_edge_interval(&vertices[vertex],
                        &vertices[next], y,
                        !merge_each_row && reverse_side[vertex] > 0 &&
                        raster_has_terminal_spill(dx, dy), &edge);
                }
                if (has_edge) {
                    if (!row_seeded[y]) {
                        row_bounds[y] = edge;
                        row_seeded[y] = 1u;
                    } else if (merge_each_row) {
                        if (edge.left < row_bounds[y].left)
                            row_bounds[y].left = edge.left;
                        if (edge.right > row_bounds[y].right)
                            row_bounds[y].right = edge.right;
                    } else {
                        int dx = vertices[next].x - vertices[vertex].x;
                        int dy = vertices[next].y - vertices[vertex].y;
                        int right_first = dx < -dy;
                        int side = reverse_side[vertex];
                        if (side == 0) {
                            if (right_first && edge.right > row_bounds[y].right) side = 1;
                            else if (edge.left < row_bounds[y].left) side = -1;
                            else if (edge.right > row_bounds[y].right) side = 1;
                            reverse_side[vertex] = (int8_t)side;
                        }
                        if (side < 0) row_bounds[y].left = edge.left;
                        if (side > 0) row_bounds[y].right = edge.right;
                    }
                }
                if (clipped) {
                    int seeded = row_seeded[y] != 0u;
                    (void)raster_record_postfill(record, y, clip_left,
                        clip_right, &row_bounds[y].left,
                        &row_bounds[y].right, &seeded);
                    row_seeded[y] = (unsigned char)seeded;
                }
            }
        }

        for (y = first_y; y <= last_y; ++y) {
            if (row_seeded[y]) {
                fill_raster_span(row_bounds[y].left, y,
                                 row_bounds[y].right, (uint8_t)color,
                                 mode, &pattern);
                drew = 1;
            }
            if (mode != RASTER_FILL_SOLID)
                pattern = rotate_pattern_rows(pattern);
        }
    }
    s_line_pattern_bits = pattern;
    if (drew && s_sprite1.sprite_bitmapptr == &s_screen_shape)
        port_video_publish("preRender_polygon");
}

/* _preRender_default (seg012:217B2) and its alternate share
   _prerender_shared_setup; both select the filled-line callback and the
   clipped line callback. */
void preRender_default(int16_t color, int16_t point_count,
                       const int16_t *points)
{
    raster_polygon(color, point_count, points, RASTER_FILL_SOLID, 1);
}

void preRender_default_alt(int16_t color, int16_t point_count,
                           const int16_t *points)
{
    raster_polygon(color, point_count, points, RASTER_FILL_SOLID, 0);
}

void preRender_patterned(int16_t pattern_bits, int16_t color,
                         int16_t point_count, const int16_t *points)
{
    s_line_pattern_bits = (uint16_t)pattern_bits;
    raster_polygon(color, point_count, points, RASTER_FILL_PATTERN, 1);
}

void preRender_unk(int16_t pattern_bits, int16_t background_color,
                   int16_t foreground_color, int16_t point_count,
                   const int16_t *points)
{
    s_line_pattern_bits = (uint16_t)pattern_bits;
    s_line_auxiliary_color = (uint8_t)foreground_color;
    raster_polygon(background_color, point_count, points,
                   RASTER_FILL_AUXILIARY, 1);
}

static int16_t raster_shift_right_signed(int16_t value, unsigned count)
{
    int divisor = 1 << count;
    int integer = value;
    if (integer >= 0)
        return (int16_t)(integer / divisor);
    return (int16_t)(-((-integer + divisor - 1) / divisor));
}

static void sphere_build_ring(const int16_t source[6], int16_t output[64])
{
    int16_t start_x = (int16_t)(source[2] - source[0]);
    int16_t start_y = (int16_t)(source[3] - source[1]);
    int16_t end_x = (int16_t)(source[4] - source[0]);
    int16_t end_y = (int16_t)(source[5] - source[1]);
    int16_t start_half_x = raster_shift_right_signed(start_x, 1);
    int16_t start_half_y = raster_shift_right_signed(start_y, 1);
    int16_t start_quarter_x = raster_shift_right_signed(start_half_x, 1);
    int16_t start_quarter_y = raster_shift_right_signed(start_half_y, 1);
    int16_t start_three_quarters_x =
        (int16_t)(start_half_x + start_quarter_x);
    int16_t start_three_quarters_y =
        (int16_t)(start_half_y + start_quarter_y);
    int16_t end_half_x = raster_shift_right_signed(end_x, 1);
    int16_t end_half_y = raster_shift_right_signed(end_y, 1);
    int16_t end_quarter_x = raster_shift_right_signed(end_half_x, 1);
    int16_t end_quarter_y = raster_shift_right_signed(end_half_y, 1);
    int16_t end_three_quarters_x = (int16_t)(end_half_x + end_quarter_x);
    int16_t end_three_quarters_y = (int16_t)(end_half_y + end_quarter_y);
    int index;

#define SPHERE_STORE_POINT(i, px, py) do { \
        output[(i) * 2] = (px); output[(i) * 2 + 1] = (py); \
    } while (0)
    SPHERE_STORE_POINT(0, start_x, start_y);
    SPHERE_STORE_POINT(8, end_x, end_y);
    SPHERE_STORE_POINT(4, mulscl((int16_t)(start_x + end_x), 0x2d41),
                       mulscl((int16_t)(start_y + end_y), 0x2d41));
    SPHERE_STORE_POINT(2, mulscl((int16_t)(start_x + end_half_x), 0x393e),
                       mulscl((int16_t)(start_y + end_half_y), 0x393e));
    SPHERE_STORE_POINT(6, mulscl((int16_t)(end_x + start_half_x), 0x393e),
                       mulscl((int16_t)(end_y + start_half_y), 0x393e));
    SPHERE_STORE_POINT(1, mulscl((int16_t)(start_x + end_quarter_x), 0x3e17),
                       mulscl((int16_t)(start_y + end_quarter_y), 0x3e17));
    SPHERE_STORE_POINT(7, mulscl((int16_t)(end_x + start_quarter_x), 0x3e17),
                       mulscl((int16_t)(end_y + start_quarter_y), 0x3e17));
    SPHERE_STORE_POINT(3, mulscl((int16_t)(start_x + end_three_quarters_x), 0x3333),
                       mulscl((int16_t)(start_y + end_three_quarters_y), 0x3333));
    SPHERE_STORE_POINT(5, mulscl((int16_t)(end_x + start_three_quarters_x), 0x3333),
                       mulscl((int16_t)(end_y + start_three_quarters_y), 0x3333));
    SPHERE_STORE_POINT(12, mulscl((int16_t)(end_x - start_x), 0x2d41),
                        mulscl((int16_t)(end_y - start_y), 0x2d41));
    SPHERE_STORE_POINT(14, mulscl((int16_t)(end_half_x - start_x), 0x393e),
                        mulscl((int16_t)(end_half_y - start_y), 0x393e));
    SPHERE_STORE_POINT(10, mulscl((int16_t)(end_x - start_half_x), 0x393e),
                        mulscl((int16_t)(end_y - start_half_y), 0x393e));
    SPHERE_STORE_POINT(15, mulscl((int16_t)(end_quarter_x - start_x), 0x3e17),
                        mulscl((int16_t)(end_quarter_y - start_y), 0x3e17));
    SPHERE_STORE_POINT(9, mulscl((int16_t)(end_x - start_quarter_x), 0x3e17),
                       mulscl((int16_t)(end_y - start_quarter_y), 0x3e17));
    SPHERE_STORE_POINT(13, mulscl((int16_t)(end_three_quarters_x - start_x), 0x3333),
                        mulscl((int16_t)(end_three_quarters_y - start_y), 0x3333));
    SPHERE_STORE_POINT(11, mulscl((int16_t)(end_x - start_three_quarters_x), 0x3333),
                        mulscl((int16_t)(end_y - start_three_quarters_y), 0x3333));

    for (index = 0; index < 16; ++index) {
        output[(index + 16) * 2] = (int16_t)(source[0] - output[index * 2]);
        output[(index + 16) * 2 + 1] =
            (int16_t)(source[1] - output[index * 2 + 1]);
        output[index * 2] = (int16_t)(source[0] + output[index * 2]);
        output[index * 2 + 1] = (int16_t)(source[1] + output[index * 2 + 1]);
    }
#undef SPHERE_STORE_POINT
}

/* Port of asm/vector_sphere_sprite_ops.ASM:_preRender_sphere. The 39 small
   sphere profiles and the large-sphere 32-point fallback are anchored by the
   corresponding DATA table and preRender_sphere_helper2 C translation. */
void preRender_sphere(int16_t center_x, int16_t center_y, int16_t size,
                      int16_t color)
{
    uint16_t raw_size = (uint16_t)size;
    uint16_t quarter = raw_size >> 2;
    uint16_t diameter = (uint16_t)(raw_size - quarter + (quarter >> 2));
    int signed_diameter = (int16_t)diameter;
    unsigned radius;
    int clip_left;
    int clip_right;
    int clip_top;
    int clip_bottom;
    int drew = 0;

    if (signed_diameter <= 0 ||
        !sprite_raster_bounds(&clip_left, &clip_right, &clip_top,
                              &clip_bottom))
        return;
    if (signed_diameter == 1) {
        port_sprite_plot_active(center_x, center_y, (uint8_t)color);
        if (s_sprite1.sprite_bitmapptr == &s_screen_shape)
            port_video_publish("preRender_sphere");
        return;
    }
    radius = (unsigned)diameter - (unsigned)(diameter >> 1);
    if (radius < 40u) {
        size_t profile_offset = (size_t)(radius - 1u) * radius / 2u;
        unsigned row_count = radius;
        int top_y = (int)center_y - (int)(diameter >> 1);
        unsigned row;
        if (profile_offset + row_count > sizeof(s_sphere_scanline_profiles))
            port_guest_unwind("sphere scanline profile exceeded its table");
        for (row = 0; row < row_count; ++row) {
            int x_radius = s_sphere_scanline_profiles[profile_offset + row];
            int left = (int)center_x - x_radius;
            int right = (int)center_x + x_radius;
            int upper_y = top_y + (int)row;
            int lower_y = top_y + (signed_diameter - 1 - (int)row);
            fill_raster_span(left, upper_y, right, (uint8_t)color,
                             RASTER_FILL_SOLID, &s_line_pattern_bits);
            if (lower_y != upper_y)
                fill_raster_span(left, lower_y, right, (uint8_t)color,
                                 RASTER_FILL_SOLID, &s_line_pattern_bits);
            drew = 1;
        }
    } else {
        int16_t source[6];
        int16_t points[64];
        source[0] = center_x;
        source[1] = center_y;
        source[2] = center_x;
        source[3] = (int16_t)(center_y + (signed_diameter >> 1));
        source[4] = (int16_t)(center_x + (int)(raw_size >> 1));
        source[5] = center_y;
        sphere_build_ring(source, points);
        raster_polygon(color, 32, points, RASTER_FILL_SOLID, 1);
        return;
    }
    if (drew && s_sprite1.sprite_bitmapptr == &s_screen_shape)
        port_video_publish("preRender_sphere");
}



static uint32_t raster_line_fixed(int16_t whole, uint16_t fraction)
{
    return ((uint32_t)(uint16_t)whole << 16) | fraction;
}

static void raster_line_store_fixed(int16_t *whole, uint16_t *fraction,
                                    uint32_t fixed)
{
    *fraction = (uint16_t)fixed;
    *whole = (int16_t)(uint16_t)(fixed >> 16);
}

static int16_t raster_line_round_fixed(int16_t whole, uint16_t fraction)
{
    return (int16_t)(uint16_t)(whole + (fraction >= 0x8000u));
}

static uint32_t raster_line_div_round(uint32_t numerator, uint16_t slope)
{
    uint32_t quotient;
    uint32_t remainder;
    if (slope == 0u)
        return 0u;
    quotient = numerator / slope;
    remainder = numerator % slope;
    if (remainder > (uint32_t)(slope >> 1))
        ++quotient;
    return quotient;
}

static unsigned draw_line_record(unsigned x0_raw, unsigned y0_raw,
                                 unsigned x1_raw, unsigned y1_raw,
                                 void *record_pointer,
                                 int clipping_enabled)
{
    RasterLineRecord *record = (RasterLineRecord *)record_pointer;
    int left;
    int right;
    int top;
    int bottom;
    int start_x = (int16_t)(uint16_t)x0_raw;
    int start_y = (int16_t)(uint16_t)y0_raw;
    int end_x = (int16_t)(uint16_t)x1_raw;
    int end_y = (int16_t)(uint16_t)y1_raw;
    int dx;
    int dy;
    unsigned mode;
    unsigned major;
    unsigned minor;
    uint32_t slope;
    unsigned flags;
    unsigned start_code;
    unsigned end_code;

    if (record == NULL || !sprite_raster_bounds(&left, &right, &top, &bottom))
        return 1u;
    /* _preRender_line stores its color at descriptor offset 16 before it
       calls this routine; the DOS clipper deliberately preserves that word. */
    record->x_fraction_low = 0;
    record->y_fraction_low = 0;
    record->slope = 0;
    record->step_count = 0;
    record->top_clipped_rows = 0;
    record->bottom_clipped_rows = 0;
    record->left_clipped_rows = 0;
    record->right_clipped_rows = 0;
    record->mode = 0xffu;
    if (start_y > end_y) {
        int swap = start_x; start_x = end_x; end_x = swap;
        swap = start_y; start_y = end_y; end_y = swap;
    }
    record->start_x = (int16_t)start_x;
    record->start_y = (int16_t)start_y;
    record->end_x = (int16_t)end_x;
    record->end_y = (int16_t)end_y;
    dx = end_x - start_x;
    dy = end_y - start_y;

    /* The assembly rejects a common outcode before measuring the slope. In
       particular this leaves mode 0xff and the step word zero. */
    if (dy != 0 && clipping_enabled) {
        unsigned first = 0u;
        unsigned last = 0u;
        unsigned common;
        if (start_y < top) first |= 4u;
        else if (start_y >= bottom) {
            record->clip_flags = 8u;
            record->start_y = (int16_t)bottom;
            record->x_fraction_low = 0;
            return 8u;
        }
        if (end_y < top) last |= 4u;
        else if (end_y >= bottom) last |= 8u;
        if (start_x < left) first |= 2u;
        else if (start_x >= right) first |= 1u;
        if (end_x < left) last |= 2u;
        else if (end_x >= right) last |= 1u;
        common = first & last;
        if (common != 0u) {
            record->clip_flags = (uint8_t)common;
            if (common & 4u) {
                record->start_y = (int16_t)top;
                record->x_fraction_low = 0;
                record->end_y = (int16_t)(top - 1);
                return common;
            }
            if (common & 8u) {
                record->start_y = (int16_t)bottom;
                record->x_fraction_low = 0;
                return common;
            }
            {
                int clipped_end = end_y >= bottom ? bottom - 1 : end_y;
                int rounded_start = start_y +
                    (record->y_fraction_low >= 0x8000u);
                int count;
                if (rounded_start < top) rounded_start = top;
                record->start_y = (int16_t)rounded_start;
                record->x_fraction_low = 0;
                record->end_y = (int16_t)(rounded_start - 1);
                count = clipped_end - rounded_start + 1;
                if (common & 2u)
                    record->bottom_clipped_rows = (uint16_t)count;
                else
                    record->right_clipped_rows = (uint16_t)count;
                return common;
            }
        }
    }
    if (dy == 0) {
        if (dx < 0) {
            int16_t swap = record->start_x;
            record->start_x = record->end_x;
            record->end_x = swap;
            dx = -dx;
            mode = 0u;
        } else {
            mode = dx == 0 ? 9u : 1u;
        }
        major = (unsigned)dx;
        minor = 0;
    } else {
        int abs_dx = dx < 0 ? -dx : dx;
        unsigned abs_dy = (unsigned)dy;
        if (dx == 0) {
            mode = 2u;
            major = abs_dy;
            minor = 0;
        } else if (dx > 0) {
            if ((unsigned)abs_dx < abs_dy) {
                mode = 6u;
                major = abs_dy;
                minor = (unsigned)abs_dx;
            } else if ((unsigned)abs_dx == abs_dy) {
                mode = 4u;
                major = abs_dy;
                minor = 0;
            } else {
                mode = 8u;
                major = (unsigned)abs_dx;
                minor = abs_dy;
            }
        } else if ((unsigned)abs_dx < abs_dy) {
            mode = 5u;
            major = abs_dy;
            minor = (unsigned)abs_dx;
        } else if ((unsigned)abs_dx == abs_dy) {
            mode = 3u;
            major = abs_dy;
            minor = 0;
        } else {
            mode = 7u;
            major = (unsigned)abs_dx;
            minor = abs_dy;
        }
    }
    record->mode = (uint8_t)mode;
    record->step_count = (uint16_t)(major + 1u);
    if (major != 0u && minor != 0u) {
        uint64_t fixed = (uint64_t)minor << 16;
        slope = (uint32_t)(fixed / major);
        if (major >= 50u && fixed % major > major / 2u)
            ++slope;
        record->slope = (uint16_t)slope;
    }
    if (!clipping_enabled)
        return 0u;

    /* The DOS clipper uses endpoint outcodes, then adjusts the descriptor in
       fixed point. Keep its half-open bounds and path-specific counters here;
       drawing consumes this record, so geometric host clipping loses samples. */
    start_code = end_code = 0u;
    if (start_y < top) start_code |= 4u;
    else if (start_y >= bottom) start_code |= 8u;
    if (end_y < top) end_code |= 4u;
    else if (end_y >= bottom) end_code |= 8u;
    if (start_x < left) start_code |= 2u;
    else if (start_x >= right) start_code |= 1u;
    if (end_x < left) end_code |= 2u;
    else if (end_x >= right) end_code |= 1u;
    flags = start_code | end_code;

    if (dy == 0) {
        int sx = record->start_x;
        int ex = record->end_x;
        int count = ex - sx + 1;
        if (start_y < top) {
            record->start_y = (int16_t)top;
            record->end_y = (int16_t)top;
            record->step_count = 0;
            record->clip_flags = 4u;
            return 4u;
        }
        if (start_y >= bottom) {
            record->start_y = (int16_t)bottom;
            record->end_y = (int16_t)bottom;
            record->step_count = 0;
            record->clip_flags = 8u;
            return 8u;
        }
        record->step_count = (uint16_t)count;
        if (ex < left) {
            record->end_y = (int16_t)(record->end_y - 1);
            record->bottom_clipped_rows = 1u;
            record->clip_flags = 2u;
            return 2u;
        }
        if (sx >= right) {
            record->end_y = (int16_t)(record->end_y - 1);
            record->right_clipped_rows = 1u;
            record->clip_flags = 1u;
            return 1u;
        }
        if (sx < left) {
            int amount = left - sx;
            record->start_x = (int16_t)left;
            record->step_count = (uint16_t)(record->step_count - amount);
        }
        if (ex >= right) {
            int amount = ex - (right - 1);
            record->end_x = (int16_t)(right - 1);
            record->step_count = (uint16_t)(record->step_count - amount);
        }
        return 0u;
    }

    /* A shared outcode is the ASM reject path. The caller receives the
       surviving clip flag and a degenerate edge descriptor. */
    if ((start_code & end_code) != 0u) {
        unsigned clip = start_code & end_code;
        record->clip_flags = (uint8_t)clip;
        record->step_count = 0;
        if (clip & 4u) {
            record->start_y = (int16_t)top;
            record->x_fraction_low = 0;
            record->end_y = (int16_t)(top - 1);
        } else if (clip & 8u) {
            record->start_y = (int16_t)bottom;
            record->x_fraction_low = 0;
        } else {
            int end_row = record->end_y;
            int start_row = record->start_y;
            int rounded_start = start_row +
                ((record->y_fraction_low + 0x8000u) > 0xffffu);
            if (end_row >= bottom) end_row = bottom - 1;
            if (rounded_start < top) rounded_start = top;
            record->start_y = (int16_t)rounded_start;
            record->x_fraction_low = 0;
            record->end_y = (int16_t)(rounded_start - 1);
            record->step_count = (uint16_t)(end_row - rounded_start);
            if (clip & 2u) record->bottom_clipped_rows += record->step_count;
            else record->right_clipped_rows += record->step_count;
        }
        return clip;
    }
    if (flags == 0u)
        return 0u;

    {
        int old_start_y = record->start_y;
        int old_end_y = record->end_y;
        uint32_t fixed;
        uint32_t amount;
        uint32_t next;

        /* Top edge (table entries 4..7 and 12..15). */
        if ((flags & 4u) != 0u) {
            amount = (uint32_t)(top - record->start_y);
            record->start_y = (int16_t)top;
            switch (mode) {
            case 2u:
                record->step_count = (uint16_t)(record->step_count - amount);
                break;
            case 3u:
                record->start_x = (int16_t)(record->start_x - (int)amount);
                record->step_count = (uint16_t)(record->step_count - amount);
                break;
            case 4u:
                record->start_x = (int16_t)(record->start_x + (int)amount);
                record->step_count = (uint16_t)(record->step_count - amount);
                break;
            case 5u:
            case 6u:
                fixed = raster_line_fixed(record->start_x,
                                          record->x_fraction_low);
                next = (uint32_t)((uint64_t)record->slope * amount);
                fixed = mode == 5u ? fixed - next : fixed + next;
                raster_line_store_fixed(&record->start_x,
                                        &record->x_fraction_low, fixed);
                record->step_count = (uint16_t)(record->step_count - amount);
                break;
            case 7u:
            case 8u:
                amount = raster_line_div_round(amount << 16,
                                                record->slope);
                record->start_x = (int16_t)(record->start_x +
                    (mode == 7u ? -(int)amount : (int)amount));
                record->step_count = (uint16_t)(record->step_count - amount);
                if (record->step_count == 0u ||
                    (int16_t)record->step_count < 0) {
                    record->step_count = 1u;
                    record->start_y = (int16_t)top;
                    record->start_x = record->end_x;
                } else {
                    fixed = raster_line_fixed((int16_t)old_start_y,
                                              record->y_fraction_low);
                    next = (uint32_t)((uint64_t)record->slope * amount);
                    fixed += next;
                    raster_line_store_fixed(&record->start_y,
                                            &record->y_fraction_low, fixed);
                }
                break;
            default:
                break;
            }

            /* Original path continues into the bottom handler if both y
               endpoints were outside, otherwise it rechecks only y. */
            if ((flags & 8u) != 0u) {
                flags = 8u;
                goto line_clip_bottom;
            }
        }

        /* Bottom edge (table entries 8..11). */
        if ((flags & 8u) != 0u) {
line_clip_bottom:
            amount = (uint32_t)(record->end_y - (bottom - 1));
            record->end_y = (int16_t)(bottom - 1);
            switch (mode) {
            case 2u:
                record->step_count = (uint16_t)(record->step_count - amount);
                break;
            case 3u:
                record->end_x = (int16_t)(record->end_x + (int)amount);
                record->step_count = (uint16_t)(record->step_count - amount);
                break;
            case 4u:
                record->end_x = (int16_t)(record->end_x - (int)amount);
                record->step_count = (uint16_t)(record->step_count - amount);
                break;
            case 5u:
            case 6u:
                record->step_count = (uint16_t)(record->step_count - amount);
                fixed = raster_line_fixed(record->start_x,
                                          record->x_fraction_low);
                next = (uint32_t)((uint64_t)record->slope *
                                  (record->step_count - 1u));
                fixed = mode == 5u ? fixed - next : fixed + next;
                record->end_x = raster_line_round_fixed(
                    (int16_t)(fixed >> 16), (uint16_t)fixed);
                break;
            case 7u:
            case 8u:
                fixed = raster_line_fixed(record->start_y,
                                          record->y_fraction_low);
                next = ((uint32_t)(uint16_t)(bottom - 1) << 16) - fixed;
                amount = raster_line_div_round(next, record->slope);
                record->end_x = (int16_t)(record->start_x +
                    (mode == 7u ? -(int)amount : (int)amount));
                record->step_count = (uint16_t)(amount + 1u);
                break;
            default:
                break;
            }
        }

        old_end_y = record->end_y;

        /* After top/bottom adjustment the original routine recomputes both
           X outcodes from the fixed-point start and integer end, then loops
           through the X clip dispatch. */
        if (((start_code | end_code) & 12u) != 0u) {
            unsigned first_x = 0u;
            unsigned last_x = 0u;
            unsigned common_x;
            int rounded_start_x = record->start_x +
                (record->x_fraction_low >= 0x8000u);
            if (record->start_x < left) first_x |= 2u;
            if (rounded_start_x >= right) first_x |= 1u;
            if (record->end_x < left) last_x |= 2u;
            else if (record->end_x >= right) last_x |= 1u;
            common_x = first_x & last_x;
            if (common_x != 0u) {
                int start_row = record->start_y +
                    (record->y_fraction_low >= 0x8000u);
                int end_row = record->end_y;
                int count;
                record->clip_flags = (uint8_t)common_x;
                record->step_count = 0;
                if (end_row >= bottom) end_row = bottom - 1;
                if (start_row < top) start_row = top;
                record->start_y = (int16_t)start_row;
                record->y_fraction_low = 0;
                record->end_y = (int16_t)(start_row - 1);
                count = end_row - start_row + 1;
                if (common_x & 2u)
                    record->bottom_clipped_rows = (uint16_t)count;
                else
                    record->right_clipped_rows = (uint16_t)count;
                return common_x;
            }
            flags = first_x | last_x;
            if (flags == 0u)
                return 0u;
        }

        /* Left edge. This is reached for outcode 2 or 3; mode 2 falls back
           to the same degenerate descriptor as the DOS clip initializer. */
        if ((flags & 2u) != 0u && (flags & 4u) == 0u &&
            (flags & 8u) == 0u) {
            int edge = left;
            switch (mode) {
            case 2u:
                record->clip_flags = 2u;
                record->step_count = 0;
                record->start_y = (int16_t)top;
                record->y_fraction_low = 0;
                record->end_y = (int16_t)(top - 1);
                return 2u;
            case 3u: {
                int delta = edge - record->end_x;
                record->end_x = (int16_t)edge;
                record->bottom_clipped_rows =
                    (uint16_t)(record->bottom_clipped_rows + delta);
                record->step_count = (uint16_t)(record->step_count - delta);
                record->end_y = (int16_t)(record->end_y - delta);
                break;
            }
            case 4u: {
                int delta = edge - record->start_x;
                record->start_x = (int16_t)edge;
                record->top_clipped_rows =
                    (uint16_t)(record->top_clipped_rows + delta);
                record->start_y = (int16_t)(record->start_y + delta);
                record->step_count = (uint16_t)(record->step_count - delta);
                break;
            }
            case 5u: {
                fixed = raster_line_fixed(record->start_x,
                                          record->x_fraction_low) -
                        ((uint32_t)(uint16_t)edge << 16);
                amount = raster_line_div_round(fixed, record->slope);
                record->end_x = (int16_t)edge;
                record->bottom_clipped_rows = (uint16_t)(
                    record->bottom_clipped_rows +
                    record->step_count - (amount + 1u));
                record->step_count = (uint16_t)(amount + 1u);
                record->end_y = (int16_t)(record->start_y + amount);
                break;
            }
            case 6u: {
                uint32_t xfixed = raster_line_fixed(
                    record->start_x, record->x_fraction_low);
                uint32_t distance = ((uint32_t)(uint16_t)edge << 16) - xfixed;
                amount = raster_line_div_round(distance, record->slope);
                fixed = xfixed + (uint32_t)((uint64_t)record->slope * amount);
                raster_line_store_fixed(&record->start_x,
                                        &record->x_fraction_low, fixed);
                record->start_y = (int16_t)(record->start_y + amount);
                record->top_clipped_rows = (uint16_t)(
                    record->top_clipped_rows + amount);
                record->step_count = (uint16_t)(record->step_count - amount);
                break;
            }
            case 7u: {
                int delta = record->start_x - edge;
                int previous_end = record->end_y;
                fixed = raster_line_fixed(record->start_y,
                                          record->y_fraction_low) +
                        (uint32_t)((uint64_t)record->slope * delta);
                record->end_x = (int16_t)edge;
                record->end_y = raster_line_round_fixed(
                    (int16_t)(fixed >> 16), (uint16_t)fixed);
                record->bottom_clipped_rows = (uint16_t)(
                    record->bottom_clipped_rows +
                    previous_end - record->end_y);
                record->step_count = (uint16_t)(delta + 1);
                break;
            }
            case 8u: {
                int delta = edge - record->start_x;
                int16_t old_rounded = raster_line_round_fixed(
                    record->start_y, record->y_fraction_low);
                fixed = raster_line_fixed(record->start_y,
                                          record->y_fraction_low) +
                        (uint32_t)((uint64_t)record->slope * delta);
                raster_line_store_fixed(&record->start_y,
                                        &record->y_fraction_low, fixed);
                record->start_x = (int16_t)edge;
                record->top_clipped_rows = (uint16_t)(
                    record->top_clipped_rows +
                    raster_line_round_fixed(record->start_y,
                                            record->y_fraction_low) - old_rounded);
                record->step_count = (uint16_t)(record->step_count - delta);
                break;
            }
            default:
                break;
            }
            if ((flags & 1u) != 0u) {
                flags = 1u;
                goto line_clip_right;
            }
            return 0u;
        }

        /* Right edge (outcode 1). */
        if ((flags & 1u) != 0u && (flags & (4u | 8u)) == 0u) {
line_clip_right:
            {
                int edge = right - 1;
                switch (mode) {
                case 2u:
                    record->clip_flags = 1u;
                    record->step_count = 0;
                    record->start_y = (int16_t)top;
                    record->y_fraction_low = 0;
                    record->end_y = (int16_t)(top - 1);
                    return 1u;
                case 3u: {
                    int delta = record->start_x - edge;
                    record->start_x = (int16_t)edge;
                    record->start_y = (int16_t)(record->start_y + delta);
                    record->left_clipped_rows = (uint16_t)(
                        record->left_clipped_rows + delta);
                    record->step_count = (uint16_t)(record->step_count - delta);
                    break;
                }
                case 4u: {
                    int delta = record->end_x - edge;
                    record->end_x = (int16_t)edge;
                    record->right_clipped_rows = (uint16_t)(
                        record->right_clipped_rows + delta);
                    record->step_count = (uint16_t)(record->step_count - delta);
                    record->end_y = (int16_t)(record->end_y - delta);
                    break;
                }
                case 5u: {
                    fixed = raster_line_fixed(record->start_x,
                                              record->x_fraction_low) -
                            ((uint32_t)(uint16_t)edge << 16);
                    amount = raster_line_div_round(fixed, record->slope);
                    if (amount >= record->step_count) {
                        record->clip_flags = 1u;
                        record->step_count = 0;
                        record->start_y = (int16_t)top;
                        record->y_fraction_low = 0;
                        record->end_y = (int16_t)(top - 1);
                        return 1u;
                    }
                    fixed = raster_line_fixed(record->start_x,
                                              record->x_fraction_low) -
                            (uint32_t)((uint64_t)record->slope * amount);
                    raster_line_store_fixed(&record->start_x,
                                            &record->x_fraction_low, fixed);
                    record->start_y = (int16_t)(record->start_y + amount);
                    record->left_clipped_rows = (uint16_t)(
                        record->left_clipped_rows + amount);
                    record->step_count = (uint16_t)(record->step_count - amount);
                    break;
                }
                case 6u: {
                    fixed = raster_line_fixed(record->start_x,
                                              record->x_fraction_low);
                    amount = (uint32_t)(edge -
                        raster_line_round_fixed(record->start_x,
                                                record->x_fraction_low));
                    if (fixed > ((uint32_t)(uint16_t)edge << 16)) {
                        record->clip_flags = 1u;
                        record->step_count = 0;
                        record->start_y = (int16_t)top;
                        record->y_fraction_low = 0;
                        record->end_y = (int16_t)(top - 1);
                        return 1u;
                    }
                    amount = raster_line_div_round(
                        ((uint32_t)(uint16_t)edge << 16) - fixed,
                        record->slope);
                    if (amount >= record->step_count) {
                        record->clip_flags = 1u;
                        record->step_count = 0;
                        record->start_y = (int16_t)top;
                        record->y_fraction_low = 0;
                        record->end_y = (int16_t)(top - 1);
                        return 1u;
                    }
                    record->end_x = (int16_t)edge;
                    record->end_y = (int16_t)(record->start_y + amount);
                    record->right_clipped_rows = (uint16_t)(
                        record->right_clipped_rows + old_end_y - record->end_y);
                    record->step_count = (uint16_t)(amount + 1u);
                    break;
                }
                case 7u: {
                    int delta = record->start_x - edge;
                    int16_t old_rounded = raster_line_round_fixed(
                        record->start_y, record->y_fraction_low);
                    fixed = raster_line_fixed(record->start_y,
                                              record->y_fraction_low) +
                            (uint32_t)((uint64_t)record->slope * delta);
                    raster_line_store_fixed(&record->start_y,
                                            &record->y_fraction_low, fixed);
                    record->start_x = (int16_t)edge;
                    record->left_clipped_rows = (uint16_t)(
                        record->left_clipped_rows +
                        raster_line_round_fixed(record->start_y,
                                                record->y_fraction_low) - old_rounded);
                    record->step_count = (uint16_t)(record->step_count - delta);
                    break;
                }
                case 8u: {
                    int delta = edge + 1 - record->start_x;
                    fixed = raster_line_fixed(record->start_y,
                                              record->y_fraction_low) +
                            (uint32_t)((uint64_t)record->slope *
                                       (delta - 1));
                    record->end_x = (int16_t)edge;
                    record->end_y = raster_line_round_fixed(
                        (int16_t)(fixed >> 16), (uint16_t)fixed);
                    record->right_clipped_rows = (uint16_t)(
                        record->right_clipped_rows + old_end_y - record->end_y);
                    record->step_count = (uint16_t)delta;
                    break;
                }
                default:
                    break;
                }
            }
            return 0u;
        }

        /* Vertical reclip after a corner crossing. Preserve the integer and
           half-word phases just as the assembly recomputation does. */
        {
            int sy = raster_line_round_fixed(record->start_y,
                                             record->y_fraction_low);
            unsigned start_v = sy < top ? 2u : sy >= bottom ? 1u : 0u;
            unsigned end_v = record->end_y < top ? 2u :
                             record->end_y >= bottom ? 1u : 0u;
            if ((start_v & end_v) != 0u) {
                unsigned clip = start_v & end_v;
                record->clip_flags = (uint8_t)(clip == 2u ? 4u : 8u);
                record->step_count = 0;
                if (clip == 2u) {
                    record->start_y = (int16_t)top;
                    record->x_fraction_low = 0;
                    record->end_y = (int16_t)(top - 1);
                    return 4u;
                }
                record->start_y = (int16_t)bottom;
                record->x_fraction_low = 0;
                return 8u;
            }
            if (start_v != 0u || end_v != 0u) {
                unsigned vertical_flags =
                    (start_v == 2u || end_v == 2u ? 4u : 0u) |
                    (start_v == 1u || end_v == 1u ? 8u : 0u);
                if ((vertical_flags & 4u) != 0u) {
                    amount = (uint32_t)(top - record->start_y);
                    record->start_y = (int16_t)top;
                    if (mode == 2u || mode == 3u || mode == 4u ||
                        mode == 5u || mode == 6u)
                        record->step_count = (uint16_t)(record->step_count - amount);
                } else {
                    flags = vertical_flags;
                    goto line_clip_bottom;
                }
            }
        }
        return 0u;
    }
}

/* Source callers pass 16-bit coordinates in the DOS ABI. Host `unsigned`
   arguments are 32-bit, so only the low word is interpreted as a coordinate. */
unsigned draw_line_related(unsigned x0, unsigned y0, unsigned x1,
                           unsigned y1, int *record)
{
    return draw_line_record(x0, y0, x1, y1, record, 1);
}

unsigned draw_line_related_alt(unsigned x0, unsigned y0, unsigned x1,
                               unsigned y1, int *record)
{
    return draw_line_record(x0, y0, x1, y1, record, 0);
}

static void raster_line_draw_clipped(int16_t x1, int16_t y1,
                                     int16_t x2, int16_t y2,
                                     int16_t color)
{
    RasterLineRecord record;
    uint32_t xround;
    uint32_t yround;
    uint16_t xfrac;
    uint16_t yfrac;
    int x;
    int y;
    unsigned count;
    unsigned step;
    if (sprite_pixels(&s_sprite1, NULL) == NULL || s_sprite1.lineofs == NULL)
        return;
    memset(&record, 0, sizeof(record));
    record.color = (uint16_t)color;
    if (draw_line_record((uint16_t)x1, (uint16_t)y1,
                         (uint16_t)x2, (uint16_t)y2, &record, 1) != 0u ||
        record.step_count == 0u)
        return;

    xround = (uint32_t)record.x_fraction_low + 0x8000u;
    yround = (uint32_t)record.y_fraction_low + 0x8000u;
    x = record.start_x + (int)(xround >> 16);
    y = record.start_y + (int)(yround >> 16);
    xfrac = (uint16_t)xround;
    yfrac = (uint16_t)yround;
    count = record.step_count;

    switch (record.mode) {
    case 0u:
    case 1u:
        for (step = 0; step < count; ++step)
            port_sprite_plot_active((int16_t)(x + (int)step), (int16_t)y,
                                    (uint8_t)record.color);
        break;
    case 2u:
        for (step = 0; step < count; ++step)
            port_sprite_plot_active((int16_t)x,
                                    (int16_t)(y + (int)step),
                                    (uint8_t)record.color);
        break;
    case 3u:
    case 4u:
        for (step = 0; step < count; ++step) {
            port_sprite_plot_active((int16_t)x, (int16_t)(y + (int)step),
                                    (uint8_t)record.color);
            x += record.mode == 3u ? -1 : 1;
        }
        break;
    case 5u:
    case 6u:
        for (step = 0; step < count; ++step) {
            port_sprite_plot_active((int16_t)x, (int16_t)(y + (int)step),
                                    (uint8_t)record.color);
            if (record.mode == 5u) {
                if (xfrac < record.slope)
                    --x;
                xfrac = (uint16_t)(xfrac - record.slope);
            } else {
                uint32_t sum = (uint32_t)xfrac + record.slope;
                if (sum > UINT16_MAX)
                    ++x;
                xfrac = (uint16_t)sum;
            }
        }
        break;
    case 7u:
    case 8u:
        for (step = 0; step < count; ++step) {
            port_sprite_plot_active((int16_t)x, (int16_t)y,
                                    (uint8_t)record.color);
            x += record.mode == 7u ? -1 : 1;
            {
                uint32_t sum = (uint32_t)yfrac + record.slope;
                if (sum > UINT16_MAX)
                    ++y;
                yfrac = (uint16_t)sum;
            }
        }
        break;
    case 9u:
        port_sprite_plot_active((int16_t)x, (int16_t)y,
                                (uint8_t)record.color);
        break;
    default:
        break;
    }
}

void skybox_op_helper(uint16_t color, uint16_t point_count,
                      struct RasterPoint p0, struct RasterPoint p1,
                      struct RasterPoint p2, struct RasterPoint p3)
{
    const int16_t points[8] = {
        (int16_t)p0.x, (int16_t)p0.y, (int16_t)p1.x, (int16_t)p1.y,
        (int16_t)p2.x, (int16_t)p2.y, (int16_t)p3.x, (int16_t)p3.y
    };
    raster_polygon((int16_t)color, (int16_t)point_count, points,
                   RASTER_FILL_SOLID, 1);
}

void preRender_wheel_helper4(int16_t color, int16_t point_count, ...)
{
    va_list args;
    int16_t points[8];
    unsigned i;
    if (point_count <= 0 || point_count > 64)
        return;
    va_start(args, point_count);
    /* preRender_wheel.c supplies promoted I16 x/y scalars. In the original
       16-bit ABI the assembly views those stack words in place; host default
       argument promotion makes each scalar an int and needs explicit decode. */
    if (point_count != 4) {
        va_end(args);
        return;
    }
    for (i = 0; i < 8u; ++i)
        points[i] = (int16_t)va_arg(args, int);
    va_end(args);
    raster_polygon(color, point_count, points, RASTER_FILL_SOLID, 0);
}
