#ifndef PORT_RENDER_TILE_EXPANSION_H
#define PORT_RENDER_TILE_EXPANSION_H

#include <stddef.h>
#include <stdint.h>

#define PORT_AHEAD_BASE_COUNT 23u
#define PORT_AHEAD_ROW_COUNT 5u
#define PORT_AHEAD_MAX_EXTRA_PER_SIDE 12u
#define PORT_AHEAD_MAX_COUNT \
    (PORT_AHEAD_BASE_COUNT + 2u * PORT_AHEAD_ROW_COUNT * PORT_AHEAD_MAX_EXTRA_PER_SIDE)

/* Expand each lateral row of one recovered 23-tile table. The original 23
   entries remain in their original relative order; new entries are inserted
   at row edges and inherit that edge's detail threshold. No source table is
   changed. Returns zero for malformed input or insufficient output space. */
size_t port_expand_ahead_table(const int8_t *source, size_t source_count,
                               unsigned extra_per_side,
                               int8_t *output, size_t output_capacity);

#endif
