#include "port_runtime.h"
#include "render/viewport.h"
#include "render/renderer.h"

#include <SDL3/SDL.h>

static SDL_Window *s_window;
static SDL_Renderer *s_renderer;
static int s_initial_width=960, s_initial_height=720;
static int s_hidden;
void port_sdl_hidden_window(void) { s_hidden=1; }
void port_sdl_initial_size(int width,int height)
{ s_initial_width=width;s_initial_height=height; }
static int s_quit;

static void mouse_to_logical(float x, float y, int *logical_x, int *logical_y)
{
    int window_w = 0;
    int window_h = 0;
    int output_w = 0;
    int output_h = 0;
    SDL_GetWindowSize(s_window, &window_w, &window_h);
    SDL_GetRenderOutputSize(s_renderer, &output_w, &output_h);
    port_viewport_mouse(window_w, window_h, output_w, output_h,
                        x, y, logical_x, logical_y);
}

void port_sdl_init(const char *title)
{
    s_quit = 0;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
    s_window = SDL_CreateWindow(title, s_initial_width, s_initial_height,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY |
                              (s_hidden ? SDL_WINDOW_HIDDEN : 0));
    if (s_window == NULL) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        return;
    }
    s_renderer = SDL_CreateRenderer(s_window, "software");
#ifdef PORT_RENDER_INSTRUMENT
    if (port_render_mode()) {
        SDL_DestroyRenderer(s_renderer);
        s_renderer = SDL_CreateRenderer(s_window, "gpu");
        if (!s_renderer) s_renderer = SDL_CreateRenderer(s_window, NULL);
        if (s_renderer) SDL_Log("Modern renderer backend: %s",SDL_GetRendererName(s_renderer));
    }
#endif
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
        } else if (event.type == SDL_EVENT_WINDOW_EXPOSED ||
                   event.type == SDL_EVENT_WINDOW_RESIZED ||
                   event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED ||
                   event.type == SDL_EVENT_WINDOW_RESTORED ||
                   event.type == SDL_EVENT_WINDOW_SHOWN ||
                   event.type == SDL_EVENT_WINDOW_MAXIMIZED ||
                   event.type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED) {
            port_video_request_redraw(0);
        } else if (event.type == SDL_EVENT_RENDER_TARGETS_RESET ||
                   event.type == SDL_EVENT_RENDER_DEVICE_RESET) {
            port_video_request_redraw(1);
        } else if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
            port_input_handle_event(event.type, 0, 0);
        } else if ((event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) &&
                   event.key.scancode == SDL_SCANCODE_F12 &&
                   port_video_debug_capture_enabled()) {
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
                port_video_debug_capture();
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
