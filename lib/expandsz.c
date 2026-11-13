#include "files.h"
#include "memory.h"
#include "types.h"
/* Compute expanded shape-resource storage requirements in paragraph units. */

/* Local signed 16-byte shape view; other units use unsigned position words. */
/* Local view: SHAPE2D; struct.count is 10; another unit uses 6. */
struct SHAPE2D {
    I16 width; I16 height; I16 s2d_hotspot_x; I16 s2d_hotspot_y;
    I16 x; I16 y; I8 s2d_plane_color0, s2d_plane_color1_bg, s2d_plane_color2_flip, s2d_plane_color3_flags;
};

/* Sum each shape header and expanded pixel span, then return the paragraph count.
 * Params and return follow the declared C signature. */
I16 FAR file_load_shape2d_expandedsize(void FAR *memchunk) {
    I16 shapecount, i;
    U16 pixels;
    I32 size;
    struct SHAPE2D FAR *memshape;
    shapecount = file_get_res_shape_count(memchunk);
    size = shapecount * RESOURCE_DIRECTORY_ENTRY_BYTES + sizeof(struct SHAPE2D);
    for (i = 0; i < shapecount; ++i) {
        memshape = shapepointer((U8 FAR *)memchunk, i);
        pixels = (memshape->width * memshape->height) << SHAPE_EXPANSION_PIXEL_SHIFT;
        size += pixels;
        size += sizeof(struct SHAPE2D);
    }
    size += sizeof(struct SHAPE2D);
    size >>= DOS_PARAGRAPH_SHIFT;
    return size;
}
