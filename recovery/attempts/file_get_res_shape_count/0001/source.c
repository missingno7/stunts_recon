unsigned short file_get_res_shape_count(void far *memchunk)
{
    return ((unsigned short far *)memchunk)[2];
}