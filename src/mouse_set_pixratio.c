typedef struct MouseRegs {
    unsigned int ax;
    unsigned int bx;
    unsigned int cx;
    unsigned int dx;
} MouseRegs;

extern MouseRegs regs_x86;
extern unsigned int word_45D7C;
extern unsigned int word_44D3C;
extern unsigned int word_44D62;
extern unsigned int mousehorscale;
extern int word_40318;
extern int showmouse;
extern int far int86(unsigned int interrupt_no, MouseRegs *input, MouseRegs *output);
void far mouse_set_pixratio(unsigned int xratio, unsigned int yratio);
void far mouse_set_minmax(unsigned int xmin, unsigned int ymin, unsigned int xmax, unsigned int ymax);

void far mouse_set_pixratio(unsigned int xratio, unsigned int yratio)
{
    regs_x86.ax = 15;
    regs_x86.cx = xratio;
    regs_x86.dx = yratio;
    int86(0x33, &regs_x86, &regs_x86);
}

int far mouse_init(unsigned int width, unsigned int height)
{
    unsigned int status;
    regs_x86.ax = 0xc201;
    int86(0x15, &regs_x86, &regs_x86);
    regs_x86.ax = 0;
    int86(0x33, &regs_x86, &regs_x86);
    status = regs_x86.ax;
    word_45D7C = regs_x86.bx;
    if (status != 0) {
        if (width == 320) mousehorscale = 1;
        else mousehorscale = 0;
        mouse_set_minmax(0, 0, width - 1, height - 1);
        mouse_set_pixratio(16, 16);
        word_40318 = 0xffff;
    }
    return status;
}

void far mouse_set_minmax(unsigned int xmin, unsigned int ymin,
                          unsigned int xmax, unsigned int ymax)
{
    regs_x86.ax = 7;
    regs_x86.cx = xmin << mousehorscale;
    regs_x86.dx = xmax << mousehorscale;
    int86(0x33, &regs_x86, &regs_x86);
    regs_x86.ax = 8;
    regs_x86.cx = ymin;
    regs_x86.dx = ymax;
    int86(0x33, &regs_x86, &regs_x86);
}

static unsigned int far mouse_get_position(void)
{
    regs_x86.ax = 3;
    int86(0x33, &regs_x86, &regs_x86);
    word_45D7C = regs_x86.bx;
    word_44D3C = regs_x86.cx >> mousehorscale;
    word_44D62 = regs_x86.dx;
    return word_45D7C;
}

static void far mouse_show_cursor(void)
{
    ++showmouse;
    if (showmouse < 1) return;
    showmouse = 1;
    regs_x86.ax = 1;
    int86(0x33, &regs_x86, &regs_x86);
}

static void far mouse_hide_cursor(void)
{
    --showmouse;
    if (showmouse != 0) return;
    regs_x86.ax = 2;
    int86(0x33, &regs_x86, &regs_x86);
}
