#include "audio_backend.h"
#include <SDL3/SDL.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define RATE 48000u
#define CAP 65536u
#define BLOCK 2048u
/* One guest producer, one device consumer. Neither owns the other's cursor.
   No callback touches the driver, synthesizer, resource pointers or game state. */
static int16_t s_pcm[CAP];
static atomic_uint s_read, s_write, s_submitted, s_consumed, s_dropped;
static atomic_uint s_underrun_frames, s_underrun_callbacks;
static SDL_AudioStream *s_stream;
static int s_initialized, s_started, s_null;
static unsigned s_buffer_frames=1024, s_device_frames;
int port_audio_sdl_configure(const char *output,unsigned frames)
{
    if(strcmp(output,"device") && strcmp(output,"none")) return 0;
    if(frames<128 || frames>4096 || (frames&(frames-1))) return 0;
    s_null=!strcmp(output,"none"); s_buffer_frames=frames;
    atomic_store(&s_submitted,0); atomic_store(&s_consumed,0);
    atomic_store(&s_dropped,0); atomic_store(&s_underrun_frames,0);
    atomic_store(&s_underrun_callbacks,0);
    return 1;
}
static void SDLCALL consume(void *unused,SDL_AudioStream *stream,int bytes,int total)
{
    int16_t block[BLOCK];
    unsigned missing=0;
    (void)unused;(void)total;
    while(bytes>0) {
        unsigned frames=((unsigned)bytes+1)/2;
        if(frames>BLOCK) frames=BLOCK;
        unsigned read=atomic_load_explicit(&s_read,memory_order_relaxed);
        unsigned write=atomic_load_explicit(&s_write,memory_order_acquire);
        unsigned available=write-read;
        unsigned n=available<frames ? available : frames;
        for(unsigned i=0;i<n;++i) block[i]=s_pcm[(read+i)&(CAP-1)];
        memset(block+n,0,(frames-n)*sizeof(*block));
        atomic_store_explicit(&s_read,read+n,memory_order_release);
        atomic_fetch_add(&s_consumed,n); missing+=frames-n;
        if(!SDL_PutAudioStreamData(stream,block,(int)(frames*sizeof(*block)))) break;
        bytes-=(int)(frames*sizeof(*block));
    }
    if(missing) {
        atomic_fetch_add(&s_underrun_frames,missing);
        atomic_fetch_add(&s_underrun_callbacks,1);
    }
}
void port_audio_sdl_submit(const int16_t *samples,size_t count)
{
    if(!s_stream || s_null) return;
    unsigned write=atomic_load_explicit(&s_write,memory_order_relaxed);
    unsigned read=atomic_load_explicit(&s_read,memory_order_acquire);
    unsigned space=CAP-(write-read);
    unsigned n=count<space ? (unsigned)count : space;
    for(unsigned i=0;i<n;++i) s_pcm[(write+i)&(CAP-1)]=samples[i];
    atomic_store_explicit(&s_write,write+n,memory_order_release);
    atomic_fetch_add(&s_submitted,n);
    atomic_fetch_add(&s_dropped,(unsigned)count-n);
    /* Start only after genuine game output has filled a short latency buffer.
       A fast manual run may drop presentation samples; game work never waits. */
    unsigned prefill=(s_device_frames>s_buffer_frames?s_device_frames:s_buffer_frames)*3;
    if(prefill<4096) prefill=4096;
    if(!s_started && write+n-read>=prefill) {
        if(SDL_ResumeAudioStreamDevice(s_stream)) s_started=1;
    }
}
int port_audio_sdl_start(void)
{
    SDL_AudioSpec spec={SDL_AUDIO_S16,1,RATE};
    char frames[24];
    if(s_null) { s_device_frames=0; SDL_Log("Audio null sink selected; logical driver stays enabled"); return 0; }
    if(s_stream) return 1;
    snprintf(frames,sizeof frames,"%u",s_buffer_frames);
    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES,frames);
    if(!SDL_InitSubSystem(SDL_INIT_AUDIO)) return 0;
    s_initialized=1;
    atomic_store(&s_read,0); atomic_store(&s_write,0); s_started=0;
    s_stream=SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,&spec,consume,NULL);
    if(!s_stream) { port_audio_sdl_stop(); return 0; }
    int obtained=0;
    SDL_AudioSpec obtained_spec;
    if(!SDL_GetAudioDeviceFormat(SDL_GetAudioStreamDevice(s_stream),&obtained_spec,&obtained) || obtained<=0) {
        SDL_Log("Could not query audio device format: %s",SDL_GetError());
        port_audio_sdl_stop(); return 0;
    }
    s_device_frames=(unsigned)obtained;
    return 1;
}
void port_audio_sdl_stop(void)
{
    if(s_stream) { SDL_DestroyAudioStream(s_stream); s_stream=NULL; }
    if(s_initialized) { SDL_QuitSubSystem(SDL_INIT_AUDIO); s_initialized=0; }
    s_started=0;
}
void port_audio_sdl_stats(PortAudioSinkStats *s)
{
    *s=(PortAudioSinkStats){atomic_load(&s_submitted),atomic_load(&s_consumed),
        atomic_load(&s_dropped),atomic_load(&s_underrun_frames),atomic_load(&s_underrun_callbacks),s_device_frames};
}
