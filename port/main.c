#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int16_t stunts_game_main(int16_t argc, char *argv[]);
extern char audiodriverstring[];

static SDL_Thread *s_guest_thread;
static SDL_ThreadID s_guest_thread_id;
static SDL_AtomicInt s_guest_done;
static SDL_AtomicInt s_guest_exit_status;
static jmp_buf s_guest_escape;
static int s_guest_escape_ready;
static char s_guest_stop_reason[256];
static char s_guest_requested_stop_reason[256];
static uint64_t s_guest_step_limit;
static SDL_AtomicInt s_guest_stop_pending;

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
    s_guest_thread_id = SDL_GetCurrentThreadID();
    s_guest_escape_ready = 1;
    if (setjmp(s_guest_escape) == 0) {
        int16_t result = stunts_game_main(1, args.argv);
        SDL_SetAtomicInt(&s_guest_exit_status, result);
        snprintf(s_guest_stop_reason, sizeof(s_guest_stop_reason),
                 "game entry returned %d", (int)result);
        fprintf(stderr, "PORT startup stopped: %s\n", s_guest_stop_reason);
        port_trace_host_stop(s_guest_stop_reason);
    } else {
        fprintf(stderr, "PORT guest stopped: %s\n",
                s_guest_stop_reason);
    }
    s_guest_escape_ready = 0;
    SDL_SetAtomicInt(&s_guest_done, 1);
    return 0;
}

void port_guest_set_exit_status(int status)
{
    SDL_SetAtomicInt(&s_guest_exit_status, status);
}

void port_guest_unwind(const char *symbol)
{
    port_guest_set_exit_status(1);
    snprintf(s_guest_stop_reason, sizeof(s_guest_stop_reason),
             "unresolved host service: %s", symbol != NULL ? symbol : "unknown");
    port_trace_host_stop(s_guest_stop_reason);
    if (s_guest_escape_ready &&
        SDL_GetCurrentThreadID() == s_guest_thread_id)
        longjmp(s_guest_escape, 1);
    fprintf(stderr, "PORT fatal stub outside guest thread: %s\n", s_guest_stop_reason);
    abort();
}

void port_stub_fail(const char *symbol)
{
    port_guest_unwind(symbol);
}

void port_guest_set_step_limit(uint64_t limit)
{
    s_guest_step_limit = limit;
    s_guest_requested_stop_reason[0] = '\0';
    SDL_SetAtomicInt(&s_guest_stop_pending, 0);
}

void port_guest_request_stop(const char *reason)
{
    if (SDL_CompareAndSwapAtomicInt(&s_guest_stop_pending, 0, -1)) {
        snprintf(s_guest_requested_stop_reason,
                 sizeof(s_guest_requested_stop_reason), "%s",
                 reason != NULL ? reason : "host requested shutdown");
        port_trace_host_stop(s_guest_requested_stop_reason);
        SDL_SetAtomicInt(&s_guest_stop_pending, 1);
        port_guest_notify_activity();
    }
}

void port_guest_note_sim_step(uint64_t step_id)
{
    if (s_guest_step_limit != 0 && step_id >= s_guest_step_limit)
        port_guest_request_stop("requested simulation-step capture boundary reached");
}

void port_guest_check_stop(void)
{
    if (!s_guest_escape_ready || SDL_GetCurrentThreadID() != s_guest_thread_id)
        return;
    if (SDL_GetAtomicInt(&s_guest_stop_pending) != 1) {
        port_timer_pump();
        return;
    }
    snprintf(s_guest_stop_reason, sizeof(s_guest_stop_reason), "%s",
             s_guest_requested_stop_reason[0] != '\0'
                 ? s_guest_requested_stop_reason
                 : "host requested shutdown");
    longjmp(s_guest_escape, 1);
}

void port_guest_stop_after_publication(void)
{
    port_guest_check_stop();
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
    const char *trace_path = "";
    const char *asset_root = NULL;
    char default_asset_root[1024];
    char default_save_root[1024];
    int use_game_folder = 0;
    const char *capture_dir = "";
    const char *input_script = NULL;
    const char *test_startup_seed = NULL;
    const char *audio_driver = "ad15";
    const char *diagnostics_root = NULL;
    const char *config_path = NULL;
    char default_config_path[1024];
    int debug = 0;
    int run_ms = -1;
    int test_auto_protection = 0;
    uint64_t stop_after_sim_steps = 0;
    int i;
    uint64_t start_ns;
    uint64_t next_present_ns;
    int should_quit = 0;
    for (i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "--trace=", 8) == 0)
            trace_path = argv[i] + 8;
        else if (strncmp(argv[i], "--assets=", 9) == 0)
            asset_root = argv[i] + 9;
        else if (strncmp(argv[i], "--capture-dir=", 14) == 0)
            capture_dir = argv[i] + 14;
        else if (strncmp(argv[i], "--input-script=", 15) == 0)
            input_script = argv[i] + 15;
        else if (strncmp(argv[i], "--audio=", 8) == 0)
            audio_driver = argv[i] + 8;
        else if (strcmp(argv[i], "--debug") == 0)
            debug = 1;
        else if (strncmp(argv[i], "--diagnostics-dir=", 18) == 0)
            diagnostics_root = argv[i] + 18;
        else if (strncmp(argv[i], "--config=", 9) == 0)
            config_path = argv[i] + 9;
        else if (strncmp(argv[i], "--test-startup-seed=", 20) == 0)
            test_startup_seed = argv[i] + 20;
        else if (strcmp(argv[i], "--test-auto-protection") == 0)
            test_auto_protection = 1;
        else if (strncmp(argv[i], "--stop-after-sim-steps=", 23) == 0) {
            char *end = NULL;
            unsigned long long parsed = strtoull(argv[i] + 23, &end, 10);
            if (end == argv[i] + 23 || *end != '\0' || parsed == 0) {
                fprintf(stderr, "Invalid --stop-after-sim-steps value: %s\n",
                        argv[i] + 23);
                return 2;
            }
            stop_after_sim_steps = (uint64_t)parsed;
        }
        else if (parse_run_ms(argv[i]) >= 0)
            run_ms = parse_run_ms(argv[i]);
    }
    if (strcmp(audio_driver, "ad15") != 0 &&
        strcmp(audio_driver, "pc15") != 0 &&
        strcmp(audio_driver, "none") != 0) {
        fprintf(stderr, "Invalid --audio value: %s (use ad15, pc15 or none)\n",
                audio_driver);
        return 2;
    }
    /* Keep the driver's two-letter bank prefix and its implementation in
       agreement. The historical startup string has room for four characters. */
    memcpy(audiodriverstring, audio_driver, 5);
    if (asset_root == NULL)
        asset_root = getenv("STUNTS_ASSET_ROOT");
    if (asset_root == NULL || asset_root[0] == '\0') {
        const char *base_path = SDL_GetBasePath();
        SDL_PathInfo info;
        int length = base_path == NULL ? -1 :
            snprintf(default_asset_root, sizeof(default_asset_root),
                     "%sFONTDEF.FNT", base_path);
        if (length < 0 || (size_t)length >= sizeof(default_asset_root)) {
            fprintf(stderr, "Could not locate runtime assets: %s\n", SDL_GetError());
            return 1;
        }
        use_game_folder = SDL_GetPathInfo(default_asset_root, &info) &&
                          info.type == SDL_PATHTYPE_FILE;
        length = snprintf(default_asset_root, sizeof(default_asset_root),
                          "%s%s", base_path,
                          use_game_folder ? "" : "runtime/assets");
        if (length < 0 || (size_t)length >= sizeof(default_asset_root)) {
            fprintf(stderr, "Asset directory path is too long\n");
            return 1;
        }
        asset_root = default_asset_root;
    }
    port_diagnostics_init(debug, diagnostics_root);
    if (config_path == NULL) {
        const char *base = SDL_GetBasePath();
        int length = base == NULL ? -1 : snprintf(default_config_path,
            sizeof(default_config_path), "%sconfig.json", base);
        if (length < 0 || (size_t)length >= sizeof(default_config_path)) {
            port_diagnostics_close(2, "config path unavailable");
            return 2;
        }
        config_path = default_config_path;
    }
    port_config_load(config_path);
    port_diagnostics_note("config_path", config_path);
    port_diagnostics_note("manual_word_check",
        port_config_manual_word_check() ? "true" : "false");
    if (debug && trace_path[0] == '\0')
        trace_path = port_diagnostics_trace_path();
    port_diagnostics_note("asset_root", asset_root);
    port_diagnostics_note("audio_driver", audio_driver);
    port_diagnostics_note("trace_path", trace_path);
    port_diagnostics_note("input_script", input_script);
    port_diagnostics_note("startup_seed", test_startup_seed);
    if (!port_test_startup_seed_load(test_startup_seed)) {
        port_diagnostics_close(2, "invalid test startup seed");
        return 2;
    }
    port_runtime_set_asset_root(asset_root);
    if (use_game_folder) {
        int length = snprintf(default_save_root, sizeof(default_save_root),
                              "%ssaves", asset_root);
        if (length < 0 || (size_t)length >= sizeof(default_save_root) ||
            !port_runtime_set_save_root(default_save_root)) {
            fprintf(stderr, "Could not locate game-folder saves\n");
            port_diagnostics_close(1, "could not locate game-folder saves");
            return 1;
        }
    }
    port_sdl_init("Stunts 1.1 - SDL3 faithful port");
    port_video_set_capture_dir(capture_dir);
    port_trace_open(trace_path, port_runtime_asset_root());
    port_memory_init();
    port_input_enable_test_auto_protection(test_auto_protection);
    if (!port_input_script_load(input_script)) {
        port_trace_host_stop("invalid input script");
        port_trace_close();
        port_sdl_shutdown();
        port_diagnostics_close(1, "invalid input script");
        return 1;
    }
    port_guest_set_step_limit(stop_after_sim_steps);
    port_timer_start();
    SDL_SetAtomicInt(&s_guest_done, 0);
    s_guest_thread = SDL_CreateThread(guest_thread_main, "stunts-game", NULL);
    if (s_guest_thread == NULL) {
        fprintf(stderr, "Could not create guest thread: %s\n", SDL_GetError());
        port_trace_host_stop("guest thread creation failed");
        port_guest_set_exit_status(1);
        should_quit = 1;
    }
    start_ns = SDL_GetTicksNS();
    next_present_ns = start_ns;
    while (!should_quit && !SDL_GetAtomicInt(&s_guest_done)) {
        uint64_t now;
        port_input_script_pump(SDL_GetTicksNS());
        if (port_sdl_poll()) {
            port_guest_request_stop("SDL quit event");
            break;
        }
        port_video_present();
        next_present_ns += 16666667u;
        port_sdl_sleep_until(next_present_ns);
        now = SDL_GetTicksNS();
        if (run_ms >= 0 && now - start_ns >= (uint64_t)run_ms * 1000000u) {
            port_guest_request_stop("requested run duration reached");
            break;
        }
    }
    if (s_guest_thread != NULL) {
        /* Wait for a cooperative guest boundary before SDL teardown. */
        SDL_WaitThread(s_guest_thread, NULL);
        s_guest_thread = NULL;
    }
    port_timer_stop();
    port_audio_shutdown();
    port_trace_close();
    port_sdl_shutdown();
    port_diagnostics_close(SDL_GetAtomicInt(&s_guest_exit_status), s_guest_stop_reason);
    return SDL_GetAtomicInt(&s_guest_exit_status);
}
