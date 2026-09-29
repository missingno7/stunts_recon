# Worker report — F4b

## RESULT
Completed family-B ASM port-fate inventory for all 35 selected source modules / manifest owners, with 155 public code entries, 42 public data exports, and 12 associated `_DATA` manifest contributions. All 52 manifest ASM owners are classified: 17 are explicitly assigned to other families. Per-routine category, known semantics, platform dependency, replacement, indexed caller evidence, first-link requirement, confidence, and source references are recorded.

## STATE CHANGE
- New scratch artifacts only: `build/workers/F4b/inventory_B.json`, `build/workers/F4b/inventory_B.md`, and generator `build/workers/F4b/build_inventory.py`.
- No canonical source, recipe, manifest, oracle, evidence, tool, or acceptance state changed. No strict historical matches were attempted.
- `promote.py --verify-only`: not applicable; this mission produced no matching C/ASM candidate or recipe to verify. `search.py --history --limit 3` returned no output.

## STRONGEST EVIDENCE
- `layout/manifest.json`: 52 `MATCHING_ASM` owners, authoritative extents, and 12 family-B `_DATA` children. Recipe basenames map address-named manifest owners to source modules; inventory records both IDs.
- `docs/porting/platform-boundary.md:16-17,137,154,191,195`: indexed-surface contract; exact 64,000-word A000h clear behavior; BIOS DAC call; filled-span semantics; window creation and incomplete end boundary.
- `asm/video_set_mode_13h.ASM`: BIOS mode 13h, BDA updates, cleanup registration. `asm/sprite_shape_video_ops.ASM:1031`: BIOS DAC block service. `asm/vector_sphere_sprite_ops.ASM:397-411`: `video_clear_color` entry.
- `asm/patterned_lines_windows.ASM:242-245,3845-3879`: mutable CS window arena and setup entries. `asm/sprite_rectangle_scaled_blitters.ASM:244-245,509-558`: CODE identity table and runtime initialization. `asm/line_sprite_shape_render.ASM` declares two CS sprite descriptors plus the 200-word line-offset table.
- `python tools/context.py video_set_palette --asm --callers --symbols` returned the 25-byte service body and semantic caller `load_palandcursor`. Caller paths in `evidence/functions.json` are explicitly marked source-semantic-only.
- In read-only `stunts_forged`, no equivalent standalone C implementation of the full family-B surface was verified. `src/stunts_render.hpp` has a PortForge guest-machine RLE hook; `src/port/render/` has typed C++ surface, raster, font, window, projection, RLE, and prerender analogues. They are semantic leads, not verified one-to-one SDL3 replacements.

## HYPOTHESES TESTED
- Separated direct platform services (BIOS/ports/segments/interrupts) from software renderer and portable integer/projection algorithms using each body, its Purpose/platform comments, and the platform-boundary index.
- Checked `nopsub_*` labels against bodies: names alone do not imply NOP; entries include projection math, input handling, retrace polling, and renderer setup/tails.
- Checked mixed owners for non-renderer routines; assigned file/resource, timer, input, and diagnostic support owners outside family B where their manifest recipe identifies another family, while retaining every public export of selected mixed modules.
- Did not vary compiler profile or flags or test source-shape hypotheses.

## NEW KNOWLEDGE
- Completeness is explicit: 35 family-B source modules / manifest owners; 17 other-family owners; 197 public exports = 155 code entries + 42 data exports; 12 associated `_DATA` contributions.
- 81/155 public code entries have indexed direct or transitive C caller paths; 4 more are callback/vector candidates; the remaining 70 have no indexed C path and are not classified dead.
- `patterned_lines_windows` has a 3,600-byte mutable CODE descriptor/row-table arena; `sprite_rectangle_scaled_blitters` initializes its 256-byte identity table in CODE at runtime. Both require ordinary writable host storage.
- `video_clear_color` performs 64,000 STOSW stores through A000h: 128,000 byte writes with 16-bit DI wrap. Preserve this behavior until visible-fill intent is resolved.
- `textmode_debug_console` targets B000h monochrome text memory and is separate from the indexed game renderer.
- Port replacements group into SDL3 display/palette/presentation, host filesystem/runtime policy, or portable C translation of the same indexed raster/projection/prerender algorithm. Shape/resource decoding and allocator ownership remain adjacent families.

## TOOLCHAIN
None proposed. No compiler/assembler experiment was conducted and no status change to `evidence/toolchain-hypotheses.json` or symptoms S1-S6 is supported by this inventory. Canonical MSC 5.10 profile remains untouched.

## REMAINING BLOCKER
- Freeze classification: **B** host runtime/SDL3 policy still needs implementation; **C** clear-color visible-fill intent is uncertain although machine behavior is known; **D** indexed callers are source-semantic and some entry boundaries/indirect use need stronger evidence; **E** some address/nopsub and `sprite_1_unk` semantics remain unknown. No **F** historical freeze blocker was found here.
- No unresolved C reconstruction target was investigated; matching blocker classes 1-8 do not apply to this port-fate assignment. Do not infer object sizes from accesses or treat host pointers as segmented pointers.

## RECOMMENDED NEXT ACTION
Use `inventory_B.json` to schedule replacements behind explicit indexed-surface, palette, host display-state, writable-window-buffer, and timing/input interfaces. Confirm source-semantic first-link paths against binding/relocation evidence before removing providers. Keep SDL3 adapters outside the historical oracle.

## NEEDS OPUS?
NO.
