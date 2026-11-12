#include "resource.h"
#include "memory.h"
#include "stunts_types.h"
/* READABILITY: Compute expanded shape-resource storage requirements in paragraph units. */

/* PORT: Resource headers use MSC 16-bit int layout; assert these field offsets in a host port. */
/* Local signed 16-byte shape view; other units use unsigned position words. */
/* Local view: SHAPE2D; struct.count is 10; another unit uses 6. */
struct SHAPE2D {
    I16 width; I16 height; I16 s2d_hotspot_x; I16 s2d_hotspot_y;
    I16 x; I16 y; I8 s2d_plane_color0, s2d_plane_color1_bg, s2d_plane_color2_flip, s2d_plane_color3_flags;
};

/* Local view: file_get_res_shape_count; return is signed word; another unit uses unsigned word. */
extern I16 FAR file_get_res_shape_count(void FAR *memchunk);
/* Local view: shapepointer; parameters[0].pointer is unsigned byte; another unit uses signed byte. */
extern struct SHAPE2D FAR * FAR shapepointer(U8 FAR *memchunk, I16 index);
/* Sum each shape header and expanded pixel span, then return the paragraph count.
 * Params and return follow the declared C signature. */
/* Local view: file_load_shape2d_expandedsize; return is signed word; another unit uses unsigned word. */
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
    /* PORT: The 16-bit compiler accumulates this size as long before narrowing to int. */
    size >>= DOS_PARAGRAPH_SHIFT;
    return size;
}
