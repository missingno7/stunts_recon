#include "stunts_types.h"
/* READABILITY: Find, decompress, unflip, expand, and palette-map shape resources in the game memory manager. */

#define SHAPE2D_HEADER_BYTES 16
#define PALETTE_REMAP_ENTRY_COUNT 16
I8 shape_extensions[] = ".PVS\0.XVS\0.VSH\0.PES\0.ESH\0";
I8 *shape2d_extension_suffixes[6] = {
    shape_extensions,
    shape_extensions + 5,
    shape_extensions + 10,
    shape_extensions + 15,
    shape_extensions + 20,
    shape_extensions + 25
};
U8 palmap[PALETTE_REMAP_ENTRY_COUNT] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15
};
extern I8 *strcpy(I8 *, const I8 *);
extern I16 stricmp(const I8 *, const I8 *);
extern void fatal_error(const I8 *, ...);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void FAR *mmgr_get_chunk_by_name(const I8 *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern const I8 *file_find(const I8 *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void FAR *file_decomp(const I8 *, I16);
extern U16S file_get_unflip_size(I8 FAR *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void FAR *mmgr_alloc_pages(const I8 *, U16S);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_unflip_shape2d(U8 FAR *, I8 FAR *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_unflip_shape2d_pes(U8 FAR *, I8 FAR *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void mmgr_release(void FAR *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void FAR *file_load_binary(const I8 *, I16);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern U16S file_load_shape2d_expandedsize(void FAR *);
extern I8 FAR *locate_shape_nofatal(I8 FAR *, const I8 *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_load_shape2d_palmap_init(U8 FAR *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_load_shape2d_expand(U8 FAR *, I8 FAR *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern I8 FAR *mmgr_op_unk(I8 FAR *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_load_shape2d_palmap_apply(U8 FAR *, U8 []);
void FAR *file_load_shape2d(I8 *, I16);
/* Load a shape resource with fatal-error reporting enabled.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): request the shape through the game resource loader. */
void FAR* file_load_shape2d_fatal(I8* shapename) {
    /* PLATFORM(file): request this shape through the shared resource loader. */
	return file_load_shape2d(shapename, 1);
}

/* Load a shape resource without fatal-error reporting.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): request the shape through the game resource loader. */
void FAR* file_load_shape2d_nofatal(I8* shapename) {
    /* PLATFORM(file): request this shape through the shared resource loader. */
	return file_load_shape2d(shapename, 0);
}

/* Resolve the resource extension, fetch or decompress the file, transform its image pages, and apply its palette map.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): find, load, and decompress shape resource files. */
/* PLATFORM(memory): allocate and release game paragraph pages for shape transforms. */
void FAR* file_load_shape2d(I8* shapename, I16 fatal) {
	I8 ext[6];
	void FAR* loaded_chunk;
	void FAR* work_pages;
	I8* extension_ptr;
	I8 FAR* palette_data;
	I16 ext_index;
	I8* old_suffix_ptr;
	I8 file_buffer[100];
	U16S output_size;
	I8 character;

	strcpy(file_buffer, shapename);
	extension_ptr = file_buffer;
	while ((character = *extension_ptr) && character != '.') {
		extension_ptr++;
	}

	if (*extension_ptr == 0) {
		for (ext_index = 0; *shape2d_extension_suffixes[ext_index]; ext_index++) {
			old_suffix_ptr = extension_ptr;
			strcpy(old_suffix_ptr, shape2d_extension_suffixes[ext_index]);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
			if ((loaded_chunk = mmgr_get_chunk_by_name(file_buffer)) != 0)
				return loaded_chunk;
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
			if (file_find(file_buffer))
				break;
		}
	} else {
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
		if ((loaded_chunk = mmgr_get_chunk_by_name(file_buffer)) != 0)
			return loaded_chunk;
	}

	strcpy(ext, extension_ptr);
	if (stricmp(ext, ".PVS") == 0) {
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
		loaded_chunk = file_decomp(file_buffer, fatal);
		if (loaded_chunk) {
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
			work_pages = mmgr_alloc_pages("UNFLIP", file_get_unflip_size(loaded_chunk));
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
			file_unflip_shape2d(loaded_chunk, work_pages);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
			mmgr_release(work_pages);
		}
		return loaded_chunk;
	}
	if (stricmp(ext, ".XVS") == 0) {
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
		loaded_chunk = file_decomp(file_buffer, fatal);
		return loaded_chunk;
	}
	if (stricmp(ext, ".PES") == 0) {
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
		loaded_chunk = file_decomp(file_buffer, fatal);
		if (!loaded_chunk)
			return loaded_chunk;
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
		work_pages = mmgr_alloc_pages("UNFLIP", 1000);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
		file_unflip_shape2d_pes(loaded_chunk, work_pages);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
		mmgr_release(work_pages);
	} else if (stricmp(ext, ".ESH") == 0) {
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
		loaded_chunk = file_load_binary(file_buffer, fatal);
		if (!loaded_chunk)
			return loaded_chunk;
	} else {
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
		loaded_chunk = file_load_binary(file_buffer, fatal);
		return loaded_chunk;
	}

    /* PLATFORM(file): read or transform a shape resource through the game file service. */
	output_size = file_load_shape2d_expandedsize(loaded_chunk);
	palette_data = locate_shape_nofatal(loaded_chunk, "!MGA");
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
	if (palette_data) file_load_shape2d_palmap_init((U8 FAR*)(palette_data + SHAPE2D_HEADER_BYTES));
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
	work_pages = mmgr_alloc_pages(file_buffer, output_size);
	*(U32 FAR*)work_pages = (U32)output_size * 16UL;
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
	file_load_shape2d_expand((U8 FAR*)loaded_chunk, (I8 FAR*)work_pages);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
	mmgr_release(loaded_chunk);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
	loaded_chunk = mmgr_op_unk((I8 FAR*)work_pages);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
	file_load_shape2d_palmap_apply((U8 FAR*)loaded_chunk, palmap);
	return loaded_chunk;
}
/* Copy the resource palette map into the shared shape palette table.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
void file_load_shape2d_palmap_init(U8 FAR* pal) {
	I16 i;
	
	for (i = 0; i < PALETTE_REMAP_ENTRY_COUNT; ++i) {
		palmap[i] = pal[i];
	}
}
