#include "port_runtime.h"

#include <string.h>

extern PortSprite sprite2;
extern void clear_rect(int16_t, int16_t, int16_t, int16_t, int16_t);
extern void sprite_1_unk(int16_t, int16_t, int16_t, int16_t, int16_t);
extern void putpixel_iconFillings(const PortShape2D *, int16_t, int16_t);
extern void putpixel_iconMask(const PortShape2D *, int16_t, int16_t);
extern void shape2d_op_unk4(const PortShape2D *);
extern void shape2d_render_bmp_as_mask(const PortShape2D *);

static PortSprite s_default_sprite2;

__declspec(dllexport) int dh_screen_reset(const uint8_t *initial,
                                          uint32_t length)
{
    if (length > PORT_VIDEO_MEMORY_BYTES || (length != 0 && initial == NULL))
        return 0;
    port_memory_init();
    port_sprite_init();
    sprite_copy_2_to_1();
    s_default_sprite2 = sprite2;
    memset(port_video_pixels(), 0, PORT_VIDEO_MEMORY_BYTES);
    if (length != 0)
        memcpy(port_video_pixels(), initial, length);
    return 1;
}

__declspec(dllexport) int dh_call_sprite_1_unk3(const uint8_t *pixels,
                                                 uint16_t width,
                                                 uint16_t height,
                                                 uint16_t x, uint16_t y,
                                                 uint16_t phase)
{
    uint32_t count = (uint32_t)width * height;
    uint32_t bytes = count + sizeof(PortShape2D);
    uint16_t paragraphs;
    PortShape2D *shape;
    if (pixels == NULL || width == 0 || height == 0 || count > 0xFFF0u)
        return 0;
    paragraphs = (uint16_t)((bytes + 15u) >> 4);
    shape = (PortShape2D *)mmgr_alloc_pages("DIFFHARNESS", paragraphs);
    if (shape == NULL)
        return 0;
    memset(shape, 0, bytes);
    shape->width = width;
    shape->height = height;
    shape->pos_x = x;
    shape->pos_y = y;
    memcpy((uint8_t *)shape + sizeof(*shape), pixels, count);
    sprite_1_unk3(shape, (int16_t)phase);
    mmgr_free(shape);
    return 1;
}

__declspec(dllexport) void dh_call_draw_filled_rect(int16_t x, int16_t y,
                                                    int16_t width,
                                                    int16_t height,
                                                    int16_t color)
{
    draw_filled_rect(x, y, width, height, color);
}

__declspec(dllexport) int dh_call_sprite_1_unk(int16_t x, int16_t y,
                                               int16_t width,
                                               int16_t height,
                                               int16_t color)
{
    sprite_1_unk(x, y, width, height, color);
    return 1;
}

static PortShape2D *dh_alloc_shape(uint16_t width, uint16_t height,
                                   uint16_t x, uint16_t y,
                                   const uint8_t *pixels, uint32_t bytes)
{
    uint32_t total = (uint32_t)sizeof(PortShape2D) + bytes;
    uint16_t paragraphs;
    PortShape2D *shape;
    if (pixels == NULL || bytes > 0xFFF0u || total > 0xFFF0u)
        return NULL;
    paragraphs = (uint16_t)((total + 15u) >> 4);
    shape = (PortShape2D *)mmgr_alloc_pages("DIFFHARNESS AUX", paragraphs);
    if (shape == NULL)
        return NULL;
    memset(shape, 0, total);
    shape->width = width;
    shape->height = height;
    shape->pos_x = x;
    shape->pos_y = y;
    memcpy((uint8_t *)shape + sizeof(*shape), pixels, bytes);
    return shape;
}

__declspec(dllexport) int dh_call_icon_combine(const uint8_t *pixels,
                                               uint16_t width,
                                               uint16_t height,
                                               int16_t x, int16_t y,
                                               int use_and)
{
    uint32_t count = (uint32_t)width * height;
    PortShape2D *shape = dh_alloc_shape(width, height, (uint16_t)x,
                                        (uint16_t)y, pixels, count);
    if (shape == NULL)
        return 0;
    if (use_and)
        putpixel_iconMask(shape, x, y);
    else
        putpixel_iconFillings(shape, x, y);
    mmgr_free(shape);
    return 1;
}

__declspec(dllexport) int dh_call_shape2d_runs(const uint8_t *encoded,
                                               uint32_t encoded_size,
                                               uint16_t width,
                                               uint16_t height,
                                               uint16_t x, uint16_t y,
                                               int use_and)
{
    PortShape2D *shape = dh_alloc_shape(width, height, x, y,
                                        encoded, encoded_size);
    if (shape == NULL)
        return 0;
    if (use_and)
        shape2d_render_bmp_as_mask(shape);
    else
        shape2d_op_unk4(shape);
    mmgr_free(shape);
    return 1;
}

__declspec(dllexport) int dh_call_clear_rect(const uint8_t *source_pixels,
                                             uint16_t source_width,
                                             uint16_t source_height,
                                             int16_t x, int16_t y,
                                             int16_t width, int16_t height,
                                             int16_t destination_offset)
{
    uint32_t count = (uint32_t)source_width * source_height;
    PortSprite *source;
    if (source_pixels == NULL || source_width == 0 || source_height == 0 ||
        count > 0xFFF0u)
        return 0;
    source = sprite_make_window(source_width, source_height, 0);
    if (source == NULL)
        return 0;
    memcpy((uint8_t *)source->sprite_bitmapptr + sizeof(PortShape2D),
           source_pixels, count);
    /* reset() selected the screen as sprite1; install this window as sprite2. */
    sprite2 = *source;
    clear_rect(x, y, width, height, destination_offset);
    sprite2 = s_default_sprite2;
    sprite_free_window(source);
    return 1;
}

__declspec(dllexport) int16_t dh_call_mulscl(int16_t left, int16_t right)
{
    return mulscl(left, right);
}

__declspec(dllexport) uint32_t dh_read_frame(uint8_t *output,
                                             uint32_t capacity)
{
    uint32_t count = capacity < PORT_VIDEO_MEMORY_BYTES
                         ? capacity : PORT_VIDEO_MEMORY_BYTES;
    if (output == NULL)
        return 0;
    memcpy(output, port_video_pixels(), count);
    return count;
}
