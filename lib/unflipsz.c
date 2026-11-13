#include "memory.h"
#include "video.h"
#include "types.h"
/* Compute the largest unflipped shape allocation in paragraph units. */

/* Local 14-byte prefix; the complete shape record has two further flag bytes. */
/* Local view: SHAPE2D; struct.count is 8; another unit uses 6. */
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
#include "files.h"

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
