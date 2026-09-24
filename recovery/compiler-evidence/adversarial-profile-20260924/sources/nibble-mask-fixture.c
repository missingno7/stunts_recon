typedef unsigned short u16;

u16 far mask_arg(u16 x)
{
    return x & 0x000f;
}

u16 far mask_sum(u16 x, u16 y)
{
    u16 sum = x + y;
    return sum & 0x000f;
}
