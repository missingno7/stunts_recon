# Runtime measurements: original MCGA image

## Capture scope

The measurements below came from DOSBox-X 2022.09.0 at `cycles=fixed 30000`, with original game assets copied to a scratch game directory. The no-input trace enabled file, INT 10h, VGA, PIT, I/O and keyboard logging. Logger timestamps are approximate milliseconds at the configured cycle rate; event order is stronger evidence than sub-second precision. Raw captures are retained locally under `build/workers/P3b/logs/` and are not committed.

| Approximate time | Observation | What it establishes |
|---:|---|---|
| 0.419 s | INT 10h mode 13h; VGA log reports 70.086 Hz and a 320?400 guest mode | Display setup and emulator-reported refresh. |
| 0.422?0.502 s | `sdmain.PVS`, then `sdtitl.PVS` opened | Startup and title resource flow. |
| 30.05?37.33 s | Credit resources, then `sdmsel.PVS` | Menu entry on the idle path. |
| 98.24?98.30 s | `default.rpl`, `game.pre`, `sdgame2.PVS`, `city.PVS`, `game1.p3s`, `game2.p3s` opened | No-input run entered default replay playback and loaded its course/world resources. |

That run is not a live race measurement. `run_menu` forces selection zero and Enter after `unused_count > 6000`; with `TEDIT.PRE` present, the top-level path enters replay setup. A separate INT 1Ch-scheduled keyboard run pressed Enter/Escape at selected counts, opened `tedit` resources and track files (`ALLJUMPS.TRK`, `DEFAULT.TRK`), then `sdgame.PVS`, `game.pre`, car resources and an `opp1.*` search. It did not open `DEFAULT.RPL`, consistent with reaching track/live-game setup. It did not count physics steps or rendered frames.

## Accepted-source timing derivation (2026-09-29)

### PIT and INT 8

The accepted default `timer_setup_interrupt` path selects divisor `0x2E9C` (11,932), initializes a five-IRQ countdown, enables the saved-vector chain gate, writes `0xB6` to port `0x43`, then writes `0x9C`, `0x2E` to port `0x40` (`asm/timer_video_interrupt_runtime.ASM:223-230,261-289`). PIT command `0xB6` selects channel 2, low/high-byte access, mode 3. The following data writes select channel 0. This sequence therefore **does not program channel 0's mode/access control word**; channel 0 inherits whatever state the BIOS/host established earlier.

Under the conventional BIOS state (channel 0 already in mode 3, low/high access) and a 1,193,182 Hz PIT input, divisor 11,932 predicts `1,193,182 / 11,932 = 99.9985 Hz`. Treat this as a conditional static model, not a measured fact. The previous scratch harness using these exact control/data writes observed 93 INT 1Ch callbacks during a nominal INT 15h second; the explicit channel-0 `0x36` control harness observed 98. Those coarse harness results support the approximate 100 Hz model but do not measure the live game or prove its inherited channel-0 state. Keep the original uncertainty until an in-game trace confirms it.

The installed game INT 8 handler processes the five-count divider on every raw IRQ. With default setup it calls the saved INT 8 vector on each fifth IRQ while `_timer_sound_countdown_enabled` remains nonzero; it does not chain on the intervening four IRQs (`asm/timer_video_interrupt_runtime.ASM:474-520,559-567`). Thus the conditional prediction is approximately 20 saved-vector calls/s for approximately 100 raw IRQ/s. Registered game timer callbacks are scanned at the raw IRQ cadence, subject to callback/re-entry guards. Teardown restores the saved INT 8 vector and writes zero twice to port `0x40`; it does not restore a captured prior divisor (`asm/timer_video_interrupt_runtime.ASM:320-358`).

### Simulation clock, replay, and rendering

The accepted startup benchmark selects `frm_rate2=20` when `timerdelta3 < 75`, otherwise 10 (`src/obj_seg031.c:314-321`). `initialize_game_state` sets `frmcs_time=100/rate_frame` (`src/obj_seg001_complete.c:1631`). `frame_callback` runs from the timer callback, counts raw callback ticks, and advances `g_clocks` when its countdown reaches zero (`src/obj_seg005.c:1104-1164`). If raw IRQ cadence is approximately 100 Hz, this predicts a target of 20 or 10 simulation-clock events/s; the actual runtime-selected rate and callback frequency were not logged.

`gm_playmode` 0 is the ordinary live/input path; each target event advances its input/replay timeline. `gm_playmode` 1 is selected by the no-recorded-replay setup branch (`src/obj_seg005.c:701-715`) and `replay_unk2` records zero input for that mode (`src/obj_seg005.c:1200-1202`). `gm_playmode` 2 is the recorded-replay path (`src/obj_seg005.c:717-730`), where the callback advances recorded time as described below. The main loop calls `update_gamestate()` until `core.game_frame == tmr2`, then takes its render/present path. Multiple queued simulation steps can be consumed before one render pass; live interactive input mode also skips a duplicate frame (`src/obj_seg005.c:733-745,777-782,1187-1240`). Consequently source inspection establishes the pacing and catch-up relationship but does not supply actual render totals, simulation totals, or missed/duplicate frame counts.

Replay has additional internal branches: `g_rplmodui==2` calls `replay_unk2(0)` only on every second target-clock event; `g_rplmodui==3` calls it in the case and again after the switch, advancing up to two recorded steps per event (`src/obj_seg005.c:1144-1160,1187-1195`). Replay UI case 2 assigns value 3 at line 2522; no direct C assignment of value 2 was found in the inspected sources, so its reachability and display label remain unproven. These are conditional source relationships, not measured user-visible playback rates.

### Retrace polling

`video_get_status` reads port `0x3DA`, masks bit 3, and returns that status (`asm/video_get_status.ASM:5-9`). Accepted calls to `random_wait` occur after startup timing setup and after track setup (`src/obj_seg031.c:342,351-359; src/obj_seg000.c:602`). The helper polls until the bit changes or its 12,000-iteration limit is reached, then advances random generators. No per-frame retrace wait is present on the inspected game loop. This does not say the renderer or BIOS never accesses VGA status by another path; the direct helper itself is setup polling rather than frame pacing.

### Keyboard and audio callbacks

Keyboard setup saves and replaces INT 9 and INT 16; the INT 9 handler reads the scan code, acknowledges the controller/PIC, and returns with IRET without chaining to the saved INT 9 handler. Its exit handler restores the saved vectors (`asm/keyboard_interrupt_runtime.ASM:73-110,157-187,113-154`). Runtime vector values and interrupt counts remain unmeasured.

The audio-driver entry result sets `audio_driver_mode=1` only when it exceeds `0x7F`; the accepted loader registers `audiodriver_timer` (`src/obj_seg027.c:515-525`). The `AD15.DRV` entry returns `0x0A` on successful OPL detection (asset SHA-256 `E32B44C8EE6BF2046FC93359C225F3D99E559C94926594F6E3468E1C317AFE26`), leaving the mode zero. `audiodriver_timer` is the driver event-processing callback: it is registered once with the timer service, has no divider, and runs on each raw timer callback while its binary and locks permit (`src/obj_seg028.c:113-131`). A separate `audio_driver_timer` pitch/rate updater is registered by `audio_add_driver_timer` (`src/obj_seg007.c:63-70`); it runs on every callback in mode 0 and skips every other callback in nonzero mode (`src/obj_seg007.c:180-227`). Thus AD15 predicts a full raw-callback driver hook and, if the separate updater is registered, a full-rate updater too. DOSBox's OPL detection and live callback counts/rates remain unmeasured.

## Dynamic evidence still required

- DOSBox-X `io=debug` emitted no individual guest IN/OUT records in a known port probe or in the game trace. Actual game writes/reads on PIT ports `0x40?0x43`, palette DAC ports `0x3C8/0x3C9`, and retrace status port `0x3DA` were not counted.
- Channel 0's inherited control word, live PIT rate, BIOS INT 8 chaining rate/count and timer callback totals are unknown. Runtime IVT snapshots and keyboard IRQ counts were not captured.
- The game can poll `0x3DA` during startup randomization; its read count and any ongoing pacing effect are unknown. The palette source uses BIOS INT 10h `AX=1012h`; BIOS DAC I/O was not visible in the trace.
- Startup code selects `rate_frame` as 10 or 20 from a timing benchmark, but the selected runtime value was not recorded. Actual render count, simulation-step count, catch-up batches and replay timing were not measured.
- Whether `AD15.DRV` selects `audio_driver_mode`, the effective audio callback cadence, and the driver's runtime port traffic remain unknown.
- The input-scheduled run reached the live setup path, but its exact entry into active simulation and the number of active steps were not instrumented. The old vector-snapshot prototype produced an empty trace.

## Evidence and reproduction

Static anchors: `asm/timer_setup_interrupt.ASM:7-13,41-72`, `asm/timer_intr_group.ASM`, `asm/sub_303BA.ASM`, `asm/video_get_status.ASM:3-11`, `asm/video_set_palette.ASM:12-24`, `src/obj_seg031.c:160-267`, `src/obj_seg000.c:574-607,879-945`, and `src/obj_seg005.c:617-674,690-800`.

P3b raw traces: `build/workers/P3b/logs/game-filelog.log`, `game-keyseq3.log`, `pit-measure.log`, and `pit36-valid.log`; the methodology and negative logging result are in `build/workers/P3b/REPORT.md`. For interactive instruction/I/O and vector capture, use [the DOSBox-X debugger guide](CAPTURE.md) and `tools/porting/run_capture.ps1`; analysis is bounded by `tools/porting/analyze_capture.py` and writes to `build/porting/runtime-capture/`. The official DOSBox-X 2026.08.31 executable/archive hashes and provenance are in [CAPTURE.md](CAPTURE.md). The reproducible no-input probe uses the tracked script `tools/porting/run_runtime_probe.ps1`; it copies `assets/` into a fresh `build/porting/runtime-probe/<run>/game` directory, writes a DOSBox-X config and log beside it, waits up to the selected duration, and stops only the DOSBox-X process it started. Run from the repository root:

```powershell
.	ools\porting
un_runtime_probe.ps1 -Seconds 110
```

The run needs the ignored original `assets/` inputs and the local DOSBox-X executable (default `C:\tools\dosbox-x\dosbox-x.exe`). It does not alter those inputs. The scheduled-key live-setup path remains a P3b scratch experiment; its TSR and logs are not a general-purpose measured runtime harness.
