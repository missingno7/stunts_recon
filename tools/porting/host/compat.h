#ifndef H1_HOST_COMPAT_H
#define H1_HOST_COMPAT_H

/* Scratch host-probe shim. It approximates primitive widths and erases the
   segmented-memory/calling-convention keywords; it does not emulate DOS. */
#include <stdint.h>
#include <stddef.h>

#define I8 char
#define I8S signed char
#define U8 uint8_t
#define I16 int16_t
#define U16 uint16_t
#define I16S int16_t
#define U16S uint16_t
#define I32 int32_t
#define U32 uint32_t

#undef far
#undef near
#undef huge
#undef _far
#undef _near
#undef _huge
#undef cdecl
#undef _cdecl
#undef __cdecl
#undef pascal
#undef _pascal
#undef __pascal
#undef interrupt
#undef _interrupt
#undef __interrupt
#undef loadds
#undef _loadds
#undef __loadds
#undef _fastcall
#undef __fastcall
#define far
#define near
#define huge
#define _far
#define _near
#define _huge
#define cdecl
#define _cdecl
#define __cdecl
#define pascal
#define _pascal
#define __pascal
#define interrupt
#define _interrupt
#define __interrupt
#define loadds
#define _loadds
#define __loadds
#define _fastcall
#define __fastcall

#endif
