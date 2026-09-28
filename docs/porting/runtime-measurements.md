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

## PIT and display rate

`timer_setup_interrupt` loads divisor `0x2E9C` (11,932), writes `0xB6` to PIT control port `0x43`, then writes low/high divisor bytes `0x9C`, `0x2E` to port `0x40` (`asm/timer_setup_interrupt.ASM:7-13,41-72`). **The observed `0xB6` control word selects PIT channel 2, low/high-byte access, mode 3; the following port `0x40` data writes address channel 0.** The game therefore does not explicitly set channel 0's control word in this sequence. Its effective mode and rate depend on the earlier BIOS/emulator state.

At a conventional 1,193,182 Hz PIT input, 11,932 counts correspond to 99.9985 Hz in mode 3. A scratch harness using the game's exact `0xB6` plus channel-0 data sequence observed 93 INT 1Ch callbacks during a nominal one-second INT 15h wait. A control harness that explicitly wrote `0x36` for channel 0 and the same divisor observed 98 callbacks and a DOSBox-X PIT log of 99.9985 Hz. These callback counts are approximate. They support an approximately 100 Hz interpretation, but they do not prove the live game's actual channel-0 mode or rate.

The setup call is seen once after mode 13h at startup; these traces show no menu/race reprogramming. The exit helper restores the saved INT 8 vector and writes zero twice to port `0x40` (`asm/audio_stop_unk.ASM:25-46`). The INT 8 source divides callbacks and conditionally chains the saved vector, but BIOS tick counts and chain counts were not sampled. The emulator-reported **70.086 Hz** is display refresh, not a measured game render or simulation cadence.

## Explicitly unmeasured

- DOSBox-X `io=debug` emitted no individual guest IN/OUT records in a known port probe or in the game trace. Actual game writes/reads on PIT ports `0x40?0x43`, palette DAC ports `0x3C8/0x3C9`, and retrace status port `0x3DA` were not counted.
- Channel 0's inherited control word, live PIT rate, BIOS INT 8 chaining rate/count and timer callback totals are unknown. Runtime IVT snapshots and keyboard IRQ counts were not captured.
- The game can poll `0x3DA` during startup randomization; its read count and any ongoing pacing effect are unknown. The palette source uses BIOS INT 10h `AX=1012h`; BIOS DAC I/O was not visible in the trace.
- Startup code selects `rate_frame` as 10 or 20 from a timing benchmark, but the selected runtime value was not recorded. Actual render count, simulation-step count, catch-up batches and replay timing were not measured.
- Whether `AD15.DRV` selects `audio_driver_mode`, the effective audio callback cadence, and the driver's runtime port traffic remain unknown.
- The input-scheduled run reached the live setup path, but its exact entry into active simulation and the number of active steps were not instrumented. The old vector-snapshot prototype produced an empty trace.

## Evidence and reproduction

Static anchors: `asm/timer_setup_interrupt.ASM:7-13,41-72`, `asm/timer_intr_group.ASM`, `asm/sub_303BA.ASM`, `asm/video_get_status.ASM:3-11`, `asm/video_set_palette.ASM:12-24`, `src/obj_seg031.c:160-267`, `src/obj_seg000.c:574-607,879-945`, and `src/obj_seg005.c:617-674,690-800`.

P3b raw traces: `build/workers/P3b/logs/game-filelog.log`, `game-keyseq3.log`, `pit-measure.log`, and `pit36-valid.log`; the methodology and negative logging result are in `build/workers/P3b/REPORT.md`. The reproducible no-input probe uses the tracked script `tools/porting/run_runtime_probe.ps1`; it copies `assets/` into a fresh `build/porting/runtime-probe/<run>/game` directory, writes a DOSBox-X config and log beside it, waits up to the selected duration, and stops only the DOSBox-X process it started. Run from the repository root:

```powershell
.	ools\portingun_runtime_probe.ps1 -Seconds 110
```

The run needs the ignored original `assets/` inputs and the local DOSBox-X executable (default `C:	ools\dosbox-x\dosbox-x.exe`). It does not alter those inputs. The scheduled-key live-setup path remains a P3b scratch experiment; its TSR and logs are not a general-purpose measured runtime harness.
