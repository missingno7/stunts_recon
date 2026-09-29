#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <string.h>

uint8_t port_framebuffer[PORT_VIDEO_MEMORY_BYTES];

static SDL_Renderer *s_renderer;
static SDL_Texture *s_texture;
static SDL_Palette *s_palette;
static uint64_t s_frame_id;
static SDL_Color s_colors[256];

static uint8_t dac6_to_u8(uint8_t value)
{
    value &= 0x3Fu;
    return (uint8_t)((value << 2) | (value >> 4));
}

int port_video_init(SDL_Renderer *renderer)
{
    size_t i;
    s_renderer = renderer;
    memset(port_framebuffer, 0, sizeof(port_framebuffer));
    port_sprite_init();
    for (i = 0; i < 256; ++i) {
        s_colors[i].r = 0;
        s_colors[i].g = 0;
        s_colors[i].b = 0;
        s_colors[i].a = SDL_ALPHA_OPAQUE;
    }
    s_palette = SDL_CreatePalette(256);
    if (s_palette == NULL)
        return 0;
    if (!SDL_SetPaletteColors(s_palette, s_colors, 0, 256))
        return 0;
    s_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_INDEX8,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  PORT_SCREEN_WIDTH, PORT_SCREEN_HEIGHT);
    if (s_texture == NULL)
        return 0;
    if (!SDL_SetTexturePalette(s_texture, s_palette))
        return 0;
    if (!SDL_SetTextureScaleMode(s_texture, SDL_SCALEMODE_NEAREST))
        return 0;
    return 1;
}

uint8_t *port_video_pixels(void)
{
    return port_framebuffer;
}

void port_video_set_palette(uint16_t first, uint16_t count,
                            const uint8_t *rgb6)
{
    uint16_t i;
    if (first >= 256 || count == 0 || rgb6 == NULL)
        return;
    if ((uint32_t)first + count > 256u)
        count = (uint16_t)(256u - first);
    for (i = 0; i < count; ++i) {
        s_colors[first + i].r = dac6_to_u8(rgb6[i * 3u + 0u]);
        s_colors[first + i].g = dac6_to_u8(rgb6[i * 3u + 1u]);
        s_colors[first + i].b = dac6_to_u8(rgb6[i * 3u + 2u]);
        s_colors[first + i].a = SDL_ALPHA_OPAQUE;
    }
    if (s_palette != NULL)
        SDL_SetPaletteColors(s_palette, s_colors, first, count);
}

void port_video_publish(const char *reason)
{
    ++s_frame_id;
    if (s_texture != NULL)
        SDL_UpdateTexture(s_texture, NULL, port_framebuffer, PORT_SCREEN_WIDTH);
    port_trace_video_publication(reason);
}

void port_video_present(void)
{
    int output_w = 0;
    int output_h = 0;
    int scale;
    SDL_FRect destination;
    if (s_renderer == NULL || s_texture == NULL)
        return;
    if (!SDL_GetRenderOutputSize(s_renderer, &output_w, &output_h))
        return;
    scale = output_w / PORT_SCREEN_WIDTH;
    if (output_h / PORT_SCREEN_HEIGHT < scale)
        scale = output_h / PORT_SCREEN_HEIGHT;
    if (scale < 1)
        scale = 1;
    destination.w = (float)(PORT_SCREEN_WIDTH * scale);
    destination.h = (float)(PORT_SCREEN_HEIGHT * scale);
    destination.x = (float)(output_w - (int)destination.w) / 2.0f;
    destination.y = (float)(output_h - (int)destination.h) / 2.0f;
    SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(s_renderer);
    SDL_RenderTexture(s_renderer, s_texture, NULL, &destination);
    if (SDL_RenderPresent(s_renderer))
        port_trace_host_present(s_frame_id);
}

void port_video_shutdown(void)
{
    if (s_texture != NULL) {
        SDL_DestroyTexture(s_texture);
        s_texture = NULL;
    }
    if (s_palette != NULL) {
        SDL_DestroyPalette(s_palette);
        s_palette = NULL;
    }
    s_renderer = NULL;
}

/* Implemented platform seams reached by initialize_main in M0. */
void video_set_mode_13h(void) { }
void video_set_mode4(void) { }

void video_set_palette(uint16_t first, uint16_t count, uint8_t *rgb6)
{
    port_video_set_palette(first, count, rgb6);
}

int16_t video_get_status(void)
{
    static int16_t phase;
    phase ^= 8;
    return phase;
}
