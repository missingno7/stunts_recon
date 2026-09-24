# Mixed far CALL and DGROUP offset16 binding, 2026-09-24

Scope: a complete single-public MSC OMF `UNIT_TEXT` contribution with exactly one
zero-addend external target-frame `pointer32` operand of opcode `9A` (far CALL)
and at least one external target-frame `offset16` into a reviewed DGROUP symbol.
The binder retains every SEGDEF, GRPDEF, PUBDEF, EXTDEF, FIXUPP, emitted byte,
and original MZ relocation entry as an obligation. It rejects additional
initialized/BSS contributions, nonzero target displacement, unsupported frames,
overlap, missing or unused symbols, and nonzero far addends. No source recipe or
production ownership changed in this experiment.

The discriminating fixture compiles an unchanged source object whose complete
FIXUPP order is `pointer32 @ 26`, `offset16 @ 10` (encoded addend 2), and
`offset16 @ 5` (encoded addend 1), even though those operand addresses have a
different order. It links that same object with the pinned historical
`__aFlmul` library member and a synthetic `_flags` DGROUP provider. Historical
LINK `/NOD /MAP` supplies the addresses independently of the binder. In two
link orders and both pinned MSC 5.00 and 5.10 profiles, the entire 32-byte
linked `UNIT_TEXT` equals the binder output. LINK emits one MZ relocation at
the far CALL segment word and none for the two data offsets. The UNIT-first
fixture places it at load offset 28; library-first places it at load offset 80.
The unchanged compiler object is SHA-256
`b871f0db93399cdf4f38c425f3e69256973a04828e9ca55a630bb12890232ab8`
(300 bytes). Full map, object identities, linked bytes, fixups, relocation
coordinates, and commands are recorded in ignored
`build/mixed-linker-probe.json`; the reproducible differential lives in
`tools/linker_probe.py:mixed_far_data_experiment` and dedicated tests in
`tests/test_mixed_binding.py`.

`python -m unittest discover -s tests -p '*binding.py' -v` passed 26 tests,
including controls for omitted/reordered obligations, wrong frame/target,
addend, displacement, overlapping fixups, extra public/BSS, symbol ownership,
and MZ relocation coordinates. `tools/binder.py` dispatches the bounded mode
`external-far-call-dgroup-offset16-v1` only when a recipe requests it.

This proves linker arithmetic for the bounded mixed object shape, not original
Stunts TU boundaries or data ownership. A production candidate still needs
independently reviewed code and data symbols, exact original fixup declarations
and ordered relocations, then fresh strict whole-image acceptance.

## Original address proof and strict recovery

After the fixture, the pristine image independently established `_sdgame2ptr` at DGROUP `0xAC68` (four BSS bytes) through stores/reads inside verified `load_sdgame2_shapes`, and `_textresprefix` at `0xACEE` through the verified `init_main` setter. The candidate target functions supply corroborating reads, but neither address was chosen from its desired output operand. `layout/data-symbols.json` and `tools/data_symbols.py` recheck instruction bytes, no operand relocation, startup DGROUP frame, BSS bounds and oracle hash. `docs/current/data-binding.md` has the locations and limits. Original PUBDEF names/TU ownership are still unknown.

`tools/code_symbols.py` now partitions mixed recipe fixups by kind and resolves each target through the corresponding reviewed code/data registry. `tools/prepare_far_call_candidate.py --binding external-far-call-dgroup-offset16-v1` checks the fresh complete object, these aliases, bound pristine bytes, the exact original MZ order and a bounded direct CFG before writing a reviewed recipe and overlay. Preparation alone is not acceptance.

Two functions then passed fresh FAST and whole-image promotion through `grind.py`:

| Function | Complete compiler contribution | Strict receipts |
|---|---|---|
| `free_sdgame2` | 18 bytes; offset16 `_sdgame2ptr` at object +6 and +2; pointer32 CALL `_mmgr_free` at +9 | `recovery/attempts/free_sdgame2/{0001,0002}/report.json` |
| `locate_text_res` | 52 bytes; pointer32 CALL `_locate_shape_fatal` at +43; offset16 `_textresprefix` at +7 | `recovery/attempts/locate_text_res/{0001,0002}/report.json` |

The second bounded three-task wrapper batch used six private compiler processes. Its exact-size `set_frame_callback` candidate has five ordered fixups, including a callback code-pointer offset/base pair and two currently unreviewed data addresses. That is outside this mode. `flush_stdin` and `sub_2EB07` still differ in source codegen/extent. Their full objects and hypotheses are under `build/private/research-batches/`.

Final full `tools/validate.py` passed 157 tests, independently compiled and bound all 25 active game-C functions, and retained `HYBRID_EXACT`. Reclassification reports `CHEAP=0`, `MEDIUM=5`, `SUPERVISOR=585`. Strict game C is 1,090 bytes, game ASM zero, pinned runtime 725, raw initialized 198,185, and unknown classification 45,708. The candidate-object census now covers 82 archived files / 64 distinct task-object payloads across 28 remaining supervisor tasks, with zero parser errors. The reviewed code-address registry has 136 addresses; 76 remaining supervisor tasks have all observed far-call target addresses reviewed. These coverage figures are research eligibility evidence, not additional recovered ownership.
