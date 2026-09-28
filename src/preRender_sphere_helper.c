/* READABILITY: Build a sphere point buffer and pass it to the default renderer. */
extern void far preRender_sphere_helper2(int, char *);
extern void far preRender_default_alt(int, int, char *);
/* Build sphere points in a temporary buffer, then submit them to the default renderer.
 * Params and return follow the declared C signature. */
/* PLATFORM(video): submit the generated sphere points to the default renderer. */
void preRender_sphere_helper(int source, int destination)
{
    char buffer[128];
    preRender_sphere_helper2(source, buffer);
    /* PLATFORM(video): submit the sphere vertex buffer to the renderer. */
    preRender_default_alt(destination, 32, buffer);
}
