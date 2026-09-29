#ifndef STUNTS_PORT_RUNTIME_H
#define STUNTS_PORT_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#define PORT_SCREEN_WIDTH 320
#define PORT_SCREEN_HEIGHT 200
#define PORT_FRAMEBUFFER_BYTES (PORT_SCREEN_WIDTH * PORT_SCREEN_HEIGHT)
#define PORT_VIDEO_MEMORY_BYTES 0x10000u
#define PORT_DOS_ADDRESS_BYTES 0x100000u
#define PORT_TIMER_PERIOD_NS 10000154ull

typedef enum PortFarSpace {
    PORT_FAR_REAL = 0,
    PORT_FAR_HANDLE = 1
} PortFarSpace;

/* A logical far address. Never cast this value directly to a host pointer. */
typedef struct PortFarPtr {
    uint16_t segment;
    uint16_t offset;
    uint8_t space;
} PortFarPtr;

typedef struct PortMemoryStats {
    uint32_t allocations;
    uint32_t live_bytes;
    uint32_t high_water_bytes;
} PortMemoryStats;

extern uint8_t port_framebuffer[PORT_VIDEO_MEMORY_BYTES];

struct SDL_Renderer;
typedef struct SDL_Renderer SDL_Renderer;

void port_trace_open(const char *path, const char *asset_root);
void port_trace_close(void);
void port_trace_timer_tick(uint64_t tick_id, uint64_t scheduled_ns,
                           uint64_t observed_ns);
void port_trace_sim_step(void);
void port_trace_video_publication(const char *reason);
void port_trace_host_present(uint64_t frame_id);
void port_trace_audio_publication(uint32_t frame_count);
void port_trace_host_stop(const char *reason);

void port_runtime_set_asset_root(const char *root);
const char *port_runtime_asset_root(void);
void port_stub_fail(const char *symbol);
void port_guest_unwind(const char *symbol);

void port_memory_init(void);
void *port_memory_alloc(size_t size, const char *owner, PortFarPtr *address_out);
void port_memory_free(void *pointer);
PortFarPtr port_far_normalize(PortFarPtr pointer);
PortFarPtr port_far_add(PortFarPtr pointer, uint32_t amount);
void *port_far_resolve(PortFarPtr pointer, size_t extent);
int port_far_from_host(const void *pointer, PortFarPtr *address_out,
                       size_t *remaining_out);
PortMemoryStats port_memory_stats(void);
int port_memory_extent(const void *pointer, size_t *remaining_out);
int port_memory_resize(void *pointer, size_t size);

int port_fs_open_read(const char *path);
int32_t port_fs_read(int handle, void *buffer, uint32_t bytes);
int32_t port_fs_seek(int handle, int32_t offset, int origin);
void port_fs_close(int handle);
void *port_fs_load(const char *path, size_t *length_out,
                   PortFarPtr *address_out);
int port_fs_exists(const char *path);

int port_resource_decompress(const uint8_t *source, size_t source_size,
                             uint8_t **output, size_t *output_size);

uint8_t *port_video_pixels(void);
int port_video_init(SDL_Renderer *renderer);
void port_video_set_palette(uint16_t first, uint16_t count,
                            const uint8_t *rgb6);
void port_video_publish(const char *reason);
void port_video_present(void);
void port_video_shutdown(void);

void port_timer_start(void);
void port_timer_stop(void);
uint64_t port_timer_tick_count(void);
int port_timer_game_enabled(void);
void port_timer_mark_game_enabled(int enabled);

void port_input_init(void);
void port_input_handle_event(int event_type, int scancode, int pressed);
uint8_t port_input_key_state(uint16_t dos_scan);
int port_input_dos_scan(int sdl_scancode);
void port_input_mouse_get(int16_t *x, int16_t *y, uint16_t *buttons);
void port_input_mouse_set(int16_t x, int16_t y);
void port_input_mouse_set_x_bounds(int16_t min_x, int16_t max_x);
void port_input_mouse_set_y_bounds(int16_t min_y, int16_t max_y);

int port_audio_silent_load(const char *driver_name);
void port_audio_shutdown(void);

void port_sdl_init(const char *title);
int port_sdl_poll(void);
void port_sdl_shutdown(void);
void port_sdl_sleep_until(uint64_t deadline_ns);

#endif
