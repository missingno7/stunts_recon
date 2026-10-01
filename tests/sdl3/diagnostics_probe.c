#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <SDL3/SDL.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port_runtime.h"

static SDL_AtomicInt s_guest_ready;
static SDL_AtomicInt s_guest_go;
static DWORD s_guest_thread_id;

static const char *option_value(int argc, char **argv, const char *prefix)
{
    size_t prefix_length = strlen(prefix);
    int i;

    for (i = 1; i < argc; ++i) {
        if (strncmp(argv[i], prefix, prefix_length) == 0)
            return argv[i] + prefix_length;
    }
    return NULL;
}

static int has_option(int argc, char **argv, const char *option)
{
    int i;

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], option) == 0)
            return 1;
    }
    return 0;
}

static int write_expected_thread(const char *path, DWORD thread_id)
{
    FILE *file;
    int ok;

    if (path == NULL || path[0] == '\0')
        return 0;
    file = fopen(path, "wb");
    if (file == NULL)
        return 0;
    ok = fprintf(file, "%lu\n", (unsigned long)thread_id) > 0;
    if (fclose(file) != 0)
        ok = 0;
    return ok;
}

static int write_probe_trace(const char *path)
{
    FILE *file;
    int ok;

    if (path == NULL || path[0] == '\0')
        return 1;
    file = fopen(path, "wb");
    if (file == NULL)
        return 0;
    ok = fprintf(file, "{\"event_type\":\"header\",\"build_id\":\"%s\"}\n",
                 port_diagnostics_build_id()) > 0;
    if (fclose(file) != 0)
        ok = 0;
    return ok;
}

static void trigger_access_violation(void)
{
    volatile uint32_t *invalid = (volatile uint32_t *)(uintptr_t)1u;
    *invalid = 0xC0FFEEu;
}

static int SDLCALL guest_fault_thread(void *unused)
{
    (void)unused;
    s_guest_thread_id = GetCurrentThreadId();
    SDL_SetAtomicInt(&s_guest_ready, 1);
    while (SDL_GetAtomicInt(&s_guest_go) == 0)
        SDL_Delay(1);
    trigger_access_violation();
    return 0;
}

int main(int argc, char **argv)
{
    const char *mode = option_value(argc, argv, "--mode=");
    const char *diagnostics_root = option_value(argc, argv, "--diagnostics-dir=");
    const char *expected_thread_path =
        option_value(argc, argv, "--expected-thread-file=");
    const char *trace_override = option_value(argc, argv, "--trace=");
    const char *trace_path;
    int debug = has_option(argc, argv, "--debug");

    if (mode == NULL)
        mode = "normal";
    port_diagnostics_init(debug, diagnostics_root);
    trace_path = trace_override;
    if (debug && (trace_path == NULL || trace_path[0] == '\0'))
        trace_path = port_diagnostics_trace_path();
    if (trace_path == NULL)
        trace_path = "";
    port_diagnostics_note("probe_mode", mode);
    port_diagnostics_note("trace_path", trace_path);

    if (!SDL_Init(0)) {
        port_diagnostics_note("probe_error", SDL_GetError());
        port_diagnostics_close(90, "SDL_Init failed");
        return 90;
    }
    if (!write_probe_trace(trace_path)) {
        port_diagnostics_close(91, "could not write probe trace");
        SDL_Quit();
        return 91;
    }

    if (strcmp(mode, "main-fault") == 0) {
        if (!write_expected_thread(expected_thread_path, GetCurrentThreadId()))
            return 92;
        trigger_access_violation();
    } else if (strcmp(mode, "guest-fault") == 0) {
        SDL_Thread *thread;

        SDL_SetAtomicInt(&s_guest_ready, 0);
        SDL_SetAtomicInt(&s_guest_go, 0);
        thread = SDL_CreateThread(guest_fault_thread, "diagnostics-fault", NULL);
        if (thread == NULL)
            return 93;
        while (SDL_GetAtomicInt(&s_guest_ready) == 0)
            SDL_Delay(1);
        if (!write_expected_thread(expected_thread_path, s_guest_thread_id))
            return 94;
        SDL_SetAtomicInt(&s_guest_go, 1);
        SDL_WaitThread(thread, NULL);
        return 95;
    } else if (strcmp(mode, "abort") == 0) {
        if (!write_expected_thread(expected_thread_path, GetCurrentThreadId()))
            return 96;
        abort();
    } else if (strcmp(mode, "normal") != 0) {
        port_diagnostics_close(2, "unknown probe mode");
        SDL_Quit();
        return 2;
    }

    SDL_Quit();
    port_diagnostics_close(0, "probe normal exit");
    return 0;
}
