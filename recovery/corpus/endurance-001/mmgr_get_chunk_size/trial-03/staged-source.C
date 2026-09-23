struct MEMCHUNK { char resname[12]; unsigned short ressize; unsigned short resofs; unsigned short resunk; };
extern struct MEMCHUNK *resptr1;
extern struct MEMCHUNK *resptr2;
extern void far fatal_error(char far *message, unsigned short value);
union FARADDR { char far *ptr; struct { unsigned short ofs; unsigned short seg; } words; };
unsigned short far mmgr_get_chunk_size(char far *ptr)
{
    union FARADDR address;
    unsigned short regax;
    struct MEMCHUNK *ressi;
    address.ptr = ptr;
    regax = address.words.seg;
    ressi = resptr2;
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
