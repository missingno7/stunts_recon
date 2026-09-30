#include "port_runtime.h"
#include "vga_timing.h"

#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>

uint8_t port_framebuffer[PORT_VIDEO_MEMORY_BYTES];

static SDL_Renderer *s_renderer;
static SDL_Texture *s_texture;
static SDL_Palette *s_palette;
static SDL_Mutex *s_frame_lock;
static uint64_t s_frame_id;
static SDL_Color s_colors[256];
static uint8_t s_palette6[256u * 3u];
static uint8_t s_published_frame[PORT_FRAMEBUFFER_BYTES];
static uint8_t s_published_palette6[256u * 3u];
static char s_capture_dir[512];

static void write_u16le(FILE *stream, uint16_t value)
{
    fputc((int)(value & 0xFFu), stream);
    fputc((int)(value >> 8), stream);
}

static void write_u32le(FILE *stream, uint32_t value)
{
    write_u16le(stream, (uint16_t)value);
    write_u16le(stream, (uint16_t)(value >> 16));
}

static void dump_frame(uint64_t frame_id, const char *reason,
                       const uint8_t *pixels, const uint8_t *palette)
{
    char path[768];
    FILE *stream;
    int path_length;
    if (s_capture_dir[0] == '\0')
        return;
    path_length = snprintf(path, sizeof(path), "%s/frame-%06llu.fbr", s_capture_dir,
                           (unsigned long long)frame_id);
    if (path_length < 0 || (size_t)path_length >= sizeof(path))
        return;
    stream = fopen(path, "wb");
    if (stream == NULL)
        return;
    fwrite("STFBR1\0\0", 1, 8, stream);
    write_u16le(stream, PORT_SCREEN_WIDTH);
    write_u16le(stream, PORT_SCREEN_HEIGHT);
    write_u16le(stream, sizeof(s_palette6));
    write_u16le(stream, 0);
    write_u32le(stream, (uint32_t)frame_id);
    fwrite(pixels, 1, PORT_FRAMEBUFFER_BYTES, stream);
    fwrite(palette, 1, 256u * 3u, stream);
    fclose(stream);
    path_length = snprintf(path, sizeof(path), "%s/frame-%06llu.json", s_capture_dir,
                           (unsigned long long)frame_id);
    if (path_length < 0 || (size_t)path_length >= sizeof(path))
        return;
    stream = fopen(path, "wb");
    if (stream == NULL)
        return;
    fprintf(stream, "{\"frame_id\":%llu,\"reason\":\"%s\","
                    "\"width\":%d,\"height\":%d,\"palette\":\"RGB6\"}\n",
            (unsigned long long)frame_id,
            reason != NULL ? reason : "unknown",
            PORT_SCREEN_WIDTH, PORT_SCREEN_HEIGHT);
    fclose(stream);
}

static uint8_t dac6_to_u8(uint8_t value)
{
    value &= 0x3Fu;
    return (uint8_t)((value << 2) | (value >> 4));
}

int port_video_init(SDL_Renderer *renderer)
{
    size_t i;
    s_renderer = renderer;
    s_frame_lock = SDL_CreateMutex();
    if (s_frame_lock == NULL)
        return 0;
    memset(port_framebuffer, 0, sizeof(port_framebuffer));
    memset(s_published_frame, 0, sizeof(s_published_frame));
    memset(s_palette6, 0, sizeof(s_palette6));
    memset(s_published_palette6, 0, sizeof(s_published_palette6));
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
    if (s_frame_lock != NULL)
        SDL_LockMutex(s_frame_lock);
    for (i = 0; i < count; ++i) {
        s_palette6[(first + i) * 3u + 0u] = (uint8_t)(rgb6[i * 3u + 0u] & 0x3Fu);
        s_palette6[(first + i) * 3u + 1u] = (uint8_t)(rgb6[i * 3u + 1u] & 0x3Fu);
        s_palette6[(first + i) * 3u + 2u] = (uint8_t)(rgb6[i * 3u + 2u] & 0x3Fu);
        s_colors[first + i].r = dac6_to_u8(rgb6[i * 3u + 0u]);
        s_colors[first + i].g = dac6_to_u8(rgb6[i * 3u + 1u]);
        s_colors[first + i].b = dac6_to_u8(rgb6[i * 3u + 2u]);
        s_colors[first + i].a = SDL_ALPHA_OPAQUE;
    }
    if (s_frame_lock != NULL)
        SDL_UnlockMutex(s_frame_lock);
}

void port_video_set_capture_dir(const char *path)
{
    SDL_PathInfo info;
    s_capture_dir[0] = '\0';
    if (path == NULL || path[0] == '\0')
        return;
    if (strlen(path) >= sizeof(s_capture_dir))
        return;
    strcpy(s_capture_dir, path);
    if ((!SDL_GetPathInfo(s_capture_dir, &info) ||
         info.type != SDL_PATHTYPE_DIRECTORY) &&
        !SDL_CreateDirectory(s_capture_dir)) {
        fprintf(stderr, "PORT frame capture disabled: %s\n", SDL_GetError());
        s_capture_dir[0] = '\0';
    }
}

void port_video_publish(const char *reason)
{
    uint8_t frame[PORT_FRAMEBUFFER_BYTES];
    uint8_t palette[sizeof(s_palette6)];
    uint64_t frame_id;
    if (s_frame_lock != NULL)
        SDL_LockMutex(s_frame_lock);
    memcpy(frame, port_framebuffer, sizeof(frame));
    memcpy(s_published_frame, frame, sizeof(s_published_frame));
    memcpy(palette, s_palette6, sizeof(palette));
    memcpy(s_published_palette6, palette, sizeof(s_published_palette6));
    frame_id = ++s_frame_id;
    if (s_frame_lock != NULL)
        SDL_UnlockMutex(s_frame_lock);
    dump_frame(frame_id, reason, frame, palette);
    port_trace_video_publication(reason);
    port_guest_stop_after_publication();
}

void port_video_present(void)
{
    int output_w = 0;
    int output_h = 0;
    int scale;
    SDL_FRect destination;
    uint8_t frame[PORT_FRAMEBUFFER_BYTES];
    uint8_t palette6[sizeof(s_palette6)];
    SDL_Color colors[256];
    uint64_t frame_id;
    int published_live_changes = 0;
    if (s_renderer == NULL || s_texture == NULL)
        return;
    if (s_frame_lock != NULL)
        SDL_LockMutex(s_frame_lock);
    if (memcmp(port_framebuffer, s_published_frame, sizeof(s_published_frame)) != 0 ||
        memcmp(s_palette6, s_published_palette6, sizeof(s_published_palette6)) != 0) {
        memcpy(s_published_frame, port_framebuffer, sizeof(s_published_frame));
        memcpy(s_published_palette6, s_palette6, sizeof(s_published_palette6));
        frame_id = ++s_frame_id;
        published_live_changes = 1;
    }
    memcpy(frame, s_published_frame, sizeof(frame));
    memcpy(colors, s_colors, sizeof(colors));
    memcpy(palette6, s_published_palette6, sizeof(palette6));
    frame_id = s_frame_id;
    if (s_frame_lock != NULL)
        SDL_UnlockMutex(s_frame_lock);
    if (frame_id == 0)
        return;
    if (published_live_changes) {
        dump_frame(frame_id, "host_present", frame, palette6);
        port_trace_video_publication("host_present");
    }
    SDL_SetPaletteColors(s_palette, colors, 0, 256);
    SDL_UpdateTexture(s_texture, NULL, frame, PORT_SCREEN_WIDTH);
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
        port_trace_host_present(frame_id);
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
    if (s_frame_lock != NULL) {
        SDL_DestroyMutex(s_frame_lock);
        s_frame_lock = NULL;
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

uint8_t port_video_read_status_1(void)
{
    uint8_t test_status;
    if (port_test_random_wait_status(&test_status))
        return test_status;
    return port_vga_input_status_1(port_timer_machine_time_ns());
}

int16_t video_get_status(void)
{
    /* This recovered routine returns only Input Status #1 bit 3. Preserve its
       historical interface while the port-level 3DAh read exposes both bits. */
    return (int16_t)(port_video_read_status_1() & 0x08u);
}
