# Replay corpus pilot

This pilot runs **one file per command** against the locked original 1.1 MCGA
image and the SDL3 manual-clock port. It does not implement or run the corpus
test or a sweep. No replay, track, downloaded file or binary is tracked.

Local inputs are `assets/DEFAULT.RPL` and 1,123 community files in
`build/replays/zak/`, from ZakStunts ZCT001..ZCT120 (2001–2008). Treat supplied
replays as data. The inventory reads headers and embedded tracks; it never
executes file content, downloads anything, or manufactures input combinations.

```powershell
python tools/porting/replay_corpus.py --inventory --replay-dir build/replays/zak
```

The inventory always includes DEFAULT. If the supplied directory is absent it
prints a clear community-skip message. Of 1,124 local files, 1,109 have the exact
1.1 extent (1,828-byte prefix plus the unsigned frame count at byte24):
1,993,732 input steps, 116 distinct embedded tracks, all 11 stock player cars,
27 with opponents. **15 files have a different header/extent** and are listed
under `unsupported`, rather than silently removed. Their frame counts at byte22
and total sizes suggest a 24-byte-header variant. Version handling is an open
dependency for a complete corpus test; this pilot does not convert those files
or claim a 1.1 comparison of them.

## Capture and compare one replay

Build the port and sealed observer from installed, pinned inputs:

```powershell
python port/build.py build
python tools/porting/dosbox_observer/build.py --out build/workers/replaycorpus/observer
python tools/porting/replay_corpus.py assets/DEFAULT.RPL --out build/workers/replaycorpus/default-new --dosbox build/workers/replaycorpus/observer/dosbox-x.exe
```

For a community replay, replace the positional path with a file under the
directory supplied to the inventory. For example the second pilot input was
`build/replays/zak/ZCT053_28ALANCHUYTRIX.RPL` (11,989 frames, Countach).
For a later user-supplied replay, put it anywhere local, inspect its metadata,
then use the same command with a fresh output directory. The original input is
copied byte-for-byte as `CORPUS.RPL` into each isolated asset directory. This
DOS filename alias also avoids long community filenames affecting menu entry.
Both games load it through Options → Load replay. Its embedded track is used;
no separate matching `.TRK` is needed.

The game initially simulates a loaded replay to its end while paused. The
semantic menu script answers the existing protection fixture, selects rewind,
then Play. Thus N recorded input frames yield **2N simulation steps**: epoch1
for initial reconstruction, epoch2 for playback with sound. Input frame1 starts
each epoch. Compare `(epoch, game_frame)` in observed order; never align by PIT
tick, search state contents, or drop a mismatching prefix. The original stop is
at the committed `update_gamestate` return; the native stop uses the existing
simulation-step limit after its snapshot. Sound after the final committed state
is outside that stop: each pilot playback has N−1 engine-target records. It is
not a claim about the replay end/paused-UI sound tail.

`NO_RASTERS` suppresses observer image payloads, avoiding the 4,096-unique-frame
diagnostic cap. `SIMULATION_BOUNDARIES` / native `--simulation-boundaries` retain
math/adapter calls inside simulation. UI phase and input hooks still run. These
flags change observation only; they do not skip original instructions, rendering,
sound requests, driver callbacks or simulation. Existing raster comparisons stay
available with the ordinary scenario tools.

The original subprocess has a mandatory timeout of at most 900 seconds. A timeout
kills and reaps it, records a failing receipt and packs only complete JSON rows
as **incomplete evidence**. Such a prefix cannot pass. A port run must reach its
requested step count. `--port-only --frames=253` is a bounded menu/simulation
diagnostic, not an original expectation. `--frames` counts simulation invocations,
including the initial pass, rather than promising a full playback.

## Reference and diagnostic files

`original/reference.json.gz` holds original provenance, replay/embedded-track
identities, ordered per-step SHA256 values and epoch/frame labels.
`states.bin.gz` retains **every full 1,120-byte state**, including on a difference.
The original JSONL and all simulation-boundary operands/results are retained as
gzip archives. Packing first verifies executable, oracle lock, input script,
bindings and raw capture hashes against the original receipt; it never reads
native results. It refuses to overwrite an existing expectation. Raw JSONL and
frame payloads are deleted after packing; `--keep-raw` retains them. Native logs
are also compressed after comparison. Isolated asset copies are removed after
their input hashes are sealed; repository assets and community inputs are never
removed. Receipts and comparison reports remain local in the run directory.

The comparator checks every state byte. Field labels come from the DOS-sized
canonical GAMESTATE/CARSTATE layouts; they are diagnostics, never masks. On the
first difference it reports the step, fields, and original/native boundary calls
at the preceding input frame, with identical-input output mismatches separated
from differing-input leads. It preserves the full boundary archives for further
routine attribution. A native trace is not a reference input.

Sound is recorded with frame, scope and foreground/callback origin. The report
includes strict typed request groups per simulation frame and the existing
resource/voice/entry sound projection from `compare_sound_events.py`, with exact
engine-target arguments. Only independently identified continuous engine entry24
voices use the established target rule; raw request differences remain visible.
PIT-dependent differences are reported, not erased. Sound or state disagreement,
or incomplete original capture, makes the single-file command return nonzero.

An existing original can be reused without another DOS run:

```powershell
python tools/porting/replay_corpus.py assets/DEFAULT.RPL --out build/workers/replaycorpus/default --finish-capture --port-output build/workers/replaycorpus/default/port-fixed
```

Use a new `--port-output` directory to run a rebuilt native executable against
that reference. An existing native output is compared without replaying it.
The pilot's `comparison-before-fix.json` and `comparison-fixed.json` distinguish
the baseline and repaired native runs. Large references stay local; no new
fixture or corpus test was added in this run.

## Original-grounded port fix

DEFAULT first diverged at epoch1 step252 (all 1,120 bytes through251 agreed).
The first byte-order field was `game_chase_camera_pos[0].y`, offset290; speed,
position and wheel fields followed. At input frame251 the first differing
`mat_vec` operand was Z=3899 original versus3699 native; comparable math inputs
had equal outputs. This was a caller input difference, not evidence against
`mat_vec`.

The cause is `update_car_speed` / source `doengine`: original loads32330..32365
sign-extend the negative speed delta, multiply25, call the **unsigned** long
divider, truncate to a word, then SAR1. For delta−1/mass15 the original returns
+2183; host signed division returned−1. The `port/build.py` overlay now makes
that unsigned conversion explicit. `test_sdl3_engine_arithmetic.py` executes the
locked instruction window with its original relocated runtime helpers for 60
delta/mass cases, including negative deltas and unsigned masses above32767,
then compares the generated native helper. Game/lib/include sources are unchanged.

After this fix, DEFAULT agrees on every byte of all **21,292** states.
Sound remains open: its first `audio_op_unk2` target at playback frame1 has the
elapsed-time argument565 original versus10701 native (the other eight arguments
agree); two later targets have6 versus5. The first raw per-step request group
difference is callback scope2/frame8, an extra original driver entry27h poll.
Later entry27h counts and smoothed engine entry24h requests differ as well.
`myupdatesfx1` passes the elapsed timer interval into `audio_op_unk2`; initial
precomputation and PIT/callback scheduling are the attribution lead. No arbitrary
warm-up, timer replacement, target masking or driver-poll exception was added.

The ZakStunts replay first differs at **epoch1 step2992**, after2991 exact states;
epoch2 repeats that first difference. The first field is
`playerstate.car_posWorld1.x`, byte338 (original1005545, native1005520).
There are5982 agreeing complete states across both epochs, out of23978.
At input2991 both states have speed0. `movecar` only writes `wheelAngle[4]`
inside `if (spd != 0)`, but the zero-speed plane crossing later reads it.
Locked load20411 writes SS:[BP+2*w−140h]; loads22691/22991/23421 read that same
span on the crossing paths. `move_on_plane` / `plane_rotate_op` then receives
the stale angle. The first differing `intcos` input is279 original versus18016
native, following identical `intatan(-60,-21)` results. All captured identical
math operands have matching results. This is an original automatic-storage
lifetime dependency; no zero-initialisation or guessed persistence was added.
The concrete missing evidence is the write history/lifetime of those original
stack words between calls. GAMESTATE and the current operand captures do not
establish that history. A future bounded stack observation is needed to justify
a faithful host representation. No third original replay was captured.

Zak sound also differs at its first playback target: elapsed267 versus12044.
Its state difference prevents a claim about subsequent sound equivalence.

## Timings and the deferred corpus test

Measurements below include startup/menu work and observation overhead. A
simulation-step denominator includes both epochs. Original `wall_us` milestones
use the observer's steady clock; process wall time additionally includes a small
startup/exit interval outside that clock. It cannot be split accurately into
launch versus exit after the fact. Original process time, packing, native process
time and comparison are measured separately. Native times are instrumented
manual-clock runs, with validation work sharing the machine during this pilot.

Both permitted single-file captures completed; no subset or full set was run.

| Replay | Input frames / recorded simulation steps | DOSBox process | Original packing | Native playback | Comparison | Packed original / native |
|---|---:|---:|---:|---:|---:|---:|
| DEFAULT (PMIN, HELL5 embedded track) | 10646 / 21292 | 494.284s | 11.984s | 193.731s | 9.855s | 44.48 / 40.98 MB |
| ZCT053_28ALANCHUYTRIX (COUN, ZCT52 embedded track) | 11989 / 23978 | 560.514s | 8.628s | 132.983s | 12.361s | 43.17 / 23.55 MB |

| Replay | Process ms/step | Packing ms/step | Original process+packing ms/step | Native ms/step | Compare ms/step |
|---|---:|---:|---:|---:|---:|
| DEFAULT | 23.215 | 0.563 | 23.778 | 9.099 | 0.463 |
| Zak | 23.376 | 0.360 | 23.736 | 5.546 | 0.516 |

Original observer milestones, in seconds from observer initialisation:

| Replay | Original image attached | Main menu | First state | Initial reconstruction completed | First playback state | Last playback state |
|---|---:|---:|---:|---:|---:|---:|
| DEFAULT | 0.039 | 1.262 | 2.767 | 36.573 | 37.041 | 493.616 |
| Zak | 0.032 | 1.247 | 2.758 | 20.759 | 21.240 | 559.980 |

The remaining process startup/exit interval is0.668s /0.535s respectively.
Packing times exclude the idle time between pilot stages; original process plus
packing totals506.268s /569.143s. These are sums of measured stage wall times,
not elapsed conversation time. Zak's outer capture call, including input setup
and sealing, was565.434s before packing. The raw original directories peaked at
1.376 /1.323 GB. DEFAULT native compression took10.875s, Zak4.583s; those are
storage costs outside playback and comparison. The two final comparison reports
are5.94 /6.70 MB. Thus the two current original/native/report sets occupy about
165 MB; the pre-fix DEFAULT baseline is additional retained diagnostic evidence.

Using the compatible inventory's mean1798 input frames per replay, and the
combined measured per-step costs, a roughly60-replay diagnostic run projects to
**85min original process+packing, 26min native playback, 1.8min comparison**.
For **1123 community replays**, the same mean-length extrapolation is about
**26.6h original, 8.1h native, 33min comparison**. These are planning estimates,
not measurements or guarantees; the 15 variant-header files are not yet runnable
by this tool. Driver, track and collision paths affect native time noticeably.

Keeping the current full-state/boundary references, compressed native diagnostics
and per-step comparison reports projects to **about15 GB left in `build/` by a
full capture**, approximately8 GB of that original references. Capture/packing
adds roughly1.5 GB transient headroom when files are processed sequentially;
the reusable observer build is another0.33 GB. Keeping all raw JSONL instead
would be much larger. These estimates exclude pre-existing build/tool caches and
the user's immutable local replay/asset inputs. No full-corpus disk was allocated.

The later test should always run DEFAULT, skip absent community data with a
clear message, and use a fixed roughly60-file manifest: alternate selected races
across all seasons, at least three per stock car, the longest compatible replay,
and opponent coverage. Resolve overlapping requirements deterministically from
race/car metadata plus a stable filename/hash ordering. Larger runs require an
explicit separate sweep command. Small original-only digest fixtures may go
under `tests/fixtures/`; larger sealed references stay local and are regenerated
one file at a time with the command above. The 15 header variants must be handled
explicitly before calling this a full1,123-file community test.

Current diagnostic tracing is not a sub-minute corpus runner. The future design
needs native streaming digests and bounded failure captures, rather than writing
all raw boundaries, and measured simulation throughput. Any route that bypasses
rendering/UI work must first establish that it preserves the simulation and sound
contract. The under-one-minute target remains unproven; this pilot does not add a
slow corpus test and label it fast.
