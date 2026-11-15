"""Observer-only hooks in generated audio views; frozen sources stay untouched."""
import re


def _function(text, name, edit):
    match = re.search(r"(?m)^[^;\n]*\b" + name + r"\([^;]*?\)\s*\{", text)
    if match is None:
        raise ValueError(f"Missing sound observer function {name}")
    start = text.index("{", match.start())
    depth, end = 1, start + 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[:start + 1] + edit(text[start + 1:end - 1]) + text[end - 1:]


def _hook(kind, chunk, voice, resource, values):
    return ("\n#ifdef PORT_SOUND_RECORD\n"
            "    if (port_sound_record_enabled())\n"
            f'        port_sound_record_logical("{kind}", {chunk}, {voice}, {resource},\n'
            "            (const uint32_t[]){" + ", ".join(f"(uint32_t)({v})" for v in values) +
            f"}}, {len(values)});\n#endif\n")


def adapt_sound_observers(source, text, header):
    if source not in {"lib/audiores.c", "lib/audioply.c"}:
        return text
    # This is a host header, outside the immutable include/ namespace. Angle
    # inclusion keeps extracted probes' strict project-header scanner intact.
    text = f'#include <{header}>\n' + text

    def replace(body, old, new, count=1):
        if body.count(old) != count:
            raise ValueError(f"Sound observer anchor changed in {source}: {old}")
        return body.replace(old, new)

    if source == "lib/audiores.c":
        def chunk_init(body):
            anchor = "            chunk->event_pos = 0;\n        }"
            return replace(body, anchor, anchor + _hook("chunk_init", "i", "-1",
                "chunk->event_pos", ["chunk->volume", "chunk->priority", "chunk->index"]))
        text = _function(text, "audio_init_chunk", chunk_init)
        text += """
#ifdef PORT_SOUND_RECORD
void port_sound_record_guest_tables(void)
{
    port_sound_record_tables(audiochunktable, sizeof(audiochunktable[0]),
                            snd_voices_tbl, sizeof(snd_voices_tbl[0]));
}
#endif
"""
    else:
        def chunk_event(body):
            anchor = "            read_audio_event(&audioevent, chunk->pos);"
            body = replace(body, anchor, anchor + """
#ifdef PORT_SOUND_RECORD
            if (port_sound_record_enabled())
                port_sound_record_event(chunkIndex, chunk->pos, audioevent.command,
                    audioevent.param, audioevent.value, audioevent.delta, audioevent.length,
                    audioevent.command == AUDIO_EVENT_CALL ? 2 :
                    (audioevent.command < AUDIO_EVENT_CONTROL_FIRST ||
                     audioevent.command == AUDIO_EVENT_CONTROLLER ||
                     audioevent.command == AUDIO_EVENT_WORD_PARAMETER) ? 1 : 0);
#endif
""")
            anchor = "                    callback = chunk->callback;"
            body = replace(body, anchor, _hook("chunk_complete", "chunkIndex", "-1",
                "chunk->pos", ["audioevent.command", "chunk->callback != 0"]) + anchor, 2)
            anchor = "    chunk->delay--;"
            return replace(body, anchor, anchor + _hook("chunk_state", "chunkIndex", "-1",
                "chunk->pos", ["chunk->delay", "chunk->activeVoices", "chunk->depth",
                               "chunk->loopDepth", "chunk->volume", "chunk->modeValue"]))
        text = _function(text, "process_audio_chunk_event", chunk_event)

        def voice_complete(body):
            anchor = "                voice->active = AUDIO_VOICE_IDLE;"
            return replace(body, anchor, _hook("voice_complete", "voice->resourceIndex", "i",
                "voice->data", ["voice->position", "voice->remaining", "voice->envelope_value"]) + anchor)
        text = _function(text, "reset_audio_event_state", voice_complete)

        def voice_progress(body):
            anchor = "    voice->position++;"
            return replace(body, anchor, anchor + _hook("voice_progress", "voice->resourceIndex",
                "(int)(((uintptr_t)voice - (uintptr_t)snd_voices_tbl) / sizeof(*voice))",
                "voice->data", ["voice->position", "voice->remaining", "voice->active",
                               "voice->envelope_state", "voice->envelope_value"]))
        text = _function(text, "clear_audio_voice", voice_progress)
    return text
