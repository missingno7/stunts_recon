#include "port_runtime.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>

static int s_input_events;
void port_sprite_init(void) { }
void port_input_init(void) { }
void port_input_shutdown(void) { }
void port_input_handle_event(int type, int code, int value)
{ (void)type; (void)code; (void)value; ++s_input_events; }
void port_guest_check_stop(void) { }
void port_guest_stop_after_publication(void) { }
int port_test_random_wait_status(uint8_t *status) { (void)status; return 0; }
uint64_t port_timer_machine_time_ns(void) { return SDL_GetTicksNS(); }
uint16_t port_game_frame_snapshot(void) { return 37; }
uint16_t port_game_y_rotation_snapshot(void) { return 255; }
uint8_t port_game_mode_snapshot(void) { return 0; }
uint8_t port_game_inputmode_snapshot(void) { return 1; }
uint8_t port_game_replaymode_snapshot(void) { return 0; }
uint16_t port_game_rate_snapshot(void) { return 20; }
const char *port_audio_backend_name(void) { return "none"; }

static int key(Uint32 type, bool repeat)
{
    SDL_Event event;
    SDL_zero(event);
    event.type = type;
    event.key.scancode = SDL_SCANCODE_F12;
    event.key.repeat = repeat;
    if (!SDL_PushEvent(&event)) return 0;
    port_sdl_poll();
    return 1;
}

static int redraw(Uint32 type)
{
    SDL_Event event;
    SDL_zero(event);
    event.type = type;
    if (!SDL_PushEvent(&event)) return 0;
    port_sdl_poll();
    port_video_present();
    return 1;
}

int main(int argc, char **argv)
{
    uint8_t palette[768];
    char trace[1400];
    if (argc != 2) return 2;
    port_diagnostics_init(0, argv[1]);
    snprintf(trace, sizeof(trace), "%s/trace.jsonl", port_diagnostics_directory());
    port_sdl_init("Stunts debug capture probe");
    port_trace_open(trace, "probe");
    for (unsigned i = 0; i < sizeof(palette); ++i) palette[i] = (uint8_t)(i & 63);
    port_video_set_palette(0, 256, palette);
    for (unsigned i = 0; i < PORT_FRAMEBUFFER_BYTES; ++i)
        port_video_pixels()[i] = (uint8_t)(i * 7 + i / 320);
    port_video_publish("probe_image");
    port_video_set_debug_capture_dir(port_diagnostics_directory());
    if (!key(SDL_EVENT_KEY_DOWN, false) || !key(SDL_EVENT_KEY_DOWN, true) ||
        !key(SDL_EVENT_KEY_UP, false)) return 3;
    if (s_input_events != 0) return 4;
    port_video_set_debug_capture_dir("");
    if (!key(SDL_EVENT_KEY_DOWN, false) || !key(SDL_EVENT_KEY_UP, false)) return 5;
    if (s_input_events != 2) return 6;
    port_video_present();
    for (unsigned i = 0; i < 5; ++i) port_video_present();
    port_video_publish("identical_image");
    port_video_present();
    palette[0] ^= 1;
    port_video_set_palette(0, 256, palette);
    port_video_present();
    port_video_pixels()[0] ^= 1;
    port_video_publish("changed_pixel");
    port_video_present();
    if (!redraw(SDL_EVENT_WINDOW_EXPOSED) ||
        !redraw(SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) ||
        !redraw(SDL_EVENT_WINDOW_RESTORED) ||
        !redraw(SDL_EVENT_RENDER_DEVICE_RESET)) return 7;
    port_trace_close();
    port_sdl_shutdown();
    port_diagnostics_close(0, "capture probe complete");
    return 0;
}
