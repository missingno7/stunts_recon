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
    sprite_putimage_and(shape, x, y);
}

void sprite_putimage_and_alt2(const PortShape2D *shape, int16_t x, int16_t y)
{
    if (shape != NULL)
        compose_boolean(shape, x - (int16_t)shape->unknown1,
                        y - (int16_t)shape->unknown2, 1);
}

void sprite_putimage_or(const PortShape2D *shape, int16_t x, int16_t y)
{
    if (shape != NULL)
        compose_boolean(shape, x, y, 0);
}

void sprite_putimage_or_alt(const PortShape2D *shape, int16_t x, int16_t y)
{
    if (shape != NULL)
        compose_boolean(shape, x - (int16_t)shape->unknown1,
                        y - (int16_t)shape->unknown2, 0);
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

    /* Assembly visits the row pattern from index 11 down to 0. The actual
       row offset is pattern[index] + 12*k, bounded by the shape's height. */
    for (lane = 0; lane < 12; ++lane) {
        unsigned source_row = row_pattern[11u - lane];
        unsigned row_step = 0;
        uint16_t row_phase = (uint16_t)(phase_word + lane);

        for (;;) {
            uint32_t row_index = source_row + 12u * row_step;
            uint32_t target_row = (uint32_t)base_y + row_index;
            uint16_t source_offset;
            uint16_t target_offset;
            int remaining = width;
            uint16_t xphase = row_phase;
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
                source_offset = (uint16_t)(source_offset + 1u);
                target_offset = (uint16_t)(target_offset + 1u);
                after = post_skip[slot];
                source_offset = (uint16_t)(source_offset + after);
                target_offset = (uint16_t)(target_offset + after);
                remaining -= (int)after;
                ++xphase;
            }
            ++row_step;
            ++row_phase;
        }
    }

    if (s_sprite1.sprite_bitmapptr == &s_screen_shape)
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
