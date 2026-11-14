/* Explicit register declarations below are required by the matching MSC 5.10 build. */
#include "types.h"
/* MSC 5.10 <ctype.h> macros over the pinned runtime table _ctype */
#define isupper(c) ((_ctype+1)[c] & _UPPER)
#define islower(c) ((_ctype+1)[c] & _LOWER)
#define _tolower(c) ((c)-'A'+'a')
#define tolower(c) (isupper(c) ? _tolower(c) : (c))
static I16 textboundsleft, text_bounds_right, text_bounds_upper, txt_bounds_bottom;
static I16 textoutlineleft, text_outline_right, txt_outline_top, text_outline_bottom;
#include "memory.h"
/* Local view: locate_shape_fatal; frontend raw-object exclusion retains its SHAPE2D pointer result. */
extern void FAR * FAR locate_shape_fatal(void FAR*, I8*);

char textrespfxchr;
int msecoordx;
int pos_y_ms;

/* Local view: SHAPE2D; struct.count is 2; another unit uses 6. */
struct SHAPE2D { U16S  words[6]; U8  bytes[4]; };
/* Local view: SPRITE; struct.count is 4; another unit uses 2. */
struct SPRITE { struct SHAPE2D far *sprite_bitmapptr; U16S  words[3]; U16  *lineofs; U16S  words2[9]; };
#include "input.h"
struct SPRITE far *mouse_unk_sprite_ptr;
#include "video.h"

#include "shapes3d.h"
/* Local view: SHAPE3D; struct.count is 8; another unit uses 7. */
struct SHAPE3D { U16S  numverts; I8 far *verts; U16S  numprimitives; U8  numpaints, reserved; I8 far *primitives; I8 far *cull1; I8 far *cull2; };

int g_animphase;
int g_hovercolor_idle;
#include "ui.h"
#include "timer.h"

unsigned int fontdefvalue;

signed char copy_mouse_modes[8];
signed char input_device_modestack[8];

#include "game.h"
#include "audio.h"
#include "math3d.h"
#include "viewstat.h"
#include "files.h"
int kbjoyflags;
#include "car.h"

#define SHAPE3D_RESOURCE_PRIMITIVE_BYTES 8
#define MOUSE_TRACK_FIXED_POINT_SHIFT 2

I16 flagsdown;
void far* main_data_file_addr;
/* Local view: openresource; renderer retains its byte-pointer resource prototype. */
extern void far *far openresource(I8 *);

short voicefile_gap_c[4];
void far *openvfile;
void far *musicfile;

I8 *findfiletexts[4] = { "id1", "id2", "id3", "id4" };
I8 *findfilenames[4] = { "setup.exe", "sdtitl.*", "tedit.*", "opp1.*" };
U16  font_secondary_color = 0;
/* Saves the pixels under a rectangular sprite region when the backing store has room. */
I8 far point_in_rectangle(I16 x1, I16 x2, I16 y1, I16 y2)
{
    extern unsigned char  mouse_buffer_count;
    extern struct SPRITE far *savedptr_ms;

    struct SPRITE saved_sprites[2];
    I32 required;
    required = ((I32)(x2 - x1) * (y2 - y1)) / (I32)(pixel_scales * vidflg4_is1) + SPRITE_WINDOW_OVERHEAD_BYTES;
    if ((I32)availmem() <= required) return 0;

    mouseoff();
    mssprite_arrays[mouse_buffer_count] = sprite_make_window(x2 - x1, y2 - y1, 15);
    g_mousesave_x_tbl[mouse_buffer_count] = x1;
    g_mouseyposstacktable[mouse_buffer_count] = y1;
    savewindow(&saved_sprites[0]);
    savedptr_ms[mouse_buffer_count * SPRITE_SAVE_PAIR_STRIDE] = saved_sprites[0];
    savedptr_ms[mouse_buffer_count * SPRITE_SAVE_PAIR_STRIDE + 1] = saved_sprites[1];
    displaywindow();
    readfxya(mssprite_arrays[mouse_buffer_count]->sprite_bitmapptr,
                           x1, y1);
    ++mouse_buffer_count;
    return 1;
}

int g_mouseyposstacktable[5];

int g_mousesave_x_tbl[4];

struct SPRITE far *mssprite_arrays[4];
/* Restores saved pixels and releases the temporary sprite region. */
void far restoreboxwindow(void)
{
    extern unsigned char  mouse_buffer_count;
    extern struct SPRITE far *savedptr_ms;

    struct SPRITE saved_sprites[2];

    if (mouse_buffer_count == 0) return;
    --mouse_buffer_count;
    mouseoff();
    movfxya(mssprite_arrays[mouse_buffer_count]->sprite_bitmapptr,
                      g_mousesave_x_tbl[mouse_buffer_count],
                      g_mouseyposstacktable[mouse_buffer_count]);
    saved_sprites[0] = savedptr_ms[mouse_buffer_count * SPRITE_SAVE_PAIR_STRIDE];
    saved_sprites[1] = savedptr_ms[mouse_buffer_count * SPRITE_SAVE_PAIR_STRIDE + 1];
    restorewindow(&saved_sprites[0]);
    removewindow(mssprite_arrays[mouse_buffer_count]);
    mouseon();
}

/* Local view: messagebox; renderer retains its excluded unsigned return prototype. */
extern I16 far messagebox();
/* Lays out and draws a dialog, then processes input until a choice is made. */
I16 far messagebox(I16 type, I16 check, I8 far *message, I16 x, I16 y,
                    U16  frame_arg, I16 *disabled, I8 initial)
{
    extern int dlg_colour;
    extern int performGraphColor;
    extern unsigned char _ctype[];

    I8 chr;
    I16 linehgt;
    I8 ret;
    I16 lowkey;
    I8 far *textptr;
    I16 hot_1;
    I8 n_char;
    I16 hot0;
    I16 total_h;
    I8 choice;
    I16 text_w;
    I8 oldchoice;
    I16 btn_ts[UI_DIALOG_BUTTON_CAPACITY];
    I16 i;
    I16 wide;
    I8 labelstr[80];
    U16  key;
    I8 count;
    I8 far *btn_text[UI_DIALOG_BUTTON_CAPACITY];
    I16 btn_bs[UI_DIALOG_BUTTON_CAPACITY];
    I16 btn_rs[UI_DIALOG_BUTTON_CAPACITY];
    I8 markers;
    I8 far *line_start;
    I8 lengths[UI_DIALOG_BUTTON_CAPACITY];
    I8 busy;
    I16 pos;
    I8 textbuf[80];
    I16 dlgframe[4];
    I16 btn_ls[UI_DIALOG_BUTTON_CAPACITY];

    linehgt = fontdefvalue + DIALOG_LINE_SPACING;
    total_h = 0;
    wide = DIALOG_TEXT_MIN_WIDTH;
    mouseoff();
    textptr = message;
    pos = 0;
    while ((chr = *textptr) != 0) {
        if (chr == ']') {
            textbuf[pos] = 0;
            text_w = textpixels(textbuf);
            if (text_w > wide)
                wide = text_w;
            pos = 0;
            total_h += linehgt;
        } else if (*textptr == '}') {
            textbuf[pos] = 0;
            text_w = textpixels(textbuf);
            if (text_w > wide)
                wide = text_w;
            pos = 0;
            total_h += DIALOG_COMPACT_LINE_SPACING;
        } else {
            textbuf[pos++] = *textptr;
        }
        ++textptr;
    }
    wide = (wide + DIALOG_WIDTH_PADDING) & DIALOG_TEXT_WIDTH_ALIGNMENT_MASK;
    if (x == DIALOG_POSITION_CENTER)
        x = ((PLATFORM_SCREEN_WIDTH_PIXELS - wide) / 2) & DIALOG_TEXT_WIDTH_ALIGNMENT_MASK;
    if (y == DIALOG_POSITION_CENTER)
        y = (PLATFORM_SCREEN_HEIGHT_PIXELS - total_h) / 2;
    dlgframe[0] = x;
    dlgframe[1] = x + wide;
    dlgframe[2] = y - DIALOG_FRAME_MARGIN;
    dlgframe[3] = y + total_h + DIALOG_FRAME_MARGIN;
    x += DIALOG_FRAME_MARGIN;
    wide -= DIALOG_CONTENT_WIDTH_INSET;
    if (check != 0 && !point_in_rectangle(dlgframe[0], dlgframe[1], dlgframe[2], dlgframe[3]))
        return -1;
    displaywindow();
    clippix(dlgframe[0], dlgframe[1], dlgframe[2], dlgframe[3]);
    clearwindow(0);
    sprite_draw_rect_outline(x - DIALOG_OUTLINE_MARGIN, y - DIALOG_OUTLINE_MARGIN, x + wide + DIALOG_OUTLINE_MARGIN, y + total_h + DIALOG_OUTLINE_MARGIN, frame_arg);
    textcolor(dlg_colour, 0);
    font_secondary_color = 0;
    textcolor(dlg_colour, 0);
    pos = 0;
    markers = 0;
    textptr = message;
    total_h = 1;
    while ((chr = *textptr) != 0) {
        if (chr == '[')
            goto buttons;
        if (chr == ']') {
            textbuf[pos] = 0;
            movtxy(textbuf, x, y + total_h);
            pos = 0;
            total_h += linehgt;
            line_start = textptr;
        } else if (*textptr == '}') {
            textbuf[pos] = 0;
            movtxy(textbuf, x, y + total_h);
            pos = 0;
            total_h += DIALOG_COMPACT_LINE_SPACING;
            line_start = textptr;
        } else if (*textptr == '@') {
            if (type == 3) {
                textbuf[pos] = 0;
                disabled[markers] = textpixels(textbuf) + x;
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
        btn_ls[count] = textpixels(textbuf) + x;
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
                text_w = textpixels(textbuf);
                pos = 0;
                total_h += linehgt;
            } else if (*textptr == '}') {
                textbuf[pos] = 0;
                text_w = textpixels(textbuf);
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
            text_w = textpixels(textbuf);
        btn_rs[count] = btn_ls[count] + text_w;
        ++count;
    }
    if (count > 2 && btn_ls[0] == btn_ls[1] && btn_ls[1] == btn_ls[2]) {
        for (i = 0; i < count; ++i)
            btn_rs[i] = btn_ls[i] + wide;
    }
    mouseon();
    ret = 1;
    switch (type) {
    case DIALOG_MODE_SHORT_WAIT:
        waitbios(DIALOG_SHORT_WAIT_TICKS);
        break;
    case DIALOG_MODE_WAIT_KEY:
        do {
            key = mykey(elticks());
        } while (key == 0);
        if (key == 27)
            ret = 0;
        clear_keyboardbuffer();
        break;
    case DIALOG_MODE_DRAW_ONLY:
        return 0;
    case DIALOG_MODE_MEASURE_MARKERS:
        return markers / 2;
    case DIALOG_MODE_CHOICE:
        ret = initial;
        oldchoice = -1;
        elticks();
        mouseoff();
        if (count == 2) {
            i = 0;
            do {
                hot0 = (U8 )btn_text[0][i];
                ++i;
            } while (hot0 == ' ');
            if (isupper(hot0))
                hot0 = tolower(hot0);
            i = 0;
            do {
                hot_1 = (U8 )btn_text[1][i];
                ++i;
            } while (hot_1 == ' ');
            if (isupper(hot_1))
                hot_1 = tolower(hot_1);
        }
        busy = 1;
        while (busy) {
            if (ret != oldchoice) {
                mouseoff();
                for (i = 0; i < count; ++i) {
                    if (disabled != 0 && disabled[i] != 0)
                        textcolor(performGraphColor, font_secondary_color);
                    else if (ret == i)
                        textcolor(font_secondary_color, dlg_colour);
                    else
                        textcolor(dlg_colour, font_secondary_color);
                    textptr = btn_text[i];
                    for (pos = 0; pos < lengths[i]; ++pos)
                        labelstr[pos] = textptr[pos];
                    labelstr[pos] = 0;
                    movtxy(labelstr, btn_ls[i], btn_ts[i]);
                }
                mouseon();
                if (oldchoice == -1)
                    clear_keyboardbuffer();
                oldchoice = ret;
            }
            key = mykey(elticks());
            choice = mousecursor(count, btn_ls, btn_rs, btn_ts, btn_bs);
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
                    key = KEY_ASCII_ENTER;
                } else if (hot_1 == lowkey) {
                    ret = 1;
                    key = KEY_ASCII_ENTER;
                }
            }
            switch (key) {
            case KEY_SCAN_UP:
            case KEY_SCAN_LEFT:
                do {
                    if (ret != 0)
                        --ret;
                    else
                        ret = count - 1;
                } while (disabled != 0 && disabled[ret] != 0);
                break;
            case KEY_SCAN_RIGHT:
            case KEY_SCAN_DOWN:
                do {
                    if (ret + 1 < count)
                        ++ret;
                    else
                        ret = 0;
                } while (disabled != 0 && disabled[ret] != 0);
                break;
            case KEY_ASCII_ESCAPE:
                ret = -1;
            case KEY_ASCII_ENTER:
            case ' ':
                busy = 0;
                clear_keyboardbuffer();
                break;
            }
        }
        break;
    }
    if (check != 0)
        restoreboxwindow();
    return ret;
}
/* Builds and display the file list, returning the selected file name. */
/* Local view: do_fileselect_dialog; return is signed word; another unit uses signed byte. */
I16 far floadreq(I8 *path, I8 *selected_name, I16 attributes,
                             I8 far *heading)
{
    extern char resbuftext[80];
    extern int dlg_colour;
    extern unsigned char _ctype[];

    I16 hit_l[10];
    I8 answer;
    I8 old_busy;
    I8 names[128][13];
    I8 rc;
    register unsigned idx;
    I8 first_char;
    I8 first_visible;
    I16 pressed;
    unsigned other_idx;
    I16 hit_b[10];
    I16 hit_r[10];
    I8 old_cur;
    I16 hit_t[10];
    register I16 x;
    I16 field_end;
    I8 button;
    I8 *found;
    I8 cursor;
    I8 files_found;
    I16 layout[20];
    I16 label_width;
    I8 prev_top;

    if (messagebox(3, 1, getresl(main_data_file_addr, "loa"),
                    -1, -1, dialog_frame_color, layout, 0) < 0)
        return 0;

    old_busy = g_is_busy;
    g_is_busy = 1;
    preRender_line(layout[4] - 4, layout[5] + 4, layout[4] + 0xab,
                   layout[5] + 4, dialog_frame_color);
    textcolor(dlg_colour, font_secondary_color);
    strcpyfar(resbuftext, heading);
    movtxy(resbuftext, layout[0], layout[1]);
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
    textcolor(dlg_colour, font_secondary_color);
    movtxy(path, x, layout[3]);

rescan:
    mouseon();
    files_found = 0;
    found = file_combine_and_find(path, "*", attributes);
    if (found == 0) {
        nullsub_1();
edit_path:
        textcolor(dlg_colour, font_secondary_color);
        if (instringz(path, 0x12, x, layout[3], 30000L) != KEY_ASCII_ESCAPE)
            goto rescan;
cancel:
        rc = 0;
        goto finish;
    }
    namecopy(names[0], found);
    ++files_found;
    while ((found = myfindnext()) != 0) {
        namecopy(names[files_found], found);
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
        strcpyfar(resbuftext, getresl(main_data_file_addr, "lsu"));
        movtxy(resbuftext, mycenter(resbuftext), hit_t[1]);
        strcpyfar(resbuftext, getresl(main_data_file_addr, "lsd"));
        movtxy(resbuftext, mycenter(resbuftext), hit_t[9] - 1);
    }
    cursor = 0;
    first_visible = 0;
    old_cur = -1;
    prev_top = -1;
    elticks();
    answer = 0;
    do {
        if (cursor != old_cur || first_visible != prev_top) {
            old_cur = cursor;
            prev_top = first_visible;
            mouseoff();
            for (idx = 0; idx < 7; ++idx) {
                if (first_visible + idx == cursor)
                    textcolor(font_secondary_color, dlg_colour);
                else
                    textcolor(dlg_colour, font_secondary_color);
                if (first_visible + idx < files_found) {
                    strcpy(resbuftext, names[first_visible + idx]);
                    movtxy(resbuftext, x, hit_t[idx + 2]);
                } else
                    movtxy("        ", x, hit_t[idx + 2]);
                label_width = textpixels(resbuftext);
                drawrect(label_width + x, hit_t[idx + 2], field_end - label_width - x, 8, font_secondary_color);
            }
            mouseon();
        }
        pressed = mykey(elticks());
        button = mousecursor(10, hit_l, hit_r, hit_t, hit_b);
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
        case KEY_SCAN_UP:
            --cursor;
            break;
        case KEY_SCAN_DOWN:
            if (files_found - 1 != cursor)
                ++cursor;
            break;
        case KEY_ASCII_ENTER:
        case KEY_ASCII_SPACE:
            answer = 1;
            break;
        case KEY_ASCII_ESCAPE:
            answer = -1;
            break;
        default:
            if (isupper(pressed) || islower(pressed)) {
                first_char = tolower(pressed);
                for (button = 0; button < files_found; ++button) {
                    if ((I8)tolower(names[button][0]) == first_char) {
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
    restoreboxwindow();
    g_is_busy = old_busy;
    return rc;
}
/* Joins a directory, file name, and extension in the destination buffer. */
void makename(I8 *dir, I8 *name, I8 *ext, I8 *dst)
{
    register I16 dirlen;
    I8 last_character;
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

/* Local view: fsavereq; race raw-object exclusion retains its result type. */
extern I16 far fsavereq(I8 *, I8 *, I8 far *);
/* Displays the save dialog and returns the selected file name. */
I16 far fsavereq(I8 *name, I8 *directory, I8 far *title)
{
    extern char resbuftext[80];
    extern int dlg_colour;

    register I16 key;
    I8 accepted;
    register I16 i;
    I16 layout[6];

    if (messagebox(3, 1, getresl(main_data_file_addr, "sav"),
                    -1, -1, dialog_frame_color, layout, 0) < 0)
        return 0;

    accepted = 0;
    textcolor(dlg_colour, font_secondary_color);
    strcpyfar(resbuftext, title);
    movtxy(resbuftext, layout[0], layout[1]);
    textcolor(dlg_colour, font_secondary_color);
    movtxy(name, layout[2], layout[3]);
    movtxy(directory, layout[4], layout[5]);
    mouseon();
    goto read_directory;
    while (key != KEY_ASCII_ENTER) {
        if (instringz(name, 18, layout[2], layout[3], 30000L) == 27)
            goto finish_dialog;
read_directory:
        key = instringz(directory, 8, layout[4], layout[5], 30000L);
        for (i = 0; directory[i] != 0; ++i)
            if (directory[i] == ' ')
                directory[i] = '_';
        if (key == 27)
            goto finish_dialog;
    }
    accepted = 1;
finish_dialog:
    restoreboxwindow();
    return accepted;
}
/* Normalizes path separators and reports whether the path was accepted. */
void namecopy(I8 *dest, I8 *path)
{
    I8 ch;
    I16 len;

    len = strlen(path);
    do {
        ch = path[len - 1];
        if (ch == '\\' || ch == ':') break;
        --len;
    } while (len != 0);
    {
        I16 out;
        out = 0;
        for (;;) {
            dest[out] = path[len++];
            if (dest[out++] == '.') break;
        }
        --out;
        dest[out] = 0;
    }
}

I16 input_framecount2 = 0;
I16 input_framecount3 = 0;
I16 joyflags = 0;
I16 newjoyflags = 0;
I16 mouse_oldx = 0;
I16 mouse_oldy = 0;
I16 mouse_oldbut = 0;
I16 input_framecounter = 0;
I16 joyinputcode = 0;
I16 mousebutinputcode = 0;
I16 input_framecount = 0;
/* Polls the active input devices and updates per-frame input state. */
I16 mykey(I16 delta)
{
    register I16 key;
    register I16 joy;

    input_framecount += delta;
    if (input_framecount > 20000) {
        input_framecount -= 10000;
        input_framecount2 -= 10000;
        input_framecount3 -= 10000;
    }

    key = normkeyi();
    if (key != 0) kbormouse = 0;
    joy = get_joy_flags();
    kbjoyflags = joystate();

    if (joyflags != joy) {
        newjoyflags = (joyflags ^ joy) & joy;
        joyflags = joy;

    joy_decode:
        if (newjoyflags & 0x20) joyinputcode = KEY_ASCII_ENTER;
        else if (newjoyflags & 0x10) joyinputcode = KEY_ASCII_SPACE;
        else if (newjoyflags & 0x01) joyinputcode = KEY_SCAN_UP;
        else if (newjoyflags & 0x02) joyinputcode = KEY_SCAN_DOWN;
        else if (newjoyflags & 0x08) joyinputcode = KEY_SCAN_LEFT;
        else if (newjoyflags & 0x04) joyinputcode = KEY_SCAN_RIGHT;
        if (joyinputcode != 0) {
            input_framecount3 = input_framecount;
            kbormouse = 0;
        }
    } else {
        if (joy != 0 && input_framecount3 + 20 < input_framecount)
            goto joy_decode;
    }

/* MSC 5.10: removing this unused label changes the complete object. */
mouse_poll:
    getmousepos(&flagsdown, &msecoordx, &pos_y_ms);
    if (mouse_oldx != msecoordx || mouse_oldy != pos_y_ms ||
        mouse_oldbut != flagsdown) {
        mouse_oldx = msecoordx;
        mouse_oldy = pos_y_ms;
        kbormouse = 1;
        input_framecounter = 0;
        if (mouseonstat != 0) {
            if (mouseonscreen != 0) dohidemouse();
            doshowmouse();
        }
    } else if (kbormouse != 0) {
        input_framecounter += delta;
        if (input_framecounter > 500) {
            input_framecounter = 0;
            kbormouse = 0;
            if (mouseonscreen != 0) dohidemouse();
        }
    }

    if (kbormouse != 0) {
        if (flagsdown != mouse_oldbut) {
            mouse_oldbut = flagsdown;
mouse_button_code:
            if (flagsdown & 1) mousebutinputcode = KEY_ASCII_SPACE;
            else if (flagsdown & 2) mousebutinputcode = KEY_ASCII_ENTER;
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

/* MSC 5.10: removing this unused label changes the complete object. */
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
/* Forwards the per-frame input poll through the far entry point. */
I16 far hurrykey(I16 value)
{
return mykey(value); }

/* Local view: openresource; renderer retains its byte-pointer resource prototype. */
extern void far *far openresource(I8 *);
/* Loads a resource file and releases it when the requested text resource is absent. */
void far *openresource(I8 *filename)
{
    void far *result;
    while (1) {
        I8 name[0x50];
        strcpy(name, filename);
        strcat(name, ".res");
        result = myload(1, name);
        if (result == 0) {
            strcpy(name, filename);
            strcat(name, ".pre");
            result = myload(7, name);
            if (result == 0) {
                do_dea_textres();
                continue;
            }
        }
        return result;
    }
}
/* Releases a loaded resource through the game memory manager. */
void far closeresource(void far* resptr)
{
    freememory(resptr);
}
/* Finds a named shape in a resource and returns its far pointer. */
I8 far *getres(I8 far *data, I8 *name)
{
    return locate_shape_fatal(data, name);
}
/* Finds a named text entry and records its leading character. */
I8 far * far getresl(I8 far *data, I8 *name)
{
    I8 textname[4];
    textname[0] = textrespfxchr;
    textname[1] = name[0];
    textname[2] = name[1];
    textname[3] = name[2];
    return locate_shape_fatal(data, textname);
}
/* Copies a near or far string into the destination buffer. */
void strcpyfar(I8 *destination, I8 far *source)
{
    I8 far *current = source;

    do {
        *destination = *current;
        ++destination;
        ++current;
    } while (*current != '\0');

    *destination = '\0';
}
/* Tracks mouse movement and processes the current pointer action. */
I16 far doslider(I16 op, I16 x, I16 width, I16 top, I16 height,
                       I16 value, I16 offset, I16 divisions)
{
    extern int dlg_colour;

    register I16 low;
    register I16 end_position;
    I16 swapped;
    I16 max_value;
    I16 pointer;
    I16 past_thumb;
    I16 location;
    I16 thumb;
    I16 range_length;

    if (width > height) {
        swapped = 0;
        max_value = width;
    } else {
        swapped = 1;
        max_value = height;
    }
    {
        I16 end_of_track;
        I16 factor;
        factor = divisions << MOUSE_TRACK_FIXED_POINT_SHIFT;
        end_of_track = max_value - 1;
        low = (end_of_track * value << MOUSE_TRACK_FIXED_POINT_SHIFT) / factor;
        end_position = ((value + offset) * end_of_track << MOUSE_TRACK_FIXED_POINT_SHIFT) / factor;
        range_length = end_position - low;
    }

    switch (op) {
    case 0:
        drawrect(x, top, width, height, 0);
        if (swapped == 0)
            drawrect(x + low, top, range_length, height, dlg_colour);
        else
            drawrect(x, top + low, width, range_length, dlg_colour);
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

    if (pointer < low || pointer > end_position) {
        do {
            mykey(elticks());
        } while ((flagsdown & 3) != 0);
        if (pointer < low) {
            if (value != 0) --value;
        } else if (value < divisions - 1) {
            ++value;
        }
    } else {
        value = -1;
        past_thumb = low;
        do {
            mykey(elticks());
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
                mouseoff();
                drawrect(x, top, width, height, 0);
                if (swapped == 0)
                    drawrect(x + thumb, top, range_length, height, dlg_colour);
                else
                    drawrect(x, top + thumb, width, range_length, dlg_colour);
                mouseon();
            }
        } while ((flagsdown & 3) != 0);

    }

    if (value == -1)
        value = ((max_value / divisions) / 2 + thumb) * divisions / max_value;

    {
        I16 end_of_track;
        I16 factor;
        factor = divisions << MOUSE_TRACK_FIXED_POINT_SHIFT;
        end_of_track = max_value - 1;
        low = (end_of_track * value << MOUSE_TRACK_FIXED_POINT_SHIFT) / factor;
        end_position = ((value + offset) * end_of_track << MOUSE_TRACK_FIXED_POINT_SHIFT) / factor;
        range_length = end_position - low;
    }
    mouseoff();
    drawrect(x, top, width, height, 0);
    if (swapped == 0)
        drawrect(x + low, top, range_length, height, dlg_colour);
    else
        drawrect(x, top + low, width, range_length, dlg_colour);
    mouseon();
    return value;
}
/* Redraws the mouse pointer in transparent mode when it is dirty. */
void far mouseon(void)
{
    mouseonstat = 1;
    if (kbormouse != 0 && mouseonscreen == 0)
        doshowmouse();
}
/* Redraws the mouse pointer in opaque mode when it is dirty. */
void far mouseoff(void)
{
    mouseonstat = 0;
    if (mouseonscreen != 0)
        dohidemouse();
}
/* Saves the sprite buffers, draws the pointer, and restores the buffers. */
void far dohidemouse(void)
{
    struct SPRITE saved_sprite[2];
    savewindow(saved_sprite);
    displaywindow();
    movcf(mouse_unk_sprite_ptr->sprite_bitmapptr);
    restorewindow(saved_sprite);
    mouseonscreen = 0;
}
/* Draws the pointer sprite transparently into the active sprite buffer. */
void far doshowmouse(void)
{
    struct SPRITE saved_sprite[2];
    register I16 xpos;
    xpos = msecoordx;
    xpos -= xpos % g_vid_flg2_set;
    savewindow(saved_sprite);
    displaywindow();
    readfxya(mouse_unk_sprite_ptr->sprite_bitmapptr, xpos, pos_y_ms);
    andcfxya(mouse_ptr_cursor->sprite_bitmapptr, msecoordx, pos_y_ms);
    orcfxya(spritepointermini->sprite_bitmapptr, msecoordx, pos_y_ms);
    restorewindow(saved_sprite);
    mouseonscreen = 1;
}
/* Returns the first candidate rectangle containing the current mouse position. */
I16 far mousecursor(I16 count, I16 *left, I16 *right, I16 *top, I16 *bottom)
{
    register I16 index;
    if (kbormouse != 0) {
        for (index = 0; index < count; ++index) {
            if (left[index] <= msecoordx &&
                right[index] >= msecoordx &&
                top[index] <= pos_y_ms &&
                bottom[index] >= pos_y_ms)
                return (I8S )index;
        }
    }
    return -1;
}
/* Polls keyboard or mouse state and updates the input frame counter. */
void far clear_keyboardbuffer(void)
{
    I8 done;
    do {
        if (joystate() & 0x30) {
            done = 1;
        } else if (mykey(elticks()) != 0) {
            done = 1;
        } else if (kbormouse != 0 && (flagsdown & 3) != 0) {
            done = 1;
        } else {
            done = 0;
        }
    } while (done != 0);
}
/* Runs the waiting-input update and copies the active sprite buffer. */
void far waitforkey(void)
{
    while (mykey(elticks()) == 0) { }
    clear_keyboardbuffer();
}
/* Sets sprite buffer 1 from the current sprite argument. */
void usescreen(void)
{
sprite_setup1_from_arg_pointer(&sprite2); }
/* Sets sprite buffer 1 from the current sprite and clears its color. */
void usescreenc(void)
{
sprite_setup1_from_arg_pointer(&sprite2); clearwindow(0); }
/* Sets sprite buffer 1 from the active window sprite. */
void usebuffer(void)
{
sprite_setup1_from_arg_pointer(g_wndspr); }
/* Sets sprite buffer 1 from the active window sprite and clears its color. */
void usebufferc(void)
{
sprite_setup1_from_arg_pointer(g_wndspr); clearwindow(0); }
/* Draws a line of introductory text with the selected font and color. */
I16 * shadowtext(I8 *str, I16 x, I16 y, I16 color, I16 shadow)
{


    text_bounds_upper = y;
    txt_bounds_bottom = y + fontdefvalue + 1;
    textboundsleft = x;
    text_bounds_right = x + textpixels(str) + 1;
    textcolor(shadow, 0);
    fortextxy(str, x + 1, y + 1);
    textcolor(color, 0);
    fortextxy(str, x, y);
    return &textboundsleft;
}

/* Local view: hiscore_draw_text; frontend retains its excluded void/far prototype. */
extern I16 * hiscore_draw_text(I8 *, I16, I16, I16, I16);
/* Draws a high-score text line with its outline bounds. */
I16 * hiscore_draw_text(I8 *str, I16 x, I16 y, I16 color, I16 shadow)
{


    txt_outline_top = y - 1;
    text_outline_bottom = y + fontdefvalue + 1;
    textoutlineleft = x - 1;
    text_outline_right = x + textpixels(str) + 1;
    textcolor(shadow, 0);
    fortextxy(str, x + 1, y + 1);
    fortextxy(str, x - 1, y + 1);
    fortextxy(str, x + 1, y - 1);
    fortextxy(str, x - 1, y - 1);
    textcolor(color, 0);
    fortextxy(str, x, y);
    return &textoutlineleft;
}
/* Invokes the common line editor at the requested screen position. */
I16 far instringz(I8 *buffer, I16 x, I16 y, I16 width, I16 limit, I16 flags)
{
    I16 result;
    I16 index;


    mouseoff();
    result = get_pstring(2, buffer, 0, x, x * 9 + 9, y, width,
                       kb_line_edit_noop,
                       limit, flags);
    mouseon();
    x = strlen(buffer);
    index = x - 1;
    while (buffer[index] == ' ')
        --index;
    buffer[index + 1] = 0;
    return result;
}
/* Waits for repeated input or timeout while updating the prompt display. */
I16 far mykeywait(I16 timeout)
{
    register I16 result;
    I16 delta_time;
    register I16 time_total;
    time_total = 0;
    elticks();
    while (timeout > time_total) {
        delta_time = elticks();
        time_total += delta_time;
        result = hurrykey(delta_time);
        if (result != 0) return result;
    }
    return 0;
}
/* Draws the line segments associated with the current prompt. */
void far sprite_draw_beveled_frame(I16 left, I16 top, I16 width, I16 height,
                        I16 light, I16 middle, I16 dark)
{
    register I16 x_right;
    register I16 bottom_end;
    I16 left1, top_row2, top_y1, left2;
    I16 y_bottom1, right_2, right1, y_bottom2;

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
/* Draws a button body and its centered label. */
void far draw3dbutton(I8 far *caption, I16 x, I16 y, I16 width, I16 height,
                     I16 light, I16 dark, I16 sprite_id, I16 font_style)
{
    I16 pos;
    I8 c;
    I16 line_no;
    I8 line[80];
    I16 line_cnt;
    I16 text_y;
    register I16 rgt;
    register I16 bottom;
    I16 idx;
    I16 text_size;
    extern char resbuftext[80];


    rgt = x + width;
    bottom = y + height;
    drawrect(x, y, width, height, sprite_id);
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
    textcolor(font_style, 0);
    strcpyfar(resbuftext, caption);
    line_cnt = 1;
    text_size = strlen(resbuftext);
    for (idx = 0; idx < text_size; idx++)
        if (resbuftext[idx] == ']')
            line_cnt++;
    pos = 0;
    line_no = 0;
    text_y = (height - line_cnt * BUTTON_TEXT_LINE_HEIGHT) / 2 + 1;
    for (idx = 0; idx < text_size + 1; idx++) {
        if ((c = resbuftext[idx]) == ']' || c == 0) {
            line[pos] = 0;
            fortextxy(line, x + (width - textpixels(line)) / 2,
                           line_no * BUTTON_TEXT_LINE_HEIGHT + y + text_y);
            line_no++;
            pos = 0;
        } else {
            line[pos] = c;
            pos++;
        }
    }
}
/* Copies the compact 3D shape header into the game shape record. */
void makeobjdef(I8 far *shapeptr, struct SHAPE3D *gameshape)
{
    gameshape->numverts = ((struct SHAPE3DHEADER far *)shapeptr)->header_numverts;
    gameshape->numprimitives = ((struct SHAPE3DHEADER far *)shapeptr)->header_numprimitives;
    gameshape->numpaints = ((struct SHAPE3DHEADER far *)shapeptr)->header_numpaints;
    gameshape->verts = shapeptr + SHAPE3D_RESOURCE_HEADER_BYTES;
    gameshape->cull1 = shapeptr + gameshape->numverts * SHAPE3D_RESOURCE_VERTEX_BYTES + SHAPE3D_RESOURCE_HEADER_BYTES;
    gameshape->cull2 = shapeptr + gameshape->numprimitives * SHAPE3D_RESOURCE_CULL_RECORD_BYTES
                       + gameshape->numverts * SHAPE3D_RESOURCE_VERTEX_BYTES + SHAPE3D_RESOURCE_HEADER_BYTES;
    gameshape->primitives = shapeptr + gameshape->numprimitives * SHAPE3D_RESOURCE_PRIMITIVE_BYTES
                            + gameshape->numverts * SHAPE3D_RESOURCE_VERTEX_BYTES + SHAPE3D_RESOURCE_HEADER_BYTES;
}
/* Loads or select a named font through the far font entry point. */
I16 far mycenter(I8 *name)
{
    return (PLATFORM_SCREEN_WIDTH_PIXELS - textpixels(name)) / 2;
}
/* Commits the active sprite buffer to the video display. */
I16 far sprite_blit_to_video(struct SPRITE far *sprite, I16 mode)
{
    register I16 index;
    register I16 result;
    usescreen();
    mouseoff();
    if (mode == -2) {
        movcf(sprite->sprite_bitmapptr);
        mouseon();
        return 0;
    }
    switch (mode) {
    default:
        for (index = 0; index < 4; ++index) {
            result = hurrykey(elticks());
            if (result != 0) {
                usescreen();
                movcf(sprite->sprite_bitmapptr);
                mouseon();
                return result;
            }
            sprite_blit_dither_phase(sprite->sprite_bitmapptr, index);
        }
    }
    mouseon();
    return 0;
}
/* Displays the waiting dialog and redraw the mouse pointer. */
void far loading(void)
{

    messagebox(0, 0, getresl(main_data_file_addr, "wai"),
                UI_DIALOG_CENTER, waitm_ms, dialog_frame_color, 0, 0);
    mouseoff();
}
/* Formats an integer in the caller buffer with width and zero-fill options. */
void far myitoa(I8 *buffer, I16 value, I16 zero_fill, I16 width)
{
    register I16 length;
    register I16 index;

    itoa(value, buffer, 10);
    if (width != 0) {
        for (length = 0; buffer[length] != 0; ++length) {
/* Find the terminating byte before adjusting the field width. */
        }
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
/* Clears menu animation and idle counters. */
void docursorinit(void)
{
    g_animphase = 0;
    g_hovercolor_idle = 0;
    unused_count = 0;
}
/* Advances the pointer animation and redraw it when its selected state changes. */
I16 far docursor(I16 index, I16 *a, I16 *b, I16 *c, I16 *d,
                               I16 first, I16 second)
{
    register I16 delta;
    register I16 selected;

    delta = elticks();
    g_animphase += delta;
    while (g_animphase > MENU_CURSOR_BLINK_PERIOD)
        g_animphase -= MENU_CURSOR_BLINK_PERIOD;
    if (g_animphase > MENU_CURSOR_BLINK_HALF_PERIOD) selected = first;
    else selected = second;
    if (selected != g_hovercolor_idle) {
        g_hovercolor_idle = selected;
        mouseoff();
        sprite_draw_rect_outline(a[index], c[index], b[index], d[index], selected);
        mouseon();
    }
    return delta;
}
/* Loads song and voice resources, initializes them, and finishes audio setup. */
void file_load_audio_resource(const I8* songfile, const I8* voicefile, const I8* name)
{
	void far* audiores;
	openvfile = myload(RESOURCE_LOAD_VOICE_RETRY, voicefile);
	musicfile = myload(RESOURCE_LOAD_SONG_RETRY, songfile);
	audiores = init_audio_resources(musicfile, openvfile, name);
	load_audio_finalize(audiores);
	is_audioloaded = 1;
}
/* Stops audio playback, releases loaded resources, and clears the loaded flag. */
void far audio_unload(void)
{
    audio_driver_func3F(2);
    freememory(musicfile);
    freememory(openvfile);
    is_audioloaded = 0;
}
/* Applies a font-definition pointer and caches its value. */
void far mysetfont(void far *data)
{
    setfont(data);
    fontdefvalue = ((struct FONTDEF_PREFIX far *)data)->value;
}
/* Applies the currently selected font definition. */
void far fontsetfontdef(void)
{
    mysetfont(def_fntadr);
}
/* Formats a frame count as a minutes, seconds, and optional hundredths string. */
void far maketime(I8 *destination, U16  frame_count, I16 hundredths)
{
    register I16 minute_count;
    I8 buffer[18];
    register I16 seconds;
    I16 minute_frames;

    minute_frames = TIME_SECONDS_PER_MINUTE * rate_frame;
    minute_count = frame_count / minute_frames;
    frame_count -= minute_count * minute_frames;
    seconds = frame_count / rate_frame;
    frame_count -= seconds * rate_frame;
    myitoa(buffer, minute_count, 0, TIME_FIELD_DIGITS);
    strcpy(destination, buffer);
    strcat(destination, ":");
    myitoa(buffer, seconds, 1, TIME_FIELD_DIGITS);
    strcat(destination, buffer);
    if (hundredths != 0) {
        strcat(destination, ".");
        myitoa(buffer, frame_count * (TIME_HUNDREDTHS_PER_SECOND / rate_frame), 1, TIME_FIELD_DIGITS);
        strcat(destination, buffer);
    }
}
/* Combines timer, random, and race-frame values into a nonnegative result. */
I16 drnd(void)
{
    register I16 random_value = (I16)(gettick() + myrandom() + rand() + race_stats.game_frame);
    return random_value < 0 ? -random_value : random_value;
}
/* Dispatches loading by resource type and offers retry after a failed load. */
void far* myload(I16 type, const I8* filename)
{
	void far* result;
	while (1) {
		switch (type) {
			case RESOURCE_LOAD_BINARY_RETRY:
				// try load the file, if it fails, show a dialog, and retry
				result = loadfilez(filename);
				break;

			case RESOURCE_LOAD_BINARY:
				return loadfilez(filename);

			case RESOURCE_LOAD_COMPRESSED:
				// try load a compressed file
				return loadpackz(filename);

			case RESOURCE_LOAD_SHAPE2D_RETRY:
				// try load a 2d shape and retry if it failed
				result = load_shape2d_nofatal_thunk(filename);
				break;

			case RESOURCE_LOAD_SHAPE2D_RESOURCE_RETRY:
				// try load a 2d shape and retry if it failed
				result = load_shape2d_res_nofatal_thunk(filename);
				break;

			case RESOURCE_LOAD_SONG_RETRY:
				// try load a song file and retry if it failed
				result = load_song_file(filename);
				break;

			case RESOURCE_LOAD_VOICE_RETRY:
				// try load a voice file and retry if it failed
				result = load_voice_file(filename);
				break;

			case RESOURCE_LOAD_SFX_RETRY:
				// try load an sfx file and retry if it failed
				result = load_sfx_file(filename);
				break;

			case RESOURCE_LOAD_SHAPE2D_ALT_RETRY:
				// try load a 2d shape and retry if it failed
				result = file_load_shape2d_nofatal2(filename);
				break;
			default:
				break;
		}

/* MSC 5.10: removing this unused label changes the complete object. */
check_result:
        if (result != 0) return result;
        if (do_dea_textres() == 2) return 0;
	}
}
/* Reads the requested resource and offers retry after a read failure. */
void far *read_file_with_retry(I16 type, U16  first, U16  second, U16  third)
{
    void far *result;
    for (;;) {
        switch (type) {
        case RESOURCE_LOAD_READ_RETRY:
            result = loadfileatz(first, second, third);
            break;
        case RESOURCE_LOAD_READ:
            return loadfileatz(first, second, third);
        }
        if (result != 0) return result;
        if (do_dea_textres() == 2) return 0;
    }
}

I8S  input_status_stack_depth = 0;
/* Pushes the current keyboard and mouse modes onto the input stack. */
void far pushmouse(void)
{
    copy_mouse_modes[input_status_stack_depth] = mouseonstat;
    input_device_modestack[input_status_stack_depth] = kbormouse;
    ++input_status_stack_depth;
}
/* Restores the most recently saved keyboard and mouse modes. */
void far pullmouse(void)
{
    if (input_status_stack_depth != 0) {
        --input_status_stack_depth;
        mouseonstat = copy_mouse_modes[input_status_stack_depth];
        kbormouse = input_device_modestack[input_status_stack_depth];
        if (kbormouse == 0)
            mouseoff();
    }
}
/* Shows joystick help, processes selection, and restores audio and input state. */
void far do_joystick_resource_text(void)
{
    extern int input_pushed;
    extern char joystick_enabled;
    extern char mousedrive;
    extern int dlg_colour;

    I16 ys[9];
    I16 ht;
    I16 xs[9];
    I16 coords[14];
    I16 cw;
    register I16 lastpos;
    register I16 cur;
    I16 idx;
    I8 hit[9];

    pushmouse();
    input_pushed = 1;
    audio_pause_playback();
    if (messagebox(3, 1, getresl(main_data_file_addr, "joy"),
                    -1, -1, dialog_frame_color, coords, 0) > 0) {
        for (idx = 0; idx < 9; ++idx)
            hit[idx] = 0;
        joystick_enabled = 1;
        mouseoff();
        drawrect(coords[2] - 4, coords[3], 1, coords[13] - coords[3] - 8, dialog_frame_color);
        drawrect(coords[4] - 4, coords[5], 1, coords[13] - coords[3] - 8, dialog_frame_color);
        drawrect(coords[0], coords[9] - 4, coords[6] - coords[0], 1, dialog_frame_color);
        drawrect(coords[0], coords[11] - 4, coords[6] - coords[0], 1, dialog_frame_color);
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
            if (normkey() != 0)
                break;
            cur = get_joy_flags();
            if (cur & 0x30)
                break;
            cur = convertjoy(cur);
            if (cur == lastpos)
                continue;
            for (idx = 0; idx < 9; ++idx)
                drawrect(xs[idx], ys[idx], cw, ht, font_secondary_color);
            drawrect(xs[cur], ys[cur], cw, ht, dlg_colour);
            lastpos = cur;
            hit[cur] = 1;
        }
        for (idx = 0; idx < 9; ++idx)
            joystick_enabled &= hit[idx];
        restoreboxwindow();
        if (joystick_enabled == 0)
            messagebox(1, 1, getresl(main_data_file_addr, "jox"),
                        -1, -1, dialog_frame_color, 0, 0);
    } else {
        joystick_enabled = 0;
    }
    purgekey();
    mousedrive = 0;
    restore_audio_volume();
    input_pushed = 0;
    pullmouse();
}
/* Shows keyboard help and restores audio and input state. */
void far mkeyon(void)
{
    extern int input_pushed;
    extern char joystick_enabled;
    extern char mousedrive;


    pushmouse();
    input_pushed = 1;
    audio_pause_playback();
    messagebox(4, 1, getresl(main_data_file_addr, "key"),
                UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
    joystick_enabled = 0;
    mousedrive = 0;
    input_pushed = 0;
    restore_audio_volume();
    pullmouse();
}
/* Shows mouse help and restores audio and input state. */

void far mmouseon(void)
{
    extern int input_pushed;
    extern char mousedrive;


    pushmouse();
    input_pushed = 1;
    audio_pause_playback();
    mousedrive = 1;
    messagebox(4, 1, getresl(main_data_file_addr, "mou"),
                UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
    input_pushed = 0;
    restore_audio_volume();
    pullmouse();
}
/* Shows pause help and restores audio and input state. */

void far msystempause(void)
{
    extern int input_pushed;


    pushmouse();
    input_pushed = 1;
    audio_pause_playback();
    messagebox(1, 1, getresl(main_data_file_addr, "pau"),
                UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
    input_pushed = 0;
    restore_audio_volume();
    pullmouse();
}
/* Toggles music and effects options from their help page. */

void far mmusictog(void)
{
    extern int input_pushed;

    pushmouse();
    input_pushed = 1;
    if (audio_toggle_flag2())
        messagebox(4, 1, getresl(main_data_file_addr, "mon"),
                    UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
    else
        messagebox(4, 1, getresl(main_data_file_addr, "mof"),
                    UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
    input_pushed = 0;
    pullmouse();
}
/* Toggles sound options from their help page. */

void far msoundtog(void)
{
    extern int input_pushed;

    pushmouse();
    input_pushed = 1;
    if (audio_toggle_flag6())
        messagebox(4, 1, getresl(main_data_file_addr, "son"),
                    UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
    else
        messagebox(4, 1, getresl(main_data_file_addr, "sof"),
                    UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
    input_pushed = 0;
    pullmouse();
}
/* Shows DOS options and runs the exit-list action when selected. */

void far mdosexit(void)
{
    extern int input_pushed;


    pushmouse();
    input_pushed = 1;
    audio_pause_playback();
    if (messagebox(2, 1, getresl(main_data_file_addr, "dos"),
                    UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0) == 1)
        dsiexit();
    input_pushed = 0;
    restore_audio_volume();
    pullmouse();
}
/* Shows graphics-level help and restores audio and input state. */

void far msetrender(void)
{
    I8 menu[512];
    U8  active[9];
    I8 rc;
    I16 saved_rate;
    register I16 selection_index;
    register I16 cursor;
    extern int input_pushed;
    extern int performGraphColor;

    pushmouse();
    input_pushed = 1;
    audio_pause_playback();
    saved_rate = frm_rate2;
    rc = 0;
    for (;;) {
        strcpyfar(menu, getresl(main_data_file_addr, "mrl"));
        for (selection_index = 0; selection_index < 9; ++selection_index) active[selection_index] = 0;
        active[renderlevel] = 1;
        active[slow_video_mode_state + 5] = 1;
        if (frm_rate2 == 10) active[7] = 1;
        else active[8] = 1;
        cursor = 0;
        for (selection_index = 0; selection_index < 9; ++selection_index) {
            while (menu[cursor] != '[') ++cursor;
            if (active[selection_index]) menu[cursor + 1] = '*';
            ++cursor;
        }
        rc = messagebox(2, 1, menu,
                         UI_DIALOG_CENTER, UI_DIALOG_CENTER, performGraphColor, 0, rc);
        switch (rc) {
        case -1: break;
        case 5: slow_video_mode_state = 0; continue;
        case 6: slow_video_mode_state = 1; continue;
        case 7: frm_rate2 = 10; continue;
        case 8: frm_rate2 = 20; continue;
        case 9: break;
        default: renderlevel = rc; continue;
        }
        break;
    }
/* MSC 5.10: removing this unused label changes the complete object. */
after_menu:
    if (saved_rate != frm_rate2)
        messagebox(1, 1, getresl(main_data_file_addr, "mrs"),
                    UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
    input_pushed = 0;
    restore_audio_volume();
    pullmouse();
}
/* Shows the general help dialog while preserving nested input state. */
I16S far do_dea_textres(void)
{
    I16S result;
    pushmouse();
    if (g_is_busy != 0) {
        result = messagebox(2, 1, getresl(main_data_file_addr, "dea"),
                             UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
        if (result != 0) result = 0;
        else result = 1;
    } else {
        messagebox(1, 1, getresl(main_data_file_addr, "der"),
                    UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
        result = 1;
    }
    pullmouse();
    return result;
}
/* Finds the requested file and asks for another path when it is absent. */


void far ensure_file_exists(I16 index)
{

    while (findfirst(findfilenames[index - 1]) == 0) {
        messagebox(1, 1, getresl(main_data_file_addr, findfiletexts[index - 1]),
                    UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
        mouseoff();
        kbormouse = 0;
    }
}
/* Shows the multiplayer help dialog. */
void far memerr(void)
{
    messagebox(1, 1, getresl(main_data_file_addr, "mer"),
                UI_DIALOG_CENTER, UI_DIALOG_CENTER, dialog_frame_color, 0, 0);
}
/* Returns the timer delta through the far entry point. */

I16  elticks(void)
{
    return elapsedticks();
}
/* Loads a 3D resource file and retries after a failed load. */

void far *fluffyload(I8 *filename)
{
    void far *result;
    while (1) {
        I8 name[0x50];
        strcpy(name, filename);
        strcat(name, ".p3s");
        result = myload(7, name);
        if (result == 0) {
            strcpy(name, filename);
            strcat(name, ".3sh");
            result = myload(1, name);
            if (result == 0) {
                do_dea_textres();
                continue;
            }
        }
        return result;
    }
}

