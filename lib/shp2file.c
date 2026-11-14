#include "chartab.h"
#include "ui.h"
#include "math3d.h"
#include "viewstat.h"
#include "game.h"
#include "video.h"
#include "memory.h"
#include "files.h"
#include "types.h"
/* Find, decompress, unflip, expand, and palette-map shape resources in the game memory manager. */

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


/* Load a shape resource with fatal-error reporting enabled.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR* loadshape(I8* shapename) {
	return file_load_shape2d(shapename, 1);
}

/* Load a shape resource without fatal-error reporting.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR* loadshapez(I8* shapename) {
	return file_load_shape2d(shapename, 0);
}

/* Resolve the resource extension, fetch or decompress the file, transform its image pages, and apply its palette map.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
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
			if ((loaded_chunk = checkmodule(file_buffer)) != 0)
				return loaded_chunk;
			if (findfirst(file_buffer))
				break;
		}
	} else {
		if ((loaded_chunk = checkmodule(file_buffer)) != 0)
			return loaded_chunk;
	}

	strcpy(ext, extension_ptr);
	if (stricmp(ext, ".PVS") == 0) {
		loaded_chunk = loadpacka(file_buffer, fatal);
		if (loaded_chunk) {
			work_pages = reservememory("UNFLIP", file_get_unflip_size(loaded_chunk));
			file_unflip_shape2d(loaded_chunk, work_pages);
			purgememory(work_pages);
		}
		return loaded_chunk;
	}
	if (stricmp(ext, ".XVS") == 0) {
		loaded_chunk = loadpacka(file_buffer, fatal);
		return loaded_chunk;
	}
	if (stricmp(ext, ".PES") == 0) {
		loaded_chunk = loadpacka(file_buffer, fatal);
		if (!loaded_chunk)
			return loaded_chunk;
		work_pages = reservememory("UNFLIP", PES_UNFLIP_WORK_PARAGRAPHS);
		file_unflip_shape2d_pes(loaded_chunk, work_pages);
		purgememory(work_pages);
	} else if (stricmp(ext, ".ESH") == 0) {
		loaded_chunk = loadfilea(file_buffer, fatal);
		if (!loaded_chunk)
			return loaded_chunk;
	} else {
		loaded_chunk = loadfilea(file_buffer, fatal);
		return loaded_chunk;
	}

	output_size = file_load_shape2d_expandedsize(loaded_chunk);
	palette_data = locateshapez(loaded_chunk, "!MGA");
	if (palette_data) file_load_shape2d_palmap_init((U8 FAR*)(palette_data + SHAPE2D_HEADER_BYTES));
	work_pages = reservememory(file_buffer, output_size);
	*(U32 FAR*)work_pages = output_size * DOS_PARAGRAPH_BYTES;
	file_load_shape2d_expand(loaded_chunk, work_pages);
	purgememory(loaded_chunk);
	loaded_chunk = packmemory(work_pages);
	file_load_shape2d_palmap_apply(loaded_chunk, palmap);
	return loaded_chunk;
}
/* Copy the resource palette map into the shared shape palette table.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void file_load_shape2d_palmap_init(U8 FAR* pal) {
	I16 i;
	for (i = 0; i < PALETTE_REMAP_ENTRY_COUNT; ++i) {
		palmap[i] = pal[i];
	}
}
