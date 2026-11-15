# Port-layer race comparison by game step

This is step 7's race-first slice. Expectations are sealed original MCGA/DOSBox-X
observations only. Source/proof trees and assets are unchanged. The
[inventory](port-layer.md) retains untested work explicitly.

## Measured results

The shared semantic script covers startup, main-menu race choice, manual
protection entry, throttle, left/right steering, A/Z presses and braking through
200 active simulation frames, with both original drivers. It does not establish
manual-transmission gear changes or a crash. Original stops at IRQ 1314/1317
(AD15/PC15); port at 1263. IRQ equality is not this contract.

| Stream | AD15 | PC15 | Claim |
|---|---:|---:|---|
| Consumed raw keyboard bytes | 42/42 exact | 42/42 exact | Ordered input, no typematic synthesis |
| Active GAMESTATE | 200/200 exact | 200/200 exact | All 1120 bytes, by frame |
| Active bounded call groups | 400/400 exact | 400/400 exact | Ordered simulation/render calls, memory and results |
| Active indexed frame groups | 192/200 exact | 192/200 exact | Palette agrees; eight pixel differences |
| Sound groups including startup | 22 equal; stream differs | 9 equal; stream differs | No whole-sound PASS |

All 16 watched routine/service contracts agree on comparable captured inputs
after fixes. Unmatched startup inputs/call counts remain visible. Twelve far-entry
offsets execute; eight ordered driver streams differ. No differing defined return
was found for an identical captured driver request; this finite call-set check
does not prove private driver state or request timing.

Twenty-four captured matrix sets (12 each `mat_vec`/`transmult`) additionally run
in the original-machine diffharness and production C: all 64 surrounding words
compare, and isolated-original results match the original capture. Tests assert
200 states, 400 active call groups, input bytes and all indexed publications at
frames 23..200. Earlier failures stay in the references. No image search grants PASS.

## Bugs fixed

1. `port/platform.c::int86`, INT 33h reset, reported presence but kept (0,0).
   It now restores (160,100), default bounds and released buttons. Original
   service observations and a dirty-state reset probe verify scaled (320,100).
2. `port/input.c::enqueue_bios_key` kept 32 words and dropped new overflow keys.
   Original `lib/kbint.ASM` initializes `kb_intr_data3` to two **bytes**: one word,
   overwriting the old key. At input slot 120 original returns `a` (97); the old
   port returned Up (4800h), then `a`. Capacity/overwrite now agree. Optional test
   answer text supplies the next character after consumption. Editor/menu tests
   allow a poll per stroke, preserving their assertions and requested strokes.
3. `port/platform.c::int86`, INT 15h C201, was a no-op returning AX=C201/BX=0.
   It now returns the observed BIOS reset result: AH=0, preserved AL=1, BX=00AA,
   clear carry. `initmouse` ignores that result before DOS reset; no gameplay
   state change was observed from this correction.

These defects predate steps 1..5. Focused original-derived regressions are in
`tests/test_sdl3_original_steps.py`. No clock/sound adjustment was made without a
demonstrated cause.

## Open differences and attribution

- Frame 1: 3311 indices differ at x=256..319, y=144..199; palette identical.
  Port shows a manual-shift graphic where original shows dashboard gauges. Port
  executes an extra inactive update/render before the first active frame; original
  capture has none. Candidate paths: `port/build.py::adapt_host_idle_waits`, native
  `run_game` overlay, dashboard copies in `port/sprite.c`. Clock admission versus
  older dashboard/buffer code is unresolved; matching GAMESTATE cannot decide it.
- Frames 5,6,18..22: 267,268,216,217,218,219,220 differing pixels, all on y=9;
  identical palettes, then exact resynchronisation. Transform/math calls agree.
  Next targets are older `port/sprite.c::raster_polygon` / line clipping and
  polygon-storage adapters in `port/game_abi.py`. No offending routine is proven.
- Startup: original has 21452 bounded foreground calls, port 20626, plus one
  inactive simulation call and 479 inactive render calls. Benchmark removal and
  CPU-work tick admission explain different schedules; no benchmark emulation or
  calibrated sleep was added to hide this.
- Sound: original frame-1 engine bind/pitch/note/volume requests already happened
  in the port's inactive frame. Callback counts and later volume/pitch inputs also
  differ (AD15 entries 12h/24h, PC15 24h). Relevant paths: `port/timer.c::port_guest_yield`,
  race waits in `port/build.py`, tick advancement in `port/audio.c`, typed dispatch
  overlay. Original ISR progress through CPU work differs from native admission;
  simulation-frame equality alone cannot align music/chunk time. No sink fault
  is established; private chunk/voice snapshots are needed to attribute the rest.

## Deferred work

The bounded race method works; the remaining ABI/span review and scenarios exceed
a few hours. Opponent selection, demonstrated gears/crash, replay pause/seek/save,
editor load/edit/save, file errors/dialogs and high scores are outside this new
slice. Existing original diffharness tests count only for their stated cases.
Legacy `tests/fixtures/sdl3/` screens have not all been recaptured; their historical
provenance remains. New race references bypass them. The obsolete input-format
parser alias and all active writers are removed; provenance is not relabelled as
a DOSBox-X capture.
