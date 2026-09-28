/* READABILITY: Compute the largest unflipped shape allocation in paragraph units. */
struct SHAPE2D {
    int s2d_width;
    int s2d_height;
    unsigned s2d_unk1;
    unsigned s2d_unk2;
    unsigned s2d_pos_x;
    unsigned s2d_pos_y;
    unsigned char s2d_unk3;
    unsigned char s2d_unk4;
};
extern int far file_get_res_shape_count(char far *memchunk);
extern struct SHAPE2D far * far file_get_shape2d(char far *memchunk, int index);

/* Find the largest pixel area among a resource?s shapes and return its paragraph allocation size.
 * Params and return follow the declared C signature. */
unsigned far file_get_unflip_size(char far *memchunk)
{
    int i, shapecount;
    unsigned size, maxsize;
    struct SHAPE2D far *memshape;

    shapecount = file_get_res_shape_count(memchunk);
    maxsize = 0;
    for (i = 0; i < shapecount; i++) {
        memshape = file_get_shape2d(memchunk, i);
        size = memshape->s2d_width * memshape->s2d_height + 0x20;
        size >>= 4;
        if (size > maxsize) maxsize = size;
    }
    return maxsize;
}
