#include "stunts_types.h"
/* READABILITY: Compute the largest unflipped shape allocation in paragraph units. */

#define SHAPE_UNFLIP_BUFFER_OVERHEAD_BYTES 0x20
#define DOS_PARAGRAPH_SHIFT 4
/* PORT: Resource headers use MSC 16-bit int layout; assert these field offsets in a host port. */
struct SHAPE2D {
    I16 s2d_width;
    I16 s2d_height;
    U16 s2d_hotspot_x;
    U16 s2d_hotspot_y;
    U16 s2d_pos_x;
    U16 s2d_pos_y;
    U8 s2d_plane_color0;
    U8 s2d_plane_color1_bg;
};
extern I16 FAR file_get_res_shape_count(I8 FAR *memchunk);
extern struct SHAPE2D FAR * FAR shapepointer(I8 FAR *memchunk, I16 index);

/* Find the largest pixel area among a resource?s shapes and return its paragraph allocation size.
 * Params and return follow the declared C signature. */
U16 FAR file_get_unflip_size(I8 FAR *memchunk)
{
    I16 i, shapecount;
    U16 size, maxsize;
    struct SHAPE2D FAR *memshape;

    shapecount = file_get_res_shape_count(memchunk);
    maxsize = 0;
    for (i = 0; i < shapecount; i++) {
        memshape = shapepointer(memchunk, i);
        size = memshape->s2d_width * memshape->s2d_height + SHAPE_UNFLIP_BUFFER_OVERHEAD_BYTES;
        size >>= DOS_PARAGRAPH_SHIFT;
        if (size > maxsize) maxsize = size;
    }
    return maxsize;
}
