#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCRIPT_FILE_LIMIT (1024u * 1024u)
#define SCRIPT_EVENT_CAP 512u
#define SCRIPT_SCANCODE_CAP 64u

typedef struct PortScriptEvent {
    uint64_t sequence;
    uint64_t due_ns;
    size_t code_count;
    uint8_t codes[SCRIPT_SCANCODE_CAP];
} PortScriptEvent;

static PortScriptEvent s_events[SCRIPT_EVENT_CAP];
static size_t s_event_count;
static size_t s_next_event;
static uint64_t s_origin_ns;

static char *json_key_value(char *begin, char *limit, const char *key)
{
    char *found = strstr(begin, key);
    char *colon;
    if (found == NULL || found >= limit)
        return NULL;
    colon = strchr(found + strlen(key), ':');
    if (colon == NULL || colon >= limit)
        return NULL;
    ++colon;
    while (colon < limit && (*colon == ' ' || *colon == '\t' ||
                             *colon == '\r' || *colon == '\n'))
        ++colon;
    return colon < limit ? colon : NULL;
}

static int json_read_string(char *begin, char *limit, const char *key,
                            char *output, size_t capacity)
{
    char *value = json_key_value(begin, limit, key);
    char *end;
    size_t length;
    if (value == NULL || *value != '"' || capacity == 0)
        return 0;
    ++value;
    end = strchr(value, '"');
    if (end == NULL || end >= limit)
        return 0;
    length = (size_t)(end - value);
    if (length >= capacity)
        return 0;
    memcpy(output, value, length);
    output[length] = '\0';
    return 1;
}

static int json_read_u64(char *begin, char *limit, const char *key,
                         uint64_t *output)
{
    char *value = json_key_value(begin, limit, key);
    char *end;
    unsigned long long parsed;
    if (value == NULL || *value == '-')
        return 0;
    errno = 0;
    parsed = strtoull(value, &end, 10);
    if (errno != 0 || end == value || end > limit)
        return 0;
    *output = (uint64_t)parsed;
    return 1;
}

static int json_read_scancodes(char *begin, char *limit,
                               PortScriptEvent *event)
{
    char *value = json_key_value(begin, limit, "\"payload\"");
    if (value == NULL || *value != '[')
        return 0;
    ++value;
    for (;;) {
        char *end;
        unsigned long parsed;
        while (value < limit && (*value == ' ' || *value == '\t' ||
                                 *value == '\r' || *value == '\n' ||
                                 *value == ','))
            ++value;
        if (value >= limit)
            return 0;
        if (*value == ']')
            return 1;
        if (*value == '-' || event->code_count == SCRIPT_SCANCODE_CAP)
            return 0;
        errno = 0;
        parsed = strtoul(value, &end, 10);
        if (errno != 0 || end == value || end > limit || parsed > 255u)
            return 0;
        event->codes[event->code_count++] = (uint8_t)parsed;
        value = end;
    }
}

static int parse_script(const char *path)
{
    FILE *stream = NULL;
    char *text = NULL;
    long file_size;
    size_t bytes_read;
    char format[80];
    char *anchor_ptr;
    char *events_ptr;
    char *scan;
    uint64_t anchor_tick;
    uint64_t previous_due = 0;
    int have_previous = 0;
    int ok = 0;

    s_event_count = 0;
    s_next_event = 0;
    if (path == NULL || path[0] == '\0')
        return 1;
    stream = fopen(path, "rb");
    if (stream == NULL) {
        fprintf(stderr, "PORT input script: cannot open '%s'\n", path);
        return 0;
    }
    if (fseek(stream, 0, SEEK_END) != 0 ||
        (file_size = ftell(stream)) < 0 ||
        (unsigned long)file_size > SCRIPT_FILE_LIMIT ||
        fseek(stream, 0, SEEK_SET) != 0)
        goto done;
    text = (char *)malloc((size_t)file_size + 1u);
    if (text == NULL)
        goto done;
    bytes_read = fread(text, 1, (size_t)file_size, stream);
    if (bytes_read != (size_t)file_size)
        goto done;
    text[bytes_read] = '\0';
    if (!json_read_string(text, text + bytes_read, "\"format\"", format,
                          sizeof(format)) ||
        strcmp(format, "portforge-exact-input-script-v1") != 0) {
        fprintf(stderr,
                "PORT input script: expected portforge-exact-input-script-v1\n");
        goto done;
    }
    anchor_ptr = json_key_value(text, text + bytes_read, "\"anchor_tick\"");
    events_ptr = strstr(text, "\"events\"");
    if (anchor_ptr == NULL || events_ptr == NULL ||
        !json_read_u64(text, events_ptr, "\"anchor_tick\"", &anchor_tick)) {
        fprintf(stderr, "PORT input script: missing anchor_tick or events\n");
        goto done;
    }

    scan = events_ptr;
    for (;;) {
        char *visible = strstr(scan, "\"visible_tick\"");
        char *next_visible;
        char *event_limit;
        char channel[64];
        uint64_t absolute_tick;
        uint64_t due_ns;
        PortScriptEvent event;
        if (visible == NULL)
            break;
        next_visible = strstr(visible + 1, "\"visible_tick\"");
        event_limit = next_visible != NULL ? next_visible : text + bytes_read;
        memset(&event, 0, sizeof(event));
        if (!json_read_u64(visible, event_limit, "\"visible_tick\"",
                           &absolute_tick) ||
            !json_read_string(visible, event_limit, "\"channel\"", channel,
                              sizeof(channel))) {
            fprintf(stderr, "PORT input script: malformed event %zu\n",
                    s_event_count);
            goto done;
        }
        if (strcmp(channel, "dos.keyboard.scancodes") != 0 ||
            !json_read_scancodes(visible, event_limit, &event)) {
            fprintf(stderr,
                    "PORT input script: event %zu must be a keyboard scancode batch\n",
                    s_event_count);
            goto done;
        }
        if (absolute_tick < anchor_tick) {
            scan = event_limit;
            continue;
        }
        due_ns = absolute_tick - anchor_tick;
        if ((have_previous && due_ns < previous_due) ||
            s_event_count >= SCRIPT_EVENT_CAP) {
            fprintf(stderr,
                    "PORT input script: unordered or excessive event list\n");
            goto done;
        }
        event.sequence = s_event_count;
        event.due_ns = due_ns;
        s_events[s_event_count++] = event;
        previous_due = due_ns;
        have_previous = 1;
        scan = event_limit;
    }
    s_origin_ns = SDL_GetTicksNS();
    ok = 1;

done:
    if (stream != NULL)
        fclose(stream);
    free(text);
    return ok;
}

int port_input_script_load(const char *path)
{
    if (!parse_script(path))
        return 0;
    if (path != NULL && path[0] != '\0')
        fprintf(stderr, "PORT input script: loaded %zu exact-time event(s)\n",
                s_event_count);
    return 1;
}

void port_input_script_pump(uint64_t now_ns)
{
    while (s_next_event < s_event_count) {
        PortScriptEvent *event = &s_events[s_next_event];
        uint64_t elapsed_ns = now_ns >= s_origin_ns ? now_ns - s_origin_ns : 0;
        uint64_t scheduled_ns = s_origin_ns + event->due_ns;
        if (elapsed_ns < event->due_ns)
            break;
        {
            size_t i;
            for (i = 0; i < event->code_count; ++i)
                port_input_apply_dos_scancode(event->codes[i]);
        }
        port_trace_input_keyboard(event->sequence, scheduled_ns, now_ns,
                                  event->codes, event->code_count);
        ++s_next_event;
    }
}
