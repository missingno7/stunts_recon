extern char far * far locate_shape_fatal(char far *data, char *name);
void locate_many_resources(char far *data, char *names, char far **result) {
    while (*names != 0) { *result++ = locate_shape_fatal(data, names); names += 4; }
}
extern char far * far locate_shape_nofatal(char far *data, char *name);
void far nopsub_367E4(char far *data, char *names, char far **results) {
    int index = 0;
    while (*names != 0) {
        results[index++] = locate_shape_nofatal(data, names);
        names += 4;
    }
}
extern char far * far locate_sound_fatal(char far *data, char *name);
void far nopsub_36826(char far *data, char *names, char far **results) {
    int index = 0;
    while (*names != 0) {
        results[index++] = locate_sound_fatal(data, names);
        names += 4;
    }
}
void far nopsub_36868(char far *data, char *names, char far **results) {
    int index = 0;
    while (*names != 0) {
        results[index++] = locate_shape_nofatal(data, names);
        names += 4;
    }
}
