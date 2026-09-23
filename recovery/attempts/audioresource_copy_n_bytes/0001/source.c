void far audioresource_copy_n_bytes(unsigned char far *source,
                                    unsigned char far *destination,
                                    int size)
{
    while (size > 0) {
        *destination = *source;
        ++source;
        ++destination;
        --size;
    }
}