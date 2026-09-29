#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT_FILE_HANDLES 64u
#define PORT_PATH_BYTES 1024u

static FILE *s_files[PORT_FILE_HANDLES];
static char s_asset_root[PORT_PATH_BYTES] = "build/sdl3/runtime/assets";

static int safe_relative_path(const char *path, char *out, size_t capacity)
{
    size_t i;
    size_t n = 0;
    if (path == NULL || path[0] == '\0' || capacity < 2)
        return 0;
    if (path[0] == '/' || path[0] == '\\' ||
        (path[0] != '\0' && path[1] == ':'))
        return 0;
    for (i = 0; path[i] != '\0'; ++i) {
        char c = path[i] == '\\' ? '/' : path[i];
        if (c == '.' && (i == 0 || path[i - 1] == '/' || path[i - 1] == '\\') &&
            (path[i + 1] == '/' || path[i + 1] == '\\' || path[i + 1] == '.'))
            return 0;
        if (n + 1 >= capacity)
            return 0;
        out[n++] = c;
    }
    out[n] = '\0';
    return 1;
}

static FILE *open_asset(const char *path)
{
    char relative[PORT_PATH_BYTES];
    char absolute[PORT_PATH_BYTES * 2u];
    int n;
    FILE *file;
    if (!safe_relative_path(path, relative, sizeof(relative)))
        return NULL;
    n = snprintf(absolute, sizeof(absolute), "%s/%s", s_asset_root, relative);
    if (n < 0 || (size_t)n >= sizeof(absolute))
        return NULL;
    file = fopen(absolute, "rb");
#ifdef _WIN32
    if (file == NULL) {
        char *p;
        for (p = absolute; *p != '\0'; ++p) {
            if (*p == '/')
                *p = '\\';
        }
        file = fopen(absolute, "rb");
    }
#endif
    return file;
}

void port_runtime_set_asset_root(const char *root)
{
    if (root == NULL || root[0] == '\0')
        return;
    strncpy(s_asset_root, root, sizeof(s_asset_root) - 1u);
    s_asset_root[sizeof(s_asset_root) - 1u] = '\0';
}

const char *port_runtime_asset_root(void)
{
    return s_asset_root;
}

int port_fs_open_read(const char *path)
{
    FILE *file = open_asset(path);
    size_t i;
    if (file == NULL)
        return -1;
    for (i = 0; i < PORT_FILE_HANDLES; ++i) {
        if (s_files[i] == NULL) {
            s_files[i] = file;
            return (int)i + 1;
        }
    }
    fclose(file);
    return -1;
}

int port_fs_exists(const char *path)
{
    int handle = port_fs_open_read(path);
    if (handle < 0)
        return 0;
    port_fs_close(handle);
    return 1;
}

static int dos_pattern_match(const char *pattern, const char *name)
{
    const char *star = NULL;
    const char *retry = NULL;
    while (*name != '\0') {
        unsigned char p = (unsigned char)*pattern;
        unsigned char n = (unsigned char)*name;
        if (p == '?' || (p != '\0' && tolower(p) == tolower(n))) {
            ++pattern;
            ++name;
        } else if (p == '*') {
            star = ++pattern;
            retry = name;
        } else if (star != NULL) {
            pattern = star;
            name = ++retry;
        } else {
            return 0;
        }
    }
    while (*pattern == '*')
        ++pattern;
    return *pattern == '\0';
}

typedef struct PortFindContext {
    const char *pattern;
    const char *relative_directory;
    char *found;
    size_t capacity;
    int matched;
} PortFindContext;

static SDL_EnumerationResult SDLCALL find_directory_entry(
    void *userdata, const char *dirname, const char *filename)
{
    PortFindContext *context = (PortFindContext *)userdata;
    char candidate[PORT_PATH_BYTES * 2u];
    SDL_PathInfo info;
    int length;
    if (!dos_pattern_match(context->pattern, filename))
        return SDL_ENUM_CONTINUE;
    length = snprintf(candidate, sizeof(candidate), "%s%s", dirname, filename);
    if (length < 0 || (size_t)length >= sizeof(candidate) ||
        !SDL_GetPathInfo(candidate, &info) || info.type != SDL_PATHTYPE_FILE)
        return SDL_ENUM_CONTINUE;
    if (context->relative_directory[0] != '\0')
        length = snprintf(context->found, context->capacity, "%s/%s",
                          context->relative_directory, filename);
    else
        length = snprintf(context->found, context->capacity, "%s", filename);
    if (length < 0 || (size_t)length >= context->capacity)
        return SDL_ENUM_FAILURE;
    context->matched = 1;
    return SDL_ENUM_SUCCESS;
}

int port_fs_find(const char *pattern, char *found, size_t capacity)
{
    char relative[PORT_PATH_BYTES];
    char directory[PORT_PATH_BYTES];
    char full_directory[PORT_PATH_BYTES * 2u];
    char *basename;
    char *slash;
    int length;
    PortFindContext context;
    SDL_PathInfo info;
    if (!safe_relative_path(pattern, relative, sizeof(relative)) ||
        found == NULL || capacity == 0)
        return 0;
    if (strchr(relative, '*') == NULL && strchr(relative, '?') == NULL) {
        int handle = port_fs_open_read(relative);
        if (handle < 0)
            return 0;
        port_fs_close(handle);
        length = snprintf(found, capacity, "%s", relative);
        return length >= 0 && (size_t)length < capacity;
    }
    slash = strrchr(relative, '/');
    if (slash != NULL) {
        size_t directory_length = (size_t)(slash - relative);
        if (directory_length >= sizeof(directory))
            return 0;
        memcpy(directory, relative, directory_length);
        directory[directory_length] = '\0';
        basename = slash + 1;
    } else {
        directory[0] = '\0';
        basename = relative;
    }
    if (strcmp(basename, "*.*") == 0)
        basename = "*";
    if (strchr(directory, '*') != NULL || strchr(directory, '?') != NULL)
        return 0;
    if (directory[0] == '\0')
        length = snprintf(full_directory, sizeof(full_directory), "%s", s_asset_root);
    else
        length = snprintf(full_directory, sizeof(full_directory), "%s/%s",
                          s_asset_root, directory);
    if (length < 0 || (size_t)length >= sizeof(full_directory) ||
        !SDL_GetPathInfo(full_directory, &info) || info.type != SDL_PATHTYPE_DIRECTORY)
        return 0;
    context.pattern = basename;
    context.relative_directory = directory;
    context.found = found;
    context.capacity = capacity;
    context.matched = 0;
    (void)SDL_EnumerateDirectory(full_directory, find_directory_entry, &context);
    return context.matched;
}

static FILE *file_from_handle(int handle)
{
    if (handle <= 0 || handle > (int)PORT_FILE_HANDLES)
        return NULL;
    return s_files[(size_t)handle - 1u];
}

int32_t port_fs_read(int handle, void *buffer, uint32_t bytes)
{
    FILE *file = file_from_handle(handle);
    size_t got;
    if (file == NULL || buffer == NULL)
        return -1;
    got = fread(buffer, 1, bytes, file);
    if (got < bytes && ferror(file))
        return -1;
    return (int32_t)got;
}

int32_t port_fs_seek(int handle, int32_t offset, int origin)
{
    FILE *file = file_from_handle(handle);
    long position;
    if (file == NULL || fseek(file, (long)offset, origin) != 0)
        return -1;
    position = ftell(file);
    return position < 0 ? -1 : (int32_t)position;
}

void port_fs_close(int handle)
{
    FILE *file = file_from_handle(handle);
    if (file != NULL) {
        fclose(file);
        s_files[(size_t)handle - 1u] = NULL;
    }
}

/* Translates asm/file_read.ASM:_file_read_fatal. DOS reads in 0x4000-byte
   chunks, advancing the destination segment by 0x400 paragraphs each time.
   The host pointer is already normalized by the port's far-memory model. */
void file_read_fatal(const char *filename, uint8_t *destination)
{
    uint8_t chunk[0x4000];
    size_t remaining;
    size_t offset = 0;
    int handle;
    if (filename == NULL || destination == NULL ||
        !port_memory_extent(destination, &remaining))
        fatal_error("%s FILE ERROR", filename != NULL ? filename : "(null)");

    handle = port_fs_open_read(filename);
    if (handle < 0)
        fatal_error("%s FILE ERROR", filename);

    for (;;) {
        int32_t got = port_fs_read(handle, chunk, (uint32_t)sizeof(chunk));
        if (got < 0 || (size_t)got > remaining - offset) {
            port_fs_close(handle);
            fatal_error("%s FILE ERROR", filename);
        }
        memcpy(destination + offset, chunk, (size_t)got);
        offset += (size_t)got;
        if ((size_t)got != sizeof(chunk))
            break;
    }
    port_fs_close(handle);
}

void *port_fs_load(const char *path, size_t *length_out,
                   PortFarPtr *address_out)
{
    FILE *file = open_asset(path);
    long size;
    void *data;
    if (file == NULL)
        return NULL;
    if (fseek(file, 0, SEEK_END) != 0 || (size = ftell(file)) < 0 ||
        fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }
    data = port_memory_alloc((size_t)size + 1u, path, address_out);
    if (data == NULL) {
        fclose(file);
        return NULL;
    }
    if (fread(data, 1, (size_t)size, file) != (size_t)size) {
        port_memory_free(data);
        fclose(file);
        return NULL;
    }
    ((uint8_t *)data)[size] = 0;
    fclose(file);
    if (length_out != NULL)
        *length_out = (size_t)size;
    return data;
}
