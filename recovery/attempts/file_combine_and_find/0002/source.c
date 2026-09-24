extern void far file_build_path(char *dir, char *name, char *ext, char *dst);
extern char * far file_find(char *query);
char * far file_combine_and_find(char *dir, char *name, char *ext)
{
    char path[80];
    file_build_path(dir, name, ext, path);
    return file_find(path);
}
