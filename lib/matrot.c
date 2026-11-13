#include "types.h"
/* Write fixed-point X, Y, and Z rotation matrices using the fast sine and cosine helpers. */

#define MATRIX_IDENTITY_Q14 0x4000 /* Q14 representation of 1.0. */
#include "math3d.h"

/* Local view: MATRIX; embedded MAT3 record differs from the shared flat nine-word array. */
struct MATRIX { struct MAT3 m; };
#include "trig.h"

/* Write a fixed-point X-axis rotation matrix.
 * Params and return follow the declared C signature. */
void xformx(struct MATRIX *outmat, I16 angle)
{
    I16 s, c;
    c = intcos(angle);
    s = intsin(angle);
    outmat->m._11 = MATRIX_IDENTITY_Q14; outmat->m._21 = 0;  outmat->m._31 = 0;
    outmat->m._12 = 0;      outmat->m._22 = c;  outmat->m._32 = s;
    outmat->m._13 = 0;      outmat->m._23 = -s; outmat->m._33 = c;
}

/* Write a fixed-point Y-axis rotation matrix.
 * Params and return follow the declared C signature. */
void xformy(struct MATRIX *outmat, I16 angle)
{
    I16 s, c;
    c = intcos(angle);
    s = intsin(angle);
    outmat->m._11 = c;  outmat->m._21 = 0;      outmat->m._31 = -s;
    outmat->m._12 = 0;  outmat->m._22 = MATRIX_IDENTITY_Q14; outmat->m._32 = 0;
    outmat->m._13 = s;  outmat->m._23 = 0;      outmat->m._33 = c;
}

/* Write a fixed-point Z-axis rotation matrix.
 * Params and return follow the declared C signature. */
void xformz(struct MATRIX *outmat, I16 angle)
{
    I16 s, c;
    c = intcos(angle);
    s = intsin(angle);
    outmat->m._11 = c;  outmat->m._21 = s; outmat->m._31 = 0;
    outmat->m._12 = -s; outmat->m._22 = c; outmat->m._32 = 0;
    outmat->m._13 = 0;  outmat->m._23 = 0; outmat->m._33 = MATRIX_IDENTITY_Q14;
}
