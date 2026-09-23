struct MEMCHUNK { char resname[12]; unsigned short ressize; unsigned short resofs; unsigned short resunk; };
extern struct MEMCHUNK *resptr1;
extern struct MEMCHUNK *resptr2;
extern void far fatal_error(char far *message, unsigned short value);
unsigned short far mmgr_get_chunk_size(char far *ptr)
{
    unsigned short regax = (unsigned short)(((unsigned long)ptr) >> 16);
    struct MEMCHUNK *ressi = resptr2;
    for (;;) {
        if (ressi == resptr1) {
            fatal_error("memory manager - BLOCK NOT FOUND", regax);
            return 0;
        }
        if (regax == ressi->resofs) break;
        --ressi;
    }
    return ressi->ressize;
}
