#include "port_runtime.h"

#include <stdint.h>

/* The second DOS sprite descriptor is intentionally shared with sprite.c.
   Its line table uses offsets in the bitmap segment, just like sprite1. */
extern PortSprite sprite2;

typedef struct PortSpriteView {
    uint8_t *pixels;
    size_t extent;
    PortSprite *sprite;
} PortSpriteView;

static int active_view(PortSpriteView *view)
{
    return view != NULL &&
           port_sprite_active_view(&view->pixels, &view->extent,
                                  &view->sprite) &&
           view->pixels != NULL && view->sprite != NULL &&
           view->sprite->sprite_bitmapptr != NULL &&
           view->sprite->lineofs != NULL;
}

static int source_view(PortSprite *source, const PortSpriteView *active,
                       PortSpriteView *view)
{
    size_t extent;
    uint8_t *pixels;

    if (source == NULL || source->sprite_bitmapptr == NULL ||
        source->lineofs == NULL || view == NULL)
        return 0;

    if (active != NULL && active->sprite != NULL &&
        source->sprite_bitmapptr == active->sprite->sprite_bitmapptr) {
        view->pixels = active->pixels;
        view->extent = active->extent;
        view->sprite = source;
        return 1;
    }

    if (port_memory_extent(source->sprite_bitmapptr, &extent) &&
        extent >= sizeof(PortShape2D)) {
        view->pixels = (uint8_t *)source->sprite_bitmapptr;
        view->extent = extent;
        view->sprite = source;
        return 1;
    }

    /* sprite.c keeps the screen's header and row table private; the screen
       bitmap itself is the exported 64 KiB MCGA aperture. */
    if (source->sprite_bitmapptr->width == PORT_SCREEN_WIDTH &&
        source->sprite_bitmapptr->height == PORT_SCREEN_HEIGHT) {
        pixels = port_video_pixels();
        if (pixels != NULL) {
            view->pixels = pixels;
            view->extent = PORT_VIDEO_MEMORY_BYTES;
            view->sprite = source;
            return 1;
        }
    }
    return 0;
}

static uint8_t *shape_data(const PortShape2D *shape, size_t *extent_out)
{
    size_t extent;
    if (shape == NULL || !port_memory_extent(shape, &extent) ||
        extent < sizeof(*shape))
        return NULL;
    if (extent_out != NULL)
        *extent_out = extent - sizeof(*shape);
    return (uint8_t *)shape + sizeof(*shape);
}

static int offset_fits(size_t offset, size_t count, size_t extent)
{
    return offset <= extent && count <= extent - offset;
}

static int32_t signed_word(uint16_t value)
{
    return value <= INT16_MAX ? (int32_t)value
                              : (int32_t)value - 65536;
}

static void publish_screen_write(const char *reason,
                                 const PortSpriteView *view)
{
    if (view != NULL && view->pixels == port_video_pixels())
        port_video_publish(reason);
}

/* seg012:0x35C4E copies a rectangle from sprite2 to sprite1. Its fifth
   argument is a linear destination displacement: the original converts
   (x + displacement) to a row/column using sprite1.width2, then starts the
   copy at that destination cell. */
void clear_rect(int16_t x, int16_t y, int16_t width, int16_t height,
                int16_t destination_offset)
{
    PortSpriteView destination;
    PortSpriteView source;
    PortSprite *target;
    PortSprite *input = &sprite2;
    int32_t linear;
    int32_t quotient;
    int32_t remainder;
    int32_t source_y;
    int32_t target_y;
    int32_t target_x;
    int32_t row;
    uint16_t target_width;

    if (width <= 0 || height <= 0 || !active_view(&destination))
        return;
    target = destination.sprite;
    target_width = target->words2[6];
    if (target_width == 0 || !source_view(input, &destination, &source))
        return;

    linear = signed_word((uint16_t)((uint16_t)destination_offset +
                                    (uint16_t)x));
    quotient = linear / (int32_t)target_width;
    remainder = linear % (int32_t)target_width;
    source_y = y;
    target_y = signed_word((uint16_t)((uint16_t)y +
                                      (uint16_t)quotient));
    target_x = signed_word((uint16_t)remainder);

    for (row = 0; row < height; ++row, ++source_y, ++target_y) {
        size_t source_at;
        size_t destination_at;
        int column;
        if (source_y < 0 || target_y < 0 || target_x < 0 ||
            (uint32_t)source_y >= input->sprite_bitmapptr->height ||
            (uint32_t)target_y >= target->sprite_bitmapptr->height ||
            (uint32_t)target_x >= target_width)
            continue;
        source_at = (uint16_t)(input->lineofs[(uint16_t)source_y] +
                               (uint16_t)x);
        destination_at = (uint16_t)(target->lineofs[(uint16_t)target_y] +
                                    (uint16_t)target_x);
        for (column = 0; column < width; ++column) {
            size_t source_pixel = (uint16_t)(source_at + (uint16_t)column);
            size_t destination_pixel =
                (uint16_t)(destination_at + (uint16_t)column);
            if (offset_fits(source_pixel, 1u, source.extent) &&
                offset_fits(destination_pixel, 1u, destination.extent))
                destination.pixels[destination_pixel] =
                    source.pixels[source_pixel];
        }
    }
    publish_screen_write("clear_rect", &destination);
}

/* The shared fill tail at seg012:0x24027 writes an unclipped solid rectangle.
   Each active row starts one sprite pitch after the previous one; byte writes
   preserve the odd-width tail handled by STOSB in the DOS routine. */
void sprite_1_unk(int16_t x, int16_t y, int16_t width, int16_t height,
                  int16_t color)
{
    PortSpriteView destination;
    PortSprite *sprite;
    uint16_t pitch;
    uint16_t row_offset;
    int row;
    int wrote = 0;

    if (width <= 0 || height <= 0 || !active_view(&destination))
        return;
    sprite = destination.sprite;
    if ((uint16_t)y >= sprite->sprite_bitmapptr->height)
        return;
    pitch = sprite->words2[4];
    row_offset = (uint16_t)(sprite->lineofs[(uint16_t)y] + (uint16_t)x);
    for (row = 0; row < height; ++row) {
        uint16_t pixel_offset = row_offset;
        int column;
        if ((uint32_t)(uint16_t)y + (uint32_t)row >=
            sprite->sprite_bitmapptr->height)
            break;
        for (column = 0; column < width; ++column) {
            if (offset_fits(pixel_offset, 1u, destination.extent)) {
                destination.pixels[pixel_offset] = (uint8_t)color;
                wrote = 1;
            }
            pixel_offset = (uint16_t)(pixel_offset + 1u);
        }
        row_offset = (uint16_t)(row_offset + pitch);
    }
    if (wrote)
        publish_screen_write("sprite_1_unk", &destination);
}

/* The icon routines operate on uncompressed one-byte-per-pixel shape data.
   Their destination starts at the requested x/y, ignores the sprite clip
   rectangle, and combines each source byte with the destination byte. */
static void icon_combine(const PortShape2D *shape, int16_t x, int16_t y,
                         int use_and)
{
    PortSpriteView destination;
    uint8_t *source = shape_data(shape, NULL);
    uint32_t row;
    uint16_t width;
    uint16_t height;
    uint16_t pitch;
    uint16_t destination_offset;
    int wrote = 0;

    if (shape == NULL || source == NULL || !active_view(&destination))
        return;
    width = shape->width;
    height = shape->height;
    if (width == 0 || height == 0)
        return;
    {
        size_t available;
        if (!port_memory_extent(shape, &available) ||
            available < sizeof(*shape) ||
            (size_t)width * (size_t)height > available - sizeof(*shape))
            return;
    }
    if ((uint16_t)y >= destination.sprite->sprite_bitmapptr->height)
        return;

    pitch = destination.sprite->words2[4];
    destination_offset =
        (uint16_t)(destination.sprite->lineofs[(uint16_t)y] + (uint16_t)x);
    for (row = 0; row < height; ++row) {
        uint32_t column;
        if ((uint32_t)(uint16_t)y + row >=
            destination.sprite->sprite_bitmapptr->height)
            break;
        for (column = 0; column < width; ++column) {
            if (offset_fits(destination_offset, 1u, destination.extent)) {
                if (use_and)
                    destination.pixels[destination_offset] &= source[0];
                else
                    destination.pixels[destination_offset] |= source[0];
                wrote = 1;
            }
            /* The DOS width-one mask tail uses `and word ptr [di], ax` after
               SHR has reduced the width to zero and LODSB supplied AL. AH is
               still zero, so it also clears the byte immediately following
               the requested pixel. Preserve that observable singleton case. */
            if (use_and && width == 1u) {
                uint16_t adjacent =
                    (uint16_t)(destination_offset + 1u);
                if (offset_fits(adjacent, 1u, destination.extent)) {
                    destination.pixels[adjacent] = 0;
                    wrote = 1;
                }
            }
            ++source;
            destination_offset = (uint16_t)(destination_offset + 1u);
        }
        destination_offset =
            (uint16_t)(destination_offset + (uint16_t)(pitch - width));
    }
    if (wrote)
        publish_screen_write(use_and ? "putpixel_iconMask"
                                     : "putpixel_iconFillings",
                             &destination);
}

void putpixel_iconFillings(const PortShape2D *shape, int16_t x, int16_t y)
{
    icon_combine(shape, x, y, 0);
}

void putpixel_iconMask(const PortShape2D *shape, int16_t x, int16_t y)
{
    icon_combine(shape, x, y, 1);
}

/* shape2d_op_unk4 and shape2d_render_bmp_as_mask share the DOS signed-run
   decoder. Positive packets repeat one operand byte, negative packets carry
   literal operand bytes, and a zero control byte terminates the stream. The
   first helper ORs each mask byte into sprite1; the second ANDs it. */
static void shape2d_combine_runs(const PortShape2D *shape, int use_and,
                                const char *operation)
{
    PortSpriteView destination;
    size_t source_extent;
    uint8_t *source = shape_data(shape, &source_extent);
    size_t pixel_count;
    size_t pixel_index = 0;
    size_t source_index = 0;
    uint16_t width;
    uint16_t height;
    uint16_t origin_x;
    uint16_t origin_y;

    if (shape == NULL || source == NULL || !active_view(&destination))
        return;
    width = shape->width;
    height = shape->height;
    if (width == 0 || height == 0)
        return;
    pixel_count = (size_t)width * (size_t)height;
    if (pixel_count > (SIZE_MAX - 1u) / 2u) {
        port_guest_unwind("shape2d RLE dimensions overflowed");
        return;
    }
    if (source_extent > pixel_count * 2u + 1u)
        source_extent = pixel_count * 2u + 1u;
    origin_x = shape->pos_x;
    origin_y = shape->pos_y;

    while (source_index < source_extent && pixel_index < pixel_count) {
        int8_t control = (int8_t)source[source_index++];
        size_t run;
        size_t count;
        size_t item;
        uint8_t repeated = 0;

        if (control == 0)
            break;
        run = (size_t)(control < 0 ? -(int)control : (int)control);
        count = run < pixel_count - pixel_index
                    ? run : pixel_count - pixel_index;
        if (control > 0) {
            if (source_index >= source_extent) {
                port_guest_unwind("shape2d RLE repeat value is outside its source");
                return;
            }
            repeated = source[source_index++];
        } else if (count > source_extent - source_index) {
            port_guest_unwind("shape2d RLE literal packet is outside its source");
            return;
        }

        for (item = 0; item < count; ++item, ++pixel_index) {
            uint32_t column = (uint32_t)(pixel_index % width);
            uint32_t row = (uint32_t)(pixel_index / width);
            uint32_t x = (uint32_t)origin_x + column;
            uint32_t y = (uint32_t)origin_y + row;
            uint8_t mask = control > 0 ? repeated : source[source_index++];
            if (x < destination.sprite->sprite_bitmapptr->width &&
                y < destination.sprite->sprite_bitmapptr->height) {
                size_t offset =
                    (size_t)destination.sprite->lineofs[y] + (size_t)x;
                if (offset_fits(offset, 1u, destination.extent)) {
                    if (use_and)
                        destination.pixels[offset] &= mask;
                    else
                        destination.pixels[offset] |= mask;
                }
            }
        }
        if (control < 0 && count < run)
            break;
    }
    publish_screen_write(operation, &destination);
}

void shape2d_op_unk4(const PortShape2D *shape)
{
    shape2d_combine_runs(shape, 0, "shape2d_op_unk4");
}

void shape2d_render_bmp_as_mask(const PortShape2D *shape)
{
    shape2d_combine_runs(shape, 1, "shape2d_render_bmp_as_mask");
}

/* The translated parser calls this at the current pixel while `literal_cnt`
   pixels may still be pending in front of it. `max_pixels` is the remaining
   logical shape extent from this pointer, so the scan never reads an allocator
   neighbor (the DOS FAR helper could scan through its 16-bit offset window). */
int16_t port_parse_shape2d_helper3(const uint8_t *source,
                                   uint16_t max_pixels)
{
    uint8_t value;
    uint16_t count = 0;

    if (source == NULL || max_pixels == 0)
        return 0;

    value = source[0];
    while (count < max_pixels && source[count] == value)
        ++count;

    /* Keep the original I16 return ABI if a single run spans the sign bit. */
    if (count > INT16_MAX)
        return (int16_t)((int32_t)count - 65536);
    return (int16_t)count;
}
