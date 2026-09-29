# DOSBox-X interactive capture guide

## Release provenance

The interactive debugger workflow uses the official DOSBox-X 2026.08.31 Visual Studio x64 release at `C:\tools\dosbox-x-2026.08.31-vsbuild-win64\bin\x64\Release\dosbox-x.exe`. The release archive SHA-256 is `4a22d27fd49b81e1321494311f9cccb2b617a9be35367b60d83e0cf2bca39c8b`; the executable SHA-256 is `3a741ab543b1028946848db9fb894fa93371170f551b9b25dcb7f5d3c3ce7e65`. The download and 154-file inventory are cataloged in `C:\tools\CATALOG.md` and `C:\tools\catalog.json`.

## Scope

This workflow runs the original game files from a newly copied `build/porting/runtime-capture/run-*/game` directory. It never mounts or modifies `assets/`. DOSBox-X's ordinary `io=debug` logger did not emit guest port instructions in the earlier probe; this path uses the official release's CPU instruction logger, which records the instruction, CS:IP, and pre-instruction registers. That supports counting `IN`/`OUT` instructions and reconstructing OUT data where registers show it. It is not a cycle-accurate timestamp stream, and an `IN` value is not available from its pre-instruction row.

## Run

From the repository root in an interactive Windows PowerShell window:

```powershell
.\tools\porting\run_capture.ps1
```

The script copies the full asset tree to a fresh capture session folder and starts `C:\tools\dosbox-x-2026.08.31-vsbuild-win64\bin\x64\Release\dosbox-x.exe` in the foreground with `-break-start -console`. The foreground process remains visible and the script waits until you close DOSBox-X; it does not detach a process. `LOAD.EXE /U MCGA /SSB` launches from the scratch mount.

## Debugger sequence

1. At the startup debugger prompt, enter `INTVEC vectors-at-start.ivt`, then `G` to resume.
2. Let startup reach the title. For a live race sample, navigate the original menus into an active race; for replay, start playback and note the selected internal playback option if known.
3. Press `Alt+Pause` to break into the debugger. This mapper shortcut and `INTVEC`/`LOG` commands are documented by the official release's `README.debugger`.
4. Enter `INTVEC vectors-live.ivt` for the installed vectors. Enter `LOG 40000` for a bounded 0x40000-instruction (262,144 instruction) sample. DOSBox-X writes `LOGCPU.TXT` in the session directory and stops back in the debugger when the count is reached.
5. Resume with `G` or continue interacting. To snapshot restored vectors, break again at DOS after leaving the game and enter `INTVEC vectors-after-exit.ivt`.
6. Close DOSBox-X normally. The script returns to PowerShell; the whole session, including `dosbox.log`, `LOGCPU.TXT`, copied game, and vector snapshots, remains below `build/porting/runtime-capture/run-*`.

Each session's config records VGA, INT 10h, PIT, PIC, keyboard, I/O, file, and INT 21h logs. The CPU log is the source for guest `IN`/`OUT` counts; `INTVEC` files compare installed/restored vectors. Capture separate sessions for title/idle, live race, and replay if one bounded CPU log does not cover each state. Preserve the raw files. Analyze a finished session with:

```powershell
python .\tools\porting\analyze_capture.py `
  --trace .\build\porting\runtime-capture\run-<session>\LOGCPU.TXT `
  --before .\build\porting\runtime-capture\run-<session>\vectors-at-start.ivt `
  --after .\build\porting\runtime-capture\run-<session>\vectors-live.ivt `
  --entry game-int8=SEG:OFF --entry game-int9=SEG:OFF `
  --entry saved-int8=SEG:OFF --out .\build\porting\runtime-capture\run-<session>\analysis.json
```

Replace each `SEG:OFF` with the corresponding hexadecimal address from the `INTVEC` snapshots; omit an entry not needed for that sample. The analyzer writes a port event CSV and vector text diff alongside the JSON summary. It records port hits for `40h`-`43h`, `3DAh`, and `3C8h`/`3C9h`, plus observed OUT byte values. `--entry` counts executions beginning at a supplied vector handler address; compare `game-int8` with `saved-int8` to inspect the five-to-one chain ratio within that sample, and `game-int9`/`saved-int9` to check keyboard chaining. These are counts within the bounded instruction window, not rates without an independent elapsed-time source. The exact simulation/render count still requires identifying/counting the matching accepted code paths in a sufficiently long trace; keep it distinct from source-only predictions.

## Calibration limits

- The game sets PIT channel 0's data but the `0xB6` control word selects channel 2. An observed 40h write alone cannot prove channel 0's effective mode; correlate the host's inherited control state or use a debugger I/O breakpoint/readback method that observes it.
- `INTVEC` captures vectors at snapshots. Use before/start, installed/in-race, and after/exit snapshots to establish changes; a single snapshot has no history.
- A bounded `LOG` sample contains instruction order but no wall-clock time. To report rates, combine it with a separately calibrated interval/counter source and disclose that timing method.
- DOSBox-X's heavy log is a finite instruction buffer. `LOG` with explicit count is preferred for reproducible bounded windows. `HEAVYLOG` is not an unlimited trace.
- The official release is cataloged and hash-verified. No guest-game trace was captured in the integration pass; retain the evidence status as pending until a user-run session is analyzed.
