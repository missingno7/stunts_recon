#include "port_runtime.h"

#include <string.h>

/* Font header/glyph layout and pixel traversal follow the accepted routines
   asm/font_draw_text.ASM, asm/font_entries.ASM, and
   asm/line_sprite_shape_render.ASM. The built-in 1408-byte block comes from
   src/fardata_11039.c; loaded FNT records are bounded by the game allocator. */
#define PORT_FONT_HEADER_BYTES 22u
#define PORT_FONT_GLYPH_TABLE_BYTES (256u * 2u)

extern uint8_t fontdef_default[1408];
/* The locked image initially selects this built-in font segment. */
static const uint8_t *s_font_data = fontdef_default;
static size_t s_font_extent = sizeof(fontdef_default);

static uint16_t read_u16(const uint8_t *p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static void write_u16(uint8_t *p, uint16_t value)
{
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}

static const uint8_t *font_data(void)
{
    if (s_font_data == NULL)
        return NULL;
    return s_font_data;
}

static int font_glyph(const uint8_t *font, uint8_t character,
                      const uint8_t **glyph_out)
{
    size_t table_offset = PORT_FONT_HEADER_BYTES + (size_t)character * 2u;
    uint16_t glyph;
    if (table_offset + 2u > s_font_extent)
        port_guest_unwind("short font glyph offset table");
    glyph = read_u16(font + table_offset);
    if (glyph == 0) {
        *glyph_out = NULL;
        return 0;
    }
    if (glyph >= s_font_extent)
        port_guest_unwind("font glyph offset outside record");
    *glyph_out = font + glyph;
    return 1;
}

void set_fontdefseg(const void *font)
{
    size_t extent;
    if (font == NULL)
        port_guest_unwind("null font definition");
    if (port_memory_extent(font, &extent)) {
        s_font_data = (const uint8_t *)font;
        s_font_extent = extent;
    } else if (font == fontdef_default) {
        s_font_data = (const uint8_t *)font;
        s_font_extent = sizeof(fontdef_default);
    } else {
        port_guest_unwind("font definition has no registered extent");
    }
    if (s_font_extent < PORT_FONT_HEADER_BYTES + PORT_FONT_GLYPH_TABLE_BYTES)
        port_guest_unwind("font definition is shorter than its header");
    port_line_input_select_font(s_font_data);
}

void font_setup_unknown(uint16_t foreground, uint16_t background)
{
    const uint8_t *font = font_data();
    if (font == NULL)
        return;
    /* The DOS setup stores whole words after XOR AH,AH. */
    write_u16((uint8_t *)font, (uint8_t)foreground);
    write_u16((uint8_t *)font + 2u, (uint8_t)background);
}

static int16_t measure_text(const char *text, uint16_t count, int counted)
{
    const uint8_t *font = font_data();
    uint32_t width = 0;
    uint16_t remaining = count;
    if (font == NULL || text == NULL)
        return 0;
    if (counted && remaining == 0)
        return 0;
    while (*text != '\0') {
        const uint8_t *glyph;
        uint8_t character = (uint8_t)*text++;
        uint16_t advance = read_u16(font + 16u);
        if (!font_glyph(font, character, &glyph))
            continue;
        if (font[20] != 0)
            advance = glyph[0];
        width += advance;
        /* FONT_OP decrements DX only for a glyph with a nonzero offset.
           FONT_OP2 starts DX at zero and uses the same word decrement. */
        if (--remaining == 0)
            break;
    }
    return (int16_t)width;
}

int16_t font_op(const char *text, uint16_t count)
{
    return measure_text(text, count, 1);
}

int16_t font_op2(const char *text)
{
    return measure_text(text, 0, 0);
}

static void render_text(const char *text, int16_t x, int16_t y, int opaque)
{
    const uint8_t *font = font_data();
    uint8_t *mutable_font;
    uint16_t pen_x = (uint16_t)x;
    uint16_t pen_y = (uint16_t)y;
    if (font == NULL || text == NULL)
        return;
    mutable_font = (uint8_t *)font;
    write_u16(mutable_font + 8u, (uint16_t)x);
    write_u16(mutable_font + 10u, (uint16_t)y);
    while (*text != '\0') {
        const uint8_t *glyph;
        uint8_t character = (uint8_t)*text++;
        uint16_t height = read_u16(font + 14u);
        uint16_t advance = read_u16(font + 16u);
        uint8_t rowbytes = font[12];
        const uint8_t *bitmap;
        uint32_t bytes_needed;
        uint16_t row;
        if (!font_glyph(font, character, &glyph)) {
            if (character == '\r' || character == '\n') {
                pen_x = read_u16(font + 4u);
                pen_y = (uint16_t)(pen_y + read_u16(font + 18u));
                write_u16(mutable_font + 8u, (uint16_t)pen_x);
                write_u16(mutable_font + 10u, (uint16_t)pen_y);
            }
            continue;
        }
        bitmap = glyph;
        if (font[20] != 0) {
            advance = *bitmap++;
            rowbytes = (uint8_t)((advance + 7u) >> 3);
            write_u16(mutable_font + 16u, advance);
            mutable_font[12] = rowbytes;
        }
        bytes_needed = (uint32_t)rowbytes * height;
        if ((size_t)(bitmap - font) > s_font_extent ||
            bytes_needed > s_font_extent - (size_t)(bitmap - font))
            port_guest_unwind("short font glyph bitmap");
        for (row = 0; row < height; ++row) {
            uint8_t column_byte;
            for (column_byte = 0; column_byte < rowbytes; ++column_byte) {
                uint8_t bits = bitmap[(size_t)row * rowbytes + column_byte];
                unsigned bit;
                for (bit = 0; bit < 8u; ++bit) {
                    int foreground = (bits & (uint8_t)(0x80u >> bit)) != 0u;
                    if (foreground || opaque)
                        port_sprite_plot_font((int16_t)(pen_x + column_byte * 8u + bit),
                                              (int16_t)(pen_y + row),
                                              font[foreground ? 0 : 2]);
                }
            }
        }
        pen_x = (uint16_t)(pen_x + advance);
        write_u16(mutable_font + 8u, (uint16_t)pen_x);
        write_u16(mutable_font + 10u, (uint16_t)pen_y);
    }
}

void font_draw_text(const char *text, int16_t x, int16_t y)
{
    render_text(text, x, y, 0);
}

void draw_text_at(const char *text, int16_t x, int16_t y)
{
    /* Frozen DRAW_TEXT_AT writes AH (background) for each clear glyph bit;
       FONT_DRAW_TEXT instead advances DI without writing that pixel. */
    render_text(text, x, y, 1);
}
