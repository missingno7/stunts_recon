# Port Forge runtime oracle

This record captures bounded observations of Stunts 1.1 MCGA under Port Forge. It describes those recorded workloads; it is not a claim of broad DOS or SDL conformance.

## Reproduction identity

| Item | Identity |
|---|---|
| Port Forge repository | `stunts_forged/port_forge` (the read-only campaign checkout is archived locally at `build/workers/F5c/port_forge_a0e111b/`) |
| Source commit | `a0e111bc0a49750115cd2963bcba4c0ea0ea0d4d` |
| CPU/work profile | `stunts-normalized-interactive-v7`, 9,000,000 work units/second |
| Game configuration | `game.json`, SHA-256 `45475B05F70DB3E9458FFBDBBCC60EE49CD6D28C1E71C27CF717E226BE91DB2F` |
| Saved replay result | `[0,16214)`, 29 canonical comparisons, 0 mismatches; canonical SHA-256 `ccfb41418ba3a4d910bd78cb669ea622cf70b82ea541fad51933289e73b3e2ec` |
| Trace format | `stunts-runtime-trace-v1`, newline-delimited JSON |

Reproduction requires the named Port Forge revision and work profile, the same `game.json`, the matching replay base snapshot and input artifacts, plus the original game assets. Rebuild the `pf_dos_session` observer against that checkout, then replay the saved race workload with `replay-rm` through the recorded range, or rerun the active-pause workload with `exact-record-rm` and its saved input script. The local command wrappers and inputs are under `build/workers/F5c/` and `build/workers/F5/portforge_run/artifacts/`; they are ignored research artifacts, not inputs to the historical acceptance gate.

Normalize any resulting event trace by passing its path explicitly; the normalizer has no fixed checkout or scratch path:

```powershell
python tools/porting/normalize_portforge_trace.py build/workers/F5c/event_trace.jsonl `
  --out build/porting/runtime-oracle/replay.jsonl
python tools/porting/normalize_portforge_trace.py build/workers/F5c/active_pause_event_trace.jsonl `
  --out build/porting/runtime-oracle/active-pause.jsonl
```

The committed fixture under `tests/fixtures/` is only a small schema excerpt. Full traces stay under ignored `build/workers/`.

## Trace schema

`stunts-runtime-trace-v1` is newline-delimited JSON. Every row has `trace_schema` and `event_type`; timed rows use integer `machine_tick` nanoseconds and may carry a separate diagnostic `host_ns`. Tick origins are session-local. Run metadata belongs in the first `header` row. The current F5c captures contain `timer_tick`, `video_publication`, and `audio_publication` rows; the remaining rows below are defined observer or SDL3 producer events, not claims that F5c emitted them.

| Event type | Fields to retain or emit |
|---|---|
| `header` | Run mode and source/build/config identities. |
| `timer_tick`, `timer_summary`, `timing_retention` | `tick_id`, `scheduled_tick`, `observed_tick`; aggregate deadline/IRQ counts and retained-series counts/completeness. |
| `task_boundary` | Guest task point, occurrence, phase, global ordinal, guest instruction ordinal and outcome. |
| `input` | Channel, payload, sequence, semantic boundary or visible input tick, and tick-source note. |
| `simulation_step_coverage`, `simulation_step_sample` | Function invocation/completion totals and retained replay samples. Sample ticks can refer to a predecessor task boundary and must be labelled as estimates. |
| `simulation_step` (SDL3) | Monotonic `sim_step_id`, `game_frame`, `machine_tick`, and `host_ns` when available; emit only after a game update commits. |
| `video_publication` | `frame_id`, scheduled and actual machine tick, phase, `game_frame`, optional framebuffer/palette hashes and device progress counters; SDL3 also carries current `sim_step_id`. |
| `host_present` | `frame_id`, monotonic `present_id`, host monotonic time; emit only after the SDL presentation succeeds. |
| `host_window` | 100 ms machine-time bucket with video publications, host presents, audio queue state and diagnostic host wall time. |
| `audio_publication` | Guest-side nonempty render batch, `audio_publication_id`, interval start/end ticks and `audio_frame_count`; this is not a host audio callback. |
| `audio_callback` (SDL3) | Monotonic `audio_callback_id`, `machine_tick`, `host_ns`, and submitted frame count for each SDL callback. |
| `audio_adapter_summary`, `audio_queue_summary` | Callback/publication aggregates and terminal queue, underrun and starvation counters. |
| `visual_state` | Optional frame-grounded visual annotation with source frame, framebuffer hash, machine tick and visible state label. |

Captured state projection fields include `data_segment`, `dgroup_anchor_ok`, `game_frame`, `game_inputmode`, `game_replay_mode`, `rate_target_hz`, `game_jumpCount`, and raw player bytes `player_CARSTATE_tail_byte_0` through `_9`. The captures also retain unvalidated words as `unvalidated_u16_0x4108`, `unvalidated_u16_0x8E5C`, and `unvalidated_u16_0x8E5E`; keep them explicitly unvalidated. `framespersec_global` and `globalgamesettings_game_framespersec` are observed names for rate state. Unknown fields remain raw; CARSTATE bytes are not complete crash/airborne labels.

The normalizer preserves source fields and adds `machine_tick` from an observed/actual tick when needed, `game_mode_pair_raw`, `timer_interval_ticks`, timer frame deltas, `video_phase`, and same-phase frame-counter deltas. Counter wraps/resets are marked `game_frame_counter_discontinuity`. It does not generate `sim_step_id` from sampled counters. For SDL3, increment `sim_step_id` only after a committed simulation update; the per-publication step count is the ID delta between consecutive `video_publication` rows. Keep `audio_publication` separate from `audio_callback`, and set `runtime_mode` (`live`, `replay`, `paused`) and `rate_target_hz` (10 or 20) from guest state at each event boundary, never from host refresh rate.

## Measured relationships

The full replay event stream contains 8,001 timer deadlines, 5,608 frame-start video publications and 24,019 nonempty audio publications. All 37,628 event rows carry the validated DGROUP anchor. Its playback completed the saved replay with the verification identity above.

Timer intervals were 10,000,153 ns for 2,794 intervals and 10,000,154 ns for 5,206 intervals: about 99.9985 deadlines/second in this profile. This measures Port Forge timer deadlines for the capture; it does not establish the original PC's inherited PIT control word or INT 8 vector chain behavior.

During the sampled active race, `globalgamesettings.game_framespersec` was 20. The observed frame advance between consecutive `FrameStart` publications is grouped by raw `game_inputmode`/`game_replay_mode` pair below. This is the sampled game-frame-counter delta, not an independently emitted simulation-step ID; a counter reset at a mode boundary is excluded as a step count.

| Raw mode pair | Frame advances between publications | Total advances |
|---|---|---:|
| `input0_replay0` | 0: 4,429 | 0 |
| `input0_replay1` | 0: 110, 1: 7, 2: 2 | 11 |
| `input1_replay0` | 0: 831, 1: 133, 2: 14, 6: 13, 7: 15, 8: 14, 9: 11, 10: 13, 11: 9, 12: 4, 13: 1 | 845 |

Thus the active-race capture spans 0–13 game-frame advances between rendered frame publications. For that mode pair, 845 game-frame advances accompany 1,058 video publications and 4,532 nonempty audio render batches: 5.363 audio publications per advance. These audio batches are not SDL audio callbacks or a measurement of `.DRV` hook cadence.

In the saved pause/resume workload, the P-key events occur at 89.280 s and 94.300 s. A paused overlay is visually confirmed at 89.321773555 s; active driving is visible again at 94.401225444 s. From paused-overlay visibility to the resume key event, 498 timer samples, 349 video publications and 995 audio batches continue while `game_frame` does not advance. During the input-to-visible-resume settling interval, 10 timer samples, 7 video publications and 21 audio batches include two game-frame advances. The complete 12-second observation contains 1,200 timer, 841 video and 2,400 audio publications.

## Coverage limits

- No workload selected the 10 Hz target; only the sampled 20 Hz race is measured.
- No independently grounded crash or airborne interval was reached. `game_jumpCount` stayed zero; `CARSTATE` C9 is retained raw and is not a complete crash label.
- These observer runs emitted `FrameStart` samples only. `VerticalRetraceStart`, retrace polling and palette/status port traffic were not measured.
- Timer deadlines and game callbacks are observable in Port Forge, but this trace does not supply original DOS IVT snapshots, INT 8 chain totals, keyboard IRQ counts, audio-driver port traffic or far-call cleanup behavior.
- Audio publications count nonempty Port Forge render batches, not host audio callbacks.
- The F5c exact-capture artifact's internal ring is capped at 4,096 observations (`raw_complete=false`); its separate streamed event trace contains all 4,441 external samples.

These limits remain explicit in [unresolved questions](../unresolved.md) and [runtime measurements](runtime-measurements.md). Evidence details are in `../evidence/freeze-campaign/F5c-runtime-oracle.md`, `trace_summary.json`, and `active_pause_summary.json`.

## SDL3 trace producer

The SDL3 port should emit the same schema alongside the runtime, with its own source/build/config identity in the header. Emit one `timer_tick` per emulated timer interrupt, one `simulation_step` only after a committed game update, one `video_publication` at the game render boundary, one `host_present` only after SDL presentation succeeds, and one `audio_callback` per SDL callback. Give each kind a monotonic ID (`tick_id`, `sim_step_id`, `frame_id`, `present_id`, `audio_callback_id`); include both machine tick and host monotonic nanoseconds when available.

Attach `sim_step_id` to each `video_publication`. The number of completed steps since the prior rendered publication is the difference in those IDs and may be 0, 1, or greater than 1. Preserve `audio_publication` for game-side batches as a distinct event from `audio_callback`. Set `runtime_mode` and `rate_target_hz` from guest state at the event boundary, never from the host refresh rate. Keep segmented pointers, raw state bytes, and unknown record fields explicit until a bounded mapping has been proven.
