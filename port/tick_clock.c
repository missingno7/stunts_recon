#include "tick_clock.h"

#include <stddef.h>

void port_clock_init(PortTickClock *clock)
{
    unsigned i;
    atomic_init(&clock->raw_tick, 0);
    atomic_init(&clock->elapsed, 0);
    atomic_init(&clock->setup_generation, 0);
    atomic_init(&clock->enabled, 0);
    clock->producer_generation = 0;
    clock->divider = 5;
    clock->dispatched_tick = 0;
    clock->counter = clock->last_delta = 0;
    clock->copy_deadline = clock->input_deadline = 0;
    clock->dispatch_depth = 0;
    clock->callback_slot = -1;
    for (i = 0; i < PORT_CLOCK_CALLBACK_CAP; ++i)
        clock->callbacks[i] = NULL;
}

void port_clock_setup(PortTickClock *clock)
{
    clock->counter = clock->last_delta = 0;
    clock->dispatched_tick = atomic_load(&clock->raw_tick);
    atomic_fetch_add(&clock->setup_generation, 1);
    atomic_store(&clock->enabled, 1);
}

void port_clock_advance(PortTickClock *clock, uint32_t ticks)
{
    while (ticks-- != 0) {
        uint32_t generation = atomic_load(&clock->setup_generation);
        if (generation != clock->producer_generation) {
            clock->producer_generation = generation;
            clock->divider = 5;
        }
        /* Publish in the same order as the former SDL producer. */
        atomic_fetch_add(&clock->raw_tick, 1);
        if (atomic_load(&clock->enabled) && --clock->divider == 0) {
            atomic_store(&clock->elapsed,
                (uint32_t)(atomic_load(&clock->elapsed) + 1u));
            clock->divider = 5;
        }
    }
}

void port_clock_dispatch(PortTickClock *clock, const volatile uint16_t *input_pushed)
{
    uint64_t target = atomic_load(&clock->raw_tick);
    unsigned outer_depth = clock->dispatch_depth++;
    int saved_slot = clock->callback_slot;
    while (clock->dispatched_tick < target) {
        unsigned i;
        ++clock->dispatched_tick;
        /* Locked ISR: CMP BYTE PTR _input_pushed,0 (timerint.ASM:727).
           A nonzero high byte alone never suppresses callbacks. */
        if (!atomic_load(&clock->enabled) ||
            (input_pushed != NULL && (uint8_t)*input_pushed != 0))
            continue;
        ++clock->counter;
        if (outer_depth != 0)
            continue;
        for (i = 0; i < PORT_CLOCK_CALLBACK_CAP; ++i) {
            void (*callback)(void) = clock->callbacks[i];
            if (callback == NULL)
                break;
            clock->callback_slot = (int)i;
            callback();
        }
    }
    clock->callback_slot = saved_slot;
    --clock->dispatch_depth;
}

int port_clock_add(PortTickClock *clock, void (*callback)(void))
{
    unsigned i;
    if (callback == NULL)
        return 1;
    for (i = 0; i < PORT_CLOCK_CALLBACK_CAP; ++i)
        if (clock->callbacks[i] == NULL) {
            clock->callbacks[i] = callback;
            return 1;
        }
    return 0;
}

void port_clock_remove(PortTickClock *clock, void (*callback)(void))
{
    unsigned i;
    if (callback == NULL)
        return;
    for (i = 0; i < PORT_CLOCK_CALLBACK_CAP; ++i)
        if (clock->callbacks[i] == callback) {
            for (++i; i < PORT_CLOCK_CALLBACK_CAP; ++i)
                clock->callbacks[i - 1] = clock->callbacks[i];
            clock->callbacks[PORT_CLOCK_CALLBACK_CAP - 1] = NULL;
            break;
        }
}

uint32_t port_clock_delta(PortTickClock *clock)
{
    uint32_t delta = clock->counter - clock->last_delta;
    clock->last_delta = clock->counter;
    return delta;
}

void port_clock_seed_counter(PortTickClock *clock, uint32_t value)
{
    clock->counter = clock->last_delta = value;
}

void port_clock_set_copy(PortTickClock *clock, uint32_t ticks)
{
    clock->copy_deadline = port_clock_counter_deadline(clock, ticks);
}

int port_clock_test_copy(const PortTickClock *clock)
{
    return port_clock_counter_reached(clock, clock->copy_deadline);
}

uint32_t port_clock_counter_deadline(const PortTickClock *clock, uint32_t ticks)
{
    return clock->counter + ticks;
}

uint32_t port_clock_elapsed_deadline(const PortTickClock *clock, uint32_t ticks)
{
    return (uint32_t)atomic_load(&clock->elapsed) + ticks;
}

int port_clock_counter_reached(const PortTickClock *clock, uint32_t deadline)
{
    return clock->counter >= deadline;
}

void port_clock_set_input(PortTickClock *clock, uint32_t ticks)
{
    clock->input_deadline = port_clock_elapsed_deadline(clock, ticks);
}

int port_clock_bios_reached(uint32_t now, uint32_t deadline)
{
    return (now >> 16) >= (deadline >> 16) &&
           (now & 0xffffu) >= (deadline & 0xffffu);
}

int port_clock_test_input(const PortTickClock *clock)
{
    return port_clock_bios_reached((uint32_t)atomic_load(&clock->elapsed),
                                   clock->input_deadline);
}
