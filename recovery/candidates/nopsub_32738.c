typedef unsigned short u16;
typedef unsigned long u32;
u16 far nopsub_32738(u32 dividend, u16 divisor)
{
    return (u16)(dividend / divisor);
}
