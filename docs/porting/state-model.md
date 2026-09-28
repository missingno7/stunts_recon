# Deterministic state and data layouts

This is the porting summary of the accepted global-state inventory. Names in `layout/names-registry.json` are reconstruction aliases constrained by address and bindings; they are not claimed to be original identifiers. The complete 547-symbol inventory and 52 accepted aggregate tags / 130 translation-unit layouts are generated under `build/porting/state-model.json` and `build/porting/state-model.md`. The size measurements use isolated probes with the canonical MSC 5.10 medium-model profile (`/AM /O /Gs`). See the P2b report at `build/workers/P2b/REPORT.md` for the source audit and probe method.

## Replay checkpoint: `GAMESTATE`

The live replay checkpoint `core` is **0x460 bytes (1,120 bytes)**. `update_gamestate` copies the full aggregate to the checkpoint ring; `restore_gamestate` restores the full aggregate and reinitializes the Kevin PRNG from the included seed. The complete extent is authoritative, including fields still named by offsets.

| Offset range | Contents | Notes |
|---|---|---|
| `0x000?0x11F` | `game_longs1[24]`, `game_longs2[24]`, `game_longs3[24]` | Three 24-element `long` world-position/history arrays. |
| `0x120?0x137` | Four short `VECTOR`s | `game_vec1[2]` plus the two remaining vector slots. |
| `0x138?0x151` | Frame/timing, travel-distance, finish, penalty, speed and jump fields | `game_frame` is at `0x140`; the legacy counters remain integer fields. |
| `0x152?0x221` | Player `CARSTATE` (0xD0 bytes) | Embedded simulation state. |
| `0x222?0x2F1` | Opponent `CARSTATE` (0xD0 bytes) | Embedded simulation state. |
| `0x2F2?0x2FD` | Start row/column and paired start positions | Six `short` values. |
| `0x2FE?0x3BD` | Four 24-element `short` arrays | Preserve the full spans while some entries remain opaque. |
| `0x3BE?0x3ED` | 48 opaque bytes | Retain in snapshots. |
| `0x3EE?0x3F3` | `kevinseed[6]` | Mutable six-byte PRNG seed. |
| `0x3F4?0x45F` | Input/replay flags and opaque fields/arrays | Retain the complete tail. |

The accepted `GAMESTATE` tag has a **separate 1,014-byte view** in `src/obj_seg004.c`; do not substitute that translation-unit view for the 1,120-byte replay object. The full model keeps same-named tags per translation unit.

## Embedded `CARSTATE`

Each `CARSTATE` is **0xD0 bytes (208 bytes)**. Its main blocks are two 12-byte `VECTORLONG` world positions at `0x00` and `0x0C`, a 6-byte rotation `VECTOR` at `0x18`, scalar speed/RPM/gearing/steering and grip fields through `0x4B`, five four-wheel `short` result arrays at `0x4C?0x73`, two four-wheel `VECTOR` coordinate arrays at `0x74?0xA3`, three additional vectors at `0xA4?0xB5`, words at `0xB6?0xBB`, and byte flags/state at `0xBC?0xCF`.

Evidence-backed units are narrow: `car_speed` and `car_speed2` are unsigned Q8 mph (`raw = mph ? 256`); `car_speed` is rev-coupled and `car_speed2` is actual movement speed, so they diverge during jumps. The long world positions are 64 times short map/vector coordinates at the proven conversion sites. RPM thresholds are engine-rate values, but their physical calibration is not established. Other values without a proven conversion remain raw words/bytes.

## Race inputs and common vector records

| Record | Layout | Porting interpretation |
|---|---|---|
| `SIMD` | **776 bytes (0x308)**: gear/torque data, RPM thresholds, grip/sliding, collision and wheel coordinates, dashboard tables, aero data pointer | Selected car setup affects deterministic simulation. Preserve source table values and rebuild process-local pointers to their tables. Some fields and signedness differ by accepted TU declaration while measured size remains 776. |
| `GAMEINFO` | **26 bytes (0x1A)**: player/opponent car IDs, materials and transmissions, track name, frame rate and recorded-frame count | Race/replay setup. The first 26 bytes of an `.RPL` are loaded as this header. Backup/menu instances have a different replay role from active settings. |
| `VECTOR` / `VECTORLONG` | **6 / 12 bytes**: signed short or long `x,y,z` | Coordinate frame depends on the instance. Car world-position conversion uses the ?64 relationship; screen-space vectors are integer pixel coordinates. |
| `POINT2D` | **4 bytes**: two `int`s | Instance-dependent: projected points are screen pixels; collision/dashboard points use their owning setup or simulation frame. |
| `AUDIO_CAR_FRAME` | **34 bytes** ? 40 records in the 1,360-byte `audio_frmarr` | Derived audio presentation data (spatial offsets and copied RPM), not part of the authoritative replay snapshot. |

Fast trig uses `0x400` units per turn, `0x100` per quadrant and a table peak of 16,384 (Q14). The transform and table values are fixed inputs to deterministic math; do not replace them with floating-point approximations.

## Replay-determinism boundary

Preserve as the simulation/replay state set:

- The **entire 0x460-byte `GAMESTATE`**, both embedded cars, opaque bytes and six-byte Kevin seed.
- The ordered per-frame event bytes addressed through `g_tdreplay16buf` and indexed by `core.game_frame`.
- Simulation-affecting race setup: selected `GAMEINFO`, track/map data, car `SIMD` tuning and the fixed lookup tables consumed by the update call closure.
- In-memory checkpoint order and frame order. Seeking restores a full checkpoint then advances through the recorded bytes.

The `.RPL` writer stores `GAMEINFO`, track data and event bytes, but not the Kevin seed; reproducing the start of a replay across sessions still depends on matching the legacy seed initialization and call order. The pointer slots for replay/checkpoint buffers are process-local addresses: rebuild them on load and preserve their pointed-to contents separately.

Keyboard, joystick, mouse, menu, timer/interrupt, audio-driver and render-workspace state is outside the physics checkpoint. Preserve legacy resources and recorded input, but rebuild host/device pointers and derive presentation state from the authoritative simulation where the behavior allows it.

## Unknowns and limits

The full inventory classifies **102 globals as bit-exact replay state**, **434 as presentation or process-local state**, and leaves **11 globals with unknown replay roles**. Physical units remain unknown for **342 of 547 globals**. Three tags (`GAMEINFO`, `TRACKOBJECT`, `VECTOR`) have instance-dependent replay roles; 27 aggregate tags have no registered global or embedded instance. Offset-named fields and escaped-pointer accesses remain explicit unknowns. Empty reader/writer lists mean no direct access was found in accepted bodies, not proof that the object is unused.

Use the full generated model for the exact rows, compiler evidence, alias views, readers/writers and `address_escapes`. Resolve an unknown only when a concrete port consumer needs it, using the accepted declarations and call/access evidence; keep it unclassified until that evidence exists.

Evidence: `src/obj_seg003.c:90-235`, `src/obj_seg001_complete.c:1671-1770`, `src/obj_seg001_complete.c:740-745,1692-1695`, `asm/sincos.ASM:6-262,273-332`, and `build/workers/P2b/REPORT.md`.
