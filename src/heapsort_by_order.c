#include "stunts_types.h"
/* READABILITY: Sort the heap keys in descending order while applying each swap to the paired data array. */
/* Sort heap keys descending and apply the same swaps to the associated data.
 * Params and return follow the declared C signature. */
void heapsortorder(I16 n, I16 *heap, I16 *data)
{
    I16 temp;
    I16 gap;
    I16 i;
    I16 j;

    for (gap = n / 2; gap > 0; gap = gap / 2) {
        for (i = gap; i < n; i++) {
            for (j = i - gap; j >= 0; j = j - gap) {
                if (heap[j] >= heap[j + gap]) break;
                temp = heap[j];
                heap[j] = heap[j + gap];
                heap[j + gap] = temp;
                temp = data[j];
                data[j] = data[j + gap];
                data[j + gap] = temp;
            }
        }
    }
}
