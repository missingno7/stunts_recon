#ifndef STUNTS_PORT_TRANSITION_WORK_H
#define STUNTS_PORT_TRANSITION_WORK_H

#include <stdint.h>

/* Frozen sprite_1_unk3: 32 entry/exit instructions, 19 per lane,
   18 per active row and 14 per copied pixel. Work is retired instructions,
   not physical CPU cycles. The reference Stunts machine profile uses 9M/s. */
#define PORT_DOS_WORK_PER_SECOND 9000000ull
#define PORT_TRANSITION_FIXED_WORK 32u
#define PORT_TRANSITION_LANE_WORK 19u

static inline uint32_t port_transition_row_work(uint16_t width, uint16_t phase)
{
    static const uint8_t before[4] = {1, 3, 0, 2};
    uint32_t copied = 0;
    int32_t remaining = width;
    while (remaining > before[(phase + copied) & 3u]) {
        ++copied;
        remaining -= 4;
    }
    return 18u + 14u * copied;
}

typedef struct PortVideoTransition {
    uint64_t origin_ns;
    uint64_t work_units;
    uint64_t next_publication_ns;
} PortVideoTransition;

void port_video_transition_begin(PortVideoTransition *transition);
void port_video_transition_advance(PortVideoTransition *transition,
                                   uint32_t work_units);

#endif
