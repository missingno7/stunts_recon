/* Fixed-point arithmetic, rotations and projection in DOS word coordinates. */
#ifndef MATH3D_H
#define MATH3D_H
#include "vec3.h"
#include "matrix.h"
struct POINT2D;
#include "trig.h"
#include "mathops.h"
#include "radius.h"
/* norm_zone widens this result by zero extension in the locked code. */

/* Named values with reviewed widths and meanings. */
#define ANGLE_EIGHTH_TURN 0x80 /* I16, 128 */
#define ANGLE_HALF_TURN 0x200 /* I16, 512 */
#define ANGLE_NEGATIVE_EIGHTH_TURN_U16 0xff80 /* U16, 65408 */
#define ANGLE_QUADRANT_OFFSET_MASK 0xff /* I16, 255 */
#define ANGLE_QUARTER_TURN 0x100 /* I16, 256 */
#define ANGLE_THREE_QUARTERS_TURN 0x300 /* I16, 768 */
#define ANGLE_TURN_MASK 0x03ff /* I16, 1023 */
#define ANGLE_UNITS_PER_TURN 0x400 /* I16, 1024 */
#define ANGLE_ZERO 0 /* I16, 0 */
#define HEADING_OCTANT_SHIFT 7 /* I16, 7 */
#define MATRIX_ELEMENT_COUNT 9 /* I16, 9 */
#define MATRIX_IDENTITY_Q14 0x4000 /* I16, 16384 */
#define MATRIX_Y_ROTATION_CACHE_INVALID 0xFFFF /* U16, 65535 */
#define PLANE_NORMAL_DOT_DIVISOR 0x2000 /* I16, 8192 */
#define Q8_FRACTION_BITS 8 /* I16, 8 */
#define RING_HALF_COMPONENT_SHIFT 1 /* I16, 1 */
#define RING_WEIGHT_DIAGONAL_Q14 0x2d41 /* I16, 11585 */
#define RING_WEIGHT_HALF_AXIS_Q14 0x393e /* I16, 14654 */
#define RING_WEIGHT_QUARTER_AXIS_Q14 0x3e17 /* I16, 15895 */
#define RING_WEIGHT_THREE_QUARTER_AXIS_Q14 0x3333 /* I16, 13107 */
#define ROTATION_COMPOSE_REVERSED 1 /* I16, 1 */
#define ROTATION_PRESENT_X 2 /* I16, 2 */
#define ROTATION_PRESENT_Y 1 /* I16, 1 */
#define ROTATION_PRESENT_Z 4 /* I16, 4 */
#define WORLD_COORDINATE_SCALE 64L /* I32, 64 */
#define WORLD_COORDINATE_SCALE_I16 64 /* I16, 64 */
#define WORLD_COORDINATE_SHIFT 6 /* I16, 6 */
#endif
