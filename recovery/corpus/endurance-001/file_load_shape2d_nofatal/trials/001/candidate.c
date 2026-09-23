static void far * far file_load_shape2d(char *shapename, int fatal)
{
    return (void far *)0;
}
void far * far file_load_shape2d_nofatal(char *shapename)
{
    return file_load_shape2d(shapename, 0);
}
