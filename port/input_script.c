#include "port_runtime.h"

#include <SDL3/SDL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCRIPT_FILE_LIMIT (1024u * 1024u)
/* Recorded UI sessions include frequent mouse observations. Keep their full
   input stream rather than requiring lossy path simplification. */
#define SCRIPT_EVENT_CAP 4096u
#define SCRIPT_SCANCODE_CAP 64u

typedef struct PortScriptEvent {
    uint64_t sequence;
    uint64_t due_ns;
    uint64_t due_tick;
    int mouse;
    size_t code_count;
    uint8_t codes[SCRIPT_SCANCODE_CAP];
    double mouse_u;
    double mouse_v;
    uint16_t mouse_buttons;
    unsigned phase;
    unsigned visit;
    int checkpoint, stop;
    char label[64];
    int consumed;
} PortScriptEvent;

static PortScriptEvent s_events[SCRIPT_EVENT_CAP];
static size_t s_event_count;
static size_t s_next_event;
static uint64_t s_origin_ns;
static int s_semantic;
static unsigned s_phase;
static uint64_t s_key_ordinal;
static int s_scoped;
static unsigned s_evaluation_score;
static unsigned s_visits[32],s_visit,s_scope_depth;
static struct {unsigned phase,visit;uint64_t key;} s_scopes[32];

static char *json_key_value(char *begin, char *limit, const char *key)
{
    char *found = strstr(begin, key);
    char *colon;
    if (found == NULL || found >= limit)
        return NULL;
    colon = strchr(found + strlen(key), ':');
    if (colon == NULL || colon >= limit)
        return NULL;
    ++colon;
    while (colon < limit && (*colon == ' ' || *colon == '\t' ||
                             *colon == '\r' || *colon == '\n'))
        ++colon;
    return colon < limit ? colon : NULL;
}

static int json_read_string(char *begin, char *limit, const char *key,
                            char *output, size_t capacity)
{
    char *value = json_key_value(begin, limit, key);
    char *end;
    size_t length;
    if (value == NULL || *value != '"' || capacity == 0)
        return 0;
    ++value;
    end = strchr(value, '"');
    if (end == NULL || end >= limit)
        return 0;
    length = (size_t)(end - value);
    if (length >= capacity)
        return 0;
    memcpy(output, value, length);
    output[length] = '\0';
    return 1;
}

static int json_read_u64(char *begin, char *limit, const char *key,
                         uint64_t *output)
{
    char *value = json_key_value(begin, limit, key);
    char *end;
    unsigned long long parsed;
    if (value == NULL || *value == '-')
        return 0;
    errno = 0;
    parsed = strtoull(value, &end, 10);
    if (errno != 0 || end == value || end > limit)
        return 0;
    *output = (uint64_t)parsed;
    return 1;
}

static int json_read_double(char *begin, char *limit, const char *key,
                            double *output)
{
    char *value = json_key_value(begin, limit, key);
    char *end;
    double parsed;
    if (value == NULL)
        return 0;
    parsed = strtod(value, &end);
    if (end == value || end > limit)
        return 0;
    *output = parsed;
    return 1;
}

static int json_read_mouse(char *begin, char *limit, PortScriptEvent *event)
{
    uint64_t buttons;
    if (!json_read_double(begin, limit, "\"u\"", &event->mouse_u) ||
        !json_read_double(begin, limit, "\"v\"", &event->mouse_v) ||
        !json_read_u64(begin, limit, "\"buttons\"", &buttons) ||
        event->mouse_u < 0.0 || event->mouse_u > 1.0 ||
        event->mouse_v < 0.0 || event->mouse_v > 1.0 || buttons > 7u)
        return 0;
    event->mouse_buttons = (uint16_t)buttons;
    event->mouse = 1;
    return 1;
}

static int json_read_scancodes(char *begin, char *limit,
                               PortScriptEvent *event)
{
    char *value = json_key_value(begin, limit, "\"payload\"");
    if (value == NULL || *value != '[')
        return 0;
    ++value;
    for (;;) {
        char *end;
        unsigned long parsed;
        while (value < limit && (*value == ' ' || *value == '\t' ||
                                 *value == '\r' || *value == '\n' ||
                                 *value == ','))
            ++value;
        if (value >= limit)
            return 0;
        if (*value == ']')
            return 1;
        if (*value == '-' || event->code_count == SCRIPT_SCANCODE_CAP)
            return 0;
        errno = 0;
        parsed = strtoul(value, &end, 10);
        if (errno != 0 || end == value || end > limit || parsed > 255u)
            return 0;
        event->codes[event->code_count++] = (uint8_t)parsed;
        value = end;
    }
}

static int parse_script(const char *path)
{
    FILE *stream = NULL;
    char *text = NULL;
    long file_size;
    size_t bytes_read;
    char format[80];
    char *anchor_ptr;
    char *events_ptr;
    char *scan;
    uint64_t anchor_tick;
    uint64_t previous_due = 0;
    int have_previous = 0;
    int ok = 0;

    s_event_count = 0;
    s_next_event = 0;
    s_semantic=0;s_scoped=0;s_evaluation_score=0;s_phase=0;s_key_ordinal=0;
    s_visit=s_scope_depth=0;memset(s_visits,0,sizeof(s_visits));
    if (path == NULL || path[0] == '\0')
        return 1;
    stream = fopen(path, "rb");
    if (stream == NULL) {
        fprintf(stderr, "PORT input script: cannot open '%s'\n", path);
        return 0;
    }
    if (fseek(stream, 0, SEEK_END) != 0 ||
        (file_size = ftell(stream)) < 0 ||
        (unsigned long)file_size > SCRIPT_FILE_LIMIT ||
        fseek(stream, 0, SEEK_SET) != 0)
        goto done;
    text = (char *)malloc((size_t)file_size + 1u);
    if (text == NULL)
        goto done;
    bytes_read = fread(text, 1, (size_t)file_size, stream);
    if (bytes_read != (size_t)file_size)
        goto done;
    text[bytes_read] = '\0';
    if (!json_read_string(text, text + bytes_read, "\"format\"", format,
                          sizeof(format)) ||
        (strcmp(format, "stunts-step-input-v1") != 0 &&
         strcmp(format, "stunts-step-input-v2") != 0 &&
         strcmp(format, "stunts-exact-input-script-v1") != 0)) {
        fprintf(stderr,
                "PORT input script: expected stunts-exact-input-script-v1\n");
        goto done;
    }
    s_scoped=strcmp(format,"stunts-step-input-v2")==0;
    s_semantic = s_scoped || strcmp(format,"stunts-step-input-v1")==0;
    memset(s_visits,0,sizeof(s_visits));s_visit=s_scope_depth=0;
    s_phase=0;s_key_ordinal=0;
    s_evaluation_score=0;
    if(s_scoped){uint64_t value=0;
        json_read_u64(text,text+bytes_read,"\"evaluation_finish_frames\"",&value);
        if(value>32767)goto done;
        s_evaluation_score=(unsigned)value;}
    anchor_ptr = json_key_value(text, text + bytes_read, "\"anchor_tick\"");
    events_ptr = strstr(text, "\"events\"");
    if (anchor_ptr == NULL || events_ptr == NULL ||
        !json_read_u64(text, events_ptr, "\"anchor_tick\"", &anchor_tick)) {
        fprintf(stderr, "PORT input script: missing anchor_tick or events\n");
        goto done;
    }

    scan = events_ptr;
    for (;;) {
        const char *stamp_key=s_semantic?"\"at\"":"\"visible_tick\"";
        char *visible = strstr(scan, stamp_key);
        char *next_visible;
        char *event_limit;
        char channel[64];
        uint64_t absolute_tick;
        uint64_t due_ns;
        PortScriptEvent event;
        if (visible == NULL)
            break;
        next_visible = strstr(visible + 1, stamp_key);
        event_limit = next_visible != NULL ? next_visible : text + bytes_read;
        memset(&event, 0, sizeof(event));
        if (!json_read_u64(visible, event_limit, stamp_key,
                           &absolute_tick) ||
            !json_read_string(visible, event_limit, "\"channel\"", channel,
                              sizeof(channel))) {
            fprintf(stderr, "PORT input script: malformed event %zu\n",
                    s_event_count);
            goto done;
        }
        if ((strcmp(channel, "dos.keyboard.scancodes") == 0 &&
             !json_read_scancodes(visible, event_limit, &event)) ||
            (strcmp(channel, "dos.mouse.normalized") == 0 &&
             !json_read_mouse(visible, event_limit, &event)) ||
            (strcmp(channel, "dos.keyboard.scancodes") != 0 &&
             strcmp(channel, "dos.mouse.normalized") != 0 &&
             !(s_scoped && strcmp(channel,"oracle.checkpoint")==0))) {
            fprintf(stderr,
                    "PORT input script: event %zu has an unsupported channel or payload\n",
                    s_event_count);
            goto done;
        }
        if (absolute_tick < anchor_tick) {
            scan = event_limit;
            continue;
        }
        due_ns = absolute_tick - anchor_tick;
        if ((!s_semantic && have_previous && due_ns < previous_due) ||
            s_event_count >= SCRIPT_EVENT_CAP) {
            fprintf(stderr,
                    "PORT input script: unordered or excessive event list\n");
            goto done;
        }
        event.sequence = s_event_count;
        event.due_ns = due_ns;
        if(s_semantic) {
            uint64_t phase;
            if(!json_read_u64(visible,event_limit,"\"phase\"",&phase) || phase<1 || phase>31)goto done;
            event.phase=(unsigned)phase;
            event.visit=1;
            if(s_scoped){
                uint64_t visit,stop=0;
                if(json_read_u64(visible,event_limit,"\"visit\"",&visit))event.visit=(unsigned)visit;
                if(strcmp(channel,"oracle.checkpoint")==0){
                    if(!json_read_string(visible,event_limit,"\"label\"",event.label,sizeof(event.label)))goto done;
                    json_read_u64(visible,event_limit,"\"stop\"",&stop);
                    event.checkpoint=1;event.stop=stop!=0;
                }
            }
        }
        s_events[s_event_count++] = event;
        previous_due = due_ns;
        have_previous = 1;
        scan = event_limit;
    }
#ifdef PORT_TICK_HANDSHAKE
    s_origin_ns = 0;
    for (size_t i = 0; i < s_event_count; ++i) {
        uint64_t ns = s_events[i].due_ns;
        uint64_t tick = ns / PORT_TIMER_PERIOD_NS + (ns % PORT_TIMER_PERIOD_NS != 0);
        s_events[i].due_tick = tick ? tick : 1; /* first grant is tick one */
        if(!s_semantic)s_events[i].due_ns = s_events[i].due_tick * PORT_TIMER_PERIOD_NS;
    }
#else
    s_origin_ns = SDL_GetTicksNS();
#endif
    ok = 1;

done:
    if (stream != NULL)
        fclose(stream);
    free(text);
    return ok;
}

int port_input_script_load(const char *path)
{
    if (!parse_script(path))
        return 0;
    if (path != NULL && path[0] != '\0')
        fprintf(stderr, "PORT input script: loaded %zu exact-time event(s)\n",
                s_event_count);
    return 1;
}

void port_input_script_pump(uint64_t now_ns)
{
    if(s_semantic)return;
    while (s_next_event < s_event_count) {
        PortScriptEvent *event = &s_events[s_next_event];
#ifndef PORT_TICK_HANDSHAKE
        uint64_t elapsed_ns = now_ns >= s_origin_ns ? now_ns - s_origin_ns : 0;
#endif
        uint64_t scheduled_ns = s_origin_ns + event->due_ns;
#ifdef PORT_TICK_HANDSHAKE
        if (now_ns / PORT_TIMER_PERIOD_NS < event->due_tick) break;
#else
        if (elapsed_ns < event->due_ns) break;
#endif
        if (event->mouse) {
            /* The input protocol uses the DOS mouse device's inclusive bounds
               0..320 and 0..200, not the last visible pixel coordinates.
               The game's mouse bounds handle the endpoint clamp. */
            int16_t x = (int16_t)(event->mouse_u * PORT_SCREEN_WIDTH);
            int16_t y = (int16_t)(event->mouse_v * PORT_SCREEN_HEIGHT);
            port_input_mouse_set(x, y);
            port_input_mouse_set_buttons(event->mouse_buttons);
            port_trace_input_mouse(event->sequence, scheduled_ns, now_ns,
                                   event->mouse_u, event->mouse_v,
                                   event->mouse_buttons);
        } else {
            size_t i;
            for (i = 0; i < event->code_count; ++i)
                port_input_apply_dos_scancode(event->codes[i]);
            port_trace_input_keyboard(event->sequence, scheduled_ns, now_ns,
                                      event->codes, event->code_count);
        }
        ++s_next_event;
    }
}

static void semantic_deliver(unsigned phase,uint64_t at)
{
    size_t i;
    if(!s_semantic)return;
    for(i=0;i<s_event_count;++i){
        PortScriptEvent *event=&s_events[i];
        uint64_t now;
        if(event->consumed || event->phase!=phase || event->due_ns!=at ||
           (s_scoped && phase!=6 && event->visit!=s_visit))continue;
        event->consumed=1;
        if(event->checkpoint){
#ifdef PORT_BOUNDARY_RECORD
            char reason[80];snprintf(reason,sizeof(reason),"checkpoint.%s",event->label);
            port_video_publish(reason);
            if(event->stop){port_guest_request_stop("scenario checkpoint reached");port_guest_check_stop();}
#endif
            continue;
        }
#ifdef PORT_TICK_HANDSHAKE
        now=port_timer_machine_time_ns();
#else
        now=SDL_GetTicksNS();
#endif
        if(event->mouse){
            port_input_mouse_set((int16_t)(event->mouse_u*PORT_SCREEN_WIDTH),
                                 (int16_t)(event->mouse_v*PORT_SCREEN_HEIGHT));
            port_input_mouse_set_buttons(event->mouse_buttons);
            port_trace_input_mouse(event->sequence,now,now,event->mouse_u,event->mouse_v,event->mouse_buttons);
        }else{
            size_t j;
            for(j=0;j<event->code_count;++j)port_input_apply_dos_scancode(event->codes[j]);
            port_trace_input_keyboard(event->sequence,now,now,event->codes,event->code_count);
        }
    }
}
void port_input_script_phase(unsigned phase)
{
    if(s_scoped)return;
    if(s_phase!=phase){s_phase=phase;s_key_ordinal=0;}
}
void port_input_script_poll(void){if(!s_scoped || s_phase==4 || s_phase==5)semantic_deliver(s_phase,s_key_ordinal);}
void port_input_script_ui_poll(void){if(s_scoped)semantic_deliver(s_phase,s_key_ordinal);}
void port_input_script_key_consumed(void){++s_key_ordinal;}
void port_input_script_step(uint16_t frame){semantic_deliver(6,frame);}
unsigned port_input_script_phase_value(void){return s_phase;}
unsigned port_input_script_evaluation_score(void){return s_evaluation_score;}
void port_input_script_context(unsigned *phase,unsigned *visit,uint64_t *key){
    *phase=s_phase;*visit=s_visit;*key=s_key_ordinal;
}
void port_input_script_scope_enter(unsigned phase){
    if(!s_scoped)return;
    if(phase>=32 || s_scope_depth>=32)port_guest_unwind("scenario scope overflow");
    s_scopes[s_scope_depth].phase=s_phase;s_scopes[s_scope_depth].visit=s_visit;
    s_scopes[s_scope_depth++].key=s_key_ordinal;
    s_phase=phase;s_visit=++s_visits[phase];s_key_ordinal=0;
}
void port_input_script_scope_leave(void){
    if(!s_scoped)return;
    if(!s_scope_depth)port_guest_unwind("scenario scope underflow");
    semantic_deliver(s_phase,UINT32_MAX); /* explicit completed-call checkpoint */
    --s_scope_depth;s_phase=s_scopes[s_scope_depth].phase;
    s_visit=s_scopes[s_scope_depth].visit;s_key_ordinal=s_scopes[s_scope_depth].key;
}
