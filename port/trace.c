#include "port_runtime.h"
#include "audio_backend.h"

#include <SDL3/SDL.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static SDL_Mutex *s_lock;
static FILE *s_trace;
static uint64_t s_trace_origin_ns;
static uint64_t s_sim_step_id;
static uint64_t s_video_frame_id;
static uint64_t s_present_id;
static uint64_t s_audio_publication_id;
static uint64_t s_indices_hash, s_palette_hash;

extern uint16_t port_game_frame_snapshot(void);
extern uint16_t port_game_y_rotation_snapshot(void);

static void json_string(FILE *out, const char *value)
{
    const unsigned char *p = (const unsigned char *)(value != NULL ? value : "");
    fputc('"', out);
    while (*p != 0) {
        switch (*p) {
        case '"': fputs("\\\"", out); break;
        case '\\': fputs("\\\\", out); break;
        case '\n': fputs("\\n", out); break;
        case '\r': fputs("\\r", out); break;
        case '\t': fputs("\\t", out); break;
        default:
            if (*p < 0x20)
                fprintf(out, "\\u%04x", (unsigned)*p);
            else
                fputc(*p, out);
            break;
        }
        ++p;
    }
    fputc('"', out);
}

static void trace_lock(void)
{
    if (s_lock != NULL)
        SDL_LockMutex(s_lock);
}

static void trace_unlock(void)
{
    if (s_lock != NULL)
        SDL_UnlockMutex(s_lock);
}

static uint64_t relative_ns(uint64_t absolute_ns)
{
#ifdef PORT_TICK_HANDSHAKE
    return absolute_ns; /* input timestamps already belong to the tick timeline */
#else
    return absolute_ns >= s_trace_origin_ns ? absolute_ns - s_trace_origin_ns : 0;
#endif
}

static uint64_t game_ns(uint64_t diagnostic_host_ns)
{
#ifdef PORT_TICK_HANDSHAKE
    (void)diagnostic_host_ns;
    return port_timer_machine_time_ns();
#else
    return relative_ns(diagnostic_host_ns);
#endif
}

void port_trace_open(const char *path, const char *asset_root)
{
    s_lock = SDL_CreateMutex();
    s_trace_origin_ns = SDL_GetTicksNS();
    s_sim_step_id = 0;
    s_video_frame_id = 0;
    s_present_id = 0;
    s_audio_publication_id = 0;
    if (path == NULL || path[0] == '\0')
        return;
    s_trace = fopen(path, "wb");
    if (s_trace == NULL) {
        SDL_Log("Could not open trace file '%s': %s", path != NULL ? path : "runtime-trace.jsonl",
                strerror(errno));
        return;
    }
    fputs("{\"trace_schema\":\"stunts-runtime-trace-v1\",\"event_type\":\"header\","
          "\"runtime_mode\":\"startup\",\"screen_width\":320,\"screen_height\":200,"
          "\"indexed_palette_entries\":256,\"timer_target_hz\":99.99846,"
          "\"timer_period_ns\":10000154,\"build_id\":", s_trace);
    json_string(s_trace, port_diagnostics_build_id());
    fputs(",\"asset_root\":", s_trace);
    json_string(s_trace, asset_root);
    fputs("}\n", s_trace);
    fflush(s_trace);
}

void port_trace_close(void)
{
    if (s_trace != NULL) {
        fflush(s_trace);
        fclose(s_trace);
        s_trace = NULL;
    }
    if (s_lock != NULL) {
        SDL_DestroyMutex(s_lock);
        s_lock = NULL;
    }
}

void port_trace_timer_tick(uint64_t tick_id, uint64_t scheduled_ns,
                           uint64_t observed_ns)
{
    uint64_t scheduled = relative_ns(scheduled_ns);
#ifdef PORT_TICK_HANDSHAKE
    uint64_t observed = scheduled;
#else
    uint64_t observed = relative_ns(observed_ns);
#endif
    trace_lock();
    if (s_trace != NULL) {
        fprintf(s_trace,
                "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"timer_tick\",\"tick_id\":%llu,"
                "\"scheduled_tick\":%llu,\"observed_tick\":%llu,"
                "\"raw_tick_id\":%llu,\"machine_tick\":%llu,\"host_ns\":%llu,"
                "\"timer_target_hz\":99.99846}\n",
                (unsigned long long)tick_id,
                (unsigned long long)scheduled,
                (unsigned long long)observed,
                (unsigned long long)(observed / PORT_TIMER_PERIOD_NS),
                (unsigned long long)observed,
                (unsigned long long)observed_ns);
        fflush(s_trace);
    }
    trace_unlock();
}

uint64_t port_trace_sim_step(const uint8_t *game_state, size_t state_size)
{
    uint64_t now = SDL_GetTicksNS();
    uint16_t game_frame = port_game_frame_snapshot();
    uint8_t game_mode = port_game_mode_snapshot();
    uint8_t input_mode = port_game_inputmode_snapshot();
    uint8_t replay_mode = port_game_replaymode_snapshot();
    uint16_t rate = port_game_rate_snapshot();
    uint64_t step_id;
    size_t i;
    static const char hex[] = "0123456789abcdef";
    trace_lock();
    step_id = ++s_sim_step_id;
    if (s_trace != NULL) {
        fprintf(s_trace,
                "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"simulation_step\",\"sim_step_id\":%llu,"
                "\"game_frame\":%u,\"raw_tick_id\":%llu,\"machine_tick\":%llu,\"host_ns\":%llu,"
                "\"runtime_mode\":",
                (unsigned long long)step_id,
                (unsigned)game_frame,
                (unsigned long long)(game_ns(now) / PORT_TIMER_PERIOD_NS),
                (unsigned long long)game_ns(now),
                (unsigned long long)now);
        json_string(s_trace, replay_mode != 0 ? "replay" :
                    (game_mode == 0 ? "live" : "menu"));
        fprintf(s_trace,
                ",\"game_mode\":%u,\"game_inputmode\":%u,"
                "\"game_replay_mode\":%u,\"rate_target_hz\":%u}\n",
                (unsigned)game_mode, (unsigned)input_mode,
                (unsigned)replay_mode, (unsigned)rate);
        fprintf(s_trace,
                "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"state_snapshot\",\"sim_step_id\":%llu,"
                "\"state_type\":\"GAMESTATE\",\"raw_tick_id\":%llu,\"state_size\":%u,"
                "\"data_segment\":null,\"dgroup_anchor_ok\":false,"
                "\"state_origin\":\"SDL3_GUEST_NATIVE\",\"bytes_hex\":\"",
                (unsigned long long)step_id,
                (unsigned long long)(game_ns(now) / PORT_TIMER_PERIOD_NS), (unsigned)state_size);
        if (game_state != NULL) {
            for (i = 0; i < state_size; ++i) {
                fputc(hex[game_state[i] >> 4], s_trace);
                fputc(hex[game_state[i] & 0x0Fu], s_trace);
            }
        }
        fputs("\"}\n", s_trace);
        fflush(s_trace);
    }
    trace_unlock();
    return step_id;
}

void port_trace_input_keyboard(uint64_t sequence, uint64_t scheduled_ns,
                               uint64_t actual_ns, const uint8_t *codes,
                               size_t code_count)
{
    size_t i;
    trace_lock();
    if (s_trace != NULL) {
        fprintf(s_trace,
                "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"input\",\"channel\":\"dos.keyboard.scancodes\","
                "\"sequence\":%llu,\"scheduled_tick\":%llu,"
                "\"raw_tick_id\":%llu,\"machine_tick\":%llu,\"payload\":[",
                (unsigned long long)sequence,
                (unsigned long long)relative_ns(scheduled_ns),
                (unsigned long long)(relative_ns(actual_ns) / PORT_TIMER_PERIOD_NS),
                (unsigned long long)relative_ns(actual_ns));
        for (i = 0; i < code_count; ++i)
            fprintf(s_trace, "%s%u", i == 0 ? "" : ",", (unsigned)codes[i]);
        fputs("]}\n", s_trace);
        fflush(s_trace);
    }
    trace_unlock();
}

void port_trace_input_mouse(uint64_t sequence, uint64_t scheduled_ns,
                            uint64_t actual_ns, double u, double v,
                            uint16_t buttons)
{
    trace_lock();
    if (s_trace != NULL) {
        fprintf(s_trace,
                "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"input\",\"channel\":\"dos.mouse\","
                "\"sequence\":%llu,\"scheduled_tick\":%llu,"
                "\"raw_tick_id\":%llu,\"machine_tick\":%llu,\"payload\":{"
                "\"u\":%.9f,\"v\":%.9f,\"buttons\":%u}}\n",
                (unsigned long long)sequence,
                (unsigned long long)relative_ns(scheduled_ns),
                (unsigned long long)(relative_ns(actual_ns) / PORT_TIMER_PERIOD_NS),
                (unsigned long long)relative_ns(actual_ns),
                u, v, (unsigned)buttons);
        fflush(s_trace);
    }
    trace_unlock();
}

void port_trace_key_consumed(uint16_t bios_word)
{
    trace_lock();
    if (s_trace != NULL)
        fprintf(s_trace, "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"key_consumed\",\"raw_tick_id\":%llu,\"bios_word\":%u}\n",
                (unsigned long long)(game_ns(0) / PORT_TIMER_PERIOD_NS), (unsigned)bios_word);
    trace_unlock();
}

static uint64_t indexed_hash(const uint8_t *bytes, size_t count)
{
    uint64_t hash = UINT64_C(14695981039346656037);
    size_t i;
    for (i = 0; i < count; ++i)
        hash = (hash ^ bytes[i]) * UINT64_C(1099511628211);
    return hash;
}

void port_trace_indexed_hashes(const uint8_t *indices, const uint8_t *palette6)
{
    if (s_trace == NULL) return;
    s_indices_hash = indexed_hash(indices, 64000);
    s_palette_hash = indexed_hash(palette6, 768);
}

void port_trace_video_publication(const char *reason)
{
    uint64_t now = SDL_GetTicksNS();
    uint16_t game_frame = port_game_frame_snapshot();
    uint8_t game_mode = port_game_mode_snapshot();
    uint8_t input_mode = port_game_inputmode_snapshot();
    uint8_t replay_mode = port_game_replaymode_snapshot();
    uint16_t rate = port_game_rate_snapshot();
    uint16_t y_rotation = port_game_y_rotation_snapshot();
    trace_lock();
    ++s_video_frame_id;
    if (s_trace != NULL) {
        fprintf(s_trace,
                "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"%s\",\"frame_id\":%llu,"
                "\"sim_step_id\":%llu,\"game_frame\":%u,"
                "\"video_phase\":\"%s\",\"raw_tick_id\":%llu,\"machine_tick\":%llu,"
                "\"host_ns\":%llu,\"game_mode\":%u,"
                "\"game_inputmode\":%u,\"game_replay_mode\":%u,"
                "\"rate_target_hz\":%u,\"renderer_y_rotation\":%u,\"publication_reason\":",
                strcmp(reason,"race_frame_complete")==0?"frame_complete":"video_publication",
                (unsigned long long)s_video_frame_id,
                (unsigned long long)s_sim_step_id,
                (unsigned)game_frame,
                strcmp(reason,"race_frame_complete")==0?"frame_end":"frame_start",
                (unsigned long long)(game_ns(now) / PORT_TIMER_PERIOD_NS),
                (unsigned long long)game_ns(now),
                (unsigned long long)now,
                (unsigned)game_mode, (unsigned)input_mode,
                (unsigned)replay_mode, (unsigned)rate, (unsigned)y_rotation);
        json_string(s_trace, reason);
        fprintf(s_trace, ",\"indices_fnv1a64\":\"%016llx\",\"palette_fnv1a64\":\"%016llx\"}\n",
                (unsigned long long)s_indices_hash, (unsigned long long)s_palette_hash);
        fflush(s_trace);
    }
    trace_unlock();
}

void port_trace_host_present(uint64_t frame_id)
{
    uint64_t now = SDL_GetTicksNS();
    trace_lock();
    ++s_present_id;
    if (s_trace != NULL) {
        fprintf(s_trace,
                "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"host_present\",\"frame_id\":%llu,"
                "\"present_id\":%llu,\"raw_tick_id\":%llu,\"machine_tick\":%llu,\"host_ns\":%llu}\n",
                (unsigned long long)frame_id,
                (unsigned long long)s_present_id,
                (unsigned long long)(game_ns(now) / PORT_TIMER_PERIOD_NS),
                (unsigned long long)game_ns(now),
                (unsigned long long)now);
        fflush(s_trace);
    }
    trace_unlock();
}

void port_trace_debug_capture(uint64_t frame_id, const char *directory)
{
    uint64_t now = SDL_GetTicksNS();
    trace_lock();
    if (s_trace != NULL) {
        fprintf(s_trace, "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                        "\"event_type\":\"debug_capture\",\"frame_id\":%llu,"
                        "\"raw_tick_id\":%llu,\"machine_tick\":%llu,\"directory\":",
                (unsigned long long)frame_id,
                (unsigned long long)(game_ns(now) / PORT_TIMER_PERIOD_NS),
                (unsigned long long)game_ns(now));
        json_string(s_trace, directory);
        fputs("}\n", s_trace);
        fflush(s_trace);
    }
    trace_unlock();
}

void port_trace_audio_publication(uint32_t frame_count)
{
    uint64_t now = SDL_GetTicksNS();
    trace_lock();
    ++s_audio_publication_id;
    if (s_trace != NULL) {
        fprintf(s_trace,
                "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"audio_publication\","
                "\"audio_publication_id\":%llu,\"audio_frame_count\":%u,"
                "\"raw_tick_id\":%llu,\"machine_tick\":%llu,\"host_ns\":%llu,"
                "\"audio_backend\":\"%s\"}\n",
                (unsigned long long)s_audio_publication_id,
                (unsigned)frame_count,
                (unsigned long long)(game_ns(now) / PORT_TIMER_PERIOD_NS),
                (unsigned long long)game_ns(now),
                (unsigned long long)now, port_audio_backend_name());
        fflush(s_trace);
    }
    trace_unlock();
}

void port_trace_host_stop(const char *reason)
{
    uint64_t now = SDL_GetTicksNS();
    trace_lock();
    if (s_trace != NULL) {
        fprintf(s_trace,
                "{\"trace_schema\":\"stunts-runtime-trace-v1\","
                "\"event_type\":\"host_stop\",\"raw_tick_id\":%llu,\"machine_tick\":%llu,"
                "\"host_ns\":%llu,\"reason\":",
                (unsigned long long)(game_ns(now) / PORT_TIMER_PERIOD_NS),
                (unsigned long long)game_ns(now),
                (unsigned long long)now);
        json_string(s_trace, reason);
        fputs("}\n", s_trace);
        fflush(s_trace);
    }
    trace_unlock();
}

void port_trace_startup(uint64_t seed,const char *source,int rate,int detail,int dirty)
{
    trace_lock();
    if(s_trace) {
        fprintf(s_trace,"{\"event_type\":\"startup_settings\",\"seed\":\"%016llx\",\"seed_source\":\"%s\",\"frame_rate\":%d,\"detail_level\":%d,\"drawing_path\":\"%s\"}\n",
            (unsigned long long)seed,source,rate,detail,dirty?"dirty_rects":"full");
        fflush(s_trace);
    }
    trace_unlock();
}

#ifdef PORT_AUDIO_TICK_SINK
void port_trace_audio_sink(void)
{
    PortAudioSinkStats s; port_audio_sdl_stats(&s);
    uint64_t tick=port_timer_raw_tick(), host=SDL_GetTicksNS();
    trace_lock();
    if(s_trace) {
        fprintf(s_trace,"{\"event_type\":\"audio_sink\",\"raw_tick_id\":%llu,\"host_ns\":%llu,\"submitted_frames\":%u,\"consumed_frames\":%u,\"dropped_frames\":%u,\"underrun_frames\":%u,\"underrun_callbacks\":%u,\"device_buffer_frames\":%u}\n",
            (unsigned long long)tick,(unsigned long long)host,
            s.submitted,s.consumed,s.dropped,s.underrun_frames,s.underrun_callbacks,s.device_buffer_frames);
        fflush(s_trace);
    }
    trace_unlock();
}

#endif
