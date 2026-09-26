extern unsigned strlen(char *s);
extern char *strcpy(char *destination, char *source);
extern void far file_build_path(char *dir, char *name, char *ext, char *dst);
extern char *strcat(char *destination, char *source);

void far file_build_path(char *dir, char *name, char *ext, char *dst)
{
    register int dirlen;
    char last_character;
    if (dir) {
        strcpy(dst, dir);
        dirlen = strlen(dir);
    } else {
        dst[0] = 0;
        dirlen = 0;
    }
    if (dirlen) {
        last_character = dir[dirlen - 1];
        if (last_character != ':' && last_character != '\\')
            strcat(dst, "\\");
    }
    strcat(dst, name);
    strcat(dst, ext);
}
