# F5c Runtime Event Sampling

## RESULT

Obtained event-level samples at the original F5 PortForge v7 profile by rebuilding its exact source revision (`a0e111bc0a49750115cd2963bcba4c0ea0ea0d4d`). Both the full saved replay and a saved active-pause input capture replayed with strict canonical verification. The observer sampled every PIT deadline and configured video publication, with mapped game state and audio publication data.

## STATE CHANGE

Canonical source, recipes, layout, evidence, assets, tools, and other repositories were not changed. All source adaptations, build outputs, raw logs, captures, JSONL traces, analyses, and this report are under `build/workers/F5c/`. The scratch boundary checker needed an empty-`git ls-files` fallback because this worker archive is nested beneath the main repository's ignored `build/` directory; only the scratch copy was changed.

## STRONGEST EVIDENCE

- Profile identity: `trace_summary.json` records source commit `a0e111bc0a49750115cd2963bcba4c0ea0ea0d4d`, profile `stunts-normalized-interactive-v7` at 9,000,000 wu/s, and `game.json` SHA-256 `45475B05F70DB3E9458FFBDBBCC60EE49CD6D28C1E71C27CF717E226BE91DB2F`.
- Full saved replay: `event_trace.jsonl` has 8,001 timer, 5,608 video, and 24,019 audio events; all 37,628 samples validate DGROUP 0x3B87. Strict replay covered `[0,16214)`, 29 canonical comparisons, zero mismatches; canonical SHA-256 `ccfb41418ba3a4d910bd78cb669ea622cf70b82ea541fad51933289e73b3e2ec` (`trace_summary.json`).
- Exact active-pause capture: `active_pause_event_trace.jsonl` has 1,200 timer, 841 video, and 2,400 audio events; all 4,441 samples validate the same DGROUP anchor. Strict exact replay passed at tick 100014750444 / instruction 900132754, canonical SHA-256 `15b8b462d807dbbb1499086552419762ae0477210116c2b356d65e39d6c2c8b6`, output SHA-256 `385aa0378adc333f4d7bd44f721d1de037454166bd26ca9bd7bedc37ba3ab80e` (`active_pause_exact_replay_verify.log`).
- State mapping basis: `build/workers/F5b/map_validation.json` and `docs/porting/state-model.md`; validated fields include `game_frame`, input/replay mode, `globalgamesettings.game_framespersec`, `game_jumpCount`, and the player CARSTATE raw tail. Unvalidated offsets remain explicitly raw in the summaries.

## HYPOTHESES TESTED

- **v5/v7 timing mismatch:** confirmed the F5 capture used v7 and rebuilt the observer from its exact PortForge source revision and matching `game.json`; strict verification succeeded.
- **New observer workload:** used F5's saved exact input script for the active pause/resume capture, avoiding a new manual recording and preserving reproducibility.
- **Built-in replay behavior:** replayed the saved F5 workload through completion and tracked menu/replay/race state transitions.
- **Pause behavior:** P make/break pairs occur at machine ticks 89280000000 and 94300000000; annotated paused and resumed frames occur at 89321773555 and 94401225444. From paused-overlay visibility to the resume P event (4.978226445 seconds), 498 timer samples and 349 video publications show no `game_frame` advance; 995 audio batches (219,397 sample frames) continue. Between resume P and resumed-frame visibility, 10 timer, 7 video, and 21 audio events include two frame advances. This separates the paused interval from input/render settling.

## NEW KNOWLEDGE

- `globalgamesettings.game_framespersec` is 20 during the sampled race and throughout the pause capture; it changes from 0 to 20 at tick 65908959222 and returns to 0 at tick 77820000000 in the full replay. No 10 value occurred in either workload.
- In active race mode (`inputmode=1`, `replay_mode=0`), 845 simulation frame advances accompany 1,058 video publications and 4,532 nonempty PortForge audio batches: 5.363 batches per simulation advance. These count guest render batches, not host audio callbacks.
- During the pause capture, mode fields remain `inputmode=1`, `replay_mode=2`; the explicit pause interval suppresses game-frame advances while timer, video, and audio events continue.
- The full replay's active race video frame-delta histogram is 0:831, 1:133, 2:14, 6:13, 7:15, 8:14, 9:11, 10:13, 11:9, 12:4, 13:1. Larger deltas represent catch-up across publications; the separate 11-to-0 transition is a mode/reset boundary.
- Replay playback in the menu used `inputmode=0`, `replay_mode=1`; 11 game-frame advances were sampled before the counter reset. No P pause scancode occurred in that workload.

## TOOLCHAIN

None. This investigation concerns PortForge runtime timing, not the pinned MSC historical compiler profile.

## REMAINING BLOCKER

- No 10 Hz workload was reached; its selection condition remains an unobserved semantic case (freeze class E, nonblocking).
- Neither workload reached a labeled crash or airborne event: `game_jumpCount` stayed 0, and CARSTATE C9 remains a raw byte rather than a grounded crash label (class E, nonblocking).
- These observer runs published FrameStart samples only; VerticalRetraceStart was not sampled (evidence-tool/configuration limitation, freeze class D).
- The exact artifact's internal observation ring ended at 4,096 entries (`raw_complete=false`); the separate external event callback stream contains all 4,441 timer/video/audio samples and the machine output still passed exact replay verification (class D logging limitation).
- No historical function target is unresolved, so a per-function blocker class does not apply. There is no freeze blocker (class F).

## RECOMMENDED NEXT ACTION

Close this bounded attempt with the verified traces. If further runtime evidence is needed, capture a workload that demonstrably selects 10 Hz or produces a mapped crash/airborne state, and enable VerticalRetraceStart sampling.

## NEEDS OPUS?

NO.
