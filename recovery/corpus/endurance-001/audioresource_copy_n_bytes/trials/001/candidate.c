void far audioresource_copy_n_bytes(unsigned char far *source,
                                    unsigned char far *destination,
                                    int size)
{
    register unsigned int remaining;
    if (size > 0) {
        for (remaining = (unsigned int)size; remaining != 0; --remaining) {
            *destination++ = *source++;
        }
    }
}
