#include "port_runtime.h"

#include <string.h>

__declspec(dllexport) int dh_screen_reset(const uint8_t *initial,
                                          uint32_t length)
{
    if (length > PORT_VIDEO_MEMORY_BYTES || (length != 0 && initial == NULL))
        return 0;
    port_memory_init();
    port_sprite_init();
    sprite_copy_2_to_1();
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
