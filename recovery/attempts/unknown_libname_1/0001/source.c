void far unknown_libname_1(unsigned char *block)
{
    if (block != 0) {
        block[-2] |= 1;
    }
}