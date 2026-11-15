#include "port_runtime.h"
#include "tick_clock.h"

#include <SDL3/SDL.h>

static SDL_Thread *s_thread;
static SDL_AtomicInt s_running;
static SDL_AtomicInt s_activity_generation;
static SDL_Mutex *s_activity_lock;
static SDL_Condition *s_activity_changed;
static SDL_ThreadID s_guest_thread_id;
static PortTickClock s_clock;
static uint64_t s_virtual_clock_origin_ns;
extern volatile uint16_t input_pushed;

uint32_t port_guest_activity_snapshot(void)
{
    return (uint32_t)SDL_GetAtomicInt(&s_activity_generation);
}

/* Publish after updating input, IRQ, or shutdown state. Pair the predicate
   and condition under one mutex so an event between polling and sleeping
   cannot be lost. No game callbacks execute on the producer threads. */
void port_guest_notify_activity(void)
{
    if (s_activity_lock != NULL)
        SDL_LockMutex(s_activity_lock);
    SDL_AddAtomicInt(&s_activity_generation, 1);
    if (s_activity_changed != NULL)
        SDL_BroadcastCondition(s_activity_changed);
    if (s_activity_lock != NULL)
        SDL_UnlockMutex(s_activity_lock);
}

void port_guest_wait_for_activity(uint32_t observed)
{
    if (s_guest_thread_id == 0 ||
        SDL_GetCurrentThreadID() != s_guest_thread_id || s_clock.dispatch_depth != 0)
        return;
    if (s_activity_lock != NULL && s_activity_changed != NULL) {
        SDL_LockMutex(s_activity_lock);
        if (port_guest_activity_snapshot() == observed &&
            s_clock.dispatched_tick == atomic_load(&s_clock.raw_tick))
            /* A bounded wait also keeps shutdown cooperative if the clock
               producer fails. Spurious wakes simply resume the original poll. */
            SDL_WaitConditionTimeout(s_activity_changed, s_activity_lock,
                (int32_t)((PORT_TIMER_PERIOD_NS + 999999u) / 1000000u));
        SDL_UnlockMutex(s_activity_lock);
    }
    /* The next original poll owns callback dispatch and timer-delta reads.
       Returning on shutdown lets that poll reach its cooperative stop check. */
}

/* The SDL clock thread publishes raw PIT ticks only. DOS timer callbacks run
   in the guest's address space and touch unsynchronized game/audio globals, so
   they must execute on the guest thread. The guest drains elapsed IRQs at its
   cooperative boundaries. */
void port_timer_pump(void)
{
    if (s_guest_thread_id != 0 &&
        SDL_GetCurrentThreadID() == s_guest_thread_id)
        port_clock_dispatch(&s_clock, &input_pushed);
}

/* Guest-dispatched epoch, not the producer's possibly-ahead raw publication.
   Sound requests never read a host clock or advance this observer. */
void port_timer_observation(uint64_t *tick, int *slot)
{
    *tick = s_clock.dispatched_tick;
    *slot = s_clock.dispatch_depth != 0 ? s_clock.callback_slot : -1;
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
            port_clock_advance(&s_clock, 1);
            tick_id = atomic_load(&s_clock.raw_tick);
            port_trace_timer_tick(tick_id, next, now);
            port_guest_notify_activity();
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
    s_activity_lock = SDL_CreateMutex();
    s_activity_changed = SDL_CreateCondition();
    SDL_SetAtomicInt(&s_activity_generation, 0);
    SDL_SetAtomicInt(&s_running, 1);
    port_clock_init(&s_clock);
    s_guest_thread_id = 0;
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
    s_guest_thread_id = 0;
    SDL_DestroyCondition(s_activity_changed);
    SDL_DestroyMutex(s_activity_lock);
    s_activity_changed = NULL;
    s_activity_lock = NULL;
}

uint64_t port_timer_tick_count(void)
{
    return (uint64_t)(uint32_t)atomic_load(&s_clock.elapsed);
}

uint64_t port_timer_machine_time_ns(void)
{
    uint64_t now = SDL_GetTicksNS();
    return now >= s_virtual_clock_origin_ns
        ? now - s_virtual_clock_origin_ns : 0;
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
        SDL_DelayNS(1000000u);
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
        SDL_DelayNS(1000000u);
    }
}

void waitticks(void)
{
    while (gettick() < s_clock.copy_deadline) {
        port_guest_check_stop();
        SDL_DelayNS(1000000u);
    }
}
