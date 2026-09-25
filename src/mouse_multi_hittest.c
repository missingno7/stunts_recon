extern unsigned char kbormouse;
extern int mouse_xpos;
extern int word_361CE;

int far mouse_multi_hittest(int count, int *left, int *right, int *top, int *bottom)
{
    register int index;
    if (kbormouse != 0) {
        for (index = 0; index < count; ++index) {
            if (left[index] <= mouse_xpos &&
                right[index] >= mouse_xpos &&
                top[index] <= word_361CE &&
                bottom[index] >= word_361CE)
                return (signed char)index;
        }
    }
    return -1;
}
