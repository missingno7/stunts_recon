/* Batch adapter for tests/test_matrix_machine_semantics.py. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void mat_vec(const int16_t *input, const int16_t *matrix, int16_t *output);
void mat_multiply(const int16_t *right, const int16_t *left,
                  int16_t *output);

void port_guest_unwind(const char *message)
{
    fprintf(stderr, "unexpected guest unwind: %s\n", message);
    exit(4);
}

int main(void)
{
    unsigned mode, first, second, output, index, parsed;
    uint16_t words[64];

    while (scanf("%u %u %u %u", &mode, &first, &second, &output) == 4) {
        for (index = 0; index < 64; ++index) {
            if (scanf("%u", &parsed) != 1)
                return 2;
            words[index] = (uint16_t)parsed;
        }
        if (mode == 0) {
            mat_vec((const int16_t *)&words[first],
                    (const int16_t *)&words[second],
                    (int16_t *)&words[output]);
        } else if (mode == 1) {
            mat_multiply((const int16_t *)&words[first],
                         (const int16_t *)&words[second],
                         (int16_t *)&words[output]);
        } else {
            return 3;
        }
        for (index = 0; index < 64; ++index)
            printf("%s%04x", index ? " " : "", (unsigned)words[index]);
        putchar('\n');
    }
    return 0;
}
