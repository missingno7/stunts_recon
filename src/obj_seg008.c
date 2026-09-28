/* MSC 5.10 <ctype.h> macros over the pinned runtime table _ctype */
#define _UPPER 0x1
#define _LOWER 0x2
#define isupper(c) ((_ctype+1)[c] & _UPPER)
#define islower(c) ((_ctype+1)[c] & _LOWER)
#define _tolower(c) ((c)-'A'+'a')
#define tolower(c) (isupper(c) ? _tolower(c) : (c))
static int textboundsleft, text_bounds_right, text_bounds_upper, txt_bounds_bottom;
static int textoutlineleft, text_outline_right, txt_outline_top, text_outline_bottom;

extern char *itoa(int value, char *buffer, int radix);

extern unsigned strlen(char *s);
extern char *strcpy(char *destination, char *source);
extern char *strcat(char *destination, char *source);
extern void far* mmgr_free(char far* ptr);
extern char far *locate_shape_fatal(char far *data, char *name);
char textrespfxchr;
int msecoordx;
int pos_y_ms;
extern char kbormouse;
struct SHAPE2D { unsigned short words[6]; unsigned char bytes[4]; };
struct SPRITE { struct SHAPE2D far *sprite_bitmapptr; unsigned short words[3]; unsigned int *lineofs; unsigned short words2[9]; };
extern struct SPRITE far sprite2;
extern struct SPRITE far *g_wndspr;
struct SPRITE far *mouse_unk_sprite_ptr;
extern struct SPRITE far *mouse_ptr_cursor;
extern struct SPRITE far *spritepointermini;
void sprite_setup1_from_arg_pointer(struct SPRITE far *argsprite);
void sprite_clear_1_color(unsigned char color);
void far sprite_copy_2_to_1(void);
void far sprite_copy_both_to_arg(struct SPRITE *argsprite);
void far sprite_copy_arg_to_both(struct SPRITE *argsprite);
void far sprputimage(struct SHAPE2D far *shape);
void far sprite_clear_shape_alt(struct SHAPE2D far *shape, int x, int y);
void far sprite_putimage_and(struct SHAPE2D far *shape, unsigned short x, unsigned short y);
void far sprite_putimage_or(struct SHAPE2D far *shape, unsigned short x, unsigned short y);
void far sprite_1_unk3(struct SHAPE2D far *shape, int index);
struct SHAPE3DHEADER { unsigned char numverts, numprimitives, numpaints, reserved; };
struct SHAPE3D { unsigned short numverts; char far *verts; unsigned short numprimitives; unsigned char numpaints, reserved; char far *primitives; char far *cull1; char far *cull2; };
extern int far font_op2(char *name);
int g_animphase;
int g_hovercolor_idle;
extern unsigned int unused_count;
extern int rate_frame;
struct FONTDEF_PREFIX { unsigned char bytes[14]; unsigned short value; };
unsigned int fontdefvalue;
extern void far *def_fntadr;
extern void far set_fontdefseg(void far *data);
extern signed char mouse_transparent_mode;
signed char copy_mouse_modes[8];
signed char input_device_modestack[8];
extern unsigned long timer_get_delta(void);
extern unsigned long far timer_get_delta_alt(void);
extern unsigned long far timer_get_counter(void);
extern int far rand(void);
extern int far get_kevinrandom(void);
struct GAMESTATE_SNAPSHOT {
    long game_travDist;
    unsigned short game_frame;
    short game_total_finish;
    short field_144;
    short game_pEndFrame;
    short game_oEndFrame;
    unsigned short game_penalty;
    unsigned short game_impactSpeed;
    unsigned short game_topSpeed;
    short game_jumpCount;
};
extern struct GAMESTATE_SNAPSHOT race_stats;
extern int far input_checking(int delta);
extern int far input_do_checking(int delta);
extern int far input_repeat_check(int timeout);
int kbjoyflags;
int flagsdown;
extern int g_vid_flg2_set;
extern char mouse_isdirty;
extern char g_is_busy;
extern unsigned short dialogarg2;
void far *main_data_file_addr;
extern int far kb_get_char(void);
extern int far get_joy_flags(void);
extern int far get_kb_or_joy_flags(void);
extern void far mouse_get_state(int *buttons, int *x, int *y);
extern void far mouse_draw_opaque(void);
extern void far mouse_draw_transparent(void);
extern void far msdrawopaquechk(void);
extern void far msdrawtransparentchk(void);
extern void far input_pop_status(void);
extern void far check_input(void);
extern void far msdrawopaquechk(void);
extern void far msdrawtransparentchk(void);
extern void far input_pop_status(void);
extern void far file_build_path(char *dir, char *name, char *ext, char *dst);
extern void far *file_load_resource(int type, char *filename);
extern void far *file_load_resource_file(char *filename);
extern void far file_load_audio_resource(char *songfile, char *voicefile, char *name);
extern void far *file_load_3dres(char *filename);
extern short far do_dea_textres(void);
extern void far *file_load_binary_nofatal(char *filename);
extern void far *load_shape2d_nofatal_thunk(char *filename);
extern void far *load_shape2d_res_nofatal_thunk(char *filename);
extern void far *load_song_file(char *filename);
extern void far *load_voice_file(char *filename);
extern void far *load_sfx_file(char *filename);
extern void far *file_decomp_nofatal(char *filename);
extern void far *file_load_shape2d_nofatal2(char *filename);
extern void far *file_read_nofatal(unsigned int first, unsigned int second, unsigned int third);
extern void far *init_audio_resources(void far *song, void far *voice, char *name);
extern void far load_audio_finalize(void far *audiores);
extern void far audio_driver_func3F(int command);
short voicefile_gap_c[4];
void far *openvfile;
void far *musicfile;
extern char is_audioloaded;
extern char far * far locate_text_resource(char far *data, char *name);
extern void copy_string(char *destination, char far *source);
extern void parse_filepath_separators(char *dest, char *path);



char *findfiletexts[4] = { "id1", "id2", "id3", "id4" };
char *findfilenames[4] = { "setup.exe", "sdtitl.*", "tedit.*", "opp1.*" };
unsigned int font_secondary_color = 0;

/* target file_build_path @ 4370; candidate from build\workers\tuseg008\file_build_path_ch.c */
/* merged owner s008-a member point_in_rectangle */
char far point_in_rectangle(int x1, int x2, int y1, int y2)
{ /* PURPOSE: Save the pixels under a rectangular sprite region when the backing store has room. Params: x1, x2, y1, y2. Returns: char. Globals: reads g_mousesave_x_tbl, g_mouseyposstacktable, mssprite_arrays; writes g_mousesave_x_tbl, g_mouseyposstacktable, mssprite_arrays. */ /* PLATFORM(memory): allocates or releases game-managed memory. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    extern int pixel_scales;
    extern int vidflg4_is1;
    extern unsigned char mouse_buffer_count;
    extern int g_mousesave_x_tbl[];
    extern int g_mouseyposstacktable[];
    extern struct SPRITE far *mssprite_arrays[];
    extern struct SPRITE far *savedptr_ms;
    extern long far mmgr_get_res_ofs_diff_scaled(void);
    extern struct SPRITE far *far sprite_make_window(int width, int height, int flags);
    extern void far sprite_copy_both_to_arg(struct SPRITE *argsprite);
    extern void far sprite_copy_2_to_1(void);
    extern void far sprite_clear_shape_alt(struct SHAPE2D far *shape, int x, int y);
    struct SPRITE saved_sprites[2];
    long required;
    required = ((long)(x2 - x1) * (y2 - y1)) / (long)(pixel_scales * vidflg4_is1) + 18L;
    if (mmgr_get_res_ofs_diff_scaled() /* PLATFORM(memory): query available scaled resource memory. */ <= required) return 0;

    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    mssprite_arrays[mouse_buffer_count] = sprite_make_window(x2 - x1, y2 - y1, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    g_mousesave_x_tbl[mouse_buffer_count] = x1;
    g_mouseyposstacktable[mouse_buffer_count] = y1;
    sprite_copy_both_to_arg(&saved_sprites[0]) /* PLATFORM(video): save both active sprite buffers. */;
    savedptr_ms[mouse_buffer_count * 2] = saved_sprites[0];
    savedptr_ms[mouse_buffer_count * 2 + 1] = saved_sprites[1];
    sprite_copy_2_to_1() /* PLATFORM(video): copy sprite buffer 2 into buffer 1. */;
    sprite_clear_shape_alt(mssprite_arrays[mouse_buffer_count]->sprite_bitmapptr,
                           x1, y1) /* PLATFORM(video): clear the requested shape in sprite buffer 1. */;
    ++mouse_buffer_count;
    return 1;
}
int g_mouseyposstacktable[5];
int g_mousesave_x_tbl[4];
struct SPRITE far *mssprite_arrays[4];

/* merged owner s008-a member restore_mouse_sprite */
void far restore_mouse_sprite(void)
{ /* PURPOSE: Restore saved pixels and release the temporary sprite region. Params: none. Returns: void. Globals: reads g_mousesave_x_tbl, g_mouseyposstacktable, mssprite_arrays; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    extern unsigned char mouse_buffer_count;
    extern int g_mousesave_x_tbl[];
    extern int g_mouseyposstacktable[];
    extern struct SPRITE far *mssprite_arrays[];
    extern struct SPRITE far *savedptr_ms;
    extern void far sprite_shape_to_1(struct SHAPE2D far *shape, int x, int y);
    extern void far sprite_copy_arg_to_both(struct SPRITE *argsprite);
    extern void far sprite_free_window(void far *window);
    struct SPRITE saved_sprites[2];

    if (mouse_buffer_count == 0) return;
    --mouse_buffer_count;
    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    sprite_shape_to_1(mssprite_arrays[mouse_buffer_count]->sprite_bitmapptr,
                      g_mousesave_x_tbl[mouse_buffer_count],
                      g_mouseyposstacktable[mouse_buffer_count]) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
    saved_sprites[0] = savedptr_ms[mouse_buffer_count * 2];
    saved_sprites[1] = savedptr_ms[mouse_buffer_count * 2 + 1];
    sprite_copy_arg_to_both(&saved_sprites[0]) /* PLATFORM(video): restore both active sprite buffers. */;
    sprite_free_window(mssprite_arrays[mouse_buffer_count]) /* PLATFORM(video): release a temporary sprite window. */;
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
}

/* merged owner s008-dlg member show_dialog */
int far show_dialog(int type, int check, char far *message, int x, int y,
                    unsigned int frame_arg, int *disabled, char initial)
{ /* PURPOSE: Lay out and draw a dialog, then process input until a choice is made. Params: type, check, message, x, y, frame_arg, disabled, initial. Returns: int. Globals: reads font_secondary_color, fontdefvalue; writes font_secondary_color. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(input_mouse): polls or updates mouse state. */
    extern int dlg_colour; extern unsigned int font_secondary_color; extern int performGraphColor;
    extern unsigned char _ctype[];
    extern int far sprset1size(int, int, int, int);
    extern void far sprite_1_unk4(int, int, int, int, unsigned int);
    extern void far font_setup_unknown(unsigned int, unsigned int);
    extern void far draw_text_at(char *, int, int);
    extern int far wait_for_input_delay(long);
    extern int far mouse_multi_hittest(int, int *, int *, int *, int *);
    extern unsigned long far timer_get_delta_alt(void);
    char chr;
    int linehgt;
    char ret;
    int lowkey;
    char far *textptr;
    int hot_1;
    char n_char;
    int hot0;
    int total_h;
    char choice;
    int text_w;
    char oldchoice;
    int btn_ts[20];
    int i;
    int wide;
    char labelstr[80];
    unsigned int key;
    char count;
    char far *btn_text[20];
    int btn_bs[20];
    int btn_rs[20];
    char markers;
    char far *line_start;
    char lengths[20];
    char busy;
    int pos;
    char textbuf[80];
    int dlgframe[4];
    int btn_ls[20];

    linehgt = fontdefvalue + 2;
    total_h = 0;
    wide = 32;
    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    textptr = message;
    pos = 0;
    while ((chr = *textptr) != 0) {
        if (chr == ']') {
            textbuf[pos] = 0;
            text_w = font_op2(textbuf) /* PLATFORM(video): load or select a named font. */;
            if (text_w > wide)
                wide = text_w;
            pos = 0;
            total_h += linehgt;
        } else if (*textptr == '}') {
            textbuf[pos] = 0;
            text_w = font_op2(textbuf) /* PLATFORM(video): load or select a named font. */;
            if (text_w > wide)
                wide = text_w;
            pos = 0;
            total_h += 4;
        } else {
            textbuf[pos++] = *textptr;
        }
        ++textptr;
    }
    wide = (wide + 24) & 0xfff8;
    if (x == -1)
        x = ((320 - wide) / 2) & 0xfff8;
    if (y == -1)
        y = (200 - total_h) / 2;
    dlgframe[0] = x;
    dlgframe[1] = x + wide;
    dlgframe[2] = y - 8;
    dlgframe[3] = y + total_h + 8;
    x += 8;
    wide -= 16;
    if (check != 0 && !point_in_rectangle(dlgframe[0], dlgframe[1], dlgframe[2], dlgframe[3]))
        return -1;
    sprite_copy_2_to_1() /* PLATFORM(video): copy sprite buffer 2 into buffer 1. */;
    sprset1size(dlgframe[0], dlgframe[1], dlgframe[2], dlgframe[3]) /* PLATFORM(video): set the active sprite buffer bounds. */;
    sprite_clear_1_color(0) /* PLATFORM(video): clear the active sprite buffer with the requested color. */;
    sprite_1_unk4(x - 4, y - 4, x + wide + 4, y + total_h + 4, frame_arg) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
    font_setup_unknown(dlg_colour, 0) /* PLATFORM(video): select the font and color mode. */;
    font_secondary_color = 0;
    font_setup_unknown(dlg_colour, 0) /* PLATFORM(video): select the font and color mode. */;
    pos = 0;
    markers = 0;
    textptr = message;
    total_h = 1;
    while ((chr = *textptr) != 0) {
        if (chr == '[')
            goto buttons;
        if (chr == ']') {
            textbuf[pos] = 0;
            draw_text_at(textbuf, x, y + total_h) /* PLATFORM(video): draw a text string at screen coordinates. */;
            pos = 0;
            total_h += linehgt;
            line_start = textptr;
        } else if (*textptr == '}') {
            textbuf[pos] = 0;
            draw_text_at(textbuf, x, y + total_h) /* PLATFORM(video): draw a text string at screen coordinates. */;
            pos = 0;
            total_h += 4;
            line_start = textptr;
        } else if (*textptr == '@') {
            if (type == 3) {
                textbuf[pos] = 0;
                disabled[markers] = font_op2(textbuf) /* PLATFORM(video): load or select a named font. */ + x;
                disabled[markers + 1] = y + total_h;
                markers += 2;
            }
            textbuf[pos++] = ' ';
        } else {
            textbuf[pos++] = *textptr;
        }
        ++textptr;
    }
buttons:
    count = 0;
    while (*textptr == '[') {
        ++textptr;
        btn_text[count] = textptr;
        textbuf[pos] = 0;
        btn_ls[count] = font_op2(textbuf) /* PLATFORM(video): load or select a named font. */ + x;
        btn_ts[count] = y + total_h;
        btn_bs[count] = y + total_h + linehgt;
        n_char = 0;
        textbuf[pos++] = ' ';
        text_w = 0;
        while ((chr = *textptr) != 0) {
            if (chr == '[')
                goto end_button;
            if (chr == ']') {
                textbuf[pos] = 0;
                text_w = font_op2(textbuf) /* PLATFORM(video): load or select a named font. */;
                pos = 0;
                total_h += linehgt;
            } else if (*textptr == '}') {
                textbuf[pos] = 0;
                text_w = font_op2(textbuf) /* PLATFORM(video): load or select a named font. */;
                pos = 0;
                total_h += 3;
            } else {
                textbuf[pos++] = *textptr;
                ++n_char;
            }
            ++textptr;
        }
end_button:
        lengths[count] = n_char;
        textbuf[pos] = 0;
        if (text_w == 0)
            text_w = font_op2(textbuf) /* PLATFORM(video): load or select a named font. */;
        btn_rs[count] = btn_ls[count] + text_w;
        ++count;
    }
    if (count > 2 && btn_ls[0] == btn_ls[1] && btn_ls[1] == btn_ls[2]) {
        for (i = 0; i < count; ++i)
            btn_rs[i] = btn_ls[i] + wide;
    }
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    ret = 1;
    switch (type) {
    case 4:
        wait_for_input_delay(8L) /* PLATFORM(timer): wait for the requested delay while servicing input. */;
        break;
    case 1:
        do {
            key = input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */;
        } while (key == 0);
        if (key == 27)
            ret = 0;
        check_input();
        break;
    case 0:
        return 0;
    case 3:
        return (char)(markers / 2);
    case 2:
        ret = initial;
        oldchoice = -1;
        timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */;
        msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
        if (count == 2) {
            i = 0;
            do {
                hot0 = (unsigned char)btn_text[0][i];
                ++i;
            } while (hot0 == ' ');
            if (isupper(hot0))
                hot0 = tolower(hot0);
            i = 0;
            do {
                hot_1 = (unsigned char)btn_text[1][i];
                ++i;
            } while (hot_1 == ' ');
            if (isupper(hot_1))
                hot_1 = tolower(hot_1);
        }
        busy = 1;
        while (busy) {
            if (ret != oldchoice) {
                msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
                for (i = 0; i < count; ++i) {
                    if (disabled != 0 && disabled[i] != 0)
                        font_setup_unknown(performGraphColor, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
                    else if (ret == i)
                        font_setup_unknown(font_secondary_color, dlg_colour) /* PLATFORM(video): select the font and color mode. */;
                    else
                        font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
                    textptr = btn_text[i];
                    for (pos = 0; pos < lengths[i]; ++pos)
                        labelstr[pos] = textptr[pos];
                    labelstr[pos] = 0;
                    draw_text_at(labelstr, btn_ls[i], btn_ts[i]) /* PLATFORM(video): draw a text string at screen coordinates. */;
                }
                msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
                if (oldchoice == -1)
                    check_input();
                oldchoice = ret;
            }
            key = input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */;
            choice = mouse_multi_hittest(count, btn_ls, btn_rs, btn_ts, btn_bs) /* PLATFORM(input_mouse): test the mouse against candidate rectangles. */;
            if (choice != -1) {
                if (disabled == 0)
                    ret = choice;
                else if (disabled[choice] == 0)
                    ret = choice;
            }
            if (count == 2 && key != 0) {
                lowkey = key;
                if (isupper(lowkey))
                    lowkey = tolower(lowkey);
                if (hot0 == lowkey) {
                    ret = 0;
                    key = 13;
                } else if (hot_1 == lowkey) {
                    ret = 1;
                    key = 13;
                }
            }
            switch (key) {
            case 0x4800:
            case 0x4b00:
                do {
                    if (ret != 0)
                        --ret;
                    else
                        ret = count - 1;
                } while (disabled != 0 && disabled[ret] != 0);
                break;
            case 0x4d00:
            case 0x5000:
                do {
                    if (ret + 1 < count)
                        ++ret;
                    else
                        ret = 0;
                } while (disabled != 0 && disabled[ret] != 0);
                break;
            case 27:
                ret = -1;
            case 13:
            case ' ':
                busy = 0;
                check_input();
                break;
            }
        }
        break;
    }
    if (check != 0)
        restore_mouse_sprite();
    return ret;
}

/* merged owner s008-dlg member do_fileselect_dialog */
int far do_fileselect_dialog(char *path, char *selected_name, int attributes,
                             char far *heading)
{ /* PURPOSE: Build and display the file list, returning the selected file name. Params: path, selected_name, attributes, heading. Returns: int. Globals: reads dialogarg2, flagsdown, font_secondary_color, g_is_busy, main_data_file_addr; writes g_is_busy. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(input_mouse): polls or updates mouse state. */
    extern char resbuftext[];
    extern int dlg_colour; extern unsigned int font_secondary_color; extern int performGraphColor;
    extern unsigned char _ctype[];
    extern char *file_combine_and_find(char *, char *, int);
    extern char *file_find_next_alt(void);
    extern int far call_read_line(char *, int, int, int, long);
    extern void far preRender_line(int, int, int, int, int);
    extern int far sprite_1_unk(int, int, int, int, int);
    int hit_l[10];
    char answer;
    char old_busy;
    char names[128][13];
    char rc;
    register unsigned idx;
    char first_char;
    char first_visible;
    int pressed;
    unsigned other_idx;
    int hit_b[10];
    int hit_r[10];
    char old_cur;
    int hit_t[10];
    register int x;
    int field_end;
    char button;
    char *found;
    char cursor;
    char files_found;
    int layout[20];
    int label_width;
    char prev_top;

    if (show_dialog(3, 1, locate_text_resource(main_data_file_addr, "loa") /* PLATFORM(file): locate a named text entry in resource data. */,
                    -1, -1, dialogarg2, layout, 0) /* PLATFORM(video): present the interactive dialog renderer. */ < 0)
        return 0;

    old_busy = g_is_busy;
    g_is_busy = 1;
    preRender_line(layout[4] - 4, layout[5] + 4, layout[4] + 0xab,
                   layout[5] + 4, dialogarg2) /* PLATFORM(video): draw a line primitive. */;
    font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
    copy_string(resbuftext, heading);
    draw_text_at(resbuftext, layout[0], layout[1]) /* PLATFORM(video): draw a text string at screen coordinates. */;
    x = layout[2];
    field_end = x + 0xa2;
    for (button = 0; button < 10; ++button) {
        hit_l[button] = x;
        hit_r[button] = field_end;
        if (button == 9)
            hit_t[button] = hit_t[button - 1] + 10;
        else
            hit_t[button] = layout[button * 2 + 3];
        hit_b[button] = hit_t[button] + 10;
    }
    font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
    draw_text_at(path, x, layout[3]) /* PLATFORM(video): draw a text string at screen coordinates. */;

rescan:
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    files_found = 0;
    found = file_combine_and_find(path, "*", attributes) /* PLATFORM(file): search for the combined directory/file path. */;
    if (found == 0) {
        nullsub_1();
edit_path:
        font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
        if (call_read_line(path, 0x12, x, layout[3], 30000L) != 0x1b)
            goto rescan;
cancel:
        rc = 0;
        goto finish;
    }
    parse_filepath_separators(names[0], found);
    ++files_found;
    while ((found = file_find_next_alt() /* PLATFORM(file): advance to the next file-list entry. */) != 0) {
        parse_filepath_separators(names[files_found], found);
        ++files_found;
        if (files_found == 0x80)
            break;
    }
    nullsub_1();
    if (files_found > 1) {
        for (idx = 0; idx < files_found - 1; ++idx) {
            for (other_idx = idx + 1; other_idx < files_found; ++other_idx) {
                if (strcmp(names[idx], names[other_idx]) > 0) {
                    strcpy(resbuftext, names[idx]);
                    strcpy(names[idx], names[other_idx]);
                    strcpy(names[other_idx], resbuftext);
                }
            }
        }
    }
    if (files_found > 7) {
        copy_string(resbuftext, locate_text_resource(main_data_file_addr, "lsu") /* PLATFORM(file): locate a named text entry in resource data. */);
        draw_text_at(resbuftext, font_op2_alt(resbuftext) /* PLATFORM(video): load or select a named font through the alternate entry point. */, hit_t[1]) /* PLATFORM(video): draw a text string at screen coordinates. */;
        copy_string(resbuftext, locate_text_resource(main_data_file_addr, "lsd") /* PLATFORM(file): locate a named text entry in resource data. */);
        draw_text_at(resbuftext, font_op2_alt(resbuftext) /* PLATFORM(video): load or select a named font through the alternate entry point. */, hit_t[9] - 1) /* PLATFORM(video): draw a text string at screen coordinates. */;
    }
    cursor = 0;
    first_visible = 0;
    old_cur = -1;
    prev_top = -1;
    timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */;
    answer = 0;
    do {
        if (cursor != old_cur || first_visible != prev_top) {
            old_cur = cursor;
            prev_top = first_visible;
            msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
            for (idx = 0; idx < 7; ++idx) {
                if (first_visible + idx == cursor)
                    font_setup_unknown(font_secondary_color, dlg_colour) /* PLATFORM(video): select the font and color mode. */;
                else
                    font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
                if (first_visible + idx < files_found) {
                    strcpy(resbuftext, names[first_visible + idx]);
                    draw_text_at(resbuftext, x, hit_t[idx + 2]) /* PLATFORM(video): draw a text string at screen coordinates. */;
                } else
                    draw_text_at("        ", x, hit_t[idx + 2]) /* PLATFORM(video): draw a text string at screen coordinates. */;
                label_width = font_op2(resbuftext) /* PLATFORM(video): load or select a named font. */;
                sprite_1_unk(label_width + x, hit_t[idx + 2], field_end - label_width - x, 8, font_secondary_color) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
            }
            msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
        }
        pressed = input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */;
        button = mouse_multi_hittest(10, hit_l, hit_r, hit_t, hit_b) /* PLATFORM(input_mouse): test the mouse against candidate rectangles. */;
        if (button != -1) {
            if (button == 0) {
                if (flagsdown & 3) {
                    cursor = 0;
                    first_visible = -1;
                    pressed = 0;
                }
            } else if (button == 1) {
                if (flagsdown & 3) {
                    if (cursor + first_visible != 0)
                        --cursor;
                    if (cursor < first_visible)
                        first_visible = cursor;
                    pressed = 0;
                }
            } else if (button == 9) {
                if (flagsdown & 3) {
                    if (files_found - 1 != cursor)
                        ++cursor;
                    pressed = 0;
                }
            } else if (first_visible + button - 2 < files_found)
                cursor = first_visible + button - 2;
        }
        switch (pressed) {
        case 0x4800:
            --cursor;
            break;
        case 0x5000:
            if (files_found - 1 != cursor)
                ++cursor;
            break;
        case 0x0d:
        case 0x20:
            answer = 1;
            break;
        case 0x1b:
            answer = -1;
            break;
        default:
            if (isupper(pressed) || islower(pressed)) {
                first_char = tolower(pressed);
                for (button = 0; button < files_found; ++button) {
                    if ((char)tolower(names[button][0]) == first_char) {
                        cursor = button;
                        break;
                    }
                }
            }
            break;
        }
        if (cursor < first_visible)
            first_visible = cursor;
        if (first_visible < 0)
            goto edit_path;
        while (first_visible + 6 < cursor)
            ++first_visible;
    } while (answer == 0);
    if (answer == -1)
        goto cancel;
    strcpy(selected_name, names[cursor]);
    rc = 1;
finish:
    restore_mouse_sprite();
    g_is_busy = old_busy;
    return rc;
}

void file_build_path(char *dir, char *name, char *ext, char *dst)
{ /* PURPOSE: Join a directory, file name, and extension in the destination buffer. Params: dir, name, ext, dst. Returns: void. Globals: none. */
    register int dirlen;
    char last_character;
    if (dir) {
        strcpy(dst, dir);
        dirlen = strlen(dir);
    } else {
        dst[0] = 0;
        dirlen = 0;
    }
    if (dirlen) {
        last_character = dir[dirlen - 1];
        if (last_character != ':' && last_character != '\\')
            strcat(dst, "\\");
    }
    strcat(dst, name);
    strcat(dst, ext);
}

/* target parse_filepath_separators @ 4786; accepted source src/parse_filepath_separators.c */
/* merged owner s008-dlg member do_savefile_dialog */
int far do_savefile_dialog(char *name, char *directory, char far *title)
{ /* PURPOSE: Display the save dialog and return the selected file name. Params: name, directory, title. Returns: int. Globals: reads dialogarg2, font_secondary_color, main_data_file_addr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    extern char resbuftext;
    extern char far * far locate_text_resource(char far *, char *);
    extern void copy_string(char *, char far *);
    extern unsigned int dlg_colour, font_secondary_color;
    extern int far call_read_line(char *, int, int, int, long);
    register int key;
    char accepted;
    register int i;
    int layout[6];

    if (show_dialog(3, 1, locate_text_resource(main_data_file_addr, "sav") /* PLATFORM(file): locate a named text entry in resource data. */,
                    -1, -1, dialogarg2, layout, 0) /* PLATFORM(video): present the interactive dialog renderer. */ < 0)
        return 0;

    accepted = 0;
    font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
    copy_string(&resbuftext, title);
    draw_text_at(&resbuftext, layout[0], layout[1]) /* PLATFORM(video): draw a text string at screen coordinates. */;
    font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
    draw_text_at(name, layout[2], layout[3]) /* PLATFORM(video): draw a text string at screen coordinates. */;
    draw_text_at(directory, layout[4], layout[5]) /* PLATFORM(video): draw a text string at screen coordinates. */;
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    goto read_directory;
    while (key != 13) {
        if (call_read_line(name, 18, layout[2], layout[3], 30000L) == 27)
            goto finish_dialog;
read_directory:
        key = call_read_line(directory, 8, layout[4], layout[5], 30000L);
        for (i = 0; directory[i] != 0; ++i)
            if (directory[i] == ' ')
                directory[i] = '_';
        if (key == 27)
            goto finish_dialog;
    }
    accepted = 1;
finish_dialog:
    restore_mouse_sprite();
    return accepted;
}

void parse_filepath_separators(char *dest, char *path)
{ /* PURPOSE: Normalize path separators and report whether the path was accepted. Params: dest, path. Returns: void. Globals: none. */
    char ch;
    int len;

    len = strlen(path);
    do {
        ch = path[len - 1];
        if (ch == '\\' || ch == ':') break;
        --len;
    } while (len != 0);
    {
        int out;
        out = 0;
        for (;;) {
            dest[out] = path[len++];
            if (dest[out++] == '.') break;
        }
        --out;
        dest[out] = 0;
    }
}

/* target input_checking @ 4884; candidate from build\workers\tuseg008\input_checking.c */
int input_framecount2 = 0;
int input_framecount3 = 0;
int joyflags = 0;
int newjoyflags = 0;
int mouse_oldx = 0;
int mouse_oldy = 0;
int mouse_oldbut = 0;
int input_framecounter = 0;
int joyinputcode = 0;
int mousebutinputcode = 0;
int input_framecount = 0;

int input_checking(int delta)
{ /* PURPOSE: Poll the active input devices and update per-frame input state. Params: delta. Returns: int. Globals: reads current keyboard, mouse and joystick state; writes latched device flags, mouse coordinates and per-frame input counters. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(input_joy): polls or updates joystick state. */ /* PLATFORM(input_mouse): polls or updates mouse state. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    register int key;
    register int joy;

    input_framecount += delta;
    if (input_framecount > 20000) {
        input_framecount -= 10000;
        input_framecount2 -= 10000;
        input_framecount3 -= 10000;
    }

    key = kb_get_char() /* PLATFORM(input_kb): read the next keyboard character. */;
    if (key != 0) kbormouse = 0;
    joy = get_joy_flags() /* PLATFORM(input_joy): read joystick button/direction state. */;
    kbjoyflags = get_kb_or_joy_flags() /* PLATFORM(input_kb): read the configured keyboard-or-joystick state. */;

    if (joyflags != joy) {
        newjoyflags = (joyflags ^ joy) & joy;
        joyflags = joy;

    joy_decode:
        if (newjoyflags & 0x20) joyinputcode = 0x0d;
        else if (newjoyflags & 0x10) joyinputcode = 0x20;
        else if (newjoyflags & 0x01) joyinputcode = 0x4800;
        else if (newjoyflags & 0x02) joyinputcode = 0x5000;
        else if (newjoyflags & 0x08) joyinputcode = 0x4b00;
        else if (newjoyflags & 0x04) joyinputcode = 0x4d00;
        if (joyinputcode != 0) {
            input_framecount3 = input_framecount;
            kbormouse = 0;
        }
        goto mouse_poll;
    } else {
        if (joy != 0 && input_framecount3 + 20 < input_framecount)
            goto joy_decode;
    }

mouse_poll:
    mouse_get_state(&flagsdown, &msecoordx, &pos_y_ms) /* PLATFORM(input_mouse): read mouse buttons and coordinates. */;
    if (mouse_oldx != msecoordx || mouse_oldy != pos_y_ms ||
        mouse_oldbut != flagsdown) {
        mouse_oldx = msecoordx;
        mouse_oldy = pos_y_ms;
        kbormouse = 1;
        input_framecounter = 0;
        if (mouse_transparent_mode != 0) {
            if (mouse_isdirty != 0) mouse_draw_opaque() /* PLATFORM(video): draw the pointer using its opaque sprite path. */;
            mouse_draw_transparent() /* PLATFORM(video): draw the pointer using its transparent sprite path. */;
        }
    } else if (kbormouse != 0) {
        input_framecounter += delta;
        if (input_framecounter > 500) {
            input_framecounter = 0;
            kbormouse = 0;
            if (mouse_isdirty != 0) mouse_draw_opaque() /* PLATFORM(video): draw the pointer using its opaque sprite path. */;
        }
    }

    if (kbormouse != 0) {
        if (flagsdown != mouse_oldbut) {
            mouse_oldbut = flagsdown;
mouse_button_code:
            if (flagsdown & 1) mousebutinputcode = 0x20;
            else if (flagsdown & 2) mousebutinputcode = 0x0d;
            if (mousebutinputcode != 0) input_framecount2 = input_framecount;
            input_framecounter = 0;
        } else if (flagsdown != 0 && input_framecount2 + 20 < input_framecount) {
            goto mouse_button_code;
        }
        if (flagsdown != 0) {
            if (flagsdown & 1) kbjoyflags |= 0x20;
            else if (flagsdown & 2) kbjoyflags |= 0x10;
        }
    }

repeat_joy:
    if (key == 0) {
        if (joyinputcode != 0) {
            key = joyinputcode;
            joyinputcode = 0;
        } else if (mousebutinputcode != 0) {
            key = mousebutinputcode;
            mousebutinputcode = 0;
        }
    }
    return key;
}

/* target input_do_checking @ 5426; candidate from build\workers\inputtu\repeat_types_ulong.c */
int far input_do_checking(int value) { /* PURPOSE: Forward the per-frame input poll through the far entry point. Params: value. Returns: int. Globals: none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ return input_checking(value) /* PLATFORM(input_kb): poll the configured input devices. */; }

/* target file_load_resfile @ 5442; candidate from build\workers\tuseg008\file_load_resfile_scope.c */
void far *file_load_resource_file(char *filename)
{ /* PURPOSE: Load a resource file and release it when the requested text resource is absent. Params: filename. Returns: void *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
    void far *result;
    while (1) {
        char name[0x50];
        strcpy(name, filename);
        strcat(name, ".res");
        result = file_load_resource(1, name) /* PLATFORM(file): load the requested resource type. */;
        if (result == 0) {
            strcpy(name, filename);
            strcat(name, ".pre");
            result = file_load_resource(7, name) /* PLATFORM(file): load the requested resource type. */;
            if (result == 0) {
                do_dea_textres();
                continue;
            }
        }
        return result;
    }
}

/* target unload_resource @ 5576; accepted source src/unload_resource.c */
void far unload_resource(void far* resptr) { /* PURPOSE: Release a loaded resource through the game memory manager. Params: resptr. Returns: void. Globals: none. */ /* PLATFORM(memory): allocates or releases game-managed memory. */
    mmgr_free(resptr) /* PLATFORM(memory): release the resource through game memory management. */;
}

/* target locate_shape_alt @ 5596; accepted source src/locate_shape_alt.c */
char far *locate_shape_alt(char far *data, char *name)
{ /* PURPOSE: Find a named shape in a resource and return its far pointer. Params: data, name. Returns: char *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
    return locate_shape_fatal(data, name) /* PLATFORM(file): locate a named shape in resource data. */;
}

/* target locate_text_res @ 5618; accepted source src/locate_text_res.c */
char far * far locate_text_resource(char far *data, char *name)
{ /* PURPOSE: Find a named text entry and record its leading character. Params: data, name. Returns: char *. Globals: reads textrespfxchr; writes none. */ /* PLATFORM(file): uses file and resource services. */
    char textname[4];
    textname[0] = textrespfxchr;
    textname[1] = name[0];
    textname[2] = name[1];
    textname[3] = name[2];
    return locate_shape_fatal(data, textname) /* PLATFORM(file): locate a named shape in resource data. */;
}

/* target copy_string @ 5670; accepted source src/copy_string.c */
void copy_string(char *destination, char far *source)
{ /* PURPOSE: Copy a near or far string into the destination buffer. Params: destination, source. Returns: void. Globals: none. */
    char far *current = source;

    do {
        *destination = *current;
        ++destination;
        ++current;
    } while (*current != '\0');

    *destination = '\0';
}

/* target mouse_draw_transparent_check @ 6382; candidate from build\workers\periph\mouse_draw_transparent_check.c */
/* merged owner s008-dlg member mouse_track_op */
int far mouse_track_op(int op, int x, int width, int top, int height,
                       int value, int offset, int divisions)
{ /* PURPOSE: Track mouse movement and process the current pointer action. Params: op, x, width, top, height, value, offset, divisions. Returns: int. Globals: reads flagsdown, msecoordx, pos_y_ms; writes flagsdown. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    extern int far sprite_1_unk(int, int, int, int, int);
    extern int dlg_colour;
    register int low;
    register int end_position;
    int swapped;
    int max_value;
    int pointer;
    int past_thumb;
    int location;
    int thumb;
    int range_length;

    if (width > height) {
        swapped = 0;
        max_value = width;
    } else {
        swapped = 1;
        max_value = height;
    }
    {
        int end_of_track;
        int factor;
        factor = divisions << 2;
        end_of_track = max_value - 1;
        low = (end_of_track * value << 2) / factor;
        end_position = ((value + offset) * end_of_track << 2) / factor;
        range_length = end_position - low;
    }

    switch (op) {
    case 0:
        sprite_1_unk(x, top, width, height, 0) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        if (swapped == 0)
            sprite_1_unk(x + low, top, range_length, height, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        else
            sprite_1_unk(x, top + low, width, range_length, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        return value;
    case 1:
        break;
    default:
        return value;
    }

    if (swapped == 0)
        pointer = msecoordx - x;
    else
        pointer = pos_y_ms - top;

    if (pointer < low || pointer > end_position) {        do {
            input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */;
        } while ((*(unsigned char *)&flagsdown & 3) != 0);
        if (pointer < low) {
            if (value != 0) --value;
        } else if (value < divisions - 1) {
            ++value;
        }
    } else {
        value = -1;
        past_thumb = low;
        do {
            input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */;
            if (swapped == 0)
                location = msecoordx - x;
            else
                location = pos_y_ms - top;
            thumb = location - pointer + low;
            if (thumb < 0) {
                thumb = 0;
            } else if (thumb + range_length > max_value - 1) {
                thumb = max_value - range_length - 1;
            }
            if (thumb != past_thumb) {
                past_thumb = thumb;
                msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
                sprite_1_unk(x, top, width, height, 0) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
                if (swapped == 0)
                    sprite_1_unk(x + thumb, top, range_length, height, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
                else
                    sprite_1_unk(x, top + thumb, width, range_length, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
                msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
            }
        } while ((*(unsigned char *)&flagsdown & 3) != 0);

    }

    if (value == -1)
        value = ((max_value / divisions) / 2 + thumb) * divisions / max_value;

    {
        int end_of_track;
        int factor;
        factor = divisions << 2;
        end_of_track = max_value - 1;
        low = (end_of_track * value << 2) / factor;
        end_position = ((value + offset) * end_of_track << 2) / factor;
        range_length = end_position - low;
    }
    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    sprite_1_unk(x, top, width, height, 0) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
    if (swapped == 0)
        sprite_1_unk(x + low, top, range_length, height, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
    else
        sprite_1_unk(x, top + low, width, range_length, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    return value;
}

void far msdrawtransparentchk(void)
{ /* PURPOSE: Redraw the mouse pointer in transparent mode when it is dirty. Params: none. Returns: void. Globals: reads kbormouse, mouse_isdirty; writes mouse_isdirty, mouse_transparent_mode. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    mouse_transparent_mode = 1;
    if (kbormouse != 0 && mouse_isdirty == 0)
        mouse_draw_transparent() /* PLATFORM(video): draw the pointer using its transparent sprite path. */;
}

/* target mouse_draw_opaque_check @ 6406; candidate from build\workers\periph\mouse_draw_opaque_check.c */
void far msdrawopaquechk(void)
{ /* PURPOSE: Redraw the mouse pointer in opaque mode when it is dirty. Params: none. Returns: void. Globals: reads mouse_isdirty; writes mouse_transparent_mode. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    mouse_transparent_mode = 0;
    if (mouse_isdirty != 0)
        mouse_draw_opaque() /* PLATFORM(video): draw the pointer using its opaque sprite path. */;
}

/* target mouse_draw_opaque @ 6424; candidate from build\workers\tuseg008\mouse_draw_opaque.c */
void far mouse_draw_opaque(void)
{ /* PURPOSE: Save the sprite buffers, draw the pointer, and restore the buffers. Params: none. Returns: void. Globals: reads mouse_unk_sprite_ptr; writes mouse_isdirty. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    struct SPRITE saved_sprite[2];
    sprite_copy_both_to_arg(saved_sprite) /* PLATFORM(video): save both active sprite buffers. */;
    sprite_copy_2_to_1() /* PLATFORM(video): copy sprite buffer 2 into buffer 1. */;
    sprputimage(mouse_unk_sprite_ptr->sprite_bitmapptr) /* PLATFORM(video): draw the selected shape. */;
    sprite_copy_arg_to_both(saved_sprite) /* PLATFORM(video): restore both active sprite buffers. */;
    mouse_isdirty = 0;
}

/* target mouse_draw_transparent @ 6484; candidate from build\workers\tuseg008\mouse_draw_transparent.c */
void far mouse_draw_transparent(void)
{ /* PURPOSE: Draw the pointer sprite transparently into the active sprite buffer. Params: none. Returns: void. Globals: reads g_vid_flg2_set, mouse_ptr_cursor, mouse_unk_sprite_ptr, msecoordx, pos_y_ms, spritepointermini; writes mouse_isdirty. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    struct SPRITE saved_sprite[2];
    register int xpos;
    xpos = msecoordx;
    xpos -= xpos % g_vid_flg2_set;
    sprite_copy_both_to_arg(saved_sprite) /* PLATFORM(video): save both active sprite buffers. */;
    sprite_copy_2_to_1() /* PLATFORM(video): copy sprite buffer 2 into buffer 1. */;
    sprite_clear_shape_alt(mouse_unk_sprite_ptr->sprite_bitmapptr, xpos, pos_y_ms) /* PLATFORM(video): clear the requested shape in sprite buffer 1. */;
    sprite_putimage_and(mouse_ptr_cursor->sprite_bitmapptr, msecoordx, pos_y_ms) /* PLATFORM(video): apply a sprite mask at the requested position. */;
    sprite_putimage_or(spritepointermini->sprite_bitmapptr, msecoordx, pos_y_ms) /* PLATFORM(video): apply sprite pixels at the requested position. */;
    sprite_copy_arg_to_both(saved_sprite) /* PLATFORM(video): restore both active sprite buffers. */;
    mouse_isdirty = 1;
}

/* target mouse_multi_hittest @ 6624; accepted source src/mouse_multi_hittest.c */
int far mouse_multi_hittest(int count, int *left, int *right, int *top, int *bottom)
{ /* PURPOSE: Return the first candidate rectangle containing the current mouse position. Params: count, left, right, top, bottom. Returns: int. Globals: reads kbormouse, msecoordx, pos_y_ms; writes none. */
    register int index;
    if (kbormouse != 0) {
        for (index = 0; index < count; ++index) {
            if (left[index] <= msecoordx &&
                right[index] >= msecoordx &&
                top[index] <= pos_y_ms &&
                bottom[index] >= pos_y_ms)
                return (signed char)index;
        }
    }
    return -1;
}

/* target check_input @ 6708; candidate from build\workers\tuseg008\check_input.c */
void far check_input(void)
{ /* PURPOSE: Poll keyboard or mouse state and update the input frame counter. Params: none. Returns: void. Globals: reads flagsdown, kbormouse; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    char done;
    do {
        if (get_kb_or_joy_flags() /* PLATFORM(input_kb): read the configured keyboard-or-joystick state. */ & 0x30) {
            done = 1;
        } else if (input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */ != 0) {
            done = 1;
        } else if (kbormouse != 0 && (flagsdown & 3) != 0) {
            done = 1;
        } else {
            done = 0;
        }
    } while (done != 0);
}

/* target nopsub_28F26 @ 6774; candidate from build\workers\tuseg008\nopsub_28F26.c */
void far nopsub_28F26(void)
{ /* PURPOSE: Run the waiting-input update and copy the active sprite buffer. Params: none. Returns: void. Globals: none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    while (input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */ == 0) { }
    check_input();
}

/* target sprite_copy_2_to_1_2 @ 6796; accepted source src/sprite_copy_2_to_1_2.c */
void sprite_clear_1_color(unsigned char color);void sprcopy2to12(void) { /* PURPOSE: Set sprite buffer 1 from the current sprite argument. Params: none. Returns: void. Globals: reads sprite2; writes sprite2. */ /* PLATFORM(video): draws pixels, sprites, or text. */ sprite_setup1_from_arg_pointer(&sprite2) /* PLATFORM(video): select the sprite described by the argument pointer. */; }

/* target sprite_copy_2_to_1_clear @ 6814; accepted source src/sprite_copy_2_to_1_clear.c */
void sprite_clear_1_color(unsigned char color);void sprite_copy_2_to_1_clear(void) { /* PURPOSE: Set sprite buffer 1 from the current sprite and clear its color. Params: none. Returns: void. Globals: reads sprite2; writes sprite2. */ /* PLATFORM(video): draws pixels, sprites, or text. */ sprite_setup1_from_arg_pointer(&sprite2) /* PLATFORM(video): select the sprite described by the argument pointer. */; sprite_clear_1_color(0) /* PLATFORM(video): clear the active sprite buffer with the requested color. */; }

/* target sprite_copy_wnd_to_1 @ 6842; accepted source src/sprite_copy_wnd_to_1.c */
void sprite_clear_1_color(unsigned char color);void sprite_copy_wnd_to_1(void) { /* PURPOSE: Set sprite buffer 1 from the active window sprite. Params: none. Returns: void. Globals: reads g_wndspr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ sprite_setup1_from_arg_pointer(g_wndspr) /* PLATFORM(video): select the sprite described by the argument pointer. */; }

/* target sprite_copy_wnd_to_1_clear @ 6860; accepted source src/sprite_copy_wnd_to_1_clear.c */
void sprite_clear_1_color(unsigned char color);void sprite_copy_wnd_to_1_clear(void) { /* PURPOSE: Set sprite buffer 1 from the active window sprite and clear its color. Params: none. Returns: void. Globals: reads g_wndspr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ sprite_setup1_from_arg_pointer(g_wndspr) /* PLATFORM(video): select the sprite described by the argument pointer. */; sprite_clear_1_color(0) /* PLATFORM(video): clear the active sprite buffer with the requested color. */; }

/* target input_repeat_check @ 7306; candidate from build\workers\inputtu\repeat_types_ulong.c */
/* merged owner s008-rest member intro_draw_text */
int * introtext(char *str, int x, int y, int color, int shadow)
{ /* PURPOSE: Draw a line of introductory text with the selected font and color. Params: str, x, y, color, shadow. Returns: int *. Globals: reads fontdefvalue; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    extern int textboundsleft, text_bounds_right, text_bounds_upper, txt_bounds_bottom;
    extern unsigned int fontdefvalue;
    extern void far font_setup_unknown(int color, int flags);
    extern void far font_draw_text(char *text, int x, int y);

    text_bounds_upper = y;
    txt_bounds_bottom = y + ((int)fontdefvalue) + 1;
    textboundsleft = x;
    text_bounds_right = x + font_op2(str) /* PLATFORM(video): load or select a named font. */ + 1;
    font_setup_unknown(shadow, 0) /* PLATFORM(video): select the font and color mode. */;
    font_draw_text(str, x + 1, y + 1) /* PLATFORM(video): draw text through the selected font. */;
    font_setup_unknown(color, 0) /* PLATFORM(video): select the font and color mode. */;
    font_draw_text(str, x, y) /* PLATFORM(video): draw text through the selected font. */;
    return &textboundsleft;
}

/* merged owner s008-rest member hiscore_draw_text */
int * hiscore_draw_text(char *str, int x, int y, int color, int shadow)
{ /* PURPOSE: Draw a high-score text line with its outline bounds. Params: str, x, y, color, shadow. Returns: int *. Globals: reads fontdefvalue, text_outline_bottom, text_outline_right, textoutlineleft, txt_outline_top; writes text_outline_bottom, text_outline_right, textoutlineleft, txt_outline_top. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    extern int textoutlineleft, text_outline_right, txt_outline_top, text_outline_bottom;
    extern unsigned int fontdefvalue;
    extern void far font_setup_unknown(int color, int flags);
    extern void far font_draw_text(char *text, int x, int y);

    txt_outline_top = y - 1;
    text_outline_bottom = y + ((int)fontdefvalue) + 1;
    textoutlineleft = x - 1;
    text_outline_right = x + font_op2(str) /* PLATFORM(video): load or select a named font. */ + 1;
    font_setup_unknown(shadow, 0) /* PLATFORM(video): select the font and color mode. */;
    font_draw_text(str, x + 1, y + 1) /* PLATFORM(video): draw text through the selected font. */;
    font_draw_text(str, x - 1, y + 1) /* PLATFORM(video): draw text through the selected font. */;
    font_draw_text(str, x + 1, y - 1) /* PLATFORM(video): draw text through the selected font. */;
    font_draw_text(str, x - 1, y - 1) /* PLATFORM(video): draw text through the selected font. */;
    font_setup_unknown(color, 0) /* PLATFORM(video): select the font and color mode. */;
    font_draw_text(str, x, y) /* PLATFORM(video): draw text through the selected font. */;
    return &textoutlineleft;
}

/* merged owner s008-rest member call_read_line */
int far call_read_line(char *buffer, int x, int y, int width, int limit, int flags)
{ /* PURPOSE: Invoke the common line editor at the requested screen position. Params: buffer, x, y, width, limit, flags. Returns: int. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(input_kb): polls or updates keyboard state. */
    int result;
    int index;
    extern int far read_line(int, char *, int, int, int, int, int, void (far *)(void), int, int);
    extern void far nopsub_36AF2(void);

    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    result = read_line(2, buffer, 0, x, x * 9 + 9, y, width,
                       nopsub_36AF2,
                       limit, flags) /* PLATFORM(input_kb): run the keyboard line editor. */;
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    x = strlen(buffer);
    index = x - 1;
    while (buffer[index] == ' ')
        --index;
    buffer[index + 1] = 0;
    return result;
}

int far input_repeat_check(int timeout)
{ /* PURPOSE: Wait for repeated input or timeout while updating the prompt display. Params: timeout. Returns: int. Globals: none. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(input_kb): polls or updates keyboard state. */
    register int result;
    int delta_time;
    register int time_total;
    time_total = 0;
    timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */;
    while (timeout > time_total) {
        delta_time = timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */;
        time_total += delta_time;
        result = input_do_checking(delta_time) /* PLATFORM(input_kb): poll the configured input devices through the far entry point. */;
        if (result != 0) return result;
    }
    return 0;
}

/* target shape3d_init_shape @ 8362; accepted source src/shape3d_init_shape.c */
/* merged owner s008-rest member draw_lines_unk */
void far draw_lines_unknown(int left, int top, int width, int height,
                        int light, int middle, int dark)
{ /* PURPOSE: Draw the line segments associated with the current prompt. Params: left, top, width, height, light, middle, dark. Returns: void. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    register int x_right;
    register int bottom_end;
    int left1, top_row2, top_y1, left2;
    int y_bottom1, right_2, right1, y_bottom2;
    extern void far preRender_line(int x1, int y1, int x2, int y2, int color);

    x_right = left + width;
    bottom_end = top + height;
    preRender_line(left, top, x_right, top, light) /* PLATFORM(video): draw a line primitive. */;
    top_y1 = top + 1;
    preRender_line(left + 1, top_y1, x_right - 1, top_y1, light) /* PLATFORM(video): draw a line primitive. */;
    top_row2 = top + 2;
    preRender_line(left + 2, top_row2, x_right - 2, top_row2, middle) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(left, top, left, bottom_end, light) /* PLATFORM(video): draw a line primitive. */;
    left1 = left + 1;
    preRender_line(left1, top + 1, left1, bottom_end - 1, light) /* PLATFORM(video): draw a line primitive. */;
    left2 = left + 2;
    preRender_line(left2, top + 2, left2, bottom_end - 2, middle) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(left, bottom_end, x_right, bottom_end, dark) /* PLATFORM(video): draw a line primitive. */;
    y_bottom1 = bottom_end - 1;
    preRender_line(left + 1, y_bottom1, x_right - 1, y_bottom1, dark) /* PLATFORM(video): draw a line primitive. */;
    y_bottom2 = bottom_end - 2;
    preRender_line(left + 2, y_bottom2, x_right - 2, y_bottom2, middle) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(x_right, top, x_right, bottom_end, dark) /* PLATFORM(video): draw a line primitive. */;
    right1 = x_right - 1;
    preRender_line(right1, top + 1, right1, bottom_end - 1, dark) /* PLATFORM(video): draw a line primitive. */;
    right_2 = x_right - 2;
    preRender_line(right_2, top + 2, right_2, bottom_end - 2, middle) /* PLATFORM(video): draw a line primitive. */;
}

/* merged owner s008-rest member draw_button */
void far draw_button(char far *caption, int x, int y, int width, int height,
                     int light, int dark, int sprite_id, int font_style)
{ /* PURPOSE: Draw a button body and its centered label. Params: caption, x, y, width, height, light, dark, sprite_id, font_style. Returns: void. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    int pos;
    char c;
    int line_no;
    char line[80];
    int line_cnt;
    int text_y;
    register int rgt;
    register int bottom;
    int idx;
    int text_size;
    extern char resbuftext[];
    extern int far sprite_1_unk(int, int, int, int, int);
    extern void far preRender_line(int, int, int, int, int);
    extern void far font_setup_unknown(int, int);
    extern void far font_draw_text(char *, int, int);

    rgt = x + width;
    bottom = y + height;
    sprite_1_unk(x, y, width, height, sprite_id) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
    preRender_line(x, y, rgt, y, light) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(x + 1, y + 1, rgt - 1, y + 1, light) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(x + 2, y + 2, rgt - 2, y + 2, light) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(x, y, x, bottom, light) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(x + 1, y + 1, x + 1, bottom - 1, light) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(x + 2, y + 2, x + 2, bottom - 2, light) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(x, bottom, rgt, bottom, dark) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(x + 1, bottom - 1, rgt - 1, bottom - 1, dark) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(x + 2, bottom - 2, rgt - 2, bottom - 2, dark) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(rgt, y, rgt, bottom, dark) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(rgt - 1, y + 1, rgt - 1, bottom - 1, dark) /* PLATFORM(video): draw a line primitive. */;
    preRender_line(rgt - 2, y + 2, rgt - 2, bottom - 2, dark) /* PLATFORM(video): draw a line primitive. */;
    if (caption == 0)
        return;
    font_setup_unknown(font_style, 0) /* PLATFORM(video): select the font and color mode. */;
    copy_string(resbuftext, caption);
    line_cnt = 1;
    text_size = strlen(resbuftext);
    for (idx = 0; idx < text_size; idx++)
        if (resbuftext[idx] == ']')
            line_cnt++;
    pos = 0;
    line_no = 0;
    text_y = (height - line_cnt * 8) / 2 + 1;
    for (idx = 0; idx < text_size + 1; idx++) {
        if ((c = resbuftext[idx]) == ']' || c == 0) {
            line[pos] = 0;
            font_draw_text(line, x + (width - font_op2(line) /* PLATFORM(video): load or select a named font. */) / 2,
                           line_no * 8 + y + text_y) /* PLATFORM(video): draw text through the selected font. */;
            line_no++;
            pos = 0;
        } else {
            line[pos] = c;
            pos++;
        }
    }
}

void shape3d_init_shape(char far *shapeptr, struct SHAPE3D *gameshape)
{ /* PURPOSE: Copy the compact 3D shape header into the game shape record. Params: shapeptr, gameshape. Returns: void. Globals: none. */
    gameshape->numverts = ((struct SHAPE3DHEADER far *)shapeptr)->numverts;
    gameshape->numprimitives = ((struct SHAPE3DHEADER far *)shapeptr)->numprimitives;
    gameshape->numpaints = ((struct SHAPE3DHEADER far *)shapeptr)->numpaints;
    gameshape->verts = shapeptr + 4;
    gameshape->cull1 = shapeptr + gameshape->numverts * 6 + 4;
    gameshape->cull2 = shapeptr + gameshape->numprimitives * 4
                       + gameshape->numverts * 6 + 4;
    gameshape->primitives = shapeptr + gameshape->numprimitives * 8
                            + gameshape->numverts * 6 + 4;
}

/* target font_op2_alt @ 8534; accepted source src/font_op2_alt.c */
int far font_op2_alt(char *name)
{ /* PURPOSE: Load or select a named font through the far font entry point. Params: name. Returns: int. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    return (320 - font_op2(name) /* PLATFORM(video): load or select a named font. */) / 2;
}

/* target sprite_blit_to_video @ 8560; candidate from build\workers\tuseg008\sprite_blit_to_video.c */
int far sprite_blit_to_video(struct SPRITE far *sprite, int mode)
{ /* PURPOSE: Commit the active sprite buffer to the video display. Params: sprite, mode. Returns: int. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    register int index;
    register int result;
    sprcopy2to12() /* PLATFORM(video): copy the current sprite into the active buffer. */;
    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    if (mode == -2) {
        sprputimage(sprite->sprite_bitmapptr) /* PLATFORM(video): draw the selected shape. */;
        msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
        return 0;
    }
    switch (mode) {
    default:
        for (index = 0; index < 4; ++index) {
            result = input_do_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices through the far entry point. */;
            if (result != 0) {
                sprcopy2to12() /* PLATFORM(video): copy the current sprite into the active buffer. */;
                sprputimage(sprite->sprite_bitmapptr) /* PLATFORM(video): draw the selected shape. */;
                msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
                return result;
            }
            sprite_1_unk3(sprite->sprite_bitmapptr, index) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        }
    }
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    return 0;
}

/* target reset_idle_counters @ 8898; accepted source src/reset_idle_counters.c */
/* merged owner s008-rest member show_waiting */
void far show_waiting(void)
{ /* PURPOSE: Display the waiting dialog and redraw the mouse pointer. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern int waitm_ms;
    show_dialog(0, 0, locate_text_resource(main_data_file_addr, "wai") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, waitm_ms, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
}

void far print_int_as_string_maybe(char *buffer, int value, int zero_fill, int width)
{ /* PURPOSE: Format an integer in the caller buffer with width and zero-fill options. Params: buffer, value, zero_fill, width. Returns: void. Globals: none. */
    register int length;
    register int index;

    itoa(value, buffer, 10);
    if (width != 0) {
        for (length = 0; buffer[length] != 0; ++length)
            ;
        while (width != length) {
            if (width < length) {
                for (index = 0; index < length; ++index)
                    buffer[index] = buffer[index + 1];
                --length;
            } else if (width > length) {
                for (index = length; index >= 0; --index)
                    buffer[index + 1] = buffer[index];
                buffer[0] = ' ';
                ++length;
            }
        }
    }
    if (zero_fill != 0) {
        length = 0;
        while (buffer[length] == ' ')
            buffer[length++] = '0';
    }
}

void reset_idle_counters(void) { /* PURPOSE: Clear menu animation and idle counters. Params: none. Returns: void. Globals: reads none; writes g_animphase, g_hovercolor_idle, unused_count. */
    g_animphase = 0;
    g_hovercolor_idle = 0;
    unused_count = 0;
}

/* target file_load_audiores @ 9036; candidate from build\references\restunts\src\restunts\c\fileio.c */
/* merged owner s008-rest member mouse_timer_sprite_unk */
int far mouse_timer_sprite_unknown(int index, int *a, int *b, int *c, int *d,
                               int first, int second)
{ /* PURPOSE: Advance the pointer animation and redraw it when its selected state changes. Params: index, a, b, c, d, first, second. Returns: int. Globals: reads g_animphase, g_hovercolor_idle; writes g_animphase, g_hovercolor_idle. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    register int delta;
    register int selected;
    extern void far sprite_1_unk4(int, int, int, int, int);
    delta = (int)timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */;
    g_animphase += delta;
    while (g_animphase > 60)
        g_animphase -= 60;
    if (g_animphase > 30) selected = first;
    else selected = second;
    if (selected != g_hovercolor_idle) {
        g_hovercolor_idle = selected;
        msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
        sprite_1_unk4(a[index], c[index], b[index], d[index], selected) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    }
    return delta;
}

void file_load_audio_resource(const char* songfile, const char* voicefile, const char* name) { /* PURPOSE: Load song and voice resources, initialize them, and finish audio setup. Params: songfile, voicefile, name. Returns: void. Globals: reads musicfile, openvfile; writes is_audioloaded, musicfile, openvfile. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
	void far* audiores;
	openvfile = file_load_resource(5, voicefile) /* PLATFORM(file): load the requested resource type. */;
	musicfile = file_load_resource(4, songfile) /* PLATFORM(file): load the requested resource type. */;
	audiores = init_audio_resources(musicfile, openvfile, name) /* PLATFORM(audio): resolve song or voice resources for the audio engine. */;
	load_audio_finalize(audiores) /* PLATFORM(audio): finish audio-resource initialization. */;
	is_audioloaded = 1;
}

/* target audio_unload @ 9128; candidate from build\workers\tuseg008\audio_unload.c */
void far audio_unload(void)
{ /* PURPOSE: Stop audio playback, release loaded resources, and clear the loaded flag. Params: none. Returns: void. Globals: reads musicfile, openvfile; writes is_audioloaded. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(memory): allocates or releases game-managed memory. */
    audio_driver_func3F(2) /* PLATFORM(audio): stop active audio playback. */;
    mmgr_free(musicfile) /* PLATFORM(memory): release the resource through game memory management. */;
    mmgr_free(openvfile) /* PLATFORM(memory): release the resource through game memory management. */;
    is_audioloaded = 0;
}

/* target font_set_fontdef2 @ 9178; accepted source src/font_set_fontdef2.c */
void far fontsetfontdef2(void far *data)
{ /* PURPOSE: Apply a font-definition pointer and cache its value. Params: data. Returns: void. Globals: reads none; writes fontdefvalue. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    set_fontdefseg(data) /* PLATFORM(video): select the font-definition data segment. */;
    fontdefvalue = ((struct FONTDEF_PREFIX far *)data)->value;
}

/* target font_set_fontdef @ 9208; candidate from build\workers\font\font_set_fontdef.c */
void far fontsetfontdef(void)
{ /* PURPOSE: Apply the currently selected font definition. Params: none. Returns: void. Globals: reads def_fntadr; writes none. */
    fontsetfontdef2(def_fntadr);
}

/* target get_super_random @ 9438; candidate from build\references\restunts\src\restunts\c\restunts.c */
void far fmtframestr(char *destination, unsigned int frame_count, int hundredths)
{ /* PURPOSE: Format a frame count as a minutes, seconds, and optional hundredths string. Params: destination, frame_count, hundredths. Returns: void. Globals: reads rate_frame; writes none. */
    register int minute_count;
    char buffer[18];
    register int seconds;
    int minute_frames;

    minute_frames = 60 * rate_frame;
    minute_count = frame_count / minute_frames;
    frame_count -= minute_count * minute_frames;
    seconds = frame_count / rate_frame;
    frame_count -= seconds * rate_frame;
    print_int_as_string_maybe(buffer, minute_count, 0, 2);
    strcpy(destination, buffer);
    strcat(destination, ":");
    print_int_as_string_maybe(buffer, seconds, 1, 2);
    strcat(destination, buffer);
    if (hundredths != 0) {
        strcat(destination, ".");
        print_int_as_string_maybe(buffer, frame_count * (100 / rate_frame), 1, 2);
        strcat(destination, buffer);
    }
}

int get_super_random(void)
{ /* PURPOSE: Combine timer, random, and race-frame values into a nonnegative result. Params: none. Returns: int. Globals: reads race_stats; writes none. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    register int val = (int)(timer_get_counter() /* PLATFORM(timer): read the current timer count. */ + get_kevinrandom() + rand() + race_stats.game_frame);
    return val < 0 ? -val : val;
}

/* target file_load_resource @ 9498; candidate from build\references\restunts\src\restunts\c\fileio.c */
void far* file_load_resource(int type, const char* filename) { /* PURPOSE: Dispatch loading by resource type and offer retry after a failed load. Params: type, filename. Returns: void *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
	void far* result;
	while (1) {
		switch (type) {
			case 0:
				// try load the file, if it fails, show a dialog, and retry
				result = file_load_binary_nofatal(filename) /* PLATFORM(file): load a binary file without terminating on failure. */;
				goto check_result;

			case 1:
				return file_load_binary_nofatal(filename) /* PLATFORM(file): load a binary file without terminating on failure. */;

			case 7:
				// try load a compressed file
				return file_decomp_nofatal(filename) /* PLATFORM(file): decompress a resource without terminating on failure. */;

			case 2:
				// try load a 2d shape and retry if it failed
				result = load_shape2d_nofatal_thunk(filename) /* PLATFORM(file): load a 2D shape without terminating on failure. */;
				goto check_result;

			case 3:
				// try load a 2d shape and retry if it failed
				result = load_shape2d_res_nofatal_thunk(filename) /* PLATFORM(file): load a 2D shape from resource data. */;
				goto check_result;

			case 4:
				// try load a song file and retry if it failed
				result = load_song_file(filename) /* PLATFORM(file): load song data. */;
				goto check_result;

			case 5:
				// try load a voice file and retry if it failed
				result = load_voice_file(filename) /* PLATFORM(file): load voice data. */;
				goto check_result;

			case 6:
				// try load an sfx file and retry if it failed
				result = load_sfx_file(filename) /* PLATFORM(file): load sound-effect data. */;
				goto check_result;

			case 8:
				// try load a 2d shape and retry if it failed
				result = file_load_shape2d_nofatal2(filename) /* PLATFORM(file): load a 2D shape without terminating on failure. */;
				goto check_result;
			default:
				break;
		}

check_result:
        if (result != 0) return result;
        if (do_dea_textres() == 2) return 0;
	}
}

/* target input_push_status @ 9788; accepted source src/input_push_status.c */
void far *read_file_with_retry(int type, unsigned int first, unsigned int second, unsigned int third)
{ /* PURPOSE: Read the requested resource and offer retry after a read failure. Params: type, first, second, third. Returns: void *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
    void far *result;
    for (;;) {
        switch (type) {
        case 9:
            result = file_read_nofatal(first, second, third) /* PLATFORM(file): read a file without terminating on failure. */;
            break;
        case 10:
            return file_read_nofatal(first, second, third) /* PLATFORM(file): read a file without terminating on failure. */;
        }
        if (result != 0) return result;
        if (do_dea_textres() == 2) return 0;
    }
}

signed char input_status_stack_depth = 0;

void far input_push_status(void)
{ /* PURPOSE: Push the current keyboard and mouse modes onto the input stack. Params: none. Returns: void. Globals: reads input_status_stack_depth, kbormouse, mouse_transparent_mode; writes copy_mouse_modes, input_device_modestack, input_status_stack_depth. */
    copy_mouse_modes[input_status_stack_depth] = mouse_transparent_mode;
    input_device_modestack[input_status_stack_depth] = kbormouse;
    ++input_status_stack_depth;
}

/* target input_pop_status @ 9816; candidate from build\workers\periph\input_pop_status.c */
void far input_pop_status(void)
{ /* PURPOSE: Restore the most recently saved keyboard and mouse modes. Params: none. Returns: void. Globals: reads copy_mouse_modes, input_device_modestack, input_status_stack_depth, kbormouse; writes input_status_stack_depth, kbormouse, mouse_transparent_mode. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    if (input_status_stack_depth != 0) {
        --input_status_stack_depth;
        mouse_transparent_mode = copy_mouse_modes[input_status_stack_depth];
        kbormouse = input_device_modestack[input_status_stack_depth];
        if (kbormouse == 0)
            msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    }
}

/* target do_dea_textres @ 11368; candidate from build\workers\tuseg008\do_dea_textres.c */
/* merged owner s008-rest member do_joy_restext */
void far do_joystick_resource_text(void)
{ /* PURPOSE: Show joystick help, process selection, and restore audio and input state. Params: none. Returns: void. Globals: reads dialogarg2, font_secondary_color, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(input_joy): polls or updates joystick state. */
    extern int word_3F88E;
    extern char byte_3FE00, byte_3B8F2;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int *, int);
    extern void far reset_joystick_selection(void);
    extern int far joystick_flags_to_index(int);
    extern int far kb_check(void);
    extern int far kb_read_char(void);
    extern int far get_joy_flags(void);
    extern int far sprite_1_unk(int, int, int, int, int);
    extern unsigned int dlg_colour;
    extern void far restore_audio_volume(void);
    int ys[9];
    int ht;
    int xs[9];
    int coords[14];
    int cw;
    register int lastpos;
    register int cur;
    int idx;
    char hit[9];

    input_push_status() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    word_3F88E = 1;
    audio_unk() /* PLATFORM(audio): read audio option state. */;
    if (show_dialog(3, 1, locate_text_resource(main_data_file_addr, "joy") /* PLATFORM(file): locate a named text entry in resource data. */,
                    -1, -1, dialogarg2, coords, 0) /* PLATFORM(video): present the interactive dialog renderer. */ > 0) {
        for (idx = 0; idx < 9; ++idx)
            hit[idx] = 0;
        byte_3FE00 = 1;
        msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
        sprite_1_unk(coords[2] - 4, coords[3], 1, coords[13] - coords[3] - 8, dialogarg2) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        sprite_1_unk(coords[4] - 4, coords[5], 1, coords[13] - coords[3] - 8, dialogarg2) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        sprite_1_unk(coords[0], coords[9] - 4, coords[6] - coords[0], 1, dialogarg2) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        sprite_1_unk(coords[0], coords[11] - 4, coords[6] - coords[0], 1, dialogarg2) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        xs[5] = xs[1] = xs[0] = coords[2];
        xs[4] = xs[3] = xs[2] = coords[4];
        xs[8] = xs[7] = xs[6] = coords[0];
        ys[7] = ys[3] = ys[0] = coords[9];
        ys[8] = ys[2] = ys[1] = coords[3];
        ys[6] = ys[5] = ys[4] = coords[11];
        cw = coords[2] - coords[0] - 8;
        ht = coords[9] - coords[1] - 8;
        lastpos = -1;
        reset_joystick_selection();
        for (;;) {
            if (kb_read_char() /* PLATFORM(input_kb): read the selected keyboard character. */ != 0)
                break;
            cur = get_joy_flags() /* PLATFORM(input_joy): read joystick button/direction state. */;
            if (cur & 0x30)
                break;
            cur = joystick_flags_to_index(cur) /* PLATFORM(input_joy): convert joystick flags to a control index. */;
            if (cur == lastpos)
                continue;
            for (idx = 0; idx < 9; ++idx)
                sprite_1_unk(xs[idx], ys[idx], cw, ht, font_secondary_color) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
            sprite_1_unk(xs[cur], ys[cur], cw, ht, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
            lastpos = cur;
            hit[cur] = 1;
        }
        for (idx = 0; idx < 9; ++idx)
            byte_3FE00 &= hit[idx];
        restore_mouse_sprite();
        if (byte_3FE00 == 0)
            show_dialog(1, 1, locate_text_resource(main_data_file_addr, "jox") /* PLATFORM(file): locate a named text entry in resource data. */,
                        -1, -1, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    } else {
        byte_3FE00 = 0;
    }
    kb_check() /* PLATFORM(input_kb): poll keyboard state. */;
    byte_3B8F2 = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    word_3F88E = 0;
    input_pop_status() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_key_restext */
void far do_key_resource_text(void)
{ /* PURPOSE: Show keyboard help and restore audio and input state. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    extern unsigned int word_3F88E;
    extern unsigned char byte_3FE00, byte_3B8F2;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern void far restore_audio_volume(void);

    input_push_status() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    ((unsigned int)word_3F88E) = 1;
    audio_unk() /* PLATFORM(audio): read audio option state. */;
    show_dialog(4, 1, locate_text_resource(main_data_file_addr, "key") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    ((unsigned char)byte_3FE00) = 0;
    ((unsigned char)byte_3B8F2) = 0;
    ((unsigned int)word_3F88E) = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    input_pop_status() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_mou_restext */
void far do_mou_resource_text(void)
{ /* PURPOSE: Show mouse help and restore audio and input state. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    extern unsigned int word_3F88E;
    extern unsigned char byte_3B8F2;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern void far restore_audio_volume(void);
    input_push_status() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    ((unsigned int)word_3F88E) = 1;
    audio_unk() /* PLATFORM(audio): read audio option state. */;
    ((unsigned char)byte_3B8F2) = 1;
    show_dialog(4, 1, locate_text_resource(main_data_file_addr, "mou") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    ((unsigned int)word_3F88E) = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    input_pop_status() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_pau_restext */
void far do_pau_restext(void)
{ /* PURPOSE: Show pause help and restore audio and input state. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    extern unsigned int word_3F88E;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern void far restore_audio_volume(void);
    input_push_status() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    ((unsigned int)word_3F88E) = 1;
    audio_unk() /* PLATFORM(audio): read audio option state. */;
    show_dialog(1, 1, locate_text_resource(main_data_file_addr, "pau") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    ((unsigned int)word_3F88E) = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    input_pop_status() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_mof_restext */
void far do_mof_resource_text(void)
{ /* PURPOSE: Toggle music and effects options from their help page. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    extern unsigned int word_3F88E;
    extern int far audio_toggle_flag2(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    input_push_status() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    ((unsigned int)word_3F88E) = 1;
    if (audio_toggle_flag2() /* PLATFORM(audio): toggle the music/effects audio option. */)
        show_dialog(4, 1, locate_text_resource(main_data_file_addr, "mon") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    else
        show_dialog(4, 1, locate_text_resource(main_data_file_addr, "mof") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    ((unsigned int)word_3F88E) = 0;
    input_pop_status() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_sonsof_restext */
void far do_sonsof_resource_text(void)
{ /* PURPOSE: Toggle sound options from their help page. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    extern unsigned int word_3F88E;
    extern int far audio_toggle_flag6(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    input_push_status() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    ((unsigned int)word_3F88E) = 1;
    if (audio_toggle_flag6() /* PLATFORM(audio): toggle the sound audio option. */)
        show_dialog(4, 1, locate_text_resource(main_data_file_addr, "son") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    else
        show_dialog(4, 1, locate_text_resource(main_data_file_addr, "sof") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    ((unsigned int)word_3F88E) = 0;
    input_pop_status() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_dos_restext */
void far do_dos_resource_text(void)
{ /* PURPOSE: Show DOS options and run the exit-list action when selected. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(dos): uses the DOS exit-list service. */
    extern unsigned int word_3F88E;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern void far call_exitlist2(void);
    extern void far restore_audio_volume(void);
    input_push_status() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    ((unsigned int)word_3F88E) = 1;
    audio_unk() /* PLATFORM(audio): read audio option state. */;
    if (show_dialog(2, 1, locate_text_resource(main_data_file_addr, "dos") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */ == 1)
        call_exitlist2() /* PLATFORM(dos): run the DOS exit-list action. */;
    ((unsigned int)word_3F88E) = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    input_pop_status() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member show_graphic_levels_menu */
void far show_graphic_levels_menu(void)
{ /* PURPOSE: Show graphics-level help and restore audio and input state. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    char menu[512];
    unsigned char active[9];
    char rc;
    int saved_rate;
    register int selection_index;
    register int cursor;
    extern unsigned char detail_lvl;
    extern unsigned int slow_video_mode_state, frm_rate2, word_3F88E; extern int performGraphColor;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);

    input_push_status() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    ((unsigned int)word_3F88E) = 1;
    audio_unk() /* PLATFORM(audio): read audio option state. */;
    saved_rate = frm_rate2;
    rc = 0;
    for (;;) {
        copy_string(menu, locate_text_resource(main_data_file_addr, "mrl") /* PLATFORM(file): locate a named text entry in resource data. */);
        for (selection_index = 0; selection_index < 9; ++selection_index) active[selection_index] = 0;
        active[detail_lvl] = 1;
        active[slow_video_mode_state + 5] = 1;
        if (frm_rate2 == 10) active[7] = 1;
        else active[8] = 1;
        cursor = 0;
        for (selection_index = 0; selection_index < 9; ++selection_index) {
            while (menu[cursor] != '[') ++cursor;
            if (active[selection_index]) menu[cursor + 1] = '*';
            ++cursor;
        }
        rc = show_dialog(2, 1, (char far *)menu,
                         0xffff, 0xffff, performGraphColor, 0, rc) /* PLATFORM(video): present the interactive dialog renderer. */;
        switch (rc) {
        case -1: goto after_menu;
        case 5: slow_video_mode_state = 0; continue;
        case 6: slow_video_mode_state = 1; continue;
        case 7: frm_rate2 = 10; continue;
        case 8: frm_rate2 = 20; continue;
        case 9: goto after_menu;
        default: detail_lvl = rc; continue;
        }
    }
after_menu:
    if (saved_rate != frm_rate2)
        show_dialog(1, 1, locate_text_resource(main_data_file_addr, "mrs") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    ((unsigned int)word_3F88E) = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    input_pop_status() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

short far do_dea_textres(void)
{ /* PURPOSE: Show the general help dialog while preserving nested input state. Params: none. Returns: short. Globals: reads dialogarg2, g_is_busy, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    short result;
    input_push_status() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    if (g_is_busy != 0) {
        result = show_dialog(2, 1, locate_text_resource(main_data_file_addr, "dea") /* PLATFORM(file): locate a named text entry in resource data. */,
                             0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
        if (result != 0) result = 0;
        else result = 1;
    } else {
        show_dialog(1, 1, locate_text_resource(main_data_file_addr, "der") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
        result = 1;
    }
    input_pop_status() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
    return result;
}

/* target do_mer_restext @ 11600; reconstructed from anchored call sequence */
/* merged owner s008-rest member ensure_file_exists */
void far ensure_file_exists(int index)
{ /* PURPOSE: Find the requested file and ask for another path when it is absent. Params: index. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes kbormouse. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern int far file_find(char *filename);
    while (file_find(findfilenames[index - 1]) /* PLATFORM(file): search for a file by name. */ == 0) {
        show_dialog(1, 1, locate_text_resource(main_data_file_addr, findfiletexts[index - 1]) /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
        msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
        kbormouse = 0;
    }
}

void far do_mer_restext(void)
{ /* PURPOSE: Show the multiplayer help dialog. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    show_dialog(1, 1, locate_text_resource(main_data_file_addr, "mer") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, 0xffff, dialogarg2, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
}

/* target timer_get_delta_alt @ 11648; accepted source src/timer_get_delta_alt.c */
unsigned long timer_get_delta_alt(void)
{ /* PURPOSE: Return the timer delta through the far entry point. Params: none. Returns: unsigned long. Globals: none. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    return timer_get_delta() /* PLATFORM(timer): read the elapsed timer delta. */;
}

/* target file_load_3dres @ 11654; candidate from build\workers\tuseg008\file_load_3dres_scope.c */
void far *file_load_3dres(char *filename)
{ /* PURPOSE: Load a 3D resource file and retry after a failed load. Params: filename. Returns: void *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
    void far *result;
    while (1) {
        char name[0x50];
        strcpy(name, filename);
        strcat(name, ".p3s");
        result = file_load_resource(7, name) /* PLATFORM(file): load the requested resource type. */;
        if (result == 0) {
            strcpy(name, filename);
            strcat(name, ".3sh");
            result = file_load_resource(1, name) /* PLATFORM(file): load the requested resource type. */;
            if (result == 0) {
                do_dea_textres();
                continue;
            }
        }
        return result;
    }
}

