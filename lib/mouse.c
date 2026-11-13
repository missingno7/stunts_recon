#include "types.h"
#define MOUSE_DEFAULT_PIXEL_RATIO 16
/* Adapt the DOS mouse driver register interface to the game cursor state and coordinate conventions. */
#include "input.h"
#include "math3d.h"
#include "viewstat.h"
#include "game.h"

MouseRegs msregisterms;
unsigned int ms_buttons;
unsigned int cursorxposition;
unsigned int mouse_api_y;

/* Set the DOS mouse horizontal and vertical pixel ratios.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR mouse_set_pixratio(U16 xratio, U16 yratio)
{
    msregisterms.ax = MOUSE_FN_SET_PIXEL_RATIO;
    msregisterms.cx = xratio;
    msregisterms.dx = yratio;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Initialize the BIOS mouse interface and DOS mouse driver; configure bounds and pixel scaling when present.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
I16 FAR initmouse(U16 width, U16 height)
{
    U16 status;
    msregisterms.ax = PLATFORM_BIOS_MOUSE_INIT_SELECTOR;
    int86(PLATFORM_BIOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    msregisterms.ax = MOUSE_FN_RESET;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    status = msregisterms.ax;
    ms_buttons = msregisterms.bx;
    if (status != 0) {
        if (width == PLATFORM_SCREEN_WIDTH_PIXELS) mousehorscale = 1;
        else mousehorscale = 0;
        setmousebounds(0, 0, width - 1, height - 1);
        mouse_set_pixratio(MOUSE_DEFAULT_PIXEL_RATIO, MOUSE_DEFAULT_PIXEL_RATIO);
        mouse_button_state_cache = 0xffff;
    }
    return status;
}

/* Set horizontal and vertical cursor limits, applying the game horizontal scale to X coordinates.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR setmousebounds(U16 xmin, U16 ymin,
                          U16 xmax, U16 ymax)
{
    msregisterms.ax = MOUSE_FN_SET_HORIZONTAL_RANGE;
    msregisterms.cx = xmin << mousehorscale;
    msregisterms.dx = xmax << mousehorscale;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    msregisterms.ax = MOUSE_FN_SET_VERTICAL_RANGE;
    msregisterms.cx = ymin;
    msregisterms.dx = ymax;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Read the DOS mouse position and button mask into the game cursor globals.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
static U16 FAR mouse_get_position(void)
{
    msregisterms.ax = MOUSE_FN_GET_POSITION_AND_BUTTONS;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    ms_buttons = msregisterms.bx;
    cursorxposition = msregisterms.cx >> mousehorscale;
    mouse_api_y = msregisterms.dx;
    return ms_buttons;
}

/* Increment the nested show count and show the DOS mouse cursor when it becomes visible.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
static void FAR mouse_show_cursor(void)
{
    ++mouseshown;
    if (mouseshown < 1) return;
    mouseshown = 1;
    msregisterms.ax = MOUSE_FN_SHOW_CURSOR;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Decrement the nested show count and hide the DOS mouse cursor at zero.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
static void FAR mouse_hide_cursor(void)
{
    --mouseshown;
    if (mouseshown != 0) return;
    msregisterms.ax = MOUSE_FN_HIDE_CURSOR;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Update cached coordinates and move the DOS mouse cursor.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR setmousepos(U16 x, U16 y)
{
    msregisterms.ax = MOUSE_FN_SET_POSITION;
    cursorxposition = x;
    msregisterms.cx = x << mousehorscale;
    msregisterms.dx = y;
    mouse_api_y = y;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Return the current button mask and scaled X and unscaled Y coordinates.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR getmousepos(U16 *ax, U16 *cx, U16 *dx)
{
    msregisterms.ax = MOUSE_FN_GET_POSITION_AND_BUTTONS;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
    *ax = msregisterms.bx;
    *cx = msregisterms.cx >> mousehorscale;
    *dx = msregisterms.dx;
}

/* Set a horizontal DOS mouse cursor range.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR setmousex(I16 xmin, I16 xmax)
{
    msregisterms.ax = MOUSE_FN_SET_HORIZONTAL_RANGE;
    msregisterms.cx = xmin >> mousehorscale;
    msregisterms.dx = xmax >> mousehorscale;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

/* Set a vertical DOS mouse cursor range.
 * Params and return follow the declared C signature; shared state is noted where the body writes it.
 */
void FAR setmousey(U16 ymin, U16 ymax)
{
    msregisterms.ax = MOUSE_FN_SET_VERTICAL_RANGE;
    msregisterms.cx = ymin;
    msregisterms.dx = ymax;
    int86(PLATFORM_DOS_MOUSE_INTERRUPT, &msregisterms, &msregisterms);
}

I16 mouse_button_state_cache = 0;
U16 mousehorscale = 0;
I16 mouseshown = 0;
