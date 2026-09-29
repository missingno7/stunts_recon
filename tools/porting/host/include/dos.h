#ifndef H1_HOST_DOS_H
#define H1_HOST_DOS_H
#include <stdint.h>
union REGS {
    struct { uint16_t ax, bx, cx, dx, si, di, cflag, flags; } x;
    struct { uint8_t al, ah, bl, bh, cl, ch, dl, dh; } h;
};
struct SREGS { uint16_t es, cs, ss, ds; };
int int86(int interrupt_no, union REGS *in, union REGS *out);
int int86x(int interrupt_no, union REGS *in, union REGS *out, struct SREGS *seg);
void *getvect(int interrupt_no);
void setvect(int interrupt_no, void (*handler)(void));
#endif
