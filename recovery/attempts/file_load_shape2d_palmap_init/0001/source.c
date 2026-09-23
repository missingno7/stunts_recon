extern unsigned char palmap[];
void file_load_shape2d_palmap_init(unsigned char far* pal)
{
    int i;
    for (i = 0; i < 0x10; ++i) {
        palmap[i] = pal[i];
    }
}
