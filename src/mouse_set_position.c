typedef struct MouseRegs {
    unsigned int ax;
    unsigned int bx;
    unsigned int cx;
    unsigned int dx;
} MouseRegs;

extern MouseRegs regs_x86;
extern unsigned int mousehorscale;
extern unsigned int word_44D3C;
extern unsigned int word_44D62;
extern int far int86(unsigned int interrupt_no, MouseRegs *input, MouseRegs *output);

void far mouse_set_position(unsigned int x, unsigned int y)
{
    regs_x86.ax = 4;
    word_44D3C = x;
    regs_x86.cx = x << mousehorscale;
    regs_x86.dx = y;
    word_44D62 = y;
    int86(0x33, &regs_x86, &regs_x86);
}
