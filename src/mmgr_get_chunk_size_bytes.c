extern unsigned short far mmgr_get_chunk_size(char far *ptr);

unsigned long far mmgr_get_chunk_size_bytes(char far *ptr)
{
    return ((unsigned long)mmgr_get_chunk_size(ptr)) << 4;
}
