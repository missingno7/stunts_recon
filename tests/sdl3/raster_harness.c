#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../port/sprite.c"
#include "../../port/sincos.c"

static uint8_t test_video[PORT_VIDEO_MEMORY_BYTES];

uint8_t *port_video_pixels(void)
{
    return test_video;
}

void port_video_publish(const char *reason)
{
    (void)reason;
}

void port_guest_unwind(const char *symbol)
{
    fprintf(stderr, "unexpected guest unwind: %s\n", symbol);
    abort();
}

int port_memory_extent(const void *pointer, size_t *remaining_out)
{
    (void)pointer;
    (void)remaining_out;
    return 0;
}

void *mmgr_alloc_pages(const char *name, uint16_t paragraphs)
{
    (void)name;
    return calloc((size_t)paragraphs, 16u);
}

void mmgr_free(void *pointer)
{
    free(pointer);
}

int port_sprite_active_view(uint8_t **pixels_out, size_t *extent_out,
                            PortSprite **sprite_out);
void preRender_patterned(int16_t pattern_bits, int16_t color,
                         int16_t point_count, const int16_t *points);
void preRender_unk(int16_t pattern_bits, int16_t background_color,
                   int16_t foreground_color, int16_t point_count,
                   const int16_t *points);
void preRender_sphere(int16_t center_x, int16_t center_y, int16_t size,
                      int16_t color);
unsigned draw_line_related(unsigned x0, unsigned y0, unsigned x1,
                           unsigned y1, int *record);
unsigned draw_line_related_alt(unsigned x0, unsigned y0, unsigned x1,
                               unsigned y1, int *record);

static void require_pixel(int x, int y, uint8_t expected, const char *case_name)
{
    uint8_t actual = test_video[(size_t)y * PORT_SCREEN_WIDTH + (size_t)x];
    if (actual != expected) {
        fprintf(stderr, "%s: pixel (%d,%d) expected %u, got %u\n",
                case_name, x, y, (unsigned)expected, (unsigned)actual);
        exit(1);
    }
}

static int16_t parse_i16(const char *text)
{
    return (int16_t)strtol(text, NULL, 0);
}

static int write_output(const char *path, const void *bytes, size_t length)
{
    FILE *stream = fopen(path, "wb");
    int ok;
    if (stream == NULL)
        return 0;
    ok = fwrite(bytes, 1u, length, stream) == length;
    if (fclose(stream) != 0)
        ok = 0;
    return ok;
}

static int run_dump(int argc, char **argv)
{
    const char *operation = argv[1];
    const char *output_path = argv[argc - 1];
    int16_t coordinates[64];
    int16_t record[14] = { 0 };
    uint8_t record_output[sizeof(uint16_t) + sizeof(record)];
    uint16_t line_result;
    int16_t count;
    int i;

    port_sprite_init();
    memset(test_video, 0, sizeof(test_video));
    if (strcmp(operation, "default") == 0 ||
        strcmp(operation, "pattern") == 0 ||
        strcmp(operation, "unknown") == 0 ||
        strcmp(operation, "wheel") == 0 ||
        strcmp(operation, "alt") == 0 ||
        strcmp(operation, "skybox") == 0) {
        int arg = 2;
        int16_t color;
        if (strcmp(operation, "pattern") == 0) {
            int16_t pattern = parse_i16(argv[arg++]);
            color = parse_i16(argv[arg++]);
            count = parse_i16(argv[arg++]);
            if (count <= 0 || count > 32)
                return 2;
            for (i = 0; i < count * 2; ++i)
                coordinates[i] = parse_i16(argv[arg++]);
            preRender_patterned(pattern, color, count, coordinates);
        } else if (strcmp(operation, "unknown") == 0) {
            int16_t pattern = parse_i16(argv[arg++]);
            int16_t background = parse_i16(argv[arg++]);
            int16_t foreground = parse_i16(argv[arg++]);
            count = parse_i16(argv[arg++]);
            if (count <= 0 || count > 32)
                return 2;
            for (i = 0; i < count * 2; ++i)
                coordinates[i] = parse_i16(argv[arg++]);
            preRender_unk(pattern, background, foreground, count, coordinates);
        } else if (strcmp(operation, "wheel") == 0) {
            color = parse_i16(argv[arg++]);
            count = parse_i16(argv[arg++]);
            if (count != 4)
                return 2;
            for (i = 0; i < count * 2; ++i)
                coordinates[i] = parse_i16(argv[arg++]);
            preRender_wheel_helper4(color, count,
                                    coordinates[0], coordinates[1],
                                    coordinates[2], coordinates[3],
                                    coordinates[4], coordinates[5],
                                    coordinates[6], coordinates[7]);
        } else if (strcmp(operation, "alt") == 0) {
            color = parse_i16(argv[arg++]);
            count = parse_i16(argv[arg++]);
            if (count <= 0 || count > 32)
                return 2;
            for (i = 0; i < count * 2; ++i)
                coordinates[i] = parse_i16(argv[arg++]);
            preRender_default_alt(color, count, coordinates);
        } else if (strcmp(operation, "skybox") == 0) {
            color = parse_i16(argv[arg++]);
            count = parse_i16(argv[arg++]);
            if (count != 4)
                return 2;
            for (i = 0; i < 8; ++i)
                coordinates[i] = parse_i16(argv[arg++]);
            skybox_op_helper((uint16_t)color, (uint16_t)count,
                             (RasterPoint){ coordinates[0], coordinates[1] },
                             (RasterPoint){ coordinates[2], coordinates[3] },
                             (RasterPoint){ coordinates[4], coordinates[5] },
                             (RasterPoint){ coordinates[6], coordinates[7] });
        } else {
            color = parse_i16(argv[arg++]);
            count = parse_i16(argv[arg++]);
            if (count <= 0 || count > 32)
                return 2;
            for (i = 0; i < count * 2; ++i)
                coordinates[i] = parse_i16(argv[arg++]);
            preRender_default(color, count, coordinates);
        }
        return write_output(output_path, test_video,
                            PORT_SCREEN_WIDTH * PORT_SCREEN_HEIGHT) ? 0 : 3;
    }
    if (strcmp(operation, "sphere") == 0) {
        preRender_sphere(parse_i16(argv[2]), parse_i16(argv[3]),
                         parse_i16(argv[4]), parse_i16(argv[5]));
        return write_output(output_path, test_video,
                            PORT_SCREEN_WIDTH * PORT_SCREEN_HEIGHT) ? 0 : 3;
    }
    if (strcmp(operation, "spherepoints") == 0) {
        int16_t source[6];
        int16_t points[64];
        for (i = 0; i < 6; ++i)
            source[i] = parse_i16(argv[2 + i]);
        sphere_build_ring(source, points);
        return write_output(output_path, points, sizeof(points)) ? 0 : 3;
    }
    if (strcmp(operation, "line") == 0) {
        preRender_line(parse_i16(argv[2]), parse_i16(argv[3]),
                       parse_i16(argv[4]), parse_i16(argv[5]),
                       parse_i16(argv[6]));
        return write_output(output_path, test_video,
                            PORT_SCREEN_WIDTH * PORT_SCREEN_HEIGHT) ? 0 : 3;
    }
    if (strcmp(operation, "record") == 0) {
        line_result = (uint16_t)draw_line_related(
            (unsigned)parse_i16(argv[2]), (unsigned)parse_i16(argv[3]),
            (unsigned)parse_i16(argv[4]), (unsigned)parse_i16(argv[5]),
            (int *)record);
        memcpy(record_output, &line_result, sizeof(line_result));
        memcpy(record_output + sizeof(line_result), record, sizeof(record));
        return write_output(output_path, record_output,
                            sizeof(record_output)) ? 0 : 3;
    }
    return 4;
}

int main(int argc, char **argv)
{
    static const int16_t square[] = { 1, 2, 4, 2, 4, 4, 1, 4 };
    uint8_t *active_pixels = NULL;
    size_t active_extent = 0;
    PortSprite *active_sprite = NULL;
    int16_t record[14] = { 0 };

    if (argc > 1)
        return run_dump(argc, argv);

    port_sprite_init();
    if (!port_sprite_active_view(&active_pixels, &active_extent,
                                 &active_sprite) ||
        active_pixels != test_video || active_extent != PORT_VIDEO_MEMORY_BYTES ||
        active_sprite == NULL)
        return 2;

    memset(test_video, 0, sizeof(test_video));
    preRender_patterned((int16_t)0xaa55, 17, 4, square);
    require_pixel(1, 2, 0, "pattern even first row");
    require_pixel(2, 2, 17, "pattern even first row");
    require_pixel(3, 2, 0, "pattern even first row");
    require_pixel(4, 2, 17, "pattern even first row");
    require_pixel(1, 3, 17, "pattern odd second row");
    require_pixel(2, 3, 0, "pattern odd second row");
    require_pixel(3, 3, 17, "pattern odd second row");
    require_pixel(4, 3, 0, "pattern odd second row");
    require_pixel(1, 4, 0, "pattern even third row");
    require_pixel(2, 4, 17, "pattern even third row");

    memset(test_video, 0, sizeof(test_video));
    preRender_unk((int16_t)0xffff, 7, 19, 4, square);
    require_pixel(1, 2, 19, "secondary material color");
    require_pixel(4, 4, 19, "secondary material color");

    memset(test_video, 0, sizeof(test_video));
    preRender_wheel_helper4(23, 4,
                            6, 7, 9, 7, 9, 10, 6, 10);
    require_pixel(6, 7, 23, "wheel varargs corner");
    require_pixel(9, 10, 23, "wheel varargs opposite corner");

    memset(test_video, 0, sizeof(test_video));
    preRender_sphere(10, 10, 4, 5);
    require_pixel(8, 9, 5, "small sphere upper profile");
    require_pixel(7, 10, 5, "small sphere widest profile");
    require_pixel(13, 10, 5, "small sphere widest profile");
    require_pixel(8, 11, 5, "small sphere lower profile");
    require_pixel(7, 9, 0, "small sphere upper extent");

    memset(test_video, 0, sizeof(test_video));
    preRender_sphere(80, 90, 100, 9);
    require_pixel(80, 130, 9, "large sphere effective vertical radius");
    require_pixel(63, 52, 9, "large sphere upper DDA boundary");
    require_pixel(97, 52, 9, "large sphere upper DDA boundary");
    require_pixel(57, 126, 0, "large sphere excludes terminal edge sample");
    require_pixel(58, 126, 9, "large sphere terminal edge vertex");
    require_pixel(67, 129, 0, "large sphere lower DDA boundary");
    require_pixel(68, 129, 9, "large sphere lower DDA boundary");
    require_pixel(75, 130, 9, "large sphere bottom scanline");

    memset(test_video, 0, sizeof(test_video));
    skybox_op_helper(23, 4, (RasterPoint){ 6, 7 }, (RasterPoint){ 9, 7 },
                     (RasterPoint){ 9, 10 }, (RasterPoint){ 6, 10 });
    require_pixel(6, 7, 23, "skybox helper first corner");
    require_pixel(9, 10, 23, "skybox helper opposite corner");

    if (draw_line_related((unsigned)(int16_t)-5, 10, 5, 10,
                          (int *)record) != 0u ||
        record[1] != 0 || record[3] != 10 || record[4] != 5 ||
        record[5] != 10)
        return 3;
    if (draw_line_related(10, 10, 20, 20, (int *)record) != 0u ||
        record[1] != 10 || record[3] != 10 || record[4] != 20 ||
        record[5] != 20 || record[7] != 11 || record[9] != 4)
        return 5;
    if (draw_line_related((unsigned)(int16_t)-8, 1,
                          (unsigned)(int16_t)-2, 8,
                          (int *)record) == 0u)
        return 4;

    memset(record, 0, sizeof(record));
    if (draw_line_related(333, 88, 164, (unsigned)(int16_t)-21,
                          (int *)record) != 0u ||
        record[1] != 197 || record[2] != 18621 ||
        record[3] != 0 || record[4] != 319 || record[5] != 79 ||
        record[6] != (int16_t)42269 || record[7] != 123 ||
        record[9] != 8 || record[13] != 9)
        return 6;

    memset(record, 0, sizeof(record));
    if (draw_line_related_alt((unsigned)(int16_t)-10, 20, 30, 25,
                              (int *)record) != 0u ||
        record[1] != -10 || record[3] != 20 ||
        record[4] != 30 || record[5] != 25 ||
        record[7] != 41 || record[9] != 8)
        return 8;

    memset(record, 0, sizeof(record));
    if (draw_line_related((unsigned)(int16_t)-9, 222, 268, 187,
                          (int *)record) != 0u ||
        record[1] != 268 || record[3] != 187 ||
        record[4] != 173 || record[5] != 199 ||
        record[7] != 96 || record[9] != 7)
        return 7;

    memset(test_video, 0, sizeof(test_video));
    preRender_line(333, 88, 164, (int16_t)-21, 5);
    require_pixel(196, 0, 0, "top/right clipped descriptor start");
    require_pixel(197, 0, 5, "top/right clipped descriptor start");
    require_pixel(319, 79, 5, "top/right clipped descriptor end");
    require_pixel(320, 79, 0, "top/right clipped descriptor extent");

    memset(test_video, 0, sizeof(test_video));
    preRender_line((int16_t)-9, 222, 268, 187, 5);
    require_pixel(172, 199, 0, "bottom clipped line sample before endpoint");
    require_pixel(173, 199, 5, "bottom clipped line endpoint");
    require_pixel(176, 199, 5, "bottom clipped line endpoint span");
    require_pixel(177, 199, 0, "bottom clipped line sample after endpoint");

    return 0;
}
