#include "platform_hw.h"
/* READABILITY: Initialize game startup state, set up input/video/audio services, and load the palette and cursor resources. */
/* MSC 5.10 <ctype.h> macros over the pinned runtime table _ctype */
#define _UPPER 0x1
#define _LOWER 0x2
#define isupper(c) ((_ctype+1)[c] & _UPPER)
#define islower(c) ((_ctype+1)[c] & _LOWER)
#define _tolower(c) ((c)-'A'+'a')
#define tolower(c) (isupper(c) ? _tolower(c) : (c))
extern void far* load_shape2d_nofatal_thunk(char* shapename);
/* Forward a nonfatal shape-resource load to its shared loader.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): forward the shape path to the shared file/resource loader. */
void far* file_load_shape2d_nofatal2(char* shapename) { return load_shape2d_nofatal_thunk(shapename); }
    /* PLATFORM(file): use the game file and resource search service. */
extern void far file_build_path(char *dir, char *name, char *ext, char *dst);
    /* PLATFORM(file): use the game file and resource search service. */
extern char * far file_find(char *query);
/* Build a directory/name/extension path and ask the file service to find it.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): build a DOS path and request a file search. */
char * far file_combine_and_find(char *dir, char *name, char *ext)
{
    char path[80];
    /* PLATFORM(file): use the game file and resource search service. */
    file_build_path(dir, name, ext, path);
    /* PLATFORM(file): use the game file and resource search service. */
    return file_find(path);
}
    /* PLATFORM(file): use the game file and resource search service. */
extern const char* file_find_next(void);
/* Forward enumeration to the shared file-search service.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): continue the shared DOS file search. */
const char* file_find_next_alt(void) { return file_find_next(); }
void nullsub_1(void) {}
void nullsub_2(void) {}
struct POINT2D { int px, py; };
struct RECTANGLE { int left, right, top, bottom; };
extern short pixel_scales, g_vid_flg2_set, vidflg3is_minus1, vidflg4_is1;
extern unsigned char g_videoflg5;
unsigned char g_vid_flag6;
extern char textrespfxchr;
char audiodriverstring[] = "pc15";
extern unsigned char _ctype[];
unsigned short slow_video_mode_state;
extern unsigned short frm_rate2;
extern unsigned short rate_frame;
extern unsigned short statemgmtcpy;
extern unsigned char detail_lvl;
extern int *material_pattern2_table_ptr, *material_pattern_table_pointer, *material_color_table_pointer, *material_clrlist_ptr;
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
extern void far kb_init_interrupt(void);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
extern void far kb_shift_checking2(void);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
extern int far kb_call_readchar_callback(void);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
extern void far kb_reg_callback(int, void (far *)(void));
extern void far show_graphic_levels_menu(void);
extern void far do_joystick_resource_text(void);
extern void far do_key_resource_text(void);
extern void far do_mof_resource_text(void);
extern void far do_pau_restext(void);
extern void far do_dos_resource_text(void);
extern void far do_sonsof_resource_text(void);
extern short far do_dea_textres(void);
    /* PLATFORM(memory): use the game-managed far-memory service. */
extern void far mmgr_alloc_a000(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far video_set_mode_13h(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far video_set_mode4(void);
    /* PLATFORM(timer): use the game timer service. */
extern void far timer_setup_interrupt(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far sprite_copy_2_to_1_clear(void);
    /* PLATFORM(input_mouse): initialize DOS mouse bounds for the display. */
extern short far mouse_init(unsigned short, unsigned short);
    /* PLATFORM(audio): call the configured DOS audio driver service. */
extern short far audio_load_driver(char*, short, short);
    /* PLATFORM(audio): call the configured DOS audio driver service. */
extern void far audio_stop_unknown(void);
extern void far exit(short);
    /* PLATFORM(audio): call the configured DOS audio driver service. */
extern short far audio_toggle_flag2(void);
    /* PLATFORM(audio): call the configured DOS audio driver service. */
extern short far audio_toggle_flag6(void);
extern short far set_criterr_handler(short (far *)(void));
void load_palandcursor(void);
void random_wait(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far sprite_copy_2_to_1(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far sprset1size(unsigned short, unsigned short, unsigned short, unsigned short);
    /* PLATFORM(timer): use the game timer service. */
extern int far timer_get_delta_alt(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far sprite_clear_1_color(unsigned char);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far sprite_clear_1_color(unsigned char);
extern void far rect_adjust_from_point(struct POINT2D*, struct RECTANGLE*);
extern void far copy_material_list_pointers(void*, void*, void*, void*, unsigned short);
extern unsigned int far strlen(char*);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern int far video_get_status(void);
extern int far rand(void);
extern int far get_kevinrandom(void);
    /* PLATFORM(file): read startup shape resources through the game file service. */
extern void far * far file_load_shape2d_fatal_thunk(char*);
    /* PLATFORM(file): read startup shape resources through the game file service. */
extern void far * far locate_shape_fatal(void far*, char*);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far video_set_palette(unsigned short, unsigned short, unsigned char*);
    /* PLATFORM(memory): use the game-managed far-memory service. */
extern void far mmgr_free(void far*);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far * far sprite_make_window(unsigned short, unsigned short, unsigned short);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far sprite_setup1_from_arg_pointer(void far*);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far sprite_shape_to_1(void far*, unsigned short, unsigned short);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void far sprcopy2to12(void);
void far *spritepointermini;
void far *mouse_ptr_cursor;
extern void far *mouse_unk_sprite_ptr;


/* Initialize game input, video, timer, mouse, audio, and startup resources; choose timing-dependent detail settings.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_kb): initialize keyboard input and install game key callbacks. */
/* PLATFORM(dos): install the DOS critical-error hook and exit on startup failure. */
/* PLATFORM(video): select video mode and initialize the startup render surfaces. */
/* PLATFORM(timer): initialize the game timer and measure startup rendering speed. */
/* PLATFORM(input_mouse): initialize the DOS mouse for the 320x200 screen. */
/* PLATFORM(audio): initialize or disable the selected DOS sound driver. */
/* PLATFORM(file): load the startup shape and palette resources. */
void initialize_main(int argc, char* argv[])
{
	register int i, j;
	unsigned char mode_4, nosound, unknown;
	int timer1, middle_delta, timerdelta3;
	struct POINT2D tmppoint;
	struct RECTANGLE limits;

	// Keyboard
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_init_interrupt();
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_shift_checking2();
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_call_readchar_callback();

    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_reg_callback(0x0007, &show_graphic_levels_menu);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_reg_callback(0x000A, &do_joystick_resource_text);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_reg_callback(0x000B, &do_key_resource_text);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_reg_callback(0x3200, &do_mof_resource_text);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_reg_callback(0x0010, &do_pau_restext);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_reg_callback('p', &do_pau_restext);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_reg_callback(0x0011, &do_dos_resource_text);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_reg_callback(0x0013, &do_sonsof_resource_text);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_reg_callback(0x0018, &do_dos_resource_text);
	
	// Video
	pixel_scales = 1;
	g_vid_flg2_set = 1;
	vidflg3is_minus1 = -1;
	vidflg4_is1 = 1;

    /* PLATFORM(memory): use the game-managed far-memory service. */
	mmgr_alloc_a000();
	
	g_videoflg5 = 0;
	g_vid_flag6 = 1;
	
	textrespfxchr = 'e';
	
	// Parse arguments.
	mode_4 = 0;
	nosound = 0;
	unknown = 0;
	
	for (i = 1; argc > i; ++i) {
		if (argv[i][0] == '/') {
			switch (argv[i][1]) {
				case 'h':
					mode_4 = 1;
					break;

				case 's':
                    if (tolower(argv[i][2]) == 's' && tolower(argv[i][3]) == 'b') {
                        audiodriverstring[0] = 'a';
                        audiodriverstring[1] = 'd';
                    }
                    else {
                        audiodriverstring[0] = argv[i][2];
                        audiodriverstring[1] = argv[i][3];
                    }
                    break;

                case 'n':
					if (argv[i][2] == 's') {
						nosound = 1;
					}
					else if (argv[i][2] == 'd') {
						unknown = 1;
					}
					break;
			}
		}
	}
	
	// Unused "/nd" switch. Maybe used when loading other video drivers?
	(void)unknown;

	// Video mode.
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
	video_set_mode_13h();
	if (mode_4) {
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
		video_set_mode4();
	}

    /* PLATFORM(timer): use the game timer service. */
	timer_setup_interrupt();

    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
	sprite_copy_2_to_1_clear();

    /* PLATFORM(input_mouse): initialize DOS mouse bounds for the display. */
	mouse_init(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS);

	// Audio driver.
    /* PLATFORM(audio): call the configured DOS audio driver service. */
	if (audio_load_driver(audiodriverstring, 0, 0)) {
    /* PLATFORM(audio): call the configured DOS audio driver service. */
		audio_stop_unknown();
    /* PLATFORM(dos): terminate the DOS process on startup failure. */
		exit(1);
	}
	
	if (nosound) {
    /* PLATFORM(audio): call the configured DOS audio driver service. */
		audio_toggle_flag2();
    /* PLATFORM(audio): call the configured DOS audio driver service. */
		audio_toggle_flag6();
	}
	
    /* PLATFORM(dos): install the DOS critical-error callback. */
	set_criterr_handler(&do_dea_textres);
	
	load_palandcursor();
	
	// Timing measures.
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
		sprite_copy_2_to_1();
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
	sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, 120);

    /* PLATFORM(timer): use the game timer service. */
	timer_get_delta_alt();
	for (i = 0; i < 15; ++i) {
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
		sprite_clear_1_color(0); // the c impl is too slow/wrong and produces faulty timing values
	}
    /* PLATFORM(timer): use the game timer service. */
	timer1 = timer_get_delta_alt();
	
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
	sprset1size(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, 60);

	for (i = 0; i < 15; ++i) {
		limits.left = 0;
		limits.right = 0;
		limits.top = 0;
		limits.bottom = 0;
		
		for (j = 0; j < 400; ++j) {
			tmppoint.py = tmppoint.px = j;
			rect_adjust_from_point(&tmppoint, &limits);
		}
		
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
		sprite_clear_1_color(0);
	}
	
    /* PLATFORM(timer): use the game timer service. */
	middle_delta = timer_get_delta_alt();

	for (i = 0; i < 146; ++i) {
		for (j = 0; j < 255; ++j) {
			rect_adjust_from_point(&tmppoint, &limits);
		}
	}
	
    /* PLATFORM(timer): use the game timer service. */
	timerdelta3 = timer_get_delta_alt();
	
	if (middle_delta > timer1)
		slow_video_mode_state = 0;
	else
		slow_video_mode_state = 1;
	if (timerdelta3 < 75)
		frm_rate2 = 20;
	else
		frm_rate2 = 10;

	if (timerdelta3 < 35) {
		detail_lvl = 0;
	}
	else if (timerdelta3 < 55) {
		detail_lvl = 1;
	}
	else if (timerdelta3 < 75) {
		detail_lvl = 2;
	}
	else if (timerdelta3 < 100 || !slow_video_mode_state) {
		detail_lvl = 3;
	}
	else {
		detail_lvl = 4;
	}

	rate_frame = frm_rate2;
	statemgmtcpy = slow_video_mode_state;
	
	random_wait();
	
	copy_material_list_pointers(material_clrlist_ptr, material_color_table_pointer, material_pattern_table_pointer, material_pattern2_table_ptr, 0);
}

/* Wait for a video-status transition and advance the game random generators.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(video): synchronize the startup delay against video status. */
void random_wait(void)
{
    register int status1, i;
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    status1 = video_get_status();
    i = 0;
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    while (status1 == video_get_status() && i < 12000)
        ++i;
    if (i == 1024)
        /* PLATFORM(bios): read the BIOS tick-count byte as the fallback seed. */
        i = *((signed char *)0x046c);
    while (i--) { rand(); get_kevinrandom(); }
    i &= 0xff;
    while (i--) { get_kevinrandom(); rand(); }
}

/* Load the startup palette and mouse cursor shapes, then copy them to the render surfaces.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): load the palette and cursor data through the game file service. */
/* PLATFORM(video): upload the palette and copy cursor shapes to render surfaces. */
/* PLATFORM(memory): release loaded shape pages after copying the cursor. */
void load_palandcursor(void)
{
    unsigned char colors[PLATFORM_PALETTE_RGB_BYTES];
    unsigned char far *palptr;
    int count;
    int height;
    void far *shape_data;
    void far *filedata;
    int cursor_width;

    /* PLATFORM(file): read startup shape resources through the game file service. */
    filedata = file_load_shape2d_fatal_thunk("sdmain");
    /* PLATFORM(file): read startup shape resources through the game file service. */
    palptr = (unsigned char far *)locate_shape_fatal(filedata, "!pal");
    palptr += 0x10;
    for (count = 0; count < 0x300; ++count)
        colors[count] = palptr[count];
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    video_set_palette(0, PLATFORM_PALETTE_COLOR_COUNT, colors);

    /* PLATFORM(file): read startup shape resources through the game file service. */
    shape_data = locate_shape_fatal(filedata, "smou");
    cursor_width = ((short far *)shape_data)[0] * g_vid_flg2_set;
    height = ((short far *)shape_data)[1];
    /* PLATFORM(memory): use the game-managed far-memory service. */
    mmgr_free(filedata);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    spritepointermini = sprite_make_window(cursor_width, height, PLATFORM_VGA_COLOR_WHITE);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    mouse_ptr_cursor = sprite_make_window(cursor_width, height, PLATFORM_VGA_COLOR_WHITE);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    mouse_unk_sprite_ptr = sprite_make_window(cursor_width + g_vid_flg2_set, height, PLATFORM_VGA_COLOR_WHITE);

    /* PLATFORM(file): read startup shape resources through the game file service. */
    filedata = file_load_shape2d_fatal_thunk("sdmain");
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    sprite_setup1_from_arg_pointer(spritepointermini);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    sprite_shape_to_1(locate_shape_fatal(filedata, "smou"), 0, 0);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    sprite_setup1_from_arg_pointer(mouse_ptr_cursor);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    sprite_shape_to_1(locate_shape_fatal(filedata, "mmou"), 0, 0);
    /* PLATFORM(memory): use the game-managed far-memory service. */
    mmgr_free(filedata);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    sprcopy2to12();
}
int get_0(void) { return 0; }
    /* PLATFORM(memory): use the game-managed far-memory service. */
extern void far *mmgr_alloc_pages(const char *name, unsigned short paras);

/* Convert byte count to paragraph count and request pages from the game memory manager.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(memory): request paragraph-backed storage from the game memory manager. */
void far *mmgr_alloc_resbytes(const char *name, long int size)
{
    /* PLATFORM(memory): use the game-managed far-memory service. */
    return mmgr_alloc_pages(name, size / 16 + 1);
}
extern unsigned short mmgr_get_ofs_diff(void);

unsigned long mmgr_get_res_ofs_diff_scaled(void)
{
    /* PLATFORM(memory): read the current game-memory-manager offset difference. */
    return ((unsigned long)mmgr_get_ofs_diff()) << 4;
}
extern unsigned short far mmgr_get_chunk_size(char far *ptr);

/* Convert a memory-manager chunk size from paragraphs to bytes.
 * PLATFORM(memory): query the game memory manager chunk record. */
unsigned long far mmgr_get_chunk_size_bytes(char far *ptr)
{
    /* PLATFORM(memory): read this game-memory-manager chunk size. */
    return ((unsigned long)mmgr_get_chunk_size(ptr)) << 4;
}
