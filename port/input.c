#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <string.h>

#define PORT_KEY_QUEUE_CAP 32u

/* These replace the original assembly-owned globals. The source image uses a
   word for input_pushed and a byte for joystick_enabled. */
volatile uint16_t input_pushed;
uint8_t joystick_enabled;

static uint8_t s_key_down[128];
static uint8_t s_extended_down[128];
#define PORT_KEY_CALLBACK_CAP 64u
static uint16_t s_callback_scan[PORT_KEY_CALLBACK_CAP];
static void (*s_callbacks[PORT_KEY_CALLBACK_CAP])(void);
static unsigned s_callback_count;
static SDL_Mutex *s_input_lock;
static uint16_t s_key_queue[PORT_KEY_QUEUE_CAP];
static unsigned s_key_head;
static unsigned s_key_tail;
static unsigned s_key_count;
static uint8_t s_caps_lock;
static uint8_t s_num_lock;
static uint8_t s_scroll_lock;
static uint8_t s_extended_prefix;
static int s_mouse_x;
static int s_mouse_y;
static uint8_t s_mouse_buttons;
static int s_mouse_min_x;
static int s_mouse_min_y;
static int s_mouse_max_x = PORT_SCREEN_WIDTH - 1;
static int s_mouse_max_y = PORT_SCREEN_HEIGHT - 1;
static int s_test_auto_protection;
static uint64_t s_live_input_sequence;
static uint64_t s_test_text_sequence;

static void input_lock(void)
{
    if (s_input_lock != NULL)
        SDL_LockMutex(s_input_lock);
}

static void input_unlock(void)
{
    if (s_input_lock != NULL)
        SDL_UnlockMutex(s_input_lock);
}

static void enqueue_bios_key(uint16_t key)
{
    if (s_key_count == PORT_KEY_QUEUE_CAP)
        return;
    s_key_queue[s_key_tail] = key;
    s_key_tail = (s_key_tail + 1u) % PORT_KEY_QUEUE_CAP;
    ++s_key_count;
}

static int shifted(void)
{
    return s_key_down[0x2Au] || s_key_down[0x36u];
}

static uint8_t ascii_for_scan(int scan)
{
    int shift = shifted();
    int control = s_key_down[0x1Du] || s_extended_down[0x1Du];
    int caps = s_caps_lock != 0;
    int keypad_numeric = (s_num_lock != 0) != shift;
    char lower = 0;
    switch (scan) {
    case 0x02: return (uint8_t)(shift ? '!' : '1');
    case 0x03: return (uint8_t)(shift ? '@' : '2');
    case 0x04: return (uint8_t)(shift ? '#' : '3');
    case 0x05: return (uint8_t)(shift ? '$' : '4');
    case 0x06: return (uint8_t)(shift ? '%' : '5');
    case 0x07: return (uint8_t)(shift ? '^' : '6');
    case 0x08: return (uint8_t)(shift ? '&' : '7');
    case 0x09: return (uint8_t)(shift ? '*' : '8');
    case 0x0A: return (uint8_t)(shift ? '(' : '9');
    case 0x0B: return (uint8_t)(shift ? ')' : '0');
    case 0x0C: return (uint8_t)(shift ? '_' : '-');
    case 0x0D: return (uint8_t)(shift ? '+' : '=');
    case 0x0F: return '\t';
    case 0x1A: return (uint8_t)(shift ? '{' : '[');
    case 0x1B: return (uint8_t)(shift ? '}' : ']');
    case 0x1C: return '\r';
    case 0x27: return (uint8_t)(shift ? ':' : ';');
    case 0x28: return (uint8_t)(shift ? '"' : '\'');
    case 0x29: return (uint8_t)(shift ? '~' : '`');
    case 0x2B: return (uint8_t)(shift ? '|' : '\\');
    case 0x33: return (uint8_t)(shift ? '<' : ',');
    case 0x34: return (uint8_t)(shift ? '>' : '.');
    case 0x35: return (uint8_t)(shift ? '?' : '/');
    case 0x37: return '*';
    case 0x39: return ' ';
    case 0x4A: return '-';
    case 0x4E: return '+';
    default: break;
    }
    if (scan == 0x0E) return '\b';
    if (scan == 0x01) return 0x1Bu;
    if (scan == 0x47) return (uint8_t)(keypad_numeric ? '7' : 0);
    if (scan == 0x48) return (uint8_t)(keypad_numeric ? '8' : 0);
    if (scan == 0x49) return (uint8_t)(keypad_numeric ? '9' : 0);
    if (scan == 0x4B) return (uint8_t)(keypad_numeric ? '4' : 0);
    if (scan == 0x4C) return (uint8_t)(keypad_numeric ? '5' : 0);
    if (scan == 0x4D) return (uint8_t)(keypad_numeric ? '6' : 0);
    if (scan == 0x4F) return (uint8_t)(keypad_numeric ? '1' : 0);
    if (scan == 0x50) return (uint8_t)(keypad_numeric ? '2' : 0);
    if (scan == 0x51) return (uint8_t)(keypad_numeric ? '3' : 0);
    if (scan == 0x52) return (uint8_t)(keypad_numeric ? '0' : 0);
    if (scan == 0x53) return (uint8_t)(keypad_numeric ? '.' : 0);
    switch (scan) {
    case 0x10: lower = 'q'; break; case 0x11: lower = 'w'; break;
    case 0x12: lower = 'e'; break; case 0x13: lower = 'r'; break;
    case 0x14: lower = 't'; break; case 0x15: lower = 'y'; break;
    case 0x16: lower = 'u'; break; case 0x17: lower = 'i'; break;
    case 0x18: lower = 'o'; break; case 0x19: lower = 'p'; break;
    case 0x1E: lower = 'a'; break; case 0x1F: lower = 's'; break;
    case 0x20: lower = 'd'; break; case 0x21: lower = 'f'; break;
    case 0x22: lower = 'g'; break; case 0x23: lower = 'h'; break;
    case 0x24: lower = 'j'; break; case 0x25: lower = 'k'; break;
    case 0x26: lower = 'l'; break; case 0x2C: lower = 'z'; break;
    case 0x2D: lower = 'x'; break; case 0x2E: lower = 'c'; break;
    case 0x2F: lower = 'v'; break; case 0x30: lower = 'b'; break;
    case 0x31: lower = 'n'; break; case 0x32: lower = 'm'; break;
    default: break;
    }
    if (lower == 0)
        return 0;
    if (control)
        return (uint8_t)(lower - 'a' + 1);
    return (uint8_t)((shift != caps) ? lower - 'a' + 'A' : lower);
}

static uint16_t pop_bios_key(int *found)
{
    uint16_t key = 0;
    input_lock();
    *found = s_key_count != 0;
    if (*found) {
        key = s_key_queue[s_key_head];
        s_key_head = (s_key_head + 1u) % PORT_KEY_QUEUE_CAP;
        --s_key_count;
    }
    input_unlock();
    return key;
}

int port_input_dos_scan(int scancode)
{
    switch ((SDL_Scancode)scancode) {
    case SDL_SCANCODE_ESCAPE: return 0x01;
    case SDL_SCANCODE_1: return 0x02; case SDL_SCANCODE_2: return 0x03;
    case SDL_SCANCODE_3: return 0x04; case SDL_SCANCODE_4: return 0x05;
    case SDL_SCANCODE_5: return 0x06; case SDL_SCANCODE_6: return 0x07;
    case SDL_SCANCODE_7: return 0x08; case SDL_SCANCODE_8: return 0x09;
    case SDL_SCANCODE_9: return 0x0A; case SDL_SCANCODE_0: return 0x0B;
    case SDL_SCANCODE_MINUS: return 0x0C; case SDL_SCANCODE_EQUALS: return 0x0D;
    case SDL_SCANCODE_BACKSPACE: return 0x0E; case SDL_SCANCODE_TAB: return 0x0F;
    case SDL_SCANCODE_Q: return 0x10; case SDL_SCANCODE_W: return 0x11;
    case SDL_SCANCODE_E: return 0x12; case SDL_SCANCODE_R: return 0x13;
    case SDL_SCANCODE_T: return 0x14; case SDL_SCANCODE_Y: return 0x15;
    case SDL_SCANCODE_U: return 0x16; case SDL_SCANCODE_I: return 0x17;
    case SDL_SCANCODE_O: return 0x18; case SDL_SCANCODE_P: return 0x19;
    case SDL_SCANCODE_LEFTBRACKET: return 0x1A; case SDL_SCANCODE_RIGHTBRACKET: return 0x1B;
    case SDL_SCANCODE_RETURN: return 0x1C; case SDL_SCANCODE_LCTRL: return 0x1D;
    case SDL_SCANCODE_A: return 0x1E; case SDL_SCANCODE_S: return 0x1F;
    case SDL_SCANCODE_D: return 0x20; case SDL_SCANCODE_F: return 0x21;
    case SDL_SCANCODE_G: return 0x22; case SDL_SCANCODE_H: return 0x23;
    case SDL_SCANCODE_J: return 0x24; case SDL_SCANCODE_K: return 0x25;
    case SDL_SCANCODE_L: return 0x26; case SDL_SCANCODE_SEMICOLON: return 0x27;
    case SDL_SCANCODE_APOSTROPHE: return 0x28; case SDL_SCANCODE_GRAVE: return 0x29;
    case SDL_SCANCODE_LSHIFT: return 0x2A; case SDL_SCANCODE_BACKSLASH: return 0x2B;
    case SDL_SCANCODE_Z: return 0x2C; case SDL_SCANCODE_X: return 0x2D;
    case SDL_SCANCODE_C: return 0x2E; case SDL_SCANCODE_V: return 0x2F;
    case SDL_SCANCODE_B: return 0x30; case SDL_SCANCODE_N: return 0x31;
    case SDL_SCANCODE_M: return 0x32; case SDL_SCANCODE_COMMA: return 0x33;
    case SDL_SCANCODE_PERIOD: return 0x34; case SDL_SCANCODE_SLASH: return 0x35;
    case SDL_SCANCODE_RSHIFT: return 0x36; case SDL_SCANCODE_KP_MULTIPLY: return 0x37;
    case SDL_SCANCODE_LALT: return 0x38; case SDL_SCANCODE_SPACE: return 0x39;
    case SDL_SCANCODE_CAPSLOCK: return 0x3A; case SDL_SCANCODE_F1: return 0x3B;
    case SDL_SCANCODE_F2: return 0x3C; case SDL_SCANCODE_F3: return 0x3D;
    case SDL_SCANCODE_F4: return 0x3E; case SDL_SCANCODE_F5: return 0x3F;
    case SDL_SCANCODE_F6: return 0x40; case SDL_SCANCODE_F7: return 0x41;
    case SDL_SCANCODE_F8: return 0x42; case SDL_SCANCODE_F9: return 0x43;
    case SDL_SCANCODE_F10: return 0x44; case SDL_SCANCODE_NUMLOCKCLEAR: return 0x45;
    case SDL_SCANCODE_SCROLLLOCK: return 0x46; case SDL_SCANCODE_KP_7: return 0x47;
    case SDL_SCANCODE_KP_8: return 0x48; case SDL_SCANCODE_KP_9: return 0x49;
    case SDL_SCANCODE_KP_MINUS: return 0x4A; case SDL_SCANCODE_KP_4: return 0x4B;
    case SDL_SCANCODE_KP_5: return 0x4C; case SDL_SCANCODE_KP_6: return 0x4D;
    case SDL_SCANCODE_KP_PLUS: return 0x4E; case SDL_SCANCODE_KP_1: return 0x4F;
    case SDL_SCANCODE_KP_2: return 0x50; case SDL_SCANCODE_KP_3: return 0x51;
    case SDL_SCANCODE_KP_0: return 0x52; case SDL_SCANCODE_KP_PERIOD: return 0x53;
    case SDL_SCANCODE_F11: return 0x57; case SDL_SCANCODE_F12: return 0x58;
    case SDL_SCANCODE_KP_ENTER: return 0xE01C;
    case SDL_SCANCODE_KP_DIVIDE: return 0xE035;
    case SDL_SCANCODE_RCTRL: return 0xE01D;
    case SDL_SCANCODE_RALT: return 0xE038;
    case SDL_SCANCODE_PRINTSCREEN: return 0xE037;
    case SDL_SCANCODE_LGUI: return 0xE05B;
    case SDL_SCANCODE_RGUI: return 0xE05C;
    case SDL_SCANCODE_APPLICATION: return 0xE05D;
    case SDL_SCANCODE_UP: return 0xE048; case SDL_SCANCODE_LEFT: return 0xE04B;
    case SDL_SCANCODE_RIGHT: return 0xE04D; case SDL_SCANCODE_DOWN: return 0xE050;
    case SDL_SCANCODE_HOME: return 0xE047; case SDL_SCANCODE_END: return 0xE04F;
    case SDL_SCANCODE_PAGEUP: return 0xE049; case SDL_SCANCODE_PAGEDOWN: return 0xE051;
    case SDL_SCANCODE_INSERT: return 0xE052; case SDL_SCANCODE_DELETE: return 0xE053;
    default: return 0;
    }
}

void port_input_init(void)
{
    SDL_Keymod modifiers;
    if (s_input_lock == NULL)
        s_input_lock = SDL_CreateMutex();
    modifiers = SDL_GetModState();
    input_lock();
    memset(s_key_down, 0, sizeof(s_key_down));
    memset(s_extended_down, 0, sizeof(s_extended_down));
    memset(s_key_queue, 0, sizeof(s_key_queue));
    s_key_head = 0;
    s_key_tail = 0;
    s_key_count = 0;
    s_caps_lock = (modifiers & SDL_KMOD_CAPS) != 0;
    s_num_lock = (modifiers & SDL_KMOD_NUM) != 0;
    s_scroll_lock = 0;
    s_extended_prefix = 0;
    s_callback_count = 0;
    s_test_auto_protection = 0;
    s_live_input_sequence = 0;
    s_test_text_sequence = 0;
    s_mouse_x = 0;
    s_mouse_y = 0;
    s_mouse_buttons = 0;
    s_mouse_min_x = 0;
    s_mouse_min_y = 0;
    s_mouse_max_x = PORT_SCREEN_WIDTH - 1;
    s_mouse_max_y = PORT_SCREEN_HEIGHT - 1;
    input_unlock();
}

void port_input_enable_test_auto_protection(int enabled)
{
    s_test_auto_protection = enabled != 0;
}

int port_input_test_auto_protection_enabled(void)
{
    return s_test_auto_protection;
}

static int test_text_scan(unsigned char character)
{
    if (character >= 'A' && character <= 'Z')
        character = (unsigned char)(character - 'A' + 'a');
    if (character >= 'a' && character <= 'z') {
        static const uint8_t scans[26] = {
            0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17,
            0x24, 0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13,
            0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c
        };
        return scans[character - 'a'];
    }
    if (character >= '1' && character <= '9')
        return 0x02 + (character - '1');
    if (character == '0') return 0x0b;
    if (character == ' ') return 0x39;
    if (character == '-') return 0x0c;
    if (character == '.') return 0x34;
    return 0;
}

static int is_lock_or_modifier_scan(int scan, int extended)
{
    if (extended)
        return scan == 0x1Du || scan == 0x38u ||
               scan == 0x5Bu || scan == 0x5Cu || scan == 0x5Du;
    return scan == 0x2Au || scan == 0x36u || scan == 0x1Du ||
           scan == 0x38u || scan == 0x3Au || scan == 0x45u ||
           scan == 0x46u;
}

static void trace_live_mouse(void)
{
    int x, y;
    uint16_t buttons;
    uint64_t now = SDL_GetTicksNS();
    input_lock();
    x = s_mouse_x;
    y = s_mouse_y;
    buttons = s_mouse_buttons;
    input_unlock();
    port_trace_input_mouse(s_live_input_sequence++, now, now,
                          (double)x / PORT_SCREEN_WIDTH,
                          (double)y / PORT_SCREEN_HEIGHT, buttons);
}

static void clear_pressed_input(void)
{
    uint8_t releases[128u * 3u];
    size_t count = 0;
    unsigned scan;
    uint64_t now;
    input_lock();
    for (scan = 1; scan < 128; ++scan) {
        if (s_key_down[scan])
            releases[count++] = (uint8_t)(scan | 0x80u);
        if (s_extended_down[scan]) {
            releases[count++] = 0xE0;
            releases[count++] = (uint8_t)(scan | 0x80u);
        }
    }
    memset(s_key_down, 0, sizeof(s_key_down));
    memset(s_extended_down, 0, sizeof(s_extended_down));
    s_extended_prefix = 0;
    s_mouse_buttons = 0;
    input_unlock();
    now = SDL_GetTicksNS();
    if (count != 0)
        port_trace_input_keyboard(s_live_input_sequence++, now, now, releases, count);
    trace_live_mouse();
}

/* Feed a manual answer through the same raw DOS keyboard path as a tester.
   Only this explicit test option enables the helper, and the trace stores key
   scans rather than the answer text. */
int port_input_type_test_text(const char *text)
{
    uint8_t codes[48];
    size_t count = 0;
    size_t i;
    uint64_t now;
    if (!s_test_auto_protection || text == NULL)
        return 0;
    for (i = 0; text[i] != '\0'; ++i) {
        int scan = test_text_scan((unsigned char)text[i]);
        if (scan == 0 || count + 2u > sizeof(codes) - 2u) {
            port_guest_unwind("test protection answer contains unsupported or excessive text");
            return 0;
        }
        codes[count++] = (uint8_t)scan;
        codes[count++] = (uint8_t)(scan | 0x80);
    }
    if (count + 2u > sizeof(codes)) {
        port_guest_unwind("test protection answer is too long");
        return 0;
    }
    codes[count++] = 0x1c;
    codes[count++] = 0x9c;
    for (i = 0; i < count; ++i)
        port_input_apply_dos_scancode(codes[i]);
    now = SDL_GetTicksNS();
    port_trace_input_keyboard(s_test_text_sequence++, now, now, codes, count);
    return 1;
}

void port_input_handle_event(int event_type, int code, int value)
{
    if (event_type == SDL_EVENT_KEY_DOWN || event_type == SDL_EVENT_KEY_UP) {
        int dos_code = port_input_dos_scan(code);
        int down = event_type == SDL_EVENT_KEY_DOWN;
        int scan = dos_code & 0xFF;
        input_lock();
        if (dos_code >= 0xE000) {
            int was_down = s_extended_down[dos_code & 0x7Fu] != 0;
            s_extended_down[dos_code & 0x7Fu] = (uint8_t)down;
            if (scan > 0 && scan < 128)
                s_key_down[scan] = (uint8_t)down;
            if (down && (!was_down || value != 0) &&
                !is_lock_or_modifier_scan(scan, 1)) {
                uint8_t ascii = scan == 0x1Cu ? '\r' :
                                scan == 0x35u ? '/' : 0;
                enqueue_bios_key((uint16_t)((scan << 8) | ascii));
            }
        } else if (dos_code > 0 && dos_code < 128) {
            int was_down = s_key_down[dos_code] != 0;
            s_key_down[dos_code] = (uint8_t)down;
            if (down && !was_down && dos_code == 0x3A)
                s_caps_lock ^= 1u;
            if (down && !was_down && dos_code == 0x45)
                s_num_lock ^= 1u;
            if (down && !was_down && dos_code == 0x46)
                s_scroll_lock ^= 1u;
            if (down && (!was_down || value != 0)) {
                uint8_t ascii = ascii_for_scan(dos_code);
                if (!is_lock_or_modifier_scan(dos_code, 0))
                    enqueue_bios_key((uint16_t)((dos_code << 8) | ascii));
            }
        }
        input_unlock();
        if (dos_code > 0) {
            uint8_t codes[2];
            size_t count = 0;
            uint64_t now = SDL_GetTicksNS();
            if (dos_code >= 0xE000)
                codes[count++] = 0xE0;
            codes[count++] = (uint8_t)((dos_code & 0xFF) | (down ? 0 : 0x80));
            port_trace_input_keyboard(s_live_input_sequence++, now, now, codes, count);
        }
    } else if (event_type == SDL_EVENT_MOUSE_MOTION) {
        input_lock();
        s_mouse_x = code < s_mouse_min_x ? s_mouse_min_x : code;
        s_mouse_y = value < s_mouse_min_y ? s_mouse_min_y : value;
        if (s_mouse_x > s_mouse_max_x) s_mouse_x = s_mouse_max_x;
        if (s_mouse_y > s_mouse_max_y) s_mouse_y = s_mouse_max_y;
        input_unlock();
        trace_live_mouse();
    } else if (event_type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
               event_type == SDL_EVENT_MOUSE_BUTTON_UP) {
        uint8_t bit = code == 1 ? 1u : code == 2 ? 4u : code == 3 ? 2u : 0u;
        input_lock();
        if (event_type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            s_mouse_buttons |= bit;
        else
            s_mouse_buttons &= (uint8_t)~bit;
        input_unlock();
        trace_live_mouse();
    } else if (event_type == SDL_EVENT_WINDOW_FOCUS_LOST) {
        clear_pressed_input();
    }
}

/* Apply the raw Set-1 bytes used by Port Forge's exact DOS input channel.
   E0-prefixed navigation keys stay distinct from keypad keys. */
void port_input_apply_dos_scancode(uint8_t code)
{
    int extended;
    int scan;
    int down;
    int was_down;
    uint16_t key;

    if (code == 0xE0u) {
        s_extended_prefix = 1;
        return;
    }
    if (code == 0xE1u) {
        s_extended_prefix = 0;
        return;
    }
    extended = s_extended_prefix != 0;
    s_extended_prefix = 0;
    scan = code & 0x7Fu;
    down = (code & 0x80u) == 0;
    if (scan == 0 || scan >= 128)
        return;

    input_lock();
    if (extended) {
        was_down = s_extended_down[scan] != 0;
        s_extended_down[scan] = (uint8_t)down;
        if (scan < 128)
            s_key_down[scan] = (uint8_t)down;
        if (down && !was_down && !is_lock_or_modifier_scan(scan, 1)) {
            uint8_t ascii = scan == 0x1Cu ? '\r' :
                            scan == 0x35u ? '/' : 0;
            enqueue_bios_key((uint16_t)((scan << 8) | ascii));
        }
    } else {
        was_down = s_key_down[scan] != 0;
        s_key_down[scan] = (uint8_t)down;
        if (down && !was_down && scan == 0x3Au)
            s_caps_lock ^= 1u;
        if (down && !was_down && scan == 0x45u)
            s_num_lock ^= 1u;
        if (down && !was_down && scan == 0x46u)
            s_scroll_lock ^= 1u;
        if (down && !was_down) {
            uint8_t ascii = ascii_for_scan(scan);
            key = (uint16_t)(scan << 8) | ascii;
            if (scan != 0 && !is_lock_or_modifier_scan(scan, 0))
                enqueue_bios_key(key);
        }
    }
    input_unlock();
}

uint8_t port_input_key_state(uint16_t dos_scan)
{
    uint8_t state = 0;
    input_lock();
    if (dos_scan < 128)
        state = s_key_down[dos_scan];
    else if ((dos_scan & 0xFF00u) == 0xE000u)
        state = s_extended_down[dos_scan & 0x7Fu];
    input_unlock();
    return state;
}

void port_input_mouse_get(int16_t *x, int16_t *y, uint16_t *buttons)
{
    input_lock();
    if (x != NULL) *x = (int16_t)s_mouse_x;
    if (y != NULL) *y = (int16_t)s_mouse_y;
    if (buttons != NULL) *buttons = (uint16_t)s_mouse_buttons;
    input_unlock();
}

void port_input_mouse_set(int16_t x, int16_t y)
{
    input_lock();
    s_mouse_x = x < s_mouse_min_x ? s_mouse_min_x : x;
    s_mouse_y = y < s_mouse_min_y ? s_mouse_min_y : y;
    if (s_mouse_x > s_mouse_max_x) s_mouse_x = s_mouse_max_x;
    if (s_mouse_y > s_mouse_max_y) s_mouse_y = s_mouse_max_y;
    input_unlock();
}

void port_input_mouse_set_buttons(uint16_t buttons)
{
    input_lock();
    s_mouse_buttons = (uint8_t)(buttons & 7u);
    input_unlock();
}

void port_input_mouse_set_x_bounds(int16_t min_x, int16_t max_x)
{
    input_lock();
    s_mouse_min_x = min_x;
    s_mouse_max_x = max_x;
    if (s_mouse_x < s_mouse_min_x) s_mouse_x = s_mouse_min_x;
    if (s_mouse_x > s_mouse_max_x) s_mouse_x = s_mouse_max_x;
    input_unlock();
}

void port_input_mouse_set_y_bounds(int16_t min_y, int16_t max_y)
{
    input_lock();
    s_mouse_min_y = min_y;
    s_mouse_max_y = max_y;
    if (s_mouse_y < s_mouse_min_y) s_mouse_y = s_mouse_min_y;
    if (s_mouse_y > s_mouse_max_y) s_mouse_y = s_mouse_max_y;
    input_unlock();
}

void kb_init_interrupt(void)
{
    input_lock();
    memset(s_key_down, 0, sizeof(s_key_down));
    memset(s_extended_down, 0, sizeof(s_extended_down));
    s_extended_prefix = 0;
    s_key_head = s_key_tail = s_key_count = 0;
    input_unlock();
}

void port_input_shutdown(void)
{
    clear_pressed_input();
    if (s_input_lock != NULL) {
        SDL_DestroyMutex(s_input_lock);
        s_input_lock = NULL;
    }
}

/* Port replacement for asm/keyboard_input_callbacks.ASM's lcall [0x468c].
   The host's BIOS-key queue is the configured DOS reader callback target. */
int16_t kb_call_readchar_callback(void)
{
    return kb_read_char();
}

/* The legacy service peeks at the BIOS queue, waits until a key is available,
   and consumes that one key. Keep waiting cooperatively so host shutdown can
   stop the guest instead of stranding SDL teardown in SDL_WaitThread. */
void flush_stdin(void)
{
    while (kb_call_readchar_callback() == 0) {
        port_guest_check_stop();
        SDL_DelayNS(1000000u);
    }
}

void kb_reg_callback(uint16_t key, void (*callback)(void))
{
    unsigned i;
    if (callback == NULL)
        return;
    input_lock();
    for (i = 0; i < s_callback_count; ++i) {
        if (s_callback_scan[i] == key && s_callbacks[i] == callback) {
            input_unlock();
            return;
        }
    }
    if (s_callback_count < PORT_KEY_CALLBACK_CAP) {
        s_callback_scan[s_callback_count] = key;
        s_callbacks[s_callback_count] = callback;
        ++s_callback_count;
    }
    input_unlock();
}

static int dispatch_key_callback(uint16_t bios_key)
{
    uint8_t ascii = (uint8_t)bios_key;
    uint8_t scan = (uint8_t)(bios_key >> 8);
    uint16_t lookup = ascii != 0 ? (uint16_t)(ascii & 0x7Fu) : scan;
    void (*callback)(void) = NULL;
    unsigned i;
    input_lock();
    for (i = 0; i < s_callback_count; ++i) {
        uint16_t registered = s_callback_scan[i];
        uint16_t registered_lookup = (registered & 0xFFu) != 0
                                        ? (registered & 0x7Fu)
                                        : ((registered >> 8) & 0xFFu);
        if (registered_lookup == lookup) {
            callback = s_callbacks[i];
            break;
        }
    }
    input_unlock();
    if (callback != NULL) {
        callback();
        return 1;
    }
    return 0;
}

int16_t kb_get_char(void)
{
    int found;
    port_guest_check_stop();
    uint16_t key = pop_bios_key(&found);
    uint8_t ascii;
    if (!found)
        return 0;
    if (dispatch_key_callback(key))
        return 0;
    ascii = (uint8_t)key;
    return ascii != 0 ? (int16_t)ascii : (int16_t)(key & 0xFF00u);
}

int16_t kb_read_char(void)
{
    int found;
    port_guest_check_stop();
    uint16_t key = pop_bios_key(&found);
    uint8_t ascii;
    if (!found)
        return 0;
    ascii = (uint8_t)key;
    return ascii != 0 ? (int16_t)ascii : (int16_t)(key & 0xFF00u);
}

int16_t kb_check(void)
{
    port_guest_check_stop();
    input_lock();
    s_key_head = s_key_tail;
    s_key_count = 0;
    input_unlock();
    return 0;
}

int16_t kb_get_key_state(int16_t scan_code)
{
    port_guest_check_stop();
    return (int16_t)port_input_key_state((uint16_t)scan_code);
}

int16_t get_joy_flags(void)
{
    /* No joystick device is enabled in the M1 keyboard-only input adapter. */
    return 0;
}

int16_t joystick_flags_to_index(int16_t flags)
{
    /* Exact 16-entry conversion table from
       asm/input_keyboard_joystick_services.ASM. */
    static const uint8_t axis_index[16] = {
        0, 1, 5, 0, 3, 2, 4, 3, 7, 8, 6, 7, 0, 1, 5, 0
    };
    return (int16_t)axis_index[(uint16_t)flags & 0x0fu];
}

void reset_joystick_selection(void)
{
    /* The legacy routine enabled its gameport unconditionally. SDL3 has no
       joystick backend yet, so reset to the only valid host selection. */
    joystick_enabled = 0;
}

int8_t replay_axis_value(void)
{
    int left;
    int right;

    /* Keyboard movement uses the same keypad scans as get_kb_or_joy_flags:
       7/4/1 steer left and 9/6/3 steer right. A keyboard has no analog
       travel, so map its held direction to the original axis endpoints
       (-31 at minimum, +33 at maximum); opposing keys cancel. */
    left = port_input_key_state(0x47u) || port_input_key_state(0x4bu) ||
           port_input_key_state(0x4fu);
    right = port_input_key_state(0x49u) || port_input_key_state(0x4du) ||
            port_input_key_state(0x51u);
    if (left == right)
        return 0;
    return left ? (int8_t)-31 : (int8_t)33;
}

static void set_num_lock(int enabled)
{
    input_lock();
    s_num_lock = enabled != 0;
    input_unlock();
}

static int16_t peek_keyboard_char(void)
{
    int found;
    uint16_t key;
    input_lock();
    found = s_key_count != 0;
    key = found ? s_key_queue[s_key_head] : 0;
    input_unlock();
    if (!found)
        return 0;
    return (uint8_t)key != 0 ? (int16_t)(uint8_t)key
                             : (int16_t)(key & 0xFF00u);
}

void keyboard_shift_checking1(void)
{
    set_num_lock(1);
    (void)peek_keyboard_char();
}

void kb_shift_checking1(void)
{
    keyboard_shift_checking1();
}

int16_t get_kb_or_joy_flags(void)
{
    static const uint8_t scans[10] = {
        0x39, 0x1C, 0x47, 0x48, 0x49,
        0x4D, 0x51, 0x50, 0x4F, 0x4B
    };
    static const uint8_t flags[10] = {
        0x10, 0x20, 0x09, 0x01, 0x05,
        0x04, 0x06, 0x02, 0x0A, 0x08
    };
    uint8_t result = 0;
    unsigned i;
    for (i = 0; i < 10u; ++i)
        if (port_input_key_state(scans[i]))
            result |= flags[i];
    return result != 0 ? result : get_joy_flags();
}

void kb_shift_checking2(void)
{
    set_num_lock(0);
    (void)peek_keyboard_char();
}

void keyboard_exit_handler(void)
{
    /* The DOS handler restores its vectors and clears only the held modifier
       bits; BIOS lock-toggle state survives the restore. */
    kb_init_interrupt();
}
