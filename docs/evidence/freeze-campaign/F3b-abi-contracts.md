# F3b — declaration-view investigation

## RESULT

The machine contracts are clear for all five targets. The `send_audio_stop_event` spelling with an `I16` return and `return process_audio_event(...)` reproduces the complete accepted `obj_seg028` OMF and passes the fresh strict verify-only gate. For `read_file_with_retry`, `call_read_line`, and `nullsub_2`, the proposed semantic declarations preserve the target code/data bytes but do not preserve the full pinned OMF object identity. `locate_shape_fatal` is an assembly entry into a shared search tail; its different pointer return views have the same far-pointer ABI.

No canonical file was changed.

## STATE CHANGE

- Exact candidate: [send_audio_stop_event.c](D:/Prog/stunts_recon/build/workers/F3b/send_audio_stop_event.c), verified against [obj_seg028.json](D:/Prog/stunts_recon/recipes/obj_seg028.json). `promote.py --cosmetic --verify-only` reported `COSMETIC_VERIFIED_ONLY obj_seg028 8080 OBJ bytes`; strict `promote.py obj_seg028 ... --recipe recipes/obj_seg028.json --verify-only` produced `VERIFIED_ONLY`, 5,272 contribution bytes, fresh 210,384-byte image hash `1adb8259b3f6634062b94826e1f167649d9f2deeb6cedc9989127fc37e9ce615`, 2,588 ordered relocations, and BSS gate `PLACED`. Report: `build/acceptance/obj_seg028/report.json`.
- Read-file candidate: [read_file_with_retry.c](D:/Prog/stunts_recon/build/workers/F3b/read_file_with_retry.c), tested with current [obj_seg008.json](D:/Prog/stunts_recon/recipes/obj_seg008.json). Cosmetic verification stops at `COMDEF names differ from the reviewed communal declarations`. Diagnostic compile shows `_DATA` and all 11,788 `UNIT_TEXT` bytes equal to baseline; OMF is 21,233 vs 21,232 bytes, with communal/external/public/fixup records reordered. The block-scope declaration variant has the same COMDEF rejection.
- Line-editor candidate: [call_read_line.c](D:/Prog/stunts_recon/build/workers/F3b/call_read_line.c), tested with current [obj_seg008.json](D:/Prog/stunts_recon/recipes/obj_seg008.json). Cosmetic verification refuses full OMF identity (21,253 vs 21,232 bytes). After mapping the post-call assignment back to the second stack word, all `UNIT_TEXT` and `_DATA` bytes match baseline.
- No-op candidate: [nullsub_2.c](D:/Prog/stunts_recon/build/workers/F3b/nullsub_2.c), tested with current [obj_seg031.json](D:/Prog/stunts_recon/recipes/obj_seg031.json). Cosmetic verification refuses full OMF identity (4,473 bytes both); all code/data segment bytes, including the member, match. Extracted member candidate [nullsub_2_member.c](D:/Prog/stunts_recon/build/workers/F3b/nullsub_2_member.c) emits the exact target `CB 90` in `search.py`. Its standalone strict verify-only recipe reaches ownership staging, then stops because that interval is already owned by accepted `obj_seg031` (`Candidate is not wholly raw-owned`).

Raw run logs, compiler comparison scripts, and context captures are in `build/workers/F3b/`; see `verify_*.log`, `compare_omf.log`, `compare_omf.py`, and `context_*.json`.

## STRONGEST EVIDENCE

- `read_file_with_retry` is target stable ID `load_19a86`, extent `[105094,105196)` (102 bytes). At entry it reads type from `[BP+6]`; forwarding pushes `[BP+0C]`, `[BP+0A]`, `[BP+08]` to `file_read_nofatal`, so the caller words are type, near-name offset, destination offset, destination segment. It returns the far result in `DX:AX` and uses `RETF`; the sole source caller is `highscore_write_a`. Its target bytes push destination segment/offset, near path, then type 10, far-call the wrapper, and `ADD SP,8`.
- `send_audio_stop_event` is target member `load_29050`, `[168016,168072)` (56 bytes). It calls `process_audio_event`, then only `ADD SP,4; POP DS; POP BP; RETF; NOP`; none writes AX. The caller `audio_op_unk` removes its two pushed words and stores AX at `[SI+12h]`. The accepted `process_audio_event` body returns `voiceNum`, so this is the caller-visible result (selected voice index, or -1 on its two failure exits).
- `call_read_line` is extent `[102588,102714)` (126 bytes). It consumes a near buffer at `[BP+6]` followed by five 16-bit words through `[BP+10h]`. Every recorded source caller uses five C parameters with a 32-bit final argument; that occupies the same six stack words. The context index records five call leads: `enter_hiscore`, `security_check`, `do_fileselect_dialog`, and two `do_savefile_dialog` sites. The extracted target CODE for the candidate is byte-identical after preserving the write to `[BP+8]`.
- `nullsub_2` is extent `[171604,171606)` with bytes `CB 90`: far return plus alignment. It reads no arguments, writes no return value, and has no outgoing calls. The extracted candidate is a two-byte exact match.
- `locate_shape_fatal` is extent `[135069,135081)` with bytes `55 8B EC 1E 56 57 BA 01 00 EB 0A 90`. It sets `DX=1` and jumps into the shared locate tail at `0x20FB2`; adjacent entries set other selectors. The shared successful path returns the computed far address in `DX:AX`; selector 1 routes a miss to `fatal_error`. Caller declarations vary among `char far *`, `unsigned char far *`, `struct SHAPE2D far *`, and `void far *`, all the same four-byte far-pointer ABI. This shared-entry structure is positive ASM evidence, not inference from a failed C match.
- Context captures: `context_read_file_with_retry.json`, `context_highscore_write_a.json`, `context_send_audio_stop_event.json`, `context_audio_op_unk.json`, `context_call_read_line.json`, `context_nullsub_2.json`, `context_locate_shape_fatal.json`, and `context_locate_sound_fatal.json`. The independent source/extent anchors are in `evidence/functions.json` and the listed owning recipes.

## HYPOTHESES TESTED

| Target | Machine contract | Source spelling / strict object result | Safe port contract |
|---|---|---|---|
| `read_file_with_retry` | Four stack words; selector at +6, near name at +8, far destination at +A/+C; caller cleans 8 bytes; far result in DX:AX. | `(I16 type, I8 *name, void far *destination)` plus a matching pointer declaration for `file_read_nofatal` leaves target code/data identical, but full OMF identity fails on COMDEF ordering. | Selector plus near filename and segmented far destination; return an opaque segmented far result. Map it to host storage through an explicit port adapter; do not cast a DOS far pointer to a host pointer. |
| `send_audio_stop_event` | Two word arguments; AX returns `process_audio_event`'s voice number; caller stores AX. | `I16 FAR _loadds ... (U16 rate, I16 handle) { ... return process_audio_event(...); }` is full-OBJ identical and passes strict verify-only. | Signed 16-bit voice/channel result; preserve `-1` failure behavior and the 16-bit rate/handle inputs. |
| `call_read_line` | Near buffer plus five words; the final two words come from the caller's 32-bit final argument. | Five-parameter spelling with an `I32` final argument can preserve every CODE/DATA byte, but its full OBJ identity differs. It calls same-TU near helpers, so the correct test unit is all of `obj_seg008`. | Preserve the five-argument compatibility view or explicitly split the final 32-bit value into its low/high 16-bit words; those words feed the final two `read_line` arguments. Keep their meanings opaque until independently resolved. |
| `nullsub_2` | Far return only; no argument reads, no meaningful AX result. | `void far nullsub_2(void far *resource, I16 type) {}` emits exact member bytes; full `obj_seg031` OMF identity differs. Standalone strict staging is blocked by the existing whole-object owner. | No-op with the caller's far-resource and 16-bit type arguments; ignore both and return void. |
| `locate_shape_fatal` | Far resource pointer plus near name; far pointer result in DX:AX; missing shape is fatal. | No C spelling candidate: the target entry branches into the shared assembly search tail. The `char*`/shape-pointer/`void*` views do not alter the machine ABI. | Treat the return as an opaque resource-relative segmented address and preserve fatal-on-miss behavior. Resolve it through the port's resource adapter, never as a host pointer cast. |

The complete compile diagnostics and OMF segment/record comparison are in `diagnose_objects.py` and `compare_omf.log`. `search.py`'s standalone no-op result is `build/search/a115808e-bd6a-4338-957f-e6770d6285e8/report.json` (diagnostic authority only).

## NEW KNOWLEDGE

- `read_file_with_retry`'s four physical words are independently anchored by both callee BP offsets and its only caller's reverse-order pushes plus `ADD SP,8`.
- The candidate pointer spelling preserves every code/data byte, but changes COMDEF ordering; source spelling is not enough to claim full OMF identity.
- `call_read_line`'s five-parameter `I32` caller view is stack-compatible with the recovered six-word body; its second physical word is also the local result destination after `strlen`.
- `send_audio_stop_event`'s AX value is live through the far epilogue and is explicitly consumed by the C6 caller. The explicit `int` return is a justified, exact source spelling.
- `nullsub_2` can declare the caller's unused parameters while remaining a two-byte far-return member; the existing accepted object prevents standalone republishing.
- `locate_shape_fatal` is a selector stub into a shared ASM tail; its return pointer is a segmented resource address and missing entries take the fatal path.

## TOOLCHAIN

No proposed status changes. All tests used the canonical MSC 5.10 medium profile. `CC-MSC510-AM-O-Gs` remains CANONICAL; `FACT-tu-extern-count-limits-cse` remains SUPPORTED. No falsified/non-discriminating entry was retested, and no profile/flag variation was run.

## REMAINING BLOCKER

- `read_file_with_retry` — **BLOCKER CLASS 2** (declaration/type/TU-context); freeze class **C**. ABI and behavior are proven. The pointer signature preserves code/data, but COMDEF order prevents a full-OBJ identity claim.
- `send_audio_stop_event` — **BLOCKER CLASS: none; resolved**. Freeze class: none. Candidate source spelling is byte-identical and strict verified.
- `call_read_line` — **BLOCKER CLASS 2**; freeze class **C**. ABI is proven; candidate source spelling preserves code/data but not complete OMF identity.
- `nullsub_2` — **BLOCKER CLASS 2**; freeze class **C**. No-op behavior and caller stack view are proven; full owner OMF identity differs, and its bytes already belong to `obj_seg031`.
- `locate_shape_fatal` — **BLOCKER CLASS 2**; freeze class **C**. Return-view discrepancy is declaration-only after machine analysis; shared ASM behavior and fatal miss path are known.

None is a freeze blocker (F). The residuals concern source declaration spelling or port contract, not unknown target bytes.

## RECOMMENDED NEXT ACTION

Use the exact `send_audio_stop_event.c` candidate if the supervisor wants to align the historical definition with the already observed caller result. Keep the other accepted sources unchanged; carry their proven ABI contracts into port headers. Keep `locate_shape_fatal` as an opaque segmented-resource API with the fatal miss policy.

## NEEDS OPUS?

NO.
