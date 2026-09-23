/* Pure C recreation candidate for isolated compiler study. */
void mmgr_copy_paras(unsigned short srcseg, unsigned short destseg, short paras)
{
    unsigned short count;
    unsigned short far *srcptr;
    unsigned short far *destptr;
    while (paras != 0) {
        count = 0x8000;
        paras -= 0x1000;
        if (paras < 0) {
            count = (paras + 0x1000) << 3;
            paras = 0;
        }
        srcptr = (unsigned short far *)(((unsigned long)srcseg) << 16);
        destptr = (unsigned short far *)(((unsigned long)destseg) << 16);
        while (count) {
            *destptr++ = *srcptr++;
            --count;
        }
        srcseg += 0x1000;
        destseg += 0x1000;
    }
}
