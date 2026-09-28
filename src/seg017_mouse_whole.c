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

void far mouse_set_pixratio(unsigned int xratio, unsigned int yratio)
{
    msregisterms.ax = 15;
    msregisterms.cx = xratio;
    msregisterms.dx = yratio;
    int86(0x33, &msregisterms, &msregisterms);
}

int far mouse_init(unsigned int width, unsigned int height)
{
    unsigned int status;
    msregisterms.ax = 0xc201;
    int86(0x15, &msregisterms, &msregisterms);
    msregisterms.ax = 0;
    int86(0x33, &msregisterms, &msregisterms);
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

void far mouse_set_minmax(unsigned int xmin, unsigned int ymin,
                          unsigned int xmax, unsigned int ymax)
{
    msregisterms.ax = 7;
    msregisterms.cx = xmin << mousehorscale;
    msregisterms.dx = xmax << mousehorscale;
    int86(0x33, &msregisterms, &msregisterms);
    msregisterms.ax = 8;
    msregisterms.cx = ymin;
    msregisterms.dx = ymax;
    int86(0x33, &msregisterms, &msregisterms);
}

static unsigned int far mouse_get_position(void)
{
    msregisterms.ax = 3;
    int86(0x33, &msregisterms, &msregisterms);
    ms_buttons = msregisterms.bx;
    cursorxposition = msregisterms.cx >> mousehorscale;
    mouse_api_y = msregisterms.dx;
    return ms_buttons;
}

static void far mouse_show_cursor(void)
{
    ++showmouse;
    if (showmouse < 1) return;
    showmouse = 1;
    msregisterms.ax = 1;
    int86(0x33, &msregisterms, &msregisterms);
}

static void far mouse_hide_cursor(void)
{
    --showmouse;
    if (showmouse != 0) return;
    msregisterms.ax = 2;
    int86(0x33, &msregisterms, &msregisterms);
}

void far mouse_set_position(unsigned int x, unsigned int y)
{
    msregisterms.ax = 4;
    cursorxposition = x;
    msregisterms.cx = x << mousehorscale;
    msregisterms.dx = y;
    mouse_api_y = y;
    int86(0x33, &msregisterms, &msregisterms);
}

void far mouse_get_state(unsigned int *ax, unsigned int *cx, unsigned int *dx)
{
    msregisterms.ax = 3;
    int86(0x33, &msregisterms, &msregisterms);
    *ax = msregisterms.bx;
    *cx = msregisterms.cx >> mousehorscale;
    *dx = msregisterms.dx;
}

void far nopsub_36A9A(int xmin, int xmax)
{
    msregisterms.ax = 7;
    msregisterms.cx = xmin >> mousehorscale;
    msregisterms.dx = xmax >> mousehorscale;
    int86(0x33, &msregisterms, &msregisterms);
}

void far nopsub_36ACA(unsigned int ymin, unsigned int ymax)
{
    msregisterms.ax = 8;
    msregisterms.cx = ymin;
    msregisterms.dx = ymax;
    int86(0x33, &msregisterms, &msregisterms);
}

int mouse_button_state_cache = 0;
unsigned int mousehorscale = 0;
int showmouse = 0;
