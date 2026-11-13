#include "audio.h"
#include "files.h"
#include "types.h"
/* READABILITY: Walk packed four-character resource-name records and fill result pointers with shape or sound lookups. */
/* Local view: locate_shape_fatal; frontend raw-object exclusion retains its SHAPE2D pointer result. */
extern void FAR * FAR locate_shape_fatal(void FAR*, I8*);

/* Resolve packed four-character shape names into the caller result array; stop at the empty-name terminator.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve shape names in the loaded resource block. */
void locateshapes(I8 FAR *data, I8 *names, I8 FAR **result) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
    while (*names != 0) { *result++ = locate_shape_fatal(data, names); names += 4; }
}

/* Resolve packed shape names without fatal error handling.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve shape names in the loaded resource block. */
void FAR locateshapesz(I8 FAR *data, I8 *names, I8 FAR **results) {
    I16 index = 0;
    while (*names != 0) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
        results[index++] = locateshapez(data, names);
        names += 4;
    }
}

/* Resolve packed sound names with fatal error handling.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve sound names in the loaded resource block. */
void FAR file_find_sounds_required(I8 FAR *data, I8 *names, I8 FAR **results) {
    I16 index = 0;
    while (*names != 0) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
        results[index++] = locate_sound_fatal(data, names);
        names += 4;
    }
}
/* Resolve packed shape names without fatal error handling using indexed output slots.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve shape names in the loaded resource block. */
void FAR file_find_shapes_optional_alt(I8 FAR *data, I8 *names, I8 FAR **results) {
    I16 index = 0;
    while (*names != 0) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
        results[index++] = locateshapez(data, names);
        names += 4;
    }
}
