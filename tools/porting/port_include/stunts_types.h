/* PORTING ONLY: NOT PART OF THE MATCHING BUILD. */
#ifndef STUNTS_PORT_TYPES_H
#define STUNTS_PORT_TYPES_H
#include <stdint.h>
#include <stddef.h>
/* Target widths: char 8, int/short 16, long 32. Far/near segment qualifiers
   are documented at API boundaries and erased only for this flat host probe. */
typedef int8_t st_i8;
typedef uint8_t st_u8;
typedef int16_t st_i16;
typedef uint16_t st_u16;
typedef int32_t st_i32;
typedef uint32_t st_u32;
/* 16-bit offset into the target medium-model DGROUP; not a host pointer. */
typedef uint16_t st_near_data_offset;
/* Legacy lowercase aliases used by one target object; PORT_BUILD overlays
   repeat these typedefs with the same fixed-width types. */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#ifndef I8
#define I8 char
#endif
#ifndef I8S
#define I8S signed char
#endif
#ifndef U8
#define U8 uint8_t
#endif
#ifndef I16
#define I16 int16_t
#endif
#ifndef U16
#define U16 uint16_t
#endif
#ifndef I16S
#define I16S int16_t
#endif
#ifndef U16S
#define U16S uint16_t
#endif
#ifndef I32
#define I32 int32_t
#endif
#ifndef U32
#define U32 uint32_t
#endif
#ifndef far
#define far
#endif
#ifndef near
#define near
#endif
#ifndef huge
#define huge
#endif
#ifndef _far
#define _far
#endif
#ifndef _near
#define _near
#endif
#ifndef _huge
#define _huge
#endif
#ifndef FAR
#define FAR far
#endif
#ifndef NEAR
#define NEAR near
#endif
#ifndef HUGE
#define HUGE huge
#endif
#endif
