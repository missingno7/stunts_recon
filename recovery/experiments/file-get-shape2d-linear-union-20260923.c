struct SHAPE2D;
union FAR_BITS {
    unsigned char far *pointer;
    struct { unsigned short offset, segment; } words;
};
struct SHAPE2D far *file_get_shape2d(unsigned char far *memchunk, int index)
{
    unsigned short shapecount, offsetofs, dataofs;
    unsigned long chunkofs, linear;
    union FAR_BITS input, result;
    shapecount = *(unsigned short far *)&memchunk[4];
    offsetofs = (index << 2) + (shapecount << 2) + 6;
    dataofs = (shapecount << 3) + 6;
    chunkofs = *(unsigned long far *)(&memchunk[offsetofs]);
    input.pointer = memchunk;
    linear = ((unsigned long)input.words.segment << 4) + input.words.offset + dataofs + chunkofs;
    result.words.offset = (unsigned short)(linear & 15);
    result.words.segment = (unsigned short)(linear >> 4);
    return (struct SHAPE2D far *)result.pointer;
}
