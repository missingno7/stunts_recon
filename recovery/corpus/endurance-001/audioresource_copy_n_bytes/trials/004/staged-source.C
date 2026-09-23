void far audioresource_copy_n_bytes(unsigned char far *source,
                                    unsigned char far *destination,
                                    int size)
{
    register unsigned char far *src = source;
    register unsigned char far *dst = destination;
    register int remaining;
    if (size > 0) {
        for (remaining = size; remaining != 0; --remaining) {
            *dst++ = *src++;
        }
    }
}
