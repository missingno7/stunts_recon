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
extern void far *mmgr_get_chunk_by_name(const char *);
extern const char *file_find(const char *);
extern void far *file_decomp(const char *, int);
extern unsigned short file_get_unflip_size(char far *);
extern void far *mmgr_alloc_pages(const char *, unsigned short);
extern void file_unflip_shape2d(unsigned char far *, char far *);
extern void file_unflip_shape2d_pes(unsigned char far *, char far *);
extern void mmgr_release(void far *);
extern void far *file_load_binary(const char *, int);
extern unsigned short file_load_shape2d_expandedsize(void far *);
extern char far *locate_shape_nofatal(char far *, const char *);
extern void file_load_shape2d_palmap_init(unsigned char far *);
extern void file_load_shape2d_expand(unsigned char far *, char far *);
extern char far *mmgr_op_unk(char far *);
extern void file_load_shape2d_palmap_apply(unsigned char far *, unsigned char []);
void far *file_load_shape2d(char *, int);
void far* file_load_shape2d_fatal(char* shapename) {
	return file_load_shape2d(shapename, 1);
}

void far* file_load_shape2d_nofatal(char* shapename) {
	return file_load_shape2d(shapename, 0);
}

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
			if ((loaded_chunk = mmgr_get_chunk_by_name(file_buffer)) != 0)
				return loaded_chunk;
			if (file_find(file_buffer))
				break;
		}
	} else {
		if ((loaded_chunk = mmgr_get_chunk_by_name(file_buffer)) != 0)
			return loaded_chunk;
	}

	strcpy(ext, extension_ptr);
	if (stricmp(ext, ".PVS") == 0) {
		loaded_chunk = file_decomp(file_buffer, fatal);
		if (loaded_chunk) {
			work_pages = mmgr_alloc_pages("UNFLIP", file_get_unflip_size(loaded_chunk));
			file_unflip_shape2d(loaded_chunk, work_pages);
			mmgr_release(work_pages);
		}
		return loaded_chunk;
	}
	if (stricmp(ext, ".XVS") == 0) {
		loaded_chunk = file_decomp(file_buffer, fatal);
		return loaded_chunk;
	}
	if (stricmp(ext, ".PES") == 0) {
		loaded_chunk = file_decomp(file_buffer, fatal);
		if (!loaded_chunk)
			return loaded_chunk;
		work_pages = mmgr_alloc_pages("UNFLIP", 1000);
		file_unflip_shape2d_pes(loaded_chunk, work_pages);
		mmgr_release(work_pages);
	} else if (stricmp(ext, ".ESH") == 0) {
		loaded_chunk = file_load_binary(file_buffer, fatal);
		if (!loaded_chunk)
			return loaded_chunk;
	} else {
		loaded_chunk = file_load_binary(file_buffer, fatal);
		return loaded_chunk;
	}

	output_size = file_load_shape2d_expandedsize(loaded_chunk);
	palette_data = locate_shape_nofatal(loaded_chunk, "!MGA");
	if (palette_data) file_load_shape2d_palmap_init((unsigned char far*)(palette_data + 16));
	work_pages = mmgr_alloc_pages(file_buffer, output_size);
	*(unsigned long far*)work_pages = (unsigned long)output_size * 16UL;
	file_load_shape2d_expand((unsigned char far*)loaded_chunk, (char far*)work_pages);
	mmgr_release(loaded_chunk);
	loaded_chunk = mmgr_op_unk((char far*)work_pages);
	file_load_shape2d_palmap_apply((unsigned char far*)loaded_chunk, palmap);
	return loaded_chunk;
}
void file_load_shape2d_palmap_init(unsigned char far* pal) {
	int i;
	
	for (i = 0; i < 0x10; ++i) {
		palmap[i] = pal[i];
	}
}
