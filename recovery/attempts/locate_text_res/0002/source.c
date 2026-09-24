extern char far * far locate_shape_fatal(char far *data, char *name);
extern char textresprefix;
char far * far locate_text_res(char far *data, char *name)
{
    char textname[4];
    textname[0] = textresprefix;
    textname[1] = name[0];
    textname[2] = name[1];
    textname[3] = name[2];
    return locate_shape_fatal(data, textname);
}
