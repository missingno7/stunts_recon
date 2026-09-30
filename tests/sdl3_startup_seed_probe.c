#include "../port/port_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t s_next_counter;

void port_timer_test_set_next_counter(uint32_t counter)
{
    s_next_counter = counter;
}

void port_guest_unwind(const char *reason)
{
    fprintf(stderr, "unexpected guest unwind: %s\n", reason);
    exit(99);
}

int main(int argc, char **argv)
{
    static const uint8_t expected_pre[6] = {0xa3, 0x6f, 0x84, 0x77, 0x75, 0xbb};
    static const uint8_t expected_post[6] = {0x3d, 0x9a, 0x2b, 0xa7, 0x30, 0xbc};
    uint8_t state[6];
    uint8_t status = 0;
    uint32_t read;
    if (argc != 2 || !port_test_startup_seed_load(argv[1]))
        return 2;
    port_test_random_wait_begin();
    for (read = 1; read <= 6078u; ++read) {
        uint8_t expected = read < 6078u ? 0u : 9u;
        if (!port_test_random_wait_status(&status) || status != expected)
            return 3;
    }
    port_test_random_wait_end();
    if (port_test_random_wait_read_count() != 6078u || s_next_counter != 340u)
        return 4;
    get_kevinrandom_seed(state);
    if (memcmp(state, expected_pre, sizeof(state)) != 0)
        return 5;
    if (port_random_test_rand() != 9479)
        return 6;
    if (get_kevinrandom() != 0x3d)
        return 7;
    get_kevinrandom_seed(state);
    if (memcmp(state, expected_post, sizeof(state)) != 0)
        return 8;
    puts("startup seed exact: reads=6078 timer=340 rand=9479 Kevin=0x3d");
    return 0;
}
