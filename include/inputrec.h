/* Shared inputrec declarations; DOS medium-model types. */
#ifndef STUNTS_INPUTREC_H
#define STUNTS_INPUTREC_H
struct MouseRegs;
#pragma pack()
struct MouseRegs {
    unsigned int ax;
    unsigned int bx;
    unsigned int cx;
    unsigned int dx;
    unsigned int si;
    unsigned int di;
    unsigned int cflag;
};
typedef struct MouseRegs MouseRegs;
#endif
