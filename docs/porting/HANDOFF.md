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

## Current implementation

All 38 historical C units and 30 host objects link with zero generated function
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
physics regression executes locked load-image windows at 21706–21735 and
34129–34166, then compares the generated helper and collision function. In a
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

The 2026-10-01 startup-fix run passed all 792 tests without skips, all 90 independent
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
