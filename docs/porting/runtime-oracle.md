# Original-game runtime oracle: DOSBox-X

The current reference executes the locked original MCGA executable and original
AD15/PC15 drivers in patched DOSBox-X 2026.08.31. The earlier experimental
interpreter capture route is retired. Its archived static images, protocol names
and provenance records are historical evidence; they are not new tick references.
No expectation below is generated from the SDL port. Port-layer acceptance now
uses the semantic-step race comparison below. Absolute IRQ comparisons remain
diagnostics and explicitly bounded earlier test subsets.

## Build and capture

See [observer source and build prerequisites](../../tools/porting/dosbox_observer/README.md).
The Stunts copy lives under ignored `build/`; SimAnt and installed DOSBox-X trees
are read-only. Readable patches and pinned source hashes are tracked. The normal
CPU/PIT/PIC execute unchanged; SimAnt's ideal-machine clock is not used.

```powershell
python tools/porting/dosbox_observer/build.py --out build/workers/portclock/dosbox-observer
python tools/porting/tick_oracle_scenarios.py replay build/workers/portclock/replay.json
python tools/porting/original_tick_oracle.py --dosbox build/workers/portclock/dosbox-observer/dosbox-x.exe --script build/workers/portclock/replay.json --output build/workers/portclock/replay-original --ticks 2550 --audio ad15 --port-exe build/sdl3/stunts.exe
```

Output must be a fresh directory. Capture uses scratch asset/save copies and a
bounded subprocess, never mounts writable `assets/`, and leaves no process running.
The MZ is obtained from `oracle.verify(write=False)`, not the port build. Receipts
hash the original MZ, driver, oracle lock, script, binding table, observer and
DOSBox executable, plus the completed JSONL and frame dictionary. Build receipts
add all reused compiler-object hashes. A safety exit and subprocess timeout bound
failed captures. The port run uses faithful rendering, manual ticks, a null audio
sink with the real translated driver, and the same input/seed/settings.

The scenario limits are startup 1000, selection 2200, race 1800, replay 2550 raw
ticks. Repeat each with `--audio ad15` and `--audio pc15`. The default CPU profile
is `cycles=fixed 30000`, normal core, turbo scheduling. It is recorded, not a
claim of cycle-independent original behavior. The observer forces rate 20,
detail 0 and dirty rectangles at the original benchmark decision join. Its
benchmark still executes; the modern port explicitly replaces it with settings.
The existing startup-seed fixture supplies chosen inputs (CRT/Kevin PRNG states,
counter 592, 8014 frontend stir iterations; initial stir 12000). The fixture's
historical capture metadata is not used as expected behavior in this comparison.

## Recorded boundaries

`original.jsonl` labels every observation by the actual original INT 8 entry
count and ordered sequence within that tick. The load segment is derived at the
MZ entry; accepted symbols and locked instruction bytes ground all hooks.

- Keyboard FIFO delivery uses raw Set-1 bytes from the script, with no added
  physical-key typematic. Records include actual port-60 reads and INT 16h
  dequeues as consumed by `normkey`/`normkeyi`; ASCII normalization matches them.
- Every far driver jump-table entry records raw argument words, entry/return
  tick, AX, foreground/callback context and callback slot. Stable voice/chunk
  indices and loaded-resource name/hash/offset replace pointers in comparisons.
  Logical chunk events remain port-only; no PCM or device-demand counts compare.
- Explicit graphics routine completions record 320x200 indices and 256 DAC6
  RGB entries, deduplicated in `original.frames` (64768 bytes per unique image).
  Pixels come from host VRAM using the pinned compatible chain-4 map; unsupported
  video mappings abort. Reads cannot charge guest VGA wait states or alter latches.
- `movesim` returns record all 1120 GAMESTATE bytes. The portable projection
  retains full-state SHA-256, frame, input mode, travel distance and car position.
  Callback rows retain replay mode/control/target/frame; mouse observations show
  what the game's hit-test path actually read, separately from injected input.

Routine completion is observed immediately before its matching RETF, after its
body/epilogue and before another IRQ can stamp it with the next tick. The INT 16h
shim uses RETF 2. A publication is a named drawing boundary, not VGA scanout:
partial transition-row publications and host presents have no original peer.
Void driver AX values and raw guest pointer words are retained but not compared
as defined portable scalars. Same-tick event order is preserved within each stream.

## Comparison and references

`compare_tick_streams.py` reports the first strict divergence with neighboring
events, then bounded content resynchronization and further differences. Tick
offset histograms are diagnostics: alignment never changes the PASS criterion.
Empty/unexercised streams are not passes. Complete scenario results and attribution
are in [the divergence list](tick-divergences.md).

```powershell
python tools/porting/compare_tick_streams.py --original build/workers/portclock/replay-original/original --trace build/workers/portclock/replay-original/port/trace.jsonl --sound build/workers/portclock/replay-original/port/sound.jsonl --out build/workers/portclock/replay-comparison.json
python tools/porting/pack_original_ticks.py build/workers/portclock/replay-original --scenario replay --out build/workers/portclock/replay-ad15-reference.json
python -m unittest tests.test_sdl3_original_ticks -v
```

The packer accepts only sealed DOSBox-X captures and never reads a port trace.
Compact references under `tools/porting/tick_oracle_cases/` retain original stream
hashes, bounded driver initialization, input, sparse compressed frames and state
hashes. The input scripts retain CRLF bytes covered by their sealed hashes;
repository attributes deliberately disable automatic newline conversion.
Tests assert only demonstrated agreeing subsets with ticks intact. A
matching state by game-frame number is explicitly a content diagnostic, not an
absolute-tick pass. No complete scenario is silently accepted on image search.
For instruction logging and manual debugger checks, [CAPTURE.md](CAPTURE.md)
remains available independently of this observer.

## Port-layer comparison by simulation step

The [joined inventory](port-layer.md) identifies replacements, services and
semantic adapters. [Race findings](step-divergences.md) describe the first bounded
slice; it is not coverage of every entry.

```powershell
python tools/porting/dosbox_observer/build.py --out build/workers/portclock/observer-step7
python tools/porting/step_oracle.py --out build/workers/portclock/race-step7-ad --dosbox build/workers/portclock/observer-step7/dosbox-x.exe --audio ad15 --frames 200
python -m tools.porting.compare_step_streams build/workers/portclock/race-step7-ad/original build/workers/portclock/race-step7-ad/port --out build/workers/portclock/race-step7-ad/comparison.json
python -m tools.porting.pack_original_steps build/workers/portclock/race-step7-ad/original --out build/workers/portclock/new-race-ad15.json
python -m unittest tests.test_sdl3_original_steps tests.test_sdl3_port_layer -v
```

Repeat with `--audio pc15` and fresh output. Comparison exits nonzero for open
differences; tests assert only named agreeing subsets. Packer verifies seals,
accepts original data only and refuses overwrite. Compact references under
`tools/porting/step_oracle_cases/` hold original group/state hashes, sparse compressed
indexed frames, service cases and matrix arguments, never port expectations.

`stunts-step-input-v1` uses `anchor_tick:0` and events with `at`, `phase`, `channel`,
`payload` (write `at` first, as the shared writer does). Phases 1..5 are intro,
main menu, protection, line input and race UI. `at:K` delivers at the next input
poll after K successful key dequeues in that phase, starting at zero. Phase 6
uses recorded input slot N (`tmr2` at `moveeverything` entry), before keyboard
state is sampled into replay byte N. That byte drives simulation step N even
when original CPU work later causes several catch-up steps. The same hooks are
injected into the private native overlay; no game/proof source or original
instruction changes. `--boundary-frames=N` stops at the next simulation entry
after active frame N; a safety tick cap still bounds failure.

For semantic keyboard delivery, normal DOSBox core yields at the initiating call
frame until the real PIC/INT9 delivers all queued bytes. PIT/IRQ counting continues;
this is an input-delivery barrier, not a substituted clock. Without a transaction,
the core takes its unchanged path. `core_normal.cpp.patch` and `stunts_boundary.inc`
are sealed with the build; capture rejects stale observers. Accepted symbols,
recipe frames and reviewed bounded spans ground hooks; entry bytes are retained.

`--boundary-trace=PATH` records the shared completed-call JSONL schema on the port:
unsigned DOS argument bits, defined returns, bounded memory before/after and alias
offsets. Pointers become null plus region/overlap identity. Output-only initial
bytes are observations, not input-contract bytes. INT 15h/33h project defined
request/result registers; unrelated preserved registers are outside the claim.
Sound rows also carry game frame, active mode and simulation/render scope. Private
driver memory, arbitrary DOS buffers and callback-internal routine calls remain open.

Acceptance groups by `(active mode, input game frame, simulation/render scope)`
and preserves call order. Active states align by frame number; inactive startup
cannot overwrite them. Duplicate active frame numbers reject this single-race
contract: replay needs a specified epoch/seek contract. Extra original catch-up
steps align by frame, never ticks or a search for matching bytes. Missing rendered
frames remain differences. The tool gives first differences with context, then
ordered within-group resynchronisation for diagnosis. Neither resynchronisation
nor unordered identical-input matches turns a differing full stream into PASS.

### Completed race frames and raster diagnosis (step 8)

Race image acceptance now uses the original `dodash(2)` return, after its scene
copies and dashboard restoration. The port publishes that same boundary as
`event_type:frame_complete`, `publication_reason:race_frame_complete`, with
`video_phase:frame_end`. Existing partial `video_publication` rows remain intact.
This also updates the displayed immutable snapshot; restoration after the last
scene blit previously left frame 1's shift graphic on screen. Paused replay UI
iterations which do not call `dodash(2)` keep their existing publication path.

`--graphics` on `step_oracle.py` enables eleven additional original/native
contracts during the first 22 race frames. FAR shape arguments and NEAR vertex
lists use their reviewed original stack widths. Captures include vertex bytes,
clip words, the active row 9, a 64x56 dashboard-corner hash and unpacked source
pixel hash. Whole source images are bounded by their shape headers; this is a
targeted diagnostic span, not complete raster-memory coverage. Hash bytes are
serialized little-endian on both sides. VGA peeks do not charge guest wait states.

```powershell
python tools/porting/step_oracle.py --out build/workers/portclock/race-step8-ad --dosbox build/workers/portclock/observer-step7/dosbox-x.exe --audio ad15 --frames 200
python -m tools.porting.pack_original_steps build/workers/portclock/race-step8-ad/original --out build/workers/portclock/completed-race-ad15.json
python tools/porting/step_oracle.py --out build/workers/portclock/raster-step8 --dosbox build/workers/portclock/observer-step7/dosbox-x.exe --audio ad15 --frames 24 --graphics
python -m tools.porting.pack_original_rasters build/workers/portclock/raster-step8/original --out build/workers/portclock/race-horizon.json
python -m tools.porting.compare_sound_events build/workers/portclock/race-step8-ad/original build/workers/portclock/race-step8-ad/port --out build/workers/portclock/sound-step8.json
python -m unittest tests.test_sdl3_original_steps tests.test_sdl3_race_horizon -v
```

Rebuild the observer with the current tracked includes before these commands;
repeat the 200-frame capture/packer with PC15. The packers refuse overwrite and
read only sealed original observations. `completed-race-*.json` and
`race-horizon.json` are the new compact original references; old files retain
their original provenance. Packer sparse frames now select completed frames.

Sound capture also observes original/native engine initialization, start, timer
callback entry and the nine scalar arguments to `audio_op_unk2`. Sound comparison
aligns target records by consumption order within this single-engine race, then
compares driver sequences by entry/resource/voice without dropping repetitions.
Render-frame movement can be diagnosed separately from changed content. The
initial pitch ramp retains its raw 34-versus-one callback prehistory. Step 9's
agreed contract compares exact engine pitch targets instead of smoothed entry
24h values, only for voices bound with note FFFFh to ADENG/PCENG resources.
Every other per-resource/voice/entry request remains ordered and exact; different
entries are not asserted to interleave identically. Both 200-frame race streams
agree under that rule. Startup and later crash/replay tails are separate open
comparisons, not exempted by it. See [findings](step-divergences.md).


### Scoped menu, editor and replay cases (step 9)

`scenario_oracle.py` adds `stunts-step-input-v2`. It retains phase 6's recorded
simulation input-slot meaning; menus use a stack of `(phase, visit, key ordinal)`.
Nested dialogs restore their parent's ordinal. An event's `visit` defaults to 1;
`at:K` is delivered at the named UI's `mykey` entry after K consumed keys. Direct
line/race input uses the underlying input poll. Delivery is suppressed during
`clear_keyboardbuffer` and video transitions, so a future key cannot be injected
into a flush. Real PIC/INT9 consumes original scan bytes as before. Event-array
order resolves simultaneous events. Neither implementation uses host time here.

| Phase | Original boundary |
|---|---|
| 1..6 | v1 meanings retained (intro/menu/protection/line/race/input slot) |
| 7,8,9,10 | selectcar, selectopponent, teditmain, trackedit |
| 11,12,13 | floadreq, fsavereq, messagebox |
| 14,15,16 | evaluation, drawhighs, addhighs |
| 17,18 | sprite_blit_to_video, mykeywait |
| 19..22 | doreplay modes 0..3 |
| 23,24 | clear_keyboardbuffer, savereplay |

`oracle.checkpoint` payloads contain `label`, integer `stop`, and optional
`optional`. A checkpoint publishes exact VGA indices/DAC6 on both sides before
its next input event. `at:4294967295` means the scoped function's completed-call
boundary; save tests stop after the file writer returns. A terminal checkpoint
is required; reaching the safety tick cap is failure, not a shortened success.
The original observer uses accepted FAR entries and matching SS:SP returns;
`boundary-bindings.json` seals those addresses and argument widths.

Replay state/frame comparison uses a new epoch on a repeated/decreasing committed
simulation frame, never by matching contents. A direct seek repaint without a
new simulation commit is a named UI checkpoint, not an invented state step.
The first completed dashboard frame after each committed active state is compared;
extra idle repaints are diagnostic. `compare_scenarios.py` reports exact ordered
scan/key input, all state hashes, all completed frame hashes, checkpoint pixels,
whole saved-file hashes and bounded call returns/regions for identical inputs.
UI idle calls have no fixed CPU-independent count, so those finite operand
matches do not grant whole ordered-call-stream equality. Unmatched inputs remain
explicit. `savefilez` captures its filename, length and defined status, and
`instringz` captures the original XY, initial string, bounded output and result.
Saved file content is checked independently, not represented by a host pointer.

```powershell
python tools/porting/dosbox_observer/build.py --out build/workers/portclock/observer9
python tools/porting/scenario_oracle.py editor --out build/workers/portclock/new-editor --dosbox build/workers/portclock/observer9/dosbox-x.exe --audio ad15 --ticks 1000
python -m tools.porting.compare_scenarios build/workers/portclock/new-editor --out build/workers/portclock/new-editor/comparison.json
python tools/porting/pack_original_scenarios.py build/workers/portclock/new-editor --out build/workers/portclock/new-cases --name editor --audio ad15
python -m unittest tests.test_sdl3_scenarios tests.test_sdl3_framebuffer tests.test_sdl3_selection_frames -v
```

Use fresh outputs; repeat `opponent`, `selection`, `editor`, `file-error`,
`manual-crash`, `replay`, `highscore` with both `--audio ad15` and `pc15`.
Use `--ticks 4000` for crash/highscore, 1700 for replay, 1000 for other cases.
`startup` needs only AD15 for the three original startup screen fixtures. The
stored cases embed the exact original script; export its `script` object to JSON
and pass `--script=PATH` to reproduce that input even if the generator changes.
Packers verify all original seals and never read candidate outputs. Compact cases
store state/frame hashes and at most eight distinct original operand cases per
routine; raw comparison has more calls. Receipts retain the original binary,
observer, bindings and capture identities. Hashes of raw HIG files remain stored
even though unwritten stack tails prevent byte equality; defined-field tests are
explicitly narrower. New references total about 1 MiB, excluding old race cases.

The high-score scenario is an explicit post-race UI fixture: root
`evaluation_finish_frames:200` supplies finish/penalty words at `evaluation`
entry in both original and native runs. The original still executes its score
insertion, text entry and file routines. This tests qualifying-score presentation
and saving; it is not evidence of an earned qualifying lap. Without this opt-in
v2 field no result words are changed. `stunts_observer.inc`/`stunts_boundary.inc`
record every prescribed startup/score control. The six-byte Kevin and CRT/counter
seed vector is a shared input, not independently measured entropy.

All ten legacy screen fixtures are now sealed DOSBox-X checkpoints. The
[fixture README](../../tests/fixtures/sdl3/README.md) gives original-only repacking.
Tests compare named checkpoint images directly, rather than searching the run for
matching pixels. PC15 Lancia rotation and raw score string tails remain declared
open cases in `step_oracle_cases/scenario-results.json`; they were not regenerated
from native output or silently converted into full-scene passes.
