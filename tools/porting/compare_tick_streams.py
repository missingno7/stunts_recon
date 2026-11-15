"""Strict, ordered original/port comparisons: never search or shift tick labels.

The common video scope is the explicitly hooked publication routines (see docs).
All original raw stack words remain in the capture; only typed scalar arguments
and meaningful driver returns participate in the current portable projection.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import struct

FRAME_BYTES = 64768
VIDEO_REASONS = {"palette", "sprite_blit_to_video", "sprite_1_unk3",
                 "shape2d_op_unk3", "preRender_line", "preRender_sphere",
                 "shape2d_op_unk", "shape2d_op_unknown5", "draw_filled_rect",
                 "preRender_polygon", "doshowmouse", "dohidemouse", "sprite_1_unk", "clear_rect",
                 "putpixel_iconFillings", "putpixel_iconMask", "shape2d_op_unk4",
                 "shape2d_render_bmp_as_mask", "tmaskxcfxya", "tmaskxf", "shapeexpl"}
# Offsets within the original C FAR call's argument words. Pointer words are
# retained separately in raw captures, never compared with host addresses.
SCALARS = {0x00: (), 0x03: (), 0x06: (), 0x09: (0, 3, 4), 0x0c: (0,),
           0x0f: (0,), 0x12: (0, 2), 0x15: (0, 2, 3), 0x18: (),
           0x1b: (1, 2), 0x1e: (0,), 0x21: (0,), 0x24: (0, 2),
           0x27: (0,), 0x30: (), 0x36: (0,), 0x39: (0,), 0x3c: (),
           0x3f: (0,), 0x42: ()}


def read_rows(path):
    return [json.loads(line) for line in Path(path).read_text(encoding="utf-8").splitlines() if line.strip()]


def fnv(data):
    h = 14695981039346656037
    for b in data:
        h = ((h ^ b) * 1099511628211) & ((1 << 64)-1)
    return f"{h:016x}"


def state_projection(data):
    if len(data) != 1120:
        raise ValueError("GAMESTATE must be exactly 1120 DOS bytes")
    # Locked GAMESTATE: game_frame +140h, player CARSTATE +152h, posWorld1
    # comprises three signed 32-bit coordinates at CARSTATE offsets 0,4,8.
    return dict(game_frame=struct.unpack_from("<H", data, 320)[0],
                game_inputmode=data[1013],state_sha256=hashlib.sha256(data).hexdigest(),
                travel_distance=struct.unpack_from("<i", data, 316)[0],
                player_position=list(struct.unpack_from("<iii", data, 338)))


def key_word(word):
    # normkey/normkeyi expose AL for ASCII and AH<<8 for extended keys. The
    # host queue retains a BIOS scan byte which the original INT16 shim strips.
    return word & 255 if word & 255 else word & 0xff00


def original_streams(prefix):
    prefix = Path(prefix)
    rows = read_rows(prefix.with_suffix(".jsonl"))
    if rows[0].get("schema")!="stunts-dosbox-ticks-v1":
        raise ValueError("expected DOSBox-X capture")
    if rows[0].get("tick_source") != "original-INT8-entry":
        raise ValueError("reference lacks original interrupt tick identity")
    if rows[0].get("mouse_present") is not True:
        raise ValueError("reference did not enable the mouse used by the port")
    result = {k: [] for k in ("sound", "frames", "states", "keys", "scans")}
    end = rows[-1]
    if end.get("kind") != "end" or end.get("terminal") != "tick-limit":
        raise ValueError("reference is incomplete")
    exclusive = end["raw_tick"]
    with prefix.with_suffix(".frames").open("rb") as frames:
        cache={}
        used=set()
        for row in rows:
            kind, tick = row["kind"], row.get("raw_tick", 0)
            if kind == "frame":
                payload=row.get("payload",row["ordinal"])
                used.add(payload)
                if payload not in cache:
                    frames.seek(payload*FRAME_BYTES)
                    data = frames.read(FRAME_BYTES)
                    if len(data) != FRAME_BYTES:
                        raise ValueError("truncated original frame payload")
                    cache[payload]=(fnv(data[:64000]),fnv(data[64000:]))
                reason = "palette" if row["reason"] == "video_set_palette" else row["reason"]
                if reason in VIDEO_REASONS and tick < exclusive:
                    result["frames"].append(dict(raw_tick=tick, reason=reason,
                        indices=cache[payload][0], palette=cache[payload][1]))
            elif tick >= exclusive:
                continue
            elif kind == "driver":
                entry = row["entry"]
                if entry not in SCALARS:
                    raise ValueError(f"unprojected original driver entry {entry:#x}")
                result["sound"].append(dict(raw_tick=tick, driver=row["driver"], entry=entry,
                    source=row["source"],callback_slot=row["callback_slot"], arguments=[row["stack_u16"][i] for i in SCALARS[entry]],
                    voice=row["voice"],chunk=row["chunk"],resource=row["resource"],payload=row["payload_hex"],
                    result=(row["return_ax"] & 255) if entry==0 else row["return_ax"] if entry in (0x36,0x3c) else None))
            elif kind == "state":
                result["states"].append(dict(raw_tick=tick, **state_projection(bytes.fromhex(row["bytes_hex"]))))
            elif kind == "key":
                result["keys"].append(dict(raw_tick=tick, key_word=key_word(row["bios_word"])))
            elif kind == "scan":
                result["scans"].append(dict(raw_tick=tick, scancode=row["scancode"]))
        frames.seek(0,2)
        if used!=set(range(len(used))) or frames.tell()!=len(used)*FRAME_BYTES:
            raise ValueError("extra original frame payload")
    return result


def port_streams(trace, sound, exclusive):
    result = {k: [] for k in ("sound", "frames", "states", "keys", "scans")}
    for row in read_rows(trace):
        tick, kind = row.get("raw_tick_id", 0), row.get("event_type")
        if tick >= exclusive:
            continue
        if kind == "state_snapshot":
            result["states"].append(dict(raw_tick=tick, **state_projection(bytes.fromhex(row["bytes_hex"]))))
        elif kind == "video_publication" and row["publication_reason"] in VIDEO_REASONS:
            result["frames"].append(dict(raw_tick=tick, reason=row["publication_reason"],
                indices=row["indices_fnv1a64"], palette=row["palette_fnv1a64"]))
        elif kind == "key_consumed":
            result["keys"].append(dict(raw_tick=tick, key_word=key_word(row["bios_word"])))
        elif kind == "input" and row["channel"] == "dos.keyboard.scancodes":
            result["scans"].extend(dict(raw_tick=tick, scancode=s) for s in row["payload"])
    for row in read_rows(sound):
        if row["kind"] == "driver" and row["raw_tick"] < exclusive:
            result["sound"].append(dict(raw_tick=row["raw_tick"], driver=row["driver"], entry=row["entry"],
                callback_slot=row["callback_slot"],
                voice=row["voice"],chunk=row["chunk"],resource=row["resource"],payload=row["payload_hex"],
                source=row["source"], arguments=row["arguments_u16"], result=row["return_u16"]))
    return result


def first_difference(expected, actual):
    for index in range(max(len(expected), len(actual))):
        left = expected[index] if index < len(expected) else None
        right = actual[index] if index < len(actual) else None
        if left != right:
            return dict(index=index, original=left, port=right,
                        original_context=expected[max(0,index-2):index+3],
                        port_context=actual[max(0,index-2):index+3])
    return None


def resynchronised_differences(expected, actual, *, window=24, limit=12):
    """Bounded content alignment for diagnostics only; ticks NEVER decide PASS.

    Preserve tick differences in equal content runs. A missing/different event
    searches at most window events ahead on each side, then continues. This is
    deliberately separate from the strict comparison above.
    """
    def content(row):return {k:v for k,v in row.items() if k!='raw_tick'}
    left=[content(r) for r in expected];right=[content(r) for r in actual]
    i=j=0;differences=[];matches=0;offsets={}
    while i<len(left) and j<len(right):
        if left[i]==right[j]:
            matches+=1
            delta=actual[j]['raw_tick']-expected[i]['raw_tick']
            offsets[str(delta)]=offsets.get(str(delta),0)+1
            i+=1;j+=1;continue
        candidates=[(a+b,a,b) for a in range(min(window,len(left)-i))
                    for b in range(min(window,len(right)-j)) if left[i+a]==right[j+b]]
        if candidates:_,a,b=min(candidates)
        else:a=b=1
        if len(differences)<limit:
            differences.append(dict(original_index=i,port_index=j,
                original=expected[i:i+min(a,3)],port=actual[j:j+min(b,3)],
                original_skipped=a,port_skipped=b,resynchronised=bool(candidates)))
        i+=a;j+=b
    if (i<len(left) or j<len(right)) and len(differences)<limit:
        differences.append(dict(original_index=i,port_index=j,
            original=expected[i:i+3],port=actual[j:j+3],
            original_remaining=len(left)-i,port_remaining=len(right)-j))
    return dict(content_matches=matches,port_minus_original_tick_histogram=offsets,
                differences=differences,diagnostic_only=True)


def compare(original, port):
    return {k: dict(original_count=len(original[k]), port_count=len(port[k]),
                    agrees=bool(original[k]) and original[k]==port[k],
                    first_difference=first_difference(original[k], port[k]),
                    diagnostic=resynchronised_differences(original[k],port[k])) for k in original}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original", type=Path, required=True, help="capture prefix, without suffix")
    p.add_argument("--trace", type=Path, required=True)
    p.add_argument("--sound", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    args=p.parse_args()
    original=original_streams(args.original)
    end=read_rows(args.original.with_suffix(".jsonl"))[-1]
    report=compare(original, port_streams(args.trace,args.sound,end["raw_tick"]))
    args.out.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps({k:v["agrees"] for k,v in report.items()}))
    return 0 if all(v["agrees"] for v in report.values()) else 1


if __name__ == "__main__":
    raise SystemExit(main())
