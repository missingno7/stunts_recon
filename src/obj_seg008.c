#include "platform_hw.h"
/* Explicit register declarations below are required by the matching MSC 5.10 build. */
#define DIALOG_TEXT_WIDTH_ALIGNMENT_MASK 0xfff8
#define SHAPE3D_RESOURCE_HEADER_BYTES 4
#define SHAPE3D_RESOURCE_VERTEX_BYTES 6
#define SHAPE3D_RESOURCE_CULL_RECORD_BYTES 4
#define SHAPE3D_RESOURCE_PRIMITIVE_BYTES 8
#define MOUSE_TRACK_FIXED_POINT_SHIFT 2
#include "stunts_types.h"
/* MSC 5.10 <ctype.h> macros over the pinned runtime table _ctype */
#define _UPPER 0x1
#define _LOWER 0x2
#define isupper(c) ((_ctype+1)[c] & _UPPER)
#define islower(c) ((_ctype+1)[c] & _LOWER)
#define _tolower(c) ((c)-'A'+'a')
/* PORT: MSC ctype lookup assumes an 8-bit character code; negative plain-char values can index before _ctype+1 on a host. */
#define tolower(c) (isupper(c) ? _tolower(c) : (c))
static I16 textboundsleft, text_bounds_right, text_bounds_upper, txt_bounds_bottom;
static I16 textoutlineleft, text_outline_right, txt_outline_top, text_outline_bottom;



/* Local view: strlen; return is unsigned word; another unit uses signed word. */
extern unsigned strlen(I8 *s);


/* Local view: freememory; return is far pointer; another unit uses void. */
extern void far* freememory(I8 far* ptr);
/* Local view: locate_shape_fatal; return.pointer is signed byte; another unit uses struct SHAPE2D. */
extern I8 far *locate_shape_fatal(I8 far *data, I8 *name);
char textrespfxchr;
int msecoordx;
int pos_y_ms;

/* PORT: These game records rely on 16-bit pointers and MSC default 2-byte field alignment. */
/* Local view: SHAPE2D; struct.count is 2; another unit uses 6. */
struct SHAPE2D { U16S  words[6]; U8  bytes[4]; };
/* PORT: This renderer record stores a far pointer; pointer width and default packing are part of the target ABI. */
/* Local view: SPRITE; struct.count is 4; another unit uses 2. */
struct SPRITE { struct SHAPE2D far *sprite_bitmapptr; U16S  words[3]; U16  *lineofs; U16S  words2[9]; };


/* Local view: mouse_unk_sprite_ptr; pointer is struct SPRITE; another unit uses void. */
struct SPRITE far *mouse_unk_sprite_ptr;

/* Local view: spritepointermini; pointer is struct SPRITE; another unit uses void. */
extern struct SPRITE far *spritepointermini;
/* Local view: sprite_setup1_from_arg_pointer; parameters[0].pointer is struct SPRITE; another unit uses void. */
void sprite_setup1_from_arg_pointer(struct SPRITE far *argsprite);
/* Local view: sprite_clear_1_color; parameters[0] is unsigned byte; another unit uses signed word. */
void sprite_clear_1_color(U8  color);



/* Local view: sprputimage; parameters[0].pointer is struct SHAPE2D; another unit uses void. */
void far sprputimage(struct SHAPE2D far *shape);
/* Local view: sprite_clear_shape_alt; parameters[0].pointer is struct SHAPE2D; another unit uses void. */
void far sprite_clear_shape_alt(struct SHAPE2D far *shape, I16 x, I16 y);
/* Local view: andcfxya; parameters[0].pointer is struct SHAPE2D; another unit uses void. */
void far andcfxya(struct SHAPE2D far *shape, U16S  x, U16S  y);
/* Local view: orcfxya; parameters[1] is unsigned word; another unit uses signed word. */
void far orcfxya(struct SHAPE2D far *shape, U16S  x, U16S  y);

/* PORT: Resource headers are read directly; preserve byte fields and 2-byte record alignment. */
/* Local view: SHAPE3DHEADER; struct[0].name is numverts; another unit uses header_numverts. */
struct SHAPE3DHEADER { U8  numverts, numprimitives, numpaints, reserved; };
/* PORT: This resource view contains several far pointers and uses the MSC medium-model layout. */
/* Local view: SHAPE3D; struct.count is 8; another unit uses 7. */
struct SHAPE3D { U16S  numverts; I8 far *verts; U16S  numprimitives; U8  numpaints, reserved; I8 far *primitives; I8 far *cull1; I8 far *cull2; };

int g_animphase;
int g_hovercolor_idle;
/* Local view: unused_count; type is unsigned word; another unit uses signed word. */
extern U16  unused_count;
/* Local view: rate_frame; type is signed word; another unit uses unsigned word. */
extern I16 rate_frame;
#include "fontrec.h"

unsigned int fontdefvalue;



signed char copy_mouse_modes[8];
signed char input_device_modestack[8];
/* Local view: elapsedticks; return is unsigned long; another unit uses signed word. */
extern U32  elapsedticks(void);
/* Local view: timer_get_delta_alt; return is signed word; another unit uses unsigned long. */
extern I16 far timer_get_delta_alt(void);



/* PORT: This compact snapshot is an MSC 16-bit record; host widths and alignment must not alter its byte layout. */
#include "gamerec.h"

/* Local view: mouse_ptr_cursor; pointer is struct SPRITE; another unit uses void. */
extern struct SPRITE far *mouse_ptr_cursor;
#include "audapi.h"
#include "auddrv.h"
#include "dialog.h"
#include "game.h"
#include "gfxapi.h"
#include "input.h"
#include "prim3d.h"
#include "resapi.h"
#include "shape2d.h"
#include "text.h"
#include "timer.h"





/* Local view: kbjoyflags; type is signed word; another unit uses signed byte. */
int kbjoyflags;
/* Local view: flagsdown; type is signed word; another unit uses unsigned byte. */
int flagsdown;
/* Local view: g_vid_flg2_set; type is signed word; another unit uses unsigned word. */
extern I16 g_vid_flg2_set;

/* Local view: g_is_busy; type is signed byte; another unit uses unsigned byte. */
extern I8 g_is_busy;
/* Local view: dialog_frame_color; type is unsigned word; another unit uses signed word. */
extern U16S  dialog_frame_color;
/* Local view: main_data_file_addr; pointer is void; another unit uses signed byte. */
void far *main_data_file_addr;



/* Local view: getmousepos; parameters[0].pointer is signed word; another unit uses unsigned word. */
extern void far getmousepos(I16 *buttons, I16 *x, I16 *y);











/* Local view: file_load_resource_file; return.pointer is void; another unit uses signed byte. */
extern void far *file_load_resource_file(I8 *filename);

/* Local view: fluffyload; return.pointer is void; another unit uses signed byte. */
extern void far *fluffyload(I8 *filename);













short voicefile_gap_c[4];
void far *openvfile;
void far *musicfile;
/* Local view: is_audioloaded; type is signed byte; another unit uses signed word. */
extern I8 is_audioloaded;
/* Local view: locate_text_resource; parameters[0].pointer is signed byte; another unit uses void. */
extern I8 far * far locate_text_resource(I8 far *data, I8 *name);





I8 *findfiletexts[4] = { "id1", "id2", "id3", "id4" };
I8 *findfilenames[4] = { "setup.exe", "sdtitl.*", "tedit.*", "opp1.*" };
U16  font_secondary_color = 0;

/* target file_build_path @ 4370; candidate from build\workers\tuseg008\file_build_path_ch.c */
/* merged owner s008-a member point_in_rectangle */
I8 far point_in_rectangle(I16 x1, I16 x2, I16 y1, I16 y2)
{ /* PURPOSE: Save the pixels under a rectangular sprite region when the backing store has room. Params: x1, x2, y1, y2. Returns: char. Globals: reads g_mousesave_x_tbl, g_mouseyposstacktable, mssprite_arrays; writes g_mousesave_x_tbl, g_mouseyposstacktable, mssprite_arrays. */ /* PLATFORM(memory): allocates or releases game-managed memory. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    
    
    /* Local view: mouse_buffer_count; type is unsigned byte; another unit uses signed byte. */
    extern U8  mouse_buffer_count;
    #include "mousebuf.h"
    /* Local view: savedptr_ms; pointer is struct SPRITE; another unit uses signed byte. */
    extern struct SPRITE far *savedptr_ms;
    /* Local view: availmem; return is signed long; another unit uses unsigned long. */
    extern I32 far availmem(void);
    /* Local view: sprite_make_window; return.pointer is struct SPRITE; another unit uses void. */
    extern struct SPRITE far *far sprite_make_window(I16 width, I16 height, I16 flags);
    
    
    /* Local view: sprite_clear_shape_alt; parameters[0].pointer is struct SHAPE2D; another unit uses void. */
    extern void far sprite_clear_shape_alt(struct SHAPE2D far *shape, I16 x, I16 y);
    struct SPRITE saved_sprites[2];
    I32 required;
    required = ((I32)(x2 - x1) * (y2 - y1)) / (I32)(pixel_scales * vidflg4_is1) + 18L;
    if (availmem() /* PLATFORM(memory): query available scaled resource memory. */ <= required) return 0;

    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    mssprite_arrays[mouse_buffer_count] = sprite_make_window(x2 - x1, y2 - y1, 15) /* PLATFORM(video): allocate a temporary sprite window. */;
    g_mousesave_x_tbl[mouse_buffer_count] = x1;
    g_mouseyposstacktable[mouse_buffer_count] = y1;
    savewindow(&saved_sprites[0]) /* PLATFORM(video): save both active sprite buffers. */;
    savedptr_ms[mouse_buffer_count * 2] = saved_sprites[0];
    savedptr_ms[mouse_buffer_count * 2 + 1] = saved_sprites[1];
    displaywindow() /* PLATFORM(video): copy sprite buffer 2 into buffer 1. */;
    sprite_clear_shape_alt(mssprite_arrays[mouse_buffer_count]->sprite_bitmapptr,
                           x1, y1) /* PLATFORM(video): clear the requested shape in sprite buffer 1. */;
    ++mouse_buffer_count;
    return 1;
}
/* Local view: g_mouseyposstacktable; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
int g_mouseyposstacktable[5];
/* Local view: g_mousesave_x_tbl; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
int g_mousesave_x_tbl[4];
/* Local view: mssprite_arrays; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
struct SPRITE far *mssprite_arrays[4];

/* merged owner s008-a member restore_mouse_sprite */
void far restoreboxwindow(void)
{ /* PURPOSE: Restore saved pixels and release the temporary sprite region. Params: none. Returns: void. Globals: reads g_mousesave_x_tbl, g_mouseyposstacktable, mssprite_arrays; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    /* Local view: mouse_buffer_count; type is unsigned byte; another unit uses signed byte. */
    extern U8  mouse_buffer_count;
    /* Local view: g_mousesave_x_tbl; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
    extern I16 g_mousesave_x_tbl[];
    /* Local view: g_mouseyposstacktable; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
    extern I16 g_mouseyposstacktable[];
    /* Local view: mssprite_arrays; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
    extern struct SPRITE far *mssprite_arrays[];
    /* Local view: savedptr_ms; pointer is struct SPRITE; another unit uses signed byte. */
    extern struct SPRITE far *savedptr_ms;
    /* Local view: movfxya; parameters[0].pointer is struct SHAPE2D; another unit uses void. */
    extern void far movfxya(struct SHAPE2D far *shape, I16 x, I16 y);
    
    /* Local view: sprite_free_window; parameters[0].pointer is void; another unit uses struct SPRITE. */
    extern void far sprite_free_window(void far *window);
    struct SPRITE saved_sprites[2];

    if (mouse_buffer_count == 0) return;
    --mouse_buffer_count;
    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    movfxya(mssprite_arrays[mouse_buffer_count]->sprite_bitmapptr,
                      g_mousesave_x_tbl[mouse_buffer_count],
                      g_mouseyposstacktable[mouse_buffer_count]) /* PLATFORM(video): copy a shape into sprite buffer 1. */;
    saved_sprites[0] = savedptr_ms[mouse_buffer_count * 2];
    saved_sprites[1] = savedptr_ms[mouse_buffer_count * 2 + 1];
    restorewindow(&saved_sprites[0]) /* PLATFORM(video): restore both active sprite buffers. */;
    sprite_free_window(mssprite_arrays[mouse_buffer_count]) /* PLATFORM(video): release a temporary sprite window. */;
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
}

/* merged owner s008-dlg member show_dialog */
/* Local view: messagebox; parameters is ['i16', 'i16', {'pointer': 'i8', 'qualifiers': [], 'segment': 'far'}, 'i16', 'i16', 'u16', {'pointer': 'i16', 'qualifiers': [], 'segment': 'near'}, 'i8']; another unit uses None. */
I16 far messagebox(I16 type, I16 check, I8 far *message, I16 x, I16 y,
                    U16  frame_arg, I16 *disabled, I8 initial)
{ /* PURPOSE: Lay out and draw a dialog, then process input until a choice is made. Params: type, check, message, x, y, frame_arg, disabled, initial. Returns: int. Globals: reads font_secondary_color, fontdefvalue; writes font_secondary_color. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(input_mouse): polls or updates mouse state. */
    /* Local view: dlg_colour; type is signed word; another unit uses unsigned word. */
    extern I16 dlg_colour;  /* Local view: performGraphColor; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
extern I16 performGraphColor;
    /* Local view: _ctype; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
    extern U8  _ctype[];
    /* Local view: sprset1size; return is signed word; another unit uses void. */
    extern I16 far sprset1size(I16, I16, I16, I16);
    /* Local view: sprite_draw_rect_outline; parameters[4] is unsigned word; another unit uses signed word. */
    extern void far sprite_draw_rect_outline(I16, I16, I16, I16, U16 );
    /* Local view: font_setup_unknown; parameters[0] is unsigned word; another unit uses signed word. */
    extern void far font_setup_unknown(U16 , U16 );
    
    
    /* Local view: mousecursor; return is signed word; another unit uses signed byte. */
    extern I16 far mousecursor(I16, I16 *, I16 *, I16 *, I16 *);
    /* Local view: timer_get_delta_alt; return is signed word; another unit uses unsigned long. */
    extern I16 far timer_get_delta_alt(void);
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
    I16 btn_ts[20];
    I16 i;
    I16 wide;
    I8 labelstr[80];
    U16  key;
    I8 count;
    I8 far *btn_text[20];
    I16 btn_bs[20];
    I16 btn_rs[20];
    I8 markers;
    I8 far *line_start;
    I8 lengths[20];
    I8 busy;
    I16 pos;
    I8 textbuf[80];
    I16 dlgframe[4];
    I16 btn_ls[20];

    linehgt = fontdefvalue + 2;
    total_h = 0;
    wide = 32;
    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    textptr = message;
    pos = 0;
    while ((chr = *textptr) != 0) {
        if (chr == ']') {
            textbuf[pos] = 0;
            text_w = textpixels(textbuf) /* PLATFORM(video): load or select a named font. */;
            if (text_w > wide)
                wide = text_w;
            pos = 0;
            total_h += linehgt;
        } else if (*textptr == '}') {
            textbuf[pos] = 0;
            text_w = textpixels(textbuf) /* PLATFORM(video): load or select a named font. */;
            if (text_w > wide)
                wide = text_w;
            pos = 0;
            total_h += 4;
        } else {
            textbuf[pos++] = *textptr;
        }
        ++textptr;
    }
    wide = (wide + 24) & DIALOG_TEXT_WIDTH_ALIGNMENT_MASK;
    if (x == -1)
        x = ((PLATFORM_SCREEN_WIDTH_PIXELS - wide) / 2) & DIALOG_TEXT_WIDTH_ALIGNMENT_MASK;
    if (y == -1)
        y = (PLATFORM_SCREEN_HEIGHT_PIXELS - total_h) / 2;
    dlgframe[0] = x;
    dlgframe[1] = x + wide;
    dlgframe[2] = y - 8;
    dlgframe[3] = y + total_h + 8;
    x += 8;
    wide -= 16;
    if (check != 0 && !point_in_rectangle(dlgframe[0], dlgframe[1], dlgframe[2], dlgframe[3]))
        return -1;
    displaywindow() /* PLATFORM(video): copy sprite buffer 2 into buffer 1. */;
    sprset1size(dlgframe[0], dlgframe[1], dlgframe[2], dlgframe[3]) /* PLATFORM(video): set the active sprite buffer bounds. */;
    sprite_clear_1_color(0) /* PLATFORM(video): clear the active sprite buffer with the requested color. */;
    sprite_draw_rect_outline(x - 4, y - 4, x + wide + 4, y + total_h + 4, frame_arg) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
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
            movtxy(textbuf, x, y + total_h) /* PLATFORM(video): draw a text string at screen coordinates. */;
            pos = 0;
            total_h += linehgt;
            line_start = textptr;
        } else if (*textptr == '}') {
            textbuf[pos] = 0;
            movtxy(textbuf, x, y + total_h) /* PLATFORM(video): draw a text string at screen coordinates. */;
            pos = 0;
            total_h += 4;
            line_start = textptr;
        } else if (*textptr == '@') {
            if (type == 3) {
                textbuf[pos] = 0;
                disabled[markers] = textpixels(textbuf) /* PLATFORM(video): load or select a named font. */ + x;
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
        btn_ls[count] = textpixels(textbuf) /* PLATFORM(video): load or select a named font. */ + x;
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
                text_w = textpixels(textbuf) /* PLATFORM(video): load or select a named font. */;
                pos = 0;
                total_h += linehgt;
            } else if (*textptr == '}') {
                textbuf[pos] = 0;
                text_w = textpixels(textbuf) /* PLATFORM(video): load or select a named font. */;
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
            text_w = textpixels(textbuf) /* PLATFORM(video): load or select a named font. */;
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
        waitbios(8L) /* PLATFORM(timer): wait for the requested delay while servicing input. */;
        break;
    case 1:
        do {
            key = input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */;
        } while (key == 0);
        if (key == 27)
            ret = 0;
        clear_keyboardbuffer();
        break;
    case 0:
        return 0;
    case 3:
        return markers / 2;
    case 2:
        ret = initial;
        oldchoice = -1;
        timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */;
        msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
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
                    movtxy(labelstr, btn_ls[i], btn_ts[i]) /* PLATFORM(video): draw a text string at screen coordinates. */;
                }
                msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
                if (oldchoice == -1)
                    clear_keyboardbuffer();
                oldchoice = ret;
            }
            key = input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */;
            choice = mousecursor(count, btn_ls, btn_rs, btn_ts, btn_bs) /* PLATFORM(input_mouse): test the mouse against candidate rectangles. */;
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

/* merged owner s008-dlg member do_fileselect_dialog */
/* Local view: do_fileselect_dialog; return is signed word; another unit uses signed byte. */
I16 far do_fileselect_dialog(I8 *path, I8 *selected_name, I16 attributes,
                             I8 far *heading)
{ /* PURPOSE: Build and display the file list, returning the selected file name. Params: path, selected_name, attributes, heading. Returns: int. Globals: reads dialogarg2, flagsdown, font_secondary_color, g_is_busy, main_data_file_addr; writes g_is_busy. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(input_mouse): polls or updates mouse state. */
    /* Local view: resbuftext; type is array[None]; another unit uses signed byte. */
    extern I8 resbuftext[];
    /* Local view: dlg_colour; type is signed word; another unit uses unsigned word. */
    extern I16 dlg_colour;  /* Local view: performGraphColor; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
extern I16 performGraphColor;
    /* Local view: _ctype; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
    extern U8  _ctype[];
    /* Local view: file_combine_and_find; parameters[2] is signed word; another unit uses near pointer. */
    extern I8 *file_combine_and_find(I8 *, I8 *, I16);
    
    /* Local view: call_read_line; return is signed word; another unit uses void. */
    extern I16 far call_read_line(I8 *, I16, I16, I16, I32);
    
    /* Local view: drawrect; return is signed word; another unit uses void. */
    extern I16 far drawrect(I16, I16, I16, I16, I16);
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

    if (messagebox(3, 1, locate_text_resource(main_data_file_addr, "loa") /* PLATFORM(file): locate a named text entry in resource data. */,
                    -1, -1, dialog_frame_color, layout, 0) /* PLATFORM(video): present the interactive dialog renderer. */ < 0)
        return 0;

    old_busy = g_is_busy;
    g_is_busy = 1;
    preRender_line(layout[4] - 4, layout[5] + 4, layout[4] + 0xab,
                   layout[5] + 4, dialog_frame_color) /* PLATFORM(video): draw a line primitive. */;
    font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
    strcpyfar(resbuftext, heading);
    movtxy(resbuftext, layout[0], layout[1]) /* PLATFORM(video): draw a text string at screen coordinates. */;
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
    movtxy(path, x, layout[3]) /* PLATFORM(video): draw a text string at screen coordinates. */;

rescan:
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    files_found = 0;
    found = file_combine_and_find(path, "*", attributes) /* PLATFORM(file): search for the combined directory/file path. */;
    if (found == 0) {
        nullsub_1();
edit_path:
        font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
        if (call_read_line(path, 0x12, x, layout[3], 30000L) != KEY_ASCII_ESCAPE)
            goto rescan;
cancel:
        rc = 0;
        goto finish;
    }
    namecopy(names[0], found);
    ++files_found;
    while ((found = file_find_next_alt() /* PLATFORM(file): advance to the next file-list entry. */) != 0) {
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
        strcpyfar(resbuftext, locate_text_resource(main_data_file_addr, "lsu") /* PLATFORM(file): locate a named text entry in resource data. */);
        movtxy(resbuftext, mycenter(resbuftext) /* PLATFORM(video): load or select a named font through the alternate entry point. */, hit_t[1]) /* PLATFORM(video): draw a text string at screen coordinates. */;
        strcpyfar(resbuftext, locate_text_resource(main_data_file_addr, "lsd") /* PLATFORM(file): locate a named text entry in resource data. */);
        movtxy(resbuftext, mycenter(resbuftext) /* PLATFORM(video): load or select a named font through the alternate entry point. */, hit_t[9] - 1) /* PLATFORM(video): draw a text string at screen coordinates. */;
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
                    movtxy(resbuftext, x, hit_t[idx + 2]) /* PLATFORM(video): draw a text string at screen coordinates. */;
                } else
                    movtxy("        ", x, hit_t[idx + 2]) /* PLATFORM(video): draw a text string at screen coordinates. */;
                label_width = textpixels(resbuftext) /* PLATFORM(video): load or select a named font. */;
                drawrect(label_width + x, hit_t[idx + 2], field_end - label_width - x, 8, font_secondary_color) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
            }
            msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
        }
        pressed = input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */;
        button = mousecursor(10, hit_l, hit_r, hit_t, hit_b) /* PLATFORM(input_mouse): test the mouse against candidate rectangles. */;
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

void makename(I8 *dir, I8 *name, I8 *ext, I8 *dst)
{ /* PURPOSE: Join a directory, file name, and extension in the destination buffer. Params: dir, name, ext, dst. Returns: void. Globals: none. */
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

/* target parse_filepath_separators @ 4786; accepted source src/parse_filepath_separators.c */
/* merged owner s008-dlg member do_savefile_dialog */
/* Local view: fsavereq; return is signed word; another unit uses signed byte. */
I16 far fsavereq(I8 *name, I8 *directory, I8 far *title)
{ /* PURPOSE: Display the save dialog and return the selected file name. Params: name, directory, title. Returns: int. Globals: reads dialogarg2, font_secondary_color, main_data_file_addr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    /* Local view: resbuftext; type is signed byte; another unit uses array[80]. */
    extern I8 resbuftext;
    /* Local view: locate_text_resource; parameters[0].pointer is signed byte; another unit uses void. */
    extern I8 far * far locate_text_resource(I8 far *, I8 *);
    
    /* Local view: dlg_colour; type is unsigned word; another unit uses signed word. */
extern unsigned int dlg_colour;

    /* Local view: call_read_line; return is signed word; another unit uses void. */
    extern I16 far call_read_line(I8 *, I16, I16, I16, I32);
    register I16 key;
    I8 accepted;
    register I16 i;
    I16 layout[6];

    if (messagebox(3, 1, locate_text_resource(main_data_file_addr, "sav") /* PLATFORM(file): locate a named text entry in resource data. */,
                    -1, -1, dialog_frame_color, layout, 0) /* PLATFORM(video): present the interactive dialog renderer. */ < 0)
        return 0;

    accepted = 0;
    font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
    strcpyfar(&resbuftext, title);
    movtxy(&resbuftext, layout[0], layout[1]) /* PLATFORM(video): draw a text string at screen coordinates. */;
    font_setup_unknown(dlg_colour, font_secondary_color) /* PLATFORM(video): select the font and color mode. */;
    movtxy(name, layout[2], layout[3]) /* PLATFORM(video): draw a text string at screen coordinates. */;
    movtxy(directory, layout[4], layout[5]) /* PLATFORM(video): draw a text string at screen coordinates. */;
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    goto read_directory;
    while (key != KEY_ASCII_ENTER) {
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
    restoreboxwindow();
    return accepted;
}

void namecopy(I8 *dest, I8 *path)
{ /* PURPOSE: Normalize path separators and report whether the path was accepted. Params: dest, path. Returns: void. Globals: none. */
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

/* target input_checking @ 4884; candidate from build\workers\tuseg008\input_checking.c */
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

I16 input_checking(I16 delta)
{ /* PURPOSE: Poll the active input devices and update per-frame input state. Params: delta. Returns: int. Globals: reads current keyboard, mouse and joystick state; writes latched device flags, mouse coordinates and per-frame input counters. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(input_joy): polls or updates joystick state. */ /* PLATFORM(input_mouse): polls or updates mouse state. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    register I16 key;
    register I16 joy;

    input_framecount += delta;
    if (input_framecount > 20000) {
        input_framecount -= 10000;
        input_framecount2 -= 10000;
        input_framecount3 -= 10000;
    }

    key = normkeyi() /* PLATFORM(input_kb): read the next keyboard character. */;
    if (key != 0) kbormouse = 0;
    joy = get_joy_flags() /* PLATFORM(input_joy): read joystick button/direction state. */;
    kbjoyflags = joystate() /* PLATFORM(input_kb): read the configured keyboard-or-joystick state. */;

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
    getmousepos(&flagsdown, &msecoordx, &pos_y_ms) /* PLATFORM(input_mouse): read mouse buttons and coordinates. */;
    if (mouse_oldx != msecoordx || mouse_oldy != pos_y_ms ||
        mouse_oldbut != flagsdown) {
        mouse_oldx = msecoordx;
        mouse_oldy = pos_y_ms;
        kbormouse = 1;
        input_framecounter = 0;
        if (mouse_transparent_mode != 0) {
            if (mouseonscreen != 0) dohidemouse() /* PLATFORM(video): draw the pointer using its opaque sprite path. */;
            doshowmouse() /* PLATFORM(video): draw the pointer using its transparent sprite path. */;
        }
    } else if (kbormouse != 0) {
        input_framecounter += delta;
        if (input_framecounter > 500) {
            input_framecounter = 0;
            kbormouse = 0;
            if (mouseonscreen != 0) dohidemouse() /* PLATFORM(video): draw the pointer using its opaque sprite path. */;
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

/* target input_do_checking @ 5426; candidate from build\workers\inputtu\repeat_types_ulong.c */
I16 far input_do_checking(I16 value) { /* PURPOSE: Forward the per-frame input poll through the far entry point. Params: value. Returns: int. Globals: none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ return input_checking(value) /* PLATFORM(input_kb): poll the configured input devices. */; }

/* target file_load_resfile @ 5442; candidate from build\workers\tuseg008\file_load_resfile_scope.c */
/* Local view: file_load_resource_file; return.pointer is void; another unit uses signed byte. */
void far *file_load_resource_file(I8 *filename)
{ /* PURPOSE: Load a resource file and release it when the requested text resource is absent. Params: filename. Returns: void *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
    void far *result;
    while (1) {
        I8 name[0x50];
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
/* Local view: closeresource; parameters[0].pointer is void; another unit uses signed byte. */
void far closeresource(void far* resptr) { /* PURPOSE: Release a loaded resource through the game memory manager. Params: resptr. Returns: void. Globals: none. */ /* PLATFORM(memory): allocates or releases game-managed memory. */
    freememory(resptr) /* PLATFORM(memory): release the resource through game memory management. */;
}

/* target locate_shape_alt @ 5596; accepted source src/locate_shape_alt.c */
/* Local view: locate_shape_alt; return.pointer is signed byte; another unit uses void. */
I8 far *locate_shape_alt(I8 far *data, I8 *name)
{ /* PURPOSE: Find a named shape in a resource and return its far pointer. Params: data, name. Returns: char *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
    return locate_shape_fatal(data, name) /* PLATFORM(file): locate a named shape in resource data. */;
}

/* target locate_text_res @ 5618; accepted source src/locate_text_res.c */
/* Local view: locate_text_resource; parameters[0].pointer is signed byte; another unit uses void. */
I8 far * far locate_text_resource(I8 far *data, I8 *name)
{ /* PURPOSE: Find a named text entry and record its leading character. Params: data, name. Returns: char *. Globals: reads textrespfxchr; writes none. */ /* PLATFORM(file): uses file and resource services. */
    I8 textname[4];
    textname[0] = textrespfxchr;
    textname[1] = name[0];
    textname[2] = name[1];
    textname[3] = name[2];
    return locate_shape_fatal(data, textname) /* PLATFORM(file): locate a named shape in resource data. */;
}

/* target copy_string @ 5670; accepted source src/copy_string.c */
void strcpyfar(I8 *destination, I8 far *source)
{ /* PURPOSE: Copy a near or far string into the destination buffer. Params: destination, source. Returns: void. Globals: none. */
    I8 far *current = source;

    do {
        *destination = *current;
        ++destination;
        ++current; /* PORT: advancing this resource source uses the original far-pointer offset semantics. */
    } while (*current != '\0');

    *destination = '\0';
}

/* target mouse_draw_transparent_check @ 6382; candidate from build\workers\periph\mouse_draw_transparent_check.c */
/* merged owner s008-dlg member mouse_track_op */
I16 far doslider(I16 op, I16 x, I16 width, I16 top, I16 height,
                       I16 value, I16 offset, I16 divisions)
{ /* PURPOSE: Track mouse movement and process the current pointer action. Params: op, x, width, top, height, value, offset, divisions. Returns: int. Globals: reads flagsdown, msecoordx, pos_y_ms; writes flagsdown. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    /* Local view: drawrect; return is signed word; another unit uses void. */
    extern I16 far drawrect(I16, I16, I16, I16, I16);
    /* Local view: dlg_colour; type is signed word; another unit uses unsigned word. */
    extern I16 dlg_colour;
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
        factor = divisions << MOUSE_TRACK_FIXED_POINT_SHIFT; /* PORT: 16-bit signed products, left shifts, and division determine the slider thumb position. */
        end_of_track = max_value - 1;
        low = (end_of_track * value << MOUSE_TRACK_FIXED_POINT_SHIFT) / factor;
        end_position = ((value + offset) * end_of_track << MOUSE_TRACK_FIXED_POINT_SHIFT) / factor;
        range_length = end_position - low;
    }

    switch (op) {
    case 0:
        drawrect(x, top, width, height, 0) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        if (swapped == 0)
            drawrect(x + low, top, range_length, height, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        else
            drawrect(x, top + low, width, range_length, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
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
            input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */;
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
                drawrect(x, top, width, height, 0) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
                if (swapped == 0)
                    drawrect(x + thumb, top, range_length, height, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
                else
                    drawrect(x, top + thumb, width, range_length, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
                msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
            }
        } while ((flagsdown & 3) != 0);

    }

    if (value == -1)
        value = ((max_value / divisions) / 2 + thumb) * divisions / max_value;

    {
        I16 end_of_track;
        I16 factor;
        factor = divisions << MOUSE_TRACK_FIXED_POINT_SHIFT; /* PORT: 16-bit signed products, left shifts, and division determine the slider thumb position. */
        end_of_track = max_value - 1;
        low = (end_of_track * value << MOUSE_TRACK_FIXED_POINT_SHIFT) / factor;
        end_position = ((value + offset) * end_of_track << MOUSE_TRACK_FIXED_POINT_SHIFT) / factor;
        range_length = end_position - low;
    }
    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    drawrect(x, top, width, height, 0) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
    if (swapped == 0)
        drawrect(x + low, top, range_length, height, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
    else
        drawrect(x, top + low, width, range_length, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    return value;
}

void far msdrawtransparentchk(void)
{ /* PURPOSE: Redraw the mouse pointer in transparent mode when it is dirty. Params: none. Returns: void. Globals: reads kbormouse, mouse_isdirty; writes mouse_isdirty, mouse_transparent_mode. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    mouse_transparent_mode = 1;
    if (kbormouse != 0 && mouseonscreen == 0)
        doshowmouse() /* PLATFORM(video): draw the pointer using its transparent sprite path. */;
}

/* target mouse_draw_opaque_check @ 6406; candidate from build\workers\periph\mouse_draw_opaque_check.c */
void far msdrawopaquechk(void)
{ /* PURPOSE: Redraw the mouse pointer in opaque mode when it is dirty. Params: none. Returns: void. Globals: reads mouse_isdirty; writes mouse_transparent_mode. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    mouse_transparent_mode = 0;
    if (mouseonscreen != 0)
        dohidemouse() /* PLATFORM(video): draw the pointer using its opaque sprite path. */;
}

/* target mouse_draw_opaque @ 6424; candidate from build\workers\tuseg008\mouse_draw_opaque.c */
void far dohidemouse(void)
{ /* PURPOSE: Save the sprite buffers, draw the pointer, and restore the buffers. Params: none. Returns: void. Globals: reads mouse_unk_sprite_ptr; writes mouse_isdirty. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    struct SPRITE saved_sprite[2];
    savewindow(saved_sprite) /* PLATFORM(video): save both active sprite buffers. */;
    displaywindow() /* PLATFORM(video): copy sprite buffer 2 into buffer 1. */;
    sprputimage(mouse_unk_sprite_ptr->sprite_bitmapptr) /* PLATFORM(video): draw the selected shape. */;
    restorewindow(saved_sprite) /* PLATFORM(video): restore both active sprite buffers. */;
    mouseonscreen = 0;
}

/* target mouse_draw_transparent @ 6484; candidate from build\workers\tuseg008\mouse_draw_transparent.c */
void far doshowmouse(void)
{ /* PURPOSE: Draw the pointer sprite transparently into the active sprite buffer. Params: none. Returns: void. Globals: reads g_vid_flg2_set, mouse_ptr_cursor, mouse_unk_sprite_ptr, msecoordx, pos_y_ms, spritepointermini; writes mouse_isdirty. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    struct SPRITE saved_sprite[2];
    register I16 xpos;
    xpos = msecoordx;
    xpos -= xpos % g_vid_flg2_set;
    savewindow(saved_sprite) /* PLATFORM(video): save both active sprite buffers. */;
    displaywindow() /* PLATFORM(video): copy sprite buffer 2 into buffer 1. */;
    sprite_clear_shape_alt(mouse_unk_sprite_ptr->sprite_bitmapptr, xpos, pos_y_ms) /* PLATFORM(video): clear the requested shape in sprite buffer 1. */;
    andcfxya(mouse_ptr_cursor->sprite_bitmapptr, msecoordx, pos_y_ms) /* PLATFORM(video): apply a sprite mask at the requested position. */;
    orcfxya(spritepointermini->sprite_bitmapptr, msecoordx, pos_y_ms) /* PLATFORM(video): apply sprite pixels at the requested position. */;
    restorewindow(saved_sprite) /* PLATFORM(video): restore both active sprite buffers. */;
    mouseonscreen = 1;
}

/* target mouse_multi_hittest @ 6624; accepted source src/mouse_multi_hittest.c */
/* Local view: mousecursor; return is signed word; another unit uses signed byte. */
I16 far mousecursor(I16 count, I16 *left, I16 *right, I16 *top, I16 *bottom)
{ /* PURPOSE: Return the first candidate rectangle containing the current mouse position. Params: count, left, right, top, bottom. Returns: int. Globals: reads kbormouse, msecoordx, pos_y_ms; writes none. */
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

/* target check_input @ 6708; candidate from build\workers\tuseg008\check_input.c */
void far clear_keyboardbuffer(void)
{ /* PURPOSE: Poll keyboard or mouse state and update the input frame counter. Params: none. Returns: void. Globals: reads flagsdown, kbormouse; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    I8 done;
    do {
        if (joystate() /* PLATFORM(input_kb): read the configured keyboard-or-joystick state. */ & 0x30) {
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
void far waitforkey(void)
{ /* PURPOSE: Run the waiting-input update and copy the active sprite buffer. Params: none. Returns: void. Globals: none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    while (input_checking(timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */) /* PLATFORM(input_kb): poll the configured input devices. */ == 0) { }
    clear_keyboardbuffer();
}

/* target sprite_copy_2_to_1_2 @ 6796; accepted source src/sprite_copy_2_to_1_2.c */
/* Local view: sprite_clear_1_color; parameters[0] is unsigned byte; another unit uses signed word. */
void sprite_clear_1_color(U8  color);void sprcopy2to12(void) { /* PURPOSE: Set sprite buffer 1 from the current sprite argument. Params: none. Returns: void. Globals: reads sprite2; writes sprite2. */ /* PLATFORM(video): draws pixels, sprites, or text. */ sprite_setup1_from_arg_pointer(&sprite2) /* PLATFORM(video): select the sprite described by the argument pointer. */; }

/* target sprite_copy_2_to_1_clear @ 6814; accepted source src/sprite_copy_2_to_1_clear.c */
/* Local view: sprite_clear_1_color; parameters[0] is unsigned byte; another unit uses signed word. */
void sprite_clear_1_color(U8  color);void sprite_copy_2_to_1_clear(void) { /* PURPOSE: Set sprite buffer 1 from the current sprite and clear its color. Params: none. Returns: void. Globals: reads sprite2; writes sprite2. */ /* PLATFORM(video): draws pixels, sprites, or text. */ sprite_setup1_from_arg_pointer(&sprite2) /* PLATFORM(video): select the sprite described by the argument pointer. */; sprite_clear_1_color(0) /* PLATFORM(video): clear the active sprite buffer with the requested color. */; }

/* target sprite_copy_wnd_to_1 @ 6842; accepted source src/sprite_copy_wnd_to_1.c */
/* Local view: sprite_clear_1_color; parameters[0] is unsigned byte; another unit uses signed word. */
void sprite_clear_1_color(U8  color);void sprite_copy_wnd_to_1(void) { /* PURPOSE: Set sprite buffer 1 from the active window sprite. Params: none. Returns: void. Globals: reads g_wndspr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ sprite_setup1_from_arg_pointer(g_wndspr) /* PLATFORM(video): select the sprite described by the argument pointer. */; }

/* target sprite_copy_wnd_to_1_clear @ 6860; accepted source src/sprite_copy_wnd_to_1_clear.c */
/* Local view: sprite_clear_1_color; parameters[0] is unsigned byte; another unit uses signed word. */
void sprite_clear_1_color(U8  color);void sprite_copy_wnd_to_1_clear(void) { /* PURPOSE: Set sprite buffer 1 from the active window sprite and clear its color. Params: none. Returns: void. Globals: reads g_wndspr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ sprite_setup1_from_arg_pointer(g_wndspr) /* PLATFORM(video): select the sprite described by the argument pointer. */; sprite_clear_1_color(0) /* PLATFORM(video): clear the active sprite buffer with the requested color. */; }

/* target input_repeat_check @ 7306; candidate from build\workers\inputtu\repeat_types_ulong.c */
/* merged owner s008-rest member intro_draw_text */
/* Local view: shadowtext; return is near pointer; another unit uses void. */
I16 * shadowtext(I8 *str, I16 x, I16 y, I16 color, I16 shadow)
{ /* PURPOSE: Draw a line of introductory text with the selected font and color. Params: str, x, y, color, shadow. Returns: int *. Globals: reads fontdefvalue; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    
    
    /* Local view: font_setup_unknown; parameters[0] is signed word; another unit uses unsigned word. */
    extern void far font_setup_unknown(I16 color, I16 flags);
    

    text_bounds_upper = y;
    txt_bounds_bottom = y + fontdefvalue + 1;
    textboundsleft = x;
    text_bounds_right = x + textpixels(str) /* PLATFORM(video): load or select a named font. */ + 1;
    font_setup_unknown(shadow, 0) /* PLATFORM(video): select the font and color mode. */;
    font_draw_text(str, x + 1, y + 1) /* PLATFORM(video): draw text through the selected font. */;
    font_setup_unknown(color, 0) /* PLATFORM(video): select the font and color mode. */;
    font_draw_text(str, x, y) /* PLATFORM(video): draw text through the selected font. */;
    return &textboundsleft;
}

/* merged owner s008-rest member hiscore_draw_text */
/* Local view: hiscore_draw_text; return is near pointer; another unit uses void. */
I16 * hiscore_draw_text(I8 *str, I16 x, I16 y, I16 color, I16 shadow)
{ /* PURPOSE: Draw a high-score text line with its outline bounds. Params: str, x, y, color, shadow. Returns: int *. Globals: reads fontdefvalue, text_outline_bottom, text_outline_right, textoutlineleft, txt_outline_top; writes text_outline_bottom, text_outline_right, textoutlineleft, txt_outline_top. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    
    
    /* Local view: font_setup_unknown; parameters[0] is signed word; another unit uses unsigned word. */
    extern void far font_setup_unknown(I16 color, I16 flags);
    

    txt_outline_top = y - 1;
    text_outline_bottom = y + fontdefvalue + 1;
    textoutlineleft = x - 1;
    text_outline_right = x + textpixels(str) /* PLATFORM(video): load or select a named font. */ + 1;
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
/* Local view: call_read_line; return is signed word; another unit uses void. */
I16 far call_read_line(I8 *buffer, I16 x, I16 y, I16 width, I16 limit, I16 flags)
{ /* PURPOSE: Invoke the common line editor at the requested screen position. Params: buffer, x, y, width, limit, flags. Returns: int. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(input_kb): polls or updates keyboard state. */
    I16 result;
    I16 index;
    /* Local view: get_pstring; parameters[0] is signed word; another unit uses signed byte. */
    extern I16 far get_pstring(I16, I8 *, I16, I16, I16, I16, I16, void (far *)(void), I16, I16);
    

    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
    result = get_pstring(2, buffer, 0, x, x * 9 + 9, y, width,
                       kb_line_edit_noop,
                       limit, flags) /* PLATFORM(input_kb): run the keyboard line editor. */;
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    x = strlen(buffer);
    index = x - 1;
    while (buffer[index] == ' ')
        --index;
    buffer[index + 1] = 0;
    return result;
}

I16 far input_repeat_check(I16 timeout)
{ /* PURPOSE: Wait for repeated input or timeout while updating the prompt display. Params: timeout. Returns: int. Globals: none. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(input_kb): polls or updates keyboard state. */
    register I16 result;
    I16 delta_time;
    register I16 time_total;
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
void far sprite_draw_beveled_frame(I16 left, I16 top, I16 width, I16 height,
                        I16 light, I16 middle, I16 dark)
{ /* PURPOSE: Draw the line segments associated with the current prompt. Params: left, top, width, height, light, middle, dark. Returns: void. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    register I16 x_right;
    register I16 bottom_end;
    I16 left1, top_row2, top_y1, left2;
    I16 y_bottom1, right_2, right1, y_bottom2;
    

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
void far draw3dbutton(I8 far *caption, I16 x, I16 y, I16 width, I16 height,
                     I16 light, I16 dark, I16 sprite_id, I16 font_style)
{ /* PURPOSE: Draw a button body and its centered label. Params: caption, x, y, width, height, light, dark, sprite_id, font_style. Returns: void. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
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
    /* Local view: resbuftext; type is array[None]; another unit uses signed byte. */
    extern I8 resbuftext[];
    /* Local view: drawrect; return is signed word; another unit uses void. */
    extern I16 far drawrect(I16, I16, I16, I16, I16);
    
    /* Local view: font_setup_unknown; parameters[0] is signed word; another unit uses unsigned word. */
    extern void far font_setup_unknown(I16, I16);
    

    rgt = x + width;
    bottom = y + height;
    drawrect(x, y, width, height, sprite_id) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
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
    strcpyfar(resbuftext, caption);
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
            font_draw_text(line, x + (width - textpixels(line) /* PLATFORM(video): load or select a named font. */) / 2,
                           line_no * 8 + y + text_y) /* PLATFORM(video): draw text through the selected font. */;
            line_no++;
            pos = 0;
        } else {
            line[pos] = c;
            pos++;
        }
    }
}

void makeobjdef(I8 far *shapeptr, struct SHAPE3D *gameshape)
{ /* PURPOSE: Copy the compact 3D shape header into the game shape record. Params: shapeptr, gameshape. Returns: void. Globals: none. */
    gameshape->numverts = ((struct SHAPE3DHEADER far *)shapeptr)->numverts;
    gameshape->numprimitives = ((struct SHAPE3DHEADER far *)shapeptr)->numprimitives;
    gameshape->numpaints = ((struct SHAPE3DHEADER far *)shapeptr)->numpaints;
    gameshape->verts = shapeptr + SHAPE3D_RESOURCE_HEADER_BYTES; /* PORT: far-pointer arithmetic follows 16-bit segment:offset rules and the encoded resource record sizes. */
    gameshape->cull1 = shapeptr + gameshape->numverts * SHAPE3D_RESOURCE_VERTEX_BYTES + SHAPE3D_RESOURCE_HEADER_BYTES; /* PORT: resource offsets must retain 16-bit far-pointer arithmetic and exact record sizes. */ /* PORT: resource offsets must retain 16-bit far-pointer arithmetic and exact record sizes. */
    gameshape->cull2 = shapeptr + gameshape->numprimitives * SHAPE3D_RESOURCE_CULL_RECORD_BYTES
                       + gameshape->numverts * SHAPE3D_RESOURCE_VERTEX_BYTES + SHAPE3D_RESOURCE_HEADER_BYTES; /* PORT: resource offsets must retain 16-bit far-pointer arithmetic and exact record sizes. */
    gameshape->primitives = shapeptr + gameshape->numprimitives * SHAPE3D_RESOURCE_PRIMITIVE_BYTES
                            + gameshape->numverts * SHAPE3D_RESOURCE_VERTEX_BYTES + SHAPE3D_RESOURCE_HEADER_BYTES; /* PORT: resource offsets must retain 16-bit far-pointer arithmetic and exact record sizes. */
}

/* target font_op2_alt @ 8534; accepted source src/font_op2_alt.c */
I16 far mycenter(I8 *name)
{ /* PURPOSE: Load or select a named font through the far font entry point. Params: name. Returns: int. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    return (PLATFORM_SCREEN_WIDTH_PIXELS - textpixels(name) /* PLATFORM(video): load or select a named font. */) / 2;
}

/* target sprite_blit_to_video @ 8560; candidate from build\workers\tuseg008\sprite_blit_to_video.c */
/* Local view: sprite_blit_to_video; parameters[0].pointer is struct SPRITE; another unit uses void. */
I16 far sprite_blit_to_video(struct SPRITE far *sprite, I16 mode)
{ /* PURPOSE: Commit the active sprite buffer to the video display. Params: sprite, mode. Returns: int. Globals: none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    register I16 index;
    register I16 result;
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
            sprite_blit_dither_phase(sprite->sprite_bitmapptr, index) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        }
    }
    msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    return 0;
}

/* target reset_idle_counters @ 8898; accepted source src/reset_idle_counters.c */
/* merged owner s008-rest member show_waiting */
void far loading(void)
{ /* PURPOSE: Display the waiting dialog and redraw the mouse pointer. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16, I16);
    
    messagebox(0, 0, locate_text_resource(main_data_file_addr, "wai") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, waitm_ms, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
}

/* Local view: print_int_as_string_maybe; return is void; another unit uses signed word. */
void far print_int_as_string_maybe(I8 *buffer, I16 value, I16 zero_fill, I16 width)
{ /* PURPOSE: Format an integer in the caller buffer with width and zero-fill options. Params: buffer, value, zero_fill, width. Returns: void. Globals: none. */
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

void reset_idle_counters(void) { /* PURPOSE: Clear menu animation and idle counters. Params: none. Returns: void. Globals: reads none; writes g_animphase, g_hovercolor_idle, unused_count. */
    g_animphase = 0;
    g_hovercolor_idle = 0;
    unused_count = 0;
}

/* target file_load_audiores @ 9036; candidate from build\references\restunts\src\restunts\c\fileio.c */
/* merged owner s008-rest member mouse_timer_sprite_unk */
I16 far mouse_timer_sprite_unknown(I16 index, I16 *a, I16 *b, I16 *c, I16 *d,
                               I16 first, I16 second)
{ /* PURPOSE: Advance the pointer animation and redraw it when its selected state changes. Params: index, a, b, c, d, first, second. Returns: int. Globals: reads g_animphase, g_hovercolor_idle; writes g_animphase, g_hovercolor_idle. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    register I16 delta;
    register I16 selected;
    /* Local view: sprite_draw_rect_outline; parameters[4] is signed word; another unit uses unsigned word. */
    extern void far sprite_draw_rect_outline(I16, I16, I16, I16, I16);
    delta = timer_get_delta_alt() /* PLATFORM(timer): read the elapsed timer delta through the far entry point. */;
    g_animphase += delta;
    while (g_animphase > 60)
        g_animphase -= 60;
    if (g_animphase > 30) selected = first;
    else selected = second;
    if (selected != g_hovercolor_idle) {
        g_hovercolor_idle = selected;
        msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
        sprite_draw_rect_outline(a[index], c[index], b[index], d[index], selected) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        msdrawtransparentchk() /* PLATFORM(video): redraw the pointer in transparent mode when needed. */;
    }
    return delta;
}

void file_load_audio_resource(const I8* songfile, const I8* voicefile, const I8* name) { /* PURPOSE: Load song and voice resources, initialize them, and finish audio setup. Params: songfile, voicefile, name. Returns: void. Globals: reads musicfile, openvfile; writes is_audioloaded, musicfile, openvfile. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */
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
    freememory(musicfile) /* PLATFORM(memory): release the resource through game memory management. */;
    freememory(openvfile) /* PLATFORM(memory): release the resource through game memory management. */;
    is_audioloaded = 0;
}

/* target font_set_fontdef2 @ 9178; accepted source src/font_set_fontdef2.c */
void far fontsetfontdef2(void far *data)
{ /* PURPOSE: Apply a font-definition pointer and cache its value. Params: data. Returns: void. Globals: reads none; writes fontdefvalue. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    setfont(data) /* PLATFORM(video): select the font-definition data segment. */;
    fontdefvalue = ((struct FONTDEF_PREFIX far *)data)->value;
}

/* target font_set_fontdef @ 9208; candidate from build\workers\font\font_set_fontdef.c */
void far fontsetfontdef(void)
{ /* PURPOSE: Apply the currently selected font definition. Params: none. Returns: void. Globals: reads def_fntadr; writes none. */
    fontsetfontdef2(def_fntadr);
}

/* target get_super_random @ 9438; candidate from build\references\restunts\src\restunts\c\restunts.c */
/* Local view: fmtframestr; parameters[1] is unsigned word; another unit uses signed word. */
void far fmtframestr(I8 *destination, U16  frame_count, I16 hundredths)
{ /* PURPOSE: Format a frame count as a minutes, seconds, and optional hundredths string. Params: destination, frame_count, hundredths. Returns: void. Globals: reads rate_frame; writes none. */
    register I16 minute_count;
    I8 buffer[18];
    register I16 seconds;
    I16 minute_frames;

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

I16 get_super_random(void)
{ /* PURPOSE: Combine timer, random, and race-frame values into a nonnegative result. Params: none. Returns: int. Globals: reads race_stats; writes none. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    register I16 random_value = (I16)(gettick() /* PLATFORM(timer): read the current timer count. */ + get_kevinrandom() + rand() + race_stats.game_frame);
    return random_value < 0 ? -random_value : random_value;
}

/* target file_load_resource @ 9498; candidate from build\references\restunts\src\restunts\c\fileio.c */
void far* file_load_resource(I16 type, const I8* filename) { /* PURPOSE: Dispatch loading by resource type and offer retry after a failed load. Params: type, filename. Returns: void *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
	void far* result;
	while (1) {
		switch (type) {
			case 0:
				// try load the file, if it fails, show a dialog, and retry
				result = loadfilez(filename) /* PLATFORM(file): load a binary file without terminating on failure. */;
				break;

			case 1:
				return loadfilez(filename) /* PLATFORM(file): load a binary file without terminating on failure. */;

			case 7:
				// try load a compressed file
				return loadpackz(filename) /* PLATFORM(file): decompress a resource without terminating on failure. */;

			case 2:
				// try load a 2d shape and retry if it failed
				result = load_shape2d_nofatal_thunk(filename) /* PLATFORM(file): load a 2D shape without terminating on failure. */;
				break;

			case 3:
				// try load a 2d shape and retry if it failed
				result = load_shape2d_res_nofatal_thunk(filename) /* PLATFORM(file): load a 2D shape from resource data. */;
				break;

			case 4:
				// try load a song file and retry if it failed
				result = load_song_file(filename) /* PLATFORM(file): load song data. */;
				break;

			case 5:
				// try load a voice file and retry if it failed
				result = load_voice_file(filename) /* PLATFORM(file): load voice data. */;
				break;

			case 6:
				// try load an sfx file and retry if it failed
				result = load_sfx_file(filename) /* PLATFORM(file): load sound-effect data. */;
				break;

			case 8:
				// try load a 2d shape and retry if it failed
				result = file_load_shape2d_nofatal2(filename) /* PLATFORM(file): load a 2D shape without terminating on failure. */;
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

/* target input_push_status @ 9788; accepted source src/input_push_status.c */
/* Local view: read_file_with_retry; parameters.count is 4; another unit uses 3. */
void far *read_file_with_retry(I16 type, U16  first, U16  second, U16  third)
{ /* PURPOSE: Read the requested resource and offer retry after a read failure. Params: type, first, second, third. Returns: void *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
    void far *result;
    for (;;) {
        switch (type) {
        case 9:
            result = loadfileatz(first, second, third) /* PLATFORM(file): read a file without terminating on failure. */;
            break;
        case 10:
            return loadfileatz(first, second, third) /* PLATFORM(file): read a file without terminating on failure. */;
        }
        if (result != 0) return result;
        if (do_dea_textres() == 2) return 0;
    }
}

I8S  input_status_stack_depth = 0;

void far pushmouse(void)
{ /* PURPOSE: Push the current keyboard and mouse modes onto the input stack. Params: none. Returns: void. Globals: reads input_status_stack_depth, kbormouse, mouse_transparent_mode; writes copy_mouse_modes, input_device_modestack, input_status_stack_depth. */
    copy_mouse_modes[input_status_stack_depth] = mouse_transparent_mode;
    input_device_modestack[input_status_stack_depth] = kbormouse;
    ++input_status_stack_depth;
}

/* target input_pop_status @ 9816; candidate from build\workers\periph\input_pop_status.c */
void far pullmouse(void)
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
    /* Local view: input_pushed; type is signed word; another unit uses unsigned word. */
    extern I16 input_pushed;
    /* Local view: joystick_enabled, mouse_enabled; type is signed byte; another unit uses unsigned byte. */
    extern I8 joystick_enabled, mouse_enabled;
    /* Local view: audio_pause_playback; return is signed word; another unit uses void. */
    extern I16 far audio_pause_playback(void);
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16 *, I16);
    
    
    
    
    
    /* Local view: drawrect; return is signed word; another unit uses void. */
    extern I16 far drawrect(I16, I16, I16, I16, I16);
    /* Local view: dlg_colour; type is unsigned word; another unit uses signed word. */
    extern U16  dlg_colour;
    
    I16 ys[9];
    I16 ht;
    I16 xs[9];
    I16 coords[14];
    I16 cw;
    register I16 lastpos;
    register I16 cur;
    I16 idx;
    I8 hit[9];

    pushmouse() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    input_pushed = 1;
    audio_pause_playback() /* PLATFORM(audio): read audio option state. */;
    if (messagebox(3, 1, locate_text_resource(main_data_file_addr, "joy") /* PLATFORM(file): locate a named text entry in resource data. */,
                    -1, -1, dialog_frame_color, coords, 0) /* PLATFORM(video): present the interactive dialog renderer. */ > 0) {
        for (idx = 0; idx < 9; ++idx)
            hit[idx] = 0;
        joystick_enabled = 1;
        msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
        drawrect(coords[2] - 4, coords[3], 1, coords[13] - coords[3] - 8, dialog_frame_color) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        drawrect(coords[4] - 4, coords[5], 1, coords[13] - coords[3] - 8, dialog_frame_color) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        drawrect(coords[0], coords[9] - 4, coords[6] - coords[0], 1, dialog_frame_color) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
        drawrect(coords[0], coords[11] - 4, coords[6] - coords[0], 1, dialog_frame_color) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
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
            if (normkey() /* PLATFORM(input_kb): read the selected keyboard character. */ != 0)
                break;
            cur = get_joy_flags() /* PLATFORM(input_joy): read joystick button/direction state. */;
            if (cur & 0x30)
                break;
            cur = convertjoy(cur) /* PLATFORM(input_joy): convert joystick flags to a control index. */;
            if (cur == lastpos)
                continue;
            for (idx = 0; idx < 9; ++idx)
                drawrect(xs[idx], ys[idx], cw, ht, font_secondary_color) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
            drawrect(xs[cur], ys[cur], cw, ht, dlg_colour) /* PLATFORM(video): draw through the legacy sprite primitive interface. */;
            lastpos = cur;
            hit[cur] = 1;
        }
        for (idx = 0; idx < 9; ++idx)
            joystick_enabled &= hit[idx];
        restoreboxwindow();
        if (joystick_enabled == 0)
            messagebox(1, 1, locate_text_resource(main_data_file_addr, "jox") /* PLATFORM(file): locate a named text entry in resource data. */,
                        -1, -1, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    } else {
        joystick_enabled = 0;
    }
    purgekey() /* PLATFORM(input_kb): poll keyboard state. */;
    mouse_enabled = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    input_pushed = 0;
    pullmouse() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_key_restext */
void far mkeyon(void)
{ /* PURPOSE: Show keyboard help and restore audio and input state. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    /* Local view: input_pushed; type is unsigned word; another unit uses signed word. */
    extern U16  input_pushed;
    /* Local view: joystick_enabled, mouse_enabled; type is unsigned byte; another unit uses signed byte. */
    extern U8  joystick_enabled, mouse_enabled;
    /* Local view: audio_pause_playback; return is signed word; another unit uses void. */
    extern I16 far audio_pause_playback(void);
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16, I16);
    

    pushmouse() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    input_pushed = 1;
    audio_pause_playback() /* PLATFORM(audio): read audio option state. */;
    messagebox(4, 1, locate_text_resource(main_data_file_addr, "key") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    joystick_enabled = 0;
    mouse_enabled = 0;
    input_pushed = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    pullmouse() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_mou_restext */
void far do_mou_resource_text(void)
{ /* PURPOSE: Show mouse help and restore audio and input state. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    /* Local view: input_pushed; type is unsigned word; another unit uses signed word. */
    extern U16  input_pushed;
    /* Local view: mouse_enabled; type is unsigned byte; another unit uses signed byte. */
    extern U8  mouse_enabled;
    /* Local view: audio_pause_playback; return is signed word; another unit uses void. */
    extern I16 far audio_pause_playback(void);
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16, I16);
    
    pushmouse() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    input_pushed = 1;
    audio_pause_playback() /* PLATFORM(audio): read audio option state. */;
    mouse_enabled = 1;
    messagebox(4, 1, locate_text_resource(main_data_file_addr, "mou") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    input_pushed = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    pullmouse() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_pau_restext */
void far msystempause(void)
{ /* PURPOSE: Show pause help and restore audio and input state. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    /* Local view: input_pushed; type is unsigned word; another unit uses signed word. */
    extern U16  input_pushed;
    /* Local view: audio_pause_playback; return is signed word; another unit uses void. */
    extern I16 far audio_pause_playback(void);
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16, I16);
    
    pushmouse() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    input_pushed = 1;
    audio_pause_playback() /* PLATFORM(audio): read audio option state. */;
    messagebox(1, 1, locate_text_resource(main_data_file_addr, "pau") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    input_pushed = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    pullmouse() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_mof_restext */
void far do_mof_resource_text(void)
{ /* PURPOSE: Toggle music and effects options from their help page. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    /* Local view: input_pushed; type is unsigned word; another unit uses signed word. */
    extern U16  input_pushed;
    
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16, I16);
    pushmouse() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    input_pushed = 1;
    if (audio_toggle_flag2() /* PLATFORM(audio): toggle the music/effects audio option. */)
        messagebox(4, 1, locate_text_resource(main_data_file_addr, "mon") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    else
        messagebox(4, 1, locate_text_resource(main_data_file_addr, "mof") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    input_pushed = 0;
    pullmouse() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_sonsof_restext */
void far do_sonsof_resource_text(void)
{ /* PURPOSE: Toggle sound options from their help page. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    /* Local view: input_pushed; type is unsigned word; another unit uses signed word. */
    extern U16  input_pushed;
    
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16, I16);
    pushmouse() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    input_pushed = 1;
    if (audio_toggle_flag6() /* PLATFORM(audio): toggle the sound audio option. */)
        messagebox(4, 1, locate_text_resource(main_data_file_addr, "son") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    else
        messagebox(4, 1, locate_text_resource(main_data_file_addr, "sof") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    input_pushed = 0;
    pullmouse() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member do_dos_restext */
void far do_dos_resource_text(void)
{ /* PURPOSE: Show DOS options and run the exit-list action when selected. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(dos): uses the DOS exit-list service. */
    /* Local view: input_pushed; type is unsigned word; another unit uses signed word. */
    extern U16  input_pushed;
    /* Local view: audio_pause_playback; return is signed word; another unit uses void. */
    extern I16 far audio_pause_playback(void);
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16, I16);
    
    
    pushmouse() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    input_pushed = 1;
    audio_pause_playback() /* PLATFORM(audio): read audio option state. */;
    if (messagebox(2, 1, locate_text_resource(main_data_file_addr, "dos") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */ == 1)
        dsiexit() /* PLATFORM(dos): run the DOS exit-list action. */;
    input_pushed = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    pullmouse() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

/* merged owner s008-rest member show_graphic_levels_menu */
void far show_graphic_levels_menu(void)
{ /* PURPOSE: Show graphics-level help and restore audio and input state. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(audio): loads, updates, or releases audio resources. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    I8 menu[512];
    U8  active[9];
    I8 rc;
    I16 saved_rate;
    register I16 selection_index;
    register I16 cursor;
    /* Local view: renderlevel; type is unsigned byte; another unit uses signed byte. */
    extern U8  renderlevel;
    /* Local view: input_pushed; type is unsigned word; another unit uses signed word. */
extern unsigned int input_pushed;
 /* Local view: performGraphColor; MSC 5.10 requires separate block extern indices; a file-scope declaration collapses these imports. */
extern I16 performGraphColor;
    /* Local view: audio_pause_playback; return is signed word; another unit uses void. */
    extern I16 far audio_pause_playback(void);
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16, I16);

    pushmouse() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    input_pushed = 1;
    audio_pause_playback() /* PLATFORM(audio): read audio option state. */;
    saved_rate = frm_rate2;
    rc = 0;
    for (;;) {
        strcpyfar(menu, locate_text_resource(main_data_file_addr, "mrl") /* PLATFORM(file): locate a named text entry in resource data. */);
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
                         0xffff, 0xffff, performGraphColor, 0, rc) /* PLATFORM(video): present the interactive dialog renderer. */;
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
        messagebox(1, 1, locate_text_resource(main_data_file_addr, "mrs") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
    input_pushed = 0;
    restore_audio_volume() /* PLATFORM(audio): restore the configured audio volume. */;
    pullmouse() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
}

I16S far do_dea_textres(void)
{ /* PURPOSE: Show the general help dialog while preserving nested input state. Params: none. Returns: short. Globals: reads dialogarg2, g_is_busy, main_data_file_addr; writes none. */ /* PLATFORM(input_kb): polls or updates keyboard state. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    I16S result;
    pushmouse() /* PLATFORM(input_kb): save the active keyboard and mouse modes. */;
    if (g_is_busy != 0) {
        result = messagebox(2, 1, locate_text_resource(main_data_file_addr, "dea") /* PLATFORM(file): locate a named text entry in resource data. */,
                             0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
        if (result != 0) result = 0;
        else result = 1;
    } else {
        messagebox(1, 1, locate_text_resource(main_data_file_addr, "der") /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
        result = 1;
    }
    pullmouse() /* PLATFORM(input_kb): restore the saved keyboard and mouse modes. */;
    return result;
}

/* target do_mer_restext @ 11600; reconstructed from anchored call sequence */
/* merged owner s008-rest member ensure_file_exists */
void far ensure_file_exists(I16 index)
{ /* PURPOSE: Find the requested file and ask for another path when it is absent. Params: index. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes kbormouse. */ /* PLATFORM(file): uses file and resource services. */ /* PLATFORM(video): draws pixels, sprites, or text. */
    /* Local view: messagebox; return is signed word; another unit uses unsigned word. */
    extern I16 far messagebox(I16, I16, I8 far *, I16, I16, I16, I16, I16);
    /* Local view: findfirst; return is signed word; another unit uses near pointer. */
    extern I16 far findfirst(I8 *filename);
    while (findfirst(findfilenames[index - 1]) /* PLATFORM(file): search for a file by name. */ == 0) {
        messagebox(1, 1, locate_text_resource(main_data_file_addr, findfiletexts[index - 1]) /* PLATFORM(file): locate a named text entry in resource data. */,
                    0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
        msdrawopaquechk() /* PLATFORM(video): redraw the pointer in opaque mode when needed. */;
        kbormouse = 0;
    }
}

void far memerr(void)
{ /* PURPOSE: Show the multiplayer help dialog. Params: none. Returns: void. Globals: reads dialogarg2, main_data_file_addr; writes none. */ /* PLATFORM(video): draws pixels, sprites, or text. */ /* PLATFORM(file): uses file and resource services. */
    messagebox(1, 1, locate_text_resource(main_data_file_addr, "mer") /* PLATFORM(file): locate a named text entry in resource data. */,
                0xffff, 0xffff, dialog_frame_color, 0, 0) /* PLATFORM(video): present the interactive dialog renderer. */;
}

/* target timer_get_delta_alt @ 11648; accepted source src/timer_get_delta_alt.c */
/* Local view: timer_get_delta_alt; return is signed word; another unit uses unsigned long. */
I16  timer_get_delta_alt(void)
{ /* PURPOSE: Return the timer delta through the far entry point. Params: none. Returns: unsigned long. Globals: none. */ /* PLATFORM(timer): registers, removes, or reads the game timer. */
    return elapsedticks() /* PLATFORM(timer): read the elapsed timer delta. */;
}

/* target file_load_3dres @ 11654; candidate from build\workers\tuseg008\file_load_3dres_scope.c */
/* Local view: fluffyload; return.pointer is void; another unit uses signed byte. */
void far *fluffyload(I8 *filename)
{ /* PURPOSE: Load a 3D resource file and retry after a failed load. Params: filename. Returns: void *. Globals: none. */ /* PLATFORM(file): uses file and resource services. */
    void far *result;
    while (1) {
        I8 name[0x50];
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

