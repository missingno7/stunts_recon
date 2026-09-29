#include "port_runtime.h"

#include <SDL3/SDL.h>

#define PORT_TIMER_CALLBACK_CAP 64u

static SDL_Thread *s_thread;
static SDL_Mutex *s_callback_lock;
static SDL_AtomicInt s_running;
static SDL_AtomicInt s_game_enabled;
static SDL_AtomicInt s_tick_count;
static void (*s_callbacks[PORT_TIMER_CALLBACK_CAP])(void);
static unsigned s_callback_count;
static uint64_t s_audio_wait_target;

static int timer_thread(void *unused)
{
    uint64_t origin = SDL_GetTicksNS();
    uint64_t tick_id = 0;
    uint64_t next = origin + PORT_TIMER_PERIOD_NS;
    (void)unused;
    while (SDL_GetAtomicInt(&s_running)) {
        uint64_t now = SDL_GetTicksNS();
        if (now < next) {
            SDL_DelayNS(next - now);
            continue;
        }
        do {
            ++tick_id;
            SDL_SetAtomicInt(&s_tick_count, (int)tick_id);
            port_trace_timer_tick(tick_id, next, now);
            next = origin + (tick_id + 1u) * PORT_TIMER_PERIOD_NS;
        } while (now >= next && SDL_GetAtomicInt(&s_running));
    }
    return 0;
}

void port_timer_start(void)
{
    if (s_thread != NULL)
        return;
    s_callback_lock = SDL_CreateMutex();
    SDL_SetAtomicInt(&s_running, 1);
    SDL_SetAtomicInt(&s_game_enabled, 0);
    SDL_SetAtomicInt(&s_tick_count, 0);
    s_callback_count = 0;
    s_thread = SDL_CreateThread(timer_thread, "stunts-pit-clock", NULL);
    if (s_thread == NULL)
        SDL_Log("Could not start timer thread: %s", SDL_GetError());
}

void port_timer_stop(void)
{
    if (s_thread != NULL) {
        SDL_SetAtomicInt(&s_running, 0);
        SDL_WaitThread(s_thread, NULL);
        s_thread = NULL;
    }
    if (s_callback_lock != NULL) {
        SDL_DestroyMutex(s_callback_lock);
        s_callback_lock = NULL;
    }
}

uint64_t port_timer_tick_count(void)
{
    int count = SDL_GetAtomicInt(&s_tick_count);
    return count < 0 ? 0 : (uint64_t)(unsigned)count;
}

int port_timer_game_enabled(void)
{
    return SDL_GetAtomicInt(&s_game_enabled) != 0;
}

void port_timer_mark_game_enabled(int enabled)
{
    SDL_SetAtomicInt(&s_game_enabled, enabled != 0);
}

/* M0 deadline service. Guest callback dispatch is intentionally not enabled
   until its interrupt/reentry boundary is recovered. */
void timer_setup_interrupt(void)
{
    port_timer_mark_game_enabled(1);
}

void timer_reg_callback(void (*callback)(void))
{
    unsigned i;
    if (callback == NULL || s_callback_lock == NULL)
        return;
    SDL_LockMutex(s_callback_lock);
    for (i = 0; i < s_callback_count; ++i) {
        if (s_callbacks[i] == callback) {
            SDL_UnlockMutex(s_callback_lock);
            return;
        }
    }
    if (s_callback_count < PORT_TIMER_CALLBACK_CAP)
        s_callbacks[s_callback_count++] = callback;
    SDL_UnlockMutex(s_callback_lock);
}

void timer_remove_callback(void (*callback)(void))
{
    unsigned i;
    if (callback == NULL || s_callback_lock == NULL)
        return;
    SDL_LockMutex(s_callback_lock);
    for (i = 0; i < s_callback_count; ++i) {
        if (s_callbacks[i] == callback) {
            s_callbacks[i] = s_callbacks[--s_callback_count];
            break;
        }
    }
    SDL_UnlockMutex(s_callback_lock);
}

void timer_copy_counter(int32_t ticks)
{
    uint64_t now = port_timer_tick_count();
    s_audio_wait_target = now + (uint32_t)ticks;
}

void timer_wait_for_dx(void)
{
    while (port_timer_tick_count() < s_audio_wait_target)
        SDL_DelayNS(1000000u);
}
