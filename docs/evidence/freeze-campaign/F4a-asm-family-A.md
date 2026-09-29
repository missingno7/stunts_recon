# F4a — Phase 4 ASM port-fate inventory, family A

## RESULT
Completed the family A inventory from manifest-owned ASM. It covers 24 code owners, all 143 public procedure/label entries in their sources, and all 12 associated initialized ASM data owners (4,863 bytes total). Per-routine and per-data rows record category, behavior, device/OS dependency, port replacement, current manifest-owned C references, first-link need, confidence, and evidence. See `inventory_A.json` and `inventory_A.md`.

## STATE CHANGE
Wrote only `build/workers/F4a/inventory_A.json`, `build/workers/F4a/inventory_A.md`, `build/workers/F4a/REPORT.md`, and the scratch generator `build/workers/F4a/make_inventory.py`. No canonical source, recipe, manifest, evidence, or oracle files changed. No strict candidate was prepared, so historical `search.py` and `promote.py --verify-only` were not applicable to this port-fate task; no full validation was run.

## STRONGEST EVIDENCE
- `layout/manifest.json`: 52 ASM code owners total; 24 assigned to A, 28 listed as other families; 12 `_DATA` children assigned to A, 7 outside A. The inventoried extents come from these owners.
- Public-entry semantics and dependencies: the owner source files in `asm/`, including shared-entry timer/decompression routines and the public `restore_div_zero_vector` label; current C symbol references were scanned only in manifest-owned `MATCHING_C` source inputs, excluding extern declarations and comments.
- Boundary contracts: `docs/porting/platform-boundary.md`, `docs/porting/formats.md`, `docs/porting/runtime-model.md`, `docs/porting/runtime-measurements.md`, `docs/porting/audio-drivers.md`, and `docs/porting/port-headers.md`.
- Read-only independent leads in `D:\Games\DOS\dos_recosystem\stunts_forged\src`: `stunts_render.hpp`, `port\render\rle_sprite.hpp`, `port\render\rle_and_sprite.hpp`, `port\render\rle_or_sprite.hpp`, `port\assets\pes.hpp`, `port\assets\pes_legacy_plane_reorder.hpp`, `port\render\font.hpp`, `port\render\surface8.hpp`, and `platform\win32_player.cpp`.

## HYPOTHESES TESTED
- Owner boundary check: every active ASM source whose purpose includes `mmgr_*`, `file_*`/`locate_*`/decompression, keyboard/joystick/timer/vector, video lifecycle, `obj_seg002`, game cleanup, or active audio glue is represented; ambiguous shape-table pointer helper `load_226ba` is included with resource lookup.
- No manifest-owned mouse ASM source exists. The mouse service is outside this ASM family inventory and remains a C/host boundary.
- Active manifest audio-related ASM is `audio_stop_unknown` inside `timer_video_interrupt_runtime`. Existing `audio_add_driver_timer.ASM`, `audio_remove_driver_timer.ASM`, and `audio_function2.ASM` recipe/source files are not active manifest owners and are listed as out of scope.
- `stunts_forged` already contains useful renderer and PES algorithm analogues, but I found no complete host replacement for DOS filesystem, memory-segment, interrupt, keyboard ISR, joystick port, or PIT timer paths. Its DOS VM services are compatibility leads, not SDL3 replacements.
- No compiler/profile hypothesis was tested or changed. `evidence/toolchain-hypotheses.json` was read before any toolchain reasoning; this task required none.

## NEW KNOWLEDGE
- Family A consists of 24 code owners / 143 public entries plus 12 exact initialized data extents. The other 28 code owners and 7 data owners are enumerated in the inventory as renderer, geometry, font, sprite, or misc families.
- The selected `_DATA` owners include IRQ/vector snapshots, timer callbacks/counters, keyboard ring/key maps, joystick calibration data, resource-manager tables, and debug/file-error state. Their complete manifest extents are retained; data rows also give exported source labels and current C symbol-reference evidence.
- `nopsub_326BA` is a shape-record pointer-table lookup (offset `6 + selector*4`), not a no-op. `nopsub_30180` is an alternate timer setup entry. `sub_303BA` chains the previous timer IRQ. `nopsub_kb_get_readchar_callback` returns the stored far callback address in DX:AX.
- The game-side `file_decomp_rle` is a segmented stream decoder with optional sequential pass and `0x8000` window-wrap behavior. `stunts_forged` RLE sprite compositors are useful leads but do not establish equivalence to this resource decompressor.
- Direct C-reference status is based on current manifest-owned C source expressions. A `false` value is not proof against ASM calls, callbacks, vectors, or indirect entry; each row says this explicitly.

## TOOLCHAIN
No proposal. The pinned historical compiler/assembler profile is untouched; no compiler output was generated.

## REMAINING BLOCKER
- **E — semantic unknown:** `obj_seg002` has code that constructs calls to undocumented INT 60h, 61h, and 62h. No active C source references were found, but the complete ASM/callback reachability/provider chain is not established.
- **B — port-runtime policy:** timer cadence must preserve the game callback/deadline behavior; the platform docs leave effective PIT rate unresolved, so the SDL3 clock policy still needs an independently measured/selected host rule.
- **B — port-runtime policy:** implement filesystem wildcard/path, segmented-buffer, DOS paragraph-size, fatal/nonfatal, and partial-write cleanup adapters; these are grouped in the inventory with their C first-link references.
- No family A finding identifies an F-class historical-freeze blocker. This report does not establish whole-project freeze readiness.

## RECOMMENDED NEXT ACTION
Use the rows marked `translation_required_before_first_link=true` as the SDL3 adapter/export checklist. Resolve INT 60h/61h/62h reachability, then measure original timer callback cadence before selecting host clock behavior. Keep file/resource views as bounded host spans and map A000/B000/VGA state to explicit SDL3 surface or debug-output contracts.

## NEEDS OPUS?
NO.
