typedef struct MouseRegs {
    unsigned int ax;
    unsigned int bx;
    unsigned int cx;
    unsigned int dx;
} MouseRegs;

extern MouseRegs regs_x86;
extern unsigned int mousehorscale;
extern int far int86(unsigned int interrupt_no, MouseRegs *input, MouseRegs *output);

void far mouse_get_state(unsigned int *ax, unsigned int *cx, unsigned int *dx)
{
    regs_x86.ax = 3;
    int86(0x33, &regs_x86, &regs_x86);
    *ax = regs_x86.bx;
    *cx = regs_x86.cx >> mousehorscale;
    *dx = regs_x86.dx;
}
