#include "../port/port_runtime.h"

#include <SDL3/SDL.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

volatile uint16_t input_pushed;

static char s_events[128];
static unsigned s_event_count;
static jmp_buf s_full_table;
static int s_expect_full_table;

void port_trace_timer_tick(uint64_t id, uint64_t due, uint64_t observed)
{
    (void)id; (void)due; (void)observed;
}

void port_guest_check_stop(void) {}

void port_guest_unwind(const char *reason)
{
    if (s_expect_full_table)
        longjmp(s_full_table, 1);
    fprintf(stderr, "unexpected guest unwind: %s\n", reason);
    abort();
}

/* Include the implementation so the test can inject raw IRQs without sleeps
   or a production-only test API. */
#include "../port/timer.c"

static int check(int condition, const char *message)
{
    if (!condition)
        fprintf(stderr, "FAILED: %s\n", message);
    return condition;
}

static void callback_a(void) { s_events[s_event_count++] = 'A'; }
static void callback_b(void) { s_events[s_event_count++] = 'B'; }
static void callback_c(void) { s_events[s_event_count++] = 'C'; }
static void callback_d(void) { s_events[s_event_count++] = 'D'; }
static void callback_e(void) { s_events[s_event_count++] = 'E'; }
static void callback_f(void) { s_events[s_event_count++] = 'F'; }
static void callback_remove_b(void)
{
    s_events[s_event_count++] = 'R';
    timer_remove_callback(callback_b);
}
static void callback_wait_tick(void)
{
    s_events[s_event_count++] = 'W';
    timer_get_counter_unk(1);
    s_events[s_event_count++] = 'w';
}

static int feed_nested_tick(void *unused)
{
    (void)unused;
    SDL_DelayNS(10000000u);
    SDL_AddAtomicInt(&s_tick_count, 1);
    return 0;
}

static int foreign_pump(void *unused)
{
    (void)unused;
    port_timer_pump();
    return 0;
}

static void raw_irq(unsigned count)
{
    SDL_AddAtomicInt(&s_tick_count, (int)count);
}

int main(void)
{
    SDL_Thread *foreign;
    int ok = 1;
    if (!SDL_Init(0)) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 2;
    }
    timer_setup_interrupt();
    timer_reg_callback(callback_a);
    timer_reg_callback(callback_b);
    timer_reg_callback(callback_c);
    raw_irq(2);
    foreign = SDL_CreateThread(foreign_pump, "foreign-pump", NULL);
    if (foreign == NULL) return 2;
    SDL_WaitThread(foreign, NULL);
    ok &= check(s_event_count == 0,
                "a foreign thread cannot run game callbacks");
    ok &= check(timer_get_counter() == 2,
                "each eligible raw IRQ advances the callback counter");
    ok &= check(s_event_count == 6 &&
                s_events[0] == 'A' && s_events[1] == 'B' && s_events[2] == 'C' &&
                s_events[3] == 'A' && s_events[4] == 'B' && s_events[5] == 'C',
                "callbacks follow slot order on every raw IRQ");

    input_pushed = 1;
    raw_irq(2);
    port_timer_pump();
    ok &= check(timer_get_counter() == 2 && s_event_count == 6,
                "input_pushed suppresses both callback count and dispatch");
    input_pushed = 0;

    timer_remove_callback(callback_a);
    timer_remove_callback(callback_b);
    timer_remove_callback(callback_c);
    s_event_count = 0;
    timer_reg_callback(callback_remove_b);
    timer_reg_callback(callback_b);
    timer_reg_callback(callback_c);
    raw_irq(1);
    port_timer_pump();
    ok &= check(s_event_count == 2 && s_events[0] == 'R' && s_events[1] == 'C',
                "ISR reads live slots after a callback removes the next slot");
    timer_remove_callback(callback_remove_b);
    timer_remove_callback(callback_c);

    s_event_count = 0;
    timer_reg_callback(callback_wait_tick);
    raw_irq(1);
    foreign = SDL_CreateThread(feed_nested_tick, "nested-irq", NULL);
    if (foreign == NULL) return 2;
    port_timer_pump();
    SDL_WaitThread(foreign, NULL);
    ok &= check(s_event_count == 2 && s_events[0] == 'W' &&
                s_events[1] == 'w',
                "nested IRQ advances wait counter without reentering callback");
    timer_remove_callback(callback_wait_tick);

    timer_reg_callback(callback_a);
    timer_reg_callback(callback_b);
    timer_reg_callback(callback_c);
    timer_reg_callback(callback_d);
    timer_reg_callback(callback_e);
    if (setjmp(s_full_table) == 0) {
        s_expect_full_table = 1;
        timer_reg_callback(callback_f);
        ok &= check(0, "sixth callback must be rejected");
    }
    s_expect_full_table = 0;
    timer_remove_callback(callback_a);
    timer_remove_callback(callback_b);
    timer_remove_callback(callback_c);
    timer_remove_callback(callback_d);
    timer_remove_callback(callback_e);

    port_timer_test_seed_counter(0xfffffffeu);
    port_timer_copy_counter_words(4, 0);
    ok &= check(timer_compare_dx() == 1,
                "DOS unsigned counter comparison treats wrapped target as passed");
    raw_irq(1);
    ok &= check(timer_get_counter() == 0xffffffffu,
                "callback counter wraps at 32 bits");
    raw_irq(1);
    ok &= check(timer_get_counter() == 0,
                "callback counter carries through 32-bit wrap");

    SDL_SetAtomicInt(&s_elapsed_tick_count, 0x00020000);
    set_add_value(0x100);
    SDL_SetAtomicInt(&s_elapsed_tick_count, 0x00030000);
    ok &= check(poll_input_abort() == 0,
                "input deadline retains original separate high/low comparison");
    SDL_SetAtomicInt(&s_elapsed_tick_count, 0x00030100);
    ok &= check(poll_input_abort() == 1,
                "input deadline completes when both words reach target");

    /* Check the real clock thread's divider and dispatch boundary. Freeze its
       producer before comparing counters so there is no scheduling race. */
    port_timer_start();
    timer_setup_interrupt();
    timer_reg_callback(callback_a);
    s_event_count = 0;
    SDL_DelayNS(75000000u);
    SDL_SetAtomicInt(&s_running, 0);
    SDL_WaitThread(s_thread, NULL);
    s_thread = NULL;
    {
        uint32_t raw = (uint32_t)SDL_GetAtomicInt(&s_tick_count);
        uint32_t elapsed = (uint32_t)port_timer_tick_count();
        ok &= check(raw >= 5 && elapsed == raw / 5,
                    "elapsed counter advances once for each five raw IRQs");
        ok &= check(s_event_count == 0 && SDL_GetAtomicInt(&s_callback_counter) == 0,
                    "clock thread never enters guest callback code");
        port_timer_pump();
        ok &= check(s_event_count == raw && timer_get_counter() == raw,
                    "guest pump delivers every pending raw callback tick");
    }
    port_timer_stop();

    SDL_Quit();
    if (ok)
        puts("SDL3 timer adapter checks passed");
    return ok ? 0 : 1;
}
