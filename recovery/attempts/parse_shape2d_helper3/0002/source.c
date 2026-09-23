int parse_shape2d_helper3(char far *source)
{
    char value = *source;
    int count = 0;
    while (*source++ == value)
        ++count;
    return count;
}
