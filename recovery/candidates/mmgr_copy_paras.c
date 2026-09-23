#include <dos.h>

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
        srcptr = (unsigned short far *)MK_FP(srcseg, 0);
        destptr = (unsigned short far *)MK_FP(destseg, 0);
        while (count) {
            *destptr = *srcptr;
            ++srcptr;
            ++destptr;
            --count;
        }
        srcseg += 0x1000;
        destseg += 0x1000;
    }
}