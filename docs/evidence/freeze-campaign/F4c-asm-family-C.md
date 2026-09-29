# F4c - Phase 4 family C ASM port-fate inventory

## RESULT
Complete inventory is in `inventory_C.json` and `inventory_C.md`.
It covers every manifest game ASM code/data owner, all indexed routines, and all pinned runtime/LIBH code and data rows.
Game classification: C math/fixed-point/projection/geometry/pre-render; A DOS/file/input/timer/audio/memory/CRT-adjacent; B video and software renderer.
No contribution is classified DEAD or NOP from its label alone; `nopsub` entries with active math, interrupt, shape or raster behavior remain categorized by code.

## STATE CHANGE
Scratch-only outputs: `build/workers/F4c/inventory_C.json`, `inventory_C.md`, and this report (raw command output: `tooling.log`). No canonical source, recipe, layout or evidence changed.
`projectiondata9_times_ratio` verify-only result: 14 bytes, fresh HYBRID_EXACT and ordered relocations; no promotion or acceptance-state change.

## STRONGEST EVIDENCE
`layout/manifest.json`: 52 MATCHING_ASM code owners and 19 MATCHING_ASM_DATA owners; each has a row with its complete extent/hash.
`evidence/functions.json` plus recipe-member comparison: 234 indexed game routines; every public recipe member is indexed, including the 38 family C routines.
`docs/porting/platform-boundary.md:191-200,212-213`: renderer contracts, `mat_invert` function-boundary gap, and overlapping `sprite_make_wnd` span.
`asm/font_matrix_shape_decode.ASM:467-523`: active source transposes a 3x3 matrix, with alias-safe in-place swaps and a non-aliasing copy; source has an explicit `endp`.
`asm/patterned_lines_windows.ASM:151-3845`: active `sprite_make_wnd` source creates owned window storage and descriptor/line offsets; source has an explicit `endp`.

## HYPOTHESES TESTED
Routine family assignment used module bodies, procedure-purpose comments, source-semantic caller leads, and `docs/porting/platform-boundary.md`; caller edges are not claimed as binary call proof.
No family C procedure contains a direct INT, IN/OUT, CLI or STI instruction; pre-render functions do call B raster helpers or use far `imagefunc`/`spritefunc` callbacks.
No manifest ASM sorting routine was found; Restunts `heapsort_by_order` is C (`build/references/restunts/src/restunts/c/heapsort.c`).
A whole-owner `search.py` lookup for `font_matrix_shape_decode` failed before compilation because its recipe name is not an indexed function; no single-member candidate was attempted (see `tooling.log`).
Pinned library review covers 53 code owners, 25 data extents and four storage views; eight LIBH helper members cover portable fixed-width signed/unsigned multiply, divide and shift operations.
Restunts `math.c` provides C semantic leads; `stunts_forged` geometry and runtime paths are C++ hooks/cores, not drop-in C replacements.

## NEW KNOWLEDGE
Family C has 38 routines across 14 code owners; six of those owners also contain A or B routines and must remain complete contributions.
Five DATA owners contain C fields; four DATA owners are mixed, with each named label classified separately; 10 are A-only and four B-only.
Pre-render geometry is portable, but line/pixel/sphere helpers cross into the indexed software renderer; translate the callback edge to SDL3 while preserving fixed-width behavior.
The mixed `graphics_resource_runtime:_DATA`, `projection_vector_window:_DATA`, `font_matrix_shape_decode:_DATA`, and `prerender_wheel_raster:_DATA` fields are enumerated with full extents in the inventory.

## TOOLCHAIN
No compiler/profile/assembler hypotheses were varied and no S1-S6 status proposal is made; pinned MSC 5.10/MASM project profiles remain unchanged.
The verify-only helper result does not change toolchain evidence; the current external `_projection_x_scale` binding is reported in the inventory diagnostic.

## REMAINING BLOCKER
`mat_invert` - BLOCKER CLASS 5 (function boundary/structure evidence); freeze class D. The index/docs omit its end although the active ASM source shows an alias-safe transpose body.
`sprite_make_wnd` - BLOCKER CLASS 5 (function boundary/structure evidence); freeze class D. The indexed boundary overlaps an unmapped span although the active ASM source has an endp and allocation behavior.
`vector_op_unk` - BLOCKER CLASS 8 (unknown operation); freeze class E. Preserve the source call boundary until operation behavior is independently named.
`nopsub_326BA` - BLOCKER CLASS 8 (unknown SHAPE3D field roles/declaration); freeze class E. Keep observed offsets and far-record ABI explicit.
Pinned CRT fields with opaque roles are freeze class E; DOS/BIOS/timer/audio/video adapter policy is freeze class B. No class F freeze blocker identified by this port-fate pass.

## RECOMMENDED NEXT ACTION
Port portable math/fixed-point and geometry routines behind the current typed interfaces; translate the software renderer and SDL3 host edges as separate dependencies.
Keep unknown fields/records explicit, and resolve the two function-boundary discrepancies only if a function-level port or independent boundary map needs them.

## NEEDS OPUS?
NO
