#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT_FILE_HANDLES 64u
#define PORT_PATH_BYTES 1024u
#define PORT_FULL_PATH_BYTES (PORT_PATH_BYTES * 3u)
#define PORT_FILE_CHUNK_BYTES 0x4000u

static FILE *s_files[PORT_FILE_HANDLES];
static char s_asset_root[PORT_PATH_BYTES] = "build/sdl3/runtime/assets";
static char s_save_root[PORT_PATH_BYTES] = "build/sdl3/runtime/saves";

typedef struct PortFindState {
    char pattern[PORT_PATH_BYTES];
    char directory[PORT_PATH_BYTES];
    size_t next_ordinal;
    char *output;
    size_t output_capacity;
    int active;
} PortFindState;

static PortFindState s_find_state;

static int append_path(char *output, size_t capacity,
                       const char *directory, const char *name)
{
    size_t directory_length;
    int length;
    if (output == NULL || directory == NULL || name == NULL || capacity == 0)
        return 0;
    directory_length = strlen(directory);
    if (directory_length != 0 &&
        (directory[directory_length - 1u] == '/' ||
         directory[directory_length - 1u] == '\\'))
        length = snprintf(output, capacity, "%s%s", directory, name);
    else
        length = snprintf(output, capacity, "%s/%s", directory, name);
    return length >= 0 && (size_t)length < capacity;
}

static int dos_case_equal(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        if (tolower((unsigned char)*left) != tolower((unsigned char)*right))
            return 0;
        ++left;
        ++right;
    }
    return *left == '\0' && *right == '\0';
}

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
        if (c == ':')
            return 0;
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

typedef struct PortComponentContext {
    const char *requested;
    char *actual;
    size_t capacity;
    SDL_PathType type;
    int found;
} PortComponentContext;

static SDL_EnumerationResult SDLCALL find_case_component(
    void *userdata, const char *dirname, const char *filename)
{
    PortComponentContext *context = (PortComponentContext *)userdata;
    char candidate[PORT_FULL_PATH_BYTES];
    SDL_PathInfo info;
    size_t length;
    if (!dos_case_equal(context->requested, filename) ||
        !append_path(candidate, sizeof(candidate), dirname, filename) ||
        !SDL_GetPathInfo(candidate, &info))
        return SDL_ENUM_CONTINUE;
    length = strlen(filename);
    if (length + 1u > context->capacity)
        return SDL_ENUM_FAILURE;
    memcpy(context->actual, filename, length + 1u);
    context->type = info.type;
    context->found = 1;
    return SDL_ENUM_SUCCESS;
}

/* Resolve each DOS path component against the host directory without relying
   on the host filesystem's case rules. `required_type` is FILE or DIRECTORY. */
static int resolve_existing_path(const char *root, const char *relative,
                                 SDL_PathType required_type, char *resolved,
                                 size_t capacity)
{
    char normalized[PORT_PATH_BYTES];
    char current[PORT_FULL_PATH_BYTES];
    char next[PORT_FULL_PATH_BYTES];
    const char *part;
    if (root == NULL || root[0] == '\0' ||
        !safe_relative_path(relative, normalized, sizeof(normalized)) ||
        !append_path(current, sizeof(current), root, ""))
        return 0;
    part = normalized;
    for (;;) {
        const char *separator = strchr(part, '/');
        size_t part_length = separator != NULL ? (size_t)(separator - part) : strlen(part);
        char component[PORT_PATH_BYTES];
        char actual[PORT_PATH_BYTES];
        PortComponentContext context;
        if (part_length == 0 || part_length >= sizeof(component))
            return 0;
        memcpy(component, part, part_length);
        component[part_length] = '\0';
        context.requested = component;
        context.actual = actual;
        context.capacity = sizeof(actual);
        context.type = SDL_PATHTYPE_OTHER;
        context.found = 0;
        (void)SDL_EnumerateDirectory(current, find_case_component, &context);
        if (!context.found)
            return 0;
        if (separator != NULL && context.type != SDL_PATHTYPE_DIRECTORY)
            return 0;
        if (separator == NULL && context.type != required_type)
            return 0;
        if (!append_path(next, sizeof(next), current, actual))
            return 0;
        memcpy(current, next, strlen(next) + 1u);
        if (separator == NULL)
            break;
        part = separator + 1;
    }
    if (strlen(current) + 1u > capacity)
        return 0;
    memcpy(resolved, current, strlen(current) + 1u);
    return 1;
}

static int find_component(const char *directory, const char *requested,
                          char *actual, size_t capacity,
                          SDL_PathType *type_out)
{
    PortComponentContext context;
    context.requested = requested;
    context.actual = actual;
    context.capacity = capacity;
    context.type = SDL_PATHTYPE_OTHER;
    context.found = 0;
    (void)SDL_EnumerateDirectory(directory, find_case_component, &context);
    if (!context.found)
        return 0;
    if (type_out != NULL)
        *type_out = context.type;
    return 1;
}

static int resolve_directory(const char *root, const char *relative,
                             char *resolved, size_t capacity)
{
    SDL_PathInfo info;
    if (relative == NULL || relative[0] == '\0') {
        if (root == NULL || root[0] == '\0' ||
            !SDL_GetPathInfo(root, &info) || info.type != SDL_PATHTYPE_DIRECTORY ||
            strlen(root) + 1u > capacity)
            return 0;
        memcpy(resolved, root, strlen(root) + 1u);
        return 1;
    }
    return resolve_existing_path(root, relative, SDL_PATHTYPE_DIRECTORY,
                                 resolved, capacity);
}

static FILE *open_resolved_read(char *absolute)
{
    FILE *file;
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

static FILE *open_root_read(const char *root, const char *path)
{
    char absolute[PORT_FULL_PATH_BYTES];
    if (!resolve_existing_path(root, path, SDL_PATHTYPE_FILE,
                               absolute, sizeof(absolute)))
        return NULL;
    return open_resolved_read(absolute);
}

static FILE *open_asset(const char *path)
{
    char absolute[PORT_FULL_PATH_BYTES];
    if (resolve_existing_path(s_save_root, path, SDL_PATHTYPE_FILE,
                              absolute, sizeof(absolute)))
        return open_resolved_read(absolute);
    return open_root_read(s_asset_root, path);
}

void port_runtime_set_asset_root(const char *root)
{
    char normalized_root[PORT_PATH_BYTES];
    char save_root[PORT_PATH_BYTES];
    size_t length;
    size_t parent_length;
    const char *separator;
    int written;
    if (root == NULL || root[0] == '\0')
        return;
    length = strlen(root);
    if (length >= sizeof(normalized_root))
        return;
    memcpy(normalized_root, root, length + 1u);
    while (length > 1u &&
           (normalized_root[length - 1u] == '/' || normalized_root[length - 1u] == '\\'))
        normalized_root[--length] = '\0';
    if (length == 2u && normalized_root[1] == ':') {
        normalized_root[2] = '\\';
        normalized_root[3] = '\0';
        length = 3u;
    }

    separator = strrchr(normalized_root, '/');
    {
        const char *backslash = strrchr(normalized_root, '\\');
        if (backslash != NULL && (separator == NULL || backslash > separator))
            separator = backslash;
    }
    parent_length = separator != NULL ? (size_t)(separator - normalized_root) + 1u : 0u;
    written = snprintf(save_root, sizeof(save_root), "%.*ssaves",
                       (int)parent_length, normalized_root);
    if (written >= 0 && (size_t)written < sizeof(save_root) &&
        dos_case_equal(save_root, normalized_root))
        written = snprintf(save_root, sizeof(save_root), "%.*sw",
                           (int)parent_length, normalized_root);
    if (written < 0 || (size_t)written >= sizeof(save_root))
        return;

    memcpy(s_asset_root, normalized_root, length + 1u);
    memcpy(s_save_root, save_root, (size_t)written + 1u);
    s_find_state.active = 0;
}

const char *port_runtime_asset_root(void)
{
    return s_asset_root;
}

int port_runtime_set_save_root(const char *root)
{
    size_t length;
    if (root == NULL || root[0] == '\0')
        return 0;
    length = strlen(root);
    if (length >= sizeof(s_save_root) || dos_case_equal(root, s_asset_root))
        return 0;
    memcpy(s_save_root, root, length + 1u);
    s_find_state.active = 0;
    return 1;
}

static int open_game_file(const char *path)
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

int port_fs_open_read(const char *path)
{
    return open_game_file(path);
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

/* DOS findfirst returns a 13-byte 8.3 filename field. SDL exposes native long
   names on modern filesystems, which do not fit the game's names[][13] list.
   A non-DOS host name has no portable short-name alias, so do not expose it in
   wildcard enumeration. Literal open/read/write paths remain usable. */
static int dos_83_filename(const char *name)
{
    static const char punctuation[] = "!#$%&'()-@^_`{}~";
    const char *dot = NULL;
    size_t base_length;
    size_t extension_length = 0;
    const unsigned char *p;
    if (name == NULL || name[0] == '\0')
        return 0;
    for (p = (const unsigned char *)name; *p != '\0'; ++p) {
        if (*p == '.') {
            if (dot != NULL)
                return 0;
            dot = (const char *)p;
        } else if (!( (*p >= 'A' && *p <= 'Z') ||
                      (*p >= 'a' && *p <= 'z') ||
                      (*p >= '0' && *p <= '9') ||
                      strchr(punctuation, (int)*p) != NULL)) {
            return 0;
        }
    }
    base_length = dot != NULL ? (size_t)(dot - name) : strlen(name);
    if (dot != NULL)
        extension_length = strlen(dot + 1);
    return base_length != 0u && base_length <= 8u &&
           extension_length <= 3u && (dot == NULL || extension_length != 0u);
}

typedef struct PortFindRun {
    const char *pattern;
    const char *relative_directory;
    const char *save_root;
    size_t skip;
    size_t count;
    char *found;
    size_t capacity;
    int exclude_saved_duplicates;
    int matched;
    int overflow;
} PortFindRun;

static int saved_file_shadows(const PortFindRun *run, const char *filename)
{
    char relative[PORT_PATH_BYTES];
    char ignored[PORT_FULL_PATH_BYTES];
    int length;
    if (run->relative_directory[0] != '\0')
        length = snprintf(relative, sizeof(relative), "%s/%s",
                          run->relative_directory, filename);
    else
        length = snprintf(relative, sizeof(relative), "%s", filename);
    if (length < 0 || (size_t)length >= sizeof(relative))
        return 0;
    return resolve_existing_path(run->save_root, relative, SDL_PATHTYPE_FILE,
                                 ignored, sizeof(ignored));
}

static SDL_EnumerationResult SDLCALL enumerate_matching_entry(
    void *userdata, const char *dirname, const char *filename)
{
    PortFindRun *run = (PortFindRun *)userdata;
    char candidate[PORT_FULL_PATH_BYTES];
    char relative[PORT_PATH_BYTES];
    SDL_PathInfo info;
    int length;
    if (!dos_83_filename(filename) ||
        !dos_pattern_match(run->pattern, filename) ||
        !append_path(candidate, sizeof(candidate), dirname, filename) ||
        !SDL_GetPathInfo(candidate, &info) || info.type != SDL_PATHTYPE_FILE ||
        (run->exclude_saved_duplicates && saved_file_shadows(run, filename)))
        return SDL_ENUM_CONTINUE;
    ++run->count;
    if (run->count <= run->skip)
        return SDL_ENUM_CONTINUE;
    if (run->relative_directory[0] != '\0')
        length = snprintf(relative, sizeof(relative), "%s/%s",
                          run->relative_directory, filename);
    else
        length = snprintf(relative, sizeof(relative), "%s", filename);
    if (length < 0 || (size_t)length >= sizeof(relative)) {
        run->overflow = 1;
        return SDL_ENUM_FAILURE;
    }
    if ((size_t)length + 1u > run->capacity) {
        run->overflow = 1;
        return SDL_ENUM_FAILURE;
    }
    memcpy(run->found, relative, (size_t)length + 1u);
    run->matched = 1;
    return SDL_ENUM_SUCCESS;
}

static int find_ordinal_in_root(const char *root, const char *directory,
                                const char *pattern, const char *save_root,
                                size_t skip, int exclude_saved_duplicates,
                                char *found, size_t capacity, size_t *count_out)
{
    char full_directory[PORT_FULL_PATH_BYTES];
    PortFindRun run;
    int enumerated;
    *count_out = 0;
    if (!resolve_directory(root, directory, full_directory, sizeof(full_directory)))
        return 0;
    run.pattern = pattern;
    run.relative_directory = directory;
    run.save_root = save_root;
    run.skip = skip;
    run.count = 0;
    run.found = found;
    run.capacity = capacity;
    run.exclude_saved_duplicates = exclude_saved_duplicates;
    run.matched = 0;
    run.overflow = 0;
    enumerated = SDL_EnumerateDirectory(full_directory,
                                         enumerate_matching_entry, &run);
    *count_out = run.count;
    return enumerated && !run.overflow && run.matched;
}

static int find_wildcard_ordinal(const char *directory, const char *pattern,
                                 size_t ordinal, char *found, size_t capacity)
{
    size_t saved_count = 0;
    size_t ignored_count = 0;
    if (find_ordinal_in_root(s_save_root, directory, pattern, s_save_root,
                             ordinal, 0, found, capacity, &saved_count))
        return 1;
    if (saved_count > ordinal)
        return 0;
    return find_ordinal_in_root(s_asset_root, directory, pattern, s_save_root,
                                ordinal - saved_count, 1, found, capacity,
                                &ignored_count);
}

int port_fs_find(const char *pattern, char *found, size_t capacity)
{
    char relative[PORT_PATH_BYTES];
    char directory[PORT_PATH_BYTES];
    char basename_buffer[PORT_PATH_BYTES];
    char *basename;
    char *slash;
    int length;
    if (found != NULL && capacity != 0)
        found[0] = '\0';
    s_find_state.active = 0;
    if (!safe_relative_path(pattern, relative, sizeof(relative)) ||
        found == NULL || capacity == 0)
        return 0;
    if (strchr(relative, '*') == NULL && strchr(relative, '?') == NULL) {
        int handle = port_fs_open_read(relative);
        size_t relative_length;
        if (handle < 0)
            return 0;
        port_fs_close(handle);
        relative_length = strlen(relative);
        if (relative_length + 1u > capacity)
            return 0;
        memcpy(found, relative, relative_length + 1u);
        return 1;
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
    if (strchr(directory, '*') != NULL || strchr(directory, '?') != NULL)
        return 0;
    if (strcmp(basename, "*.*") == 0)
        basename = "*";
    length = snprintf(basename_buffer, sizeof(basename_buffer), "%s", basename);
    if (length < 0 || (size_t)length >= sizeof(basename_buffer) ||
        !find_wildcard_ordinal(directory, basename_buffer, 0, found, capacity))
        return 0;
    if (strlen(basename_buffer) + 1u > sizeof(s_find_state.pattern) ||
        strlen(directory) + 1u > sizeof(s_find_state.directory))
        return 0;
    memcpy(s_find_state.pattern, basename_buffer, strlen(basename_buffer) + 1u);
    memcpy(s_find_state.directory, directory, strlen(directory) + 1u);
    s_find_state.next_ordinal = 1u;
    s_find_state.output = found;
    s_find_state.output_capacity = capacity;
    s_find_state.active = 1;
    return 1;
}

const char *file_find_next(void)
{
    if (!s_find_state.active)
        return NULL;
    if (!find_wildcard_ordinal(s_find_state.directory, s_find_state.pattern,
                               s_find_state.next_ordinal, s_find_state.output,
                               s_find_state.output_capacity)) {
        s_find_state.active = 0;
        return NULL;
    }
    ++s_find_state.next_ordinal;
    return s_find_state.output;
}

static int resolve_save_destination(const char *path, char *absolute,
                                    size_t capacity)
{
    char relative[PORT_PATH_BYTES];
    char current[PORT_FULL_PATH_BYTES];
    char next[PORT_FULL_PATH_BYTES];
    const char *part;
    if (!safe_relative_path(path, relative, sizeof(relative)) ||
        !SDL_CreateDirectory(s_save_root) ||
        strlen(s_save_root) + 1u > sizeof(current))
        return 0;
    memcpy(current, s_save_root, strlen(s_save_root) + 1u);
    part = relative;
    for (;;) {
        const char *separator = strchr(part, '/');
        size_t part_length = separator != NULL ? (size_t)(separator - part) : strlen(part);
        char component[PORT_PATH_BYTES];
        char actual[PORT_PATH_BYTES];
        SDL_PathType type = SDL_PATHTYPE_OTHER;
        int exists;
        if (part_length == 0 || part_length >= sizeof(component))
            return 0;
        memcpy(component, part, part_length);
        component[part_length] = '\0';
        exists = find_component(current, component, actual, sizeof(actual), &type);
        if (separator != NULL) {
            if (exists) {
                if (type != SDL_PATHTYPE_DIRECTORY)
                    return 0;
            } else {
                memcpy(actual, component, part_length + 1u);
            }
            if (!append_path(next, sizeof(next), current, actual))
                return 0;
            if (!exists && !SDL_CreateDirectory(next))
                return 0;
            memcpy(current, next, strlen(next) + 1u);
            part = separator + 1;
            continue;
        }
        if (exists) {
            if (type != SDL_PATHTYPE_FILE)
                return 0;
        } else {
            memcpy(actual, component, part_length + 1u);
        }
        if (!append_path(absolute, capacity, current, actual))
            return 0;
        return 1;
    }
}

static FILE *open_save_write(const char *path, char *absolute,
                             size_t capacity)
{
    FILE *file;
    char native_path[PORT_FULL_PATH_BYTES];
    if (!resolve_save_destination(path, absolute, capacity))
        return NULL;
    file = fopen(absolute, "wb");
#ifdef _WIN32
    if (file == NULL && strlen(absolute) + 1u <= sizeof(native_path)) {
        char *p;
        memcpy(native_path, absolute, strlen(absolute) + 1u);
        for (p = native_path; *p != '\0'; ++p) {
            if (*p == '/')
                *p = '\\';
        }
        file = fopen(native_path, "wb");
        if (file != NULL)
            memcpy(absolute, native_path, strlen(native_path) + 1u);
    }
#else
    (void)native_path;
#endif
    return file;
}

static int read_file_to_host(const char *filename, void *destination)
{
    uint8_t probe;
    size_t extent = 0;
    size_t offset = 0;
    int bounded;
    int handle;
    if (filename == NULL || destination == NULL)
        return 0;
    bounded = port_memory_extent(destination, &extent);
    handle = port_fs_open_read(filename);
    if (handle < 0)
        return 0;
    for (;;) {
        size_t request = PORT_FILE_CHUNK_BYTES;
        int32_t got;
        if (bounded) {
            if (offset == extent) {
                got = port_fs_read(handle, &probe, 1u);
                if (got != 0) {
                    port_fs_close(handle);
                    return 0;
                }
                break;
            }
            if (extent - offset < request)
                request = extent - offset;
        }
        got = port_fs_read(handle, (uint8_t *)destination + offset,
                           (uint32_t)request);
        if (got < 0) {
            port_fs_close(handle);
            return 0;
        }
        offset += (size_t)got;
        if ((size_t)got < request)
            break;
    }
    port_fs_close(handle);
    return 1;
}

/* The source ABI is `file_read_nofatal(filename, far_destination)`. The SDL
   guest's far pointer has already been normalized to this host pointer. */
void *file_read_nofatal(const char *filename, void *destination)
{
    return read_file_to_host(filename, destination) ? destination : NULL;
}

/* Reproduce DOS AH=3Ch/40h/3Eh/41h: overwrite, stream 16 KiB chunks, and
   remove an incomplete file before reporting failure. Paths are relative to
   the separate save root; reads overlay that root ahead of immutable assets. */
int file_write_fatal(const char *filename, const void *source,
                     unsigned long length)
{
    char absolute[PORT_FULL_PATH_BYTES];
    FILE *file = NULL;
    size_t remaining;
    size_t extent;
    int bounded;
    int failed = 0;
    absolute[0] = '\0';
    if (source == NULL && length != 0u)
        failed = 1;
    if (!failed) {
        bounded = port_memory_extent(source, &extent);
        if (bounded && (size_t)length > extent)
            failed = 1;
    }
    if (!failed) {
        file = open_save_write(filename, absolute, sizeof(absolute));
        if (file == NULL)
            failed = 1;
    }
    remaining = failed ? 0u : (size_t)length;
    while (!failed && remaining != 0u) {
        size_t chunk = remaining > PORT_FILE_CHUNK_BYTES ?
                       PORT_FILE_CHUNK_BYTES : remaining;
        size_t written = fwrite(source, 1u, chunk, file);
        if (written != chunk) {
            failed = 1;
            break;
        }
        source = (const uint8_t *)source + chunk;
        remaining -= chunk;
    }
    if (file != NULL && fclose(file) != 0)
        failed = 1;
    if (failed) {
        if (file != NULL)
            (void)SDL_RemovePath(absolute);
        fatal_error("%s FILE ERROR", filename != NULL ? filename : "(null)");
        return -1;
    }
    return 0;
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
void *file_read_fatal(const char *filename, void *destination)
{
    if (!read_file_to_host(filename, destination))
        fatal_error("%s FILE ERROR", filename != NULL ? filename : "(null)");
    return destination;
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
