#include "port_runtime.h"

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
