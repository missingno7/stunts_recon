#include "../port/port_runtime.h"

#include <SDL3/SDL.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void flush_stdin(void);
extern void keyboard_shift_checking1(void);
extern int8_t replay_axis_value(void);
extern int16_t joystick_flags_to_index(int16_t);
extern void reset_joystick_selection(void);
extern int16_t get_joy_flags(void);
extern uint8_t joystick_enabled;

static unsigned s_callback_count;
static unsigned s_stop_checks;
static SDL_AtomicInt s_cancel_requested;
static jmp_buf s_cancel_boundary;
static int s_cancel_armed;
static uint8_t s_last_codes[384];
static size_t s_last_code_count;
static double s_trace_u, s_trace_v;
static uint16_t s_trace_buttons;
static unsigned s_activity_wakes;

void port_guest_notify_activity(void) { ++s_activity_wakes; }

void port_guest_check_stop(void)
{
    ++s_stop_checks;
    if (s_cancel_armed && SDL_GetAtomicInt(&s_cancel_requested))
        longjmp(s_cancel_boundary, 1);
}

void port_guest_unwind(const char *symbol)
{
    fprintf(stderr, "unexpected guest unwind: %s\n", symbol);
    abort();
}

void port_trace_input_keyboard(uint64_t sequence, uint64_t scheduled_ns,
                               uint64_t actual_ns, const uint8_t *codes,
                               size_t code_count)
{
    (void)sequence;
    (void)scheduled_ns;
    (void)actual_ns;
    s_last_code_count = code_count;
    if (code_count <= sizeof(s_last_codes))
        memcpy(s_last_codes, codes, code_count);
}

void port_trace_input_mouse(uint64_t sequence, uint64_t scheduled_ns,
                            uint64_t actual_ns, double u, double v,
                            uint16_t buttons)
{
    (void)sequence; (void)scheduled_ns; (void)actual_ns;
    s_trace_u = u; s_trace_v = v; s_trace_buttons = buttons;
}

static void count_callback(void)
{
    ++s_callback_count;
}

static int flush_guest(void *unused)
{
    (void)unused;
    flush_stdin();
    return 0;
}

static int flush_guest_until_cancel(void *unused)
{
    (void)unused;
    if (setjmp(s_cancel_boundary) == 0) {
        s_cancel_armed = 1;
        flush_stdin();
        s_cancel_armed = 0;
        return 1;
    }
    s_cancel_armed = 0;
    return 0;
}

static int check(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAILED: %s\n", message);
        return 0;
    }
    return 1;
}

int main(void)
{
    int ok = 1;
    SDL_Thread *thread;
    int thread_result = 0;
    int16_t x;
    int16_t y;
    uint16_t buttons;

    if (!SDL_Init(SDL_INIT_EVENTS)) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 2;
    }
    port_input_init();

    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_UP, 0);
    ok &= check(s_activity_wakes != 0, "physical input wakes an idle guest");
    ok &= check(s_last_code_count == 2 && s_last_codes[0] == 0xE0 &&
                s_last_codes[1] == 0x48,
                "physical arrow press is recorded as extended DOS scans");
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_UP, 0);
    ok &= check(s_last_code_count == 2 && s_last_codes[1] == 0xC8,
                "physical arrow release is retained in diagnostics");
    port_input_handle_event(SDL_EVENT_MOUSE_MOTION, 160, 100);
    port_input_handle_event(SDL_EVENT_MOUSE_BUTTON_DOWN, 1, 0);
    ok &= check(s_trace_u == 0.5 && s_trace_v == 0.5 && s_trace_buttons == 1,
                "physical mouse records replay-compatible coordinates and buttons");
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_A, 0);
    port_input_handle_event(SDL_EVENT_WINDOW_FOCUS_LOST, 0, 0);
    ok &= check(s_last_code_count == 1 && s_last_codes[0] == 0x9E &&
                s_trace_buttons == 0,
                "focus loss records releases instead of leaving replay keys held");
    port_input_init();

    {
        static const int8_t expected_indices[16] = {
            0, 1, 5, 0, 3, 2, 4, 3, 7, 8, 6, 7, 0, 1, 5, 0
        };
        unsigned flags;
        for (flags = 0; flags < 16; ++flags)
            ok &= check(joystick_flags_to_index((int16_t)flags) ==
                            expected_indices[flags],
                        "joystick flags use the original 16-entry direction table");
        ok &= check(joystick_flags_to_index((int16_t)0x10) == 0,
                    "joystick direction lookup masks flags to the low nibble");
    }

    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_KP_4, 0);
    ok &= check(replay_axis_value() == -31,
                "keypad left maps to the original replay axis minimum");
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_KP_6, 0);
    ok &= check(replay_axis_value() == 0,
                "opposing keypad steering directions cancel");
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_KP_4, 0);
    ok &= check(replay_axis_value() == 33,
                "keypad right maps to the original replay axis maximum");
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_KP_6, 0);
    ok &= check(replay_axis_value() == 0,
                "replay axis returns to center when keypad steering is released");
    joystick_enabled = 1;
    reset_joystick_selection();
    ok &= check(joystick_enabled == 0 && get_joy_flags() == 0,
                "reset never reports an SDL joystick until a device backend exists");

    kb_init_interrupt();
    ok &= check(port_input_dos_scan(SDL_SCANCODE_UP) == 0xE048,
                "SDL Up maps to extended BIOS scan 48");
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_UP, 0);
    ok &= check(port_input_key_state(0x48) &&
                port_input_key_state(0xE048),
                "extended arrow updates DOS raw scan and extended state");
    port_input_handle_event(SDL_EVENT_WINDOW_FOCUS_LOST, 0, 0);
    ok &= check(!port_input_key_state(0x48) &&
                !port_input_key_state(0xE048),
                "focus loss releases held keyboard state");
    ok &= check(kb_read_char() == 0x4800,
                "extended arrow remains in the BIOS character queue");

    kb_init_interrupt();
    keyboard_shift_checking1();
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_KP_8, 0);
    ok &= check(kb_read_char() == '8', "Num Lock keypad 8 returns ASCII 8");
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_KP_8, 0);
    kb_shift_checking2();
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_KP_8, 0);
    ok &= check(kb_read_char() == 0x4800,
                "keypad 8 without Num Lock returns its navigation scan");
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_KP_8, 0);

    kb_init_interrupt();
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_LSHIFT, 0);
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_A, 0);
    ok &= check(kb_read_char() == 'A', "left Shift produces uppercase BIOS ASCII");
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_A, 0);
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_LSHIFT, 0);
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_RCTRL, 0);
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_A, 0);
    ok &= check(kb_read_char() == 1, "right Ctrl produces Ctrl-A");
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_A, 0);
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_RCTRL, 0);

    kb_reg_callback('x', count_callback);
    port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_X, 0);
    ok &= check(kb_get_char() == 0 && s_callback_count == 1,
                "registered character callback is dispatched once");
    port_input_handle_event(SDL_EVENT_KEY_UP, SDL_SCANCODE_X, 0);

    port_input_handle_event(SDL_EVENT_MOUSE_MOTION, 31, 47);
    port_input_handle_event(SDL_EVENT_MOUSE_BUTTON_DOWN, 1, 1);
    port_input_handle_event(SDL_EVENT_WINDOW_FOCUS_LOST, 0, 0);
    port_input_mouse_get(&x, &y, &buttons);
    ok &= check(x == 31 && y == 47 && buttons == 0,
                "focus loss releases mouse buttons and preserves pointer position");

    thread = SDL_CreateThread(flush_guest, "flush-stdin-test", NULL);
    if (thread == NULL) {
        fprintf(stderr, "SDL_CreateThread failed: %s\n", SDL_GetError());
        ok = 0;
    } else {
        SDL_DelayNS(10000000u);
        port_input_handle_event(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_RETURN, 0);
        SDL_WaitThread(thread, &thread_result);
        ok &= check(thread_result == 0 && kb_read_char() == 0,
                    "flush_stdin waits for then consumes one key");
        ok &= check(s_stop_checks != 0,
                    "flush_stdin reaches the cooperative stop checkpoint");
    }

    SDL_SetAtomicInt(&s_cancel_requested, 0);
    thread = SDL_CreateThread(flush_guest_until_cancel,
                              "flush-stdin-cancel-test", NULL);
    if (thread == NULL) {
        fprintf(stderr, "SDL_CreateThread failed: %s\n", SDL_GetError());
        ok = 0;
    } else {
        SDL_DelayNS(10000000u);
        SDL_SetAtomicInt(&s_cancel_requested, 1);
        SDL_WaitThread(thread, &thread_result);
        ok &= check(thread_result == 0,
                    "shutdown request releases a guest waiting in flush_stdin");
    }

    port_input_shutdown();
    SDL_Quit();
    if (ok)
        puts("SDL3 input adapter checks passed");
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
