/* READABILITY: Walk packed four-character resource-name records and fill result pointers with shape or sound lookups. */
extern char far * far locate_shape_fatal(char far *data, char *name);
/* Resolve packed four-character shape names into the caller result array; stop at the empty-name terminator.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve shape names in the loaded resource block. */
void locate_many_resources(char far *data, char *names, char far **result) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
    while (*names != 0) { *result++ = locate_shape_fatal(data, names); names += 4; }
}
extern char far * far locate_shape_nofatal(char far *data, char *name);
/* Resolve packed shape names without fatal error handling.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve shape names in the loaded resource block. */
void far nopsub_367E4(char far *data, char *names, char far **results) {
    int index = 0;
    while (*names != 0) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
        results[index++] = locate_shape_nofatal(data, names);
        names += 4;
    }
}
extern char far * far locate_sound_fatal(char far *data, char *name);
/* Resolve packed sound names with fatal error handling.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve sound names in the loaded resource block. */
void far nopsub_36826(char far *data, char *names, char far **results) {
    int index = 0;
    while (*names != 0) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
        results[index++] = locate_sound_fatal(data, names);
        names += 4;
    }
}
/* Resolve packed shape names without fatal error handling using indexed output slots.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve shape names in the loaded resource block. */
void far nopsub_36868(char far *data, char *names, char far **results) {
    int index = 0;
    while (*names != 0) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
        results[index++] = locate_shape_nofatal(data, names);
        names += 4;
    }
}
