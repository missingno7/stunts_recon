#include "port_runtime.h"

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
        if (s_windows[i].live && &s_windows[i].sprite == sprite)
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
            *extent_out = PORT_FRAMEBUFFER_BYTES;
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

void sprite_shape_to_1(const PortShape2D *shape, int16_t x, int16_t y)
{
    draw_opaque(shape, x, y);
}

void sprite_shape_to_1_alt(const PortShape2D *shape)
{
    if (shape != NULL)
        draw_opaque(shape, shape->pos_x, shape->pos_y);
}

void sprputimage(const PortShape2D *shape)
{
    if (shape != NULL)
        draw_opaque(shape, shape->pos_x, shape->pos_y);
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
