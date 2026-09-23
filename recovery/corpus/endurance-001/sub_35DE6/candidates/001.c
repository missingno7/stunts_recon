extern unsigned char incnums[];
void far sub_35DE6(unsigned int destination_offset, unsigned int count,
                   unsigned char far *source)
{
    unsigned char *destination = incnums + destination_offset;
    unsigned int i;
    for (i = 0; i < count; ++i) destination[i] = source[i];
}
