# Phase 1 audit: historical oracle and porting freeze prep

## RESULT
Recorded byte ownership appears freeze-ready: README and manifest report exact whole-image equality, exact ordered relocations, and zero raw initialized/BSS ownership. This audit found 72 classified open observations (13 A, 4 B, 24 C, 12 D, 19 E, 0 F); none is a demonstrated strict-gate failure.

## STATE CHANGE
Created only `build/workers/F1/audit.md`, `audit.json`, `freeze_gate.md`, and private scratch scan outputs. Canonical sources, recipes, tools, layout, evidence, and acceptance state were not changed. No gate or test command was run.

## STRONGEST EVIDENCE
`layout/manifest.json` has 234 initialized owner rows, 14 BSS owner rows, and zero `UNRESOLVED_RAW` entries; README records exact image and 2,588 ordered relocations. Independent `histbuild.py` remains an attribution diagnostic with 2,023/2,588 relocation positions matching and bank 0 exact. Those are distinct claims.

## HYPOTHESES TESTED
This was a read-only audit, not a compiler experiment. Cross-checked current manifest/recipes, acceptance history, porting matrices, platform-boundary inventory, state/format/audio documents, host-probe records, and toolchain-hypotheses statuses. The documented strict-central host-probe result conflicts (38/38 versus 6/38), so it needs a fresh run.

## NEW KNOWLEDGE
`validate.py`'s BSS real-link check can consume persistent `build/reallink/cache`; `--image` shares it. `histbuild.py` creates a unique cache and asserts uncached builds. The `_int86` platform-boundary extent is stale relative to the seven-word current aggregate. Several older acceptance ?open? rows are superseded by later integrations.

## TOOLCHAIN
No status proposals. The canonical compiler profile remains MSC 5.10 medium `/AM /O /Gs`, with registered TU-specific exceptions; the evidence ledger has 46 entries and explains S7-S10.

## REMAINING BLOCKER
No class F blocker found. The detailed inventory records historical attribution (A), port-runtime implementation (B), source/declaration discrepancies (C), stale or bounded evidence (D), and opaque semantic questions (E), with paths for each.

## RECOMMENDED NEXT ACTION
Have the integrator rerun both host-probe modes and reconcile the strict-central count, then run the exact freeze gate in `freeze_gate.md`; clear only `build/reallink/cache` before validation when a fresh real-link compile is required. Keep the independent histbuild relocation-order residual explicit.

## NEEDS OPUS?
NO.

Read-only audit for the requested sources. No canonical file or tool was changed, and no build/test command was run. The requested gate commands write outside F1; full validation is also explicitly reserved to the supervisor.

## Current authority snapshot

- README records full validation, exact hybrid image and 2,588 ordered relocations, exact validate.py --image, and zero raw initialized/BSS debt. Current manifest has 234 initialized-ownership rows and 14 BSS rows, with no UNRESOLVED_RAW rows.
- Manifest counts: 35 MATCHING_C code, 52 MATCHING_ASM code, 53 pinned runtime code, 19 C data, 19 ASM data, 25 pinned runtime data, 30 LINK_FILL rows, and one BSS_IN_IMAGE row. There are 90 unique active recipes (38 C and 52 ASM), among 230 recipe files. Active profiles: 37 msc510-medium, one msc600-medium-zi, 52 masm510-game; object-flag register entries cover the known exceptions.
- evidence/toolchain-hypotheses.json (updated 2026-09-28) has 46 hypotheses: 6 FALSIFIED, 4 NON_DISCRIMINATING, and S7-S10 EXPLAINED. Matching work remains pinned to MSC 5.10 medium /AM /O /Gs except supported TU-specific exceptions. This audit proposes no toolchain status change.
- No current strict-acceptance freeze blocker (class F) was found. Open items below are attribution/runtime uncertainty, port implementation debt, source declaration/context discrepancies, semantic unknowns, or evidence limitations. Exact ownership does not prove original names, TU boundaries, or the original LINK response.

Classification key: A = real historical uncertainty; B = port-runtime policy/implementation; C = source declaration discrepancy against known machine behavior; D = stale/false-positive/evidence-tool or boundary limitation; E = semantic unknown that can remain opaque without blocking freeze/port; F = genuine freeze blocker. Numeric blocker classes from source docs are preserved.

## Historical attribution and dynamic behavior

| Item | A-F | Finding and evidence |
|---|---|---|
| Original object/TU grouping, general LINK response, arbitrary binding/frame modes | A | Unproven historical attribution, not a byte ownership gap. MIGRATION.md remaining limits; docs/acceptance.md compiler profile/general-link scope; README Historical build. |
| Independent historical link/order | A | histbuild.py uses image-derived game order plus a combined library hypothesis. Its library-order diagnostic has 2,023/2,588 relocation positions (bank 0 exact); the config is an assumption, not the original LINK command. README Historical build; docs/acceptance.md integ42 and integ47. |
| Unique compiler provenance | A | MSC 5.10 is the project invariant, but CC-MSC500-same-flags is NON_DISCRIMINATING. evidence/toolchain-hypotheses.json. |
| PIT channel-0 mode/rate and INT 8 cadence | A | 0xB6 selects channel 2, then the game writes channel-0 data; control state is inherited. Live IRQ rate, BIOS chain count, callback totals lack a trace. docs/porting/README.md; runtime-model.md; runtime-measurements.md. |
| Selected simulation rate, renders, updates, catch-up, replay branch reachability | A | Source shows 10/20 target selection and catch-up-before-render; selected rate/totals and g_rplmodui==2 reachability are unmeasured. runtime-model.md; runtime-measurements.md. |
| Retrace/palette traffic, IVT and keyboard IRQ snapshots | A | Setup polling is known; guest PIT/DAC/status counts, before/live/after vectors and keyboard IRQ counts were not captured. runtime-measurements.md; CAPTURE.md. |
| Runtime audio selection/cadence/port behavior | A/B | Static driver code and caller offsets are mapped, but AD15 selection, callback cadence and actual port traffic are unmeasured; an SDL port also needs an audio service policy. audio-drivers.md; runtime-measurements.md; runtime-model.md. |
| Interactive runtime capture | A | No interactive CPU/I/O/vector capture has been analyzed. CAPTURE.md is a manual DOSBox-X workflow, not an automated gate. docs/acceptance.md integ50; runtime-measurements.md. |

## Port headers and declaration/ABI residuals

These are source/port discrepancies even when PORT_BUILD adapters let GCC compile; they do not alter accepted historical objects.

| Translation unit / item | A-F | Listed residual | Evidence |
|---|---|---|---|
| fardata_11036.c | C | unk_3B1E2 element type versus central width. | port-headers.md Class 2 matrix |
| obj_seg000.c | C | waitm_ms, savedptr_ms, unused_count, pixel_scales widths/pointers; locate_shape_fatal result view; read_file_with_retry 3 vs 4 args; call_read_line 5 vs 6; nullsub_2 callers pass args to parameterless body. | port-headers.md |
| obj_seg001_complete.c | C | track_object aliases absent; plan_memres object used as pointer/array; trackctrpos2 and row_ctr_zs widths. | port-headers.md |
| obj_seg003.c | C | track_object aliases; td10checkptr view and vector/scalar use; resbuftext array/scalar; widths/extents of sky_hgt_world, rotpr, ground_skybox, frmexcess, maxscnh, g_tdist, g_skybox_sky_clr, tsix. | port-headers.md |
| obj_seg004.c | C | track_object aliases; aCar0 aggregate versus byte-matrix use; g_trackpiecescounter, postable, r_zp, xcols, z_ctr_pos widths/extents. | port-headers.md |
| obj_seg005.c | C | Widths for g_audio_frms_ix, snd_tick_clock, g_clocks, viewyshift, replayrst, roofbmphgt_saved, dashbmpy_copy, rplbarabovehgt, dasty, dashbmy9, rfy5; camera arrays have 9 source words versus target extents 8/8/7/7. | port-headers.md; target addresses 0x2EA08/0x2EA1A/0x2EA2C/0x2EA3E |
| obj_seg006.c | C | Types differ for mat_y_rot_angle, polygonnumber, poly_cursor1, facenodeiterator, polygon_link_3_list_iter, poly_link_listit4 and four pointer globals. | port-headers.md |
| obj_seg007.c | C | send_audio_stop_event consumed as value but defined void; audio_tick_divider width. | port-headers.md |
| obj_seg008.c | C | Central views for g_animphase, g_hovercolor_idle, fontdefvalue, kbjoyflags, flagsdown, msecoordx, pos_y_ms, g_mouseyposstacktable, g_mousesave_x_tbl; call_read_line caller arity. | port-headers.md |
| obj_seg009.c | C | track_object alias; gterrtrk and lnoffsets extents. | port-headers.md |
| obj_seg028.c | C | Selected AUDIOVOICE/AUDIOCHUNK views omit source fields; audio_init_chunk caller has 7 args versus 6-arg definition. | port-headers.md |
| obj_seg031.c | C | spritepointermini and mouse_ptr_cursor pointer views; nullsub_2 cross-TU caller view. | port-headers.md |
| obj_seg032_group.c | C | timer_copy_counter callers pass two words; implementation view groups a U32. | port-headers.md |
| seg017_mouse_whole.c | C | ms_buttons, cursorxposition, mouse_api_y source widths versus target widths. | port-headers.md |
| call_read_line / timer_copy_counter | C | Machine entries load six words / combine BP+6 and BP+8; caller views are adapted as five args with a 32-bit final value / one U32. | port-headers.md adapter table; Z1 API digest |
| read_file_with_retry / send_audio_stop_event / locate_shape_fatal / nullsub_2 | C | Omitted argument and consumed return lack target evidence; locate_shape_fatal enters a generic resource thunk; nullsub_2 is retf;nop with no reads. Do not invent signatures/results. | port-headers.md adapter table; Y2 notes |
| audio_init_chunk nonzero far pointer | B | Observed target reads seven words and the call uses 0:0; define host segmented-pointer resolver before link/execute. | port-headers.md Class 6; audio-drivers.md |
| File/resource/audio dispatch hooks | B | stunts_port_read_file_with_retry_dispatch and stunts_port_send_audio_stop_event_dispatch have declarations but no host policy/definition. | port-headers.md Class 4 |
| Camera-array extent / owner boundary | C | Nine-word source arrays conflict with target 8/8/7/7 extents; establish complete owner before assigning data layout. | port-headers.md Class 5 |
| Saved replay PRNG seed | B | .RPL omits the six-byte Kevin seed; port must preserve startup/call order or separately justify seed capture. | runtime-model.md; formats.md replay |

## State, map, formats, and audio semantics

| Item | A-F | Finding and evidence |
|---|---|---|
| Unknown replay-role globals | E | Of 11, aDefault_1/postable/z_ctr_pos were clarified; g_corkscrew_type_flag is narrowed to builder output but consumer unknown; collision_response_offsets[4], unused_40E73, track3_gap[2], framerate_pad_0, extra_rclist4[2], spare_td22_1, gap_trackrow_1 remain opaque. state-model.md. |
| Physical units / physics coefficients | E | 342/547 globals have unknown units; 84 are simulation-assigned/shared, 63 exclusively. Mass, braking, torque, aero, grip and opponent speed scales need evidence. state-model.md. |
| Aggregate/state coverage | E | 27 aggregate tags have no registered instance; same-named types vary by TU; offset fields and escaped-pointer accesses remain opaque. state-model.md. |
| build_obj table pairing | C | obj_seg004 build_obj indexes td15p_9 reversed and td14tb forward, unlike track_setup/reference row roles. Preserve map layout and defer semantic override. formats.md Tracks; docs/porting/README.md. |
| Replay bits 6-7 and byte-to-glyph mapping | E | Player path uses bits 0-5; sample has no upper-bit events; byte-indexed fonts do not prove CP437/Unicode. formats.md Replays; docs/porting/README.md. |
| HIG byte 0x10 and absent CKM/TD | E | HIG byte is not initialized/rendered; no CKM/TD sample or reader. Preserve opaque spans. formats.md; docs/porting/README.md. |
| VCE declared size and VCE records | E | 7/8 VCE samples have a directory size different from physical length; voice records are only partially mapped. formats.md; audio-drivers.md. |
| Fallback formats and external loader | E | No samples for XVS, ESH, VSH, 3SH, DSF, DVC, TD, CKM; DRV/PLB opaque; HDR/COD/DIF external loader grammars not derived; 23 ancillary files inventory-only. formats.md. |
| Shape2D/palette | E | Header words/flags and !MGA mapping semantics incomplete; no !MGA sample; DAC component range/channel conversion not specified. formats.md. |
| P3S/SIMD | E | Primitive flag bits, cull tables, coordinate units, SIMD fields/chart arrays are not semantically decoded despite complete measured spans. formats.md. |
| TRK semantics | E | Terrain trailer and exhaustive terrain/element meanings unknown; adjacency check covers supplied samples only. formats.md. |
| VLE coverage | E | Additive VLE is source-derived but unexercised in supplied files; encoder/padding policy unspecified. formats.md. |
| Uncalled driver slots +2A/+2D/+33/+36/+3C | E | Static stack/register accesses known, shared logical meaning/prototype not; audio-drivers.md labels these BLOCKER CLASS 8. audio-drivers.md. |
| Audio envelope / MT32 patch bank / opaque records | E | Chip register mapping, MT32.PLB internals, codec/sample encoding and some event semantics require runtime/device evidence. audio-drivers.md; formats.md. |
| Editor strings | E | Navigation/actions are source-backed; literal localized text remains packed-resource content. modes-and-editor.md; runtime-model.md. |
| Video clear intent | E | Machine code stores 64,000 words with DI wrap (128,000 byte writes); intended pixel semantics/caller color width uncertain. platform-boundary.json video_clear_color. |

## Platform-boundary inventory (source numeric blocker classes retained)

| Target / range | Source blocker class | A-F | Finding |
|---|---:|---:|---|
| _int86 frame extent | 2 | D | platform-boundary.json says four mouse words, but current src/seg017_mouse_whole.c defines all seven WORDREGS words; current headers assert 14 bytes. Stale extent entry. |
| mouse_init INT 15h C201h | 8 | A | BIOS purpose/other input registers unspecified. platform-boundary.json entry mouse_init. |
| nopsub_36A9A | 8 | A | Mouse X limit scaling is opposite mouse_set_minmax. platform-boundary.json entry. |
| nopsub_19DFF / nopsub_19E09 / nopsub_19E13 | 8 | A | INT 61h/60h/62h provider/service contracts unknown; separate from driver far-call vectors. platform-boundary.json. |
| timer_setup_interrupt | 8 | A | Known control/data channel mismatch; effective channel-0 state/rate conditional. platform-boundary.json; runtime-measurements.md. |
| sprite_1_unk / sub_34526 / sub_35B76 | 8 | E | Surface accesses established, precise pixel operation/name not. platform-boundary.json. |
| sub_345BC | 5 | D | Alternate-entry/data boundary makes combining unsafe; bytes verified, boundary intent not. platform-boundary.json. |
| sprite_make_wnd | 5 | D | Indexed end absent and extent overlap; boundary inference incomplete. platform-boundary.json. |
| seg012 [0x01F436,0x01FDDE) | 8, coverage gap | A | 2,472 unmapped bytes; candidate INSB/OUT/INT3, boundaries/ownership unproved. platform-boundary.md/json. |
| seg012 [0x022A72,0x022AE2) | 8, coverage gap | E | 112 unmapped bytes, no source-confirmed platform op. platform-boundary.md/json. |
| seg012 [0x024CE4,0x025AF6) | 5, coverage gap | D | 3,602 bytes overlap sprite_make_wnd whose end is not verified. platform-boundary.md/json. |
| seg017 [0x026AF2,0x026AF4) | 8, coverage gap | E | Two unmapped bytes, no platform operation assigned. platform-boundary.md/json. |
| seg027 [0x028570,0x02863C) | 8, coverage gap | A | 204 unmapped audio-loader bytes; delegated driver behavior unassigned. platform-boundary.md/json. |
| Negative findings: VGA DAC, port 64h, AdLib 388h, direct Tandy/PCjr I/O, EXE self-read | — | D | “Not found in indexed reviewed code” is qualified by the five coverage gaps; not proof of whole-image absence. platform-boundary.json. |

## Host-probe conflicts and acceptance-history ledger

| Item | A-F | Current reading | Evidence |
|---|---|---|---|
| Legacy GCC host failures | D | Legacy pre-adapter survey has 8/38 failed TUs: obj_seg000, obj_seg001_complete, obj_seg003, obj_seg005, obj_seg007, obj_seg008, obj_seg009, obj_seg027 (seven numeric Class 2, seg007 Class 3). These are not matching-build failures. host-probe.md per-file rows. |
| strict-central result disagreement | D | port-headers.md says compat and strict-central 38/38; host-probe.md and acceptance integ51 say strict-central 6/38. Rerun and reconcile current report before treating host audit as a gate. |
| Former Class 4 kbormouse/audiodriverbinary/primidxcounttab | D | Current port header binds at 0x2B8F8, 0x3060A, 0x2EA62; acceptance integ51 is a prior snapshot. port-headers.md; docs/acceptance.md integ51. |
| word_349A2 | E | Current header maps it to +2 in gamerptrs at 0x349A0, not separate object; acceptance says standalone alias remains deferred. port-headers.md; docs/acceptance.md integ51. |
| integ32 “Still open” fontdefseg / word_30602 / fill 196097 | D | Superseded by integ33 far-data segment-word binding and accepted fill. docs/acceptance.md integ32-33. |
| seg000 static-set prototype and old raw BSS placeholder sections | D | Integ34 accepts complete seg000 statics; integ40 accepts full communal unit; integ42 and current manifest report zero raw initialized/BSS ownership. docs/acceptance.md integ32-42; README. |
| CRT0DAT/raw secondary-data prose in old acceptance sections | D | Historical transition notes are superseded for current ownership state; manifest/README are current authority. docs/acceptance.md integ32/39/42; layout/manifest.json. |
| total_game…total_game_jump | C | 22-byte GAMESTATE_SNAPSHOT struct copy in seg001 versus ten separate seg000 communals; possible merge needs re-verification, while current bytes are exact. docs/acceptance.md integ40 Residuals. |
| race_stats communal merge at 213832 | C | Proposal not published because based on unverified rename/context. docs/acceptance.md integ43 Excluded proposals. |
| hill_offs interior elements / word_2F7D8 | D/E | hill_offs interior names have no independent object/reference; word_2F7D8 ownership conflict remains unbound. docs/acceptance.md integ33/45 and L19-rn report. |
| globalgamesettings vs historical gmcfg; elaptm1 | E | Exact code/bindings retained; original spelling/shape not claimed. docs/acceptance.md integ40/45. |
| Historical raw and current zero raw | D | Older acceptance chapters describe earlier raw debt; latest integ42 and README/current manifest say zero. Do not count earlier snapshots as present debt. docs/acceptance.md integ42; layout/manifest.json. |

## Freeze disposition

Recorded historical acceptance is freeze-ready for byte ownership: no unresolved initialized/BSS rows, whole-image equality and exact ordered relocations. The independent histbuild library-order residual is a real attribution uncertainty but not a failure of the oracle-derived gate. Preserve A/B/C/E caveats without promoting guessed meanings, names, runtime rates, or original link behavior into facts. No class F item was identified.
