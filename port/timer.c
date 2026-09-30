#include "port_runtime.h"

#include <SDL3/SDL.h>

#define PORT_TIMER_CALLBACK_CAP 5u

static SDL_Thread *s_thread;
static SDL_Mutex *s_callback_lock;
static SDL_AtomicInt s_running;
static SDL_AtomicInt s_game_enabled;
static SDL_AtomicInt s_tick_count;
static SDL_AtomicInt s_callback_counter;
static void (*s_callbacks[PORT_TIMER_CALLBACK_CAP])(void);
static unsigned s_callback_count;
static uint64_t s_audio_wait_target;
static uint32_t s_copy_deadline;
static uint32_t s_input_deadline;
static uint32_t s_last_delta_counter;
static uint64_t s_virtual_clock_origin_ns;
extern volatile uint16_t input_pushed;

static void timer_dispatch_game_tick(void)
{
    void (*callbacks[PORT_TIMER_CALLBACK_CAP])(void);
    unsigned count;
    unsigned i;
    uint32_t current;
    if (!port_timer_game_enabled() || input_pushed != 0)
        return;
    current = (uint32_t)SDL_GetAtomicInt(&s_callback_counter) + 1u;
    SDL_SetAtomicInt(&s_callback_counter, (int32_t)current);
    if (s_callback_lock == NULL)
        return;
    SDL_LockMutex(s_callback_lock);
    count = s_callback_count;
    for (i = 0; i < count; ++i)
        callbacks[i] = s_callbacks[i];
    SDL_UnlockMutex(s_callback_lock);
    /* The DOS timer ISR invokes callbacks in registration order and allows
       callback code to register/remove services without holding its table. */
    for (i = 0; i < count; ++i)
        if (callbacks[i] != NULL)
            callbacks[i]();
}

static int timer_thread(void *unused)
{
    uint64_t origin = s_virtual_clock_origin_ns;
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
            timer_dispatch_game_tick();
            next = origin + (tick_id + 1u) * PORT_TIMER_PERIOD_NS;
        } while (now >= next && SDL_GetAtomicInt(&s_running));
    }
    return 0;
}

void port_timer_start(void)
{
    if (s_thread != NULL)
        return;
    s_virtual_clock_origin_ns = SDL_GetTicksNS();
    s_callback_lock = SDL_CreateMutex();
    SDL_SetAtomicInt(&s_running, 1);
    SDL_SetAtomicInt(&s_game_enabled, 0);
    SDL_SetAtomicInt(&s_tick_count, 0);
    SDL_SetAtomicInt(&s_callback_counter, 0);
    s_last_delta_counter = 0;
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

uint64_t port_timer_machine_time_ns(void)
{
    uint64_t now = SDL_GetTicksNS();
    return now >= s_virtual_clock_origin_ns
        ? now - s_virtual_clock_origin_ns : 0;
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
    SDL_SetAtomicInt(&s_callback_counter, 0);
    s_last_delta_counter = 0;
    port_timer_mark_game_enabled(1);
}

void timer_reg_callback(void (*callback)(void))
{
    unsigned i;
    if (callback == NULL || s_callback_lock == NULL)
        return;
    SDL_LockMutex(s_callback_lock);
    if (s_callback_count < PORT_TIMER_CALLBACK_CAP)
        s_callbacks[s_callback_count++] = callback;
    else {
        SDL_UnlockMutex(s_callback_lock);
        port_guest_unwind("No room left on timer interrupt routine list");
    }
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
            unsigned j;
            for (j = i + 1u; j < s_callback_count; ++j)
                s_callbacks[j - 1u] = s_callbacks[j];
            s_callbacks[--s_callback_count] = NULL;
            break;
        }
    }
    SDL_UnlockMutex(s_callback_lock);
}

/* Faithful state update of asm/timer_get_delta.ASM. Its 32-bit read and
   previous-value store occurred with IRQs masked; SDL atomics provide the
   same indivisible counter sample for the host timer thread. */
uint32_t timer_get_delta(void)
{
    uint32_t current = (uint32_t)SDL_GetAtomicInt(&s_callback_counter);
    uint32_t delta = current - s_last_delta_counter;
    s_last_delta_counter = current;
    return delta;
}

uint32_t timer_get_counter(void)
{
    return (uint32_t)SDL_GetAtomicInt(&s_callback_counter);
}

void timer_get_counter_unk(uint32_t ticks)
{
    uint32_t target = timer_get_counter() + ticks;
    while ((int32_t)(timer_get_counter() - target) < 0)
        SDL_DelayNS(1000000u);
}

void timer_copy_counter(int32_t ticks)
{
    uint64_t now = port_timer_tick_count();
    s_audio_wait_target = now + (uint32_t)ticks;
}

/* C host view of asm/timer_counter_deadline_helpers.ASM:_timer_copy_counter
   and _timer_compare_dx. These use the callback counter, while the separate
   audio wait adapter above uses the continuously advancing host PIT tick. */
void port_timer_copy_counter_words(uint16_t ticks_low, uint16_t ticks_high)
{
    uint32_t ticks = (uint32_t)ticks_low | ((uint32_t)ticks_high << 16);
    s_copy_deadline = timer_get_counter() + ticks;
}

int16_t timer_compare_dx(void)
{
    return (int16_t)((int32_t)(timer_get_counter() - s_copy_deadline) >= 0);
}

/* C host translations of asm/graphics_resource_runtime.ASM:_set_add_value,
   _poll_input_abort and _wait_for_input_delay (lines 364-442). The original
   helpers share the uninterrupted 99.99846 Hz elapsed-tick counter; the SDL
   timer thread supplies that same monotonically wrapping 32-bit tick domain. */
void set_add_value(int32_t ticks)
{
    s_input_deadline = (uint32_t)port_timer_tick_count() + (uint32_t)ticks;
}

int16_t poll_input_abort(void)
{
    return (int16_t)((uint32_t)port_timer_tick_count() >= s_input_deadline);
}

void wait_for_input_delay(int32_t ticks)
{
    uint32_t deadline = (uint32_t)port_timer_tick_count() + (uint32_t)ticks;
    while ((uint32_t)port_timer_tick_count() < deadline)
        SDL_DelayNS(1000000u);
}

void timer_wait_for_dx(void)
{
    while (port_timer_tick_count() < s_audio_wait_target)
        SDL_DelayNS(1000000u);
}
