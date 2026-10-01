#include "tile_expansion.h"
#include "tile_tables.generated.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_one(const int8_t source[23][3], unsigned extra)
{
    int8_t expanded[PORT_AHEAD_MAX_COUNT][3];
    size_t count = port_expand_ahead_table(&source[0][0], 23, extra,
                                          &expanded[0][0], PORT_AHEAD_MAX_COUNT);
    size_t original = 0;
    if (count != 23u + 10u * extra) {
        fprintf(stderr, "expansion returned %u for extra=%u\n", (unsigned)count, extra);
        assert(count == 23u + 10u * extra);
    }
    assert(count == 23u + 10u * extra);

    /* All recovered entries remain an ordered subsequence. */
    for (size_t i = 0; i < count && original < 23u; ++i) {
        if (expanded[i][0] == source[original][0] &&
            expanded[i][1] == source[original][1] &&
            expanded[i][2] == source[original][2])
            ++original;
    }
    assert(original == 23u);

    /* Every unique fixed-coordinate row gets its requested side cells. The
       recovered heading-2 table repeats one coordinate late in its sequence;
       the adapter preserves that source anomaly while extending its first row. */
    {
        int lateral_axis = source[1][0] != source[0][0] ? 0 : 1;
        int fixed_axis = 1 - lateral_axis;
        int seen[5];
        unsigned rows = 0;
        for (size_t src = 0; src < 23u; ++src) {
            int fixed = source[src][fixed_axis];
            int exists = 0;
            for (unsigned r = 0; r < rows; ++r) if (seen[r] == fixed) exists = 1;
            if (exists) continue;
            size_t src_end = src + 1u;
            int minimum = source[src][lateral_axis];
            int maximum = minimum;
            int min_detail = source[src][2];
            int max_detail = min_detail;
            while (src_end < 23u && source[src_end][fixed_axis] == fixed) {
                int lateral = source[src_end][lateral_axis];
                if (lateral < minimum) {
                    minimum = lateral;
                    min_detail = source[src_end][2];
                }
                if (lateral > maximum) {
                    maximum = lateral;
                    max_detail = source[src_end][2];
                }
                ++src_end;
            }
            for (unsigned k = 1; k <= extra; ++k) {
                int expected[2] = { minimum - (int)k, maximum + (int)k };
                int details[2] = { min_detail, max_detail };
                for (unsigned side = 0; side < 2; ++side) {
                    unsigned matches = 0;
                    for (size_t i = 0; i < count; ++i)
                        if (expanded[i][fixed_axis] == fixed &&
                            expanded[i][lateral_axis] == expected[side] &&
                            expanded[i][2] == details[side])
                            ++matches;
                    assert(matches == 1u);
                }
            }
            seen[rows++] = fixed;
        }
        assert(rows == 5u);
    }
}

int main(void)
{
    for (unsigned heading = 0; heading < 8; ++heading) {
        test_one(ahead_tables[heading], 0);
        test_one(ahead_tables[heading], 1);
        test_one(ahead_tables[heading], 2);
        test_one(ahead_tables[heading], 4);
        test_one(ahead_tables[heading], 12);
    }
    puts("tile expansion: all eight recovered tables pass");
    return 0;
}
