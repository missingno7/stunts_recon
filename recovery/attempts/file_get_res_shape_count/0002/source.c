unsigned short file_get_res_shape_count(void far *memchunk)
{
    register unsigned short far *shapes;
    shapes = (unsigned short far *)memchunk;
    return shapes[2];
}