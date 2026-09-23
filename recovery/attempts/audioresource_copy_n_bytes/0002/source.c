void far audioresource_copy_n_bytes(unsigned char far *source,
                                    unsigned char far *destination,
                                    int size)
{
    unsigned char far *source_cursor = source;
    unsigned char far *destination_cursor = destination;
    int remaining = size;

    while (remaining > 0) {
        *destination_cursor = *source_cursor;
        ++source_cursor;
        ++destination_cursor;
        --remaining;
    }
}