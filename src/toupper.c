#include "stunts_types.h"
/* READABILITY: Convert an ASCII lowercase letter to uppercase; leave all other integer values unchanged. */
/* Return an uppercase ASCII letter, leaving non-lowercase values unchanged.
 * Params and return follow the declared C signature. */
I16 toupper(I16 ch)
{
    if (ch >= 'a' && ch <= 'z') {
        ch -= ' ';
    }
    return ch;
}