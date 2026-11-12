
#include "audapi.h"
#include "resapi.h"
#include "stunts_types.h"
/* READABILITY: Walk packed four-character resource-name records and fill result pointers with shape or sound lookups. */
/* Local view: locate_shape_fatal; return.pointer is signed byte; another unit uses struct SHAPE2D. */
extern I8 FAR * FAR locate_shape_fatal(I8 FAR *data, I8 *name);
/* Resolve packed four-character shape names into the caller result array; stop at the empty-name terminator.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve shape names in the loaded resource block. */
/* Local view: locate_many_resources; parameters[0].pointer is signed byte; another unit uses void. */
void locateshapes(I8 FAR *data, I8 *names, I8 FAR **result) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
    while (*names != 0) { *result++ = locate_shape_fatal(data, names); names += 4; }
}
/* Local view: locateshapez; parameters is [{'pointer': 'i8', 'qualifiers': [], 'segment': 'far'}, {'pointer': 'i8', 'qualifiers': [], 'segment': 'near'}]; another unit uses None. */
extern I8 FAR * FAR locateshapez(I8 FAR *data, I8 *name);
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
