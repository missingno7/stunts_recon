# portable-sdl3 handoff (2026-10-01)

The SDL3 port is on the local `portable-sdl3` branch in `D:\Prog\stunts_recon`.
Historical sources, recipes, assets and the locked oracle stay separate from
modern adapters and generated host overlays.

## Build and run

Use i686 MinGW GCC at `C:\msys64\mingw32\bin\gcc.exe` and SDL3 3.4.16 i686 at
`C:\tools\sdl3-3.4.16-i686`. Use 64-bit Python for historical validation and
Unicorn; i686 Python is used only by the test DLL workers.

```powershell
python port/build.py build
python port/build.py run --no-build
python -m tools.porting.diffharness --assets assets
python tools/validate.py
```

The executable is `build/sdl3/stunts.exe`. Assets are copied into
`build/sdl3/runtime/assets`; writable saves use the sibling `saves` directory.
Normal runs do not enable tracing or framebuffer capture.

For an existing game installation, run `python port/build.py package`. Extract
`build/sdl3/stunts-sdl3-win32.zip` into the original Stunts 1.1 game folder and
launch `stunts-sdl3.exe`. The ZIP contains the executable, SDL DLL, licenses and
instructions; it uses the original game data already in that folder. This
layout automatically finds adjacent game files and writes into the folder's
`saves` subdirectory. It also launches from shortcuts with an unrelated working
directory and requires no Python or MinGW installation.

## Runtime configuration

First launch creates `config.json` beside the executable, independently of
the working directory. Its default is:

```json
{
  "manual_word_check": false
}
```

Set `manual_word_check` to `true` and restart to restore the original manual
question and answer dialog. The modern `security_check` entry bypass sets the
original success flag while retaining the caller's question RNG draw. Frozen
sources and the historical oracle are unchanged. Existing files, including
unknown fields, are preserved byte for byte; malformed or unavailable files
use defaults and leave any existing file intact. `--config=PATH` selects another
file. The ZIP omits `config.json`, preserving settings during upgrades.
`--test-auto-protection` still runs the original prompt and enters its answer,
even with the default config, for the existing oracle comparison fixtures.

## Crash diagnostics

Every Windows launch creates a unique `diagnostics/stunts-*` session folder
beside the executable and records build identity, paths and the exit reason
in `session.log`. An unwritable installation falls back to the local
application-data directory returned by `SDL_GetPrefPath("Stunts", "SDL3")`.
`--diagnostics-dir=PATH` selects another destination.

For detailed recording while playing, run from the game folder:

```powershell
.\stunts-sdl3.exe --debug
```

This also saves console/SDL output and the existing flushed input/GAMESTATE
trace as `trace.jsonl`. Real keyboard and mouse events, including focus-loss
releases, use the same DOS input channels as scripted runs. An explicit
`--trace=PATH` takes precedence over the default debug trace location.
Detailed traces grow while playing; normal launches keep the smaller log.

For an unhandled Windows exception or CRT abort, the report contains
`crash.txt`, a best-effort `crash.dmp`, and `stunts-crashed.exe` preserving
the exact binary and its debug information. A prestarted dedicated thread
writes the dump; the fault handler copies exception/context records and
signals it without entering SDL, trace locks or guest cleanup. DbgHelp is
loaded from the Windows system directory before gameplay. The process
terminates after reporting; this does not attempt to recover a damaged game.
Forced termination and power loss cannot produce this crash report.

After a failure, ZIP the newest session folder and provide it for analysis.
For a handled game error, `session.log` records the nonzero exit and reason.

## Idle CPU use

The native guest previously retained the DOS polling loops at full host speed.
Thread sampling found `run_menu -> input_checking -> get_kb_or_joy_flags`
as the idle-menu hotspot; the race loop also spun after catching its timer
position. Modern overlays now yield at stable UI backedges (main, track,
car and opponent menus, modal dialogs, line entry and the track editor),
paused replay controls and when the race is caught up. Selection changes still redraw immediately.

A condition-variable epoch wakes the guest on each raw PIT tick, physical
or scripted keyboard/mouse input, focus release and host shutdown. The
predicate checks both the epoch and undelivered IRQs under the condition
mutex, including events arriving before the wait. Waiting never reads a
timer delta or runs a game callback; the original guest polls retain that
ownership. The shared `input_checking`, VGA status/PRNG polling and frozen
C/ASM remain intact. An 11 ms timeout bounds shutdown if the clock
producer fails.

On this development machine, a five-second steady idle-menu window with
dummy SDL video/audio dropped from 98.4% to 0.3% of one core for the guest
and from 117.2% to 21.2% for the whole process. Percentages sum CPU time
across threads; they are not whole-machine Task Manager percentages. The
remaining steady cost is mainly the software presentation loop. These are
local measurements, not a performance guarantee for every machine.
A traced driving window fell from 116.2% to 26.9% of one core for the
whole process. Both baseline and candidate ran 100 simulation steps during
the measured five seconds, and all 215 GAMESTATE snapshots in their
15-second runs matched byte for byte. Paused replay had a separate inner
polling loop; yielding at its stable no-key edge reduced its guest thread
from 98.7% to 0.3% of one core. Replay advancement while its controls are
held and pending track-editor cursor blinks skip the idle wait. A steady
track-editor window reduced its guest from 97.8% to 0.6% of one core.

## Current implementation

All 38 historical C units and 32 host objects link with zero generated function
or data stubs. Host overlays preserve 16-bit scalar fields while translating
pointer-bearing runtime views. Frozen C and ASM are not rewritten for SDL3.

Live probes reach a race, sustain acceleration, steer both ways, brake, enter
paused replay and seek through recorded GAMESTATE frames. Additional probes
exercise replay playback, the track editor's save/reload flow in a separate
save directory, car selection, and a crash followed by the DNF results screen.
The live regressions reject stale builds and require clean bounded shutdown.

Splash, title and main-menu framebuffer/palette comparisons use recorded DOS
references. Routine tests execute locked machine code for sprite and raster
comparisons. Line cases include every screen edge, with descriptor and full
framebuffer comparisons. Resource tests cover bundled PVS/PES and P3S bounds.

The default audio backend translates AD15's AdLib/Sound Blaster FM services and
feeds the pinned Nuked OPL synthesizer into SDL. `--audio=pc15` selects the
optional PC speaker backend; `--audio=none` disables output. Driver selection
also selects the corresponding instrument banks. MT15 and TD15 are not
implemented. See [audio-drivers.md](audio-drivers.md) for signatures, machine
anchors and current synthesis limits.

## Root causes repaired

- Cross-unit pointer declarations truncated resource and filename addresses.
  Overrides now reach central declarations and local configs.
- Scene producers and consumers disagreed on record stride. Packed native views
  now agree across separately compiled units.
- A track-info consumer widened a DOS near-offset union and read 18-byte
  records from a 16-byte native producer. The two-byte link stays raw; its six
  stock nonzero offsets resolve to the corresponding camera data plus 42 bytes.
- The polygon marker overlapped its list's last slot. Host storage preserves the
  full list and its sentinel.
- Collision calculations promoted DOS words to 32-bit host integers before
  multiplication and shifts, and changed unsigned speed comparisons. The
  native overlay preserves the locked low-word IMUL, signed SAR, and unsigned
  comparisons for wall-hit thresholds and car-to-car speed loss.
- The race catch-up loop could spin without delivering timer callbacks. The
  outer loop now pumps them on the guest thread.
- The presenter read live pixels during drawing. Completed framebuffer and
  palette snapshots are now published together.
- Sprite repeat runs, clipping, projection arithmetic, callback scheduling,
  keyboard state, file services and teardown follow their original contracts.
- `mat_vec` read the frozen column-major matrix as rows. That moved the car
  selector and track vertices outside the clip rectangle. Matrix helpers now
  preserve the original coefficient strides and partial-store order, including
  overlapping inputs and outputs; 36 locked-machine memory cases cover them.
- Track-preview coordinate subtraction must wrap to a signed DOS word before
  arithmetic right shift. Nine generated-overlay expressions now reproduce
  the original SUB/SAR sequence, including far rows across the signed boundary.
- The recovered name `sprite_putimage_and_alt` hid an opaque copy routine.
  Its original entry reaches REP MOVSB/MOVSW; applying AND corrupted the sky.
  The host entry now copies pixels as the frozen routine does.
- Culling doubles its limit with SHL CX,1 before a signed comparison. Keeping
  the host's wider result changed which primitives were submitted. Two
  generated-overlay expressions now retain the low word and signed comparison.
- The rasterizer discarded polygons with every vertex on one scanline. The
  original shared setup sends those faces to a solid, inclusive line callback,
  including when the usual polygon fill is patterned.
- The host gathered all polygon intersections into even/odd pairs. The DOS
  renderer walks two ordered edge chains: forward edges replace each row's
  bounds and reverse edges merge into them. Skipped horizontal links still
  advance the traversal, so later edges can replace a shared scanline. Alternate
  callbacks also retain the first selected boundary for the rest of each reverse
  edge. Frozen wheel faces and a crossing-face counterexample cover both rules.

The build rejects implicit pointer/integer conversions, asserts established
shared native strides and field offsets in the producer/consumer units, and
rejects unresolved link stubs. Those checks catch conversion errors before a
live screen exposes them. Frozen DOS layouts and native pointer views remain
distinct: matching host strides does not mean forcing DOS sizes onto pointers.

Scalar storage width alone does not preserve DOS expression evaluation. The
physics regression executes locked load-image windows at 21706Ă˘â‚¬â€ś21735 and
34129Ă˘â‚¬â€ś34166, then compares the generated helper and collision function. In a
head-on 100 mph case, the target reduces speed to 78 mph; the unadapted host
expression instead wraps it upward to 206 mph. New arithmetic adaptations
must establish intermediate widths and signedness from the original code.

The unattended intro exposed another promotion difference: MSC compares a
signed word with the unsigned 16-bit literal `0xffff` by its word bits. GCC
promotes that word to signed 32-bit `int`, making `-1 == 65535` false. The
port adapter retains the frozen comparisons at load offsets `0xDC7D`,
`0x10CED`, and `0x10FC6`. These restore the intro camera's six-second turn
and two track predecessor assignments. `test_sdl3_word_sentinels.py` checks
the frozen instructions and compiled host expressions. The executable build
rejects `-Wtype-limits` diagnostics across all 38 game translation units.

The interlaced transition also depended on CPU work rather than a delay call.
Its native four-phase copy previously completed in about 3 ms, so the 60 Hz
presenter skipped most phases. `transition_work.h` accounts for the frozen
routine's retired instructions: 32 fixed, 19 per lane, and 18 plus 14 per
copied pixel in each active row. The reference Stunts machine profile uses
one normalized work unit per instruction at 9,000,000 units/s; this is not a
physical CPU frequency claim. A full-screen phase is 227,860 units, or
25.318 ms. Visible transitions now advance on absolute work deadlines and
publish their row progression at VGA refresh intervals. Offscreen copies
retain their unpaced pixel behavior. `test_sdl3_transition_work.py` compares
the compiled production helper with 28 frozen-machine calls.

An unattended native run through the main menu confirms the red car, rotating
and receding DSI logo, credits, and intermediate transition images. The new
original replay `sdltst_20261001_103119` independently shows that sequence;
its first click follows the completed menu. This checks startup content and
the transition primitive's timing model, not whole-game cycle timing. The
original CPU calibration selects full-frame drawing, while the native
calibration selects incremental drawing; both paths must preserve the scene.

The track chooser and Options dialog exposed a separate text translation
error. `draw_text_at` had been aliased to the transparent `font_draw_text`.
The frozen former routine writes both foreground and background pixels;
the latter preserves pixels under clear glyph bits. Retaining both paths
restores erased letters and inverted selections. The same family now preserves
whole-word color setup, proportional-font header updates, and the original
counted-width rule that skips missing glyphs. Text uses the frozen line-table
addressing without the general pixel helper's clip rectangle.

`test_sdl3_font_oracle.py` executes both locked machine entries against the
production font and sprite code. It compares the full 64 KiB framebuffer and
the complete modified font record for the built-in font and original FONTDEF,
FONTN and FONTLED resources, including restrictive clipping, swapped colors,
and a longer string overwritten by a space and terminator. The original
`sdltst2_20261001_114721` replay also reproduces the old chooser ghosting.
After correction, its chooser and Options frames match the full original
indexed images and DAC palette, including their cursors. Native scripts accept up to
4,096 events so this recording's 3,039 inputs need no path simplification.
Native scripts use wall-clock scheduling, so this is UI reproduction rather
than a claim of identical semantic-boundary timing or DOS directory ordering.
The recording also exposed two replay/file-service differences: normalized
mouse coordinates now use the original inclusive device bounds 0..320 and
0..200, and find results use uppercase DOS DTA spelling. Preserving host
filename casing previously changed the guest's case-sensitive track sort.
The input regression checks the recorded Options cursor at (102,130) and
preserves every event in a 3,039-observation stream; the file probe checks
uppercase results for literal, wildcard, saved and asset searches.
The later chooser checkpoint lands one row farther down in the wall-paced
native replay; identical whole-game input consumption timing is not established.

The manual-entry cursor exposed another shared-state translation error.
The recovered `SCREEN_RECT` used by the line editor is actually a view of
the active font header: foreground at +0, background at +2 and line height
at +18. Its frozen far pointer at load offset `0x305FE` shares its segment
word with `fontdefseg` at `0x30600`. The port had replaced it with a detached
copy of the initial font values, leaving the cursor's XOR mask at 3 even
after the dialog selected foreground 15. The native view now follows every
font selection and reads color changes from the same storage as rendering.
This also restores background and line-height changes when clearing an
editable line. Both views initially select the locked image's built-in font.

`tests/test_sdl3_line_input_oracle.py` regenerates the production C overlay
and compares both line-input helpers with the locked machine code. Seven
successive states cover the built-in font and all three shipped font records,
runtime foreground/background changes, nondefault line heights, trailing-line
clearing and two cursor toggles. Each comparison includes the full 64 KiB video
buffer, the active font record, the editable text buffer and cursor slot.
The initial alias and dialog foreground are independently checked against
locked image data.

A seeded manual-entry run confirms that every visible blink changes only
the nine cursor pixels from index 3 to index 15, with identical surrounding
pixels and palette. The XOR blitter itself matches the frozen implementation;
no cursor color constant was introduced. The related font and sprite paths
were checked for another detached header copy; none was found in that audit.

Two adaptations are explicit host boundaries. A missing music instrument is
rejected before dereferencing its null pointer; DOS performed a physical-memory
read first. The mode-3 compressor bounds scans to the remaining image pixels;
DOS FAR scans could wrap into allocator memory. Dashboard headers and decoded
pixels remain exact in 174 original-machine rendering cases; Jaguar retains
equivalent intermediate RLE packet/padding differences. They do not change the
historical byte acceptance oracle.

## Acceptance and evidence

[m1-progress.md](m1-progress.md) records the earlier startup milestone;
[sdl3-architecture.md](sdl3-architecture.md) describes the port boundary.
`build/sdl3/build-report.json` and `stub-inventory.json` describe generated output.
Scratch investigations live under ignored `build/workers/`; durable tests are
under `tests/`.

Full `tools/validate.py` is the acceptance boundary. The historical image,
relocations, independent contribution compiles and BSS checks must stay exact.
Modern routine and live checks establish bounded behavior. They do not prove
whole-game determinism or support for every audio profile and controller device.

The original-interpreter replay probe and seven independently captured indexed
references are documented in
[original-replay-oracle.md](../../tools/porting/original-replay-oracle.md).
The recorded corpus contains both v13 and v14 canonical schemas and requires
the corresponding interpreter. Track and opponent references are explicitly
labelled controlled branches of a recorded menu click. Native selection tests
compare complete indexed frames and palettes; animated poses are matched by
complete frame content, which does not establish identical animation timing.
The shared routine runner uses the original small-model SS=DS ABI so nested
calls can address stack-local vectors through near pointers.

`tests/test_sdl3_shape_pipeline.py` independently executes all 194 stock shape
resources through the locked renderer and freshly compiled production overlays.
It compares primitive counts, the complete 0x28A0-byte polygon pool, ordered
draw chains and full 64 KiB framebuffers. Its one selection-camera pose renders
161 resources and deliberately culls 33; this is explicit pose coverage rather
than an exhaustive camera or gameplay proof. `test_sdl3_polygon_raster.py`
adds original wheel faces and a crossing polygon that distinguishes the two
reverse-chain callbacks.

`tests/test_sdl3_diagnostics.py` induces main-thread and SDL game-thread
access violations and CRT aborts in child processes. It parses the minidump's
exception stream to verify the faulting thread and exception code, compares
the retained executable bytes, and checks normal/debug launches, fresh session
folders, explicit trace overrides and unavailable-directory handling.

`tests/test_sdl3_config.py` checks first-launch creation, preserved unknown
settings, Unicode paths, malformed/truncated files, unavailable locations,
and live races with the check disabled versus the original prompt enabled.
The package test also verifies config creation and upgrade preservation.

The 2026-10-01 configuration run passed all 811 tests without skips, all 90 independent
DOSBox-X contribution checks, fresh `HYBRID_EXACT` image equality with 2,588
ordered relocations, and the BSS/runtime real-link gate. Initialized ownership
is C 154,618 bytes, ASM 36,552 bytes, pinned runtime 8,768 bytes, and raw zero;
raw BSS is also zero. The complete receipt is `build/validation/report.json`.
The earlier 762-test suite did not include complete selection and preview
frames and therefore missed the reported missing models. The current gate
includes exact Countach, default-track and clock-opponent indexed frames,
all 194 stock shape resources in the documented camera pose, and a drop-in
install/upgrade launched with only the Windows system directory on PATH.
That package check saves/reloads a track, preserves all original game data,
and verifies every installed package file against its ZIP contents.

A nine-second live AD15 probe observed notes on all nine FM channels and
bipolar, nonzero PCM while the seeded race advanced. The audio regression also
renders a shipped ELPI instrument through the translated driver and pinned
synthesizer. The pinned dependency identity and build inputs are recorded in
`build/sdl3/build-report.json`.
