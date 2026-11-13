#include "files.h"
#include "chartab.h"
#include "ui.h"
#include "types.h"
/* READABILITY: Compare audio chunk names, find a chunk-table index, locate resource bytes, and copy them. */
#include "audio.h"

/* Compare up to count bytes of two names, optionally using case-sensitive matching.
 * Params and return follow the declared C signature. */
I16 FAR audioresource_compare_chunknames(I16 caseSensitive, U8 FAR *chunkName,
                                         U8 FAR *foundName, I16 count)
{
    if (count != 0) {
        do {
            if (*chunkName == 0 || *foundName == 0)
                break;
            if (caseSensitive != 0 && *chunkName != *foundName)
                return 0;
            if (caseSensitive == 0 && cap(*chunkName) != cap(*foundName))
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
I16 FAR audioresource_get_chunk_index(I16 stride, I16 numChunks, U8 *chunkName,
                                      I8 FAR *chunkNames)
{
    U8 name[5];
    I16 index;
    I16 byteIndex;

    name[4] = 0;
    for (index = 0; index < numChunks; index++) {
        for (byteIndex = 0; byteIndex < 4; byteIndex++)
            name[byteIndex] = *chunkNames++;
        if (audioresource_compare_chunknames(0, name, chunkName, 4))
            return index;
        chunkNames += stride;
    }
    return RESOURCE_NOT_FOUND;
}

/* Find a named entry in a packed audio resource and return its far data pointer.
 * Params and return follow the declared C signature. */
I8 FAR * FAR audioresource_find(I8 HUGE *resource, U8 *chunkName)
{
    U16 num;
    I16 index;
    I8 FAR *entry;
    U32 offset;
    I8 FAR *chunkData;

    chunkData = 0;
    num = *(U16 FAR *)(resource + RESOURCE_COUNT_OFFSET);
    index = audioresource_get_chunk_index(0, num, chunkName, resource + RESOURCE_DIRECTORY_HEADER_BYTES);
    if (index >= 0) {
        /* PORT: Huge-pointer addition must preserve segment:offset normalization within this resource. */
        entry = (I8 FAR *)resource + RESOURCE_DIRECTORY_HEADER_BYTES + num * RESOURCE_OFFSET_BYTES + index * RESOURCE_OFFSET_BYTES;
        offset = audioresource_get_dword(entry);
        chunkData = (I8 FAR *)resource + RESOURCE_DIRECTORY_HEADER_BYTES + num * 8 + offset;
    }
    return chunkData;
}

/* Copy size bytes from the far source span into the far destination span.
 * Params and return follow the declared C signature. */
void FAR audioresource_copy_n_bytes(U8 FAR *src, I8 FAR *dst, I16 size)
{
    I16 i;

    for (i = 0; i < size; i++, src++, dst++)
        *dst = *src;
}
