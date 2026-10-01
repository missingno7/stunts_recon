#ifndef PORT_RENDER_MODERN_H
#define PORT_RENDER_MODERN_H
#include "renderer.h"
#include "viewport.h"
#include "renderer_ui.h"
#include "renderer_camera_scene.h"
#include <SDL3/SDL.h>

/* Main-thread-only GPU resources; semantic commands remain API-independent. */
typedef struct PortModernBackend {
    SDL_Renderer *renderer;
    SDL_Texture *background, *ui, *pattern, *edges;
    SDL_Vertex *mesh;
    uint8_t *rgba;
    size_t vertices, capacity;
    uint16_t cached_pattern;
    uint8_t cached_mode, cached_color, cached_secondary;
    uint8_t cached_palette[768];
    int pattern_valid;
} PortModernBackend;

int port_modern_init(PortModernBackend *backend, SDL_Renderer *renderer);
void port_modern_destroy(PortModernBackend *backend);
int port_modern_draw_projected(PortModernBackend *backend,
                              const PortRenderCommand *commands, size_t count,
                              const uint8_t *palette6, int width, int height);
int port_modern_draw_camera(PortModernBackend *backend, const PortNativeCommand *commands,
                           size_t count, const uint8_t *palette6, int width, int height);
int port_modern_compose(PortModernBackend *backend,
                        const PortRenderCommand *commands, size_t count,
                        const PortNativeCommand *native, size_t native_count,
                        const PortRenderLayer *layer, const uint8_t *frame,
                        const uint8_t *palette6, int width, int height);
#endif
