#ifndef STUNTS_TICK_CLOCK_H
#define STUNTS_TICK_CLOCK_H

#include <stdint.h>
#include <stdatomic.h>

#define PORT_CLOCK_CALLBACK_CAP 5u

/* One producer owns advance/divider; one guest owns dispatch and DOS services.
   No wall clock, SDL, allocation, or implicit progress. Atomic publication keeps
   the existing producer/guest arrangement; it is not a tick handshake. */
typedef struct PortTickClock {
    atomic_uint_least64_t raw_tick;
    atomic_uint_least32_t elapsed;
    atomic_uint_least32_t setup_generation;
    atomic_int enabled;
    uint32_t producer_generation;
    unsigned divider;
    uint64_t dispatched_tick;
    uint32_t counter, last_delta, copy_deadline, input_deadline;
    unsigned dispatch_depth;
    int callback_slot;
    void (*callbacks[PORT_CLOCK_CALLBACK_CAP])(void);
} PortTickClock;

void port_clock_init(PortTickClock *clock);
void port_clock_setup(PortTickClock *clock);
void port_clock_advance(PortTickClock *clock, uint32_t ticks);
/* Materialize only explicitly published ticks on the guest, sampling the live
   suppression byte as the old cooperative adapter did. Nested calls count
   ticks but never recursively scan the callback list. */
void port_clock_dispatch(PortTickClock *clock, const volatile uint16_t *input_pushed);
int port_clock_add(PortTickClock *clock, void (*callback)(void));
void port_clock_remove(PortTickClock *clock, void (*callback)(void));
uint32_t port_clock_delta(PortTickClock *clock);
void port_clock_seed_counter(PortTickClock *clock, uint32_t value);
uint32_t port_clock_counter_deadline(const PortTickClock *clock, uint32_t ticks);
uint32_t port_clock_elapsed_deadline(const PortTickClock *clock, uint32_t ticks);
int port_clock_counter_reached(const PortTickClock *clock, uint32_t deadline);
void port_clock_set_copy(PortTickClock *clock, uint32_t ticks);
int port_clock_test_copy(const PortTickClock *clock);
void port_clock_set_input(PortTickClock *clock, uint32_t ticks);
int port_clock_bios_reached(uint32_t now, uint32_t deadline);
int port_clock_test_input(const PortTickClock *clock);

#endif
