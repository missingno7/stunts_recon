#include "port_runtime.h"

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int16_t projection_x_scale;

typedef struct PolarCase {
    int16_t z;
    int16_t x;
    uint16_t expected;
    const char *name;
} PolarCase;

typedef struct RatioCase {
    uint16_t scale;
    uint16_t factor;
    uint16_t divisor;
    uint16_t expected;
    const char *name;
} RatioCase;

/* Expected words were read from the locked load_1ea4e / load_2275c
   routines with the original DATA frame and retained as focused regression
   evidence; these are values, not copied oracle code or image bytes. */
static const PolarCase polar_cases[] = {
    { 1,  1, 0x0080, "equal positive"},
    { 1, -1, 0x0180, "positive z, negative x"},
    {-1,  1, 0xff80, "negative z, positive x"},
    {-1, -1, 0xfe80, "equal negative"},
    { 1,  2, 0x004c, "ratio below one"},
    { 2,  1, 0x00b4, "swapped ratio"},
    { 1, -2, 0x01b4, "quadrant 4"},
    { 2, -1, 0x014c, "quadrant 3"},
    {-1,  2, 0xffb4, "quadrant 2"},
    {-2,  1, 0xff4c, "quadrant 1 negative"},
    {-1, -2, 0xfe4c, "quadrant 6"},
    {-2, -1, 0xfeb4, "quadrant 7"},
    { 0,  7, 0x0000, "positive x axis"},
    { 7,  0, 0x0100, "positive z axis"},
    { 0, -7, 0x0200, "negative x axis"},
    {-7,  0, 0xff00, "negative z axis"},
    {0x100,  0x101, 0x0080, "near equal ratio"},
    {0x101,  0x100, 0x0080, "near equal swapped"},
    {0x100, -0x101, 0x0180, "near equal quadrant 4"},
    {0x101, -0x100, 0x0180, "near equal quadrant 3"},
    {-0x100,  0x101, 0xff80, "near equal quadrant 2"},
    {-0x101,  0x100, 0xff80, "near equal quadrant 1"},
    {-0x100, -0x101, 0xfe80, "near equal quadrant 6"},
    {-0x101, -0x100, 0xfe80, "near equal quadrant 7"},
};

static const RatioCase ratio_cases[] = {
    {0x0064, 0x0003, 0x0005, 0x003c, "simple multiply then divide"},
    {0x1000, 0x0001, 0x0001, 0x1000, "identity"},
    {0x2000, 0xffff, 0x2000, 0xffff, "unsigned factor word"},
    {0x3fff, 0x0101, 0x0100, 0x403e, "fractional quotient"},
    {0x1234, 0x0456, 0x0789, 0x0a79, "uneven ratio"},
};

static jmp_buf unwind_target;
static int expect_unwind;
static const char *unwind_symbol;

void port_guest_unwind(const char *symbol)
{
    unwind_symbol = symbol;
    if (expect_unwind)
        longjmp(unwind_target, 1);
    fprintf(stderr, "unexpected legacy arithmetic trap: %s\n", symbol);
    abort();
}

static int polang_unwinds(int16_t z, int16_t x, const char *expected_symbol)
{
    expect_unwind = 1;
    if (setjmp(unwind_target) == 0) {
        (void)polang(z, x);
        expect_unwind = 0;
        return 0;
    }
    expect_unwind = 0;
    return strcmp(unwind_symbol, expected_symbol) == 0;
}

static int ratio_unwinds(uint16_t scale, uint16_t factor, uint16_t divisor,
                         const char *expected_symbol)
{
    projection_x_scale = (int16_t)scale;
    expect_unwind = 1;
    if (setjmp(unwind_target) == 0) {
        (void)projectiondata9_times_ratio((int16_t)factor,
                                          (int16_t)divisor);
        expect_unwind = 0;
        return 0;
    }
    expect_unwind = 0;
    return strcmp(unwind_symbol, expected_symbol) == 0;
}

int main(void)
{
    size_t i;

    for (i = 0; i < sizeof(polar_cases) / sizeof(polar_cases[0]); ++i) {
        const PolarCase *test = &polar_cases[i];
        uint16_t actual = (uint16_t)polang(test->z, test->x);
        if (actual != test->expected) {
            fprintf(stderr, "polang %s (%d,%d): expected %04x, got %04x\n",
                    test->name, test->z, test->x, test->expected, actual);
            return 1;
        }
    }

    for (i = 0; i < sizeof(ratio_cases) / sizeof(ratio_cases[0]); ++i) {
        const RatioCase *test = &ratio_cases[i];
        uint16_t actual;
        projection_x_scale = (int16_t)test->scale;
        actual = (uint16_t)projectiondata9_times_ratio(
            (int16_t)test->factor, (int16_t)test->divisor);
        if (actual != test->expected) {
            fprintf(stderr,
                    "projection ratio %s (%04x*%04x/%04x): expected %04x, got %04x\n",
                    test->name, test->scale, test->factor, test->divisor,
                    test->expected, actual);
            return 1;
        }
    }

    if (!polang_unwinds((int16_t)0x8000, 0,
                        "polarAngle divide by zero")) {
        fprintf(stderr, "polang must preserve the original zero-divisor trap\n");
        return 1;
    }
    if (!polang_unwinds((int16_t)0x8000, 1,
                        "polarAngle unsigned divide overflow")) {
        fprintf(stderr, "polang must preserve the original quotient-overflow trap\n");
        return 1;
    }
    if (!ratio_unwinds(0x1000, 1, 0,
                       "projectiondata9_times_ratio divide by zero")) {
        fprintf(stderr, "projection ratio must preserve the original zero-divisor trap\n");
        return 1;
    }
    if (!ratio_unwinds(0xffff, 0xffff, 1,
                       "projectiondata9_times_ratio unsigned divide overflow")) {
        fprintf(stderr, "projection ratio must preserve the original quotient-overflow trap\n");
        return 1;
    }

    return 0;
}
