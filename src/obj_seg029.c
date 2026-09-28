/* READABILITY: Compare audio chunk names, find a chunk-table index, locate resource bytes, and copy them. */
extern int far toupper(int value);
extern unsigned long far audioresource_get_dword(char far *address);

/* Compare up to count bytes of two names, optionally using case-sensitive matching.
 * Params and return follow the declared C signature. */
int far audioresource_compare_chunknames(int caseSensitive, unsigned char far *chunkName,
                                         unsigned char far *foundName, int count)
{
    if (count != 0) {
        do {
            if (*chunkName == 0 || *foundName == 0)
                break;
            if (caseSensitive != 0 && *chunkName != *foundName)
                return 0;
            if (caseSensitive == 0 && toupper(*chunkName) != toupper(*foundName))
                return 0;
            chunkName++;
            foundName++;
            count--;
        } while (count != 0);
    }
    return 1;
}

/* Scan four-byte resource names at the given stride and return the matching entry index.
 * Params and return follow the declared C signature. */
int far audioresource_get_chunk_index(int stride, int numChunks, unsigned char *chunkName,
                                      char far *chunkNames)
{
    unsigned char name[5];
    int index;
    int byteIndex;

    name[4] = 0;
    for (index = 0; index < numChunks; index++) {
        for (byteIndex = 0; byteIndex < 4; byteIndex++)
            name[byteIndex] = *chunkNames++;
        if (audioresource_compare_chunknames(0, name, chunkName, 4))
            return index;
        chunkNames += stride;
    }
    return -1;
}

/* Find a named entry in a packed audio resource and return its far data pointer.
 * Params and return follow the declared C signature. */
char far * far audioresource_find(char huge *resource, unsigned char *chunkName)
{
    unsigned num;
    int index;
    char far *entry;
    unsigned long offset;
    char far *chunkData;

    chunkData = 0;
    num = *(unsigned far *)(resource + 4);
    index = audioresource_get_chunk_index(0, num, chunkName, resource + 6);
    if (index >= 0) {
        entry = (char far *)resource + 6 + num * 4 + index * 4;
        offset = audioresource_get_dword(entry);
        chunkData = (char far *)resource + 6 + num * 8 + offset;
    }
    return chunkData;
}

/* Copy size bytes from the far source span into the far destination span.
 * Params and return follow the declared C signature. */
void far audioresource_copy_n_bytes(unsigned char far *src, char far *dst, int size)
{
    int i;

    for (i = 0; i < size; i++, src++, dst++)
        *dst = *src;
}
