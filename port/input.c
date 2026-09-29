#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <string.h>

static uint8_t s_key_down[128];
static uint8_t s_extended_down[128];
static uint16_t s_callback_scan[32];
static void (*s_callbacks[32])(void);
static unsigned s_callback_count;
static int s_mouse_x;
static int s_mouse_y;
static uint8_t s_mouse_buttons;
static int s_mouse_min_x;
static int s_mouse_min_y;
static int s_mouse_max_x = PORT_SCREEN_WIDTH - 1;
static int s_mouse_max_y = PORT_SCREEN_HEIGHT - 1;

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
    memset(s_key_down, 0, sizeof(s_key_down));
    memset(s_extended_down, 0, sizeof(s_extended_down));
    s_callback_count = 0;
    s_mouse_x = 0;
    s_mouse_y = 0;
    s_mouse_buttons = 0;
    s_mouse_min_x = 0;
    s_mouse_min_y = 0;
    s_mouse_max_x = PORT_SCREEN_WIDTH - 1;
    s_mouse_max_y = PORT_SCREEN_HEIGHT - 1;
}

void port_input_handle_event(int event_type, int code, int value)
{
    if (event_type == SDL_EVENT_KEY_DOWN || event_type == SDL_EVENT_KEY_UP) {
        int dos_code = port_input_dos_scan(code);
        int down = event_type == SDL_EVENT_KEY_DOWN;
        if (dos_code >= 0xE000) {
            s_extended_down[dos_code & 0x7Fu] = (uint8_t)down;
        } else if (dos_code > 0 && dos_code < 128) {
            s_key_down[dos_code] = (uint8_t)down;
        }
    } else if (event_type == SDL_EVENT_MOUSE_MOTION) {
        s_mouse_x = code < s_mouse_min_x ? s_mouse_min_x : code;
        s_mouse_y = value < s_mouse_min_y ? s_mouse_min_y : value;
        if (s_mouse_x > s_mouse_max_x) s_mouse_x = s_mouse_max_x;
        if (s_mouse_y > s_mouse_max_y) s_mouse_y = s_mouse_max_y;
    } else if (event_type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
               event_type == SDL_EVENT_MOUSE_BUTTON_UP) {
        uint8_t bit = code == 1 ? 1u : code == 2 ? 4u : code == 3 ? 2u : 0u;
        if (event_type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            s_mouse_buttons |= bit;
        else
            s_mouse_buttons &= (uint8_t)~bit;
    }
}

uint8_t port_input_key_state(uint16_t dos_scan)
{
    if (dos_scan < 128)
        return s_key_down[dos_scan];
    if ((dos_scan & 0xFF00u) == 0xE000u)
        return s_extended_down[dos_scan & 0x7Fu];
    return 0;
}

void port_input_mouse_get(int16_t *x, int16_t *y, uint16_t *buttons)
{
    if (x != NULL) *x = (int16_t)s_mouse_x;
    if (y != NULL) *y = (int16_t)s_mouse_y;
    if (buttons != NULL) *buttons = (uint16_t)s_mouse_buttons;
}

void port_input_mouse_set(int16_t x, int16_t y)
{
    s_mouse_x = x < s_mouse_min_x ? s_mouse_min_x : x;
    s_mouse_y = y < s_mouse_min_y ? s_mouse_min_y : y;
    if (s_mouse_x > s_mouse_max_x) s_mouse_x = s_mouse_max_x;
    if (s_mouse_y > s_mouse_max_y) s_mouse_y = s_mouse_max_y;
}

void port_input_mouse_set_x_bounds(int16_t min_x, int16_t max_x)
{
    s_mouse_min_x = min_x;
    s_mouse_max_x = max_x;
    port_input_mouse_set((int16_t)s_mouse_x, (int16_t)s_mouse_y);
}

void port_input_mouse_set_y_bounds(int16_t min_y, int16_t max_y)
{
    s_mouse_min_y = min_y;
    s_mouse_max_y = max_y;
    port_input_mouse_set((int16_t)s_mouse_x, (int16_t)s_mouse_y);
}

void kb_init_interrupt(void) { }
void kb_shift_checking2(void) { }
void kb_call_readchar_callback(void) { }

void kb_reg_callback(uint16_t scan, void (*callback)(void))
{
    if (s_callback_count >= sizeof(s_callback_scan) / sizeof(s_callback_scan[0]))
        return;
    s_callback_scan[s_callback_count] = scan;
    s_callbacks[s_callback_count] = callback;
    ++s_callback_count;
}
