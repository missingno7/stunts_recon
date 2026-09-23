void far audioresource_copy_n_bytes(register unsigned char far *source,
                                    register unsigned char far *destination,
                                    int size)
{
    if (size > 0) {
        do {
            *destination++ = *source++;
        } while (--size > 0);
    }
}