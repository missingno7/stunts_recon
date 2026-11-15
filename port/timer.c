#include "port_runtime.h"
#include "tick_clock.h"

#include <SDL3/SDL.h>

static SDL_Mutex *s_activity_lock;
static SDL_Condition *s_grant_changed, *s_yield_changed;
static SDL_ThreadID s_guest_thread_id;
static PortTickClock s_clock;
static uint64_t s_grant;
static int s_yielded, s_finished, s_cancelled;
static SDL_AtomicInt s_activity_generation;
extern volatile uint16_t input_pushed;

uint32_t port_guest_activity_snapshot(void)
{ return (uint32_t)SDL_GetAtomicInt(&s_activity_generation); }

void port_guest_notify_activity(void)
{
    SDL_AddAtomicInt(&s_activity_generation, 1);
}

void port_timer_cancel(void)
{
    if (!s_activity_lock) return;
    SDL_LockMutex(s_activity_lock);
    s_cancelled = 1;
    SDL_BroadcastCondition(s_grant_changed);
    SDL_UnlockMutex(s_activity_lock);
}

void port_timer_guest_begin(void)
{ s_guest_thread_id = SDL_GetCurrentThreadID(); }

void port_timer_guest_finish(void)
{
    SDL_LockMutex(s_activity_lock);
    s_finished = 1;
    SDL_BroadcastCondition(s_yield_changed);
    SDL_UnlockMutex(s_activity_lock);
}

/* Main-thread barrier. A timeout is a host watchdog, never a game timer. */
int port_timer_host_wait(void)
{
    int ready;
    SDL_LockMutex(s_activity_lock);
    while (!s_yielded && !s_finished) {
        if (!SDL_WaitConditionTimeout(s_yield_changed, s_activity_lock, 5000)) {
            SDL_UnlockMutex(s_activity_lock);
            port_guest_set_exit_status(1);
            port_guest_request_stop("guest did not reach a tick boundary within 5 seconds");
            return 0;
        }
    }
    ready = !s_finished;
    SDL_UnlockMutex(s_activity_lock);
    return ready;
}

/* The only production tick grant. Call after host_wait; duplicate or pipelined
   grants are rejected rather than draining a backlog with newer input. */
int port_timer_host_grant(void)
{
    int ok;
    SDL_LockMutex(s_activity_lock);
    ok = s_yielded && !s_finished && !s_cancelled;
    if (ok) {
        ++s_grant;
        s_yielded = 0;
        SDL_SignalCondition(s_grant_changed);
    }
    SDL_UnlockMutex(s_activity_lock);
    return ok;
}

uint64_t port_timer_raw_tick(void)
{ return atomic_load(&s_clock.raw_tick); }

void port_timer_pump(void)
{
    if (s_guest_thread_id && SDL_GetCurrentThreadID() == s_guest_thread_id)
        port_clock_dispatch(&s_clock, &input_pushed);
}

void port_guest_yield(void)
{
    uint64_t before;
    if (!s_activity_lock || SDL_GetCurrentThreadID() != s_guest_thread_id) return;
    port_guest_check_stop();
    before = port_timer_raw_tick();
    SDL_LockMutex(s_activity_lock);
    s_yielded = 1;
    SDL_SignalCondition(s_yield_changed);
    while (s_grant == before && !s_cancelled)
        SDL_WaitCondition(s_grant_changed, s_activity_lock);
    SDL_UnlockMutex(s_activity_lock);
    port_guest_check_stop();
    if (s_cancelled) return;
    port_clock_advance(&s_clock, 1);
    /* Script sequences precede live-event sequences on a same-tick tie. Both
       are queued before this grant and applied by this guest, never the host. */
    port_input_script_pump(port_timer_raw_tick() * PORT_TIMER_PERIOD_NS);
    port_input_apply_queued(port_timer_raw_tick());
    port_trace_timer_tick(port_timer_raw_tick(),
        port_timer_raw_tick() * PORT_TIMER_PERIOD_NS, SDL_GetTicksNS());
    port_timer_pump(); /* nested dispatch counts ticks but skips callback scan */
}

void port_guest_wait_for_activity(uint32_t observed)
{
    if (port_guest_activity_snapshot() == observed) port_guest_yield();
}

void port_guest_wait_until_ns(uint64_t deadline)
{
    while (port_timer_machine_time_ns() < deadline) port_guest_yield();
}

void port_timer_observation(uint64_t *tick, int *slot)
{
    *tick = s_clock.dispatched_tick;
    *slot = s_clock.dispatch_depth ? s_clock.callback_slot : -1;
}

void port_timer_start(void)
{
    port_clock_init(&s_clock);
    s_grant = 0; s_yielded = s_finished = s_cancelled = 0;
    s_guest_thread_id = 0;
    SDL_SetAtomicInt(&s_activity_generation, 0);
    s_activity_lock = SDL_CreateMutex();
    s_grant_changed = SDL_CreateCondition();
    s_yield_changed = SDL_CreateCondition();
    if (!s_activity_lock || !s_grant_changed || !s_yield_changed)
        port_guest_unwind("could not create tick handshake");
}

void port_timer_stop(void)
{
    s_guest_thread_id = 0;
    SDL_DestroyCondition(s_grant_changed);
    SDL_DestroyCondition(s_yield_changed);
    SDL_DestroyMutex(s_activity_lock);
    s_grant_changed = s_yield_changed = NULL;
    s_activity_lock = NULL;
}

uint64_t port_timer_tick_count(void)
{
    return (uint64_t)(uint32_t)atomic_load(&s_clock.elapsed);
}

uint64_t port_timer_machine_time_ns(void)
{
    return port_timer_raw_tick() * PORT_TIMER_PERIOD_NS;
}

int port_timer_game_enabled(void)
{
    return atomic_load(&s_clock.enabled) != 0;
}

void port_timer_mark_game_enabled(int enabled)
{
    atomic_store(&s_clock.enabled, enabled != 0);
}

void timer_setup_interrupt(void)
{
    s_guest_thread_id = SDL_GetCurrentThreadID();
    port_clock_setup(&s_clock);
}

void addtimer(void (*callback)(void))
{
    if (callback == NULL) return;
    port_timer_pump();
    if (!port_clock_add(&s_clock, callback))
        port_guest_unwind("No room left on timer interrupt routine list");
}

void deltimer(void (*callback)(void))
{
    if (callback == NULL) return;
    port_timer_pump();
    port_clock_remove(&s_clock, callback);
}

uint32_t elapsedticks(void)
{
    port_timer_pump();
    return port_clock_delta(&s_clock);
}

uint32_t gettick(void)
{
    port_timer_pump();
    return s_clock.counter;
}

void port_timer_test_seed_counter(uint32_t counter)
{
    port_timer_pump();
    port_clock_seed_counter(&s_clock, counter);
}

void timedwait(uint32_t ticks)
{
    uint32_t target;
    port_timer_pump();
    target = port_clock_counter_deadline(&s_clock, ticks);
    for (;;) {
        port_timer_pump();
        if (port_clock_counter_reached(&s_clock, target)) break;
        port_guest_check_stop();
        port_guest_yield();
    }
}

void setticks(int32_t ticks)
{
    port_timer_pump();
    port_clock_set_copy(&s_clock, (uint32_t)ticks);
}

/* Both the split-word line editor and audio driver waits use the callback
   counter in lib/timerhlp.ASM. */
void port_timer_copy_counter_words(uint16_t ticks_low, uint16_t ticks_high)
{
    uint32_t ticks = (uint32_t)ticks_low | ((uint32_t)ticks_high << 16);
    port_timer_pump();
    port_clock_set_copy(&s_clock, ticks);
}

int16_t testticks(void)
{
    port_timer_pump();
    return (int16_t)port_clock_test_copy(&s_clock);
}

/* C host translations of lib/gfxres.ASM:_set_add_value,
   _poll_input_abort and _wait_for_input_delay (lines 364-442). These read the
   separate 32-bit elapsed counter, incremented at each fifth raw IRQ. The
   original polling code tests both words independently, including its
   non-lexicographic behavior when the high word has already advanced. */
void setbiosticks(int32_t ticks)
{
    port_clock_set_input(&s_clock, (uint32_t)ticks);
}

int16_t testbiosticks(void)
{
    return (int16_t)port_clock_test_input(&s_clock);
}

void waitbios(int32_t ticks)
{
    uint32_t deadline = port_clock_elapsed_deadline(&s_clock, (uint32_t)ticks);
    while (1) {
        uint32_t now = (uint32_t)port_timer_tick_count();
        if (port_clock_bios_reached(now, deadline))
            break;
        port_guest_check_stop();
        port_guest_yield();
    }
}

void waitticks(void)
{
    while (gettick() < s_clock.copy_deadline) {
        port_guest_check_stop();
        port_guest_yield();
    }
}
