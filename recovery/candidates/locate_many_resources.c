extern char far* locate_shape_fatal(char far* data, char* name);
void locate_many_resources(char far* data, char* names, char far** result) {
    while (*names != 0) { *result++ = locate_shape_fatal(data, names); names += 4; }
}
