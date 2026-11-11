#include "modern_wheel.h"

#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { FACE_CAPACITY = 20, FACE_POINT_CAPACITY = 18 };

typedef struct CapturedFace {
    size_t point_count;
    int color;
    ModernWheelPoint points[FACE_POINT_CAPACITY];
} CapturedFace;

typedef struct Capture {
    size_t face_count;
    CapturedFace faces[FACE_CAPACITY];
} Capture;

typedef struct LegacyWheelRect {
    int16_t left, top, section_p1_x, section_p1_y, section_p2_x, section_p2_y, x, y;
} LegacyWheelRect;

static Capture *legacy_capture;

void preRender_wheel(LegacyWheelRect *rect, int16_t count, int16_t color,
                     int16_t style_a, int16_t style_b);

static void capture_polygon(void *user, const ModernWheelPoint *points,
                            size_t point_count, int color)
{
    Capture *capture = (Capture *)user;
    CapturedFace *face;
    size_t i;
    if (capture->face_count >= FACE_CAPACITY ||
        point_count > FACE_POINT_CAPACITY) {
        fprintf(stderr, "capture capacity exceeded\n");
        exit(2);
    }
    face = &capture->faces[capture->face_count++];
    face->point_count = point_count;
    face->color = color;
    for (i = 0; i < point_count; ++i)
        face->points[i] = points[i];
}

/* These two hooks capture the real recovered preRender_wheel.c submissions. */
void preRender_default_alt(int16_t color, int16_t point_count, int8_t *points)
{
    const int16_t *xy = (const int16_t *)points;
    Capture *capture = (Capture *)legacy_capture;
    ModernWheelPoint converted[FACE_POINT_CAPACITY];
    int i;

    if (capture == NULL || point_count < 0 ||
        point_count > FACE_POINT_CAPACITY) {
        fprintf(stderr, "invalid legacy polygon submission\n");
        exit(2);
    }
    for (i = 0; i < point_count; ++i) {
        converted[i].x = xy[i * 2];
        converted[i].y = xy[i * 2 + 1];
    }
    capture_polygon(capture, converted, (size_t)point_count, color);
}

void preRender_wheel_helper4(int16_t color, int16_t point_count, ...)
{
    va_list args;
    ModernWheelPoint converted[4];
    Capture *capture = (Capture *)legacy_capture;
    int i;

    if (capture == NULL || point_count != 4) {
        fprintf(stderr, "invalid legacy wheel quad submission\n");
        exit(2);
    }
    va_start(args, point_count);
    for (i = 0; i < point_count; ++i) {
        converted[i].x = va_arg(args, int);
        converted[i].y = va_arg(args, int);
    }
    va_end(args);
    capture_polygon(capture, converted, (size_t)point_count, color);
}

static int nearly_equal(double a, double b, double tolerance)
{
    return fabs(a - b) <= tolerance;
}

static size_t side_anchor_index(const Capture *capture)
{
    double ax = capture->faces[16].points[0].x;
    double ay = capture->faces[16].points[0].y;
    double best_distance = HUGE_VAL;
    size_t best = 0;
    size_t i;
    for (i = 0; i < 16; ++i) {
        double dx = capture->faces[i].points[0].x - ax;
        double dy = capture->faces[i].points[0].y - ay;
        double distance = dx * dx + dy * dy;
        if (distance < best_distance) {
            best_distance = distance;
            best = i;
        }
    }
    return best;
}

static void assert_true(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        exit(1);
    }
}

static void check_native_structure(void)
{
    const ModernWheelPoint source[4] = {
        {0.25, 0.75}, {8.25, 0.75}, {0.25, 8.75}, {24.25, 3.75}
    };
    Capture modern = {0};
    size_t i;
    size_t emitted = modern_wheel_emit(source, 41, 52, 63,
                                       capture_polygon, &modern);

    assert_true(emitted == 19 && modern.face_count == 19,
                "native wheel emits 16 quads, 2 side faces, then 1 ring");
    for (i = 0; i < 19; ++i) {
        CapturedFace *face = &modern.faces[i];
        size_t j;
        size_t expected_count = i < 16 ? 4 : (i < 18 ? 18 : 16);
        int expected_color = i < 16 ? 41 : (i < 18 ? 52 : 63);
        assert_true(face->point_count == expected_count,
                    "face vertex count and submission order are stable");
        assert_true(face->color == expected_color,
                    "tread, side and ring retain independent material colors");
        for (j = 0; j < face->point_count; ++j)
            assert_true(isfinite(face->points[j].x) &&
                        isfinite(face->points[j].y),
                        "native wheel coordinates are finite");
    }

    assert_true(nearly_equal(modern.faces[0].points[0].x, source[1].x, 1e-12) &&
                nearly_equal(modern.faces[0].points[0].y, source[1].y, 1e-12),
                "first tread begins at recovered ring point zero");
    assert_true(nearly_equal(modern.faces[0].points[1].x,
                             modern.faces[1].points[0].x, 1e-12) &&
                nearly_equal(modern.faces[0].points[1].y,
                             modern.faces[1].points[0].y, 1e-12),
                "adjacent tread quads share their ring vertex");
    assert_true(nearly_equal(modern.faces[0].points[0].x,
                             modern.faces[15].points[1].x, 1e-12) &&
                nearly_equal(modern.faces[0].points[0].y,
                             modern.faces[15].points[1].y, 1e-12),
                "last tread quad closes onto the first ring vertex");
    assert_true(nearly_equal(modern.faces[16].points[0].x,
                             modern.faces[17].points[0].x, 1e-12) &&
                nearly_equal(modern.faces[16].points[0].y,
                             modern.faces[17].points[0].y, 1e-12),
                "both side faces use the same selected top vertex");
    assert_true(nearly_equal(modern.faces[16].points[8].x,
                             modern.faces[17].points[8].x, 1e-12) &&
                nearly_equal(modern.faces[16].points[8].y,
                             modern.faces[17].points[8].y, 1e-12),
                "both side faces meet at the same antipodal vertex");
    assert_true(modern.faces[0].points[0].x == 8.25 &&
                modern.faces[0].points[0].y == 0.75,
                "input keeps its subpixel coordinates without early quantization");
}

static void run_legacy_comparison(const int16_t values[8], double *max_error,
                                  size_t *stable_side_count,
                                  size_t *subpixel_anchor_ties)
{
    LegacyWheelRect rect;
    ModernWheelPoint source[4];
    Capture legacy = {0};
    Capture modern = {0};
    size_t i;
    size_t j;

    rect.left = values[0];
    rect.top = values[1];
    rect.section_p1_x = values[2];
    rect.section_p1_y = values[3];
    rect.section_p2_x = values[4];
    rect.section_p2_y = values[5];
    rect.x = values[6];
    rect.y = values[7];
    source[0] = (ModernWheelPoint){values[0], values[1]};
    source[1] = (ModernWheelPoint){values[2], values[3]};
    source[2] = (ModernWheelPoint){values[4], values[5]};
    source[3] = (ModernWheelPoint){values[6], values[7]};

    legacy_capture = &legacy;
    preRender_wheel(&rect, 0x2500, 41, 52, 63);
    legacy_capture = NULL;
    assert_true(legacy.face_count == 19,
                "faithful helper emits 16 quads, 2 side faces and 1 ring");
    assert_true(modern_wheel_emit(source, 41, 52, 63,
                                 capture_polygon, &modern) == 19,
                "modern adapter emits all corresponding faces");
    assert_true(modern.face_count == legacy.face_count,
                "faithful and native face order has equal length");

    if (side_anchor_index(&modern) == side_anchor_index(&legacy)) {
        ++*stable_side_count;
    } else {
        double native_anchor_y = modern.faces[16].points[0].y;
        double legacy_anchor_y = legacy.faces[16].points[0].y;
        /* The faithful helper chooses its seam from integer projected rows.
           A subpixel minimum can choose a neighboring point in the same
           reference raster row. The emitted coordinates remain unquantized. */
        assert_true(floor(native_anchor_y + 0.5) == legacy_anchor_y,
                    "changed side seam is confined to an equal reference raster row");
        ++*subpixel_anchor_ties;
    }

    for (i = 0; i < legacy.face_count; ++i) {
        assert_true(modern.faces[i].point_count == legacy.faces[i].point_count,
                    "faithful and native corresponding face sizes match");
        assert_true(modern.faces[i].color == legacy.faces[i].color,
                    "faithful and native corresponding material colors match");
        if ((i == 16 || i == 17) &&
            side_anchor_index(&modern) != side_anchor_index(&legacy))
            continue;
        for (j = 0; j < legacy.faces[i].point_count; ++j) {
            double dx = fabs(modern.faces[i].points[j].x -
                             legacy.faces[i].points[j].x);
            double dy = fabs(modern.faces[i].points[j].y -
                             legacy.faces[i].points[j].y);
            if (dx > *max_error) *max_error = dx;
            if (dy > *max_error) *max_error = dy;
        }
    }
}

int main(void)
{
    static const int16_t fixtures[][8] = {
        {110, 100, 121, 102, 112, 112, 137, 104},
        {81, 119, 68, 123, 84, 102, 99, 117},
        {240, 130, 250, 138, 229, 140, 225, 110},
        {20, 187, 29, 184, 16, 179, 39, 198},
        {161, 74, 166, 62, 176, 77, 148, 90},
        {282, 166, 273, 158, 295, 151, 304, 177}
    };
    double max_error = 0.0;
    size_t stable_side_count = 0;
    size_t subpixel_anchor_ties = 0;
    size_t i;

    check_native_structure();
    for (i = 0; i < sizeof(fixtures) / sizeof(fixtures[0]); ++i)
        run_legacy_comparison(fixtures[i], &max_error, &stable_side_count,
                              &subpixel_anchor_ties);

    /* Each legacy Q14 multiply rounds to a whole pixel. Inputs here are
       identical integers, so stable-anchor vertices differ only due to
       staged fixed-point rounding. Integer min-Y ties may choose a
       neighboring side-face seam and are measured separately. */
    assert_true(max_error <= 2.0,
                "native ring and aligned side points stay within 2 px of faithful construction");
    assert_true(stable_side_count > 0 && subpixel_anchor_ties > 0,
                "comparison covers both stable and subpixel-tied side anchors");
    printf("wheel adapter: analytic structure passed; 6 faithful fixtures, "
           "max non-tied component error %.6f px (bound 2 px), "
           "%zu stable anchors, %zu subpixel anchor ties\n",
           max_error, stable_side_count, subpixel_anchor_ties);
    return 0;
}
