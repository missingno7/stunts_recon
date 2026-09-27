extern char *itoa(int value, char *buffer, int radix);

extern unsigned strlen(char *s);
extern char *strcpy(char *destination, char *source);
extern char *strcat(char *destination, char *source);
extern void far* mmgr_free(char far* ptr);
extern char far *locate_shape_fatal(char far *data, char *name);
extern char textresprefix;
extern int mouse_xpos;
extern int word_361CE;
extern char kbormouse;
struct SHAPE2D { unsigned short words[6]; unsigned char bytes[4]; };
struct SPRITE { struct SHAPE2D far *sprite_bitmapptr; unsigned short words[3]; unsigned int *lineofs; unsigned short words2[9]; };
extern struct SPRITE far sprite2;
extern struct SPRITE far *wndsprite;
extern struct SPRITE far *mouseunkspriteptr;
extern struct SPRITE far *mmouspriteptr;
extern struct SPRITE far *smouspriteptr;
void sprite_set_1_from_argptr(struct SPRITE far *argsprite);
void sprite_clear_1_color(unsigned char color);
void far sprite_copy_2_to_1(void);
void far sprite_copy_both_to_arg(struct SPRITE *argsprite);
void far sprite_copy_arg_to_both(struct SPRITE *argsprite);
void far sprite_putimage(struct SHAPE2D far *shape);
void far sprite_clear_shape_alt(struct SHAPE2D far *shape, int x, int y);
void far sprite_putimage_and(struct SHAPE2D far *shape, unsigned short x, unsigned short y);
void far sprite_putimage_or(struct SHAPE2D far *shape, unsigned short x, unsigned short y);
void far sprite_1_unk3(struct SHAPE2D far *shape, int index);
struct SHAPE3DHEADER { unsigned char numverts, numprimitives, numpaints, reserved; };
struct SHAPE3D { unsigned short numverts; char far *verts; unsigned short numprimitives; unsigned char numpaints, reserved; char far *primitives; char far *cull1; char far *cull2; };
extern int far font_op2(char *name);
extern int word_A5AC, word_A596;
extern unsigned int word_9282;
extern int word_9260;
struct FONTDEF_PREFIX { unsigned char bytes[14]; unsigned short value; };
extern unsigned int fontdef_value;
extern void far *fontdefptr;
extern void far set_fontdefseg(void far *data);
extern signed char byte_3B8F7;
extern signed char byte_45D0C[], byte_45D14[];
extern unsigned long timer_get_delta(void);
extern unsigned long far timer_get_delta_alt(void);
extern unsigned long far timer_get_counter(void);
extern int far rand(void);
extern int far get_kevinrandom(void);
extern unsigned short gState_frame;
extern int far input_checking(int delta);
extern int far input_do_checking(int delta);
extern int far input_repeat_check(int timeout);
extern int kbjoyflags;
extern int mouse_butstate, mouse_xpos, mouse_ypos;
extern int video_flag2_is1;
extern char mouse_isdirty;
extern char g_is_busy;
extern unsigned short dialogarg2;
extern void far *mainresptr;
extern int far kb_get_char(void);
extern int far get_joy_flags(void);
extern int far get_kb_or_joy_flags(void);
extern void far mouse_get_state(int *buttons, int *x, int *y);
extern void far mouse_draw_opaque(void);
extern void far mouse_draw_transparent(void);
extern void far mouse_draw_opaque_check(void);
extern void far mouse_draw_transparent_check(void);
extern void far input_pop_status(void);
extern void far check_input(void);
extern void far mouse_draw_opaque_check(void);
extern void far mouse_draw_transparent_check(void);
extern void far input_pop_status(void);
extern void far file_build_path(char *dir, char *name, char *ext, char *dst);
extern void far *file_load_resource(int type, char *filename);
extern void far *file_load_resfile(char *filename);
extern void far file_load_audiores(char *songfile, char *voicefile, char *name);
extern void far *file_load_3dres(char *filename);
extern short far do_dea_textres(void);
extern void far *file_load_binary_nofatal(char *filename);
extern void far *file_load_shape2d_nofatal_thunk(char *filename);
extern void far *file_load_shape2d_res_nofatal_thunk(char *filename);
extern void far *load_song_file(char *filename);
extern void far *load_voice_file(char *filename);
extern void far *load_sfx_file(char *filename);
extern void far *file_decomp_nofatal(char *filename);
extern void far *file_load_shape2d_nofatal2(char *filename);
extern void far *file_read_nofatal(unsigned int first, unsigned int second, unsigned int third);
extern void far *init_audio_resources(void far *song, void far *voice, char *name);
extern void far load_audio_finalize(void far *audiores);
extern void far audio_driver_func3F(int command);
extern void far *voicefileptr;
extern void far *songfileptr;
extern char is_audioloaded;
extern char far * far locate_text_res(char far *data, char *name);
extern void copy_string(char *destination, char far *source);
extern void parse_filepath_separators(char *dest, char *path);



char *findfiletexts[4] = { "id1", "id2", "id3", "id4" };
char *findfilenames[4] = { "setup.exe", "sdtitl.*", "tedit.*", "opp1.*" };
unsigned int word_3EB90 = 0;

/* target file_build_path @ 4370; candidate from build\workers\tuseg008\file_build_path_ch.c */
/* merged owner s008-a member sub_274B0 */
char far sub_274B0(int x1, int x2, int y1, int y2)
{
    extern int word_9374;
    extern int word_aa60;
    extern unsigned char mouse_buffer_count;
    extern int mouse_saved_x[];
    extern int mouse_saved_y[];
    extern struct SPRITE far *mouse_sprite_stack[];
    extern struct SPRITE far *mouse_background;
    extern long far mmgr_get_res_ofs_diff_scaled(void);
    extern struct SPRITE far *far sprite_make_wnd(int width, int height, int flags);
    extern void far sprite_copy_both_to_arg(struct SPRITE *argsprite);
    extern void far sprite_copy_2_to_1(void);
    extern void far sprite_clear_shape_alt(struct SHAPE2D far *shape, int x, int y);
    struct SPRITE saved_sprites[2];
    long required;
    required = ((long)(x2 - x1) * (y2 - y1)) / (long)(word_9374 * word_aa60) + 18L;
    if (mmgr_get_res_ofs_diff_scaled() <= required) return 0;

    mouse_draw_opaque_check();
    mouse_sprite_stack[mouse_buffer_count] = sprite_make_wnd(x2 - x1, y2 - y1, 15);
    mouse_saved_x[mouse_buffer_count] = x1;
    mouse_saved_y[mouse_buffer_count] = y1;
    sprite_copy_both_to_arg(&saved_sprites[0]);
    mouse_background[mouse_buffer_count * 2] = saved_sprites[0];
    mouse_background[mouse_buffer_count * 2 + 1] = saved_sprites[1];
    sprite_copy_2_to_1();
    sprite_clear_shape_alt(mouse_sprite_stack[mouse_buffer_count]->sprite_bitmapptr,
                           x1, y1);
    ++mouse_buffer_count;
    return 1;
}

/* merged owner s008-a member sub_275C6 */
void far sub_275C6(void)
{
    extern unsigned char mouse_buffer_count;
    extern int mouse_saved_x[];
    extern int mouse_saved_y[];
    extern struct SPRITE far *mouse_sprite_stack[];
    extern struct SPRITE far *mouse_background;
    extern void far sprite_shape_to_1(struct SHAPE2D far *shape, int x, int y);
    extern void far sprite_copy_arg_to_both(struct SPRITE *argsprite);
    extern void far sprite_free_wnd(void far *window);
    struct SPRITE saved_sprites[2];

    if (mouse_buffer_count == 0) return;
    --mouse_buffer_count;
    mouse_draw_opaque_check();
    sprite_shape_to_1(mouse_sprite_stack[mouse_buffer_count]->sprite_bitmapptr,
                      mouse_saved_x[mouse_buffer_count],
                      mouse_saved_y[mouse_buffer_count]);
    saved_sprites[0] = mouse_background[mouse_buffer_count * 2];
    saved_sprites[1] = mouse_background[mouse_buffer_count * 2 + 1];
    sprite_copy_arg_to_both(&saved_sprites[0]);
    sprite_free_wnd(mouse_sprite_stack[mouse_buffer_count]);
    mouse_draw_transparent_check();
}

/* merged owner s008-dlg member show_dialog */
int far show_dialog(int type, int check, char far *message, int x, int y,
                    unsigned int frame_arg, int *disabled, char initial)
{
    extern unsigned int dialog_fnt_colour, word_3EB90, performGraphColor;
    extern unsigned char g_ascii_props[];
    extern int far sprite_set_1_size(int, int, int, int);
    extern void far sprite_1_unk4(int, int, int, int, unsigned int);
    extern void far font_set_unk(unsigned int, unsigned int);
    extern void far sub_345BC(char *, int, int);
    extern int far sub_2EB1E(long);
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

    linehgt = fontdef_value + 2;
    total_h = 0;
    wide = 32;
    mouse_draw_opaque_check();
    textptr = message;
    pos = 0;
    while ((chr = *textptr) != 0) {
        if (chr == ']') {
            textbuf[pos] = 0;
            text_w = font_op2(textbuf);
            if (text_w > wide)
                wide = text_w;
            pos = 0;
            total_h += linehgt;
        } else if (*textptr == '}') {
            textbuf[pos] = 0;
            text_w = font_op2(textbuf);
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
    if (check != 0 && !sub_274B0(dlgframe[0], dlgframe[1], dlgframe[2], dlgframe[3]))
        return -1;
    sprite_copy_2_to_1();
    sprite_set_1_size(dlgframe[0], dlgframe[1], dlgframe[2], dlgframe[3]);
    sprite_clear_1_color(0);
    sprite_1_unk4(x - 4, y - 4, x + wide + 4, y + total_h + 4, frame_arg);
    font_set_unk(dialog_fnt_colour, 0);
    word_3EB90 = 0;
    font_set_unk(dialog_fnt_colour, 0);
    pos = 0;
    markers = 0;
    textptr = message;
    total_h = 1;
    while ((chr = *textptr) != 0) {
        if (chr == '[')
            goto buttons;
        if (chr == ']') {
            textbuf[pos] = 0;
            sub_345BC(textbuf, x, y + total_h);
            pos = 0;
            total_h += linehgt;
            line_start = textptr;
        } else if (*textptr == '}') {
            textbuf[pos] = 0;
            sub_345BC(textbuf, x, y + total_h);
            pos = 0;
            total_h += 4;
            line_start = textptr;
        } else if (*textptr == '@') {
            if (type == 3) {
                textbuf[pos] = 0;
                disabled[markers] = font_op2(textbuf) + x;
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
        btn_ls[count] = font_op2(textbuf) + x;
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
                text_w = font_op2(textbuf);
                pos = 0;
                total_h += linehgt;
            } else if (*textptr == '}') {
                textbuf[pos] = 0;
                text_w = font_op2(textbuf);
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
            text_w = font_op2(textbuf);
        btn_rs[count] = btn_ls[count] + text_w;
        ++count;
    }
    if (count > 2 && btn_ls[0] == btn_ls[1] && btn_ls[1] == btn_ls[2]) {
        for (i = 0; i < count; ++i)
            btn_rs[i] = btn_ls[i] + wide;
    }
    mouse_draw_transparent_check();
    ret = 1;
    switch (type) {
    case 4:
        sub_2EB1E(8L);
        break;
    case 1:
        do {
            key = input_checking(timer_get_delta_alt());
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
        timer_get_delta_alt();
        mouse_draw_opaque_check();
        if (count == 2) {
            i = 0;
            do {
                hot0 = (unsigned char)btn_text[0][i];
                ++i;
            } while (hot0 == ' ');
            if (g_ascii_props[hot0] & 1)
                hot0 = (g_ascii_props[hot0] & 1) ? hot0 + 0x20 : hot0;
            i = 0;
            do {
                hot_1 = (unsigned char)btn_text[1][i];
                ++i;
            } while (hot_1 == ' ');
            if (g_ascii_props[hot_1] & 1)
                hot_1 = (g_ascii_props[hot_1] & 1) ? hot_1 + 0x20 : hot_1;
        }
        busy = 1;
        while (busy) {
            if (ret != oldchoice) {
                mouse_draw_opaque_check();
                for (i = 0; i < count; ++i) {
                    if (disabled != 0 && disabled[i] != 0)
                        font_set_unk(performGraphColor, word_3EB90);
                    else if (ret == i)
                        font_set_unk(word_3EB90, dialog_fnt_colour);
                    else
                        font_set_unk(dialog_fnt_colour, word_3EB90);
                    textptr = btn_text[i];
                    for (pos = 0; pos < lengths[i]; ++pos)
                        labelstr[pos] = textptr[pos];
                    labelstr[pos] = 0;
                    sub_345BC(labelstr, btn_ls[i], btn_ts[i]);
                }
                mouse_draw_transparent_check();
                if (oldchoice == -1)
                    check_input();
                oldchoice = ret;
            }
            key = input_checking(timer_get_delta_alt());
            choice = mouse_multi_hittest(count, btn_ls, btn_rs, btn_ts, btn_bs);
            if (choice != -1) {
                if (disabled == 0)
                    ret = choice;
                else if (disabled[choice] == 0)
                    ret = choice;
            }
            if (count == 2 && key != 0) {
                lowkey = key;
                if (g_ascii_props[lowkey] & 1)
                    lowkey = (g_ascii_props[lowkey] & 1) ? lowkey + 0x20 : lowkey;
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
        sub_275C6();
    return ret;
}

/* merged owner s008-dlg member do_fileselect_dialog */
int far do_fileselect_dialog(char *path, char *selected_name, int attributes,
                             char far *heading)
{
    extern char resID_byte1[];
    extern unsigned int dialog_fnt_colour, word_3EB90, performGraphColor;
    extern unsigned char g_ascii_props[];
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

    if (show_dialog(3, 1, locate_text_res(mainresptr, "loa"),
                    -1, -1, dialogarg2, layout, 0) < 0)
        return 0;

    old_busy = g_is_busy;
    g_is_busy = 1;
    preRender_line(layout[4] - 4, layout[5] + 4, layout[4] + 0xab,
                   layout[5] + 4, dialogarg2);
    font_set_unk(dialog_fnt_colour, word_3EB90);
    copy_string(resID_byte1, heading);
    sub_345BC(resID_byte1, layout[0], layout[1]);
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
    font_set_unk(dialog_fnt_colour, word_3EB90);
    sub_345BC(path, x, layout[3]);

rescan:
    mouse_draw_transparent_check();
    files_found = 0;
    found = file_combine_and_find(path, "*", attributes);
    if (found == 0) {
        nullsub_1();
edit_path:
        font_set_unk(dialog_fnt_colour, word_3EB90);
        if (call_read_line(path, 0x12, x, layout[3], 30000L) != 0x1b)
            goto rescan;
cancel:
        rc = 0;
        goto finish;
    }
    parse_filepath_separators(names[0], found);
    ++files_found;
    while ((found = file_find_next_alt()) != 0) {
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
                    strcpy(resID_byte1, names[idx]);
                    strcpy(names[idx], names[other_idx]);
                    strcpy(names[other_idx], resID_byte1);
                }
            }
        }
    }
    if (files_found > 7) {
        copy_string(resID_byte1, locate_text_res(mainresptr, "lsu"));
        sub_345BC(resID_byte1, font_op2_alt(resID_byte1), hit_t[1]);
        copy_string(resID_byte1, locate_text_res(mainresptr, "lsd"));
        sub_345BC(resID_byte1, font_op2_alt(resID_byte1), hit_t[9] - 1);
    }
    cursor = 0;
    first_visible = 0;
    old_cur = -1;
    prev_top = -1;
    timer_get_delta_alt();
    answer = 0;
    do {
        if (cursor != old_cur || first_visible != prev_top) {
            old_cur = cursor;
            prev_top = first_visible;
            mouse_draw_opaque_check();
            for (idx = 0; idx < 7; ++idx) {
                if (first_visible + idx == cursor)
                    font_set_unk(word_3EB90, dialog_fnt_colour);
                else
                    font_set_unk(dialog_fnt_colour, word_3EB90);
                if (first_visible + idx < files_found) {
                    strcpy(resID_byte1, names[first_visible + idx]);
                    sub_345BC(resID_byte1, x, hit_t[idx + 2]);
                } else
                    sub_345BC("        ", x, hit_t[idx + 2]);
                label_width = font_op2(resID_byte1);
                sprite_1_unk(label_width + x, hit_t[idx + 2], field_end - label_width - x, 8, word_3EB90);
            }
            mouse_draw_transparent_check();
        }
        pressed = input_checking(timer_get_delta_alt());
        button = mouse_multi_hittest(10, hit_l, hit_r, hit_t, hit_b);
        if (button != -1) {
            if (button == 0) {
                if (mouse_butstate & 3) {
                    cursor = 0;
                    first_visible = -1;
                    pressed = 0;
                }
            } else if (button == 1) {
                if (mouse_butstate & 3) {
                    if (cursor + first_visible != 0)
                        --cursor;
                    if (cursor < first_visible)
                        first_visible = cursor;
                    pressed = 0;
                }
            } else if (button == 9) {
                if (mouse_butstate & 3) {
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
            if (g_ascii_props[pressed] & 1 || g_ascii_props[pressed] & 2) {
                first_char = g_ascii_props[pressed] & 1 ? pressed + 0x20 : pressed;
                for (button = 0; button < files_found; ++button) {
                    if ((char)(g_ascii_props[names[button][0]] & 1 ? names[button][0] + 0x20 : names[button][0]) == first_char) {
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
    sub_275C6();
    g_is_busy = old_busy;
    return rc;
}

void file_build_path(char *dir, char *name, char *ext, char *dst)
{
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
{
    extern char resID_byte1;
    extern char far * far locate_text_res(char far *, char *);
    extern void copy_string(char *, char far *);
    extern unsigned int dialog_fnt_colour, word_3EB90;
    extern int far call_read_line(char *, int, int, int, long);
    register int key;
    char accepted;
    register int i;
    int layout[6];

    if (show_dialog(3, 1, locate_text_res(mainresptr, "sav"),
                    -1, -1, dialogarg2, layout, 0) < 0)
        return 0;

    accepted = 0;
    font_set_unk(dialog_fnt_colour, word_3EB90);
    copy_string(&resID_byte1, title);
    sub_345BC(&resID_byte1, layout[0], layout[1]);
    font_set_unk(dialog_fnt_colour, word_3EB90);
    sub_345BC(name, layout[2], layout[3]);
    sub_345BC(directory, layout[4], layout[5]);
    mouse_draw_transparent_check();
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
    sub_275C6();
    return accepted;
}

void parse_filepath_separators(char *dest, char *path)
{
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
{
    register int key;
    register int joy;

    input_framecount += delta;
    if (input_framecount > 20000) {
        input_framecount -= 10000;
        input_framecount2 -= 10000;
        input_framecount3 -= 10000;
    }

    key = kb_get_char();
    if (key != 0) kbormouse = 0;
    joy = get_joy_flags();
    kbjoyflags = get_kb_or_joy_flags();

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
    mouse_get_state(&mouse_butstate, &mouse_xpos, &mouse_ypos);
    if (mouse_oldx != mouse_xpos || mouse_oldy != mouse_ypos ||
        mouse_oldbut != mouse_butstate) {
        mouse_oldx = mouse_xpos;
        mouse_oldy = mouse_ypos;
        kbormouse = 1;
        input_framecounter = 0;
        if (byte_3B8F7 != 0) {
            if (mouse_isdirty != 0) mouse_draw_opaque();
            mouse_draw_transparent();
        }
    } else if (kbormouse != 0) {
        input_framecounter += delta;
        if (input_framecounter > 500) {
            input_framecounter = 0;
            kbormouse = 0;
            if (mouse_isdirty != 0) mouse_draw_opaque();
        }
    }

    if (kbormouse != 0) {
        if (mouse_butstate != mouse_oldbut) {
            mouse_oldbut = mouse_butstate;
mouse_button_code:
            if (mouse_butstate & 1) mousebutinputcode = 0x20;
            else if (mouse_butstate & 2) mousebutinputcode = 0x0d;
            if (mousebutinputcode != 0) input_framecount2 = input_framecount;
            input_framecounter = 0;
        } else if (mouse_butstate != 0 && input_framecount2 + 20 < input_framecount) {
            goto mouse_button_code;
        }
        if (mouse_butstate != 0) {
            if (mouse_butstate & 1) kbjoyflags |= 0x20;
            else if (mouse_butstate & 2) kbjoyflags |= 0x10;
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
int far input_do_checking(int value) { return input_checking(value); }

/* target file_load_resfile @ 5442; candidate from build\workers\tuseg008\file_load_resfile_scope.c */
void far *file_load_resfile(char *filename)
{
    void far *result;
    while (1) {
        char name[0x50];
        strcpy(name, filename);
        strcat(name, ".res");
        result = file_load_resource(1, name);
        if (result == 0) {
            strcpy(name, filename);
            strcat(name, ".pre");
            result = file_load_resource(7, name);
            if (result == 0) {
                do_dea_textres();
                continue;
            }
        }
        return result;
    }
}

/* target unload_resource @ 5576; accepted source src/unload_resource.c */
void far unload_resource(void far* resptr) {
    mmgr_free(resptr);
}

/* target locate_shape_alt @ 5596; accepted source src/locate_shape_alt.c */
char far *locate_shape_alt(char far *data, char *name)
{
    return locate_shape_fatal(data, name);
}

/* target locate_text_res @ 5618; accepted source src/locate_text_res.c */
char far * far locate_text_res(char far *data, char *name)
{
    char textname[4];
    textname[0] = textresprefix;
    textname[1] = name[0];
    textname[2] = name[1];
    textname[3] = name[2];
    return locate_shape_fatal(data, textname);
}

/* target copy_string @ 5670; accepted source src/copy_string.c */
void copy_string(char *destination, char far *source)
{
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
{
    extern int far sprite_1_unk(int, int, int, int, int);
    extern int dialog_fnt_colour;
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
        sprite_1_unk(x, top, width, height, 0);
        if (swapped == 0)
            sprite_1_unk(x + low, top, range_length, height, dialog_fnt_colour);
        else
            sprite_1_unk(x, top + low, width, range_length, dialog_fnt_colour);
        return value;
    case 1:
        break;
    default:
        return value;
    }

    if (swapped == 0)
        pointer = mouse_xpos - x;
    else
        pointer = mouse_ypos - top;

    if (pointer < low || pointer > end_position) {        do {
            input_checking(timer_get_delta_alt());
        } while ((*(unsigned char *)&mouse_butstate & 3) != 0);
        if (pointer < low) {
            if (value != 0) --value;
        } else if (value < divisions - 1) {
            ++value;
        }
    } else {
        value = -1;
        past_thumb = low;
        do {
            input_checking(timer_get_delta_alt());
            if (swapped == 0)
                location = mouse_xpos - x;
            else
                location = mouse_ypos - top;
            thumb = location - pointer + low;
            if (thumb < 0) {
                thumb = 0;
            } else if (thumb + range_length > max_value - 1) {
                thumb = max_value - range_length - 1;
            }
            if (thumb != past_thumb) {
                past_thumb = thumb;
                mouse_draw_opaque_check();
                sprite_1_unk(x, top, width, height, 0);
                if (swapped == 0)
                    sprite_1_unk(x + thumb, top, range_length, height, dialog_fnt_colour);
                else
                    sprite_1_unk(x, top + thumb, width, range_length, dialog_fnt_colour);
                mouse_draw_transparent_check();
            }
        } while ((*(unsigned char *)&mouse_butstate & 3) != 0);

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
    mouse_draw_opaque_check();
    sprite_1_unk(x, top, width, height, 0);
    if (swapped == 0)
        sprite_1_unk(x + low, top, range_length, height, dialog_fnt_colour);
    else
        sprite_1_unk(x, top + low, width, range_length, dialog_fnt_colour);
    mouse_draw_transparent_check();
    return value;
}

void far mouse_draw_transparent_check(void)
{
    byte_3B8F7 = 1;
    if (kbormouse != 0 && mouse_isdirty == 0)
        mouse_draw_transparent();
}

/* target mouse_draw_opaque_check @ 6406; candidate from build\workers\periph\mouse_draw_opaque_check.c */
void far mouse_draw_opaque_check(void)
{
    byte_3B8F7 = 0;
    if (mouse_isdirty != 0)
        mouse_draw_opaque();
}

/* target mouse_draw_opaque @ 6424; candidate from build\workers\tuseg008\mouse_draw_opaque.c */
void far mouse_draw_opaque(void)
{
    struct SPRITE saved_sprite[2];
    sprite_copy_both_to_arg(saved_sprite);
    sprite_copy_2_to_1();
    sprite_putimage(mouseunkspriteptr->sprite_bitmapptr);
    sprite_copy_arg_to_both(saved_sprite);
    mouse_isdirty = 0;
}

/* target mouse_draw_transparent @ 6484; candidate from build\workers\tuseg008\mouse_draw_transparent.c */
void far mouse_draw_transparent(void)
{
    struct SPRITE saved_sprite[2];
    register int xpos;
    xpos = mouse_xpos;
    xpos -= xpos % video_flag2_is1;
    sprite_copy_both_to_arg(saved_sprite);
    sprite_copy_2_to_1();
    sprite_clear_shape_alt(mouseunkspriteptr->sprite_bitmapptr, xpos, mouse_ypos);
    sprite_putimage_and(mmouspriteptr->sprite_bitmapptr, mouse_xpos, mouse_ypos);
    sprite_putimage_or(smouspriteptr->sprite_bitmapptr, mouse_xpos, mouse_ypos);
    sprite_copy_arg_to_both(saved_sprite);
    mouse_isdirty = 1;
}

/* target mouse_multi_hittest @ 6624; accepted source src/mouse_multi_hittest.c */
int far mouse_multi_hittest(int count, int *left, int *right, int *top, int *bottom)
{
    register int index;
    if (kbormouse != 0) {
        for (index = 0; index < count; ++index) {
            if (left[index] <= mouse_xpos &&
                right[index] >= mouse_xpos &&
                top[index] <= word_361CE &&
                bottom[index] >= word_361CE)
                return (signed char)index;
        }
    }
    return -1;
}

/* target check_input @ 6708; candidate from build\workers\tuseg008\check_input.c */
void far check_input(void)
{
    char done;
    do {
        if (get_kb_or_joy_flags() & 0x30) {
            done = 1;
        } else if (input_checking(timer_get_delta_alt()) != 0) {
            done = 1;
        } else if (kbormouse != 0 && (mouse_butstate & 3) != 0) {
            done = 1;
        } else {
            done = 0;
        }
    } while (done != 0);
}

/* target nopsub_28F26 @ 6774; candidate from build\workers\tuseg008\nopsub_28F26.c */
void far nopsub_28F26(void)
{
    while (input_checking(timer_get_delta_alt()) == 0) { }
    check_input();
}

/* target sprite_copy_2_to_1_2 @ 6796; accepted source src/sprite_copy_2_to_1_2.c */
void sprite_clear_1_color(unsigned char color);void sprite_copy_2_to_1_2(void) { sprite_set_1_from_argptr(&sprite2); }

/* target sprite_copy_2_to_1_clear @ 6814; accepted source src/sprite_copy_2_to_1_clear.c */
void sprite_clear_1_color(unsigned char color);void sprite_copy_2_to_1_clear(void) { sprite_set_1_from_argptr(&sprite2); sprite_clear_1_color(0); }

/* target sprite_copy_wnd_to_1 @ 6842; accepted source src/sprite_copy_wnd_to_1.c */
void sprite_clear_1_color(unsigned char color);void sprite_copy_wnd_to_1(void) { sprite_set_1_from_argptr(wndsprite); }

/* target sprite_copy_wnd_to_1_clear @ 6860; accepted source src/sprite_copy_wnd_to_1_clear.c */
void sprite_clear_1_color(unsigned char color);void sprite_copy_wnd_to_1_clear(void) { sprite_set_1_from_argptr(wndsprite); sprite_clear_1_color(0); }

/* target input_repeat_check @ 7306; candidate from build\workers\inputtu\repeat_types_ulong.c */
/* merged owner s008-rest member intro_draw_text */
int * intro_draw_text(char *str, int x, int y, int color, int shadow)
{
    extern int word_6ad8, word_6ada, word_6adc, word_6ade;
    extern int word_a282;
    extern void far font_set_unk(int color, int flags);
    extern void far font_draw_text(char *text, int x, int y);

    word_6adc = y;
    word_6ade = y + word_a282 + 1;
    word_6ad8 = x;
    word_6ada = x + font_op2(str) + 1;
    font_set_unk(shadow, 0);
    font_draw_text(str, x + 1, y + 1);
    font_set_unk(color, 0);
    font_draw_text(str, x, y);
    return &word_6ad8;
}

/* merged owner s008-rest member hiscore_draw_text */
int * hiscore_draw_text(char *str, int x, int y, int color, int shadow)
{
    extern int word_6ae0, word_6ae2, word_6ae4, word_6ae6;
    extern int word_a282;
    extern void far font_set_unk(int color, int flags);
    extern void far font_draw_text(char *text, int x, int y);

    word_6ae4 = y - 1;
    word_6ae6 = y + word_a282 + 1;
    word_6ae0 = x - 1;
    word_6ae2 = x + font_op2(str) + 1;
    font_set_unk(shadow, 0);
    font_draw_text(str, x + 1, y + 1);
    font_draw_text(str, x - 1, y + 1);
    font_draw_text(str, x + 1, y - 1);
    font_draw_text(str, x - 1, y - 1);
    font_set_unk(color, 0);
    font_draw_text(str, x, y);
    return &word_6ae0;
}

/* merged owner s008-rest member call_read_line */
int far call_read_line(char *buffer, int x, int y, int width, int limit, int flags)
{
    int result;
    int index;
    extern int far read_line(int, char *, int, int, int, int, int, void (far *)(void), int, int);
    extern void far nopsub_36AF2(void);

    mouse_draw_opaque_check();
    result = read_line(2, buffer, 0, x, x * 9 + 9, y, width,
                       nopsub_36AF2,
                       limit, flags);
    mouse_draw_transparent_check();
    x = strlen(buffer);
    index = x - 1;
    while (buffer[index] == ' ')
        --index;
    buffer[index + 1] = 0;
    return result;
}

int far input_repeat_check(int timeout)
{
    register int result;
    int delta_time;
    register int time_total;
    time_total = 0;
    timer_get_delta_alt();
    while (timeout > time_total) {
        delta_time = timer_get_delta_alt();
        time_total += delta_time;
        result = input_do_checking(delta_time);
        if (result != 0) return result;
    }
    return 0;
}

/* target shape3d_init_shape @ 8362; accepted source src/shape3d_init_shape.c */
/* merged owner s008-rest member draw_lines_unk */
void far draw_lines_unk(int left, int top, int width, int height,
                        int light, int middle, int dark)
{
    register int x_right;
    register int bottom_end;
    int left1, top_row2, top_y1, left2;
    int y_bottom1, right_2, right1, y_bottom2;
    extern void far preRender_line(int x1, int y1, int x2, int y2, int color);

    x_right = left + width;
    bottom_end = top + height;
    preRender_line(left, top, x_right, top, light);
    top_y1 = top + 1;
    preRender_line(left + 1, top_y1, x_right - 1, top_y1, light);
    top_row2 = top + 2;
    preRender_line(left + 2, top_row2, x_right - 2, top_row2, middle);
    preRender_line(left, top, left, bottom_end, light);
    left1 = left + 1;
    preRender_line(left1, top + 1, left1, bottom_end - 1, light);
    left2 = left + 2;
    preRender_line(left2, top + 2, left2, bottom_end - 2, middle);
    preRender_line(left, bottom_end, x_right, bottom_end, dark);
    y_bottom1 = bottom_end - 1;
    preRender_line(left + 1, y_bottom1, x_right - 1, y_bottom1, dark);
    y_bottom2 = bottom_end - 2;
    preRender_line(left + 2, y_bottom2, x_right - 2, y_bottom2, middle);
    preRender_line(x_right, top, x_right, bottom_end, dark);
    right1 = x_right - 1;
    preRender_line(right1, top + 1, right1, bottom_end - 1, dark);
    right_2 = x_right - 2;
    preRender_line(right_2, top + 2, right_2, bottom_end - 2, middle);
}

/* merged owner s008-rest member draw_button */
void far draw_button(char far *caption, int x, int y, int width, int height,
                     int light, int dark, int sprite_id, int font_style)
{
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
    extern char resID_byte1[];
    extern int far sprite_1_unk(int, int, int, int, int);
    extern void far preRender_line(int, int, int, int, int);
    extern void far font_set_unk(int, int);
    extern void far font_draw_text(char *, int, int);

    rgt = x + width;
    bottom = y + height;
    sprite_1_unk(x, y, width, height, sprite_id);
    preRender_line(x, y, rgt, y, light);
    preRender_line(x + 1, y + 1, rgt - 1, y + 1, light);
    preRender_line(x + 2, y + 2, rgt - 2, y + 2, light);
    preRender_line(x, y, x, bottom, light);
    preRender_line(x + 1, y + 1, x + 1, bottom - 1, light);
    preRender_line(x + 2, y + 2, x + 2, bottom - 2, light);
    preRender_line(x, bottom, rgt, bottom, dark);
    preRender_line(x + 1, bottom - 1, rgt - 1, bottom - 1, dark);
    preRender_line(x + 2, bottom - 2, rgt - 2, bottom - 2, dark);
    preRender_line(rgt, y, rgt, bottom, dark);
    preRender_line(rgt - 1, y + 1, rgt - 1, bottom - 1, dark);
    preRender_line(rgt - 2, y + 2, rgt - 2, bottom - 2, dark);
    if (caption == 0)
        return;
    font_set_unk(font_style, 0);
    copy_string(resID_byte1, caption);
    line_cnt = 1;
    text_size = strlen(resID_byte1);
    for (idx = 0; idx < text_size; idx++)
        if (resID_byte1[idx] == ']')
            line_cnt++;
    pos = 0;
    line_no = 0;
    text_y = (height - line_cnt * 8) / 2 + 1;
    for (idx = 0; idx < text_size + 1; idx++) {
        if ((c = resID_byte1[idx]) == ']' || c == 0) {
            line[pos] = 0;
            font_draw_text(line, x + (width - font_op2(line)) / 2,
                           line_no * 8 + y + text_y);
            line_no++;
            pos = 0;
        } else {
            line[pos] = c;
            pos++;
        }
    }
}

void shape3d_init_shape(char far *shapeptr, struct SHAPE3D *gameshape)
{
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
{
    return (320 - font_op2(name)) / 2;
}

/* target sprite_blit_to_video @ 8560; candidate from build\workers\tuseg008\sprite_blit_to_video.c */
int far sprite_blit_to_video(struct SPRITE far *sprite, int mode)
{
    register int index;
    register int result;
    sprite_copy_2_to_1_2();
    mouse_draw_opaque_check();
    if (mode == -2) {
        sprite_putimage(sprite->sprite_bitmapptr);
        mouse_draw_transparent_check();
        return 0;
    }
    switch (mode) {
    default:
        for (index = 0; index < 4; ++index) {
            result = input_do_checking(timer_get_delta_alt());
            if (result != 0) {
                sprite_copy_2_to_1_2();
                sprite_putimage(sprite->sprite_bitmapptr);
                mouse_draw_transparent_check();
                return result;
            }
            sprite_1_unk3(sprite->sprite_bitmapptr, index);
        }
    }
    mouse_draw_transparent_check();
    return 0;
}

/* target sub_29772 @ 8898; accepted source src/sub_29772.c */
/* merged owner s008-rest member show_waiting */
void far show_waiting(void)
{
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern int waitflag;
    show_dialog(0, 0, locate_text_res(mainresptr, "wai"),
                0xffff, waitflag, dialogarg2, 0, 0);
    mouse_draw_opaque_check();
}

void far print_int_as_string_maybe(char *buffer, int value, int zero_fill, int width)
{
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

void sub_29772(void) {
    word_A5AC = 0;
    word_A596 = 0;
    word_9282 = 0;
}

/* target file_load_audiores @ 9036; candidate from build\references\restunts\src\restunts\c\fileio.c */
/* merged owner s008-rest member mouse_timer_sprite_unk */
int far mouse_timer_sprite_unk(int index, int *a, int *b, int *c, int *d,
                               int first, int second)
{
    register int delta;
    register int selected;
    extern void far sprite_1_unk4(int, int, int, int, int);
    delta = (int)timer_get_delta_alt();
    word_A5AC += delta;
    while (word_A5AC > 60)
        word_A5AC -= 60;
    if (word_A5AC > 30) selected = first;
    else selected = second;
    if (selected != word_A596) {
        word_A596 = selected;
        mouse_draw_opaque_check();
        sprite_1_unk4(a[index], c[index], b[index], d[index], selected);
        mouse_draw_transparent_check();
    }
    return delta;
}

void file_load_audiores(const char* songfile, const char* voicefile, const char* name) {
	void far* audiores;
	voicefileptr = file_load_resource(5, voicefile);
	songfileptr = file_load_resource(4, songfile);
	audiores = init_audio_resources(songfileptr, voicefileptr, name);
	load_audio_finalize(audiores);
	is_audioloaded = 1;
}

/* target audio_unload @ 9128; candidate from build\workers\tuseg008\audio_unload.c */
void far audio_unload(void)
{
    audio_driver_func3F(2);
    mmgr_free(songfileptr);
    mmgr_free(voicefileptr);
    is_audioloaded = 0;
}

/* target font_set_fontdef2 @ 9178; accepted source src/font_set_fontdef2.c */
void far font_set_fontdef2(void far *data)
{
    set_fontdefseg(data);
    fontdef_value = ((struct FONTDEF_PREFIX far *)data)->value;
}

/* target font_set_fontdef @ 9208; candidate from build\workers\font\font_set_fontdef.c */
void far font_set_fontdef(void)
{
    font_set_fontdef2(fontdefptr);
}

/* target get_super_random @ 9438; candidate from build\references\restunts\src\restunts\c\restunts.c */
void far format_frame_as_string(char *destination, unsigned int frame_count, int hundredths)
{
    register int minute_count;
    char buffer[18];
    register int seconds;
    int minute_frames;

    minute_frames = 60 * word_9260;
    minute_count = frame_count / minute_frames;
    frame_count -= minute_count * minute_frames;
    seconds = frame_count / word_9260;
    frame_count -= seconds * word_9260;
    print_int_as_string_maybe(buffer, minute_count, 0, 2);
    strcpy(destination, buffer);
    strcat(destination, ":");
    print_int_as_string_maybe(buffer, seconds, 1, 2);
    strcat(destination, buffer);
    if (hundredths != 0) {
        strcat(destination, ".");
        print_int_as_string_maybe(buffer, frame_count * (100 / word_9260), 1, 2);
        strcat(destination, buffer);
    }
}

int get_super_random(void)
{
    register int val = (int)(timer_get_counter() + get_kevinrandom() + rand() + gState_frame);
    return val < 0 ? -val : val;
}

/* target file_load_resource @ 9498; candidate from build\references\restunts\src\restunts\c\fileio.c */
void far* file_load_resource(int type, const char* filename) {
	void far* result;
	while (1) {
		switch (type) {
			case 0:
				// try load the file, if it fails, show a dialog, and retry
				result = file_load_binary_nofatal(filename);
				goto check_result;

			case 1:
				return file_load_binary_nofatal(filename);

			case 7:
				// try load a compressed file
				return file_decomp_nofatal(filename);

			case 2:
				// try load a 2d shape and retry if it failed
				result = file_load_shape2d_nofatal_thunk(filename);
				goto check_result;

			case 3:
				// try load a 2d shape and retry if it failed
				result = file_load_shape2d_res_nofatal_thunk(filename);
				goto check_result;

			case 4:
				// try load a song file and retry if it failed
				result = load_song_file(filename);
				goto check_result;

			case 5:
				// try load a voice file and retry if it failed
				result = load_voice_file(filename);
				goto check_result;

			case 6:
				// try load an sfx file and retry if it failed
				result = load_sfx_file(filename);
				goto check_result;

			case 8:
				// try load a 2d shape and retry if it failed
				result = file_load_shape2d_nofatal2(filename);
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
void far *sub_29A86(int type, unsigned int first, unsigned int second, unsigned int third)
{
    void far *result;
    for (;;) {
        switch (type) {
        case 9:
            result = file_read_nofatal(first, second, third);
            break;
        case 10:
            return file_read_nofatal(first, second, third);
        }
        if (result != 0) return result;
        if (do_dea_textres() == 2) return 0;
    }
}

signed char byte_3EBD8 = 0;

void far input_push_status(void)
{
    byte_45D0C[byte_3EBD8] = byte_3B8F7;
    byte_45D14[byte_3EBD8] = kbormouse;
    ++byte_3EBD8;
}

/* target input_pop_status @ 9816; candidate from build\workers\periph\input_pop_status.c */
void far input_pop_status(void)
{
    if (byte_3EBD8 != 0) {
        --byte_3EBD8;
        byte_3B8F7 = byte_45D0C[byte_3EBD8];
        kbormouse = byte_45D14[byte_3EBD8];
        if (kbormouse == 0)
            mouse_draw_opaque_check();
    }
}

/* target do_dea_textres @ 11368; candidate from build\workers\tuseg008\do_dea_textres.c */
/* merged owner s008-rest member do_joy_restext */
void far do_joy_restext(void)
{
    extern int word_3F88E;
    extern char byte_3FE00, byte_3B8F2;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int *, int);
    extern void far sub_307B4(void);
    extern int far sub_307D2(int);
    extern int far kb_check(void);
    extern int far kb_read_char(void);
    extern int far get_joy_flags(void);
    extern int far sprite_1_unk(int, int, int, int, int);
    extern unsigned int dialog_fnt_colour;
    extern void far sub_372F4(void);
    int ys[9];
    int ht;
    int xs[9];
    int coords[14];
    int cw;
    register int lastpos;
    register int cur;
    int idx;
    char hit[9];

    input_push_status();
    word_3F88E = 1;
    audio_unk();
    if (show_dialog(3, 1, locate_text_res(mainresptr, "joy"),
                    -1, -1, dialogarg2, coords, 0) > 0) {
        for (idx = 0; idx < 9; ++idx)
            hit[idx] = 0;
        byte_3FE00 = 1;
        mouse_draw_opaque_check();
        sprite_1_unk(coords[2] - 4, coords[3], 1, coords[13] - coords[3] - 8, dialogarg2);
        sprite_1_unk(coords[4] - 4, coords[5], 1, coords[13] - coords[3] - 8, dialogarg2);
        sprite_1_unk(coords[0], coords[9] - 4, coords[6] - coords[0], 1, dialogarg2);
        sprite_1_unk(coords[0], coords[11] - 4, coords[6] - coords[0], 1, dialogarg2);
        xs[5] = xs[1] = xs[0] = coords[2];
        xs[4] = xs[3] = xs[2] = coords[4];
        xs[8] = xs[7] = xs[6] = coords[0];
        ys[7] = ys[3] = ys[0] = coords[9];
        ys[8] = ys[2] = ys[1] = coords[3];
        ys[6] = ys[5] = ys[4] = coords[11];
        cw = coords[2] - coords[0] - 8;
        ht = coords[9] - coords[1] - 8;
        lastpos = -1;
        sub_307B4();
        for (;;) {
            if (kb_read_char() != 0)
                break;
            cur = get_joy_flags();
            if (cur & 0x30)
                break;
            cur = sub_307D2(cur);
            if (cur == lastpos)
                continue;
            for (idx = 0; idx < 9; ++idx)
                sprite_1_unk(xs[idx], ys[idx], cw, ht, word_3EB90);
            sprite_1_unk(xs[cur], ys[cur], cw, ht, dialog_fnt_colour);
            lastpos = cur;
            hit[cur] = 1;
        }
        for (idx = 0; idx < 9; ++idx)
            byte_3FE00 &= hit[idx];
        sub_275C6();
        if (byte_3FE00 == 0)
            show_dialog(1, 1, locate_text_res(mainresptr, "jox"),
                        -1, -1, dialogarg2, 0, 0);
    } else {
        byte_3FE00 = 0;
    }
    kb_check();
    byte_3B8F2 = 0;
    sub_372F4();
    word_3F88E = 0;
    input_pop_status();
}

/* merged owner s008-rest member do_key_restext */
void far do_key_restext(void)
{
    extern unsigned int word_411e;
    extern unsigned char byte_4690, byte_182;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern void far sub_372F4(void);

    input_push_status();
    word_411e = 1;
    audio_unk();
    show_dialog(4, 1, locate_text_res(mainresptr, "key"),
                0xffff, 0xffff, dialogarg2, 0, 0);
    byte_4690 = 0;
    byte_182 = 0;
    word_411e = 0;
    sub_372F4();
    input_pop_status();
}

/* merged owner s008-rest member do_mou_restext */
void far do_mou_restext(void)
{
    extern unsigned int word_411e;
    extern unsigned char byte_182;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern void far sub_372F4(void);
    input_push_status();
    word_411e = 1;
    audio_unk();
    byte_182 = 1;
    show_dialog(4, 1, locate_text_res(mainresptr, "mou"),
                0xffff, 0xffff, dialogarg2, 0, 0);
    word_411e = 0;
    sub_372F4();
    input_pop_status();
}

/* merged owner s008-rest member do_pau_restext */
void far do_pau_restext(void)
{
    extern unsigned int word_411e;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern void far sub_372F4(void);
    input_push_status();
    word_411e = 1;
    audio_unk();
    show_dialog(1, 1, locate_text_res(mainresptr, "pau"),
                0xffff, 0xffff, dialogarg2, 0, 0);
    word_411e = 0;
    sub_372F4();
    input_pop_status();
}

/* merged owner s008-rest member do_mof_restext */
void far do_mof_restext(void)
{
    extern unsigned int word_411e;
    extern int far audio_toggle_flag2(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    input_push_status();
    word_411e = 1;
    if (audio_toggle_flag2())
        show_dialog(4, 1, locate_text_res(mainresptr, "mon"),
                    0xffff, 0xffff, dialogarg2, 0, 0);
    else
        show_dialog(4, 1, locate_text_res(mainresptr, "mof"),
                    0xffff, 0xffff, dialogarg2, 0, 0);
    word_411e = 0;
    input_pop_status();
}

/* merged owner s008-rest member do_sonsof_restext */
void far do_sonsof_restext(void)
{
    extern unsigned int word_411e;
    extern int far audio_toggle_flag6(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    input_push_status();
    word_411e = 1;
    if (audio_toggle_flag6())
        show_dialog(4, 1, locate_text_res(mainresptr, "son"),
                    0xffff, 0xffff, dialogarg2, 0, 0);
    else
        show_dialog(4, 1, locate_text_res(mainresptr, "sof"),
                    0xffff, 0xffff, dialogarg2, 0, 0);
    word_411e = 0;
    input_pop_status();
}

/* merged owner s008-rest member do_dos_restext */
void far do_dos_restext(void)
{
    extern unsigned int word_411e;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern void far call_exitlist2(void);
    extern void far sub_372F4(void);
    input_push_status();
    word_411e = 1;
    audio_unk();
    if (show_dialog(2, 1, locate_text_res(mainresptr, "dos"),
                    0xffff, 0xffff, dialogarg2, 0, 0) == 1)
        call_exitlist2();
    word_411e = 0;
    sub_372F4();
    input_pop_status();
}

/* merged owner s008-rest member show_graphic_levels_menu */
void far show_graphic_levels_menu(void)
{
    char menu[512];
    unsigned char active[9];
    char rc;
    int saved_rate;
    register int selection_index;
    register int cursor;
    extern unsigned char byte_18a;
    extern unsigned int word_957a, word_95de, word_411e, word_5090;
    extern int far audio_unk(void);
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);

    input_push_status();
    word_411e = 1;
    audio_unk();
    saved_rate = word_95de;
    rc = 0;
    for (;;) {
        copy_string(menu, locate_text_res(mainresptr, "mrl"));
        for (selection_index = 0; selection_index < 9; ++selection_index) active[selection_index] = 0;
        active[byte_18a] = 1;
        active[word_957a + 5] = 1;
        if (word_95de == 10) active[7] = 1;
        else active[8] = 1;
        cursor = 0;
        for (selection_index = 0; selection_index < 9; ++selection_index) {
            while (menu[cursor] != '[') ++cursor;
            if (active[selection_index]) menu[cursor + 1] = '*';
            ++cursor;
        }
        rc = show_dialog(2, 1, (char far *)menu,
                         0xffff, 0xffff, word_5090, 0, rc);
        switch (rc) {
        case -1: goto after_menu;
        case 5: word_957a = 0; continue;
        case 6: word_957a = 1; continue;
        case 7: word_95de = 10; continue;
        case 8: word_95de = 20; continue;
        case 9: goto after_menu;
        default: byte_18a = rc; continue;
        }
    }
after_menu:
    if (saved_rate != word_95de)
        show_dialog(1, 1, locate_text_res(mainresptr, "mrs"),
                    0xffff, 0xffff, dialogarg2, 0, 0);
    word_411e = 0;
    sub_372F4();
    input_pop_status();
}

short far do_dea_textres(void)
{
    short result;
    input_push_status();
    if (g_is_busy != 0) {
        result = show_dialog(2, 1, locate_text_res(mainresptr, "dea"),
                             0xffff, 0xffff, dialogarg2, 0, 0);
        if (result != 0) result = 0;
        else result = 1;
    } else {
        show_dialog(1, 1, locate_text_res(mainresptr, "der"),
                    0xffff, 0xffff, dialogarg2, 0, 0);
        result = 1;
    }
    input_pop_status();
    return result;
}

/* target do_mer_restext @ 11600; reconstructed from anchored call sequence */
/* merged owner s008-rest member ensure_file_exists */
void far ensure_file_exists(int index)
{
    extern int far show_dialog(int, int, char far *, int, int, int, int, int);
    extern int far file_find(char *filename);
    while (file_find(findfilenames[index - 1]) == 0) {
        show_dialog(1, 1, locate_text_res(mainresptr, findfiletexts[index - 1]),
                    0xffff, 0xffff, dialogarg2, 0, 0);
        mouse_draw_opaque_check();
        kbormouse = 0;
    }
}

void far do_mer_restext(void)
{
    show_dialog(1, 1, locate_text_res(mainresptr, "mer"),
                0xffff, 0xffff, dialogarg2, 0, 0);
}

/* target timer_get_delta_alt @ 11648; accepted source src/timer_get_delta_alt.c */
unsigned long timer_get_delta_alt(void)
{
    return timer_get_delta();
}

/* target file_load_3dres @ 11654; candidate from build\workers\tuseg008\file_load_3dres_scope.c */
void far *file_load_3dres(char *filename)
{
    void far *result;
    while (1) {
        char name[0x50];
        strcpy(name, filename);
        strcat(name, ".p3s");
        result = file_load_resource(7, name);
        if (result == 0) {
            strcpy(name, filename);
            strcat(name, ".3sh");
            result = file_load_resource(1, name);
            if (result == 0) {
                do_dea_textres();
                continue;
            }
        }
        return result;
    }
}

