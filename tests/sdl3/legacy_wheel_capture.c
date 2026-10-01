#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Point { int16_t x, y; } Point;
typedef struct WheelRect {
    int16_t x0, y0, x1, y1, x2, y2, x3, y3;
} WheelRect;
typedef struct Face {
    int16_t point_count, color;
    Point points[18];
} Face;

static Face faces[19];
static size_t face_count;

void preRender_wheel(WheelRect *, int16_t, int16_t, int16_t, int16_t);

static void save_face(int16_t color, int count, const int16_t *xy)
{
    Face *face;
    int i;
    if (face_count >= 19 || count < 0 || count > 18) {
        fprintf(stderr, "invalid legacy wheel face capture\n");
        exit(2);
    }
    face = &faces[face_count++];
    face->point_count = (int16_t)count;
    face->color = color;
    for (i = 0; i < count; ++i) {
        face->points[i].x = xy[i * 2];
        face->points[i].y = xy[i * 2 + 1];
    }
}

void preRender_default_alt(int16_t color, int16_t count, int8_t *points)
{
    save_face(color, count, (const int16_t *)points);
}

void preRender_wheel_helper4(int16_t color, int16_t count, ...)
{
    int16_t xy[8];
    va_list args;
    int i;
    if (count != 4) {
        fprintf(stderr, "legacy wheel tread was not a quad\n");
        exit(2);
    }
    va_start(args, count);
    for (i = 0; i < 8; ++i)
        xy[i] = (int16_t)va_arg(args, int);
    va_end(args);
    save_face(color, count, xy);
}

int main(int argc, char **argv)
{
    WheelRect rect;
    FILE *out;
    size_t i;
    if (argc != 13) {
        fprintf(stderr, "usage: wheel_capture out x0 y0 x1 y1 x2 y2 x3 y3 tread side ring\n");
        return 2;
    }
    out = fopen(argv[1], "wb");
    if (out == NULL)
        return 2;
    rect.x0 = (int16_t)strtol(argv[2], NULL, 0);
    rect.y0 = (int16_t)strtol(argv[3], NULL, 0);
    rect.x1 = (int16_t)strtol(argv[4], NULL, 0);
    rect.y1 = (int16_t)strtol(argv[5], NULL, 0);
    rect.x2 = (int16_t)strtol(argv[6], NULL, 0);
    rect.y2 = (int16_t)strtol(argv[7], NULL, 0);
    rect.x3 = (int16_t)strtol(argv[8], NULL, 0);
    rect.y3 = (int16_t)strtol(argv[9], NULL, 0);
    preRender_wheel(&rect, 0x2500,
        (int16_t)strtol(argv[10], NULL, 0),
        (int16_t)strtol(argv[11], NULL, 0),
        (int16_t)strtol(argv[12], NULL, 0));
    if (face_count != 19 ||
        fwrite(&(uint16_t){(uint16_t)face_count}, sizeof(uint16_t), 1, out) != 1) {
        fclose(out);
        fprintf(stderr, "unexpected legacy wheel face count %zu\n", face_count);
        return 2;
    }
    for (i = 0; i < face_count; ++i) {
        uint16_t count = (uint16_t)faces[i].point_count;
        if (fwrite(&count, sizeof count, 1, out) != 1 ||
            fwrite(&faces[i].color, sizeof(int16_t), 1, out) != 1 ||
            fwrite(faces[i].points, sizeof(Point), count, out) != count) {
            fclose(out);
            return 2;
        }
    }
    return fclose(out) == 0 ? 0 : 2;
}
