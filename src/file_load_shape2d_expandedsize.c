#include "stunts_types.h"
/* READABILITY: Compute expanded shape-resource storage requirements in paragraph units. */

#define DOS_PARAGRAPH_SHIFT 4
/* PORT: Resource headers use MSC 16-bit int layout; assert these field offsets in a host port. */
struct SHAPE2D {
    I16 width; I16 height; I16 unknown1; I16 unknown2;
    I16 x; I16 y; I8 unknown3, unknown4, unknown5, unknown6;
};
extern I16 FAR file_get_res_shape_count(void FAR *memchunk);
extern struct SHAPE2D FAR * FAR file_get_shape2d(U8 FAR *memchunk, I16 index);
/* Sum each shape header and expanded pixel span, then return the paragraph count.
 * Params and return follow the declared C signature. */
I16 FAR file_load_shape2d_expandedsize(void FAR *memchunk) {
    I16 shapecount, i;
    U16 pixels;
    I32 size;
    struct SHAPE2D FAR *memshape;
    shapecount = file_get_res_shape_count(memchunk);
    size = shapecount * 8 + sizeof(struct SHAPE2D);
    for (i = 0; i < shapecount; ++i) {
        memshape = file_get_shape2d((U8 FAR *)memchunk, i);
        pixels = (memshape->width * memshape->height) << 3;
        size += pixels;
        size += sizeof(struct SHAPE2D);
    }
    size += sizeof(struct SHAPE2D);
    /* PORT: The 16-bit compiler accumulates this size as long before narrowing to int. */
    size >>= DOS_PARAGRAPH_SHIFT;
    return size;
}
