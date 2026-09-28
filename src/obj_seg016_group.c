#include "stunts_types.h"
/* READABILITY: Walk packed four-character resource-name records and fill result pointers with shape or sound lookups. */
extern I8 FAR * FAR locate_shape_fatal(I8 FAR *data, I8 *name);
/* Resolve packed four-character shape names into the caller result array; stop at the empty-name terminator.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve shape names in the loaded resource block. */
void locate_many_resources(I8 FAR *data, I8 *names, I8 FAR **result) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
    while (*names != 0) { *result++ = locate_shape_fatal(data, names); names += 4; }
}
extern I8 FAR * FAR locate_shape_nofatal(I8 FAR *data, I8 *name);
/* Resolve packed shape names without fatal error handling.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve shape names in the loaded resource block. */
void FAR nopsub_367E4(I8 FAR *data, I8 *names, I8 FAR **results) {
    I16 index = 0;
    while (*names != 0) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
        results[index++] = locate_shape_nofatal(data, names);
        names += 4;
    }
}
extern I8 FAR * FAR locate_sound_fatal(I8 FAR *data, I8 *name);
/* Resolve packed sound names with fatal error handling.
 * Params and return follow the declared C signature. */
/* PLATFORM(file): resolve sound names in the loaded resource block. */
void FAR nopsub_36826(I8 FAR *data, I8 *names, I8 FAR **results) {
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
void FAR nopsub_36868(I8 FAR *data, I8 *names, I8 FAR **results) {
    I16 index = 0;
    while (*names != 0) {
    /* PLATFORM(file): resolve a resource name in the loaded asset block. */
        results[index++] = locate_shape_nofatal(data, names);
        names += 4;
    }
}
