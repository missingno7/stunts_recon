#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int16_t stunts_game_main(int16_t argc, char *argv[]);

static SDL_Thread *s_guest_thread;
static SDL_AtomicInt s_guest_done;
static jmp_buf s_guest_escape;
static int s_guest_escape_ready;
static char s_guest_stop_reason[256];

typedef struct GuestArgs {
    char program_name[16];
    char *argv[2];
} GuestArgs;

static int guest_thread_main(void *unused)
{
    GuestArgs args;
    (void)unused;
    memset(&args, 0, sizeof(args));
    strcpy(args.program_name, "STUNTS");
    args.argv[0] = args.program_name;
    s_guest_escape_ready = 1;
    if (setjmp(s_guest_escape) == 0) {
        int16_t result = stunts_game_main(1, args.argv);
        snprintf(s_guest_stop_reason, sizeof(s_guest_stop_reason),
                 "game entry returned %d", (int)result);
        fprintf(stderr, "PORT startup stopped: %s\n", s_guest_stop_reason);
        port_trace_host_stop(s_guest_stop_reason);
    } else {
        fprintf(stderr, "PORT startup stopped at unresolved boundary: %s\n",
                s_guest_stop_reason);
    }
    s_guest_escape_ready = 0;
    SDL_SetAtomicInt(&s_guest_done, 1);
    return 0;
}

void port_guest_unwind(const char *symbol)
{
    snprintf(s_guest_stop_reason, sizeof(s_guest_stop_reason),
             "unresolved host service: %s", symbol != NULL ? symbol : "unknown");
    port_trace_host_stop(s_guest_stop_reason);
    if (s_guest_escape_ready)
        longjmp(s_guest_escape, 1);
    fprintf(stderr, "PORT fatal stub outside guest thread: %s\n", s_guest_stop_reason);
    abort();
}

void port_stub_fail(const char *symbol)
{
    port_guest_unwind(symbol);
}

static int parse_run_ms(const char *argument)
{
    const char *value = NULL;
    if (strncmp(argument, "--run-ms=", 9) == 0)
        value = argument + 9;
    if (value == NULL)
        return -1;
    return atoi(value);
}

int main(int argc, char **argv)
{
    const char *trace_path = "build/sdl3/runtime-trace.jsonl";
    const char *asset_root = NULL;
    int run_ms = -1;
    int i;
    uint64_t start_ns;
    uint64_t next_present_ns;
    int should_quit = 0;
    for (i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "--trace=", 8) == 0)
            trace_path = argv[i] + 8;
        else if (strncmp(argv[i], "--assets=", 9) == 0)
            asset_root = argv[i] + 9;
        else if (parse_run_ms(argv[i]) >= 0)
            run_ms = parse_run_ms(argv[i]);
    }
    if (asset_root == NULL)
        asset_root = getenv("STUNTS_ASSET_ROOT");
    if (asset_root == NULL)
        asset_root = "build/sdl3/runtime/assets";
    port_runtime_set_asset_root(asset_root);
    port_sdl_init("Stunts 1.1 - SDL3 faithful port");
    port_trace_open(trace_path, port_runtime_asset_root());
    port_memory_init();
    port_input_init();
    port_timer_start();
    SDL_SetAtomicInt(&s_guest_done, 0);
    s_guest_thread = SDL_CreateThread(guest_thread_main, "stunts-game", NULL);
    if (s_guest_thread == NULL) {
        fprintf(stderr, "Could not create guest thread: %s\n", SDL_GetError());
        port_trace_host_stop("guest thread creation failed");
        should_quit = 1;
    }
    start_ns = SDL_GetTicksNS();
    next_present_ns = start_ns;
    while (!should_quit) {
        uint64_t now;
        if (port_sdl_poll())
            break;
        port_video_present();
        next_present_ns += 16666667u;
        port_sdl_sleep_until(next_present_ns);
        now = SDL_GetTicksNS();
        if (run_ms >= 0 && now - start_ns >= (uint64_t)run_ms * 1000000u)
            break;
    }
    if (s_guest_thread != NULL) {
        /* M0's startup entry stops at a logged service boundary. This join is
           normally immediate and keeps SDL teardown off an active guest. */
        SDL_WaitThread(s_guest_thread, NULL);
        s_guest_thread = NULL;
    }
    port_timer_stop();
    port_audio_shutdown();
    port_trace_close();
    port_sdl_shutdown();
    return 0;
}
