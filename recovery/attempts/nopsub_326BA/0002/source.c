void nopsub_326BA(unsigned char far *source,
                  unsigned index,
                  unsigned long *output)
{
    unsigned long far *entry;

    entry = (unsigned long far *)(source + 6 + index * 4);
    *output = *entry;
}
