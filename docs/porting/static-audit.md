# Static audit of the port boundary

Status: **partial**. Every baseline row was screened for all six classes, with
the 438 never-executed rows first. This is not a completed semantic review of
every replacement. Two new port defects were confirmed against original machine
code and fixed; inventory defects were also corrected. Four concrete candidates
remain unconfirmed and were not changed.

The baseline has 1,423 overlapping rows. Correcting original-entry joins produces
1,422: the separate native-helper rows for `int86` and `mat_invert` are now
represented by their ASM replacement rows, and `sprite_set_1_from_argptr` gains
its own helper row. No implementation was removed. The existing GCC profiles,
rejoined to these corrected mappings, mark 435 rows never executed. Those
profiles predate this audit; this audit did not collect new execution coverage.

The screen accounts for **1,423 baseline / 1,422 current rows per class**. It
joins 550 rows to original instruction/data intervals; 872 current rows lack an
isolated original mapping. There are 430 distinct code intervals, 38 with an
undecoded linear tail, and 499 groups of data-symbol names sharing an address.
All remaining obligations stay visible, including those on mapped rows. Counts
of screened rows must not be reported as counts of verified replacements.

## Rerunning and evidence

From the repository root:

```powershell
python tools/porting/semantic_audit.py --six-classes --work build/workers/portaudit/audit
python -m unittest tests.test_sdl3_static_audit -v
```

The first command runs the existing production-overlay/compiler/span audit and
writes `report.json`, `compile-report.json`, and `six-classes.json` under its
work directory. `--port-layer PATH` selects another inventory, including the
saved baseline. `--inventory-only` omits compiler verification and retains that
limit. The ordinary semantic audit's `PASS` is separate from the six-class
report's `SCREENED; semantic review incomplete` status.

Each row retains its ID, review ordinal, priority phase, coverage, portable source
and body hashes, original interval references, lexical risks, and six explicit
obligations. Each original interval retains its complete byte hash, instruction
addresses/bytes/operand widths and any undecoded tail. The data table is retained
as data, never disassembled as code. Report inputs are sealed by hashes.

Original bytes come through `diffharness.OracleImage.load()`, which reconstructs
and checks the immutable oracle lock in memory. Coordinates below are image load
offsets. Public-entry gaps are resolved from accepted owner placement plus exact
OMF public offsets in the pinned runtime declarations or accepted ASM recipe.
The next public bounds a diagnostic interval; it is not asserted to be a function
or CFG boundary. For example, `fontmat` public `_mat_invert` at member offset 576
locates load `0x22a72`; its interval also contains a trailing jump table.

Existing `semantic_mapping`, `semantic_detectors`, `semantic_bounds`, layout
contracts and `platform-boundary.json`/`asm-migration.json` are reused. No second
binary oracle or acceptance mechanism was introduced. A missing address cannot
join to another missing address. The coverage annotation also no longer infers
original unreachability from the mixed `HISTORICAL_CRT-DEAD-NOP` category.

The original-machine service runner now accepts explicit interrupt handlers as
test inputs. Unlisted interrupts still trap. The save tests supply DOS result
registers and carry and execute the unchanged original instructions through RETF;
they do not claim to emulate an entire DOS filesystem. Native probes compile the
production routine bodies with supplied host-service results. Transpose probes
compare all 128 bytes of the surrounding workspace, not just the result matrix.

## Methods by class

1. **Width/signedness.** The common command records original operand sizes,
   CMP/TEST, signed/unsigned branches, CBW/CWD, byte clearing, word arithmetic,
   MUL/IMUL, DIV/IDIV and shifts/carries. Portable signatures, source hashes and
   candidate casts/promotions accompany each row. Review must connect these
   operands to caller-consumed results and host expression types; a linear scan
   cannot establish this data flow. The new finite cases execute the original
   `convertjoy` zero-extension/masking and MSC `rand` wraparound. Existing
   `test_sdl3_result_track_bytes`, `test_sdl3_word_arithmetic`, renderer/preview
   arithmetic and sentinel tests remain required.
2. **Aliasing/layout.** The screen records every decoded memory operand, width,
   segment/base/index/displacement and access bits. It joins possible DGROUP
   accesses to nearby names/extents and lists same-address symbol groups. DS
   provenance and index ranges are explicitly hypotheses; unsized neighbours do
   not acquire invented extents. Stack operands are retained for local aliases.
   Existing `scan_spans`/`semantic_bounds` and fresh-overlay contracts check wheel
   spans and the polygon sentinel. The new transpose test covers all word offsets
   -9 through +9, including identical, disjoint and partially overlapping buffers.
   Existing high-score, wheel, track-info and polygon-storage regressions remain.
3. **Host-service results.** All 113 boundary rows are included. Original
   INT/IN/OUT/IRET sites retain an eight-instruction successor window for initial
   consumer review. This is not transitive register/flag/buffer liveness, and
   indirect sound-driver calls require their existing typed driver contracts.
   The save probe supplies successful create/write/close, failed close, short
   write, short write plus failed close, and create errors 2/5. Existing original
   mouse/INT15, file/scenario and AD15/PC15 observations are retained at their
   stated finite scopes. Other error-code translations remain open.
4. **Initial state.** Service sites and possible DGROUP accesses expose startup
   review points; they do not prove read-before-write. Compare original driver
   initialization and first consumers with `port/main.c`, `input.c`, `video.c`,
   `memory.c`, `startup.c` and clock initialization. Mouse centering and one-word
   keyboard overwrite have existing original-backed regressions. General palette
   and video-memory initialization, BIOS timer phase, PSP/environment, inherited
   handles and current-directory equivalence are not closed by this audit.
5. **MSC runtime.** Every one of the 92 runtime public rows gets an accepted
   entry interval where its OMF declaration allows it, rather than the unrelated
   sprite mapping. The report identifies pinned-member intervals separately from
   their host semantic mappings. New cases compare 12 consecutive `rand` calls
   from each of three 32-bit states with live original state/return bytes.
   Existing ctype tests compare all 257 locked bytes. This does not close every
   string overlap, formatting argument, file-mode/text translation, seek failure
   or allocation-failure path. Host libc similarity is not a certificate.
6. **Added glue.** Native calls and Python rewrite expressions are retained next
   to original CALL sites and source hashes. Review main/startup, tick grant,
   nested dispatch, input application, rendering/publication and teardown order.
   Macro/rewrite rows remain obligations even without standalone guest entries.
   Existing first-race-frame and one-row-polygon regressions are preserved.
   Result-music/ISR tails and replay audio transitions remain open exactly as
   recorded in [step-divergences.md](step-divergences.md).

## Findings

“Not a bug” below is limited to the named original/native cases. Existing ten
scenario-discovered fixes are regression anchors above, not counted as new bugs.

| Finding / row | Class | Original behaviour and evidence | Port behaviour | Verdict | Test / remaining evidence |
|---|---|---|---|---|---|
| F01: runtime/unknown-address inventory joins; `asm:library_rand_124476::_rand` and other missing-address rows | Added glue (audit tooling) | Pinned `_rand` is load `0x1e64e`, from member start 124476 + public offset 18; it is not a sprite routine. CRT category alone proves no reachability fact. | Null keys incorrectly joined to `sprite_set_1_from_argptr`; coverage notes also asserted no original caller without proof. Both corrected, with accepted public-entry coordinates and explicit unknowns. | **bug fixed** (tooling) | `test_missing_addresses_never_join_to_an_unrelated_native_function`; `test_public_interval_uses_pinned_declarations_and_rejects_unknowns` |
| F02: `asm:filewrit::_savefilez` / `port/file.c::savefilez` | Host-service results | At `0x225a3` INT21 AH=3Eh is immediately followed by XOR AX,AX at `0x225a5`, irrespective of carry. After successful writes a close error does not unlink or report failure. | Previously returned 2 and deleted the completed file on a failed `fclose`. Now completes buffered writes with `fflush` in the write phase, then ignores close status. Write failures still unlink/report. | **bug fixed** | `test_save_close_status_matches_original_including_failure`: six supplied service-result cases; pre-fix failure in scratch `save-before.log` |
| F03: `asm:fontmat::_mat_invert` / `port/matrix.c::mat_invert` | Aliasing/layout | `0x22aa3..0x22ada` writes output indices 0,3,6,1,4,7,2,5,8 and rereads source after each store. Accepted public offset locates the entry independently of the old incomplete function inventory. | Previously wrote 0,1,2,3,4,5,6,7,8: six partial-overlap cases differed. Now preserves the original store order. | **bug fixed** | `test_matrix_transpose_preserves_original_partial_overlap_order`: 19 layouts, complete surrounding memory; pre-fix `transpose-before.log` |
| F04: `asm:kbjoy::_convertjoy` | Width/signedness | `0x207d8` AND BX,0Fh; byte table load at `0x207db`; XOR AH,AH at `0x207df`. | Masks to low four bits and zero-extends the same table byte. | **not a bug** | `test_direction_table_and_msc_random_match_original_machine_cases`: 48 calls including high-bit flags |
| F05: `asm:library_rand_124476::_rand` / `port/random.c::port_random_test_rand` | MSC runtime; width/signedness | `0x1e64e..0x1e675` multiplies 32-bit state by 343FDh, adds 269EC3h modulo 2^32, returns high word masked to 15 bits. Original state is read after each call. | The enabled production compatibility generator agrees for zero, 8000h and FFFFFFF0h initial states. Host fallback outside enabled startup is outside these cases. | **not a bug** | Same runtime test: 36 consecutive-state/return comparisons |
| F06: `asm:kbjoy::_replay_axis_value` | Width/signedness; initial state | `0x207e3..0x207f9` consumes saved analog sample/minimum/scale, signed comparison, unsigned MUL, bits 8..23, subtracts 31; retained C consumes AL. | Synthesizes -31/0/+33 from held keypad keys; no analog backend supplies comparable saved samples. | **unconfirmed** | Original operands recorded; need a specified connected-controller input/calibration case and native sample mapping. No fix. |
| F07: `asm:kbjoy::_reset_joystick_selection` | Initial state | `0x207b4..0x207d1` enables sampling and resets four extrema, retaining other calibration state. | Disables joystick selection because the host backend is keyboard-only. | **unconfirmed** | Hardware/backend contract is missing. An absent native device is not comparable to supplied original joystick samples. No fix. |
| F08: `asm:library_dos_int86_123916::_int86` | Host-service results | `0x1e45f..0x1e480` stores result registers/carry and normally retains returned AX as scalar result. | Writes the register structure but returns scalar zero. Reconstructed mouse callers use the structure; full original caller-consumer liveness is not established here. | **unconfirmed** | Needs a machine-backed caller case proving whether scalar AX can affect the game. Existing INT33/INT15 projections cover only defined structure fields. No fix. |
| F09: `asm:filewrit::_savefilez` create-error cleanup | Host-service results; initial state | `0x22573..0x2258c` attempts close with a local handle set only on successful create, then unlink, retaining the create error. | Skips close/unlink if no host file opened. Return errors 2/5 agree; effects on a pre-existing destination are not established. | **unconfirmed** | Save test records original cleanup attempts; need DOSBox-X failure injection with a pre-existing destination and corresponding host failure. No fix. |

## Remaining work and validation

The JSON's six obligations on each row are the untruncated review backlog.
In particular, linear instruction windows do not settle every result consumer,
same-address labels do not settle indexed cross-object spans, and passing scenario
assertions do not settle initial hardware state or arbitrary host failures.
Unmapped helper/build/macro rows need a caller/expansion mapping, not an invented
original function. Thus the exhaustive static audit requested is still open.

Scratch evidence and the file-changing step ledger are under
`build/workers/portaudit/`. The baseline screen is `audit/six-classes-baseline.json`;
the corrected inventory screen is `audit/six-classes.json`. Nothing in `game/`,
`lib/`, `recipes/` or `layout/` was edited. Git metadata is untouched.

Requested verification passed:

- `python port/build.py build`: 38 game / 46 host objects, zero function/data stubs.
- `python tools/validate.py --tests port --no-proof`: 186 passed, no failures or
  skips; final exit 0, 147.1 seconds. The earlier run passed the tests but was
  rejected by the input-drift guard because audit files were edited during it;
  that receipt remains in `port-tests-input-drift.log`.
- `python tools/validate.py --proof-only`: `HYBRID_EXACT`, independent compiler
  PASS, BSS/runtime placement PASS; exit 0, 38.6 seconds.
- Focused audit/inventory regression: eight tests passed. `git diff --check`
  passes with `core.whitespace=cr-at-eol` for the existing CRLF detector source.

Image ownership remains C code 137,978 / C data 16,640 bytes; ASM code 30,977 /
ASM data 5,575; pinned runtime code 7,576 / data 1,192; unresolved raw **0**.
The other image bytes are 56 link fill and six BSS-in-image bytes. The proof also
reports zero raw BSS debt. These prove the reconstruction invariant separately
from port behaviour and from the unfinished static review.

Detailed outputs are in `build.log`, `focused.log`, `port-tests.log`, `proof.log`
and adjacent result receipts under the scratch directory. The port and proof
commands are scoped runs; no claim is made that the unrequested reconstruction
tool test group was rerun.
