/* READABILITY: Find, decompress, unflip, expand, and palette-map shape resources in the game memory manager. */
char shape_extensions[] = ".PVS\0.XVS\0.VSH\0.PES\0.ESH\0";
char *shape2d_extension_suffixes[6] = {
    shape_extensions,
    shape_extensions + 5,
    shape_extensions + 10,
    shape_extensions + 15,
    shape_extensions + 20,
    shape_extensions + 25
};
unsigned char palmap[16] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15
};
extern char *strcpy(char *, const char *);
extern int stricmp(const char *, const char *);
extern void fatal_error(const char *, ...);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void far *mmgr_get_chunk_by_name(const char *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern const char *file_find(const char *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void far *file_decomp(const char *, int);
extern unsigned short file_get_unflip_size(char far *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void far *mmgr_alloc_pages(const char *, unsigned short);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_unflip_shape2d(unsigned char far *, char far *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_unflip_shape2d_pes(unsigned char far *, char far *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern void mmgr_release(void far *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void far *file_load_binary(const char *, int);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern unsigned short file_load_shape2d_expandedsize(void far *);
extern char far *locate_shape_nofatal(char far *, const char *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_load_shape2d_palmap_init(unsigned char far *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_load_shape2d_expand(unsigned char far *, char far *);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
extern char far *mmgr_op_unk(char far *);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
extern void file_load_shape2d_palmap_apply(unsigned char far *, unsigned char []);
void far *file_load_shape2d(char *, int);
/* Load a shape resource with fatal-error reporting enabled.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): request the shape through the game resource loader. */
void far* file_load_shape2d_fatal(char* shapename) {
    /* PLATFORM(file): request this shape through the shared resource loader. */
	return file_load_shape2d(shapename, 1);
}

/* Load a shape resource without fatal-error reporting.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): request the shape through the game resource loader. */
void far* file_load_shape2d_nofatal(char* shapename) {
    /* PLATFORM(file): request this shape through the shared resource loader. */
	return file_load_shape2d(shapename, 0);
}

/* Resolve the resource extension, fetch or decompress the file, transform its image pages, and apply its palette map.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): find, load, and decompress shape resource files. */
/* PLATFORM(memory): allocate and release game paragraph pages for shape transforms. */
void far* file_load_shape2d(char* shapename, int fatal) {
	char ext[6];
	void far* loaded_chunk;
	void far* work_pages;
	char* extension_ptr;
	char far* palette_data;
	int ext_index;
	char* old_suffix_ptr;
	char file_buffer[100];
	unsigned short output_size;
	char character;

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
	if (palette_data) file_load_shape2d_palmap_init((unsigned char far*)(palette_data + 16));
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
	work_pages = mmgr_alloc_pages(file_buffer, output_size);
	*(unsigned long far*)work_pages = (unsigned long)output_size * 16UL;
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
	file_load_shape2d_expand((unsigned char far*)loaded_chunk, (char far*)work_pages);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
	mmgr_release(loaded_chunk);
    /* PLATFORM(memory): use game-managed storage for the shape resource. */
	loaded_chunk = mmgr_op_unk((char far*)work_pages);
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
	file_load_shape2d_palmap_apply((unsigned char far*)loaded_chunk, palmap);
	return loaded_chunk;
}
/* Copy the resource palette map into the shared shape palette table.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
    /* PLATFORM(file): read or transform a shape resource through the game file service. */
void file_load_shape2d_palmap_init(unsigned char far* pal) {
	int i;
	
	for (i = 0; i < 0x10; ++i) {
		palmap[i] = pal[i];
	}
}
