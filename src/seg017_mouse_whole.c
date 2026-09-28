#include "stunts_types.h"
#include "platform_hw.h"
#define MOUSE_DEFAULT_PIXEL_RATIO 16
/* READABILITY: Adapt the DOS mouse driver register interface to the game cursor state and coordinate conventions. */
/* PORT: INT register packets require 16-bit word fields and MSC-compatible struct layout. */
typedef struct MouseRegs {
    U16 ax;
    U16 bx;
    U16 cx;
    U16 dx;
    U16 si;
    U16 di;
    U16 cflag;
} MouseRegs;

MouseRegs msregisterms;
unsigned int ms_buttons;
unsigned int cursorxposition;
unsigned int mouse_api_y;
extern U16 mousehorscale;
extern I16 mouse_button_state_cache;
extern I16 showmouse;
extern I16 FAR int86(U16 interrupt_no, MouseRegs *input, MouseRegs *output);
void FAR mouse_set_pixratio(U16 xratio, U16 yratio);
void FAR mouse_set_minmax(U16 xmin, U16 ymin, U16 xmax, U16 ymax);

/* Set the DOS mouse horizontal and vertical pixel ratios.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): this service call sets DOS mouse pixel scaling. */
void FAR mouse_set_pixratio(U16 xratio, U16 yratio)
{
    msregisterms.ax = MOUSE_FN_SET_PIXEL_RATIO;
    msregisterms.cx = xratio;
    msregisterms.dx = yratio;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Initialize the BIOS mouse interface and DOS mouse driver; configure bounds and pixel scaling when present.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): initialize the DOS mouse and set its game bounds. */
I16 FAR mouse_init(U16 width, U16 height)
{
    U16 status;
    msregisterms.ax = PLATFORM_BIOS_MOUSE_INIT_SELECTOR;
    /* PLATFORM(bios): call BIOS INT 15h with the mouse initialization selector. */
    int86(PLATFORM_BIOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    msregisterms.ax = MOUSE_FN_RESET;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    status = msregisterms.ax;
    ms_buttons = msregisterms.bx;
    if (status != 0) {
        if (width == PLATFORM_SCREEN_WIDTH_PIXELS) mousehorscale = 1;
        else mousehorscale = 0;
        mouse_set_minmax(0, 0, width - 1, height - 1);
        mouse_set_pixratio(MOUSE_DEFAULT_PIXEL_RATIO, MOUSE_DEFAULT_PIXEL_RATIO);
        mouse_button_state_cache = 0xffff;
    }
    return status;
}

/* Set horizontal and vertical cursor limits, applying the game horizontal scale to X coordinates.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): set DOS mouse horizontal and vertical limits. */
void FAR mouse_set_minmax(U16 xmin, U16 ymin,
                          U16 xmax, U16 ymax)
{
    msregisterms.ax = MOUSE_FN_SET_HORIZONTAL_RANGE;
    msregisterms.cx = xmin << mousehorscale;
    msregisterms.dx = xmax << mousehorscale;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    msregisterms.ax = MOUSE_FN_SET_VERTICAL_RANGE;
    msregisterms.cx = ymin;
    msregisterms.dx = ymax;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Read the DOS mouse position and button mask into the game cursor globals.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): read cursor coordinates and button state. */
static U16 FAR mouse_get_position(void)
{
    msregisterms.ax = MOUSE_FN_GET_POSITION_AND_BUTTONS;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    ms_buttons = msregisterms.bx;
    cursorxposition = msregisterms.cx >> mousehorscale;
    mouse_api_y = msregisterms.dx;
    return ms_buttons;
}

/* Increment the nested show count and show the DOS mouse cursor when it becomes visible.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): show the DOS mouse cursor. */
static void FAR mouse_show_cursor(void)
{
    ++showmouse;
    if (showmouse < 1) return;
    showmouse = 1;
    msregisterms.ax = MOUSE_FN_SHOW_CURSOR;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Decrement the nested show count and hide the DOS mouse cursor at zero.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): hide the DOS mouse cursor. */
static void FAR mouse_hide_cursor(void)
{
    --showmouse;
    if (showmouse != 0) return;
    msregisterms.ax = MOUSE_FN_HIDE_CURSOR;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Update cached coordinates and move the DOS mouse cursor.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): move the DOS mouse cursor. */
void FAR mouse_set_position(U16 x, U16 y)
{
    msregisterms.ax = MOUSE_FN_SET_POSITION;
    cursorxposition = x;
    msregisterms.cx = x << mousehorscale;
    msregisterms.dx = y;
    mouse_api_y = y;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Return the current button mask and scaled X and unscaled Y coordinates.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): read the DOS mouse button and coordinate state. */
void FAR mouse_get_state(U16 *ax, U16 *cx, U16 *dx)
{
    msregisterms.ax = MOUSE_FN_GET_POSITION_AND_BUTTONS;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    *ax = msregisterms.bx;
    *cx = msregisterms.cx >> mousehorscale;
    *dx = msregisterms.dx;
}

/* Set a horizontal DOS mouse cursor range.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): update the DOS mouse horizontal range. */
void FAR nopsub_36A9A(I16 xmin, I16 xmax)
{
    msregisterms.ax = MOUSE_FN_SET_HORIZONTAL_RANGE;
    msregisterms.cx = xmin >> mousehorscale;
    msregisterms.dx = xmax >> mousehorscale;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Set a vertical DOS mouse cursor range.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): update the DOS mouse vertical range. */
void FAR nopsub_36ACA(U16 ymin, U16 ymax)
{
    msregisterms.ax = MOUSE_FN_SET_VERTICAL_RANGE;
    msregisterms.cx = ymin;
    msregisterms.dx = ymax;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

I16 mouse_button_state_cache = 0;
U16 mousehorscale = 0;
I16 showmouse = 0;
