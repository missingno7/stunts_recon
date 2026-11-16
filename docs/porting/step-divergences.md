# Port-layer comparisons by game step (steps 8 and 9)

Expectations come only from sealed original MCGA/DOSBox-X runs. Original IRQs
continue during CPU work, so IRQ equality and a fixed count of idle UI callbacks
are outside the game-step contract. No original asset, lock or proof source changed.
The scoped comparisons, including failures, are in
[scenario-results.json](../../tools/porting/step_oracle_cases/scenario-results.json).

## Scenarios and measured results

| Scenario | AD15 | PC15 | Exact demonstrated scope |
|---|---|---|---|
| Startup | agrees | shared visual path not separately recaptured | splash, title, menu; consumed input |
| Opponent selection | agrees | agrees | 4 checkpoints, ordered keys, comparable bounded calls |
| Car/track selection | agrees | Lancia pose differs | Countach/default track agree; 137 Lancia indices, same palette |
| Manual gears and crash | agrees | agrees | 434 complete states/frames; gear-up inputs at slots 40/90/150; crash at frame 354 |
| Replay pause/seek/save | agrees | agrees | 201 states/frames, pause at 201, seek to 0, 5 checkpoints, identical 2029-byte S9REPLAY.RPL |
| Editor load/place/remove/save | agrees | agrees | 8 checkpoints; DEFAULT load, road placement/removal, identical ZZZNINE.TRK and .HIG |
| File error | agrees | agrees | bad/file.trk returns 2, error UI, no nested directory created |
| High-score entry/save | defined fields/UI agree; raw file differs | same | 434 states/frames, qualifying-entry fixture, name/XY/score and unchanged older records |

The high-score case deliberately supplies finish200/penalty0 at `evaluation`
entry on BOTH implementations. It exercises the original qualification UI,
`addhighs`, text entry and saving, rather than claiming an earned lap after a
crash. This explicit scenario control is logged and off outside opted-in v2 tests.
All comparable newly captured routine inputs have matching outputs/returns;
unmatched inputs remain listed and are not finite-case passes. UI call order is
not inferred from unordered operand matching.

## Port defects found and fixed in step 9

1. **MSC character-table ABI** (`port/build.py`, `port/msc_ctype.h`): the original
   macros index a 257-byte `_ctype` table, but Windows' CRT exports wider entries.
   The first useful symptom was `floadreq` ignoring typed D and loading ALLJUMPS
   instead of DEFAULT. Generated game code now uses a private original byte table
   grounded at DATA load192414. Every EOF/byte classification is checked against
   locked original data; editor load/render/save checkpoints now agree.
2. **Result track comparison** (`port/game_abi.py::adapt_result_track_comparison`):
   native signed-char promotion made unchanged track bytes >=128 compare unequal.
   Original load399d is `26 38 01` (CMP ES:[BX+DI],AL), not a promoted integer
   comparison. The native byte helper matches all 65536 input pairs against live
   execution of that original instruction. The former result screen said the
   high-score table was unavailable (5882 differing indices); it now agrees.
3. **Missing save-directory policy** (`port/file.c::resolve_save_destination`):
   the port recursively created missing parents for DOS create requests.
   DOS AH3Ch creates a file only. Only the host-owned save root is now implicit;
   `bad/file.trk` reaches the original error dialog instead of silently saving.
4. **Nonfatal writer ABI** (`port/file.c::savefilez`): the port aborted on that
   failed create. The `_savefilez` entry uses policy1 and returns the error; the
   old descriptive fatal/nofatal aliases are inverted. Typed original/native
   filename/length/status capture agrees at return2 (not guessed error3), and
   successful editor/replay saves agree. Other host error translations are not
   whole-domain DOS proofs; short-write AX1 is grounded in lib/filewrit.ASM.
5. **High-score marker alias** (`port/game_abi.py::adapt_highscore_dialog_alias`):
   `messagebox` writes a two-word marker through &dialogY. MSC placed dialogX at
   the next word; GCC's separate locals did not preserve that alias, causing a
   font-row bounds failure. Original loads at 1b26/1b53/1b56 use BP-3Ch/-3Ah around
   the messagebox call at 1b40. The generated overlay uses an explicit two-word
   array. Both original/native `instringz` calls now agree on XY64,106, limit16,
   output and return; full entry/saved-screen images agree for both drivers.

These are older native ABI/host-service defects. No game/lib/include source was
edited. Focused tests are `test_sdl3_ctype`, `test_sdl3_result_track_bytes`, the
file-service probe and `test_sdl3_scenarios`' original editor/error/highscore cases.

## Engine pitch: closed under the agreed target contract

Original loading permits 34 `audio_driver_timer` callbacks between engine init
and start; the native clock grants one. Those callbacks smooth `rate_avg` before
playback. That count depends on original CPU speed, so no warm-up is invented.
Compare the nine exact `audio_op_unk2` pitch-target arguments in consumption order
instead of smoothed entry24h requests, **only** for ADENG/PCENG continuous voices
whose original entry09 bind uses noteFFFFh. Other pitch entries, repetitions,
arguments, payloads, returns and per-resource/voice/entry order remain exact.
Cross-entry ISR interleaving is not claimed. Original raw pitch hashes/counts
remain in references as diagnostics; the exception does not cover effect pitch.

Both 200-frame race sound streams agree in full under this declared projection,
as do the crash races through frame433 (433 targets), with both drivers. These
scopes check every other request, not merely the common prefix of differing
content. `completed-race-*.json` now also records the original continuous-voice
identities; tests no longer omit every entry24h regardless of resource/voice.
Replay sound through frame200 likewise agrees for both drivers.

## Remaining differences, with attribution limits

* **PC15 Lancia animation phase:** 137 pixels at the named key checkpoint, no
  palette difference. `selectcar` accumulates `rotationDeltaY` from `docursor`'s
  elapsed time. At key ordinal1 the first differing rotation operand is original
  `intcos(1)`/`intsin(1)` versus native `(4)`; math outputs agree for comparable
  operands. Earlier sine-call evaluation order also differs between compilers,
  without changing those results. This is CPU/idle-dependent UI animation,
  not an established raster bug. No arbitrary delay or angle override was added.
  Its original frame stays stored; the complete PC15 selection case stays red.
* **HIG raw string tails:** `addhighs` copies a partly initialised 52-byte local.
  `strcpy` writes only through NUL. In the new record, offsets9..16,38..40,44..49
  differ (17 bytes in the AD15 capture); name/car/opponent strings, style41,
  score50..51 and six older records agree. Source writes and the record layout
  explain these unused tails. No original stack residue is invented on the host.
  Whole-file hashes remain different; defined-field/UI regressions are narrower.
* **Results music/ISR tail:** every crash-race request through433 agrees under
  the pitch rule. Differences start in frame434's result UI: AD15 instrument
  poll27h counts 4 versus16 on voices1/2, later note controls on voices8/9;
  PC15 voice4 has an extra stop0fh/polls, and pump30h counts differ. High-score
  entry adds further result-UI callback progress. The observer locates these
  outside the agreeing race scope. CPU work before the first key poll is the
  attribution lead; private driver-state equivalence is still unproven. These
  tails are NOT excused by the engine-pitch rule or counted as agreeing.
* **Replay boundary sound:** original has 201 targets versus native200, with
  the final original target at committed frame201 before paused UI. Through200
  all requests agree; full pause/seek sound still differs (including 27h/30h and
  PC15 volume12h). Input, all 201 states/frames, seek checkpoint and saved replay
  bytes agree. The exact audio-update/transition scheduling boundary needs a
  separate decision/investigation; no missing target is silently discarded.

`scenario-results.json` preserves first differing request content and counts.
Named checkpoints never search for matching pixels. Full raw comparisons return
nonzero for the open cases; a passing finite regression is not whole-scenario PASS.

## Earlier race fixes retained

Step 8 fixed an injected pre-paint idle yield that admitted an extra inactive
update, and published the faithful completed frame at original `dodash(2)` RETF
(after gauge restoration). Both drivers agree on 200 states, 400 ordered core
call groups and 200 indexed completed frames. Intermediate partial frame1 blits
remain diagnostic. The y9 raster defect was the original polygon tail's JLE
before adding one scanline: a non-flat polygon clipped to one row draws nothing.
`port/sprite.c` now does so. Twenty-seven captured original polygon cases cover
frames5,6,18..22; true flat/vertical line fallbacks still draw. Step7 mouse reset,
keyboard one-word overwrite and INT15 C201 fixes remain original-anchored.

## Fixtures, coverage and verification

All ten legacy screen references were recaptured with DOSBox-X; their sidecars,
manifest and README contain original receipts and regeneration commands. Startup
and selection tests compare named checkpoints rather than searching images.
Splash/title bytes stayed identical, but provenance is newly original-captured.
The numeric startup seed is explicitly a prescribed input vector, now recorded
by the original observer; obsolete interpreter provenance/paths were removed.

The refreshed inventory has **1423 overlapping rows: 99 with finite comparable
evidence (92 agree, seven raw ordered sound streams differ), 1324 not exercised**.
The rise includes scoped adapters/helpers and four narrowly scoped contracts;
contract rows now inherit their named existing finite evidence. It does not mean
99 independent APIs or whole-domain proofs. Every unexercised row has its own
`reachability` object: a candidate scenario/probe and the concrete missing mapping
or contract. Build rewrites/macros are not guest entries; pinned MSC entries are
not linked as those routines natively. For other helpers, a candidate is not a
proved call-graph reachability claim. Unknown reachability remains explicit.
See [port-layer.md](port-layer.md) for extracting every remaining row.

`python port/build.py build`: 38 game / 46 host objects, zero function/data
stubs. Focused original-backed scenario/screen/ABI/file checks passed (24 tests);
after updating the two isolated observer-hook harnesses and migrating the old
fixture-provenance assertion, their 12 affected tests also passed. The initial
full run's three harness/schema failures (188.1s) are retained in scratch logs.
The final `python tools/validate.py --tests port --no-proof` passed **179 tests,
zero failures/skips, 195.9s** (step8 baseline186.3s; about 5% longer with 17 more
tests). `python tools/validate.py --proof-only` passed **HYBRID_EXACT**, independent
compiler PASS, unresolved raw bytes zero, **49.1s**. Semantic audit passed 672
compiler assertions. All owned captures/tests/proof processes exited; no observer
or game was launched as a persistent/background service.

Commands, detailed logs and untruncated per-row debt are under
`build/workers/portclock/step9/`; all changed paths belong to exactly one of the
four entries in `build/workers/portclock/steps9.json`. Git metadata was not edited;
changes are left for review and committing by the user.
