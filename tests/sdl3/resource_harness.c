#include "../../port/resource.c"

#include <assert.h>
#include <setjmp.h>
#undef assert
#define assert(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "check failed at %s:%d: %s\n", \
                __FILE__, __LINE__, #condition); \
        exit(1); \
    } \
} while (0)

static uint8_t shape_storage[16u + 64u];
static uint8_t surface[64u * 16u];
static uint16_t line_offsets[16];
static PortShape2D screen_shape;
static PortSprite active_sprite;
static unsigned publication_count;
static unsigned free_count;
static jmp_buf expected_unwind;
static int catch_unwind;
static const char *unwind_message;
static uint8_t resource_file_data[64];
static size_t resource_file_size;
static const char *resource_file_name;
static uint8_t managed_resource_data[64];
static unsigned managed_allocation_count;
static const void *extent_buffers[2];
static size_t extent_buffer_sizes[2];

int port_memory_extent(const void *pointer, size_t *remaining_out)
{
    uintptr_t p = (uintptr_t)pointer;
    uintptr_t base = (uintptr_t)shape_storage;
    if (p >= base && p - base < sizeof(shape_storage)) {
        if (remaining_out != NULL)
            *remaining_out = sizeof(shape_storage) - (size_t)(p - base);
        return 1;
    }
    {
        unsigned i;
        for (i = 0; i < 2u; ++i) {
            base = (uintptr_t)extent_buffers[i];
            if (extent_buffers[i] != NULL && p >= base &&
                p - base < extent_buffer_sizes[i]) {
                if (remaining_out != NULL)
                    *remaining_out = extent_buffer_sizes[i] -
                                     (size_t)(p - base);
                return 1;
            }
        }
    }
    return 0;
}

int port_sprite_active_view(uint8_t **pixels, size_t *extent,
                            PortSprite **sprite)
{
    if (pixels != NULL) *pixels = surface;
    if (extent != NULL) *extent = sizeof(surface);
    if (sprite != NULL) *sprite = &active_sprite;
    return 1;
}

uint8_t *port_video_pixels(void) { return surface; }
void port_video_publish(const char *reason)
{
    (void)reason;
    ++publication_count;
}
void port_memory_free(void *pointer)
{
    (void)pointer;
    ++free_count;
}
void *port_fs_load(const char *path, size_t *length, PortFarPtr *address)
{
    (void)address;
    if (resource_file_name == NULL || path == NULL ||
        strcmp(path, resource_file_name) != 0)
        return NULL;
    if (length != NULL)
        *length = resource_file_size;
    return resource_file_data;
}
void *port_memory_alloc(size_t size, const char *owner, PortFarPtr *address)
{
    (void)owner;
    (void)address;
    if (size > sizeof(managed_resource_data))
        return NULL;
    ++managed_allocation_count;
    return managed_resource_data;
}
int port_fs_find(const char *pattern, char *found, size_t capacity)
{
    (void)pattern;
    (void)found;
    (void)capacity;
    return 0;
}
void *file_load_shape2d_fatal(char *name)
{
    (void)name;
    return NULL;
}
void *file_load_shape2d_nofatal(char *name)
{
    (void)name;
    return NULL;
}
void *file_load_shape2d_res_nofatal(char *name)
{
    (void)name;
    return NULL;
}
void port_guest_unwind(const char *reason)
{
    if (catch_unwind) {
        unwind_message = reason;
        catch_unwind = 0;
        longjmp(expected_unwind, 1);
    }
    abort();
}

static void clear_surface(void)
{
    unsigned y;
    memset(surface, 0, sizeof(surface));
    memset(&screen_shape, 0, sizeof(screen_shape));
    screen_shape.width = 64;
    screen_shape.height = 16;
    for (y = 0; y < 16; ++y)
        line_offsets[y] = (uint16_t)(y * 64u);
    memset(&active_sprite, 0, sizeof(active_sprite));
    active_sprite.sprite_bitmapptr = &screen_shape;
    active_sprite.lineofs = line_offsets;
    active_sprite.words2[0] = 0;
    active_sprite.words2[1] = 64;
    active_sprite.words2[2] = 0;
    active_sprite.words2[3] = 16;
    active_sprite.words2[4] = 64;
    active_sprite.words2[7] = 0;
    active_sprite.words2[8] = 64;
}

static uint8_t *make_shape(uint16_t width, uint16_t height,
                           uint16_t hot_x, uint16_t hot_y,
                           uint16_t pos_x, uint16_t pos_y,
                           const uint8_t *pixels)
{
    size_t count = (size_t)width * height;
    memset(shape_storage, 0, sizeof(shape_storage));
    shape_storage[0] = (uint8_t)width;
    shape_storage[1] = (uint8_t)(width >> 8);
    shape_storage[2] = (uint8_t)height;
    shape_storage[3] = (uint8_t)(height >> 8);
    shape_storage[4] = (uint8_t)hot_x;
    shape_storage[5] = (uint8_t)(hot_x >> 8);
    shape_storage[6] = (uint8_t)hot_y;
    shape_storage[7] = (uint8_t)(hot_y >> 8);
    shape_storage[8] = (uint8_t)pos_x;
    shape_storage[9] = (uint8_t)(pos_x >> 8);
    shape_storage[10] = (uint8_t)pos_y;
    shape_storage[11] = (uint8_t)(pos_y >> 8);
    memcpy(shape_storage + 16u, pixels, count);
    return shape_storage;
}

static void make_archive(uint8_t *archive, size_t size)
{
    memset(archive, 0, size);
    archive[0] = (uint8_t)size;
    archive[1] = (uint8_t)(size >> 8);
    archive[4] = 1;
    /* count=1 gives a 14-byte directory; the one empty chunk starts there. */
    if (size >= 18u)
        archive[14] = archive[15] = archive[16] = archive[17] = 0;
}

static void make_rle_p3s(const uint8_t *archive, size_t archive_size)
{
    size_t i;
    resource_file_data[0] = 1;
    resource_file_data[1] = (uint8_t)archive_size;
    resource_file_data[2] = (uint8_t)(archive_size >> 8);
    resource_file_data[3] = (uint8_t)(archive_size >> 16);
    resource_file_data[4] = 0;
    resource_file_data[5] = 0;
    resource_file_data[6] = 0;
    resource_file_data[7] = 0;
    resource_file_data[8] = 0x81;
    resource_file_data[9] = 0xfe;
    for (i = 0; i < archive_size; ++i)
        resource_file_data[10u + i] = archive[i];
    resource_file_size = 10u + archive_size;
}

static void test_archive_spans(void)
{
    uint8_t archive[32] = {0};
    /* One 3D chunk: 4-byte header, zero vertices, zero primitives. */
    archive[0] = 18;
    archive[4] = 1;
    archive[14] = 0;
    archive[15] = 0;
    assert(validate_shape3d_archive(archive, 18));
    assert(!validate_shape3d_archive(archive, 17));

    /* A declared primitive must have both cull records inside the chunk. */
    archive[14] = 0;
    archive[15] = 1;
    archive[16] = 0;
    archive[17] = 0;
    assert(!validate_shape3d_archive(archive, 25));

    /* Reject unsupported primitive records before shape pointers are published. */
    memset(archive, 0, sizeof(archive));
    archive[4] = 1;
    archive[14] = 0;
    archive[15] = 1;
    archive[26] = 16;
    assert(!validate_shape3d_archive(archive, 28));
}

static void test_transparent_and_release_blits(void)
{
    static const uint8_t pixels[] = { 5, 0xff, 7, 8 };
    uint8_t *shape;
    clear_surface();
    shape = make_shape(2, 2, 0, 0, 3, 2, pixels);
    sprite_putimage_transparent(shape, -1, 0);
    assert(surface[0] == 0);
    assert(surface[1] == 0);
    assert(surface[64] == 8);
    assert(surface[65] == 0);

    /* The separate release entry writes its whole positioned shape row; its
       original machine body does not apply the active viewport rectangle. */
    active_sprite.words2[0] = 10;
    active_sprite.words2[1] = 20;
    active_sprite.words2[2] = 10;
    active_sprite.words2[3] = 12;
    release_shape_resources(shape);
    assert(surface[2u * 64u + 3u] == 5);
    assert(surface[2u * 64u + 4u] == 0);
    assert(surface[3u * 64u + 3u] == 7);
    assert(surface[3u * 64u + 4u] == 8);
    assert(free_count == 0);
    assert(publication_count == 2);
}

static void test_mutable_incnums_map(void)
{
    uint8_t identity[256];
    uint8_t full_map[256];
    static const uint8_t partial_map[] = { 18, 19 };
    static const uint8_t clipped_pixels[] = { 5, 6, 0xff, 7 };
    static const uint8_t partial_pixels[] = { 7, 8, 5 };
    uint8_t *shape;
    unsigned i;

    for (i = 0; i < sizeof(identity); ++i) {
        identity[i] = (uint8_t)i;
        assert(s_incnums[i] == (uint8_t)i);
    }
    memcpy(full_map, identity, sizeof(full_map));
    full_map[5] = 42;
    full_map[6] = 0xff;
    full_map[0xff] = 77;
    sub_35DC8(full_map);

    clear_surface();
    active_sprite.words2[0] = 0;
    active_sprite.words2[1] = 3;
    shape = make_shape(4, 1, 0, 0, 0, 0, clipped_pixels);
    sprite_putimage_transparent(shape, 0, 0);
    assert(surface[0] == 42);
    assert(surface[1] == 0);  /* map[6] == transparent sentinel */
    assert(surface[2] == 77); /* map[0xff] is drawable after remapping */
    assert(surface[3] == 0);  /* right-edge clipping */

    sub_35DE6(7, (uint16_t)sizeof(partial_map), partial_map);
    clear_surface();
    shape = make_shape(3, 1, 0, 0, 0, 0, partial_pixels);
    sprite_putimage_transparent(shape, 0, 1);
    assert(surface[64] == 18);
    assert(surface[65] == 19);
    assert(surface[66] == 42);

    /* The original initializer is restored for following blitter checks. */
    sub_35DC8(identity);
    sub_35DE6(256, 0, NULL); /* zero-byte REP MOVSB does not dereference SI */
    catch_unwind = 1;
    if (setjmp(expected_unwind) == 0) {
        sub_35DE6(255, 2, partial_map);
        assert(!"out-of-span incnums copy must unwind");
    }
    assert(strcmp(unwind_message,
                  "incnums partial mapping exceeds its 256-byte span") == 0);
    assert(s_incnums[255] == 255); /* rejected before mutating any table byte */
}

static void test_scaled_blit(void)
{
    static const uint8_t pixels[] = {
         1,  2,  3,  4,
         5,  6,  7,  8,
         9, 10, 11, 12,
        13, 14, 15, 16,
    };
    uint8_t *shape;
    clear_surface();
    shape = make_shape(4, 4, 0, 0, 0, 0, pixels);
    shapeexpl(128, shape, 4, 3);
    assert(surface[3u * 64u + 4u] == 6);
    assert(surface[3u * 64u + 5u] == 8);
    assert(surface[4u * 64u + 4u] == 14);
    assert(surface[4u * 64u + 5u] == 16);

    clear_surface();
    shape = make_shape(4, 4, 2, 2, 0, 0, pixels);
    shapeexpl(128, shape, 0, 0);
    assert(surface[0] == 16);
    assert(surface[1] == 0);
    assert(surface[64] == 0);
}

static void test_resource_ownership_and_validation(void)
{
    uint8_t archive[18];
    unsigned before;
    make_archive(archive, sizeof(archive));
    resource_file_name = "empty.3sh";
    resource_file_size = sizeof(archive);
    memcpy(resource_file_data, archive, sizeof(archive));
    before = free_count;
    assert(file_load_binary_nofatal(resource_file_name) == resource_file_data);
    assert(free_count == before);

    resource_file_name = "bad.3sh";
    resource_file_size = 17;
    memcpy(resource_file_data, archive, resource_file_size);
    before = free_count;
    assert(file_load_binary_nofatal(resource_file_name) == NULL);
    assert(free_count == before + 1u);

    make_rle_p3s(archive, sizeof(archive));
    resource_file_name = "empty.p3s";
    before = free_count;
    assert(file_decomp(resource_file_name, 0) == managed_resource_data);
    assert(free_count == before + 1u);
    assert(managed_allocation_count == 1u);

    make_archive(archive, 17);
    make_rle_p3s(archive, 17);
    resource_file_name = "bad.p3s";
    before = free_count;
    assert(file_decomp(resource_file_name, 0) == NULL);
    assert(free_count == before + 1u);
    assert(managed_allocation_count == 1u);
}

static int emit_blit_image_set(const char *path)
{
    static const uint8_t alpha_pixels[] = { 5, 0xff, 7, 8 };
    static const uint8_t scaled_pixels[] = {
         1,  2,  3,  4,
         5,  6,  7,  8,
         9, 10, 11, 12,
        13, 14, 15, 16,
    };
    uint8_t image_set[4u * PORT_VIDEO_MEMORY_BYTES];
    uint8_t *shape;
    FILE *output;
    int ok;

    clear_surface();
    shape = make_shape(2, 2, 0, 0, 0, 0, alpha_pixels);
    sprite_putimage_transparent(shape, -1, 0);
    memcpy(image_set, surface, sizeof(surface));

    clear_surface();
    shape = make_shape(2, 2, 0, 0, 3, 2, alpha_pixels);
    release_shape_resources(shape);
    memcpy(image_set + sizeof(surface), surface, sizeof(surface));

    clear_surface();
    shape = make_shape(4, 4, 0, 0, 0, 0, scaled_pixels);
    shapeexpl(128, shape, 4, 3);
    memcpy(image_set + 2u * sizeof(surface), surface, sizeof(surface));

    clear_surface();
    shape = make_shape(4, 4, 2, 2, 0, 0, scaled_pixels);
    shapeexpl(128, shape, 0, 0);
    memcpy(image_set + 3u * sizeof(surface), surface, sizeof(surface));

    output = fopen(path, "wb");
    if (output == NULL)
        return 0;
    ok = fwrite(image_set, 1u, sizeof(image_set), output) == sizeof(image_set);
    if (fclose(output) != 0)
        ok = 0;
    return ok;
}

static int emit_incnums(const char *path)
{
    FILE *output = fopen(path, "wb");
    int ok;
    if (output == NULL)
        return 0;
    ok = fwrite(s_incnums, 1u, sizeof(s_incnums), output) ==
         sizeof(s_incnums);
    if (fclose(output) != 0)
        ok = 0;
    return ok;
}

static int transform_shape_archive(const char *kind, const char *input_path,
                                   const char *output_path, int compressed)
{
    FILE *input = NULL;
    FILE *output = NULL;
    long input_size;
    uint8_t *encoded = NULL;
    uint8_t *decoded = NULL;
    uint8_t *scratch = NULL;
    size_t decoded_size = 0;
    size_t max_pixels = 0;
    size_t archive_extent;
    uint16_t count;
    uint16_t index;
    int ok = 0;

    input = fopen(input_path, "rb");
    if (input == NULL || fseek(input, 0, SEEK_END) != 0 ||
        (input_size = ftell(input)) <= 0 ||
        fseek(input, 0, SEEK_SET) != 0)
        goto done;
    encoded = (uint8_t *)malloc((size_t)input_size);
    if (encoded == NULL || fread(encoded, 1u, (size_t)input_size, input) !=
                           (size_t)input_size)
        goto done;
    if (compressed) {
        if (!port_resource_decompress(encoded, (size_t)input_size,
                                      &decoded, &decoded_size))
            goto done;
    } else {
        decoded = (uint8_t *)malloc((size_t)input_size);
        if (decoded == NULL)
            goto done;
        memcpy(decoded, encoded, (size_t)input_size);
        decoded_size = (size_t)input_size;
    }
    extent_buffers[0] = decoded;
    extent_buffer_sizes[0] = decoded_size;
    if (!archive_view(decoded, &archive_extent, &count, NULL))
        goto done;

    for (index = 0; index < count; ++index) {
        uint8_t *shape;
        size_t shape_size;
        size_t pixels;
        if (!shape_entry_span(decoded, index, &shape, &shape_size) ||
            shape_size < 16u)
            goto done;
        pixels = (size_t)read_u16(shape) * read_u16(shape + 2u);
        if (pixels > max_pixels)
            max_pixels = pixels;
    }
    if (max_pixels == 0u)
        max_pixels = 1u;
    scratch = (uint8_t *)malloc(max_pixels);
    if (scratch == NULL)
        goto done;
    extent_buffers[1] = scratch;
    extent_buffer_sizes[1] = max_pixels;

    if (strcmp(kind, "pvs") == 0)
        file_unflip_shape2d(decoded, scratch);
    else if (strcmp(kind, "pes") == 0)
        file_unflip_shape2d_pes(decoded, scratch);
    else
        goto done;

    output = fopen(output_path, "wb");
    if (output == NULL || fwrite(decoded, 1u, decoded_size, output) !=
                           decoded_size)
        goto done;
    ok = 1;

done:
    extent_buffers[0] = extent_buffers[1] = NULL;
    extent_buffer_sizes[0] = extent_buffer_sizes[1] = 0;
    if (output != NULL && fclose(output) != 0)
        ok = 0;
    if (input != NULL)
        fclose(input);
    free(scratch);
    free(decoded);
    free(encoded);
    if (!ok)
        fprintf(stderr, "shape archive transform failed: %s\n", input_path);
    return ok;
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        if (argc == 3 && strcmp(argv[1], "--blit-dump") == 0)
            return emit_blit_image_set(argv[2]) ? 0 : 2;
        if (argc == 3 && strcmp(argv[1], "--incnums-dump") == 0)
            return emit_incnums(argv[2]) ? 0 : 2;
        if (argc == 4 && (strcmp(argv[1], "--unflip-pvs") == 0 ||
                          strcmp(argv[1], "--unflip-pes") == 0))
            return transform_shape_archive(argv[1] + 9, argv[2], argv[3], 1) ?
                   0 : 2;
        if (argc == 4 && strcmp(argv[1], "--unflip-pvs-raw") == 0)
            return transform_shape_archive("pvs", argv[2], argv[3], 0) ?
                   0 : 2;
        FILE *file = fopen(argv[1], "rb");
        long file_size;
        uint8_t *encoded;
        uint8_t *decoded = NULL;
        size_t decoded_size = 0;
        int ok;
        if (file == NULL || fseek(file, 0, SEEK_END) != 0 ||
            (file_size = ftell(file)) < 0 || fseek(file, 0, SEEK_SET) != 0)
            return 2;
        encoded = (uint8_t *)malloc((size_t)file_size);
        if (encoded == NULL || fread(encoded, 1, (size_t)file_size, file) !=
                               (size_t)file_size)
            return 2;
        fclose(file);
        ok = port_resource_decompress(encoded, (size_t)file_size,
                                      &decoded, &decoded_size) &&
             validate_shape3d_archive(decoded, decoded_size);
        free(encoded);
        free(decoded);
        if (!ok) {
            fprintf(stderr, "invalid P3S archive: %s\n", argv[1]);
            return 1;
        }
        return 0;
    }
    test_archive_spans();
    test_transparent_and_release_blits();
    test_mutable_incnums_map();
    test_scaled_blit();
    test_resource_ownership_and_validation();
    return 0;
}
