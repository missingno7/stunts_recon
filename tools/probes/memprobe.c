/* Pass-environment probe (tools/pass_environment.py).  Replaces one compiler
   pass through CL's -B1/-B2/-B3 option and records the DOS memory the pass
   receives: its PSP, the end of the free arena above it, and the MCB chain.
   Diagnostic measurement only; it never produces or checks game bytes. */
#include <stdio.h>
#include <dos.h>
#include <stdlib.h>

static unsigned peekw(unsigned seg, unsigned off)
{
    return *(unsigned far *)(((unsigned long)seg << 16) | off);
}

int main(void)
{
    union REGS r;
    struct SREGS s;
    unsigned mcb, n = 0, env = peekw(_psp, 0x2c);
    FILE *f = fopen("MEMPROBE.TXT", "w");
    if (!f) return 2;
    r.h.ah = 0x30;
    intdos(&r, &r);
    fprintf(f, "dos %u.%u\npsp %u\nenv_paragraphs %u\n", r.h.al, r.h.ah, _psp,
            env ? peekw(env - 1, 3) : 0);
    r.h.ah = 0x52;
    intdosx(&r, &r, &s);
    mcb = peekw(s.es, r.x.bx - 2);
    while (n++ < 64) {
        unsigned char sig = *(unsigned char far *)((unsigned long)mcb << 16);
        fprintf(f, "mcb %u %c %u %u\n", mcb, sig, peekw(mcb, 1), peekw(mcb, 3));
        if (sig != 'M') break;
        mcb += peekw(mcb, 3) + 1;
    }
    fclose(f);
    return 3;
}
