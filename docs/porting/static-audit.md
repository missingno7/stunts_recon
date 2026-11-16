# Static audit of the port boundary

Status: **partial semantic review; correspondence gaps closed**. The follow-up
closes F06/F07/F08/F09 with original-machine/DOSBox-X cases. Every current row
has either an original scope or an explicit host-only reason; none is left
unmapped. A caller/expansion scope is broader than an isolated routine proof.
The report continues to distinguish static signals, reviewed contracts and
finite execution evidence; general whole-domain equivalence is not asserted.

Follow-up decisions: four confirmed bugs fixed (F06-F09), one rejected candidate
(F10), and no named finding still unconfirmed. The general semantic-review
obligations below remain separate from those closed findings.

The first pass (`51668aa`) screened 1,423 baseline / 1,422 corrected rows, with
438 / 435 respectively marked never executed by the earlier GCC profiles.
The current inventory has **1,427 overlapping rows**: 1,036 mapped and 391
host-only. Its earlier-profile join marks 438 never executed; these rows are
processed first. No new GCC coverage run is claimed. New focused tests execute
paths absent from those older profiles.

Of the previous **872** unmapped rows, **483** now have original scopes and
**389** are explicitly host-only. The old native-helper row for
`port/sprite.c::sprite_setup1_from_arg_pointer` is folded into its now-grounded
`asm:patlines::sprite_setup1_from_arg_pointer` row (load `0x25af6`); it was not
silently dropped. Six new helper/contract rows and that deduplication account
for the net inventory increase of five.

| Class | Mapped original scope | Host-only | Still unmapped |
|---|---:|---:|---:|
| Width/signedness | 1036 | 391 | 0 |
| Aliasing/layout | 1036 | 391 | 0 |
| Host-service results | 1036 | 391 | 0 |
| Initial state | 1036 | 391 | 0 |
| MSC runtime | 1036 | 391 | 0 |
| Added glue | 1036 | 391 | 0 |

Host-only rows have no direct historical routine counterpart; classes 4 and 6
record the state they expose and the ordering they add. The other four classes
are marked `host-only/no-direct-original`, not credited with a machine-equivalence
pass. There is no remaining unmapped-row reason list because its count is zero.
There are 660 diagnostic code intervals (including the four original driver
images), 49 undecoded linear tails and 499 same-address data-symbol groups.
Undecoded bytes are retained; a linear tail or a scoped mapping never becomes a
claim about executable reachability or whole-domain behaviour.

## Rerunning and evidence

From the repository root:

```powershell
python tools/porting/semantic_audit.py --six-classes --work build/workers/portaudit/round2/audit
python -m unittest tests.test_sdl3_static_audit tests.test_sdl3_static_candidates -v
```

The first command runs the existing production-overlay/compiler/span audit and
writes `report.json`, `compile-report.json`, and `six-classes.json` under its
work directory. `--port-layer PATH` selects another inventory, including the
saved baseline. `--inventory-only` omits compiler verification and retains that
limit. The ordinary semantic audit's `PASS` is separate from the six-class
report's `SCREENED; semantic review incomplete` status.

Each row retains its ID, review ordinal, priority phase, coverage, portable source
and body hashes, original interval references, lexical risks, and six explicit
obligations, correspondence classification and reason. Host additions include
their observable state and required ordering. Each original interval retains its complete byte hash, instruction
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
   stated finite scopes. Real DOSBox-X directory/FAT12 cleanup cases below extend those supplied
   results. Other error-code translations remain open.
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

"Not a bug" below is limited to the named original/native cases. Existing ten
scenario-discovered fixes are regression anchors above, not counted as new bugs.

| Finding / row | Class | Original behaviour and evidence | Port behaviour | Verdict | Test / remaining evidence |
|---|---|---|---|---|---|
| F01: runtime/unknown-address inventory joins; `asm:library_rand_124476::_rand` and other missing-address rows | Added glue (audit tooling) | Pinned `_rand` is load `0x1e64e`, from member start 124476 + public offset 18; it is not a sprite routine. CRT category alone proves no reachability fact. | Null keys incorrectly joined to `sprite_set_1_from_argptr`; coverage notes also asserted no original caller without proof. Both corrected, with accepted public-entry coordinates and explicit unknowns. | **bug fixed** (tooling) | `test_missing_addresses_never_join_to_an_unrelated_native_function`; `test_public_interval_uses_pinned_declarations_and_rejects_unknowns` |
| F02: `asm:filewrit::_savefilez` / `port/file.c::savefilez` | Host-service results | At `0x225a3` INT21 AH=3Eh is immediately followed by XOR AX,AX at `0x225a5`, irrespective of carry. After successful writes a close error does not unlink or report failure. | Previously returned 2 and deleted the completed file on a failed `fclose`. Now completes buffered writes with `fflush` in the write phase, then ignores close status. Write failures still unlink/report. | **bug fixed** | `test_save_close_status_matches_original_including_failure`: six supplied service-result cases; pre-fix failure in scratch `save-before.log` |
| F03: `asm:fontmat::_mat_invert` / `port/matrix.c::mat_invert` | Aliasing/layout | `0x22aa3..0x22ada` writes output indices 0,3,6,1,4,7,2,5,8 and rereads source after each store. Accepted public offset locates the entry independently of the old incomplete function inventory. | Previously wrote 0,1,2,3,4,5,6,7,8: six partial-overlap cases differed. Now preserves the original store order. | **bug fixed** | `test_matrix_transpose_preserves_original_partial_overlap_order`: 19 layouts, complete surrounding memory; pre-fix `transpose-before.log` |
| F04: `asm:kbjoy::_convertjoy` | Width/signedness | `0x207d8` AND BX,0Fh; byte table load at `0x207db`; XOR AH,AH at `0x207df`. | Masks to low four bits and zero-extends the same table byte. | **not a bug** | `test_direction_table_and_msc_random_match_original_machine_cases`: 48 calls including high-bit flags |
| F05: `asm:library_rand_124476::_rand` / `port/random.c::port_random_test_rand` | MSC runtime; width/signedness | `0x1e64e..0x1e675` multiplies 32-bit state by 343FDh, adds 269EC3h modulo 2^32, returns high word masked to 15 bits. Original state is read after each call. | The enabled production compatibility generator agrees for zero, 8000h and FFFFFFF0h initial states. Host fallback outside enabled startup is outside these cases. | **not a bug** | Same runtime test: 36 consecutive-state/return comparisons |
| F06: `asm:kbjoy::_replay_axis_value` | Width/signedness; initial state | `0x207e3..0x207f9` scales the saved analog X sample; keypad state is not an input. AL is consumed as a signed byte. In the no-device case the scale remains zero and AL is E1h (-31). | Removed invented keypad endpoint/centering synthesis; uses saved timeout sample and original word multiply/extraction. | **bug fixed** | `test_absent_joystick_calibration_reset_and_saved_axis_match_original`; playback-prefix bypass test |
| F07: `asm:kbjoy::_reset_joystick_selection` / `_get_joy_flags` | Initial state; width/signedness | Reset at `0x207b4` sets enable=1 and four extrema only. With no controller, port201h stays FFh, samples time out at 0050h and calibration still advances: flags 6, then 9, then 6. | Replaced forced disable/constant-zero return with the original no-response calibration path; no connected-device sampler was added. | **bug fixed** | Actual DOSBox-X `joysticktype=none` 80-call stream and 80 reset/enable-bit machine cases |
| F08: `asm:library_dos_int86_123916::_int86` | Host-service results | `0x1e45f..0x1e480` stores result words/carry and returns AX. All 12 retained source calls discard the scalar, but this is still the MSC ABI. | Returns signed16 AX promoted to host int; successful INT33 output carry does not inherit the input union field. | **bug fixed** | Six original/native scalar/union cases, including aliased buffers and an initial nonzero cflag; complete call list below |
| F09: `asm:filewrit::_savefilez` create-error cleanup | Host-service results; initial state | `0x22570..0x2258c` preserves create AX, attempts close using an uninitialised local handle, then unlinks the requested filename through SS:BP+6. Real DOSBox-X exhausted handles returns 4 and deletes an existing writable destination; read-only and missing-path unlink attempts fail. | Attempts unlink of the resolved mutable save destination even if create failed, preserves read-only files, returns measured directory-drive read-only=2, handle exhaustion=4 and backslash path-not-found=3. | **bug fixed** | Real DOSBox-X directory/FAT12 cases versus native file results and bytes; details below |

| F10: `savefilez` destination is an existing directory | Host-service results | Original load `0x22570..0x2258c` attempts AH41h after failed create; the pinned directory drive returns AX2 and retains the directory. | Path resolution rejects the directory as a file destination and leaves it intact, also returning 2. | **not a bug** | `test_save_create_cleanup_matches_real_dosbox_filesystem_effects`: real directory fixture, return, kind and file bytes compared |

## Correspondence and host review

`semantic_mapping.port_correspondence()` joins accepted recipe members and OMF
publics, current canonical aliases, actual native direct-call/callback-reference
scopes, macro expansion users and Python old-source anchors. Shared rewrite
helpers retain their enclosing adapter scopes. Declaration-wide transforms are
mapped to the retained C ABI view, not falsely attributed to one routine.
Diagnostic scopes remain explicit and may contain data or several routines.
The AD15/PC15 helper roots name original driver entries and sealed complete
`.DRV` images; the MT15/TD15 debt contract retains those original images too.
Driver coordinates are file-relative and are never interpreted as MCGA load
offsets. Whole-driver linear decoding does not prove all internal paths.

Explicit host policies cover process/SDL setup, config, diagnostics, observation,
input queues, entropy, the tick clock, sound-device sinks, projection snapshots
and enhanced rendering. Each row records its one-line reason, class-4 state and
class-6 ordering. Host classification requires an existing definition; an
invented helper in a host-policy file remains unmapped. No unmatched row defaults
to host-only. Regression checks reject missing mappings, invalid spans and the
old incorrect arithmetic-helper association.

The six-class report now records every row's correspondence and all six checks,
with original operands and portable type/risk sites where applicable. The
production audit still runs 672 compiler/layout assertions and existing bounded
span checks. Rows retain `requires-review` or `host-only/state-order-review`
when these mechanical checks cannot settle every consumer, alias or schedule.
This is the remaining semantic-review debt, distinct from the closed mapping
and candidate lists. No warning, differing stream or undecoded tail is masked.

## Every retained int86 call

`semantic_mapping.int86_call_sites()` reruns this list. All are in `lib/mouse.c`;
there are no calls in `game/` or other `lib/` C units. Each discards the scalar:

- line 22, `mouse_set_pixratio`: INT33 pixel ratio, scalar discarded.
- line 32, `initmouse`: INT15 C201, scalar discarded.
- line 34, `initmouse`: INT33 reset, scalar discarded; AX/BX read from the union.
- line 56, `setmousebounds`: horizontal bounds, scalar discarded.
- line 60, `setmousebounds`: vertical bounds, scalar discarded.
- line 69, `mouse_get_position`: position/buttons, scalar discarded.
- line 85, `mouse_show_cursor`: show, scalar discarded.
- line 96, `mouse_hide_cursor`: hide, scalar discarded.
- line 109, `setmousepos`: move, scalar discarded.
- line 118, `getmousepos`: position/buttons, scalar discarded.
- line 132, `setmousex`: horizontal bounds, scalar discarded.
- line 143, `setmousey`: vertical bounds, scalar discarded.

The new test executes the original MSC wrapper, not a rewritten approximation.
Its supplied successful BIOS/mouse responses are inputs. Native output compares
all seven register words and signed scalar AX, with separate and aliased unions.

## Create failure, deletion and full disks

`diffharness.dos_services.capture()` builds a small COM loader under ignored
`build/`, reads the complete locked load image as data, applies all ordered
relocations and calls the original FAR entry with DS=SS=DGROUP. It records DOS
requests through a private INT21 observer. The requested destination is copied
unchanged to the original near filename argument. The original routine itself
is never patched. The runner is bounded and terminates DOSBox-X after each case.
No original asset or expected image is regenerated from native output.

The original always attempts AH3Eh and AH41h after a failed create. The close
handle is unspecified stack residue on that path; the port does not invent a
host handle from that residue. Unlink uses the requested name, not a fallback
basename or source asset path. With a pre-existing writable destination and
exhausted DOS handles, the failed create returns AX4, cleanup frees a handle and
unlink deletes the file. An explicitly injected AX5 create failure likewise
reaches real DOS deletion and removes the writable destination.

On the pinned directory drive a read-only destination yields AX2 and survives
unlink. A backslash missing parent yields AX3 and the same unlink name fails;
a sibling file in the current directory remains unchanged. An existing directory
at the destination returns AX2 and survives the failed unlink; the native
resolver rejects it before a file handle is opened and retains the directory. The existing
forward-slash invalid-name AX2 scenario is preserved. Generic denied-create
AX5 remains a separate supplied result. Native cleanup is confined to the
mutable save root and retains read-only files even on hosts whose directory
permissions would otherwise permit unlink.

Full disk does not imply that create of an existing writable file fails. A
fully allocated FAT12 image with a three-byte old destination frees/reuses that
file's allocation and successfully saves `abc` (AX0). With an empty existing
file and no free clusters, create succeeds, write is short, AX becomes 1 and
cleanup removes the incomplete destination. Tests compare these original file
bytes/existence and returns against native successful/short-write service inputs.

The capture also preserves a DOSBox-X backend diagnostic: its FAT driver permits
an existing read-only file to be created/truncated, whereas the directory-drive
case fails. That FAT case is not mislabeled as a create failure or used to claim
a universal DOS errno mapping. The native file adapter follows the measured
directory-drive contract used by this port's original scenario oracle. DOSBox
backend quirks and unspecified stack-handle effects remain explicit limits of
these finite comparisons.

## Future feature: joystick device support

No SDL joystick device backend or invented connected-controller model was added.
Current support is the original with no responding controller. A real DOSBox-X
capture with `joysticktype=none` executes the original reset and 80 polling calls;
its 6/9/6 flag sequence agrees with the native calibration and with Unicorn's
explicit FFh port input. Reset also agrees under retained calibration state and
nontrivial enable-byte values. The integration probe keeps all its assertions
and updates its former keypad/disable assumptions to this original contract.

For a future connected-device implementation, preserve this original contract:

- `_joystick_enabled` bit 0 gates sampling. Reset sets it to 1 and sets X/Y
  minima to 0050h and maxima to zero; it retains previous samples, thresholds,
  scale factors and calibration countdowns.
- A write to port201h starts RC measurement; bits 0/1 are sampled with a bounded
  signed count below 0FA0h after the 14h-loop delay. Both sample words start at
  0050h and retain that timeout value when no axis responds.
- Calibration has independent 14h countdowns. X uses signed minimum/direction
  comparisons; Y uses an unsigned minimum and unsigned direction comparisons,
  and its maximum countdown tests zero rather than signed <=0. Preserve the
  actual word SUB/SHR/ADD wrap and update order.
- Positive span sets scale to unsigned 4000h/span. Thresholds are minimum plus
  half-span plus/minus quarter-span. Sample extrema/countdown reset rules are
  those in `lib/kbjoy.ASM`; they are not generic joystick dead zones.
- Buttons are active low: the first and last port samples contribute
  `((first & last) & 30h) ^ 30h`. Keyboard control flags take precedence when
  nonzero. Keypad presses do not overwrite the saved analog sample.
- `_replay_axis_value` clamps according to the original signed comparison,
  multiplies the word difference by the unsigned scale, extracts bits 8..23,
  subtracts 31 modulo 65536 and exposes AL as a signed byte to the retained C.

Joystick help/calibration requires visiting all nine direction indices. With no
controller only the timeout-derived indices appear; on leaving the dialog the
retained C rejects the selection and disables joystick input. There is no
separate successful no-device detection that should be fabricated by reset.
Recorded steering is converted into replay input bytes by `mousesteer`; replay
playback consumes the recorded stream through unchanged reconstructed C. The
original `moveeverything` gm_playmode=2 prefix advances playback without calling
live joystick sampling, independently checked for enable bytes 0,1,2,FFh.
Joystick-recorded replays therefore do not require a live controller backend.

## Validation and scope

Focused candidate/audit/input tests: **15 passed**. Reconstruction proof:
**PASS HYBRID_EXACT**, including the independent DOSBox-X compiler gate for 78
contributions; 2,588 ordered relocations, raw bytes zero.

The requested verification passed:

- `python port/build.py build`: 38 game C objects, 46 host objects, zero function
  or data stubs (`round2/build-pass.log`, `build-pass.result.json`).
- `python tools/validate.py --tests port --no-proof`: **193 passed, 0 failed,
  0 skipped**, including the final directory case (`round2/port-tests-pass.log`,
  `port-tests-pass.result.json`).
- `python tools/validate.py --proof-only`: **PASS HYBRID_EXACT** and independent
  compiler PASS (`round2/proof.log`, `proof.result.json`).

All receipts and the eight original DOS filesystem fixtures plus the 80-sample
no-device capture are under `build/workers/portaudit/round2/`. The file-changing
ledger is `build/workers/portaudit/steps2.json`: 15 paths, each in exactly one
step. The pre-existing game and DOSBox-X processes were preserved; loaded build
files were moved to scratch to allow rebuilding, and no audit subprocess remains.
No `game/`, `lib/`, `recipes/`, `layout/`, original asset or Git metadata was
edited. The existing original results-music/replay-transition differences remain
listed in step-divergences; this audit does not turn those finite failing streams
into passes.
