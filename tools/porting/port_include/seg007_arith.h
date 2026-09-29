/* PORTING ONLY: NOT PART OF THE MATCHING BUILD. */
/* PORT_BUILD model for the two original 8086 arithmetic blocks in obj_seg007.c.
   unsigned DX:AX DIV r/m16 raises #DE for divisor 0 or quotient > 0xffff. */
#ifndef X2_SEG007_ARITH_H
#define X2_SEG007_ARITH_H
#include <stdint.h>
extern void abort(void); /* C library function; declared here to avoid pulling host libc into the target API header. */

static inline uint16_t stunts_div_u32_u16(uint32_t dividend, uint16_t divisor)
{
    uint32_t quotient;
    if (divisor == 0)
        abort(); /* 8086 #DE */
    quotient = dividend / divisor;
    if (quotient > UINT16_MAX)
        abort(); /* 8086 #DE: quotient does not fit AX */
    return (uint16_t)quotient;
}
#endif
