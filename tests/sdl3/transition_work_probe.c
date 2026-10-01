#include <stdio.h>
#include <stdlib.h>

#include "../../port/transition_work.h"

int main(int argc, char **argv)
{
    static const uint8_t scale_by_lane[12] = {
        0, 6, 3, 9, 1, 7, 4, 10, 2, 8, 5, 11
    };
    if (argc != 4)
        return 2;

    const uint16_t width = (uint16_t)strtoul(argv[1], NULL, 10);
    const uint16_t height = (uint16_t)strtoul(argv[2], NULL, 10);
    const uint16_t phase = (uint16_t)strtoul(argv[3], NULL, 10);
    uint64_t work = PORT_TRANSITION_FIXED_WORK + 12u * PORT_TRANSITION_LANE_WORK;

    for (uint16_t lane = 0; lane < 12; ++lane) {
        const uint16_t scale_y = scale_by_lane[lane];
        for (uint16_t row = 0;
             (uint32_t)scale_y + 12u * row < height; ++row) {
            work += port_transition_row_work(
                width, (uint16_t)(phase + lane + row));
        }
    }
    printf("%llu\n", (unsigned long long)work);
    return 0;
}
