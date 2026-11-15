# Port-layer race comparison by game step

Step 8 closes the two visual race defects from step 7. This remains a race-first
slice, with **open sound content differences**. Expectations come only from sealed
original MCGA/DOSBox-X runs. No proof-side source, original asset or lock changed.

## Original-derived results

The shared script covers startup, main-menu race choice, manual protection,
throttle, left/right steering, A/Z presses and braking through 200 active frames,
with AD15 and PC15. A/Z presses do not demonstrate manual gears; no crash occurs.
Original stops at IRQ 1314/1317; port at 1262. IRQ equality is outside this contract.

| Stream | AD15 | PC15 | Scope |
|---|---:|---:|---|
| Consumed keyboard bytes | 42/42 | 42/42 | Exact ordered raw input |
| Active GAMESTATE | 200/200 | 200/200 | All 1120 bytes, by frame |
| Core active call groups | 400/400 | 400/400 | Ordered bounded simulation/render calls |
| Completed indexed frames | 200/200 | 200/200 | All 64000 indices and 768 palette bytes |
| Intermediate frame groups, frames 2..200 | 199/199 | 199/199 | Frame 1 partial blits remain different |
| Engine target records | 200/200 | 200/200 | Nine DOS-width arguments, consumption order |
| Active per-entry/voice/resource sound sequences | 8/9 | 10/11 | Pitch entry 24h differs; cross-entry order not claimed |

`completed-race-{ad15,pc15}.json` retain original hashes, states, sparse completed
frames and sound observations, including failures. Tests assert the agreeing
subsets above. Old references remain as historical observations. Full comparison
still exits nonzero for startup, partial frame 1 and sound; no image search or
resynchronisation grants acceptance.

## Fixed: initial update and dashboard publication

The first scheduling difference was an injected wait at `run_game`'s first outer
iteration (`port/build.py`): it yielded before the original's initial paint,
admitting one extra inactive `movesim`/render. The first iteration now paints
before that idle wait. No inactive simulation call remains in this race capture.
This is a step-2/3 adaptation defect.

That correction alone did **not** fix the 3311 dashboard indices. Graphics capture
showed identical world blit source pixels, followed on the port by
`movcfxya(gear_base_sprite,256,144)` restoring the gauges **after** `movcf` had
published an immutable host snapshot. At that partial blit the corner hash was
`06fc73eaad306a25`, then the restore made it `0c2b8ae6d099bda3`, matching original
VRAM. The first differing comparable observation was therefore the publication,
not a wrong bitmap copy. Original ISR/idle progress had already caused a dashboard
restoration before its corresponding partial blit.

The generated `dodash(2)` wrapper now publishes `race_frame_complete` after all
dashboard work. The original observer snapshots the matching original `dodash`
RETF. All 200 completed frames now agree, including frame 1; intermediate blits
remain available as diagnostics. This fixes the older snapshot/presentation
boundary, without forcing the original's CPU-dependent idle-render count.

## Fixed: the y=9 raster calls

At frame 5, `drawpolypt` call 48 supplied the same color 15, four vertices
`(-26,9),(-26,-20),(266,-20),(266,9)`, clip `[0,320,9,130,320]`, and initial row
on both sides. Original wrote nothing; `port/sprite.c::raster_polygon` filled y=9.
The shared polygon tail in locked `lib/whlrast.ASM` subtracts its clamped y bounds
and takes `JLE` **before** adding one to the scan count. A non-flat polygon clipped
to one row draws nothing. True flat/vertical primitives take the earlier line
fallback and still draw.

The C raster now follows that rule. Capture finds 1138 matching polygon calls,
20 matching outer line calls and 29 matching patterned calls through frame 22,
with no differing output for identical observed inputs. All former y=9 frame
differences disappear. `race-horizon.json` supplies 27 real original argument sets
from frames 5,6,18..22 to the machine-code diffharness and production C; tests
compare all 64000 pixels and reproduce the captured original row. This was an
older C replacement defect, independent of polygon-storage/transform adapters.

## Open: sound smoothing depends on ISR prehistory

Engine bind/initial pitch/note/volume requests now start in active frame 1 on both
sides. After removing render-frame labels, every active ordered driver sequence
except engine pitch 24h agrees. The differing pitch counts are AD15 621/627 and
PC15 699/703 (original/port). First post-start pitch is 44/10 for AD15, 82/19 for
PC15. These are **content** differences, not merely equivalent requests stamped
in different simulation frames; the comparator retains failure and bounded
resynchronisation context.

New `engine_init`, `engine_start`, `engine_tick`, `engine_target` observations show
34 original callbacks versus one port callback between initialization and engine
start, and 36 versus five before the first target, for both drivers. The unchanged
`game/audioeng.c::audio_driver_timer` smooths `rate_avg` on those callbacks even
before its continuous voice starts. All 200 subsequent `audio_op_unk2` target
records agree exactly. This establishes different smoothing prehistory from the
step-2 clock policy (no ticks during native CPU work), not device-demand feedback.
Later pitch duplicates/tails also remain unaccepted; initial prehistory alone is
not a proof of their cause. Startup music/stop/pump groups remain CPU-time-dependent.

No arbitrary 33-tick warmup, pitch filtering or benchmark emulation was inserted.
A resolution needs an explicit engine-start policy or an original-independent
logical sound contract; tuning to this DOSBox CPU budget would hide the mismatch.
`compare_sound_events.py` compares audio-frame targets in consumption order and
each per-voice/resource/entry sequence in order, while retaining raw ticks/frame
context. This diagnostic partition does not prove cross-entry interleaving.

## Coverage and deferred work

The refreshed joined inventory has 1393 overlapping rows: 80 covered by named
finite original cases (74 agreeing, six differing), 1313 without comparable
coverage. The six differing rows are ordered sound-driver streams including
startup. Ten of eleven optional graphics contracts executed; `drawpolyp` did not.
Unmatched inputs are recorded separately and never counted as agreeing cases.
Step 7's mouse reset, one-word keyboard overwrite and INT15 C201 fixes remain
covered by their original service cases and focused tests.

Opponent selection, demonstrated manual gears/crash, replay pause/seek/save,
editor load/edit/save, file dialogs/errors and high scores need further semantic
scenario/span work exceeding a few hours. They are deferred under the requested
race-first checkpoint. Existing replay tests still pass, but are not newly
original-anchored scenario coverage. Legacy `tests/fixtures/sdl3/` recapture is
also deferred; historical provenance has not been falsely relabelled. New race
references bypass those screens and are generated only by the original packers.

## Verification

`python port/build.py build`: 38 game objects, 46 host objects, zero generated
function/data stubs. Focused original-step and 27-case horizon regressions pass;
unchanged replay playback passes. `python tools/validate.py --tests port --no-proof`:
162 tests passed, zero failed/skipped, 186.3 s wall time. `--proof-only`: PASS
HYBRID_EXACT, independent compiler PASS, raw bytes zero, 49.0 s. Commands and logs
are under `build/workers/portclock/step8/`; changed-path ownership is recorded in
`build/workers/portclock/steps8.json`. No background observer/game remains running.
