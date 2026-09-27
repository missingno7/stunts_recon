void heapsort_by_order(int n, int *heap, int *data)
{
    int temp;
    int gap;
    int i;
    int j;

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
