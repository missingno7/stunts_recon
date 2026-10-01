#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *file_read_nofatal(const char *filename, void *destination);
const char *file_find_next(void);
int file_write_fatal(const char *filename, const void *source,
                     unsigned long length);

static uint8_t *s_bounded;
static size_t s_bounded_size;
static unsigned s_fatal_count;

int port_memory_extent(const void *pointer, size_t *remaining_out)
{
    uintptr_t address = (uintptr_t)pointer;
    uintptr_t start = (uintptr_t)s_bounded;
    if (pointer == NULL || s_bounded == NULL || address < start ||
        address - start >= s_bounded_size)
        return 0;
    if (remaining_out != NULL)
        *remaining_out = s_bounded_size - (size_t)(address - start);
    return 1;
}

void *port_memory_alloc(size_t size, const char *owner, PortFarPtr *address_out)
{
    (void)owner;
    if (address_out != NULL)
        memset(address_out, 0, sizeof(*address_out));
    return malloc(size);
}

void port_memory_free(void *pointer)
{
    free(pointer);
}

void fatal_error(const char *format, ...)
{
    (void)format;
    ++s_fatal_count;
}

static int path_join(char *output, size_t capacity,
                     const char *directory, const char *name)
{
    int length = snprintf(output, capacity, "%s/%s", directory, name);
    return length >= 0 && (size_t)length < capacity;
}

static int write_host_file(const char *path, const void *data, size_t size)
{
    FILE *file = fopen(path, "wb");
    int ok;
    if (file == NULL)
        return 0;
    ok = fwrite(data, 1u, size, file) == size;
    if (fclose(file) != 0)
        ok = 0;
    return ok;
}

static int read_host_file(const char *path, void *data, size_t capacity,
                          size_t *size_out)
{
    FILE *file = fopen(path, "rb");
    size_t size;
    int ok;
    if (file == NULL)
        return 0;
    size = fread(data, 1u, capacity, file);
    ok = !ferror(file) && fgetc(file) == EOF;
    if (fclose(file) != 0)
        ok = 0;
    if (size_out != NULL)
        *size_out = size;
    return ok;
}

static int check(int condition, const char *message)
{
    if (!condition)
        fprintf(stderr, "FAIL: %s\n", message);
    return condition;
}

static int equal_case(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        char a = *left >= 'A' && *left <= 'Z' ?
                 (char)(*left + ('a' - 'A')) : *left;
        char b = *right >= 'A' && *right <= 'Z' ?
                 (char)(*right + ('a' - 'A')) : *right;
        if (a != b)
            return 0;
        ++left;
        ++right;
    }
    return *left == '\0' && *right == '\0';
}

int main(int argc, char **argv)
{
    static const uint8_t asset_data[] = "immutable asset";
    static const uint8_t duplicate_asset[] = "old high score";
    static const uint8_t unique_asset[] = "another high score";
    static const uint8_t saved_override[] = "new high score";
    static const uint8_t track_fixture[] = "track asset";
    static const uint8_t exact_extent_data[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
    static uint8_t large_data[40001];
    static uint8_t read_buffer[40032];
    const char *found;
    char asset_root[1024];
    char save_root[1024];
    char case_directory[1024];
    char duplicate_asset_path[1024];
    char asset_path[1024];
    char saved_path[1024];
    char nested_path[1024];
    char first[1024];
    char listed[4][1024];
    size_t index;
    size_t asset_size = 0;
    int count = 0;
    int ok = 1;

    if (argc != 2 || !path_join(asset_root, sizeof(asset_root), argv[1], "assets") ||
        !path_join(save_root, sizeof(save_root), argv[1], "saves") ||
        !path_join(case_directory, sizeof(case_directory), asset_root, "CaseDir") ||
        !path_join(duplicate_asset_path, sizeof(duplicate_asset_path), asset_root, "Dup.HIG") ||
        !path_join(saved_path, sizeof(saved_path), save_root, "dup.hig") ||
        !path_join(nested_path, sizeof(nested_path), save_root, "nested/new.rpl")) {
        fprintf(stderr, "invalid file-service probe directory\n");
        return 2;
    }

    ok &= check(SDL_CreateDirectory(asset_root), "create asset root");
    ok &= check(SDL_CreateDirectory(case_directory), "create nested asset directory");
    ok &= check(write_host_file(duplicate_asset_path, duplicate_asset,
                                sizeof(duplicate_asset)),
                "write duplicate asset fixture");
    ok &= check(path_join(asset_path, sizeof(asset_path), asset_root, "Alpha.TXT") &&
                write_host_file(asset_path, asset_data, sizeof(asset_data)),
                "write case-lookup asset fixture");
    ok &= check(path_join(asset_path, sizeof(asset_path), asset_root, "Unique.HIG") &&
                write_host_file(asset_path, unique_asset, sizeof(unique_asset)),
                "write unique search fixture");
    ok &= check(path_join(asset_path, sizeof(asset_path), case_directory, "Mixed.RPL") &&
                write_host_file(asset_path, asset_data, sizeof(asset_data)),
                "write nested case-lookup fixture");
    ok &= check(path_join(asset_path, sizeof(asset_path), asset_root, "Track01.trk") &&
                write_host_file(asset_path, track_fixture, sizeof(track_fixture)),
                "write representable DOS track fixture");
    ok &= check(path_join(asset_path, sizeof(asset_path), asset_root, "LongTrack.trk") &&
                write_host_file(asset_path, track_fixture, sizeof(track_fixture)),
                "write long native track fixture");
    ok &= check(path_join(asset_path, sizeof(asset_path), asset_root, "Short.track") &&
                write_host_file(asset_path, track_fixture, sizeof(track_fixture)),
                "write long-extension fixture");
    if (!ok)
        return 1;

    port_runtime_set_asset_root(asset_root);
    s_bounded = read_buffer;
    s_bounded_size = sizeof(read_buffer);
    ok &= check(file_read_nofatal("alpha.txt", read_buffer) == read_buffer,
                "case-insensitive nofatal read returns original pointer");
    ok &= check(memcmp(read_buffer, asset_data, sizeof(asset_data)) == 0,
                "read returns complete asset bytes");
    ok &= check(file_read_nofatal("casedir\\mixed.rpl", read_buffer) == read_buffer,
                "DOS separators and case-insensitive nested lookup");
    ok &= check(file_read_nofatal("missing.rpl", read_buffer) == NULL,
                "nofatal read reports a missing file");
    ok &= check(port_fs_exists("longtrack.trk") && port_fs_exists("short.track"),
                "literal opens still reach explicit native long filenames");
    ok &= check(port_fs_find("*.TRK", first, sizeof(first)) &&
                equal_case(first, "Track01.trk") && file_find_next() == NULL,
                "wildcard results fit the DOS 8.3 filename buffer");

    for (index = 0; index < sizeof(large_data); ++index)
        large_data[index] = (uint8_t)(index * 37u + 11u);
    ok &= check(file_write_fatal("dup.hig", saved_override,
                                 (unsigned long)sizeof(saved_override)) == 0,
                "write save into the sibling save root");
    ok &= check(file_write_fatal("large.rpl", large_data,
                                 (unsigned long)sizeof(large_data)) == 0,
                "write across multiple 16 KiB DOS chunks");
    ok &= check(file_write_fatal("nested/new.rpl", exact_extent_data,
                                 (unsigned long)sizeof(exact_extent_data)) == 0,
                "create nested save directories");
    ok &= check(!port_fs_exists("../assets/large.rpl"),
                "asset-root traversal is rejected");
    ok &= check(port_fs_exists("DUP.HIG"), "saved file is visible to reads");
    memset(read_buffer, 0, sizeof(read_buffer));
    ok &= check(file_read_nofatal("DUP.HIG", read_buffer) == read_buffer &&
                memcmp(read_buffer, saved_override, sizeof(saved_override)) == 0,
                "saved file shadows an immutable asset case-insensitively");
    ok &= check(file_read_nofatal("large.rpl", read_buffer) == read_buffer &&
                memcmp(read_buffer, large_data, sizeof(large_data)) == 0,
                "large saved file round-trips all chunked bytes");
    ok &= check(read_host_file(duplicate_asset_path, read_buffer,
                               sizeof(read_buffer), &asset_size) &&
                asset_size == sizeof(duplicate_asset) &&
                memcmp(read_buffer, duplicate_asset, sizeof(duplicate_asset)) == 0,
                "asset tree remains unchanged after writing saves");
    ok &= check(read_host_file(saved_path, read_buffer, sizeof(read_buffer), NULL) &&
                memcmp(read_buffer, saved_override, sizeof(saved_override)) == 0,
                "save is outside the immutable asset directory");

    s_bounded = read_buffer;
    s_bounded_size = sizeof(exact_extent_data);
    ok &= check(file_read_nofatal("nested/new.rpl", read_buffer) == read_buffer &&
                memcmp(read_buffer, exact_extent_data, sizeof(exact_extent_data)) == 0,
                "bounded read accepts a file that exactly fills its destination");
    s_bounded_size = sizeof(read_buffer);

    ok &= check(port_fs_find("*.HIG", first, sizeof(first)),
                "wildcard search finds the first save-or-asset entry");
    memcpy(listed[count++], first, strlen(first) + 1u);
    while ((found = file_find_next()) != NULL && count < 4) {
        ok &= check(found == first, "find-next reuses the first-result path buffer");
        memcpy(listed[count++], found, strlen(found) + 1u);
    }
    ok &= check(count == 2, "search merges save and asset roots without duplicates");
    if (count == 2) {
        int saw_duplicate = 0;
        int saw_unique = 0;
        for (index = 0; index < (size_t)count; ++index) {
            if (equal_case(listed[index], "dup.hig"))
                saw_duplicate = 1;
            if (equal_case(listed[index], "unique.hig"))
                saw_unique = 1;
        }
        ok &= check(saw_duplicate && saw_unique,
                    "search includes the saved override and unsaved asset once each");
    }
    ok &= check(file_find_next() == NULL, "find-next ends after the last result");
    ok &= check(!port_fs_find("missing-*.rpl", first, sizeof(first)) &&
                file_find_next() == NULL,
                "failed first search invalidates prior enumeration");

    s_bounded_size = 8u;
    memset(read_buffer, 0xA5, sizeof(read_buffer));
    ok &= check(file_read_nofatal("large.rpl", read_buffer) == NULL,
                "bounded read rejects a file larger than its destination");
    ok &= check(read_buffer[8] == 0xA5,
                "bounded read never writes beyond managed destination");
    file_read_fatal("missing.rpl", read_buffer);
    ok &= check(s_fatal_count == 1u, "fatal read reports errors through fatal_error");

    s_bounded = NULL;
    s_bounded_size = 0;
    ok &= check(file_write_fatal("../escape.hig", saved_override,
                                 (unsigned long)sizeof(saved_override)) == -1,
                "invalid save path reports a fatal write failure");
    ok &= check(s_fatal_count == 2u, "write failure follows fatal-error path");
    ok &= check(!port_fs_exists("escape.hig"), "invalid save path creates no file");

    if (!ok)
        return 1;
    puts("SDL3 file service checks passed");
    return 0;
}
