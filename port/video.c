#include "port_runtime.h"
#include "vga_timing.h"
#include "transition_work.h"
#include "render/viewport.h"
#ifdef PORT_RENDER_INSTRUMENT
#include "render/renderer_modern.h"
static PortNativeCommand s_published_native[PORT_NATIVE_MAX_COMMANDS];
static PortNativeCommand s_present_native[PORT_NATIVE_MAX_COMMANDS];
static size_t s_published_native_count,s_present_native_count;
static PortModernBackend s_modern;
static PortRenderLayer s_published_layer, s_present_layer;
static PortRenderCommand s_published_commands[400], s_present_commands[400];
static size_t s_published_command_count, s_present_command_count;
static uint8_t s_present_frame[64000], s_present_palette[768];
static int s_modern_failed;
#endif

#include <SDL3/SDL.h>
#include <stdio.h>
#include <string.h>

uint8_t port_framebuffer[PORT_VIDEO_MEMORY_BYTES];

static SDL_Renderer *s_renderer;
static SDL_Texture *s_texture;
static SDL_Palette *s_palette;
static SDL_Mutex *s_frame_lock;
static uint64_t s_frame_id;
static uint64_t s_image_version;
static uint64_t s_uploaded_version;
static uint64_t s_presented_version;
static int s_redraw_needed;
static int s_recreate_texture;
static SDL_Color s_colors[256];
static uint8_t s_palette6[256u * 3u];
static uint8_t s_published_frame[PORT_FRAMEBUFFER_BYTES];
static uint8_t s_published_palette6[256u * 3u];
static char s_capture_dir[512];
static char s_render_capture_dir[512];
static int s_render_captured;
static unsigned s_world_presentations;

void port_video_set_render_capture_dir(const char *path)
{
    s_render_capture_dir[0]=0;s_render_captured=0;s_world_presentations=0;
    if (path && *path && strlen(path)<sizeof s_render_capture_dir && SDL_CreateDirectory(path))
        strcpy(s_render_capture_dir,path);
}
static char s_debug_capture_dir[1100];
static unsigned s_debug_capture_sequence;

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

static int dump_frame_to_dir(const char *directory, uint64_t frame_id, const char *reason,
                       const uint8_t *pixels, const uint8_t *palette)
{
    char path[1400];
    FILE *stream;
    int path_length;
    if (directory[0] == '\0')
        return 0;
    path_length = snprintf(path, sizeof(path), "%s/frame-%06llu.fbr", directory,
                           (unsigned long long)frame_id);
    if (path_length < 0 || (size_t)path_length >= sizeof(path))
        return 0;
    stream = fopen(path, "wb");
    if (stream == NULL)
        return 0;
    fwrite("STFBR1\0\0", 1, 8, stream);
    write_u16le(stream, PORT_SCREEN_WIDTH);
    write_u16le(stream, PORT_SCREEN_HEIGHT);
    write_u16le(stream, sizeof(s_palette6));
    write_u16le(stream, 0);
    write_u32le(stream, (uint32_t)frame_id);
    fwrite(pixels, 1, PORT_FRAMEBUFFER_BYTES, stream);
    fwrite(palette, 1, 256u * 3u, stream);
    int ok = !ferror(stream);
    if (fclose(stream) != 0 || !ok) return 0;
    path_length = snprintf(path, sizeof(path), "%s/frame-%06llu.json", directory,
                           (unsigned long long)frame_id);
    if (path_length < 0 || (size_t)path_length >= sizeof(path))
        return 0;
    stream = fopen(path, "wb");
    if (stream == NULL)
        return 0;
    fprintf(stream, "{\"frame_id\":%llu,\"reason\":\"%s\","
                    "\"width\":%d,\"height\":%d,\"palette\":\"RGB6\"}\n",
            (unsigned long long)frame_id,
            reason != NULL ? reason : "unknown",
            PORT_SCREEN_WIDTH, PORT_SCREEN_HEIGHT);
    ok = !ferror(stream);
    return fclose(stream) == 0 && ok;
}

/* A simple uncompressed indexed BMP retains the exact screenshot colours. */
static int dump_bmp(const char *directory, const uint8_t *pixels, const uint8_t *palette)
{
    char path[1400];
    int length = snprintf(path, sizeof(path), "%s/screenshot.bmp", directory);
    if (length < 0 || (size_t)length >= sizeof(path)) return 0;
    FILE *stream = fopen(path, "wb");
    if (!stream) return 0;
    fwrite("BM", 1, 2, stream);
    write_u32le(stream, 14u + 40u + 1024u + PORT_FRAMEBUFFER_BYTES);
    write_u32le(stream, 0);
    write_u32le(stream, 14u + 40u + 1024u);
    write_u32le(stream, 40);
    write_u32le(stream, PORT_SCREEN_WIDTH);
    write_u32le(stream, PORT_SCREEN_HEIGHT);
    write_u16le(stream, 1);
    write_u16le(stream, 8);
    write_u32le(stream, 0);
    write_u32le(stream, PORT_FRAMEBUFFER_BYTES);
    write_u32le(stream, 0);
    write_u32le(stream, 0);
    write_u32le(stream, 256);
    write_u32le(stream, 256);
    for (unsigned i = 0; i < 256; ++i) {
        for (int channel = 2; channel >= 0; --channel) {
            unsigned value = palette[i * 3 + channel] & 63u;
            fputc((int)((value << 2) | (value >> 4)), stream);
        }
        fputc(0, stream);
    }
    for (int row = PORT_SCREEN_HEIGHT - 1; row >= 0; --row)
        fwrite(pixels + row * PORT_SCREEN_WIDTH, 1, PORT_SCREEN_WIDTH, stream);
    int ok = !ferror(stream);
    return fclose(stream) == 0 && ok;
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
    s_frame_id = 0;
    s_image_version = 0;
    s_uploaded_version = s_presented_version = UINT64_MAX;
    s_redraw_needed = 1;
    s_recreate_texture = 0;
    s_debug_capture_sequence = 0;
    s_debug_capture_dir[0] = '\0';
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
#ifdef PORT_RENDER_INSTRUMENT
    port_render_world_clear();
    s_published_command_count=s_present_command_count=0;
    s_published_native_count=s_present_native_count=0;
    memset(&s_published_layer,0,sizeof s_published_layer);
    s_modern_failed=0;
    if (port_render_mode() && !port_modern_init(&s_modern,renderer)) return 0;
#endif
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
    port_video_publish("palette");
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
    int changed;
    if (s_frame_lock != NULL)
        SDL_LockMutex(s_frame_lock);
    memcpy(frame, port_framebuffer, sizeof(frame));
    changed = memcmp(frame, s_published_frame, sizeof(frame)) != 0 ||
              memcmp(s_palette6, s_published_palette6, sizeof(s_palette6)) != 0;
#ifdef PORT_RENDER_INSTRUMENT
    if (port_render_mode()) {
        PortRenderLayer layer;
        PortRenderCommand commands[400];
        port_render_layer_copy(&layer);
        size_t count=port_render_stream_copy(commands,400);
        changed |= memcmp(&layer,&s_published_layer,sizeof layer)!=0 ||
            count!=s_published_command_count ||
            memcmp(commands,s_published_commands,count*sizeof(*commands))!=0;
        size_t native_count=0;
        const PortNativeCommand *native=port_camera_scene(&native_count);
        /* Camera scene is immutable throughout this publication. Every scene
           change also updates the command/layer or the authoritative image. */
        changed |= native_count!=s_published_native_count ||
            memcmp(native,s_published_native,native_count*sizeof *native)!=0;
        port_camera_scene_copy(s_published_native,PORT_NATIVE_MAX_COMMANDS);
        s_published_native_count=native_count;
        s_published_layer=layer;
        memcpy(s_published_commands,commands,count*sizeof(*commands));
        s_published_command_count=count;
    }
#endif
    if (changed) ++s_image_version;
    memcpy(s_published_frame, frame, sizeof(s_published_frame));
    memcpy(palette, s_palette6, sizeof(palette));
    memcpy(s_published_palette6, palette, sizeof(s_published_palette6));
    frame_id = ++s_frame_id;
    if (s_frame_lock != NULL)
        SDL_UnlockMutex(s_frame_lock);
    /* Busy-wait redraws still have their own trace boundary. Persist only
       changed indexed images so captures cannot fill the disk with duplicates. */
    if (changed)
        dump_frame_to_dir(s_capture_dir, frame_id, reason, frame, palette);
    port_trace_video_publication(reason);
    port_guest_stop_after_publication();
}

void port_video_set_debug_capture_dir(const char *path)
{
    s_debug_capture_dir[0] = '\0';
    if (!path || !*path || strlen(path) >= sizeof(s_debug_capture_dir)) return;
    strcpy(s_debug_capture_dir, path);
}

int port_video_debug_capture_enabled(void) { return s_debug_capture_dir[0] != '\0'; }

void port_video_debug_capture(void)
{
    char directory[1200];
    uint8_t frame[PORT_FRAMEBUFFER_BYTES], palette[sizeof(s_palette6)];
    uint64_t frame_id;
    if (!port_video_debug_capture_enabled()) return;
    /* Do not read mutable guest state from the presentation thread. The frame
       ID links this immutable image to its guest publication in trace.jsonl. */
    SDL_LockMutex(s_frame_lock);
    frame_id = s_frame_id;
    memcpy(frame, s_published_frame, sizeof(frame));
    memcpy(palette, s_published_palette6, sizeof(palette));
    SDL_UnlockMutex(s_frame_lock);
    if (frame_id == 0) return;
    int length = snprintf(directory, sizeof(directory), "%s/capture-%04u",
                          s_debug_capture_dir, ++s_debug_capture_sequence);
    if (length < 0 || (size_t)length >= sizeof(directory) ||
        !SDL_CreateDirectory(directory) ||
        !dump_frame_to_dir(directory, frame_id, "debug_capture", frame, palette) ||
        !dump_bmp(directory, frame, palette)) {
        SDL_Log("Debug capture could not be saved");
        return;
    }
    port_trace_debug_capture(frame_id, directory);
    port_diagnostics_note("debug_capture", directory);
    SDL_Log("Debug capture saved: %s", directory);
}

void port_video_transition_begin(PortVideoTransition *transition)
{
    transition->origin_ns = SDL_GetTicksNS();
    transition->work_units = PORT_TRANSITION_FIXED_WORK;
    transition->next_publication_ns = transition->origin_ns +
        PORT_VGA_FRAME_DOTS * 1000000000ull / PORT_VGA_DOT_CLOCK_HZ;
}

void port_video_transition_advance(PortVideoTransition *transition,
                                   uint32_t work_units)
{
    uint64_t deadline;
    transition->work_units += work_units;
    deadline = transition->origin_ns +
        transition->work_units * 1000000000ull / PORT_DOS_WORK_PER_SECOND;
    for (;;) {
        uint64_t now;
        port_guest_check_stop();
        now = SDL_GetTicksNS();
        if (now >= deadline)
            break;
        SDL_DelayNS(deadline - now);
    }
    /* The DOS loop wrote directly to VRAM throughout its CPU work. Publish
       the same row progression at VGA refresh intervals, rather than letting
       the host overwrite all four completed phases before its next present.
       Absolute deadlines account for native work and avoid per-row drift. */
    if (deadline >= transition->next_publication_ns) {
        port_video_publish("sprite_1_unk3_progress");
        do {
            transition->next_publication_ns +=
                PORT_VGA_FRAME_DOTS * 1000000000ull / PORT_VGA_DOT_CLOCK_HZ;
        } while (deadline >= transition->next_publication_ns);
    }
}

void port_video_present(void)
{
    int output_w = 0;
    int output_h = 0;
    SDL_FRect destination;
    uint8_t frame[PORT_FRAMEBUFFER_BYTES];
    uint8_t palette6[sizeof(s_palette6)];
    SDL_Color colors[256];
    uint64_t frame_id;
    uint64_t image_version;
    int upload;
    if (s_renderer == NULL)
        return;
    if (s_recreate_texture) {
        /* SDL_EVENT_RENDER_DEVICE_RESET invalidates every texture. Uploading
           into the old object does not satisfy SDL's recovery contract. */
        SDL_Texture *replacement = SDL_CreateTexture(s_renderer,
            SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING,
            PORT_SCREEN_WIDTH, PORT_SCREEN_HEIGHT);
        if (replacement == NULL) return;
        if (!SDL_SetTexturePalette(replacement, s_palette) ||
            !SDL_SetTextureScaleMode(replacement, SDL_SCALEMODE_NEAREST)) {
            SDL_DestroyTexture(replacement);
            return;
        }
        SDL_DestroyTexture(s_texture);
        s_texture = replacement;
        s_recreate_texture = 0;
#ifdef PORT_RENDER_INSTRUMENT
        if (port_render_mode()) {
            port_modern_destroy(&s_modern);
            if (!port_modern_init(&s_modern,s_renderer)) return;
        }
#endif
    }
    if (s_texture == NULL) return;
    if (s_frame_lock != NULL)
        SDL_LockMutex(s_frame_lock);
    frame_id = s_frame_id;
    image_version = s_image_version;
    if (frame_id == 0 || (!s_redraw_needed && image_version == s_presented_version)) {
        if (s_frame_lock != NULL) SDL_UnlockMutex(s_frame_lock);
        return;
    }
    upload = image_version != s_uploaded_version;
    if (upload) {
        memcpy(frame, s_published_frame, sizeof(frame));
        memcpy(palette6, s_published_palette6, sizeof(palette6));
#ifdef PORT_RENDER_INSTRUMENT
        s_present_native_count=s_published_native_count;
        memcpy(s_present_native,s_published_native,s_present_native_count*sizeof *s_present_native);
        s_present_layer=s_published_layer;
        s_present_command_count=s_published_command_count;
        memcpy(s_present_commands,s_published_commands,s_present_command_count*sizeof(*s_present_commands));
        memcpy(s_present_frame,frame,sizeof frame);
        memcpy(s_present_palette,palette6,sizeof palette6);
#endif
    }
    if (s_frame_lock != NULL)
        SDL_UnlockMutex(s_frame_lock);
    if (upload) {
        for (unsigned i = 0; i < 256; ++i) {
            colors[i].r = dac6_to_u8(palette6[i * 3]);
            colors[i].g = dac6_to_u8(palette6[i * 3 + 1]);
            colors[i].b = dac6_to_u8(palette6[i * 3 + 2]);
            colors[i].a = SDL_ALPHA_OPAQUE;
        }
        if (!SDL_SetPaletteColors(s_palette, colors, 0, 256) ||
            !SDL_UpdateTexture(s_texture, NULL, frame, PORT_SCREEN_WIDTH)) return;
        s_uploaded_version = image_version;
    }
    if (!SDL_GetRenderOutputSize(s_renderer, &output_w, &output_h))
        return;
    PortViewport viewport = port_viewport_fit(output_w, output_h);
    destination = (SDL_FRect){viewport.x, viewport.y, viewport.w, viewport.h};
    SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(s_renderer);
    int modern_drawn=0;
#ifdef PORT_RENDER_INSTRUMENT
    if (port_render_mode() && s_present_layer.valid) {
        modern_drawn=port_modern_compose(&s_modern,s_present_commands,s_present_command_count,
            s_present_native,s_present_native_count,
            &s_present_layer,s_present_frame,s_present_palette,output_w,output_h);
        if (!modern_drawn && !s_modern_failed) {
            SDL_Log("Modern frame rejected; faithful presentation: %s",SDL_GetError());
            s_modern_failed=1;
        }
    }
#endif
    if (!modern_drawn) SDL_RenderTexture(s_renderer, s_texture, NULL, &destination);
#ifdef PORT_RENDER_INSTRUMENT
    if (modern_drawn) ++s_world_presentations;
    if (modern_drawn && s_render_capture_dir[0] && !s_render_captured && s_world_presentations>=30) {
        char path[1024];
        SDL_Surface *surface=SDL_RenderReadPixels(s_renderer,NULL);
        snprintf(path,sizeof path,"%s/output-%06llu.bmp",s_render_capture_dir,(unsigned long long)frame_id);
        if (surface && SDL_SaveBMP(surface,path)) {
            dump_frame_to_dir(s_render_capture_dir,frame_id,"native_compare",s_present_frame,s_present_palette);
            snprintf(path,sizeof path,"%s/output-%06llu.json",s_render_capture_dir,(unsigned long long)frame_id);
            FILE *f=fopen(path,"wb");
            if (f) {
                fprintf(f,"{\"frame_id\":%llu,\"width\":%d,\"height\":%d,\"camera_commands\":%u,\"projected_commands\":%u,\"backend\":\"%s\"}\n",
                    (unsigned long long)frame_id,output_w,output_h,(unsigned)s_present_native_count,
                    (unsigned)s_present_command_count,SDL_GetRendererName(s_renderer));
                fclose(f);
            }
            s_render_captured=1;
            SDL_Log("Native output and faithful comparison saved: %s",s_render_capture_dir);
        }
        SDL_DestroySurface(surface);
    }
#endif
    if (SDL_RenderPresent(s_renderer)) {
        s_presented_version = image_version;
        s_redraw_needed = 0;
        port_trace_host_present(frame_id);
    }
}

/* The presentation thread owns redraw state. Guest publications only advance
   the image generation under the frame lock; PIT and guest pacing stay intact. */
void port_video_request_redraw(int reupload)
{
    s_redraw_needed = 1;
    if (reupload) {
        s_uploaded_version = UINT64_MAX;
        s_recreate_texture = 1;
    }
}

void port_video_shutdown(void)
{
#ifdef PORT_RENDER_INSTRUMENT
    port_modern_destroy(&s_modern);
#endif
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
    port_guest_check_stop();
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

/* DOS text-mode restoration is owned by the SDL window at host shutdown. */
void video_set_mode7(void) {}
