void far unknown_libname_1(unsigned char *block)
{
    register unsigned char *p = block;
    if (p != 0) {
        p[-2] |= 1;
    }
}