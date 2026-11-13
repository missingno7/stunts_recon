#include "audio.h"
#include "files.h"
#include "types.h"
/* Walk packed four-character resource-name records and fill result pointers with shape or sound lookups. */
/* Local view: locate_shape_fatal; frontend raw-object exclusion retains its SHAPE2D pointer result. */
extern void FAR * FAR locate_shape_fatal(void FAR*, I8*);

/* Resolve packed four-character shape names into the caller result array; stop at the empty-name terminator.
 * Params and return follow the declared C signature. */
void locateshapes(I8 FAR *data, I8 *names, I8 FAR **result) {
    while (*names != 0) { *result++ = locate_shape_fatal(data, names); names += 4; }
}

/* Resolve packed shape names without fatal error handling.
 * Params and return follow the declared C signature. */
void FAR locateshapesz(I8 FAR *data, I8 *names, I8 FAR **results) {
    I16 index = 0;
    while (*names != 0) {
        results[index++] = locateshapez(data, names);
        names += 4;
    }
}

/* Resolve packed sound names with fatal error handling.
 * Params and return follow the declared C signature. */
void FAR file_find_sounds_required(I8 FAR *data, I8 *names, I8 FAR **results) {
    I16 index = 0;
    while (*names != 0) {
        results[index++] = locate_sound_fatal(data, names);
        names += 4;
    }
}
/* Resolve packed shape names without fatal error handling using indexed output slots.
 * Params and return follow the declared C signature. */
void FAR file_find_shapes_optional_alt(I8 FAR *data, I8 *names, I8 FAR **results) {
    I16 index = 0;
    while (*names != 0) {
        results[index++] = locateshapez(data, names);
        names += 4;
    }
}
