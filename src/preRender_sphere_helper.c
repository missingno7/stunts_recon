extern void far preRender_sphere_helper2(int, char *);
extern void far preRender_default_alt(int, int, char *);
void preRender_sphere_helper(int source, int destination)
{
    char buffer[128];
    preRender_sphere_helper2(source, buffer);
    preRender_default_alt(destination, 32, buffer);
}
