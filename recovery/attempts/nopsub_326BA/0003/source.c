void nopsub_326BA(unsigned char far *source,
                  unsigned index,
                  unsigned long *output)
{
    *output = ((unsigned long far *)(source + 6))[index];
}
