# Stunts original-game observer

The reference CPU, PIT, PIC, BIOS and DOS are DOSBox-X 2026.08.31. This is a
private copy of the installed SimAnt acceptance source, **without** SimAnt's
ideal-clock patches. The normal CPU core executes every game instruction.
Installed sources, objects, executables and both repositories' `.git` stay read-only.

```powershell
python tools/porting/dosbox_observer/build.py
```

Prerequisites are the installed acceptance source/object tree named by `BASE`
in `build.py` and Visual Studio 2022 Community. `source-pins.json` checks the
CPU/acceptance/PIT/PIC/input/VRAM sources. The builder copies `src` and `include`
under its output, checks readable patches against exact replacement contexts,
recompiles `dosbox.cpp`, `mouse.cpp` and `core_normal.cpp`, then links unchanged installed objects.
The default output is the reusable `build/porting/dosbox-observer` directory; use
`--rebuild` there for an incremental rebuild instead of creating another attempt directory.
`observer-build.json` hashes every reused object and all observer inputs.
`--rebuild` reuses only the private source/output and its incremental LTCG cache;
stop the observer before rebuilding. It never writes installed compiler PDBs.

`stunts_observer.inc` attaches at the locked MZ entry signature, excludes its
relocated immediate, and derives the runtime load segment. `original_tick_oracle.py`
resolves function entries using the accepted code map and names registry, verifies
locked instruction anchors, and saves bindings with entry bytes and call witnesses.
Absolute-looking constants in the include are **load offsets**, added to the
observed MZ load base; they are not fixed DOSBox physical addresses.

Raw tick N increments at load offset `20329h`, the original INT 8 handler's CLI.
Input is delivered there before the handler runs. Raw Set-1 bytes enter DOSBox's
keyboard FIFO (`KEYBOARD_AddBuffer`), not its physical-key/typematic API. Mouse
coordinates use the same truncation to inclusive logical 320x200 as the port,
then the game's `mousehorscale`; the tiny `mouse.cpp.patch` changes only the
emulated INT 33h polling device state, with its current bounds. It bypasses
host-pointer capture and PS/2 routing. This scope does not test mouse callbacks.
Actual keyboard port-60 reads and the game's INT 16h dequeue results are logged
separately from delivery. The original INT 16h shim returns with RETF 2.

Hooks observe far-call entry and the matching SS:SP immediately before RETF.
Driver returns keep AX and the original raw argument words. Eligible graphics
routine completions capture indexed pixels and DAC6; repeated full payloads are
deduplicated. Host VRAM uses the pinned compatible chain-4 mapping and aborts
on unsupported modes/banks. It never reads the guest VGA aperture, changes its
latches, charges guest cycles, or samples the host window. GAMESTATE is read at
the accepted `movesim` return. A diagnostic mouse-read row uses `mousecursor`.

Guest-memory writes are explicit shared controls: fast-machine
settings at the benchmark join, fixed stir counts at the seedrandom boundary,
and the existing seed fixture's CRT/Kevin/counter values at frontend seed return.
The actual original benchmark and PRNG loops still execute. No original code or
driver bytes are patched, no instructions skipped, no tick rebasing performed.
The fixture is a chosen input setting here, not independent expected behavior.
The runtime oracle document gives regeneration, comparisons and their limits.

For semantic input, `stunts_boundary.inc` adds bounded routine/INT 15h/33h
request/result observations, simulation/render scopes and input-slot/key-ordinal
delivery. `core_normal.cpp.patch` yields only at an initiating semantic input
frame while queued bytes await the real PIC/INT9; normal PIT time continues.
This does not skip original instructions or replace its clock. See the runtime
oracle's game-step section for the race contract and explicit coverage gaps.

Step 9 also supports scoped v2 key visits and explicit completed-call checkpoints.
Its optional `EVAL_SCORE` control writes the locked `_race_stats` finish/penalty
words only at `evaluation` entry for a declared qualifying-entry UI fixture.
This is logged and is not a claim that the scripted crash earned that score.
See runtime-oracle.md for phases, exact spans, regeneration and open comparisons.
