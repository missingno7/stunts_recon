#include "port_runtime.h"

#include <SDL3/SDL.h>

static SDL_Window *s_window;
static SDL_Renderer *s_renderer;
static int s_quit;

static void mouse_to_logical(float x, float y, int *logical_x, int *logical_y)
{
    int window_w = 0;
    int window_h = 0;
    int scale_x;
    int scale_y;
    int scale;
    int viewport_x;
    int viewport_y;
    SDL_GetWindowSize(s_window, &window_w, &window_h);
    scale_x = window_w / PORT_SCREEN_WIDTH;
    scale_y = window_h / PORT_SCREEN_HEIGHT;
    scale = scale_x < scale_y ? scale_x : scale_y;
    if (scale < 1) scale = 1;
    viewport_x = (window_w - PORT_SCREEN_WIDTH * scale) / 2;
    viewport_y = (window_h - PORT_SCREEN_HEIGHT * scale) / 2;
    *logical_x = ((int)x - viewport_x) / scale;
    *logical_y = ((int)y - viewport_y) / scale;
    if (*logical_x < 0) *logical_x = 0;
    if (*logical_y < 0) *logical_y = 0;
    if (*logical_x >= PORT_SCREEN_WIDTH) *logical_x = PORT_SCREEN_WIDTH - 1;
    if (*logical_y >= PORT_SCREEN_HEIGHT) *logical_y = PORT_SCREEN_HEIGHT - 1;
}

void port_sdl_init(const char *title)
{
    s_quit = 0;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
    s_window = SDL_CreateWindow(title, 960, 600, SDL_WINDOW_RESIZABLE);
    if (s_window == NULL) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return;
    }
    s_renderer = SDL_CreateRenderer(s_window, "software");
    if (s_renderer == NULL) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        return;
    }
    if (!port_video_init(s_renderer))
        SDL_Log("Indexed video initialization failed: %s", SDL_GetError());
    SDL_SetWindowMinimumSize(s_window, 320, 200);
    port_input_init();
}

int port_sdl_poll(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT ||
            event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            s_quit = 1;
        } else if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
            port_input_handle_event(event.type, 0, 0);
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            port_input_handle_event(event.type, event.key.scancode,
                                    event.key.repeat ? 1 : 0);
        } else if (event.type == SDL_EVENT_KEY_UP) {
            port_input_handle_event(event.type, event.key.scancode, 0);
        } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
            int logical_x;
            int logical_y;
            mouse_to_logical(event.motion.x, event.motion.y, &logical_x, &logical_y);
            port_input_handle_event(event.type, logical_x, logical_y);
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
                   event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            int logical_x;
            int logical_y;
            mouse_to_logical(event.button.x, event.button.y, &logical_x, &logical_y);
            port_input_handle_event(SDL_EVENT_MOUSE_MOTION, logical_x, logical_y);
            port_input_handle_event(event.type, event.button.button,
                                    event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
        }
    }
    return s_quit;
}

void port_sdl_shutdown(void)
{
    port_video_shutdown();
    port_input_shutdown();
    if (s_renderer != NULL) {
        SDL_DestroyRenderer(s_renderer);
        s_renderer = NULL;
    }
    if (s_window != NULL) {
        SDL_DestroyWindow(s_window);
        s_window = NULL;
    }
    SDL_Quit();
}

void port_sdl_sleep_until(uint64_t deadline_ns)
{
    uint64_t now = SDL_GetTicksNS();
    if (deadline_ns > now)
        SDL_DelayNS(deadline_ns - now);
}
