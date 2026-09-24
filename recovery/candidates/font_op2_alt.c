extern int far font_op2(char *name);
int far font_op2_alt(char *name)
{
    return (320 - font_op2(name)) / 2;
}
