#include "tile_expansion.h"

static int sign_i(int value)
{
    return value < 0 ? -1 : value > 0 ? 1 : 0;
}

static int put(int8_t *out, size_t cap, size_t *count,
               int lateral_axis, int fixed, int lateral, int detail)
{
    if (*count >= cap || lateral < -128 || lateral > 127 ||
        fixed < -128 || fixed > 127 || detail < -128 || detail > 127)
        return 0;
    out[*count * 3u + (size_t)lateral_axis] = (int8_t)lateral;
    out[*count * 3u + (size_t)(1 - lateral_axis)] = (int8_t)fixed;
    out[*count * 3u + 2u] = (int8_t)detail;
    ++*count;
    return 1;
}

size_t port_expand_ahead_table(const int8_t *source, size_t source_count,
                               unsigned extra_per_side,
                               int8_t *output, size_t output_capacity)
{
    size_t input_index = 0;
    size_t output_count = 0;
    int lateral_axis;
    int fixed_axis;
    int step;
    size_t rows = 0;
    int seen_fixed[PORT_AHEAD_ROW_COUNT];

    if (!source || !output || source_count < 6u ||
        extra_per_side > PORT_AHEAD_MAX_EXTRA_PER_SIDE)
        return 0;

    {
        int dx = (int)source[3] - (int)source[0];
        int dy = (int)source[4] - (int)source[1];
        if ((dx == 0) == (dy == 0))
            return 0;
        lateral_axis = dx != 0 ? 0 : 1;
        fixed_axis = 1 - lateral_axis;
        step = sign_i((int)source[3u + (size_t)lateral_axis] -
                      (int)source[(size_t)lateral_axis]);
        if (step == 0)
            return 0;
    }

    while (input_index < source_count) {
        size_t row_end = input_index + 1u;
        int fixed = source[input_index * 3u + (size_t)fixed_axis];
        int minimum = source[input_index * 3u + (size_t)lateral_axis];
        int maximum = minimum;
        int min_detail = source[input_index * 3u + 2u];
        int max_detail = min_detail;
        int already_seen = 0;

        for (size_t prior = 0; prior < rows; ++prior)
            if (seen_fixed[prior] == fixed)
                already_seen = 1;

        if (already_seen) {
            if (output_count >= output_capacity)
                return 0;
            output[output_count * 3u] = source[input_index * 3u];
            output[output_count * 3u + 1u] = source[input_index * 3u + 1u];
            output[output_count * 3u + 2u] = source[input_index * 3u + 2u];
            ++output_count;
            ++input_index;
            continue;
        }

        while (row_end < source_count &&
               source[row_end * 3u + (size_t)fixed_axis] == fixed) {
            int lateral = source[row_end * 3u + (size_t)lateral_axis];
            int detail = source[row_end * 3u + 2u];
            if (lateral < minimum) {
                minimum = lateral;
                min_detail = detail;
            }
            if (lateral > maximum) {
                maximum = lateral;
                max_detail = detail;
            }
            ++row_end;
        }

        if (row_end - input_index > 5u || rows >= PORT_AHEAD_ROW_COUNT)
            return 0;

        /* Add the before-edge extension in the table's established lateral
           direction. Preserve the odd near-row ordering among source entries. */
        if (step > 0) {
            for (int lateral = minimum - (int)extra_per_side;
                 lateral < minimum; ++lateral)
                if (!put(output, output_capacity, &output_count,
                         lateral_axis, fixed, lateral, min_detail))
                    return 0;
        } else {
            for (int lateral = maximum + (int)extra_per_side;
                 lateral > maximum; --lateral)
                if (!put(output, output_capacity, &output_count,
                         lateral_axis, fixed, lateral, max_detail))
                    return 0;
        }

        while (input_index < row_end) {
            if (output_count >= output_capacity)
                return 0;
            output[output_count * 3u] = source[input_index * 3u];
            output[output_count * 3u + 1u] = source[input_index * 3u + 1u];
            output[output_count * 3u + 2u] = source[input_index * 3u + 2u];
            ++output_count;
            ++input_index;
        }

        if (step > 0) {
            for (int lateral = maximum + 1;
                 lateral <= maximum + (int)extra_per_side; ++lateral)
                if (!put(output, output_capacity, &output_count,
                         lateral_axis, fixed, lateral, max_detail))
                    return 0;
        } else {
            for (int lateral = minimum - 1;
                 lateral >= minimum - (int)extra_per_side; --lateral)
                if (!put(output, output_capacity, &output_count,
                         lateral_axis, fixed, lateral, min_detail))
                    return 0;
        }
        seen_fixed[rows++] = fixed;
    }

    if (rows != PORT_AHEAD_ROW_COUNT)
        return 0;
    return output_count;
}
