extern unsigned char _based(_segname("seg012")) incnums[];
void far sub_35DE6(unsigned int destination_offset, unsigned int count,
                   unsigned char far *source)
{
    unsigned int i;
    for (i = 0; i < count; ++i) incnums[destination_offset + i] = source[i];
}
