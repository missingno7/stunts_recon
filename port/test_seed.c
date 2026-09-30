#include "port_runtime.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT_TEST_SEED_JSON_BYTES 4096u

static int s_seed_loaded;
static int s_random_wait_active;
static uint32_t s_timer_counter;
static uint32_t s_rand_state;
static uint32_t s_expected_status_reads;
static uint32_t s_observed_status_reads;
static uint8_t s_initial_status;
static uint8_t s_changed_status;
static uint8_t s_kevin_state[6];

static const char *json_member(const char *json, const char *name)
{
    char needle[80];
    const char *position;
    const char *colon;
    if (snprintf(needle, sizeof(needle), "\"%s\"", name) < 0)
        return NULL;
    position = strstr(json, needle);
    if (position == NULL)
        return NULL;
    colon = strchr(position + strlen(needle), ':');
    if (colon == NULL)
        return NULL;
    position = colon + 1;
    while (*position != '\0' && isspace((unsigned char)*position))
        ++position;
    return position;
}

static int parse_u32(const char *json, const char *name, uint32_t *value)
{
    const char *position = json_member(json, name);
    char *end;
    unsigned long parsed;
    if (position == NULL || *position == '"' || *position == '-')
        return 0;
    parsed = strtoul(position, &end, 0);
    if (end == position || parsed > UINT32_MAX)
        return 0;
    *value = (uint32_t)parsed;
    return 1;
}

static int parse_hex_string(const char *json, const char *name,
                            uint8_t *bytes, size_t byte_count)
{
    const char *position = json_member(json, name);
    size_t index;
    if (position == NULL || *position != '"')
        return 0;
    ++position;
    for (index = 0; index < byte_count; ++index) {
        char pair[3] = {position[index * 2u], position[index * 2u + 1u], '\0'};
        char *end;
        unsigned long value;
        if (!isxdigit((unsigned char)pair[0]) ||
            !isxdigit((unsigned char)pair[1]))
            return 0;
        value = strtoul(pair, &end, 16);
        if (*end != '\0' || value > 0xffu)
            return 0;
        bytes[index] = (uint8_t)value;
    }
    return position[byte_count * 2u] == '"';
}

int port_test_startup_seed_load(const char *path)
{
    FILE *stream;
    long length;
    char json[PORT_TEST_SEED_JSON_BYTES + 1u];
    uint8_t rand_bytes[4];
    const char *schema;
    const char *point;
    unsigned long rand_state;
    if (path == NULL || path[0] == '\0')
        return 1;
    stream = fopen(path, "rb");
    if (stream == NULL) {
        fprintf(stderr, "PORT cannot open test startup seed: %s\n", path);
        return 0;
    }
    if (fseek(stream, 0, SEEK_END) != 0 ||
        (length = ftell(stream)) < 0 ||
        (unsigned long)length > PORT_TEST_SEED_JSON_BYTES ||
        fseek(stream, 0, SEEK_SET) != 0 ||
        fread(json, 1, (size_t)length, stream) != (size_t)length) {
        fclose(stream);
        fprintf(stderr, "PORT invalid test startup seed size: %s\n", path);
        return 0;
    }
    fclose(stream);
    json[length] = '\0';
    schema = json_member(json, "schema");
    point = json_member(json, "logical_point");
    if (schema == NULL ||
        strncmp(schema, "\"stunts-sdl3-startup-seed-v1\"", 29) != 0 ||
        point == NULL ||
        strncmp(point, "\"after_random_wait_before_get_super_random\"", 43) != 0 ||
        !parse_u32(json, "timer_counter", &s_timer_counter) ||
        !parse_u32(json, "random_wait_status_reads", &s_expected_status_reads)) {
        fprintf(stderr, "PORT test startup seed has invalid schema or fields: %s\n", path);
        return 0;
    }
    /* The three status fields are re-read into their narrow destinations so
       malformed values cannot silently widen or wrap. */
    {
        uint32_t initial_status;
        uint32_t changed_status;
        if (!parse_u32(json, "random_wait_initial_status", &initial_status) ||
            !parse_u32(json, "random_wait_changed_status", &changed_status) ||
            s_expected_status_reads < 2u || initial_status > 0xffu ||
            changed_status > 0xffu ||
            ((initial_status ^ changed_status) & 0x08u) == 0 ||
            !parse_hex_string(json, "crt_rand_state", rand_bytes,
                              sizeof(rand_bytes)) ||
            !parse_hex_string(json, "kevin_random_state", s_kevin_state,
                              sizeof(s_kevin_state))) {
            fprintf(stderr, "PORT test startup seed has invalid state values: %s\n", path);
            return 0;
        }
        s_initial_status = (uint8_t)initial_status;
        s_changed_status = (uint8_t)changed_status;
    }
    /* JSON stores the CRT word in readable big-endian hexadecimal; the DOS
       memory snapshot stores its four bytes little-endian. */
    rand_state = ((unsigned long)rand_bytes[0] << 24) |
                 ((unsigned long)rand_bytes[1] << 16) |
                 ((unsigned long)rand_bytes[2] << 8) |
                 (unsigned long)rand_bytes[3];
    s_rand_state = (uint32_t)rand_state;
    s_seed_loaded = 1;
    return 1;
}

void port_test_random_wait_begin(void)
{
    if (!s_seed_loaded)
        return;
    s_random_wait_active = 1;
    s_observed_status_reads = 0;
}

int port_test_random_wait_status(uint8_t *status_out)
{
    if (!s_random_wait_active || status_out == NULL)
        return 0;
    ++s_observed_status_reads;
    *status_out = s_observed_status_reads < s_expected_status_reads
        ? s_initial_status : s_changed_status;
    return 1;
}

void port_test_random_wait_end(void)
{
    if (!s_random_wait_active)
        return;
    s_random_wait_active = 0;
    if (s_observed_status_reads != s_expected_status_reads) {
        port_guest_unwind("test random_wait status sequence ended at the wrong read");
        return;
    }
    initialize_kevin_random(s_kevin_state);
    port_random_test_set_state(s_rand_state);
    port_timer_test_set_next_counter(s_timer_counter);
    s_seed_loaded = 0;
    fprintf(stderr,
            "PORT test startup seed applied after random_wait: "
            "%u status reads, timer=%u\n",
            (unsigned)s_observed_status_reads, (unsigned)s_timer_counter);
}

uint32_t port_test_random_wait_read_count(void)
{
    return s_observed_status_reads;
}
