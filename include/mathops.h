#ifndef STUNTS_MATHOPS_H
#define STUNTS_MATHOPS_H
#include "stunts_types.h"
struct VECTOR;
struct MATRIX;
struct POINT2D;
extern I16 FAR fixedmult(I16, I16);
extern I16 FAR intatan(I16, I16);
extern I16 FAR int3dhypot(struct VECTOR *);
extern void FAR mat_vec(struct VECTOR *, struct MATRIX *, struct VECTOR *);
extern void FAR mat_mul_vector2(struct VECTOR *, struct MATRIX FAR *, struct VECTOR *);
extern void FAR transmult(struct MATRIX *, struct MATRIX *, struct MATRIX *);
extern void FAR mat_invert(struct MATRIX *, struct MATRIX *);
extern void FAR xformx(struct MATRIX *, I16);
extern void FAR xformy(struct MATRIX *, I16);
extern void FAR xformz(struct MATRIX *, I16);
extern struct MATRIX * FAR makematrix(I16, I16, I16, I16);
extern I16 FAR norm_zone(struct VECTOR *);
extern void FAR projectp(struct VECTOR *, struct POINT2D *);
extern void FAR zclipline(struct VECTOR *, struct VECTOR *, struct VECTOR *, I16S);
#endif
