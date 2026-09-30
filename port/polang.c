#include "port_runtime.h"

static const uint8_t atantable[257] = {
    0, 1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 10,
    10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 16, 17, 18, 18, 19, 20,
    20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 28, 29, 30,
    30, 31, 31, 32, 33, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 39,
    40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 46, 47, 48, 48, 49,
    49, 50, 51, 51, 52, 52, 53, 53, 54, 55, 55, 56, 56, 57, 57, 58,
    58, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 65, 65, 66, 66, 67,
    67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75,
    76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83,
    84, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91,
    91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98,
    98, 99, 99, 99, 100, 100, 101, 101, 102, 102, 102, 103, 103, 104, 104, 104,
    105, 105, 106, 106, 106, 107, 107, 108, 108, 108, 109, 109, 110, 110, 110, 111,
    111, 112, 112, 112, 113, 113, 113, 114, 114, 115, 115, 115, 116, 116, 116, 117,
    117, 118, 118, 118, 119, 119, 119, 120, 120, 120, 121, 121, 121, 122, 122, 122,
    123, 123, 123, 124, 124, 124, 125, 125, 125, 126, 126, 126, 127, 127, 127, 128,
    128,
};

/* _atantable[0..256] and the register-width division/quadrant dispatch in
   asm/graphics_resource_runtime.ASM::_polang. */

static uint16_t negate_word(uint16_t value)
{
    return (uint16_t)(0u - value);
}

int16_t polang(int16_t z_arg, int16_t x_arg)
{
    uint16_t z = (uint16_t)z_arg;
    uint16_t x = (uint16_t)x_arg;
    uint16_t di = 0;
    uint16_t ax = 0;
    uint16_t index;
    uint32_t dividend;
    uint16_t quotient;

    if ((int16_t)z < 0) { di |= 8; z = negate_word(z); }
    if ((int16_t)x < 0) { di |= 4; x = negate_word(x); }
    if ((int16_t)z < (int16_t)x) {
        /* Keep z/x. */
    } else if (z == x) {
        if (z != 0) ax = 0x80;
        goto quadrant_dispatch;
    } else {
        uint16_t swap = z; z = x; x = swap; di |= 2;
    }
    /* For (0, 0), the DOS routine dispatches its incoming AX unchanged.
       AX is not an ABI result there, so this port chooses deterministic zero. */
    if (x == 0) goto quadrant_dispatch;
    dividend = (uint32_t)z << 16;
    quotient = (uint16_t)(dividend / x);
    index = (uint16_t)(quotient >> 8);
    if ((quotient & 0xffu) >= 0x80u) ++index;
    ax = atantable[index];
quadrant_dispatch:
    /* DI is a byte offset into the assembly's word jump table, so its
       reachable values are the even values 0 through 14. */
    switch (di) {
    case 0: break;
    case 2: ax = (uint16_t)(negate_word(ax) + 0x100u); break;
    case 4: ax = negate_word(ax); break;
    case 6: ax = (uint16_t)(ax + 0x100u); break;
    case 8: ax = (uint16_t)(negate_word(ax) + 0x200u); break;
    case 10: ax = negate_word(ax); break;
    case 12: ax = (uint16_t)(ax - 0x100u); break;
    case 14: ax = negate_word((uint16_t)(ax + 0x100u)); break;
    default: break;
    }
    return (int16_t)ax;
}

static uint32_t quartered_word_dividend(uint16_t component)
{
    uint16_t high = component;
    uint16_t low = 0;
    unsigned shift;

    /* Reproduce SAR DX,1 / RCR AX,1 twice with DX:AX initially component:0. */
    for (shift = 0; shift < 2u; ++shift) {
        uint16_t carry = (uint16_t)(high & 1u);
        uint16_t sign = (uint16_t)(high & 0x8000u);
        high = (uint16_t)((high >> 1) | sign);
        low = (uint16_t)((low >> 1) | (uint16_t)(carry << 15));
    }
    return ((uint32_t)high << 16) | low;
}

static uint16_t polradius2d_divide(uint16_t component, uint16_t scale)
{
    uint32_t dividend = quartered_word_dividend(component);
    uint32_t quotient;

    if (scale == 0)
        port_guest_unwind("polradius2d divide by zero");
    quotient = dividend / scale;
    if (quotient > UINT16_MAX)
        port_guest_unwind("polradius2d unsigned divide overflow");
    return (uint16_t)quotient;
}

/* Semantic translation of asm/polarRadius2D.ASM. Its `polang`, sinfast, and
   cosfast calls retain the recovered word ABI; context.py does not currently
   index this name as a strict historical function extent. */
uint16_t polradius2d(int16_t z_arg, int16_t y_arg)
{
    uint16_t angle = (uint16_t)polang(z_arg, y_arg);
    uint16_t component;
    uint16_t scale;

    if ((int16_t)angle < 0)
        angle = (uint16_t)(0u - angle);
    if ((int16_t)angle >= 0x0100)
        angle = (uint16_t)(0u - (uint16_t)(angle - 0x0200));

    if ((int16_t)angle <= 0x0080) {
        scale = (uint16_t)cosfast(angle);
        component = (uint16_t)y_arg;
        if ((int16_t)component < 0)
            component = (uint16_t)(0u - component);
    } else {
        scale = (uint16_t)sinfast(angle);
        component = (uint16_t)z_arg;
        if ((int16_t)component < 0)
            component = (uint16_t)(0u - component);
    }
    return polradius2d_divide(component, scale);
}

/* The accepted PORT_BUILD objects refer to the original mixed-case C symbol.
   Keep the lower-case host helper for existing adapters while presenting the
   game's 32-bit host-int ABI and preserving its signed 16-bit AX result. */
int polarRadius2D(int z_arg, int y_arg)
{
    return (int)(int16_t)polradius2d((int16_t)z_arg, (int16_t)y_arg);
}
