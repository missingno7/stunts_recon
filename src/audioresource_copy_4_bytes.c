void audioresource_copy_4_bytes(unsigned char far *dst, unsigned char far *src)
{
    *dst++ = *src++;
    *dst++ = *src++;
    *dst++ = *src++;
    *dst = *src;
}
