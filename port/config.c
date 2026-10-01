#include "port_runtime.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define CONFIG_FILE_LIMIT 65536u
static int s_manual_word_check;
static const char s_defaults[] = "{\n  \"manual_word_check\": false\n}\n";

typedef struct ConfigJson {
    const char *next;
    int found_manual;
    int manual;
} ConfigJson;

static void whitespace(ConfigJson *json)
{
    while (*json->next && strchr(" \t\r\n", *json->next)) ++json->next;
}

static int json_string(ConfigJson *json, char *output, size_t capacity)
{
    size_t length = 0;
    if (*json->next != '"') return 0;
    ++json->next;
    while (*json->next && *json->next != '"') {
        unsigned c = (unsigned char)*json->next++;
        if (c < 32) return 0;
        if (c == '\\') {
            if (!*json->next) return 0;
            c = (unsigned char)*json->next++;
            if (c == 'u') {
                unsigned code = 0;
                for (unsigned i = 0; i < 4; ++i) {
                    const char *hex = "0123456789abcdef";
                    unsigned digit = (unsigned char)*json->next;
                    const char *match;
                    if (digit >= 'A' && digit <= 'F') digit += 'a' - 'A';
                    match = digit ? strchr(hex, (int)digit) : NULL;
                    if (!match) return 0;
                    code = code * 16 + (unsigned)(match - hex);
                    ++json->next;
                }
                /* Only ASCII is needed to identify setting keys. Other keys
                   and all unknown values stay untouched in the original file. */
                c = code > 0 && code < 128 ? code : '?';
            } else {
                if (!c || !strchr("\"\\/bfnrt", (int)c)) return 0;
                switch (c) {
                case 'b': c = '\b'; break;
                case 'f': c = '\f'; break;
                case 'n': c = '\n'; break;
                case 'r': c = '\r'; break;
                case 't': c = '\t'; break;
                }
            }
        }
        if (output && length + 1 < capacity) output[length] = (char)c;
        ++length;
    }
    if (*json->next != '"') return 0;
    ++json->next;
    if (output && capacity) output[length < capacity ? length : capacity - 1] = 0;
    return 1;
}

static int json_value(ConfigJson *json, unsigned depth, int *boolean);

static int json_container(ConfigJson *json, unsigned depth, int object)
{
    char end = object ? '}' : ']';
    if (depth > 32) return 0;
    ++json->next;
    whitespace(json);
    if (*json->next == end) { ++json->next; return 1; }
    for (;;) {
        char key[64] = {0};
        int boolean = -1;
        if (object) {
            if (*json->next != '"' || !json_string(json, key, sizeof(key))) return 0;
            whitespace(json);
            if (*json->next != ':') return 0;
            ++json->next;
        }
        if (!json_value(json, depth + 1, &boolean)) return 0;
        if (object && depth == 0 && strcmp(key, "manual_word_check") == 0) {
            if (boolean < 0 || json->found_manual) return 0;
            json->manual = boolean;
            json->found_manual = 1;
        }
        whitespace(json);
        if (*json->next == end) { ++json->next; return 1; }
        if (*json->next != ',') return 0;
        ++json->next;
        whitespace(json);
    }
}

static int digit(char c) { return c >= '0' && c <= '9'; }

static int json_value(ConfigJson *json, unsigned depth, int *boolean)
{
    whitespace(json);
    *boolean = -1;
    if (*json->next == '{') return json_container(json, depth, 1);
    if (*json->next == '[') return json_container(json, depth, 0);
    if (*json->next == '"') return json_string(json, NULL, 0);
    if (strncmp(json->next, "true", 4) == 0) {
        json->next += 4; *boolean = 1; return 1;
    }
    if (strncmp(json->next, "false", 5) == 0) {
        json->next += 5; *boolean = 0; return 1;
    }
    if (strncmp(json->next, "null", 4) == 0) { json->next += 4; return 1; }
    if (*json->next == '-') ++json->next;
    if (*json->next == '0') ++json->next;
    else {
        if (*json->next < '1' || *json->next > '9') return 0;
        do { ++json->next; } while (digit(*json->next));
    }
    if (*json->next == '.') {
        ++json->next;
        if (!digit(*json->next)) return 0;
        do { ++json->next; } while (digit(*json->next));
    }
    if (*json->next == 'e' || *json->next == 'E') {
        ++json->next;
        if (*json->next == '+' || *json->next == '-') ++json->next;
        if (!digit(*json->next)) return 0;
        do { ++json->next; } while (digit(*json->next));
    }
    return 1;
}

/* Create only a missing file, including when another launch wins the race.
   Packaging never ships config.json, so upgrades preserve user settings. */
static int create_defaults(const char *path)
{
#ifdef _WIN32
    wchar_t *wide = (wchar_t *)SDL_iconv_string("UTF-16LE", "UTF-8", path, strlen(path) + 1);
    HANDLE file;
    DWORD written = 0;
    int ok;
    if (!wide) return 0;
    file = CreateFileW(wide, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    SDL_free(wide);
    if (file == INVALID_HANDLE_VALUE) return 0;
    ok = WriteFile(file, s_defaults, (DWORD)(sizeof(s_defaults) - 1), &written, NULL) &&
         written == sizeof(s_defaults) - 1;
    if (!CloseHandle(file)) ok = 0;
    return ok;
#else
    FILE *file = fopen(path, "wx");
    int ok;
    if (!file) return 0;
    ok = fwrite(s_defaults, 1, sizeof(s_defaults) - 1, file) == sizeof(s_defaults) - 1;
    if (fclose(file) != 0) ok = 0;
    return ok;
#endif
}

int port_config_load(const char *path)
{
    SDL_IOStream *file;
    SDL_PathInfo info;
    Sint64 size;
    char *text;
    ConfigJson json;
    int ok;
    s_manual_word_check = 0;
    file = SDL_IOFromFile(path, "rb");
    if (!file && !SDL_GetPathInfo(path, &info)) {
        if (create_defaults(path)) fprintf(stderr, "PORT config created: %s\n", path);
        file = SDL_IOFromFile(path, "rb");
    }
    if (!file) {
        fprintf(stderr, "PORT config unavailable: %s; using defaults\n", path);
        return 0;
    }
    size = SDL_GetIOSize(file);
    if (size < 0 || size > CONFIG_FILE_LIMIT) {
        fprintf(stderr, "PORT config unreadable or too large: %s; using defaults\n", path);
        SDL_CloseIO(file);
        return 0;
    }
    text = SDL_calloc((size_t)size + 1, 1);
    if (!text) { SDL_CloseIO(file); return 0; }
    ok = SDL_ReadIO(file, text, (size_t)size) == (size_t)size;
    if (!SDL_CloseIO(file)) ok = 0;
    text[size] = 0;
    json = (ConfigJson){text, 0, 0};
    if (size >= 3 && memcmp(text, "\xef\xbb\xbf", 3) == 0) json.next += 3;
    whitespace(&json);
    ok = ok && !memchr(text, 0, (size_t)size) && *json.next == '{' &&
         json_container(&json, 0, 1);
    whitespace(&json);
    ok = ok && *json.next == 0;
    if (ok) s_manual_word_check = json.manual;
    else fprintf(stderr, "PORT config invalid: %s; preserving file, using defaults\n", path);
    SDL_free(text);
    return ok;
}

int port_config_manual_word_check(void) { return s_manual_word_check; }
