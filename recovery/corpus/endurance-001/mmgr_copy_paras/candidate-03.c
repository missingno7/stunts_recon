void mmgr_copy_paras(unsigned short srcseg, unsigned short destseg, unsigned short paras)
{
    unsigned short count;
    unsigned short far *srcptr;
    unsigned short far *destptr;
    while (paras != 0) {
        count = 0x8000;
        if (paras >= 0x1000) paras -= 0x1000;
        else { count = paras << 3; paras = 0; }
        srcptr = (unsigned short far *)(((unsigned long)srcseg) << 16);
        destptr = (unsigned short far *)(((unsigned long)destseg) << 16);
        while (count) { *destptr++ = *srcptr++; --count; }
        srcseg += 0x1000; destseg += 0x1000;
    }
}
