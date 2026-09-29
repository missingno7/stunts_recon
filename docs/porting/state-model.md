# Deterministic state and data layouts

This is the porting summary of the accepted global-state inventory. Names in `layout/names-registry.json` are reconstruction aliases constrained by address and bindings; they are not claimed to be original identifiers. The generated model covers 547 registered symbols and separately records four accepted-source camera arrays that have target addresses but no names-registry entries. The 52 accepted aggregate tags / 130 translation-unit layouts are generated under `build/porting/state-model.json` and `build/porting/state-model.md`. The size measurements use isolated probes with the canonical MSC 5.10 medium-model profile (`/AM /O /Gs`). See the P2b report at `build/workers/P2b/REPORT.md` for the source audit and probe method.

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

### Camera selector hitboxes

`game_camera_buttons_x1`, `game_camera_buttons_x2`, `game_camera_buttons_y1`,
and `game_camera_buttons_y2` are four 9-element `int16_t` arrays (18 bytes
each) in `obj_seg005` `_DATA`, at load addresses `0x2EA08`, `0x2EA1A`,
`0x2EA2C`, and `0x2EA3E` (DGROUP offsets `0x3298`, `0x32AA`, `0x32BC`,
`0x32CE`). Their starts are 18 bytes apart with no padding.
`loop_game` passes `game_camera_buttons_count[cammd] + 1` to
`mouse_multi_hittest`; the count table reaches 8, so the hit-test covers
indices 0 through 8. Direct drawing and camera-mode code also reads indices 7
and 8. The old 8/8/7/7 dimensions came from interior word labels and are not
allocation sizes. These arrays are presentation/menu state, outside the replay
checkpoint.

Preserve as the simulation/replay state set:

- The **entire 0x460-byte `GAMESTATE`**, both embedded cars, opaque bytes and six-byte Kevin seed.
- The ordered per-frame event bytes addressed through `g_tdreplay16buf` and indexed by `core.game_frame`.
- Simulation-affecting race setup: selected `GAMEINFO`, track/map data, car `SIMD` tuning and the fixed lookup tables consumed by the update call closure.
- In-memory checkpoint order and frame order. Seeking restores a full checkpoint then advances through the recorded bytes.

The `.RPL` writer stores `GAMEINFO`, track data and event bytes, but not the Kevin seed; reproducing the start of a replay across sessions still depends on matching the legacy seed initialization and call order. The pointer slots for replay/checkpoint buffers are process-local addresses: rebuild them on load and preserve their pointed-to contents separately.

Keyboard, joystick, mouse, menu, timer/interrupt, audio-driver and render-workspace state is outside the physics checkpoint. Preserve legacy resources and recorded input, but rebuild host/device pointers and derive presentation state from the authoritative simulation where the behavior allows it.

## Unknowns and limits

The full inventory classifies 102 globals as bit-exact replay state, 434 as presentation or process-local state, and leaves 11 globals with unknown replay roles. Physical units remain unknown for 341 of 547 registered globals. Three tags (GAMEINFO, TRACKOBJECT, VECTOR) have instance-dependent replay roles; 27 aggregate tags have no registered global or embedded instance. Offset-named fields and escaped-pointer accesses remain explicit unknowns. Empty reader/writer lists mean no direct access was found in accepted bodies, not proof that the object is unused.

“Unit unknown” does not mean an active simulation value can be dropped or that its bytes have unknown layout. Preserve accepted fixed-width arithmetic and proven conversion boundaries; keep uncalibrated coefficients as raw typed values. The 16-byte opponent speed table is copied from the `sped` resource and is indexed by surface plus object speed code in track-edge and acceleration logic; its physical unit is not established.

The 11 unknown-role rows have now been triaged against accepted access evidence:

| Global | Finding |
|---|---|
| aDefault_1 | DEFAULT is the default replay filename used by the replay file selector/loader (src/obj_seg000.c:415,2101-2106); UI/path input, not recorded simulation state. |
| postable[30] | Rebuilt as row z-edge positions row*1024; consumed by track geometry construction (src/obj_seg000.c:470, src/obj_seg004.c:210-231). |
| z_ctr_pos[30] | Rebuilt as row-center z positions row*1024+512; used to form local track coordinates (src/obj_seg000.c:471, src/obj_seg004.c:170,789). |
| g_corkscrew_type_flag | Only builder writes are visible in accepted C (src/obj_seg004.c:156,668,684); narrowed to track-object construction output, but leave its consumer/role unresolved. |
| collision_response_offsets[4] | Static initializer, no accepted direct reader/writer found; retain as unknown. |
| unused_40E73, track3_gap[2], framerate_pad_0, extra_rclist4[2], spare_td22_1, gap_trackrow_1 | No accepted direct access found; names are reconstruction aliases and do not prove padding or runtime meaning. Keep raw/unclassified. |

`postable` and `z_ctr_pos` are deterministic lookup tables rebuilt from the 30-row index; they need not be serialized as replay checkpoint state. Seven other globals (`collision_response_offsets`, `unused_40E73`, `track3_gap`, `framerate_pad_0`, `extra_rclist4`, `spare_td22_1`, and `gap_trackrow_1`) remain wholly unclassified. `g_corkscrew_type_flag` is a separate question: accepted C writes it during `build_obj`, but no accepted consumer has been identified.

### High-value physical-unit follow-up order

Of the 341 unit-unknown globals, 84 are assigned to or shared with simulation (63 are exclusively simulation-owned; 21 are shared with other subsystems). This is not one physical measurement list; several are pointers, flags, menu state, or aggregate records. Prioritize by simulation cadence and consequence:

1. core.playerstate / core.opponentstate and remaining GAMESTATE fields: read/written by frame update and full checkpoint restore. Keep units field-specific; the aggregate has mixed dimensions. Existing evidence identifies speed as Q8 mph, long/short world-coordinate scale as 64:1, and angle turns as 0x400.
2. simdp7 / ophys_7 (SIMD setup): passed into update_car_speed, update_grip, update_player_state and frame updates. Highest-value unresolved coefficients are mass, braking, torque curve, aero resistance, grip/sliding and per-surface grip (src/obj_seg001_complete.c:2182-2410; src/obj_seg001_complete.c:729-731).
3. opponent_spd_tbl[16]: read by opponent speed adjustment and track-edge prediction, initialized at 200 and loaded from opponent data (src/obj_seg001_complete.c:2307,2919,3113; src/obj_seg004.c:1928). It is a physics/AI control table; its physical scale remains unknown.
4. hillconsts, g_hillf, hgthgt: vertical terrain coordinate is consequential to initialization and collision. The accepted setup table is {0,450}, selected by the hill flag and multiplied by 64 at the long-world boundary (src/obj_seg000.c:417; src/obj_seg001_complete.c:1658-1694; src/obj_seg004.c:175,943). This resolves the two map-height levels; other dynamic hgthgt uses remain field-specific.
5. Race timing and summary metrics: rate_frame is 10 or 20 frames/s. game_frame, player/opponent end frames, game_penalty, game_total_finish, field_144 and elapsed offsets are frame counts; fmtframestr converts them to minutes/seconds by dividing by rate_frame. game_penalty adds rate_frame*3*penalty_ctr frame ticks per penalty. game_travDist adds car_speed2 once per player frame, and the average-speed display divides by frame count then shifts Q8, so it is a Q8-mph-frame accumulator rather than spatial distance. game_impactSpeed and game_topSpeed copy car_speed2 and display as Q8 mph (src/obj_seg001_complete.c:1915,1950-1952,2358-2359,3161-3202; src/obj_seg000.c:2229-2238,2287-2288; src/obj_seg008.c:1461-1480).
6. speed_recovery_divisors[5]: values {255,256,192,128,64} are divisors in a speed-recovery ratio, not a physical unit (src/obj_seg001_complete.c:729,2390).

The row/column tables are grounded: lnoffsets[i]=30*(29-i) and gterrtrk[i]=30*i are row byte offsets; postable[i]=i*1024, z_ctr_pos[i]=i*1024+512, xcols[i]=i*1024, and trackctrpos2[i]=i*1024+512. idxtrk/tagtrk hold start column/row. st_hdg is a heading angle where 0x400 is one turn (src/obj_seg000.c:464-474; start markers in src/obj_seg004.c:1289-1324; sine/cosine use in src/obj_seg001_complete.c:1654-1734). Note the accepted build_obj table-use discrepancy recorded in formats.md; do not infer table direction from reconstructed alias names alone.

Use the full generated model for the exact rows, compiler evidence, alias views, readers/writers and address_escapes. Resolve an unknown only when a concrete port consumer needs it, using the accepted declarations and call/access evidence; keep it unclassified until that evidence exists.

Evidence: `src/obj_seg003.c:90-235`, `src/obj_seg001_complete.c:1671-1770`, `src/obj_seg001_complete.c:740-745,1692-1695`, `asm/sincos.ASM:6-262,273-332`, and `build/workers/P2b/REPORT.md`.
