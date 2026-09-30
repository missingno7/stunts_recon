#include "../port/port_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t s_next_counter;

void port_timer_test_seed_counter(uint32_t counter)
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
    static const uint8_t expected_pre[6] = {0x9d, 0x08, 0xea, 0x4b, 0xfc, 0x4d};
    static const uint8_t expected_post[6] = {0x23, 0x86, 0x7e, 0x94, 0x49, 0x4e};
    uint8_t state[6];
    uint8_t status = 0;
    uint32_t read;
    if (argc != 2 || !port_test_startup_seed_load(argv[1]))
        return 2;
    port_test_random_wait_begin();
    for (read = 1; read <= 8016u; ++read) {
        uint8_t expected = read < 8016u ? 0u : 8u;
        if (!port_test_random_wait_status(&status) || status != expected)
            return 3;
    }
    port_test_random_wait_end();
    if (port_test_random_wait_read_count() != 8016u || s_next_counter != 592u)
        return 4;
    get_kevinrandom_seed(state);
    if (memcmp(state, expected_pre, sizeof(state)) != 0)
        return 5;
    if (port_random_test_rand() != 0x1b04)
        return 6;
    if (get_kevinrandom() != 0x23)
        return 7;
    get_kevinrandom_seed(state);
    if (memcmp(state, expected_post, sizeof(state)) != 0)
        return 8;
    puts("startup seed exact: reads=8016 timer=592 rand=6916 Kevin=0x23");
    return 0;
}
