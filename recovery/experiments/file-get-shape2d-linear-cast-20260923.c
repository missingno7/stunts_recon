struct SHAPE2D;
struct SHAPE2D far *file_get_shape2d(unsigned char far *memchunk, int index)
{
    unsigned short shapecount, offsetofs, dataofs;
    unsigned long chunkofs, raw, linear;
    shapecount = *(unsigned short far *)&memchunk[4];
    offsetofs = (index << 2) + (shapecount << 2) + 6;
    dataofs = (shapecount << 3) + 6;
    chunkofs = *(unsigned long far *)(&memchunk[offsetofs]);
    raw = (unsigned long)memchunk;
    linear = ((raw >> 16) << 4) + (unsigned short)raw + dataofs + chunkofs;
    return (struct SHAPE2D far *)(((linear >> 4) << 16) | (linear & 15));
}
