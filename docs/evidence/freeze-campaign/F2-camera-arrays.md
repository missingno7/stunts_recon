# RESULT

The target-backed port extents are **9/9/9/9 words (18 bytes each)**. The accepted `obj_seg005` source arrays, their exact `_DATA` payload, their address spacing, and every use through index 8 agree. The 8/8/7/7 claim is a `typeinfer` symbol-gap artifact, not an allocation measurement or maximum observed-access result.

# STATE CHANGE

No canonical source, recipe, layout, evidence, docs, or tests were changed. Rebuilt the accepted contribution using `python tools/promote.py obj_seg005 src/obj_seg005.c --recipe recipes/obj_seg005.json --verify-only`; exit 0: `VERIFIED_ONLY obj_seg005 12778 bytes; fresh HYBRID_EXACT and ordered relocations`. Scratch scan and proposals are in this worker directory. The standalone `search.py` attempt could not allocate the TU's COMDEFs without its recipe; the strict recipe-based verification is the acceptance evidence.

# STRONGEST EVIDENCE

- `src/obj_seg005.c:1915-1918` declares four initialized `I16[9]` arrays. `layout/manifest.json` assigns this accepted owner the complete 720-byte `_DATA` interval `[190344,191064)`, locked SHA-256 `ae58b7e032e63d4c4764c57d9c4d1671f4c4f73f6cdb8f8bae65fac9c4cad523`. A fresh strict verify-only build passes.
- Target load addresses are `0x2EA08`, `0x2EA1A`, `0x2EA2C`, `0x2EA3E`; corresponding DGROUP offsets are `0x3298`, `0x32AA`, `0x32BC`, `0x32CE`. Every stride is `0x12` (18 bytes), all starts are even, and `gameunk_button_x1` follows y2 at load address `0x2EA50`. There is no padding between the four spans.
- Oracle bytes at those starts decode to the exact nine source initializer values for each array. `build/workers/F2/extent_audit.json` records values, raw bytes, the locked interval hash, and per-array comparisons (`initializer_matches_target=true` for all four).
- The next labels used by typeinfer are inside the spans: `_word_3EA18` and `_word_3EA2A` at +16; `_word_3EA3A` and `_word_3EA4C` at +14; `_word_3EA3C` and `_word_3EA4E` at +16. Restunts' read-only `build/references/restunts/src/restunts/asmorig/dseg.asm:14180-14219` shows the nine `dw` values, with these interior words separately labeled.
- Accesses independently require the ninth entry. `src/obj_seg005.c:2076-2103` indexes the arrays by `camera_button_index`, reads entries 7 and 8 directly, and passes `game_camera_buttons_count[cammd] + 1` to `mouse_multi_hittest`. The count initializer includes 8. `src/mouse_multi_hittest.c:5-17` loops `index < count` and reads all four pointers, so mode 2 reads indices 0 through 8.

# HYPOTHESES TESTED

- **8/8/7/7 is maximum observed access:** rejected. `build/workers/X2/typeinfer/decls.json` reports extents 16/16/14/14 with `extent_source="gap"`; its generated declarations are `[8]/[8]/[7]/[7]`. In `tools/typeinfer.py:114-121`, a missing explicit width becomes the distance to the next registered label (or a default 2 bytes for the last symbol). `lookup()` uses that span, and `infer()` divides it by element width to produce the array count (`:766-795`). The gap stops at interior aliases; it is neither access high-water nor object-boundary evidence.
- **Nine elements from source alone:** checked against target rather than accepted on source naming. All four source initializers match the bytes at the corresponding target addresses, inside the accepted, freshly verified `_DATA` contribution. Accesses also consume all nine entries.
- Read-only `stunts_forged/src` scan found no matching camera-button names, initializer sequence, or `mouse_multi_hittest` declaration; it contributes no direct clue. The local Restunts data listing does show all nine values.
- Bounded extent audit compares registered target labels with source-backed sizes in the current port-model snapshot. Scratch prototype tests: 3/3 pass. Equivalent proposed central-header declaration checks pass against the scratch 9-word header. A whole-header GCC size probe was unsuitable because the port header depends on absent `platform_hw.h` and `FAR`/`_loadds` host definitions; the proposed regression is a direct header declaration assertion instead.

# NEW KNOWLEDGE

- Other next-label-gap undercounts found: `audio_driver_volume_command` 4 bytes vs gap 3; `camera_buttons_pressed` 9 vs 2; `audio_event_send_buffer` 12 vs 2; and `scrorder_idxs` 14 vs 2. Separate sizing discrepancies use other mechanisms: `g_mouseyposstacktable` 10 vs typeinfer's terminal fallback 2, and `resbuftext` 80 vs an explicit alias width 4. Evidence and next-label identities are in `extent_audit.json`.
- `line_input_screen_rect` is a 4-byte far-pointer view while typeinfer sees a 2-byte gap before `fontdefseg`; its Restunts representation is two adjacent public words, so treat it as a possible overlapping view rather than a proven single 4-byte C object.
- The generated 547-row `build/porting/state-model.json` omits these four camera arrays: `layout/names-registry.json` has no entries for them and `state_model.py` iterates that registry. The corrected header/docs should either add source-owned rows to the model generator or explicitly carry these accepted objects in a supplemental port-state inventory.

# TOOLCHAIN

No hypothesis/status change. The accepted recipe stays on canonical `msc510-medium` (`/AM /O /Gs`); no compiler, profile, or assembler variation was run. No S1-S6 symptom is implicated.

# REMAINING BLOCKER

No blocker remains for the four port extents or preserved bytes. Freeze classification: the 8/8/7/7 claim is **D (false positive/evidence-tool limitation)**; `typeinfer` must separate next-label gaps and observed accesses from allocated extents. The original authored-C grouping of the nine words remains unknown (blocker class 8 for source-shape history only), but does not change the proven 18-byte spans or 0..8 port contract (**E**, non-blocking). No **F** freeze blocker found.

# RECOMMENDED NEXT ACTION

Integrate `build/workers/F2/proposals/port-model.patch` for central declarations, docs, and the focused header regression. Apply the boundary/access split described in `build/workers/F2/proposals/TOOL_FIX.md`, using `extent_evidence.py` and its tests as a small prototype. Add four accepted-source-backed entries to the state model (or an explicit supplemental inventory) so the documented global model does not omit them.

# NEEDS OPUS?

NO.
