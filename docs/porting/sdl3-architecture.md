# SDL3 portable runtime architecture (M0)

## Purpose and boundaries

The port keeps the accepted `src/*.c` gameplay and simulation as the primary
implementation. The host layer lives under `port/`; generated host overlays,
objects, executables, runtime asset copies, traces, and stub inventories live
under ignored `build/sdl3/`. The historical source, recipes, ownership, oracle,
and original assets are not build outputs and are not modified by the port.

The first target is a 32-bit i686 executable using GCC 16 and SDL3 3.4.16. Its
logical display is a 320x200 indexed surface. SDL supplies the window, input,
monotonic timer, and eventual audio device. The game remains responsible for
its original simulation, render order, replay state, and file/resource
decisions.

The Empires reconstruction is a useful precedent for separating a portable
game core from host adapters, for keeping SDL out of the core, and for building
the shared core separately from its SDL3 app. Its flat-pointer substitution
does not carry over unchanged: Empires' audited paragraph rounding is an
allocation detail, while Stunts stores and computes far addresses, has explicit
`MK_FP`/segment arithmetic, calls loaded `.DRV` entries by offset, and has
medium-model pointer and record contracts. Stunts therefore keeps an explicit
segment:offset value at port boundaries. The `PORT_BUILD` flat C view is a
compile adapter only; it is not evidence that every historical far pointer is
a native host pointer.

The independent `stunts_forged`/Port Forge work is used as a runtime and
renderer clue, not as an implementation dependency. Port Forge's observed
99.9985 Hz deadlines and event schema are the M0 timing reference. Its DOS
real-mode interpreter and the C++ Stunts port have different execution and
ownership boundaries from this host-compiled C port.

## Address and segment model

Port APIs use a tagged `PortFarPtr` containing `space`, `segment`, and
`offset`. A raw packed integer or a cast from `(segment << 16) | offset` is
never dereferenced as a C pointer.

- `PORT_FAR_REAL` uses 20-bit real-mode linearization:
  `linear = ((segment << 4) + offset) & 0xFFFFF`. Normalization carries the
  high offset bits into the segment and leaves the low nibble in the offset.
  Resolution checks the requested byte extent against the emulated 1 MiB
  address space before returning a host view.
- `PORT_FAR_HANDLE` names a host allocation or loaded resource through a
  segment table. Offset arithmetic is bounded by that allocation's complete
  recorded extent. Segment advances across 64 KiB windows are explicit.
- Null is the all-zero far value. Each allocation/resource entry records its
  size, owner, lifetime, and segment range. Resource contents stay byte-for-
  byte and are not serialized as host pointer values.

The 1 MiB map reserves low DOS state and the upper ROM/video area. The
`0xA0000` to `0xAFFFF` video aperture aliases the host's 64 KiB indexed
framebuffer; its first 64,000 bytes are the visible 320x200 surface.
Paragraph allocation is 16-byte aligned and bounded;
resource blocks that do not fit use handle space. `farmalloc`-style requests
and the game memory-manager API return mapped host views only inside the port
adapter. `mmgr_free` releases the same registered allocation. M0 implements
the address representation, bounded resolver, and allocator catalog; it does
not claim that all source-level pointer fields have been migrated to tagged
values yet.

The original `0x3B87` DGROUP anchor is evidence about the DOS image, not a host
pointer base. Host globals remain native i686 objects. A later state adapter
must register a DGROUP offset and complete extent before exposing it as a far
value; it must not derive target addresses by truncating a process pointer.

## File and resource dispatch

The immutable source assets are copied read-only into
`build/sdl3/runtime/assets/`. The runtime root is configurable but defaults to
that copy. Lookup is DOS-style case-insensitive, accepts slash and backslash,
preserves the requested basename for trace/debug output, and never writes into
the source asset tree. File handles are host table entries; reads and seeks
operate on complete file bytes. A loaded resource receives an allocation/handle
entry whose full original byte extent is retained.

Archive/container parsing remains in the existing game code where available.
M0 does not invent `.PRE`, `.PVS`, `.TRK`, `.RPL`, or `.HIG` layouts to get past
an unsupported reader. A missing/unsupported reader is an explicit startup
stop recorded by the generated stub inventory. Save paths will be redirected
to a separate writable runtime directory before editor/save flows are enabled.

## Video surfaces and palette

The guest-facing indexed surface is 320x200 bytes with a 320-byte pitch and a
256-entry RGB palette. Distinct legacy drawing windows will become separate
indexed surfaces with their original clipping and copy order; M0 establishes
the primary surface and indexed SDL3 texture. The palette's stored values keep
the legacy 6-bit DAC bytes. Presentation expands each channel with
`(v << 2) | (v >> 4)` and does not change the game palette.

SDL3 uses an `SDL_PIXELFORMAT_INDEX8` streaming texture with an attached
`SDL_Palette`, nearest scaling, and an integer 320x200 viewport. Framebuffer
publication and host presentation are separate boundaries: a game-side
surface copy/update emits `video_publication`; a successful SDL render emits
`host_present`. SDL refresh rate does not pace the game timer. No per-frame
retrace wait is inserted because the accepted source only proves a startup
status poll, not a render-loop retrace contract.

## Input mapping

SDL keyboard scancodes map to the DOS set-1 make/break values consumed by the
game; extended arrow/navigation keys retain their `0xE0`-style high-byte
identity in the port scan-code representation. No Unicode translation or
host-layout text substitution is performed at the game input boundary. SDL
mouse coordinates are converted through the integer viewport to 320x200
logical coordinates, with button state kept separate from the game/replay
control byte. M0 implements the INT 15h `C201h` startup selector and the used
INT 33h reset, position, bounds, and pixel-ratio functions over SDL mouse
state. Events update host device state; sampling remains at the game's logical
input boundary. Keyboard callbacks, joystick, DOS cursor drawing, and other
interrupt functions remain explicit unresolved boundaries.

## Timer, ISR, and simulation cadence

F5c's full Port Forge trace measured intervals of 10,000,153 ns and
10,000,154 ns (about 99.9985 deadlines/s). The sampled race selected 20 Hz;
the 10 Hz startup choice was not observed. The Stunts source loads divisor
11,932 after a channel-2 control word and writes channel 0, so the inherited
DOS channel-0 mode remains unresolved. M0 follows the measured Port Forge
deadline rate without claiming that it establishes the original PC's PIT
control word.

The SDL timer thread schedules 10,000,154 ns deadlines from a monotonic
nanosecond clock. It records every due tick and catches up overdue deadlines
without dropping tick IDs. It does not use display refresh to advance time.
M0 emits timer deadlines only; emulating the timer ISR's callback list,
BIOS-chain cadence, interrupt exclusion, and reentry counters is the next
runtime stage. A registered callback must not run concurrently with a
simulation step until the callback/lock boundary has been recovered.

At the game boundary, one `simulation_step` is emitted after a committed
`update_gamestate` call, with a monotonic `sim_step_id` and the resulting
16-bit `game_frame`. Catch-up may produce several steps before one video
publication. A frame counter sample is not relabeled as a simulation step if
the update boundary was not observed.

## Audio plan

M0 is silent. It does not execute original 16-bit `.DRV` machine code. The
host audio adapter returns a successful-but-silent driver selection for the
startup path and keeps the documented game-side entry-offset/argument
contract in one port dispatch interface for later migration. Driver calls,
guest audio publications, and SDL audio callbacks remain distinct event
types. `audio_publication` is emitted only when the game publishes a guest
batch; no empty batch is fabricated to make trace counts resemble Port Forge.
An SDL audio device and callback are deferred until the call contract and
resource ownership are covered.

## Runtime trace

The port writes `stunts-runtime-trace-v1` JSONL. The header records build,
asset-root, and timer-profile identity. Timed events retain session-relative
integer nanoseconds and a separate host timestamp where available.

| Event | Emission boundary |
|---|---|
| `timer_tick` | One emulated deadline, including tick ID, scheduled and observed times. |
| `simulation_step` | After a committed `update_gamestate`, with step ID and `game_frame`. |
| `video_publication` | After the game/adapter publishes the indexed surface, with frame and step IDs. |
| `host_present` | After SDL successfully presents the current indexed texture. |
| `audio_publication` | At a guest-side nonempty audio batch boundary. |
| `host_stop` | At normal guest return or an unresolved stub boundary. |

The Port Forge normalizer remains the shared schema normalizer. Regression
checks compare event shape and deadline rate against
`docs/evidence/freeze-campaign/F5c-trace_summary.json`; startup-only missing
simulation/audio event classes are reported as coverage gaps rather than
treated as a gameplay mismatch.

## Link stubs and startup stop

The first link is intentionally strict. Its undefined-symbol diagnostics feed
the generated `build/sdl3/stub-inventory.json` and stub translation unit.
Unresolved functions log their exact symbol and unwind the guest thread at
that boundary. Unresolved data receives a zero-filled placeholder with an
explicit inventory row; those values are not accepted source ownership and
cannot be mistaken for recovered state. Implemented host services are removed
from the generated stub set. The final link is repeated until it has no new
undefined symbols or the linker reports a concrete non-symbol error.

M0 runs the original `main` entry on a guest worker after SDL, timer, input,
and the indexed framebuffer have started. The renderer's 15-word sprite
descriptor setup is translated from `asm/patterned_lines_windows.ASM:3855-3879`
as an exact 30-byte copy. Startup currently stops at the unresolved
`file_load_shape2d_fatal_thunk`, called by `init_main.c:256` for the `sdmain`
shape/palette container. Asset file dispatch works, but this container reader
is not present in the 38 active game C objects. It is preserved as a logged
stopping point instead of being approximated. The window remains available for
a bounded `--run-ms` duration; `tools/porting/regress_sdl3_trace.py` applies
the shared normalizer and compares trace structure and scheduled rate against
the F5c summary. That summary has counts and a timer histogram, not the raw
reference JSONL, so missing gameplay event classes are reported as coverage
gaps.

## Milestones and oracle comparisons

| Milestone | Work | Comparison/oracle before acceptance |
|---|---|---|
| M0 | i686 build, all active game C objects linked, generated stub inventory, SDL indexed window, measured-rate timer, startup entry, and trace regression. | 38/38 host object compiles; no undefined final-link symbols; trace schema and scheduled/observed timer rate compared with F5c summary and normalizer. Record the exact first stub. |
| M1 | Close initialization's platform boundary: DOS memory, startup files/resources, palette upload, cursor, and system startup order. | Compare startup call order and resource bytes/extents with accepted Stunts source, asset catalog, and DOSBox-X capture; no raw pointer/segment casts. |
| M2 | Timer ISR/callback model, input sampling, logical 10/20 Hz pacing, pause, catch-up, and replay. | Compare normalized Port Forge timer/frame/mode/pause trace; keep its unmeasured 10 Hz and PIT mode limits explicit; compare replay state/frame sequence against saved replay oracle. |
| M3 | Faithful indexed software surfaces, clipping, copy/blit, palette, and migrated renderer ASM capsules. | Compare per-frame 320x200 index bytes and palette against DOSBox-X captures at named publication boundaries; use exact clipping/blit cases from accepted ASM. |
| M4 | Guest audio driver call adapter and optional SDL audio backend. | Compare guest call offsets, argument bytes, and publication sequence to accepted callers/F5c guest batches; validate host callback separately. Keep vendor `.DRV` semantics unresolved without evidence. |
| M5 | Menus, track editor, replay/save/high-score paths, and DOS filesystem behavior. | Compare `.TRK`, `.RPL`, `.HIG`, and resource round trips byte-for-byte; replay/menu state transitions against source-derived behavior and runtime traces. |
| M6 | End-to-end fidelity closure and portability cleanup. | Compare deterministic replay state checkpoints and normalized event traces, repeat on clean i686 build, and keep C, ASM, pinned runtime, and stub/raw ownership separate. |

Each milestone preserves the immutable historical oracle and original asset
extents. M0's successful host link is a toolchain/runtime scaffold, not proof
of gameplay fidelity.
