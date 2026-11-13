#include "audio.h"
#include "chartab.h"
#include "ui.h"
#include "math3d.h"
#include "viewstat.h"
#include "game.h"
#include "video.h"
#include "input.h"
#include "memory.h"
#include "shapes3d.h"
#include "files.h"
#include "timer.h"
#include "dlgcolor.h"
#include "types.h"
/* Initialize game startup state, set up input/video/audio services, and load the palette and cursor resources. */
/* MSC 5.10 <ctype.h> macros over the pinned runtime table _ctype */
#define _UPPER 0x1
#define _LOWER 0x2
#define isupper(c) ((_ctype+1)[c] & _UPPER)
#define islower(c) ((_ctype+1)[c] & _LOWER)
#define _tolower(c) ((c)-'A'+'a')
#define tolower(c) (isupper(c) ? _tolower(c) : (c))

/* Forward a nonfatal shape-resource load to its shared loader.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR* file_load_shape2d_nofatal2(I8* shapename) { return load_shape2d_nofatal_thunk(shapename); }


/* Build a directory/name/extension path and ask the file service to find it.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
I8 * FAR file_combine_and_find(I8 *dir, I8 *name, I8 *ext)
{
    I8 path[80];
    makename(dir, name, ext, path);
    return findfirst(path);
}

/* Forward enumeration to the shared file-search service.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
const I8* myfindnext(void) { return findnext(); }
void nullsub_1(void) {}
void nullsub_2(void) {}
unsigned char g_vid_flag6;

I8 audiodriverstring[] = "pc15";

unsigned short slow_video_mode_state;
/* Local view: material_clrlist_ptr; renderer retains its excluded MATERIALCLRLIST pointer. */
extern I16* material_clrlist_ptr;


/* Local view: locate_shape_fatal; frontend raw-object exclusion retains its SHAPE2D pointer result. */
extern void FAR * FAR locate_shape_fatal(void FAR*, I8*);


struct SPRITE far *spritepointermini;
struct SPRITE far *mouse_ptr_cursor;

/* Initialize game input, video, timer, mouse, audio, and startup resources; choose timing-dependent detail settings.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void initmodes(I16 argc, I8* argv[])
{
	register I16 i, j;
	U8 mode_4, nosound, unknown;
	I16 timer1, middle_delta, timerdelta3;
	struct POINT2D tmppoint;
	struct RECTANGLE limits;

	// Keyboard
	kb_init_interrupt();
	kb_shift_checking2();
	testkey();

	interpretsub(0x0007, &msetrender);
	interpretsub(0x000A, &do_joystick_resource_text);
	interpretsub(0x000B, &mkeyon);
	interpretsub(0x3200, &mmusictog);
	interpretsub(0x0010, &msystempause);
	interpretsub('p', &msystempause);
	interpretsub(0x0011, &mdosexit);
	interpretsub(0x0013, &msoundtog);
	interpretsub(0x0018, &mdosexit);
	// Video
	pixel_scales = 1;
	g_vid_flg2_set = 1;
	vidflg3is_minus1 = -1;
	vidflg4_is1 = 1;

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
	initgraphics();
	if (mode_4) {
		video_set_mode4();
	}

	timer_setup_interrupt();

	usescreenc();

	initmouse(PLATFORM_SCREEN_WIDTH_PIXELS, PLATFORM_SCREEN_HEIGHT_PIXELS);

	// Audio driver.
	if (audio_load_driver(audiodriverstring, 0, 0)) {
		restoretimer();
		exit(1);
	}
	if (nosound) {
		audio_toggle_flag2();
		audio_toggle_flag6();
	}
	criterr(&do_dea_textres);
	loadpalandmouse();
	// Timing measures.
		displaywindow();
	clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, STARTUP_VIDEO_BENCH_HEIGHT);

	elticks();
	for (i = 0; i < STARTUP_VIDEO_BENCH_REPEATS; ++i) {
		clearwindow(0); // the c impl is too slow/wrong and produces faulty timing values
	}
	timer1 = elticks();
	clippix(0, PLATFORM_SCREEN_WIDTH_PIXELS, 0, STARTUP_RECT_BENCH_HEIGHT);

	for (i = 0; i < STARTUP_VIDEO_BENCH_REPEATS; ++i) {
		limits.left = 0;
		limits.right = 0;
		limits.top = 0;
		limits.bottom = 0;
		for (j = 0; j < STARTUP_RECT_BENCH_POINTS; ++j) {
			tmppoint.y = tmppoint.x = j;
			zonecheck(&tmppoint, &limits);
		}
		clearwindow(0);
	}
	middle_delta = elticks();

	for (i = 0; i < STARTUP_CPU_BENCH_OUTER; ++i) {
		for (j = 0; j < STARTUP_CPU_BENCH_INNER; ++j) {
			zonecheck(&tmppoint, &limits);
		}
	}
	timerdelta3 = elticks();
	if (middle_delta > timer1)
		slow_video_mode_state = 0;
	else
		slow_video_mode_state = 1;
	if (timerdelta3 < 75)
		frm_rate2 = 20;
	else
		frm_rate2 = 10;

	if (timerdelta3 < STARTUP_DETAIL_THRESHOLD_LOWEST) {
		renderlevel = 0;
	}
	else if (timerdelta3 < STARTUP_DETAIL_THRESHOLD_LOW) {
		renderlevel = 1;
	}
	else if (timerdelta3 < STARTUP_DETAIL_THRESHOLD_MEDIUM) {
		renderlevel = 2;
	}
	else if (timerdelta3 < STARTUP_DETAIL_THRESHOLD_HIGH || !slow_video_mode_state) {
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
void seedrandom(void)
{
    register I16 status1, i;
    status1 = vsyncstate();
    i = 0;
    while (status1 == vsyncstate() && i < STARTUP_VIDEO_WAIT_LIMIT)
        ++i;
    if (i == STARTUP_RANDOM_FALLBACK_CHECK)
        i = *((I8S *)0x046c);
    while (i--) { rand(); myrandom(); }
    i &= STARTUP_RANDOM_SEED_BYTE_MASK;
    while (i--) { myrandom(); rand(); }
}

/* Load the startup palette and mouse cursor shapes, then copy them to the render surfaces.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void loadpalandmouse(void)
{
    U8 colors[PLATFORM_PALETTE_RGB_BYTES];
    U8 FAR *palptr;
    I16 count;
    I16 height;
    void FAR *shape_data;
    void FAR *filedata;
    I16 cursor_width;

    filedata = file_load_shape2d_fatal_thunk("sdmain");
    palptr = locate_shape_fatal(filedata, "!pal");
    palptr += 0x10;
    for (count = 0; count < 0x300; ++count)
        colors[count] = palptr[count];
    vgapalette(0, PLATFORM_PALETTE_COLOR_COUNT, colors);

    shape_data = locate_shape_fatal(filedata, "smou");
    cursor_width = ((I16S FAR *)shape_data)[0] * g_vid_flg2_set;
    height = ((I16S FAR *)shape_data)[1];
    freememory(filedata);
    spritepointermini = sprite_make_window(cursor_width, height, PLATFORM_VGA_COLOR_WHITE);
    mouse_ptr_cursor = sprite_make_window(cursor_width, height, PLATFORM_VGA_COLOR_WHITE);
    mouse_unk_sprite_ptr = sprite_make_window(cursor_width + g_vid_flg2_set, height, PLATFORM_VGA_COLOR_WHITE);

    filedata = file_load_shape2d_fatal_thunk("sdmain");
    sprite_setup1_from_arg_pointer(spritepointermini);
    movfxya(locate_shape_fatal(filedata, "smou"), 0, 0);
    sprite_setup1_from_arg_pointer(mouse_ptr_cursor);
    movfxya(locate_shape_fatal(filedata, "mmou"), 0, 0);
    freememory(filedata);
    usescreen();
}
I16 get_0(void) { return 0; }

/* Convert byte count to paragraph count and request pages from the game memory manager.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR *resmem(const I8 *name, I32 size)
{
    return reservememory(name, size / DOS_PARAGRAPH_BYTES_I16 + 1);
}
U32 availmem(void)
{
    return ((U32)unlockedmemory()) << 4;
}

/* Convert a memory-manager chunk size from paragraphs to bytes.
 * */
U32 FAR modsize(I8 FAR *ptr)
{
    return ((U32)modulesize(ptr)) << 4;
}
