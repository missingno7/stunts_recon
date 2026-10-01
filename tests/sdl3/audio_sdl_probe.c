#include "audio_backend.h"

#include <SDL3/SDL.h>

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>

static atomic_uint s_render_calls;

size_t port_audio_render_s16(int16_t *output, size_t frames,
                             unsigned sample_rate)
{
    size_t i;
    (void)sample_rate;
    for (i = 0; i < frames; ++i)
        output[i] = (i & 1u) != 0 ? 1024 : -1024;
    atomic_fetch_add_explicit(&s_render_calls, 1u, memory_order_relaxed);
    return frames;
}

int main(void)
{
    unsigned calls;
    if (!SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "dummy"))
        return 2;
    if (!port_audio_sdl_start()) {
        fprintf(stderr, "SDL dummy audio stream unavailable: %s\n", SDL_GetError());
        return 3;
    }
    SDL_Delay(150);
    port_audio_sdl_stop();
    calls = atomic_load_explicit(&s_render_calls, memory_order_relaxed);
    if (calls == 0) {
        fprintf(stderr, "SDL stream did not request generated samples\n");
        return 1;
    }
    printf("SDL3 dummy playback requested samples in %u callbacks\n", calls);
    return 0;
}
