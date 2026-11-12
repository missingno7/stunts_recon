#include "stunts_types.h"
#include "platform_hw.h"
/* READABILITY: Initialize game startup state, set up input/video/audio services, and load the palette and cursor resources. */
/* MSC 5.10 <ctype.h> macros over the pinned runtime table _ctype */
#define _UPPER 0x1
#define _LOWER 0x2
#define isupper(c) ((_ctype+1)[c] & _UPPER)
#define islower(c) ((_ctype+1)[c] & _LOWER)
#define _tolower(c) ((c)-'A'+'a')
#define tolower(c) (isupper(c) ? _tolower(c) : (c))
extern void FAR* load_shape2d_nofatal_thunk(I8* shapename);
/* Forward a nonfatal shape-resource load to its shared loader.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): forward the shape path to the shared file/resource loader. */
void FAR* file_load_shape2d_nofatal2(I8* shapename) { return load_shape2d_nofatal_thunk(shapename); }
    /* PLATFORM(file): use the game file and resource search service. */
extern void FAR makename(I8 *dir, I8 *name, I8 *ext, I8 *dst);
    /* PLATFORM(file): use the game file and resource search service. */
extern I8 * FAR findfirst(I8 *query);
/* Build a directory/name/extension path and ask the file service to find it.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): build a DOS path and request a file search. */
I8 * FAR file_combine_and_find(I8 *dir, I8 *name, I8 *ext)
{
    I8 path[80];
    /* PLATFORM(file): use the game file and resource search service. */
    makename(dir, name, ext, path);
    /* PLATFORM(file): use the game file and resource search service. */
    return findfirst(path);
}
    /* PLATFORM(file): use the game file and resource search service. */
extern const I8* findnext(void);
/* Forward enumeration to the shared file-search service.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(file): continue the shared DOS file search. */
const I8* file_find_next_alt(void) { return findnext(); }
void nullsub_1(void) {}
void nullsub_2(void) {}
struct POINT2D { I16 px, py; };
struct RECTANGLE { I16 left, right, top, bottom; };
extern I16S pixel_scales, g_vid_flg2_set, vidflg3is_minus1, vidflg4_is1;
extern U8 g_videoflg5;
unsigned char g_vid_flag6;
extern I8 textrespfxchr;
I8 audiodriverstring[] = "pc15";
extern U8 _ctype[];
unsigned short slow_video_mode_state;
extern U16S frm_rate2;
extern U16S rate_frame;
extern U16S statemgmtcpy;
extern U8 renderlevel;
extern I16 *material_pattern2_table_ptr, *material_pattern_table_pointer, *material_color_table_pointer, *material_clrlist_ptr;
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
extern void FAR kb_init_interrupt(void);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
extern void FAR kb_shift_checking2(void);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
extern I16 FAR testkey(void);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
extern void FAR interpretsub(I16, void (FAR *)(void));
extern void FAR show_graphic_levels_menu(void);
extern void FAR do_joystick_resource_text(void);
extern void FAR mkeyon(void);
extern void FAR do_mof_resource_text(void);
extern void FAR msystempause(void);
extern void FAR do_dos_resource_text(void);
extern void FAR do_sonsof_resource_text(void);
extern I16S FAR do_dea_textres(void);
    /* PLATFORM(memory): use the game-managed far-memory service. */
extern void FAR initmemory(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR initgraphics(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR video_set_mode4(void);
    /* PLATFORM(timer): use the game timer service. */
extern void FAR timer_setup_interrupt(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR sprite_copy_2_to_1_clear(void);
    /* PLATFORM(input_mouse): initialize DOS mouse bounds for the display. */
extern I16S FAR initmouse(U16S, U16S);
    /* PLATFORM(audio): call the configured DOS audio driver service. */
extern I16S FAR audio_load_driver(I8*, I16S, I16S);
    /* PLATFORM(audio): call the configured DOS audio driver service. */
extern void FAR audio_stop_unknown(void);
extern void FAR exit(I16S);
    /* PLATFORM(audio): call the configured DOS audio driver service. */
extern I16S FAR audio_toggle_flag2(void);
    /* PLATFORM(audio): call the configured DOS audio driver service. */
extern I16S FAR audio_toggle_flag6(void);
extern I16S FAR criterr(I16S (FAR *)(void));
void loadpalandmouse(void);
void seedrandom(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR displaywindow(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR sprset1size(U16S, U16S, U16S, U16S);
    /* PLATFORM(timer): use the game timer service. */
extern I16 FAR timer_get_delta_alt(void);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR sprite_clear_1_color(U8);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR sprite_clear_1_color(U8);
extern void FAR zonecheck(struct POINT2D*, struct RECTANGLE*);
extern void FAR INIT_PALETTE(void*, void*, void*, void*, U16S);
extern U16 FAR strlen(I8*);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern I16 FAR vsyncstate(void);
extern I16 FAR rand(void);
extern I16 FAR get_kevinrandom(void);
    /* PLATFORM(file): read startup shape resources through the game file service. */
extern void FAR * FAR file_load_shape2d_fatal_thunk(I8*);
    /* PLATFORM(file): read startup shape resources through the game file service. */
extern void FAR * FAR locate_shape_fatal(void FAR*, I8*);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR vgapalette(U16S, U16S, U8*);
    /* PLATFORM(memory): use the game-managed far-memory service. */
extern void FAR freememory(void FAR*);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR * FAR sprite_make_window(U16S, U16S, U16S);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR sprite_setup1_from_arg_pointer(void FAR*);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR movfxya(void FAR*, U16S, U16S);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
extern void FAR sprcopy2to12(void);
void far *spritepointermini;
void far *mouse_ptr_cursor;
extern void FAR *mouse_unk_sprite_ptr;


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
void initialize_main(I16 argc, I8* argv[])
{
	register I16 i, j;
	U8 mode_4, nosound, unknown;
	I16 timer1, middle_delta, timerdelta3;
	struct POINT2D tmppoint;
	struct RECTANGLE limits;

	// Keyboard
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_init_interrupt();
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	kb_shift_checking2();
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	testkey();

    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	interpretsub(0x0007, &show_graphic_levels_menu);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	interpretsub(0x000A, &do_joystick_resource_text);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	interpretsub(0x000B, &mkeyon);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	interpretsub(0x3200, &do_mof_resource_text);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	interpretsub(0x0010, &msystempause);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	interpretsub('p', &msystempause);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	interpretsub(0x0011, &do_dos_resource_text);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	interpretsub(0x0013, &do_sonsof_resource_text);
    /* PLATFORM(input_kb): initialize keyboard service or register a game key callback. */
	interpretsub(0x0018, &do_dos_resource_text);
	
	// Video
	pixel_scales = 1;
	g_vid_flg2_set = 1;
	vidflg3is_minus1 = -1;
	vidflg4_is1 = 1;

    /* PLATFORM(memory): use the game-managed far-memory service. */
	initmemory();
	
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

	// Video mode.
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
	initgraphics();
	if (mode_4) {
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
		video_set_mode4();
	}

    /* PLATFORM(timer): use the game timer service. */
	timer_setup_interrupt();

    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
	sprite_copy_2_to_1_clear();

    /* PLATFORM(input_mouse): initialize DOS mouse bounds for the display. */
	initmouse(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS);

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
	criterr(&do_dea_textres);
	
	loadpalandmouse();
	
	// Timing measures.
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
		displaywindow();
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
			zonecheck(&tmppoint, &limits);
		}
		
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
		sprite_clear_1_color(0);
	}
	
    /* PLATFORM(timer): use the game timer service. */
	middle_delta = timer_get_delta_alt();

	for (i = 0; i < 146; ++i) {
		for (j = 0; j < 255; ++j) {
			zonecheck(&tmppoint, &limits);
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
		renderlevel = 0;
	}
	else if (timerdelta3 < 55) {
		renderlevel = 1;
	}
	else if (timerdelta3 < 75) {
		renderlevel = 2;
	}
	else if (timerdelta3 < 100 || !slow_video_mode_state) {
		renderlevel = 3;
	}
	else {
		renderlevel = 4;
	}

	rate_frame = frm_rate2;
	statemgmtcpy = slow_video_mode_state;
	
	seedrandom();
	
	INIT_PALETTE(material_clrlist_ptr, material_color_table_pointer, material_pattern_table_pointer, material_pattern2_table_ptr, 0);
}

/* Wait for a video-status transition and advance the game random generators.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(video): synchronize the startup delay against video status. */
void seedrandom(void)
{
    register I16 status1, i;
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    status1 = vsyncstate();
    i = 0;
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    while (status1 == vsyncstate() && i < 12000)
        ++i;
    if (i == 1024)
        /* PLATFORM(bios): read the BIOS tick-count byte as the fallback seed. */
        i = *((I8S *)0x046c);
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
void loadpalandmouse(void)
{
    U8 colors[PLATFORM_PALETTE_RGB_BYTES];
    U8 FAR *palptr;
    I16 count;
    I16 height;
    void FAR *shape_data;
    void FAR *filedata;
    I16 cursor_width;

    /* PLATFORM(file): read startup shape resources through the game file service. */
    filedata = file_load_shape2d_fatal_thunk("sdmain");
    /* PLATFORM(file): read startup shape resources through the game file service. */
    palptr = locate_shape_fatal(filedata, "!pal");
    palptr += 0x10;
    for (count = 0; count < 0x300; ++count)
        colors[count] = palptr[count];
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    vgapalette(0, PLATFORM_PALETTE_COLOR_COUNT, colors);

    /* PLATFORM(file): read startup shape resources through the game file service. */
    shape_data = locate_shape_fatal(filedata, "smou");
    cursor_width = ((I16S FAR *)shape_data)[0] * g_vid_flg2_set;
    height = ((I16S FAR *)shape_data)[1];
    /* PLATFORM(memory): use the game-managed far-memory service. */
    freememory(filedata);
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
    movfxya(locate_shape_fatal(filedata, "smou"), 0, 0);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    sprite_setup1_from_arg_pointer(mouse_ptr_cursor);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    movfxya(locate_shape_fatal(filedata, "mmou"), 0, 0);
    /* PLATFORM(memory): use the game-managed far-memory service. */
    freememory(filedata);
    /* PLATFORM(video): call the MCGA video or sprite renderer service. */
    sprcopy2to12();
}
I16 get_0(void) { return 0; }
    /* PLATFORM(memory): use the game-managed far-memory service. */
extern void FAR *reservememory(const I8 *name, U16S paras);

/* Convert byte count to paragraph count and request pages from the game memory manager.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(memory): request paragraph-backed storage from the game memory manager. */
void FAR *mmgr_alloc_resbytes(const I8 *name, I32 size)
{
    /* PLATFORM(memory): use the game-managed far-memory service. */
    return reservememory(name, size / 16 + 1);
}
extern U16S unlockedmemory(void);

U32 availmem(void)
{
    /* PLATFORM(memory): read the current game-memory-manager offset difference. */
    return ((U32)unlockedmemory()) << 4;
}
extern U16S FAR modulesize(I8 FAR *ptr);

/* Convert a memory-manager chunk size from paragraphs to bytes.
 * PLATFORM(memory): query the game memory manager chunk record. */
U32 FAR modsize(I8 FAR *ptr)
{
    /* PLATFORM(memory): read this game-memory-manager chunk size. */
    return ((U32)modulesize(ptr)) << 4;
}
