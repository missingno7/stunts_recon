unsigned long parse_shape2d_helper(unsigned int offset, unsigned int segment)
{
    return (((((unsigned long)segment << 1) << 1) << 1) << 1) + offset;
}
