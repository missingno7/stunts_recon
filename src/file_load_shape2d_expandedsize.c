struct SHAPE2D {
    int width; int height; int unknown1; int unknown2;
    int x; int y; char unknown3, unknown4, unknown5, unknown6;
};
extern int far file_get_res_shape_count(void far *memchunk);
extern struct SHAPE2D far * far file_get_shape2d(unsigned char far *memchunk, int index);
int far file_load_shape2d_expandedsize(void far *memchunk) {
    int shapecount, i;
    unsigned int pixels;
    long size;
    struct SHAPE2D far *memshape;
    shapecount = file_get_res_shape_count(memchunk);
    size = shapecount * 8 + sizeof(struct SHAPE2D);
    for (i = 0; i < shapecount; ++i) {
        memshape = file_get_shape2d((unsigned char far *)memchunk, i);
        pixels = (memshape->width * memshape->height) << 3;
        size += pixels;
        size += sizeof(struct SHAPE2D);
    }
    size += sizeof(struct SHAPE2D);
    size >>= 4;
    return size;
}
