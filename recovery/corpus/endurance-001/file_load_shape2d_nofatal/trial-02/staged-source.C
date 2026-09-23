extern void far * near file_load_shape2d(char *shapename, int fatal);
void far * far file_load_shape2d_nofatal(char *shapename)
{
    return file_load_shape2d(shapename, 0);
}
