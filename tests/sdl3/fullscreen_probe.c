#include "port_runtime.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static int keys, redraws;
int port_video_init(SDL_Renderer *renderer) { return renderer != NULL; }
void port_video_shutdown(void) { }
void port_video_request_redraw(int reupload) { (void)reupload; ++redraws; }
int port_video_debug_capture_enabled(void) { return 0; }
void port_video_debug_capture(void) { }
void port_video_mouse_to_logical(int ww, int wh, int ow, int oh,
    float x, float y, int *lx, int *ly)
{ (void)ww; (void)wh; (void)ow; (void)oh; *lx = (int)x; *ly = (int)y; }
void port_input_init(void) { }
void port_input_shutdown(void) { }
void port_input_handle_event(int type, int code, int value)
{ (void)code; (void)value;
  if (type == SDL_EVENT_KEY_DOWN || type == SDL_EVENT_KEY_UP) ++keys; }

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s (%s)\n", __LINE__, #condition, SDL_GetError()); \
    return 1; } } while (0)

static void key(Uint32 type, SDL_Scancode code, SDL_Keymod mod, bool repeat)
{
    SDL_Event event;
    SDL_zero(event);
    event.type = type;
    event.key.scancode = code;
    event.key.mod = mod;
    event.key.repeat = repeat;
    SDL_PushEvent(&event);
    port_sdl_poll();
}

int main(void)
{
    int count, x, y, w, h, original_x, original_y;
    SDL_Window **windows;
    SDL_Window *window;
    SDL_Rect desktop;
    port_sdl_hidden_window();
    port_sdl_initial_size(640, 480);
    port_sdl_init("Stunts borderless fullscreen probe");
    windows = SDL_GetWindows(&count);
    CHECK(windows && count == 1);
    window = windows[0];
    SDL_free(windows);
    CHECK(SDL_SetWindowPosition(window, 73, 91));
    CHECK(SDL_ShowWindow(window));
    CHECK(SDL_SyncWindow(window));
    CHECK(SDL_GetWindowPosition(window, &original_x, &original_y));
    CHECK(SDL_GetDisplayBounds(SDL_GetDisplayForWindow(window), &desktop));
    port_sdl_poll();
    keys = redraws = 0;
    key(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_RETURN, SDL_KMOD_LALT, false);
    CHECK(SDL_SyncWindow(window));
    CHECK(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
    CHECK(SDL_GetWindowFullscreenMode(window) == NULL);
#ifdef _WIN32
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "windows") == 0) {
        HWND hwnd = SDL_GetPointerProperty(SDL_GetWindowProperties(window),
                                          SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        CHECK(hwnd != NULL);
        CHECK(!(GetWindowLong(hwnd, GWL_STYLE) & (WS_CAPTION | WS_THICKFRAME)));
    }
#endif
    CHECK(SDL_GetWindowSize(window, &w, &h));
    CHECK(w == desktop.w && h == desktop.h);
    CHECK(SDL_GetWindowPosition(window, &x, &y));
    CHECK(x == desktop.x && y == desktop.y);
    CHECK(redraws > 0 && keys == 0);
    key(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_RETURN, SDL_KMOD_LALT, true);
    key(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_RETURN, SDL_KMOD_LALT, false);
    key(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_RETURN, SDL_KMOD_NONE, true);
    key(SDL_EVENT_KEY_UP, SDL_SCANCODE_RETURN, SDL_KMOD_NONE, false);
    CHECK(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);
    CHECK(keys == 0);
    key(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_KP_ENTER, SDL_KMOD_RALT, false);
    key(SDL_EVENT_KEY_UP, SDL_SCANCODE_KP_ENTER, SDL_KMOD_NONE, false);
    CHECK(SDL_SyncWindow(window));
    CHECK(!(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN));
    CHECK(SDL_GetWindowSize(window, &w, &h));
    CHECK(w == 640 && h == 480);
    CHECK(SDL_GetWindowPosition(window, &x, &y));
    CHECK(x == original_x && y == original_y);
    CHECK(keys == 0);
    key(SDL_EVENT_KEY_DOWN, SDL_SCANCODE_RETURN, SDL_KMOD_NONE, false);
    key(SDL_EVENT_KEY_UP, SDL_SCANCODE_RETURN, SDL_KMOD_NONE, false);
    CHECK(keys == 2);
    port_sdl_shutdown();
    return 0;
}
