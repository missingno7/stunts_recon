#include "platform_hw.h"
/* READABILITY: Adapt the DOS mouse driver register interface to the game cursor state and coordinate conventions. */
typedef struct MouseRegs {
    unsigned int ax;
    unsigned int bx;
    unsigned int cx;
    unsigned int dx;
    unsigned int si;
    unsigned int di;
    unsigned int cflag;
} MouseRegs;

MouseRegs msregisterms;
unsigned int ms_buttons;
unsigned int cursorxposition;
unsigned int mouse_api_y;
extern unsigned int mousehorscale;
extern int mouse_button_state_cache;
extern int showmouse;
extern int far int86(unsigned int interrupt_no, MouseRegs *input, MouseRegs *output);
void far mouse_set_pixratio(unsigned int xratio, unsigned int yratio);
void far mouse_set_minmax(unsigned int xmin, unsigned int ymin, unsigned int xmax, unsigned int ymax);

/* Set the DOS mouse horizontal and vertical pixel ratios.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): this service call sets DOS mouse pixel scaling. */
void far mouse_set_pixratio(unsigned int xratio, unsigned int yratio)
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
int far mouse_init(unsigned int width, unsigned int height)
{
    unsigned int status;
    msregisterms.ax = PLATFORM_BIOS_MOUSE_INIT_SELECTOR;
    /* PLATFORM(bios): call BIOS INT 15h with the mouse initialization selector. */
    int86(PLATFORM_BIOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    msregisterms.ax = MOUSE_FN_RESET;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    status = msregisterms.ax;
    ms_buttons = msregisterms.bx;
    if (status != 0) {
        if (width == 320) mousehorscale = 1;
        else mousehorscale = 0;
        mouse_set_minmax(0, 0, width - 1, height - 1);
        mouse_set_pixratio(16, 16);
        mouse_button_state_cache = 0xffff;
    }
    return status;
}

/* Set horizontal and vertical cursor limits, applying the game horizontal scale to X coordinates.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
/* PLATFORM(input_mouse): set DOS mouse horizontal and vertical limits. */
void far mouse_set_minmax(unsigned int xmin, unsigned int ymin,
                          unsigned int xmax, unsigned int ymax)
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
static unsigned int far mouse_get_position(void)
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
static void far mouse_show_cursor(void)
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
static void far mouse_hide_cursor(void)
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
void far mouse_set_position(unsigned int x, unsigned int y)
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
void far mouse_get_state(unsigned int *ax, unsigned int *cx, unsigned int *dx)
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
void far nopsub_36A9A(int xmin, int xmax)
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
void far nopsub_36ACA(unsigned int ymin, unsigned int ymax)
{
    msregisterms.ax = MOUSE_FN_SET_VERTICAL_RANGE;
    msregisterms.cx = ymin;
    msregisterms.dx = ymax;
    /* PLATFORM(input_mouse): call the DOS mouse driver through INT 33h. */
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

int mouse_button_state_cache = 0;
unsigned int mousehorscale = 0;
int showmouse = 0;
