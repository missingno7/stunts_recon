#include "audio_backend.h"

#include <SDL3/SDL.h>

#include <stdint.h>

#define PORT_AUDIO_RATE 48000u
#define PORT_AUDIO_CALLBACK_FRAMES 2048u

static SDL_AudioStream *s_audio_stream;
static int s_audio_subsystem_initialized;

static void SDLCALL fill_audio_stream(void *userdata, SDL_AudioStream *stream,
                                     int additional_amount, int total_amount)
{
    int frames_remaining;
    int16_t samples[PORT_AUDIO_CALLBACK_FRAMES];
    (void)userdata;
    (void)total_amount;
    if (additional_amount <= 0)
        return;

    /* The requested amount is in bytes in the stream's input format (mono
       signed 16-bit samples). Supply only the missing frames, in fixed blocks
       so the device callback never allocates memory. */
    frames_remaining = (additional_amount + (int)sizeof(int16_t) - 1) /
                       (int)sizeof(int16_t);
    while (frames_remaining > 0) {
        int frames = frames_remaining > (int)PORT_AUDIO_CALLBACK_FRAMES
                         ? (int)PORT_AUDIO_CALLBACK_FRAMES
                         : frames_remaining;
        size_t rendered = port_audio_render_s16(samples, (size_t)frames,
                                                PORT_AUDIO_RATE);
        if (rendered == 0 ||
            !SDL_PutAudioStreamData(stream, samples,
                                    (int)(rendered * sizeof(samples[0]))))
            return;
        frames_remaining -= frames;
    }
}

int port_audio_sdl_start(void)
{
    SDL_AudioSpec spec;
    if (s_audio_stream != NULL)
        return 1;
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        SDL_Log("SDL audio initialization failed: %s", SDL_GetError());
        return 0;
    }
    s_audio_subsystem_initialized = 1;

    spec.freq = (int)PORT_AUDIO_RATE;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;
    s_audio_stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, fill_audio_stream, NULL);
    if (s_audio_stream == NULL) {
        SDL_Log("SDL audio playback open failed: %s", SDL_GetError());
        port_audio_sdl_stop();
        return 0;
    }
    if (!SDL_ResumeAudioStreamDevice(s_audio_stream)) {
        SDL_Log("SDL audio playback start failed: %s", SDL_GetError());
        port_audio_sdl_stop();
        return 0;
    }
    return 1;
}

void port_audio_sdl_stop(void)
{
    if (s_audio_stream != NULL) {
        /* Destroying the stream closes its playback device and waits for the
           callback to finish before its userdata/model can be torn down. */
        SDL_DestroyAudioStream(s_audio_stream);
        s_audio_stream = NULL;
    }
    if (s_audio_subsystem_initialized) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        s_audio_subsystem_initialized = 0;
    }
}
