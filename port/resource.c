#include "port_runtime.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESOURCE_DECODE_LIMIT (32u * 1024u * 1024u)
#define RESOURCE_ARCHIVE_ENTRY_LIMIT 4096u

static const uint8_t shape3d_index_counts[16] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 2, 6, 3, 0, 0
};

/* Locked MCGA image data at load [154824, 155080): the game begins with
   identity color mapping, while sub_35DC8/sub_35DE6 mutate this table. */
static uint8_t s_incnums[256] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static uint16_t read_u16(const uint8_t *p)
{
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u24(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16);
}

static uint32_t read_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static int has_extension(const char *name, const char *extension)
{
    size_t name_length;
    size_t extension_length;
    size_t i;
    if (name == NULL || extension == NULL)
        return 0;
    name_length = strlen(name);
    extension_length = strlen(extension);
    if (extension_length == 0u || name_length < extension_length)
        return 0;
    for (i = 0; i < extension_length; ++i) {
        unsigned char actual = (unsigned char)name[name_length - extension_length + i];
        unsigned char expected = (unsigned char)extension[i];
        if (tolower(actual) != tolower(expected))
            return 0;
    }
    return 1;
}

/* P3S/3SH chunks are consumed as several independent tables by shape3d_init_shape
   and the renderer. Keep the full archive span available here so malformed counts
   cannot turn a valid leading header into pointers/read spans in the next chunk. */
static int validate_shape3d_chunk(const uint8_t *chunk, size_t chunk_size)
{
    size_t vertex_end;
    size_t primitive_pos;
    uint8_t vertex_count;
    uint8_t primitive_count;
    uint8_t paint_count;
    uint8_t primitive;

    if (chunk == NULL || chunk_size < 4u)
        return 0;
    vertex_count = chunk[0];
    primitive_count = chunk[1];
    paint_count = chunk[2];
    vertex_end = 4u + (size_t)vertex_count * 6u;
    primitive_pos = vertex_end + (size_t)primitive_count * 8u;
    if (primitive_pos > chunk_size)
        return 0;

    for (primitive = 0; primitive < primitive_count; ++primitive) {
        uint8_t type;
        size_t index_count;
        size_t index;
        size_t record_header = 2u + (size_t)paint_count;
        if (record_header > chunk_size - primitive_pos)
            return 0;
        type = chunk[primitive_pos];
        if (type >= sizeof(shape3d_index_counts) /
                    sizeof(shape3d_index_counts[0]))
            return 0;
        index_count = shape3d_index_counts[type];
        primitive_pos += record_header;
        if (index_count > chunk_size - primitive_pos)
            return 0;
        for (index = 0; index < index_count; ++index)
            if (chunk[primitive_pos + index] >= vertex_count)
                return 0;
        primitive_pos += index_count;
    }
    return 1;
}

static int validate_shape3d_archive(const uint8_t *archive, size_t extent)
{
    uint16_t count;
    size_t payload;
    size_t payload_extent;
    uint16_t index;

    if (archive == NULL || extent < 6u)
        return 0;
    count = read_u16(archive + 4u);
    if (count > RESOURCE_ARCHIVE_ENTRY_LIMIT)
        return 0;
    payload = 6u + (size_t)count * 8u;
    if (payload > extent)
        return 0;
    payload_extent = extent - payload;

    for (index = 0; index < count; ++index) {
        size_t offset_table = 6u + (size_t)count * 4u;
        size_t relative = read_u32(archive + offset_table +
                                   (size_t)index * 4u);
        size_t next = payload_extent;
        uint16_t other_index;
        if (relative > payload_extent)
            return 0;
        for (other_index = 0; other_index < count; ++other_index) {
            size_t other = read_u32(archive + offset_table +
                                    (size_t)other_index * 4u);
            if (other > relative && other < next)
                next = other;
        }
        if (next < relative ||
            !validate_shape3d_chunk(archive + payload + relative,
                                    next - relative))
            return 0;
    }
    return 1;
}

static int grow_bytes(uint8_t **data, size_t *capacity, size_t required)
{
    size_t next;
    uint8_t *larger;
    if (required > RESOURCE_DECODE_LIMIT)
        return 0;
    if (required <= *capacity)
        return 1;
    next = *capacity == 0 ? 256u : *capacity;
    while (next < required) {
        if (next > RESOURCE_DECODE_LIMIT / 2u) {
            next = RESOURCE_DECODE_LIMIT;
            break;
        }
        next *= 2u;
    }
    if (next < required)
        return 0;
    larger = (uint8_t *)realloc(*data, next);
    if (larger == NULL)
        return 0;
    *data = larger;
    *capacity = next;
    return 1;
}

/* Faithful C translation of tools/porting/format_reference.py:decompress_rle.
   The sequence stage is separate because the original pass can first expand
   repeated byte sequences before applying its byte, word, and literal runs. */
static int decode_rle(const uint8_t *source, size_t source_size,
                      uint8_t **result_out, size_t *result_size_out)
{
    uint32_t output_size;
    uint32_t sequence_size;
    uint8_t escape_raw;
    uint8_t escape_count;
    uint8_t escape[127];
    const uint8_t *encoded;
    size_t encoded_size;
    uint8_t *sequence = NULL;
    size_t sequence_length = 0;
    size_t sequence_capacity = 0;
    const uint8_t *input;
    size_t input_size;
    size_t input_pos = 0;
    uint8_t *output;
    size_t output_pos = 0;
    uint8_t lookup[256] = {0};
    size_t i;

    if (source_size < 9u || source[0] != 1u)
        return 0;
    output_size = read_u24(source + 1);
    sequence_size = read_u24(source + 4);
    escape_raw = source[8];
    escape_count = (uint8_t)(escape_raw & 0x7Fu);
    if (output_size > RESOURCE_DECODE_LIMIT ||
        9u + (size_t)escape_count > source_size)
        return 0;
    memcpy(escape, source + 9, escape_count);
    encoded = source + 9u + escape_count;
    encoded_size = source_size - 9u - escape_count;

    if ((escape_raw & 0x80u) == 0) {
        uint8_t marker;
        size_t pos = 0;
        if (escape_count <= 1u || sequence_size > encoded_size)
            return 0;
        marker = escape[1];
        while (pos < sequence_size) {
            size_t literal_start;
            size_t literal_length;
            uint8_t repetitions;
            size_t copies;
            size_t append_length;
            if (encoded[pos] != marker) {
                if (!grow_bytes(&sequence, &sequence_capacity,
                                sequence_length + 1u))
                    goto fail;
                sequence[sequence_length++] = encoded[pos++];
                continue;
            }
            literal_start = ++pos;
            while (pos < sequence_size && encoded[pos] != marker)
                ++pos;
            if (pos >= sequence_size)
                goto fail;
            literal_length = pos - literal_start;
            ++pos;
            if (pos >= sequence_size)
                goto fail;
            repetitions = (uint8_t)(encoded[pos++] - 1u);
            copies = (size_t)repetitions + 1u;
            if (literal_length != 0 && copies >
                (RESOURCE_DECODE_LIMIT - sequence_length) / literal_length)
                goto fail;
            append_length = literal_length * copies;
            if (!grow_bytes(&sequence, &sequence_capacity,
                            sequence_length + append_length))
                goto fail;
            if (literal_length != 0u) {
                for (i = 0; i < copies; ++i) {
                    memcpy(sequence + sequence_length,
                           encoded + literal_start, literal_length);
                    sequence_length += literal_length;
                }
            }
        }
        input = sequence;
        input_size = sequence_length;
    } else {
        input = encoded;
        input_size = encoded_size;
    }

    for (i = 0; i < escape_count; ++i)
        lookup[escape[i]] = (uint8_t)(i + 1u);
    output = (uint8_t *)malloc(output_size == 0 ? 1u : output_size);
    if (output == NULL)
        goto fail;
    while (output_pos < output_size) {
        uint8_t kind;
        uint32_t run;
        uint8_t value;
        if (input_pos >= input_size)
            goto fail_output;
        kind = lookup[input[input_pos++]];
        if (kind == 0) {
            output[output_pos++] = input[input_pos - 1u];
            continue;
        }
        if (kind == 1u) {
            if (input_size - input_pos < 2u)
                goto fail_output;
            run = input[input_pos++];
            value = input[input_pos++];
        } else if (kind == 3u) {
            if (input_size - input_pos < 3u)
                goto fail_output;
            run = read_u16(input + input_pos);
            value = input[input_pos + 2u];
            input_pos += 3u;
        } else {
            run = (uint32_t)kind - 1u;
            if (input_pos >= input_size)
                goto fail_output;
            value = input[input_pos++];
        }
        if (run > output_size - output_pos)
            goto fail_output;
        memset(output + output_pos, value, run);
        output_pos += run;
    }
    free(sequence);
    *result_out = output;
    *result_size_out = output_size;
    return 1;

fail_output:
    free(output);
fail:
    free(sequence);
    return 0;
}

/* Canonical, most-significant-bit-first VLE translation matching the
   validated format_reference.py decoder and fileio.c recurrence. */
static int decode_vle(const uint8_t *source, size_t source_size,
                      uint8_t **result_out, size_t *result_size_out)
{
    uint32_t output_size;
    uint8_t flags;
    uint8_t width_count;
    uint8_t counts[16];
    uint16_t starts[16];
    uint16_t ends[16];
    uint16_t alpha_starts[16];
    uint8_t alphabet[256];
    uint16_t alphabet_length = 0;
    size_t table_size;
    size_t code_pos;
    size_t bit_pos = 0;
    uint8_t *output;
    size_t output_pos = 0;
    uint8_t current = 0;
    uint32_t code = 0;
    uint16_t alpha_pos = 0;
    unsigned width_index;

    if (source_size < 7u || source[0] != 2u)
        return 0;
    output_size = read_u24(source + 1);
    flags = source[4];
    width_count = (uint8_t)(flags & 0x7Fu);
    if (output_size > RESOURCE_DECODE_LIMIT || width_count == 0u ||
        width_count > 16u || 5u + width_count > source_size)
        return 0;
    table_size = 5u + width_count;
    for (width_index = 0; width_index < width_count; ++width_index) {
        counts[width_index] = source[5u + width_index];
        alphabet_length = (uint16_t)(alphabet_length + counts[width_index]);
        if (alphabet_length > 256u)
            return 0;
    }
    if (alphabet_length == 0u || table_size + alphabet_length > source_size)
        return 0;
    memcpy(alphabet, source + table_size, alphabet_length);
    code_pos = table_size + alphabet_length;
    if (output_size != 0 && source_size - code_pos < 2u)
        return 0;

    alpha_pos = 0;
    for (width_index = 0; width_index < width_count; ++width_index) {
        starts[width_index] = (uint16_t)code;
        code += counts[width_index];
        ends[width_index] = (uint16_t)code;
        alpha_starts[width_index] = alpha_pos;
        alpha_pos = (uint16_t)(alpha_pos + counts[width_index]);
        code <<= 1;
    }
    output = (uint8_t *)malloc(output_size == 0 ? 1u : output_size);
    if (output == NULL)
        return 0;

    while (output_pos < output_size) {
        uint32_t accumulator = 0;
        int symbol_index = -1;
        for (width_index = 1; width_index <= width_count; ++width_index) {
            uint8_t bit;
            uint32_t start;
            uint32_t end;
            if (bit_pos >= (source_size - code_pos) * 8u)
                goto fail;
            bit = (uint8_t)((source[code_pos + bit_pos / 8u] >>
                             (7u - (bit_pos & 7u))) & 1u);
            ++bit_pos;
            accumulator = (accumulator << 1) | bit;
            start = starts[width_index - 1u];
            end = ends[width_index - 1u];
            if (width_index <= 8u) {
                if (accumulator >= start && accumulator < end) {
                    symbol_index = (int)alpha_starts[width_index - 1u] +
                                   (int)(accumulator - start);
                    break;
                }
            } else if (accumulator < end) {
                symbol_index = (int)accumulator +
                               (int)alpha_starts[width_index - 1u] -
                               (int)start;
                break;
            }
        }
        if (symbol_index < 0 || symbol_index >= alphabet_length)
            goto fail;
        if ((flags & 0x80u) != 0)
            current = (uint8_t)(current + alphabet[symbol_index]);
        else
            current = alphabet[symbol_index];
        output[output_pos++] = current;
    }
    *result_out = output;
    *result_size_out = output_size;
    return 1;

fail:
    free(output);
    return 0;
}

int port_resource_decompress(const uint8_t *source, size_t source_size,
                             uint8_t **output, size_t *output_size)
{
    const uint8_t *current;
    size_t current_size;
    uint8_t *owned = NULL;
    uint8_t pass_count;
    uint32_t expected_final = 0;
    int has_expected_final = 0;
    unsigned pass;

    if (source == NULL || output == NULL || output_size == NULL ||
        source_size == 0 || source_size > RESOURCE_DECODE_LIMIT)
        return 0;
    *output = NULL;
    *output_size = 0;
    if ((source[0] & 0x80u) != 0) {
        pass_count = (uint8_t)(source[0] & 0x7Fu);
        if (pass_count == 0u || source_size < 4u)
            return 0;
        expected_final = read_u24(source + 1);
        has_expected_final = 1;
        current = source + 4;
        current_size = source_size - 4u;
    } else {
        pass_count = 1;
        current = source;
        current_size = source_size;
    }
    for (pass = 0; pass < pass_count; ++pass) {
        uint8_t *next = NULL;
        size_t next_size = 0;
        int ok;
        if (current_size == 0)
            goto fail;
        if (current[0] == 1u)
            ok = decode_rle(current, current_size, &next, &next_size);
        else if (current[0] == 2u)
            ok = decode_vle(current, current_size, &next, &next_size);
        else
            ok = 0;
        if (!ok)
            goto fail;
        free(owned);
        owned = next;
        current = owned;
        current_size = next_size;
    }
    if (has_expected_final && expected_final != current_size)
        goto fail;
    *output = owned;
    *output_size = current_size;
    return 1;

fail:
    free(owned);
    return 0;
}

static void *read_whole_file(const char *name, size_t *size_out, int fatal)
{
    void *data = port_fs_load(name, size_out, NULL);
    if (data == NULL && fatal)
        port_guest_unwind("file open/read failed");
    return data;
}

static void *read_binary_resource(const char *name, int fatal)
{
    size_t size = 0;
    void *data = read_whole_file(name, &size, fatal);
    if (data != NULL && has_extension(name, ".3sh") &&
        !validate_shape3d_archive((const uint8_t *)data, size)) {
        port_memory_free(data);
        if (fatal)
            port_guest_unwind("malformed raw 3D shape archive");
        return NULL;
    }
    return data;
}

void *file_load_binary_nofatal(const char *name)
{
    return read_binary_resource(name, 0);
}

void *file_load_binary(const char *name, int fatal)
{
    return read_binary_resource(name, fatal != 0);
}

void *file_load_binary_nofatal_thunk(const char *name)
{
    return file_load_binary_nofatal(name);
}

void *file_decomp(const char *name, int fatal)
{
    size_t source_size = 0;
    size_t decoded_size = 0;
    void *source_data = read_whole_file(name, &source_size, fatal != 0);
    uint8_t *decoded = NULL;
    void *managed;
    if (source_data == NULL)
        return NULL;
    if (!port_resource_decompress((const uint8_t *)source_data, source_size,
                                  &decoded, &decoded_size)) {
        port_memory_free(source_data);
        if (fatal)
            port_guest_unwind("unsupported or malformed compressed resource");
        return NULL;
    }
    if (has_extension(name, ".p3s") &&
        !validate_shape3d_archive(decoded, decoded_size)) {
        free(decoded);
        port_memory_free(source_data);
        if (fatal)
            port_guest_unwind("malformed compressed 3D shape archive");
        return NULL;
    }
    managed = port_memory_alloc(decoded_size, name, NULL);
    if (managed != NULL)
        memcpy(managed, decoded, decoded_size);
    free(decoded);
    port_memory_free(source_data);
    if (managed == NULL && fatal)
        port_guest_unwind("resource allocation failed");
    return managed;
}

void *file_decomp_nofatal(const char *name)
{
    return file_decomp(name, 0);
}

void *file_load_shape2d_fatal_thunk(char *name)
{
    extern void *file_load_shape2d_fatal(char *);
    return file_load_shape2d_fatal(name);
}

void *load_shape2d_nofatal_thunk(char *name)
{
    extern void *file_load_shape2d_nofatal(char *);
    return file_load_shape2d_nofatal(name);
}

void *load_shape2d_res_nofatal_thunk(char *name)
{
    extern void *file_load_shape2d_res_nofatal(char *);
    return file_load_shape2d_res_nofatal(name);
}

static int archive_view(const uint8_t *archive, size_t *extent_out,
                        uint16_t *count_out, size_t *payload_out)
{
    size_t extent;
    uint16_t count;
    size_t base;
    if (archive == NULL || !port_memory_extent(archive, &extent) || extent < 6u)
        return 0;
    count = read_u16(archive + 4);
    if (count > 4096u)
        return 0;
    base = 6u + (size_t)count * 8u;
    if (base > extent)
        return 0;
    if (extent_out != NULL) *extent_out = extent;
    if (count_out != NULL) *count_out = count;
    if (payload_out != NULL) *payload_out = base;
    return 1;
}

uint16_t file_get_res_shape_count(void *archive)
{
    uint16_t count = 0;
    (void)archive_view((const uint8_t *)archive, NULL, &count, NULL);
    return count;
}

void *file_get_shape2d(uint8_t *archive, int index)
{
    size_t extent;
    size_t payload;
    size_t data_offset;
    size_t shape_end;
    uint16_t count;
    uint32_t relative;
    uint32_t next_relative;
    uint16_t i;
    if (index < 0 || !archive_view(archive, &extent, &count, &payload) ||
        (uint16_t)index >= count)
        return NULL;
    relative = read_u32(archive + 6u + (size_t)count * 4u +
                        (size_t)(uint16_t)index * 4u);
    if (relative > extent - payload)
        return NULL;
    shape_end = extent - payload;
    for (i = 0; i < count; ++i) {
        uint32_t other = read_u32(archive + 6u + (size_t)count * 4u +
                                  (size_t)i * 4u);
        if (other > relative && other < shape_end)
            shape_end = other;
    }
    data_offset = payload + relative;
    if (shape_end < relative || shape_end - relative < 16u ||
        data_offset + 16u > extent)
        return NULL;
    next_relative = (uint32_t)shape_end;
    (void)next_relative;
    return archive + data_offset;
}

static int shape_entry_span(uint8_t *archive, int index,
                            uint8_t **shape_out, size_t *length_out)
{
    size_t extent;
    size_t payload;
    size_t relative;
    size_t next;
    uint16_t count;
    uint16_t i;
    uint32_t offset;
    if (index < 0 || !archive_view(archive, &extent, &count, &payload) ||
        (uint16_t)index >= count)
        return 0;
    offset = read_u32(archive + 6u + (size_t)count * 4u +
                      (size_t)(uint16_t)index * 4u);
    relative = (size_t)offset;
    if (relative > extent - payload)
        return 0;
    next = extent - payload;
    for (i = 0; i < count; ++i) {
        uint32_t other = read_u32(archive + 6u + (size_t)count * 4u +
                                  (size_t)i * 4u);
        if ((size_t)other > relative && (size_t)other < next)
            next = (size_t)other;
    }
    if (next < relative || next - relative < 16u)
        return 0;
    *shape_out = archive + payload + relative;
    *length_out = next - relative;
    return 1;
}

static int shape2d_blit_view(uint8_t *shape, size_t *shape_extent_out,
                            uint8_t **destination_out,
                            size_t *destination_extent_out,
                            PortSprite **sprite_out)
{
    size_t shape_extent;
    size_t destination_extent;
    uint8_t *destination;
    PortSprite *sprite;
    size_t source_pixels;

    if (shape == NULL || !port_memory_extent(shape, &shape_extent) ||
        shape_extent < 16u ||
        !port_sprite_active_view(&destination, &destination_extent, &sprite) ||
        sprite == NULL || sprite->sprite_bitmapptr == NULL ||
        sprite->lineofs == NULL || sprite->words2[4] == 0u)
        return 0;

    source_pixels = (size_t)read_u16(shape) * (size_t)read_u16(shape + 2u);
    if (source_pixels > shape_extent - 16u)
        return 0;
    if (shape_extent_out != NULL)
        *shape_extent_out = shape_extent;
    if (destination_out != NULL)
        *destination_out = destination;
    if (destination_extent_out != NULL)
        *destination_extent_out = destination_extent;
    if (sprite_out != NULL)
        *sprite_out = sprite;
    return 1;
}

static int32_t signed_hotspot_offset(uint16_t hotspot, int16_t scale)
{
    int32_t product = (int32_t)(int16_t)hotspot * (int32_t)scale;
    /* IMUL's AH:DL byte pair yields the signed 8.8 offset, floor(product/256). */
    if (product >= 0)
        return product / 256;
    return -(((-product) + 255) / 256);
}

static void publish_transparent_blit(uint8_t *destination,
                                     const char *publication_reason)
{
    if (destination == port_video_pixels())
        port_video_publish(publication_reason);
}

/* SHAPE2D putimage and release operations use the game's 16-byte byte layout,
   then map source pixels through incnums; a mapped 0xff stays transparent. */
static void putimage_transparent_at(uint8_t *shape, int16_t x, int16_t y,
                                    const char *publication_reason)
{
    size_t destination_extent;
    uint8_t *destination;
    PortSprite *sprite;
    uint16_t width;
    uint16_t height;
    int32_t left;
    int32_t right;
    int32_t top;
    int32_t bottom;
    int32_t x0;
    int32_t x1;
    int32_t y0;
    int32_t y1;
    int32_t draw_y;

    if (!shape2d_blit_view(shape, NULL, &destination,
                           &destination_extent, &sprite))
        return;
    width = read_u16(shape);
    height = read_u16(shape + 2u);
    if (width == 0u || height == 0u)
        return;

    left = sprite->words2[0];
    right = sprite->words2[1];
    top = sprite->words2[2];
    bottom = sprite->words2[3];
    if (right > sprite->words2[4])
        right = sprite->words2[4];
    if (bottom > sprite->sprite_bitmapptr->height)
        bottom = sprite->sprite_bitmapptr->height;
    if (left < 0 || top < 0 || left >= right || top >= bottom)
        return;

    x0 = x;
    y0 = y;
    x1 = x0 + width;
    y1 = y0 + height;
    if (x0 < left) x0 = left;
    if (x1 > right) x1 = right;
    if (y0 < top) y0 = top;
    if (y1 > bottom) y1 = bottom;
    if (x0 >= x1 || y0 >= y1)
        return;

    for (draw_y = y0; draw_y < y1; ++draw_y) {
        size_t source_at = 16u + (size_t)(draw_y - y) * width +
                           (size_t)(x0 - x);
        size_t destination_at = (size_t)sprite->lineofs[draw_y] +
                                (size_t)x0;
        int32_t draw_x;
        if (destination_at > destination_extent ||
            (size_t)(x1 - x0) > destination_extent - destination_at)
            continue;
        for (draw_x = x0; draw_x < x1; ++draw_x) {
            uint8_t color = s_incnums[shape[source_at++]];
            if (color != 0xffu)
                destination[destination_at] = color;
            ++destination_at;
        }
    }
    publish_transparent_blit(destination, publication_reason);
}

void sub_35DC8(const uint8_t *mapping)
{
    size_t i;
    if (mapping == NULL)
        port_guest_unwind("missing full incnums mapping source");
    /* Preserve the original forward REP MOVSB behavior, including overlap. */
    for (i = 0; i < sizeof(s_incnums); ++i)
        s_incnums[i] = mapping[i];
}

void sub_35DE6(uint16_t start, uint16_t count, const uint8_t *mapping)
{
    size_t i;
    if (((size_t)count != 0u && mapping == NULL) ||
        (size_t)start > sizeof(s_incnums) ||
        (size_t)count > sizeof(s_incnums) - (size_t)start)
        port_guest_unwind("incnums partial mapping exceeds its 256-byte span");
    /* The historical helper copies exactly CX bytes from DS:SI to
       CS:_incnums+start, without clamping or touching adjacent storage. */
    for (i = 0; i < (size_t)count; ++i)
        s_incnums[(size_t)start + i] = mapping[i];
}

void sprite_putimage_transparent(void *shape_pointer, int16_t x, int16_t y)
{
    putimage_transparent_at((uint8_t *)shape_pointer, x, y,
                            "sprite_putimage_transparent");
}

void release_shape_resources(void *shape_pointer)
{
    uint8_t *shape = (uint8_t *)shape_pointer;
    size_t source_extent;
    size_t destination_extent;
    uint8_t *destination;
    PortSprite *sprite;
    uint16_t width;
    uint16_t height;
    int32_t x;
    int32_t y;
    int32_t draw_y;

    if (!shape2d_blit_view(shape, &source_extent, &destination,
                           &destination_extent, &sprite))
        return;
    width = read_u16(shape);
    height = read_u16(shape + 2u);
    if (width == 0u || height == 0u)
        return;
    x = read_u16(shape + 8u);
    y = read_u16(shape + 10u);

    /* The original release entry uses the embedded position and writes each
       complete row; unlike the other putimage entry it does not honor the
       sprite viewport rectangle. Bound writes by the host surface and pitch. */
    for (draw_y = 0; draw_y < height; ++draw_y) {
        int32_t screen_y = y + draw_y;
        size_t source_at = 16u + (size_t)draw_y * width;
        int32_t draw_x;
        if (screen_y < 0 ||
            screen_y >= sprite->sprite_bitmapptr->height)
            continue;
        if (source_at > source_extent ||
            (size_t)width > source_extent - source_at)
            return;
        for (draw_x = 0; draw_x < width; ++draw_x) {
            int32_t screen_x = x + draw_x;
            size_t destination_at;
            uint8_t color;
            if (screen_x < 0 || screen_x >= sprite->words2[4])
                continue;
            destination_at = (size_t)sprite->lineofs[screen_y] +
                             (size_t)screen_x;
            if (destination_at >= destination_extent)
                continue;
            color = s_incnums[shape[source_at + (size_t)draw_x]];
            if (color != 0xffu)
                destination[destination_at] = color;
        }
    }
    publish_transparent_blit(destination, "release_shape_resources");
}

void shapeexpl(int16_t scale, void *shape_pointer, int16_t x, int16_t y)
{
    uint8_t *shape = (uint8_t *)shape_pointer;
    size_t source_extent;
    size_t destination_extent;
    uint8_t *destination;
    PortSprite *sprite;
    uint16_t source_width;
    uint16_t source_height;
    uint16_t output_width;
    uint16_t output_height;
    uint16_t source_step;
    uint16_t initial_source;
    uint16_t scale_word = (uint16_t)scale;
    int32_t origin_x;
    int32_t origin_y;
    int32_t clip_left;
    int32_t clip_right;
    int32_t clip_top;
    int32_t clip_bottom;
    int32_t draw_x0;
    int32_t draw_x1;
    int32_t draw_y0;
    int32_t draw_y1;
    int32_t draw_y;

    if (scale_word < 2u ||
        !shape2d_blit_view(shape, &source_extent, &destination,
                           &destination_extent, &sprite))
        return;
    source_width = read_u16(shape);
    source_height = read_u16(shape + 2u);
    if (source_width == 0u || source_height == 0u)
        return;
    output_width = (uint16_t)(((uint32_t)source_width *
                               scale_word) >> 8);
    output_height = (uint16_t)(((uint32_t)source_height *
                                scale_word) >> 8);
    if (output_width == 0u || output_height == 0u)
        return;

    origin_x = (int16_t)((uint16_t)x -
                         (uint16_t)signed_hotspot_offset(read_u16(shape + 4u), scale));
    origin_y = (int16_t)((uint16_t)y -
                         (uint16_t)signed_hotspot_offset(read_u16(shape + 6u), scale));
    source_step = (uint16_t)(65536u / scale_word);
    initial_source = (uint16_t)((source_step >> 8) >> 1);

    clip_left = sprite->words2[7];
    clip_right = sprite->words2[8];
    clip_top = sprite->words2[2];
    clip_bottom = sprite->words2[3];
    if (clip_right > sprite->words2[4])
        clip_right = sprite->words2[4];
    if (clip_bottom > sprite->sprite_bitmapptr->height)
        clip_bottom = sprite->sprite_bitmapptr->height;
    if (clip_left < 0 || clip_top < 0 || clip_left >= clip_right ||
        clip_top >= clip_bottom)
        return;

    draw_x0 = origin_x;
    draw_y0 = origin_y;
    draw_x1 = draw_x0 + output_width;
    draw_y1 = draw_y0 + output_height;
    if (draw_x0 < clip_left) draw_x0 = clip_left;
    if (draw_x1 > clip_right) draw_x1 = clip_right;
    if (draw_y0 < clip_top) draw_y0 = clip_top;
    if (draw_y1 > clip_bottom) draw_y1 = clip_bottom;
    if (draw_x0 >= draw_x1 || draw_y0 >= draw_y1)
        return;

    for (draw_y = draw_y0; draw_y < draw_y1; ++draw_y) {
        uint32_t output_row = (uint32_t)(draw_y - origin_y);
        uint32_t source_y = initial_source +
                            ((uint32_t)source_step * output_row) / 256u;
        size_t destination_at = (size_t)sprite->lineofs[draw_y] +
                                (size_t)draw_x0;
        int32_t out_x;
        if (destination_at > destination_extent ||
            (size_t)(draw_x1 - draw_x0) > destination_extent - destination_at)
            continue;
        for (out_x = draw_x0; out_x < draw_x1; ++out_x) {
            uint32_t output_column = (uint32_t)(out_x - origin_x);
            uint32_t source_x = initial_source +
                                ((uint32_t)source_step * output_column) / 256u;
            size_t source_at;
            if (source_x < source_width && source_y < source_height) {
                source_at = 16u + (size_t)source_y * source_width + source_x;
                if (source_at < source_extent && shape[source_at] != 0xffu)
                    destination[destination_at] = shape[source_at];
            }
            ++destination_at;
        }
    }
    publish_transparent_blit(destination, "shapeexpl");
}

/* Translate asm/file_load_shape2d_expand.ASM's ESH directory and plane walk.
   The packed record format and bit order are also specified by the validated
   tools/porting/format_reference.py:expand_esh parser. */
void file_load_shape2d_expand(uint8_t *archive, int8_t *output_pointer)
{
    uint8_t *output = (uint8_t *)output_pointer;
    size_t source_extent;
    size_t output_extent;
    size_t payload;
    size_t output_payload;
    size_t relative = 0;
    size_t total_size;
    uint16_t count;
    uint16_t index;

    if (!archive_view(archive, &source_extent, &count, &payload) ||
        output == NULL || !port_memory_extent(output, &output_extent))
        port_guest_unwind("invalid ESH expansion buffers");
    (void)source_extent;

    output_payload = 6u + (size_t)count * 8u;
    if (output_payload > output_extent)
        port_guest_unwind("ESH output directory exceeds allocation");

    /* The assembly copies count and names, reconstructs offsets, and clears
       the input-size dword until the final record sets the expanded size. */
    memset(output, 0, 4u);
    memcpy(output + 4u, archive + 4u, 2u + (size_t)count * 4u);

    for (index = 0; index < count; ++index) {
        uint8_t *source_shape;
        size_t source_shape_size;
        uint8_t *destination_shape;
        uint8_t *destination_pixels;
        size_t pixels;
        size_t shape_bytes;
        uint16_t width;
        uint16_t height;
        uint8_t base_color;
        unsigned plane;

        if (!shape_entry_span(archive, index, &source_shape,
                              &source_shape_size))
            port_guest_unwind("ESH shape offset outside source archive");
        width = read_u16(source_shape);
        height = read_u16(source_shape + 2u);
        pixels = (size_t)width * (size_t)height;
        if (pixels > source_shape_size - 16u || pixels > (SIZE_MAX - 16u) / 8u)
            port_guest_unwind("ESH packed bitmap extent is invalid");
        shape_bytes = 16u + pixels * 8u;
        if (relative > UINT32_MAX || shape_bytes > UINT32_MAX - relative ||
            relative > output_extent - output_payload ||
            shape_bytes > output_extent - output_payload - relative)
            port_guest_unwind("expanded ESH exceeds output allocation");

        destination_shape = output + output_payload + relative;
        memcpy(destination_shape, source_shape, 16u);
        destination_shape[0] = (uint8_t)((width * 8u) & 0xffu);
        destination_shape[1] = (uint8_t)(((width * 8u) >> 8) & 0xffu);
        destination_pixels = destination_shape + 16u;
        base_color = (uint8_t)(source_shape[13] >> 4);
        memset(destination_pixels, base_color, pixels * 8u);

        for (plane = 0; plane < 4u; ++plane) {
            uint8_t pattern = (uint8_t)(source_shape[12u + plane] & 0x0fu);
            size_t byte_index;
            const uint8_t *plane_source;
            if (pattern == 0)
                break;
            if (pixels > (source_shape_size - 16u) / (plane + 1u))
                port_guest_unwind("ESH color plane exceeds source record");
            plane_source = source_shape + 16u + pixels * plane;
            for (byte_index = 0; byte_index < pixels; ++byte_index) {
                uint8_t packed = plane_source[byte_index];
                unsigned bit;
                for (bit = 0; bit < 8u; ++bit) {
                    if ((packed & (uint8_t)(0x80u >> bit)) != 0)
                        destination_pixels[byte_index * 8u + bit] |= pattern;
                }
            }
        }

        /* ESH offsets are relative to the expanded archive payload. */
        {
            size_t offset_at = 6u + (size_t)count * 4u + (size_t)index * 4u;
            uint32_t stored_offset = (uint32_t)relative;
            output[offset_at] = (uint8_t)(stored_offset & 0xffu);
            output[offset_at + 1u] = (uint8_t)((stored_offset >> 8) & 0xffu);
            output[offset_at + 2u] = (uint8_t)((stored_offset >> 16) & 0xffu);
            output[offset_at + 3u] = (uint8_t)((stored_offset >> 24) & 0xffu);
        }
        relative += shape_bytes;
    }

    /* The zero-shape path returns after the assembly's initial size clear. */
    total_size = count == 0 ? 0 : output_payload + relative;
    output[0] = (uint8_t)(total_size & 0xffu);
    output[1] = (uint8_t)((total_size >> 8) & 0xffu);
    output[2] = (uint8_t)((total_size >> 16) & 0xffu);
    output[3] = (uint8_t)((total_size >> 24) & 0xffu);
}

/* Translate asm/file_load_shape2d_palmap_apply.ASM. SHAPE2D colors produced
   by ESH expansion are four-bit indices, matching seg034_shape2d_group.c's
   16-entry palette map. */
void file_load_shape2d_palmap_apply(uint8_t *archive,
                                    const uint8_t *palette_map)
{
    size_t extent;
    uint16_t count;
    uint16_t index;

    if (palette_map == NULL ||
        !archive_view(archive, &extent, &count, NULL))
        port_guest_unwind("invalid shape palette-map buffers");

    for (index = 0; index < count; ++index) {
        uint8_t *shape;
        size_t shape_size;
        size_t pixel_count;
        size_t pixel;
        uint16_t width;
        uint16_t height;

        if (!shape_entry_span(archive, index, &shape, &shape_size))
            port_guest_unwind("shape palette-map offset outside archive");
        width = read_u16(shape);
        height = read_u16(shape + 2u);
        pixel_count = (size_t)width * (size_t)height;
        if (pixel_count > UINT16_MAX)
            pixel_count = (uint16_t)pixel_count;
        if (pixel_count > shape_size - 16u ||
            (size_t)(shape - archive) + 16u + pixel_count > extent)
            port_guest_unwind("shape palette-map pixels exceed archive");

        for (pixel = 0; pixel < pixel_count; ++pixel) {
            uint8_t color = shape[16u + pixel];
            if (color >= 16u)
                port_guest_unwind("ESH pixel exceeds palette-map range");
            shape[16u + pixel] = palette_map[color];
        }
    }
}

static int shape_name_equal(const uint8_t *stored, const char *requested)
{
    size_t i;
    if (requested == NULL || strlen(requested) < 4u)
        return 0;
    for (i = 0; i < 4u; ++i) {
        if (stored[i] != (uint8_t)requested[i])
            return 0;
    }
    return 1;
}

void *locate_shape_nofatal(uint8_t *archive, const char *name)
{
    uint16_t count;
    uint16_t i;
    uint16_t j;
    size_t extent;
    size_t payload;
    if (!archive_view(archive, &extent, &count, &payload))
        return NULL;
    for (i = 0; i < count; ++i) {
        const uint8_t *stored = archive + 6u + (size_t)i * 4u;
        if (shape_name_equal(stored, name)) {
            uint32_t relative = read_u32(archive + 6u + (size_t)count * 4u +
                                         (size_t)i * 4u);
            size_t next = extent - payload;
            if ((size_t)relative >= next)
                return NULL;
            for (j = 0; j < count; ++j) {
                uint32_t other = read_u32(archive + 6u + (size_t)count * 4u +
                                          (size_t)j * 4u);
                if (other > relative && (size_t)other < next)
                    next = (size_t)other;
            }
            if (next <= relative)
                return NULL;
            /* The locator returns a typed archive entry, not necessarily a
               SHAPE2D bitmap: entries such as gnam/gsna are plain strings. */
            return archive + payload + relative;
        }
    }
    return NULL;
}

void *locate_shape_fatal(uint8_t *archive, char *name)
{
    void *shape = locate_shape_nofatal(archive, name);
    if (shape == NULL) {
        char requested[5] = { 0, 0, 0, 0, 0 };
        char detail[96];
        uint16_t count = 0;
        unsigned i;
        if (name != NULL)
            memcpy(requested, name, 4u);
        if (archive_view(archive, NULL, &count, NULL)) {
            char entries[4][5] = { { 0 } };
            unsigned shown = count < 4u ? count : 4u;
            for (i = 0; i < shown; ++i)
                memcpy(entries[i], archive + 6u + (size_t)i * 4u, 4u);
            (void)snprintf(detail, sizeof(detail),
                           "shape %.4s absent from archive [%.4s %.4s %.4s %.4s]",
                           requested, entries[0], entries[1], entries[2], entries[3]);
        } else {
            (void)snprintf(detail, sizeof(detail),
                           "shape %.4s lookup received invalid archive", requested);
        }
        port_guest_unwind(detail);
    }
    return shape;
}

void *locate_sound_fatal(uint8_t *archive, char *name)
{
    void *sound = locate_shape_nofatal(archive, name);
    if (sound == NULL) {
        char requested[5] = { 0, 0, 0, 0, 0 };
        char detail[96];
        if (name != NULL)
            memcpy(requested, name, 4u);
        (void)snprintf(detail, sizeof(detail),
                       "sound %.4s not found in resource archive", requested);
        port_guest_unwind(detail);
    }
    return sound;
}

char *file_find(const char *name)
{
    static char found[1024];
    if (name == NULL || !port_fs_find(name, found, sizeof(found)))
        return NULL;
    return found;
}

void file_unflip_shape2d(uint8_t *archive, uint8_t *scratch)
{
    uint16_t count;
    uint16_t index;
    size_t extent;
    size_t scratch_size;
    if (!archive_view(archive, &extent, &count, NULL))
        port_guest_unwind("invalid PVS shape archive");
    if (scratch == NULL || !port_memory_extent(scratch, &scratch_size))
        port_guest_unwind("missing PVS transform workspace");
    for (index = 0; index < count; ++index) {
        uint8_t *shape;
        size_t shape_size;
        uint16_t width;
        uint16_t height;
        size_t pixels;
        uint8_t flip;
        uint8_t *temp;
        uint16_t x;
        uint16_t y;
        if (!shape_entry_span(archive, index, &shape, &shape_size))
            port_guest_unwind("invalid PVS shape extent");
        width = read_u16(shape);
        height = read_u16(shape + 2);
        pixels = (size_t)width * height;
        flip = (uint8_t)(shape[14] >> 4);
        if ((shape[15] & 0xF0u) != 0u || flip == 0u)
            continue;
        /* The original dispatcher returns immediately with AX=1 on an
           unsupported type. In particular, it does not transform any later
           entries after encountering a flag >= 4. */
        if (flip >= 4u)
            return;
        if (pixels > shape_size - 16u ||
            pixels > RESOURCE_DECODE_LIMIT || pixels > scratch_size)
            port_guest_unwind("unsupported PVS shape transform");
        temp = scratch;
        memcpy(temp, shape + 16u, pixels);
        if (flip == 1u) {
            for (y = 0; y < height; ++y)
                for (x = 0; x < width; ++x)
                    shape[16u + (size_t)y * width + x] =
                        temp[(size_t)x * height + y];
        } else if (flip == 2u) {
            for (y = 0; y < height; y = (uint16_t)(y + 2u)) {
                for (x = 0; x < width; ++x) {
                    shape[16u + (size_t)y * width + x] =
                        temp[(size_t)(y / 2u) + (size_t)x * height];
                    if ((uint16_t)(y + 1u) < height)
                        shape[16u + (size_t)(y + 1u) * width + x] =
                            temp[(size_t)((height + y + 1u) / 2u) +
                                 (size_t)x * height];
                }
            }
        } else {
            size_t even_plane_step = ((size_t)height + 1u) / 2u;
            size_t odd_plane_step = (size_t)height / 2u;
            for (y = 0; y < height; ++y) {
                size_t source_start;
                size_t source_step;
                if ((y & 1u) == 0u) {
                    source_start = (size_t)(y / 2u);
                    source_step = even_plane_step;
                } else {
                    source_start = (size_t)width * even_plane_step +
                                   (size_t)(y / 2u);
                    source_step = odd_plane_step;
                }
                for (x = 0; x < width; ++x)
                    shape[16u + (size_t)y * width + x] =
                        temp[source_start + (size_t)x * source_step];
            }
        }
    }
    (void)extent;
}

void file_unflip_shape2d_pes(uint8_t *archive, uint8_t *scratch)
{
    uint16_t count;
    uint16_t index;
    size_t scratch_size;
    if (!archive_view(archive, NULL, &count, NULL))
        port_guest_unwind("invalid PES shape archive");
    if (scratch == NULL || !port_memory_extent(scratch, &scratch_size))
        port_guest_unwind("missing PES transform workspace");
    for (index = 0; index < count; ++index) {
        uint8_t *shape;
        size_t shape_size;
        uint16_t width;
        uint16_t height;
        size_t pixels;
        uint8_t mask;
        unsigned plane;
        uint8_t *temp;
        uint16_t x;
        uint16_t y;
        if (!shape_entry_span(archive, index, &shape, &shape_size))
            port_guest_unwind("invalid PES shape extent");
        width = read_u16(shape);
        height = read_u16(shape + 2);
        pixels = (size_t)width * height;
        mask = (uint8_t)((shape[14] >> 4) & 0x0Fu);
        if ((shape[15] & 0xF0u) != 0u || mask == 0u)
            continue;
        for (plane = 0; plane < 4u; ++plane) {
            if ((mask & (1u << plane)) == 0u)
                continue;
            if ((plane + 1u) * pixels > shape_size - 16u)
                port_guest_unwind("short PES bitmap plane");
            if (pixels > scratch_size)
                port_guest_unwind("PES transform exceeds workspace");
            temp = scratch;
            memcpy(temp, shape + 16u + plane * pixels, pixels);
            for (y = 0; y < height; ++y)
                for (x = 0; x < width; ++x)
                    shape[16u + plane * pixels + (size_t)y * width + x] =
                        temp[(size_t)x * height + y];
        }
    }
}

/* 16-bit MSC pointers returned as a host span are deliberately resolved only
   inside registered game allocations. These helpers implement the accepted
   ASM's paragraph-to-byte position calculations without segment casts. */
int32_t parse_shape2d_helper(uint8_t *pointer)
{
    return (int32_t)(uintptr_t)pointer;
}

void *parse_shape2d_helper2(int32_t pointer)
{
    return (void *)(uintptr_t)(uint32_t)pointer;
}
