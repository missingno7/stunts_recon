#include "port_runtime.h"

#include <stdlib.h>
#include <string.h>

/* Six-byte BSS state owned by obj_seg002.ASM in the original game. */
uint8_t randomseeds[6];
static uint32_t s_test_rand_state;
static int s_test_rand_enabled;

void port_random_test_set_state(uint32_t state)
{
    s_test_rand_state = state;
    s_test_rand_enabled = 1;
}

int16_t port_random_test_rand(void)
{
    if (!s_test_rand_enabled)
        return (int16_t)rand();
    s_test_rand_state = s_test_rand_state * 0x000343fdu + 0x00269ec3u;
    return (int16_t)((s_test_rand_state >> 16) & 0x7fffu);
}

void initialize_kevin_random(const uint8_t *seed)
{
    if (seed == NULL)
        port_guest_unwind("null six-byte random seed");
    memcpy(randomseeds, seed, sizeof(randomseeds));
}

void get_kevinrandom_seed(uint8_t *seed_out)
{
    if (seed_out == NULL)
        port_guest_unwind("null random seed destination");
    memcpy(seed_out, randomseeds, sizeof(randomseeds));
}

/* C translation of asm/obj_seg002.ASM:get_kevinrandom. Each addition is
   byte-sized, and the six increment steps propagate a carry across the seed. */
int16_t get_kevinrandom(void)
{
    uint8_t value = randomseeds[5];
    unsigned index;
    value = (uint8_t)(value + randomseeds[4]);
    randomseeds[4] = value;
    value = (uint8_t)(value + randomseeds[3]);
    randomseeds[3] = value;
    value = (uint8_t)(value + randomseeds[2]);
    randomseeds[2] = value;
    value = (uint8_t)(value + randomseeds[1]);
    randomseeds[1] = value;
    value = (uint8_t)(value + randomseeds[0]);
    randomseeds[0] = value;
    for (index = 5; index > 0; --index) {
        if (++randomseeds[index] != 0)
            return randomseeds[0];
    }
    ++randomseeds[0];
    return randomseeds[0];
}
