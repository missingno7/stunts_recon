/* Shared mathapi declarations; unnamed prototype parameters keep C1 allocation stable. */
#ifndef STUNTS_MATHAPI_H
#define STUNTS_MATHAPI_H
#include "radius2d.h"
struct MATRIX;
struct VECTOR;
extern struct MATRIX g_matrix_yrot;
extern struct VECTOR g_op_carvector2;
extern char g_vector_bitix;
extern struct MATRIX matrix_transform_view;
extern struct MATRIX matrix_x_rotation;
extern void far INIT_MATRIX(void);
extern void far shellsort(int, int *, int *);
#endif
