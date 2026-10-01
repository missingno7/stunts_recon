#include "port_runtime.h"

#include <stdio.h>
#include <string.h>

extern int dh_screen_reset(const uint8_t *initial, uint32_t length);
extern void *file_load_shape2d_res_nofatal(char *name);

__declspec(dllexport) int dh_test_bounded_shape_run(void)
{
    static uint8_t repeated[65535];
    static const uint8_t tail[] = { 0, 0, 0, 0x86 };
    const uint8_t *invalid = (const uint8_t *)(uintptr_t)1u;
    int16_t result;

    if (port_parse_shape2d_helper3(invalid, 0) != 0 ||
        port_parse_shape2d_helper3(NULL, 0) != 0 ||
        port_parse_shape2d_helper3(NULL, 1) != 0)
        return 1;

    memset(repeated, 0xA7, sizeof(repeated));
    if (port_parse_shape2d_helper3(repeated, 1) != 1 ||
        port_parse_shape2d_helper3(repeated, 3) != 3 ||
        port_parse_shape2d_helper3(repeated, 4) != 4 ||
        port_parse_shape2d_helper3(repeated, 127) != 127)
        return 2;
    result = port_parse_shape2d_helper3(repeated, 32768);
    if (result != INT16_MIN)
        return 3;
    if (port_parse_shape2d_helper3(repeated, 65535) != -1 ||
        port_parse_shape2d_helper3(repeated + sizeof(repeated) - 1u, 1) != 1)
        return 4;
    if (port_parse_shape2d_helper3(tail, 3) != 3 ||
        port_parse_shape2d_helper3(tail, 4) != 3)
        return 5;
    return 0;
}

int port_fs_find(const char *pattern, char *found, size_t capacity)
{
    (void)pattern;
    if (found != NULL && capacity != 0)
        found[0] = '\0';
    return 0;
}

static const uint8_t *s_compressed_asset;
static size_t s_compressed_asset_size;

/* seg034's PVS path calls file_find only to select an extension. The test
   supplies the matching frozen asset bytes through port_fs_load below. */
const char *__wrap_file_find(const char *path)
{
    return path;
}

void *port_fs_load(const char *path, size_t *length_out,
                   PortFarPtr *address_out)
{
    void *copy;
    (void)path;
    (void)address_out;
    if (length_out != NULL)
        *length_out = s_compressed_asset_size;
    copy = port_memory_alloc(s_compressed_asset_size, "DASHBOARD PVS", NULL);
    if (copy != NULL)
        memcpy(copy, s_compressed_asset, s_compressed_asset_size);
    return copy;
}

static uint16_t read_u16(const uint8_t *bytes)
{
    return (uint16_t)(bytes[0] | ((uint16_t)bytes[1] << 8));
}

static uint32_t read_u32(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

static int shape_span(const uint8_t *archive, size_t archive_size,
                      const char *shape_name, const uint8_t **shape_out,
                      size_t *shape_size_out)
{
    uint16_t count;
    size_t payload;
    size_t payload_extent;
    uint16_t index;
    if (archive == NULL || shape_name == NULL || strlen(shape_name) != 4u ||
        !port_memory_extent(archive, &archive_size) || archive_size < 6u)
        return 0;
    count = read_u16(archive + 4u);
    payload = 6u + (size_t)count * 8u;
    if (payload > archive_size)
        return 0;
    payload_extent = archive_size - payload;
    for (index = 0; index < count; ++index) {
        size_t name_at = 6u + (size_t)index * 4u;
        size_t offset_at = 6u + (size_t)count * 4u + (size_t)index * 4u;
        uint32_t relative = read_u32(archive + offset_at);
        size_t next = payload_extent;
        uint16_t other;
        if (memcmp(archive + name_at, shape_name, 4u) != 0)
            continue;
        if ((size_t)relative >= payload_extent)
            return 0;
        for (other = 0; other < count; ++other) {
            uint32_t candidate = read_u32(archive + 6u +
                (size_t)count * 4u + (size_t)other * 4u);
            if (candidate > relative && (size_t)candidate < next)
                next = candidate;
        }
        if (next <= relative)
            return 0;
        *shape_out = archive + payload + relative;
        *shape_size_out = next - relative;
        return 1;
    }
    return 0;
}

/* Load via the same mode-3 host C resource/parser entry used by
   setup_car_shapes: compressed PVS -> C decompressor -> C DOS-unflip
   translation -> parse_shape2d RLE page. */
__declspec(dllexport) int dh_load_dashboard_archive(
    const uint8_t *compressed, uint32_t compressed_size, const char *asset_stem,
    uint8_t *output, uint32_t output_capacity)
{
    char resource_name[32];
    void *archive;
    size_t archive_size;
    int name_length;
    int result;
    static const uint8_t empty_frame[1] = { 0 };

    if (compressed == NULL || compressed_size == 0 || asset_stem == NULL ||
        output == NULL || !dh_screen_reset(empty_frame, 0))
        return 0;
    name_length = snprintf(resource_name, sizeof(resource_name), "%s", asset_stem);
    if (name_length < 1 || (size_t)name_length >= sizeof(resource_name))
        return 0;

    s_compressed_asset = compressed;
    s_compressed_asset_size = compressed_size;
    archive = file_load_shape2d_res_nofatal(resource_name);
    s_compressed_asset = NULL;
    s_compressed_asset_size = 0;
    if (archive == NULL)
        return 0;
    if (!port_memory_extent(archive, &archive_size) ||
        archive_size > output_capacity) {
        mmgr_release(archive);
        return 0;
    }
    memcpy(output, archive, archive_size);
    result = (int)archive_size;
    mmgr_release(archive);
    return result;
}

/* Run a loaded resource shape through the production sprite routines after
   setting the real setup_car_shapes(1) mainclip bounds. */
__declspec(dllexport) int dh_render_loaded_dashboard_shape(
    const uint8_t *shape_bytes, uint32_t shape_size,
    const uint8_t *initial, int16_t operation,
    int16_t clip_left, int16_t clip_right,
    int16_t clip_top, int16_t clip_bottom,
    int16_t x, int16_t y,
    uint16_t target_width, uint16_t target_height,
    uint8_t *output, uint32_t output_capacity)
{
    PortShape2D *shape;
    uint16_t paragraphs;
    uint32_t output_size;

    if (shape_bytes == NULL || shape_size < sizeof(PortShape2D) ||
        shape_size > 0xFFF0u || initial == NULL || output == NULL ||
        operation < 0 || operation > 5)
        return 0;
    if (!dh_screen_reset(initial, PORT_VIDEO_MEMORY_BYTES))
        return 0;

    paragraphs = (uint16_t)((shape_size + 15u) >> 4);
    shape = (PortShape2D *)mmgr_alloc_pages("DASHBOARD ASSET TEST",
                                             paragraphs);
    if (shape == NULL)
        return 0;
    memcpy(shape, shape_bytes, shape_size);

    if (operation == 2) {
        PortSprite *target;
        uint8_t *target_pixels;
        uint32_t target_bytes;

        if (target_width == 0 || target_height == 0 ||
            (uint32_t)target_width * target_height > 0xFFF0u) {
            mmgr_free(shape);
            return 0;
        }
        target_bytes = (uint32_t)target_width * target_height;
        if (output_capacity < target_bytes) {
            mmgr_free(shape);
            return 0;
        }
        target = sprite_make_window(target_width, target_height, 0);
        if (target == NULL) {
            mmgr_free(shape);
            return 0;
        }
        target_pixels = (uint8_t *)target->sprite_bitmapptr +
                        sizeof(PortShape2D);
        memset(target_pixels, 0x35, target_bytes);
        sprite_setup1_from_arg_pointer(target);
        shape2d_op_unk2(shape, x, y);
        memcpy(output, target_pixels, target_bytes);
        sprite_free_window(target);
        mmgr_free(shape);
        return (int)target_bytes;
    }

    if (operation == 3) {
        PortSprite *target;
        uint8_t *target_pixels;
        uint32_t target_bytes;

        if (target_width == 0 || target_height == 0 ||
            (uint32_t)target_width * target_height > 0xFFF0u) {
            mmgr_free(shape);
            return 0;
        }
        target_bytes = (uint32_t)target_width * target_height;
        if (output_capacity < target_bytes) {
            mmgr_free(shape);
            return 0;
        }
        target = sprite_make_window(target_width, target_height, 15);
        if (target == NULL) {
            mmgr_free(shape);
            return 0;
        }
        target_pixels = (uint8_t *)target->sprite_bitmapptr +
                        sizeof(PortShape2D);
        sprite_setup1_from_arg_pointer(target);
        shape2d_op_unknown5(shape, x, y);
        memcpy(output, target_pixels, target_bytes);
        sprite_free_window(target);
        mmgr_free(shape);
        return (int)target_bytes;
    }

    {
        PortSprite *active = NULL;
        if (!port_sprite_active_view(NULL, NULL, &active) || active == NULL) {
            mmgr_free(shape);
            return 0;
        }
        active->words2[0] = (uint16_t)clip_left;
        active->words2[1] = (uint16_t)clip_right;
        active->words2[2] = (uint16_t)clip_top;
        active->words2[3] = (uint16_t)clip_bottom;
    }

    output_size = PORT_VIDEO_MEMORY_BYTES;
    if (output_capacity < output_size) {
        mmgr_free(shape);
        return 0;
    }
    if (operation == 0)
        shape2d_op_unk3(shape);
    else if (operation == 1)
        shape2d_op_unk(shape);
    else if (operation == 4)
        shape2d_op_unk4(shape);
    else
        shape2d_render_bmp_as_mask(shape);
    memcpy(output, port_video_pixels(), output_size);
    mmgr_free(shape);
    return (int)output_size;
}
