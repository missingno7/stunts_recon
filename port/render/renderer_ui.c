#include "renderer_ui.h"
#include "../port_runtime.h"
#include <string.h>
#include <stdint.h>

static PortRenderLayer s_layer;
static uint8_t *s_base;
static size_t s_extent;
static uint16_t s_rows[200];
static uintptr_t s_alias_pixels;
static int s_contiguous;
static int s_armed, s_drawing;

void port_render_world_begin_active(void)
{
    uint8_t *base;
    size_t extent;
    PortSprite *sprite;
    if (!s_armed) return;
    if (!port_sprite_active_view(&base,&extent,&sprite) ||
        sprite->sprite_bitmapptr->width != 320 || sprite->sprite_bitmapptr->height != 200) {
        port_render_world_clear(); return;
    }
    int16_t clip[4];
    for (int i=0;i<4;++i) clip[i]=(int16_t)sprite->words2[i];
    port_render_world_begin(base,extent,sprite->lineofs,clip);
}

void port_render_world_arm(void) { s_armed = 1; }
void port_render_world_clear(void)
{ s_armed = s_drawing = s_layer.valid = 0; s_base = NULL; s_alias_pixels = 0; }

void port_render_world_begin(uint8_t *base, size_t extent,
                             const uint16_t *rows, const int16_t *clip)
{
    if (!s_armed || !base || !rows) return;
    s_drawing = 1;
    s_layer.valid = 0;
    s_base = base; s_extent = extent;
    s_alias_pixels = 0;
    memcpy(s_rows, rows, sizeof s_rows);
    s_contiguous = 1;
    for (int y=0;y<200;++y)
        if ((size_t)s_rows[y] != (size_t)s_rows[0]+y*320u) s_contiguous=0;
    memcpy(s_layer.clip, clip, sizeof s_layer.clip);
    memset(s_layer.ui_mask, 1, sizeof s_layer.ui_mask);
    for (int y = 0; y < 200; ++y) {
        if ((size_t)s_rows[y] + 320 > extent) { port_render_world_clear(); return; }
        memcpy(s_layer.background + y * 320, base + s_rows[y], 320);
        if (y >= clip[2] && y < clip[3]) {
            int left = clip[0] < 0 ? 0 : clip[0];
            int right = clip[1] > 320 ? 320 : clip[1];
            if (right > left) memset(s_layer.ui_mask + y*320 + left, 0, (size_t)(right-left));
        }
    }
}

void port_render_world_end(void)
{
    if (s_drawing) { s_drawing = 0; s_layer.valid = 1; }
    s_armed = 0;
}

/* Address comparisons use integers; pointers from independent sprite buffers
   must never be subtracted or relationally compared as C array pointers. */
static int row_offset(const uint8_t *base, size_t offset, int *row, int *x)
{
    uintptr_t address = (uintptr_t)base + offset;
    uintptr_t start = (uintptr_t)s_base;
    if (s_alias_pixels && address >= s_alias_pixels && address-s_alias_pixels < 64000) {
        size_t pixel = (size_t)(address-s_alias_pixels);
        *row = (int)(pixel/320); *x=(int)(pixel%320); return 1;
    }
    if (!s_base || address < start || address-start >= s_extent) return 0;
    size_t byte = (size_t)(address - start);
    if (s_contiguous && byte >= s_rows[0] && byte-s_rows[0] < 64000) {
        size_t pixel=byte-s_rows[0];
        *row=(int)(pixel/320); *x=(int)(pixel%320); return 1;
    }
    for (int y=0;y<200;++y) if (byte >= s_rows[y] && byte-s_rows[y] < 320) {
        *row = y; *x = (int)(byte-s_rows[y]); return 1;
    }
    return 0;
}

void port_render_ui_write(uint8_t *base, size_t offset, size_t count)
{
    if (s_drawing || !s_layer.valid) return;
    for (size_t i=0;i<count;++i) {
        int y,x;
        if (row_offset(base,offset+i,&y,&x)) s_layer.ui_mask[y*320+x] = 1;
    }
}

void port_render_ui_copy(uint8_t *dst, size_t dofs, const uint8_t *src,
                         size_t sofs, size_t count)
{
    if (s_drawing || !s_layer.valid) return;
    /* Recovered dirty-rectangle publications copy any subset of the world
       window into the same MCGA coordinates. They transfer provenance, not
       an opaque UI sprite. Width 320 is not a reliable discriminator. */
    int row, x;
    if (dst == port_video_pixels() && count > 0 &&
        row_offset(src, sofs, &row, &x) && count <= (size_t)(320-x) &&
        dofs == (size_t)row*320u+(size_t)x) {
        s_alias_pixels = (uintptr_t)dst;
        return;
    }
    port_render_ui_write(dst,dofs,count);
}

void port_render_layer_copy(PortRenderLayer *out)
{
    *out = s_layer;
    if (s_drawing) out->valid = 0;
    if (out->valid) {
        int visible_world = 0;
        for (size_t i=0;i<sizeof out->ui_mask;++i) if (!out->ui_mask[i]) { visible_world=1; break; }
        if (!visible_world) out->valid = 0;
    }
}
