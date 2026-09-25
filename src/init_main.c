struct POINT2D { int px, py; };
struct RECTANGLE { int left, right, top, bottom; };
extern short word_34AE4, video_flag2_is1, video_flag3_isFFFF, word_361D0;
extern unsigned char byte_36436, byte_359F1;
extern char textresprefix;
extern char audiodriverstring[];
extern const unsigned char g_ascii_props[256];
extern unsigned short word_34CEA, framespersec2, data_349D0, word_34984;
extern unsigned char detail_level;
extern int *word_30B10, *word_30B0E, *word_30B0C, *word_30B0A;
extern void far kb_init_interrupt(void);
extern void far kb_shift_checking2(void);
extern int far kb_call_readchar_callback(void);
extern void far kb_reg_callback(int, void (far *)(void));
extern void far show_graphic_levels_menu(void);
extern void far do_joy_restext(void);
extern void far do_key_restext(void);
extern void far do_mof_restext(void);
extern void far do_pau_restext(void);
extern void far do_dos_restext(void);
extern void far do_sonsof_restext(void);
extern short far do_dea_textres(void);
extern void far mmgr_alloc_a000(void);
extern void far video_set_mode_13h(void);
extern void far video_set_mode4(void);
extern void far timer_setup_interrupt(void);
extern void far sprite_copy_2_to_1_clear(void);
extern short far mouse_init(unsigned short, unsigned short);
extern short far audio_load_driver(char*, short, short);
extern void far audio_stop_unk(void);
extern void far libsub_quit_to_dos_alt(short);
extern short far audio_toggle_flag2(void);
extern short far audio_toggle_flag6(void);
extern short far set_criterr_handler(short (far *)(void));
void load_palandcursor(void);
void random_wait(void);
extern void far sprite_copy_2_to_1(void);
extern void far sprite_set_1_size(unsigned short, unsigned short, unsigned short, unsigned short);
extern int far timer_get_delta_alt(void);
extern void far ported_sprite_clear_1_color_(unsigned char);
extern void far sprite_clear_1_color(unsigned char);
extern void far rect_adjust_from_point(struct POINT2D*, struct RECTANGLE*);
extern void far copy_material_list_pointers(void*, void*, void*, void*, unsigned short);
extern unsigned int far strlen(char*);
extern int far video_get_status(void);
extern int far rand(void);
extern int far get_kevinrandom(void);
extern void far * far file_load_shape2d_fatal_thunk(char*);
extern void far * far locate_shape_fatal(void far*, char*);
extern void far video_set_palette(unsigned short, unsigned short, unsigned char*);
extern void far mmgr_free(void far*);
extern void far * far sprite_make_wnd(unsigned short, unsigned short, unsigned short);
extern void far sprite_set_1_from_argptr(void far*);
extern void far sprite_shape_to_1(void far*, unsigned short, unsigned short);
extern void far sprite_copy_2_to_1_2(void);
extern void far *smouspriteptr;
extern void far *mmouspriteptr;
extern void far *mouseunkspriteptr;


void init_main(int argc, char* argv[])
{
	register int i, j;
	unsigned char mode_4, nosound, unknown;
	int timer1, middle_delta, timerdelta3;
	struct POINT2D tmppoint;
	struct RECTANGLE limits;

	// Keyboard
	kb_init_interrupt();
	kb_shift_checking2();
	kb_call_readchar_callback();

	kb_reg_callback(0x0007, &show_graphic_levels_menu);
	kb_reg_callback(0x000A, &do_joy_restext);
	kb_reg_callback(0x000B, &do_key_restext);
	kb_reg_callback(0x3200, &do_mof_restext);
	kb_reg_callback(0x0010, &do_pau_restext);
	kb_reg_callback('p', &do_pau_restext);
	kb_reg_callback(0x0011, &do_dos_restext);
	kb_reg_callback(0x0013, &do_sonsof_restext);
	kb_reg_callback(0x0018, &do_dos_restext);
	
	// Video
	word_34AE4 = 1;
	video_flag2_is1 = 1;
	video_flag3_isFFFF = -1;
	word_361D0 = 1;

	mmgr_alloc_a000();
	
	byte_36436 = 0;
	byte_359F1 = 1;
	
	textresprefix = 'e';
	
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
                    if (((g_ascii_props[argv[i][2]] & 1) ? argv[i][2] + ' ' : argv[i][2]) == 's'
                     && ((g_ascii_props[argv[i][3]] & 1) ? argv[i][3] + ' ' : argv[i][3]) == 'b') {
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
	video_set_mode_13h();
	if (mode_4) {
		video_set_mode4();
	}

	timer_setup_interrupt();

	sprite_copy_2_to_1_clear();

	mouse_init(0x0140, 0x00C8);

	// Audio driver.
	if (audio_load_driver(audiodriverstring, 0, 0)) {
		audio_stop_unk();
		libsub_quit_to_dos_alt(1);
	}
	
	if (nosound) {
		audio_toggle_flag2();
		audio_toggle_flag6();
	}
	
	set_criterr_handler(&do_dea_textres);
	
	load_palandcursor();
	
	// Timing measures.
		sprite_copy_2_to_1();
	sprite_set_1_size(0, 320, 0, 120);

	timer_get_delta_alt();
	for (i = 0; i < 15; ++i) {
		ported_sprite_clear_1_color_(0); // the c impl is too slow/wrong and produces faulty timing values
	}
	timer1 = timer_get_delta_alt();
	
	sprite_set_1_size(0, 320, 0, 60);

	for (i = 0; i < 15; ++i) {
		limits.left = 0;
		limits.right = 0;
		limits.top = 0;
		limits.bottom = 0;
		
		for (j = 0; j < 400; ++j) {
			tmppoint.py = tmppoint.px = j;
			rect_adjust_from_point(&tmppoint, &limits);
		}
		
		sprite_clear_1_color(0);
	}
	
	middle_delta = timer_get_delta_alt();

	for (i = 0; i < 146; ++i) {
		for (j = 0; j < 255; ++j) {
			rect_adjust_from_point(&tmppoint, &limits);
		}
	}
	
	timerdelta3 = timer_get_delta_alt();
	
	if (middle_delta > timer1)
		word_34CEA = 0;
	else
		word_34CEA = 1;
	if (timerdelta3 < 75)
		framespersec2 = 20;
	else
		framespersec2 = 10;

	if (timerdelta3 < 35) {
		detail_level = 0;
	}
	else if (timerdelta3 < 55) {
		detail_level = 1;
	}
	else if (timerdelta3 < 75) {
		detail_level = 2;
	}
	else if (timerdelta3 < 100 || !word_34CEA) {
		detail_level = 3;
	}
	else {
		detail_level = 4;
	}

	data_349D0 = framespersec2;
	word_34984 = word_34CEA;
	
	random_wait();
	
	copy_material_list_pointers(word_30B0A, word_30B0C, word_30B0E, word_30B10, 0);
}

void random_wait(void)
{
    register int status1, i;
    status1 = video_get_status();
    i = 0;
    while (status1 == video_get_status() && i < 12000)
        ++i;
    if (i == 1024)
        i = *((signed char *)0x046c);
    while (i--) { rand(); get_kevinrandom(); }
    i &= 0xff;
    while (i--) { get_kevinrandom(); rand(); }
}

void load_palandcursor(void)
{
    unsigned char colors[0x300];
    unsigned char far *palptr;
    int count;
    int height;
    void far *shape_data;
    void far *filedata;
    int cursor_width;

    filedata = file_load_shape2d_fatal_thunk("sdmain");
    palptr = (unsigned char far *)locate_shape_fatal(filedata, "!pal");
    palptr += 0x10;
    for (count = 0; count < 0x300; ++count)
        colors[count] = palptr[count];
    video_set_palette(0, 0x100, colors);

    shape_data = locate_shape_fatal(filedata, "smou");
    cursor_width = ((short far *)shape_data)[0] * video_flag2_is1;
    height = ((short far *)shape_data)[1];
    mmgr_free(filedata);
    smouspriteptr = sprite_make_wnd(cursor_width, height, 0x0f);
    mmouspriteptr = sprite_make_wnd(cursor_width, height, 0x0f);
    mouseunkspriteptr = sprite_make_wnd(cursor_width + video_flag2_is1, height, 0x0f);

    filedata = file_load_shape2d_fatal_thunk("sdmain");
    sprite_set_1_from_argptr(smouspriteptr);
    sprite_shape_to_1(locate_shape_fatal(filedata, "smou"), 0, 0);
    sprite_set_1_from_argptr(mmouspriteptr);
    sprite_shape_to_1(locate_shape_fatal(filedata, "mmou"), 0, 0);
    mmgr_free(filedata);
    sprite_copy_2_to_1_2();
}
