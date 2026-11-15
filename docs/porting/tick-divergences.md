# Original DOSBox-X / manual-port tick comparison

These are observations, not accepted differences or regenerated port goldens.
Profile: normal core, fixed 30000 cycles, original MCGA, AD15/PC15, rate 20,
detail 0, dirty rectangles, shared explicit seed fixture and tick script.
The [runtime oracle](runtime-oracle.md) documents capture and regeneration.
Full JSONL, original frame dictionaries and first-difference/resynchronization
reports are in `build/workers/portclock/dosbox/*-g/`. Only those final captures
use raw keyboard FIFO input, direct bounded DOS mouse input and host VRAM peeks.
Earlier probes are discarded as references.

## Results (original / port)

| Scenario | Driver | Sound requests | Indexed publications | Simulation returns | Key reads | Raw scans |
|---|---|---:|---:|---:|---:|---:|
| Startup, tick 1000 | AD15 | 10028 / 10088 | 53 / 50 | unexercised | 1 / 1 exact | 2 / 2 exact |
| Startup, tick 1000 | PC15 | 1939 / 1954 | 53 / 50 | unexercised | 1 / 1 exact | 2 / 2 exact |
| Car selection, tick 2200 | AD15 | 24828 / 24888 | 2847 / 3029 | unexercised | 3 / 3 exact | 6 / 6 exact |
| Car selection, tick 2200 | PC15 | 4991 / 5006 | 2875 / 3029 | unexercised | 3 / 3 exact | 6 / 6 exact |
| Driving, tick 1800 | AD15 | 9921 / 10077 | 618 / 635 | 296 / 310 | 22 / 22, ticks differ | 43 / 43 exact |
| Driving, tick 1800 | PC15 | 4122 / 4217 | 618 / 635 | 296 / 310 | 22 / 22, ticks differ | 43 / 43 exact |
| Replay pause/seek, tick 2550 | AD15 | 11696 / 11866 | 5443 / 3475 | 310 / 318 | 19 / 19, ticks differ | 37 / 37 exact |
| Replay pause/seek, tick 2550 | PC15 | 5906 / 6011 | 5450 / 3475 | 310 / 318 | 19 / 19, ticks differ | 37 / 37 exact |

No complete scenario agrees tick for tick. Empty simulation streams do not pass.
The first AD15 sound difference is request 15: original callback +30h at tick 3,
port foreground +30h at tick 2. PC15 differs at request 1: foreground reset +1Eh
at original tick 1 versus port tick 0. AD15's first nonzero indexed transition
completion is tick 13 versus 6. PC15's first palette publication is tick 1 versus 0.
All later scenarios inherit these startup differences.

## Divergences and attribution

1. **Startup epoch and sound interleaving.** Original initialization/benchmark/
   seed stirring consumes IRQ ticks; native ordinary computation consumes none.
   Original benchmark join is tick 4; the port removes this benchmark by explicit
   configuration (`port/build.py`, startup adapter, plan step 4). The handshake
   (`port/timer.c::port_guest_yield`, steps 2/3) only admits a tick at suspensions.
   This explains an architectural timing difference; it is not evidence that the
   integer counter or 5:1 divider is wrong. Restoring a machine benchmark loop
   would contradict the chosen modern-port behavior. A common post-startup epoch
   or independently specified DOSBox program-read clock remains an open policy.
2. **Intro transition pacing.** AD15's first four completions are original ticks
   13,14,14,15 versus port 6,10,14,18; PC15 gives 16,17,18,18 versus the same port
   sequence. The actual indexed images/palettes match in order. The older port
   `port/video.c::port_video_transition_advance` uses the synthetic 9M retired
   instruction profile in `port/transition_work.h`; steps 2/3 quantize those
   deadlines to grants. This is a presentation-profile mismatch, not a demonstrated
   pixel-renderer defect. The first 50 startup images agree in content/order;
   original gets one extra menu animation triplet before the fixed stop tick.
3. **Car UI cadence.** After transition, raw-tick render counts and poses differ;
   2402 AD15 publication contents resynchronize exactly, but complete sequences
   do not. `port/build.py::adapt_host_idle_waits` changes `mykey` to suspend, with
   a car transform/draw exception; original `game/frontend.c` samples `elticks`
   while CPU drawing proceeds. This is a step-3 scheduling candidate mixed with
   older presentation cost assumptions. A pose difference alone does not identify
   wrong geometry; no arbitrary delay or rendering fix was applied.
4. **Older mouse reset behavior.** Before scripted mouse movement, original
   car/protection polling sees (160,100); the port initializes (0,0), and
   `port/platform.c::int86`, INT 33h reset, returns presence/button count without
   resetting coordinates. `port/input.c::port_input_init` owns that initial state.
   This predates steps 1–5. The observer logs consumed mouse coordinates separately
   so this cannot be confused with missing script delivery. Leave for the next round.
5. **Race activation and subsequent controls.** Original consumes the queued
   throttle key at tick 683; port at 649. First active state is 684 versus 650;
   port additionally traces an inactive initial update at 649. Active frames 1–84
   have identical complete 1120-byte GAMESTATE hashes by frame number. At frame 85,
   original tick 1104 has already seen throttle release at 1100, while port tick
   1070 has not: travel distance 601812 versus 601955 and Z 402132 versus 402145.
   This is input hitting different simulation frames after the epoch drift, not
   evidence of a physics arithmetic error. Foreground loading/initialization and
   the new suspension schedule must be separated before assigning a code defect.
6. **Replay recovery and seek completion.** After rewind, both drivers agree on
   every complete state at ticks 1851,1856,...,2096 (frames 1–50), and the first
   25 reconstructed states at tick 2240. Later seek work crosses an original IRQ
   while native finite reconstruction remains inside one granted tick
   (`port/build.py::adapt_host_idle_waits`, `doreplay`, step 3). Final replay state
   is frame 89 at original tick 2546 versus frame 90 at port tick 2550. The resumed
   schedule is one tick apart. This needs an explicit semantic completion rule,
   not a sleep calibrated to DOSBox cycles.
7. **Sound beyond the first mismatch.** Bounded resynchronization retains extra/
   missing foreground volume updates (e.g. AD15 +12h during the tick-150 fade),
   callback-order gaps and their context. `lib/audioply.c::update_audio_voice_state`
   and the foreground fade callers execute under different startup/UI timelines.
   Requests alone do not isolate a defect in `port/audio.c::port_audio_advance_tick`.
   No original driver return/query failure or device-sink defect was established
   in these workloads. Keep sound-sink attribution unresolved, not silently fixed.

## What is asserted and what remains open

`tests/test_sdl3_original_ticks.py` asserts exact raw scan delivery for all eight
cases, full startup/selection key consumption, race/replay keys through protection,
AD15's first 15 driver requests, PC15's first initialization return, an indexed
transition checkpoint at AD15 tick 14 / PC15 tick 18, and the 75 exact replay states
per driver described above. Sparse indexed payloads and hashes come only from
sealed original DOSBox-X captures; stored whole-stream hashes include divergences.

The practical blocker to complete absolute-tick equivalence is clock admission:
DOSBox PIT time advances through foreground CPU work, while the port grants time
only at semantic suspensions. Fixed cycles make this measurable, not equivalent.
Do not reintroduce busy waits or generate port expectations to hide it. Next work
should specify/ground the shared original observation clock or choose explicit
semantic epochs, then isolate the remaining older adapter differences. No
unproven change to steps 1–5 or original/proof sources was made in this pass.
