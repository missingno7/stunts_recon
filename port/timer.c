#include "port_runtime.h"

#include <SDL3/SDL.h>

#define PORT_TIMER_CALLBACK_CAP 5u

static SDL_Thread *s_thread;
static SDL_AtomicInt s_running;
static SDL_AtomicInt s_game_enabled;
static SDL_AtomicInt s_tick_count;
static SDL_AtomicInt s_elapsed_tick_count;
static SDL_AtomicInt s_setup_generation;
static SDL_AtomicInt s_callback_counter;
static SDL_AtomicInt s_activity_generation;
static SDL_Mutex *s_activity_lock;
static SDL_Condition *s_activity_changed;
static void (*s_callbacks[PORT_TIMER_CALLBACK_CAP])(void);
static SDL_ThreadID s_guest_thread_id;
static uint32_t s_dispatched_tick_count;
static int s_dispatching;
static uint32_t s_copy_deadline;
static uint32_t s_input_deadline;
static uint32_t s_last_delta_counter;
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
        SDL_GetCurrentThreadID() != s_guest_thread_id || s_dispatching)
        return;
    if (s_activity_lock != NULL && s_activity_changed != NULL) {
        SDL_LockMutex(s_activity_lock);
        if (port_guest_activity_snapshot() == observed &&
            s_dispatched_tick_count == (uint32_t)SDL_GetAtomicInt(&s_tick_count))
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
    uint32_t target;
    int nested;
    if (s_guest_thread_id == 0 ||
        SDL_GetCurrentThreadID() != s_guest_thread_id)
        return;
    nested = s_dispatching;
    if (!nested)
        s_dispatching = 1;
    target = (uint32_t)SDL_GetAtomicInt(&s_tick_count);
    /* A callback may wait for a later tick. DOS has STI during callback
       execution: a nested IRQ advances the counter but its busy guard skips
       another callback scan. A nested pump does the same. */
    while ((int32_t)(target - s_dispatched_tick_count) > 0) {
        uint32_t current;
        unsigned i;
        ++s_dispatched_tick_count;
        if (!port_timer_game_enabled() || input_pushed != 0)
            continue;
        current = (uint32_t)SDL_GetAtomicInt(&s_callback_counter) + 1u;
        SDL_SetAtomicInt(&s_callback_counter, (int32_t)current);
        if (nested)
            continue;
        /* The ISR reads each live slot after the preceding callback returns.
           Registration uses the first empty slot; removal shifts the tail. */
        for (i = 0; i < PORT_TIMER_CALLBACK_CAP; ++i) {
            void (*callback)(void) = s_callbacks[i];
            if (callback == NULL)
                break;
            callback();
        }
    }
    if (!nested)
        s_dispatching = 0;
}

static int timer_thread(void *unused)
{
    uint64_t origin = s_virtual_clock_origin_ns;
    uint64_t tick_id = 0;
    uint64_t next = origin + PORT_TIMER_PERIOD_NS;
    uint32_t setup_generation = 0;
    unsigned divider = 5;
    (void)unused;
    while (SDL_GetAtomicInt(&s_running)) {
        uint64_t now = SDL_GetTicksNS();
        if (now < next) {
            SDL_DelayNS(next - now);
            continue;
        }
        do {
            uint32_t generation = (uint32_t)SDL_GetAtomicInt(&s_setup_generation);
            if (generation != setup_generation) {
                setup_generation = generation;
                divider = 5;
            }
            ++tick_id;
            SDL_SetAtomicInt(&s_tick_count, (int)tick_id);
            if (SDL_GetAtomicInt(&s_game_enabled) && --divider == 0) {
                SDL_AddAtomicInt(&s_elapsed_tick_count, 1);
                divider = 5;
            }
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
    SDL_SetAtomicInt(&s_game_enabled, 0);
    SDL_SetAtomicInt(&s_tick_count, 0);
    SDL_SetAtomicInt(&s_elapsed_tick_count, 0);
    SDL_SetAtomicInt(&s_setup_generation, 0);
    SDL_SetAtomicInt(&s_callback_counter, 0);
    s_last_delta_counter = 0;
    s_guest_thread_id = 0;
    s_dispatched_tick_count = 0;
    s_dispatching = 0;
    SDL_zeroa(s_callbacks);
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
    return (uint64_t)(uint32_t)SDL_GetAtomicInt(&s_elapsed_tick_count);
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

void timer_setup_interrupt(void)
{
    s_guest_thread_id = SDL_GetCurrentThreadID();
    SDL_SetAtomicInt(&s_callback_counter, 0);
    s_last_delta_counter = 0;
    s_dispatched_tick_count = (uint32_t)SDL_GetAtomicInt(&s_tick_count);
    SDL_AddAtomicInt(&s_setup_generation, 1);
    port_timer_mark_game_enabled(1);
}

void timer_reg_callback(void (*callback)(void))
{
    unsigned i;
    if (callback == NULL)
        return;
    port_timer_pump();
    for (i = 0; i < PORT_TIMER_CALLBACK_CAP; ++i) {
        if (s_callbacks[i] == NULL) {
            s_callbacks[i] = callback;
            return;
        }
    }
    port_guest_unwind("No room left on timer interrupt routine list");
}

void timer_remove_callback(void (*callback)(void))
{
    unsigned i;
    if (callback == NULL)
        return;
    port_timer_pump();
    for (i = 0; i < PORT_TIMER_CALLBACK_CAP; ++i) {
        if (s_callbacks[i] == callback) {
            unsigned j;
            for (j = i + 1u; j < PORT_TIMER_CALLBACK_CAP; ++j)
                s_callbacks[j - 1u] = s_callbacks[j];
            s_callbacks[PORT_TIMER_CALLBACK_CAP - 1u] = NULL;
            break;
        }
    }
}

/* Faithful state update of asm/timer_get_delta.ASM. Its 32-bit read and
   previous-value store occurred with IRQs masked. The guest pump serializes
   both operations with callback delivery. */
uint32_t timer_get_delta(void)
{
    port_timer_pump();
    uint32_t current = (uint32_t)SDL_GetAtomicInt(&s_callback_counter);
    uint32_t delta = current - s_last_delta_counter;
    s_last_delta_counter = current;
    return delta;
}

uint32_t timer_get_counter(void)
{
    port_timer_pump();
    return (uint32_t)SDL_GetAtomicInt(&s_callback_counter);
}

void port_timer_test_seed_counter(uint32_t counter)
{
    port_timer_pump();
    SDL_SetAtomicInt(&s_callback_counter, (int32_t)counter);
    s_last_delta_counter = counter;
}

void timer_get_counter_unk(uint32_t ticks)
{
    uint32_t target = timer_get_counter() + ticks;
    while (timer_get_counter() < target) {
        port_guest_check_stop();
        SDL_DelayNS(1000000u);
    }
}

void timer_copy_counter(int32_t ticks)
{
    s_copy_deadline = timer_get_counter() + (uint32_t)ticks;
}

/* Both the split-word line editor and audio driver waits use the callback
   counter in asm/timer_counter_deadline_helpers.ASM. */
void port_timer_copy_counter_words(uint16_t ticks_low, uint16_t ticks_high)
{
    uint32_t ticks = (uint32_t)ticks_low | ((uint32_t)ticks_high << 16);
    s_copy_deadline = timer_get_counter() + ticks;
}

int16_t timer_compare_dx(void)
{
    return (int16_t)(timer_get_counter() >= s_copy_deadline);
}

/* C host translations of asm/graphics_resource_runtime.ASM:_set_add_value,
   _poll_input_abort and _wait_for_input_delay (lines 364-442). These read the
   separate 32-bit elapsed counter, incremented at each fifth raw IRQ. The
   original polling code tests both words independently, including its
   non-lexicographic behavior when the high word has already advanced. */
void set_add_value(int32_t ticks)
{
    s_input_deadline = (uint32_t)port_timer_tick_count() + (uint32_t)ticks;
}

int16_t poll_input_abort(void)
{
    uint32_t now = (uint32_t)port_timer_tick_count();
    return (int16_t)((now >> 16) >= (s_input_deadline >> 16) &&
                     (now & 0xffffu) >= (s_input_deadline & 0xffffu));
}

void wait_for_input_delay(int32_t ticks)
{
    uint32_t deadline = (uint32_t)port_timer_tick_count() + (uint32_t)ticks;
    while (1) {
        uint32_t now = (uint32_t)port_timer_tick_count();
        if ((now >> 16) >= (deadline >> 16) &&
            (now & 0xffffu) >= (deadline & 0xffffu))
            break;
        port_guest_check_stop();
        SDL_DelayNS(1000000u);
    }
}

void timer_wait_for_dx(void)
{
    while (timer_get_counter() < s_copy_deadline) {
        port_guest_check_stop();
        SDL_DelayNS(1000000u);
    }
}
