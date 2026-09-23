void far audioresource_copy_n_bytes(register unsigned char far *source,
                                    register unsigned char far *destination,
                                    int size)
{
    register unsigned int remaining;
    if (size > 0) {
        remaining = (unsigned int)size;
        do {
            *destination++ = *source++;
        } while (--remaining > 0);
    }
}
