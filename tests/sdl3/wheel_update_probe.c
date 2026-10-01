#include <stdint.h>

struct VECTOR { int16_t x, y, z; };
extern void wheel_update(struct VECTOR *, int16_t, int16_t *, int16_t *,
                         struct VECTOR *, int16_t *);
extern struct VECTOR pts_set[6], secondveccar[6], veccar[6], car_dvecs[6];
extern struct VECTOR pos_pt, ancv2;
extern struct VECTOR veco[6], secondoveh[6], opponent_pointc[6], veh_od[6];
extern struct VECTOR ctrmesh, g_op_carvector2;

static int16_t initialize_and_call(int player, const int16_t *src24x3,
                                   const int16_t *origin6,
                                   const int16_t *base4, int16_t angle,
                                   int16_t cached_angle,
                                   int16_t *out24x3, int16_t *cache5,
                                   uintptr_t *addresses8)
{
    struct VECTOR *groups[4];
    struct VECTOR *source;
    struct VECTOR *first;
    struct VECTOR *second;
    int16_t *origin;
    int i;
    if (player) {
        groups[0] = pts_set; groups[1] = secondveccar;
        groups[2] = veccar; groups[3] = car_dvecs;
        source = pts_set; first = &pos_pt; second = &ancv2;
    } else {
        groups[0] = veco; groups[1] = secondoveh;
        groups[2] = opponent_pointc; groups[3] = veh_od;
        source = veco; first = &ctrmesh; second = &g_op_carvector2;
    }
    for (i = 0; i < 4; ++i)
        for (int j = 0; j < 6; ++j)
            groups[i][j] = (struct VECTOR){src24x3[(i*6+j)*3],
                                           src24x3[(i*6+j)*3+1],
                                           src24x3[(i*6+j)*3+2]};
    first->x = origin6[0]; first->y = origin6[1]; first->z = origin6[2];
    second->x = origin6[3]; second->y = origin6[4]; second->z = origin6[5];
    origin = (int16_t *)first;
    for (i = 0; i < 24*3; ++i) out24x3[i] = (int16_t)0x5555;
    for (i = 0; i < 4; ++i) cache5[i] = (int16_t)0x1111;
    cache5[4] = cached_angle;
    wheel_update((struct VECTOR *)out24x3, angle, (int16_t *)base4, cache5,
                 source, origin);
    if (addresses8) {
        for (i = 0; i < 4; ++i) addresses8[i] = (uintptr_t)groups[i];
        addresses8[4] = (uintptr_t)first;
        addresses8[5] = (uintptr_t)second;
    }
    return 1;
}

__declspec(dllexport) int wheel_probe_globals(int player,
                                               const int16_t *src24x3,
                                               const int16_t *origin6,
                                               const int16_t *base4,
                                               int16_t angle,
                                               int16_t cached_angle,
                                               int16_t *out24x3,
                                               int16_t *cache5,
                                               uintptr_t *addresses8)
{
    return initialize_and_call(player, src24x3, origin6, base4, angle,
                               cached_angle, out24x3, cache5, addresses8);
}

__declspec(dllexport) int wheel_probe_staged(int player,
                                              const int16_t *src24x3,
                                              const int16_t *origin6,
                                              const int16_t *base4,
                                              int16_t angle,
                                              int16_t cached_angle,
                                              int16_t *out24x3,
                                              int16_t *cache5)
{
    struct VECTOR staged[24];
    int16_t staged_origin[6];
    int i;
    for (i = 0; i < 24; ++i)
        staged[i] = (struct VECTOR){src24x3[i*3], src24x3[i*3+1], src24x3[i*3+2]};
    for (i = 0; i < 6; ++i) staged_origin[i] = origin6[i];
    for (i = 0; i < 24*3; ++i) out24x3[i] = (int16_t)0x5555;
    for (i = 0; i < 4; ++i) cache5[i] = (int16_t)0x1111;
    cache5[4] = cached_angle;
    wheel_update((struct VECTOR *)out24x3, angle, (int16_t *)base4, cache5,
                 staged, staged_origin);
    return 1;
}

/* Fixed word packets avoid loading a 32-bit DLL from the oracle's Python. */
#include <stdio.h>
int main(int argc, char **argv)
{
    int16_t packet[85], out[72], cache[5];
    uintptr_t addresses[8];
    FILE *input, *output;
    if (argc != 3) return 2;
    input = fopen(argv[1], "rb"); output = fopen(argv[2], "wb");
    if (!input || !output) return 3;
    while (fread(packet, sizeof(packet), 1, input) == 1) {
        wheel_probe_globals(packet[0], packet + 3, packet + 75, packet + 81,
                            packet[1], packet[2], out, cache, addresses);
        if (fwrite(out, sizeof(out), 1, output) != 1 ||
            fwrite(cache, sizeof(cache), 1, output) != 1 ||
            fwrite(addresses, sizeof(uintptr_t), 6, output) != 6) return 4;
        wheel_probe_staged(packet[0], packet + 3, packet + 75, packet + 81,
                           packet[1], packet[2], out, cache);
        if (fwrite(out, sizeof(out), 1, output) != 1 ||
            fwrite(cache, sizeof(cache), 1, output) != 1) return 4;
    }
    if (ferror(input) || fclose(input) || fclose(output)) return 5;
    return 0;
}
