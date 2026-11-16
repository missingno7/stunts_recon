# Port-layer inventory and finite coverage

[port-layer.json](../../tools/porting/port-layer.json) is the joined review table
of native substitutions and adaptations, including enhanced-renderer helpers.
Unchanged reconstructed C is outside it; its width/layout adaptations are inside.

| View | Authority and scope |
|---|---|
| ASM replacements | `asm-migration.json`: 241 public entries in 40 reconstructed MASM modules, plus 92 pinned-runtime entries |
| Host services | All 113 `platform-boundary.json` DOS/BIOS, keyboard, mouse, VGA/port-I/O, timer and sound interfaces |
| Adapted C | Fresh `semantic_audit.py` historical/production-overlay join, including observation/input hooks |
| Build adaptations | Rewrite sites in `port/build.py`, adapters and delegated declaration/header/width transforms; source line and enclosing-function hash retained |
| Native helpers/macros | Existing function scanner plus recursive renderer/header helpers and explicit width/qualifier macros |
| Semantic contracts | All 29 scoped contracts, with authority, tests and finite claim limits preserved |

Rows overlap: their sum is not a count of independent DOS APIs. Existing
original-machine differential cases count as finite evidence. Lexical similarity,
compilation or registration alone do not. Every row says `exercised and agrees`,
`exercised and differs`, or `not exercised`. An agreeing row proves its named
bounded cases, never its entire input domain. Driver rows retain ordered-step
differences even when an identical request/return exists in an unordered call set.

The race observes 14 outermost foreground contracts: `intsin`, `intcos`,
`fixedmult`, `intatan`, `inthypot`, `mat_vec`, `transmult`, `zclipline`,
`mysetseeds`, `mygetseeds`, `myrandom`, `normkey`, `normkeyi`, `joystate`.
It also observes defined INT 15h C201/INT 33h register projections and 12 sound
far-entry offsets across AD15/PC15. The 14 routine contracts include bounded
before/after spans, alias offsets and defined returns. Private driver memory,
arbitrary DOS buffers, callback-internal decomposition and the remaining entries
are not newly covered. The step-0 clock-site inventory is linked by hash as
historical evidence; its old line numbers/shared-state model are not current claims.

Step 8 adds eleven optional raster/blit contracts; ten execute in the race's
first 22 frames. Their bounded observations include scalar arguments, vertex
lists, clip words, row 9, and hashes of the dashboard corner/source pixels.
27 real clipped polygon calls additionally run against original machine code.
The refreshed step-9 table has 1423 overlapping rows: 92 with finite agreeing
evidence, seven differing raw ordered sound streams, and 1324 without comparable
coverage. New typed `savefilez` and `instringz` cases cover error returns, line
buffer spans and marker XY; a separate data row covers all 257 MSC ctype bytes.
Scope-specific semantic-contract rows now inherit their named evidence. Full
results-music/replay transition streams are still open, even where a bounded
race sound projection agrees. Unmatched operands never count as passes.

Every remaining row carries `reachability.runtime_entry`, `scenario` and `reason`.
False means no standalone guest entry (build rewrite/macro). Null means runtime
reachability is not established: the suggested scenario/probe is a triage route,
not a fabricated call graph. Missing comparable ABI/span capture does not mean a
function never ran. Pinned MSC routines need a native semantic mapping/direct
machine test; merely replaying a native scenario cannot call the absent MSC entry.
Extract the complete per-row remaining list (1324 entries), without truncation:

```powershell
python -c "import json; r=json.load(open('tools/porting/port-layer.json')); [print(x['id'], json.dumps(x['reachability'])) for x in r['rows'] if not x['exercised']]" > build/workers/portclock/step9/unexercised.txt
python tools/porting/semantic_audit.py --work build/workers/portclock/step9/audit
python tools/porting/port_layer.py --audit build/workers/portclock/step9/audit/report.json --comparison tools/porting/step_oracle_cases/race-boundary-results.json --comparison tools/porting/step_oracle_cases/scenario-results.json
```

The compact result files contain paired comparison counts, not candidate-derived
expected values. Original executable/observer/log seals remain in the corresponding
original fixtures. [Findings](step-divergences.md) distinguish fixed defects,
finite agreeing cases, CPU-dependent observations and still-unproven sound state.

## Measured execution coverage

The SDL3 executable was built with GCC `--coverage` in a separate build directory. The first profile covers the original-reference scenario tests; the second covers the remaining port tests. GCC function counts are joined to each row's portable function, generated overlay target or affected translation unit, or macro expansion users. `status` above remains the original observation status.

A row is in the agreeing bucket when its mapped code ran during the selected original-reference tests and those asserted comparisons passed. Rows whose existing status records a known difference are counted separately. This does not prove every row's full domain.

| Port-layer kind | Agreeing original scenarios | Other tests only | Never executed | Original runs with known differences |
|---|---:|---:|---:|---:|
| `adapted_c` | 79 | 0 | 21 | 0 |
| `asm` | 110 | 0 | 223 | 0 |
| `build_adapter` | 173 | 0 | 3 | 0 |
| `host` | 39 | 0 | 74 | 0 |
| `native_helper` | 508 | 2 | 94 | 7 |
| `semantic_contract` | 24 | 0 | 5 | 0 |
| `semantic_data` | 1 | 0 | 0 | 0 |
| `semantic_macro` | 42 | 0 | 18 | 0 |
| **Total (1423)** | 976 | 2 | 438 | 7 |

Never-executed rows are grouped below. Each note names a likely way to reach the path or explains when the native game has no corresponding entry.

### HISTORICAL_CRT-DEAD-NOP (82)

- `asm:library__getbuf_119742::__getbuf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_abort_123846::_abort` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_abs::_abs` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_amalloc_123070::__amalloc` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_amalloc_123070::__amallocbrk` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_amalloc_123070::__amexpand` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_amalloc_123070::__amlink` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_chkstk_118452::__chkstk` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_chksum_118488::__nullcheck` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_crt0fp_118446::__fptrap` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_brkctl_123426::_brkctl` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_crt0_117858::__amsg_exit` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_crt0_117858::__astart` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_crt0_117858::__cintDIV` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_crt0dat_118056::__cinit` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_crt0dat_118056::__ctermsub` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_crt0dat_118056::__exit` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_crt0dat_118056::_exit` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_crt0msg_118410::__FF_MSGBANNER` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_dosret_119190::__dosret0` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_dosret_119190::__dosretax` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_dosret_119190::__dosreturn` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_dosret_119190::__maperror` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_int86_123916::_int86` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_lseek_122542::_lseek` — Reach it after entering replay, then pause, seek, or save the replay.
- `asm:library_dos_nmsghdr_119038::__NMSG_TEXT` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_nmsghdr_119038::__NMSG_WRITE` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_raise_124318::_raise` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_signal_124534::__sigentry` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_signal_124534::_signal` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_stdalloc_119124::__myalloc` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_stdargv_118526::__setargv` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_dos_stdenvp_118928::__setenvp` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_fflush_120142::_fflush` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_flsbuf_119392::__flsbuf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_flushall_119274::_flushall` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_isatty_123880::_isatty` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_itoa_123808::_itoa` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_nmalloc_122982::__nfree` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_nmalloc_122982::__nmalloc` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_nmalloc_122982::_free` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_nmalloc_122982::_malloc` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldnorm_124904::__aldiv` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldnorm_124904::__ldiv` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldnorm_124914::__almul` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldnorm_124914::__aulmul` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldnorm_124914::__lmul` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldnorm_124914::__ulmul` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldnorm_124934::__auldiv` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldnorm_124934::__uldiv` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldshift_124924::__alshr` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_oldshift_124924::__lshr` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::__output` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::_out` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::_outc` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::flagchar` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::fprint` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::getnum` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::iprint` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::putbuf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::putpad` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::putprefix` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::putsign` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_output_120254::sprint` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_printf_119326::_printf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_rand_124476::_rand` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_rand_124476::_srand` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_sftbuf_119852::__ftbuf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_sftbuf_119852::__stbuf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_sprintf_124044::_sprintf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_stackava_122962::_stackavail` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_strcat::_strcat` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_strcmp::_strcmp` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_strcpy::_strcpy` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_stricmp::_strcmpi` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_stricmp::_stricmp` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_strlen::_strlen` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_strrchr::_strrchr` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_ultoa_123836::_ultoa` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_write_122664::_write` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_xtoa::__cltoasub` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_xtoa::__cxtoa` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.

### LEGACY_SOFTWARE_RENDERER (6)

- `asm:gfxres::_slopeline` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:linerend::_readf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:patlines::_sprite_make_window` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:patlines::sprite_setup1_from_arg_pointer` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:scalblit::_clear_rect` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:whlrast::polycmn` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.

### PLATFORM (106)

- `asm:bmpmask::_andpackf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:bmpmask::_andpackfxy` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:bmpmask::_andpackfxya` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:copyblit::_shape2d_blit_bitmap_hotspot` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fileio::_loadfilea` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `asm:fileio::_loadfileata` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fileio::_loadpack` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fileio::_locate_sound_fatal` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `asm:filewrit::_savefile` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fontmat::_file_decomp_vle` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fontmat::_sprite_pattern_poly_args` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fontmat::_sprite_pattern_poly_args_alt` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fontmat::_vertsync` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fontmat::_vsyncstate` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:gfxres::_abortmessage` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `asm:gfxres::_addexit` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `asm:gfxres::_call_exitlist` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `asm:gfxres::_criterr_interrupt_handler` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_dsiexit` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `asm:gfxres::_filesize` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_filesizea` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_filesizez` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_getbiostick` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_resetcriterr` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_sprite_two_color_poly_args_alt` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_sprite_two_color_poly_list` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:gfxres::_unpacksize` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_unpacksizea` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_unpacksizez` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:gfxres::_video_set_mode4` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:gfxres::_waitbios` — Reach it during timer callbacks or a manual-clock wait in startup, a menu, or a race.
- `asm:iconmask::_andfxy` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbcalls::_currentkeysub` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `asm:kbcalls::_flush_stdin` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `asm:kbcalls::_kb_read_until_deadline` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbcalls::_kb_read_with_timeout` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbcalls::_keysub` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `asm:kbcalls::_nextkey` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbcalls::_nopsub_kb_get_readchar_callbac` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbcalls::_nopsub_kb_set_readchar_callbac` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbcalls::_purgekey` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `asm:kbint::_kb_int16_handler` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbint::_kb_int9_handler` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbint::_keyboard_exit_handler` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `asm:kbjoy::_convertjoy` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `asm:kbjoy::_input_read_char_or_joystick` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbjoy::_interpretclear` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbjoy::_interpretkey` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbjoy::_joybuttons` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbjoy::_joystick_scale_saved_y` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbjoy::_replay_axis_value` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `asm:kbjoy::_reset_joystick_selection` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `asm:kbjoy::_set_bios_mode3` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:kbshift1::_keyboard_shift_checking1` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `asm:linedisp::_draw_unknown_lines` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:linedisp::_sprite_fill_two_color_spans_at` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:linerend::_draw_filled_lines` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:linerend::_readfxy` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:linerend::_scalecfxy` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `asm:mathres::_tickcount` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_freetotop` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_ifmodule` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_initmemsize` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_lockedmemory` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_mmgr_alloc_resmem` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_mmgr_find_free` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_mmgr_init_video_reserve` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_negblockmove` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_poolmemory` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:memmgr::_posblockmove` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:patlines::_draw_patterned_lines` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:pixline::_drawcline` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:rpmrand::_intr0_handler` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:rpmrand::_nopsub_19DFF` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:rpmrand::_nopsub_19E09` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:rpmrand::_nopsub_19E13` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:rpmrand::_restore_div_zero_vector` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:scalblit::_mcgatranslation` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `asm:scalblit::_mcgatransrange` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `asm:scalblit::_scalefxy` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:shp2ops::_movpackcfxy` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:shp2ops::_movpackfxy` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:shpcopy::_movfxy` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:shpthunk::_file_load_shape2d_res_fatal_th` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:shpthunk::_file_load_shape2d_res_thunk` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:shpthunk::_file_load_shape2d_thunk` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:shpthunk::_parse_shape2d_thunk` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:sprvideo::_orpackf` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:textcon::_console_clear_mono_screen` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:textcon::_console_delete_current_row` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:textcon::_console_set_text_attribute` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:textcon::_debug_printf_text` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `asm:timerhlp::_resettick` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:timerint::_restoretimer` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `asm:timerint::_timer_intr_callback` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:timerint::_timer_setup_sound_countdown` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:timerint::_timer_tick_sound_and_chain` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:timerint::_video_set_mode7` — Reach it during timer callbacks or a manual-clock wait in startup, a menu, or a race.
- `asm:vecspr::_clearscreen` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:vecspr::_clippixwindow` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:vecspr::_drawcircle` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:vidmode::_video_add_exithandler` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:vidmode::_video_on_exit` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:whlrast::_preRender_helper` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:whlrast::_preRender_helper2` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:whlrast::_preRender_helper3` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.

### PORTABLE_ALGORITHM (29)

- `asm:fileio::_file_decomp_rle` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fileio::_file_decomp_rle_seq` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fileio::_file_decomp_rle_single` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:fontmat::_mat_invert` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_aldiv_125208::__aFNaldiv` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_almul_125244::__aFNalmul` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_almul_125244::__aFNaulmul` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_alshr_125280::__aFNalshr` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_auldiv_125314::__aFNauldiv` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_ldiv::__aFldiv` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_lmul::__aFlmul` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_lmul::__aFulmul` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_lshr::__aFlshr` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:library_uldiv::__aFuldiv` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:mathres::_nopsub_326BA` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:mathres::_scalethicknessxp` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:mathres::_scalethicknessyp` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:mathres::_thickness` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:mathres::_thicknessxp` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `asm:mathres::_thicknessyp` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:project::_projectorgp` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:project::_setprojectpb` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:rpmrand::_sgn` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:scalblit::shape2d_scale_blit_tail` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:shpthunk::_file_load_shape2d_res_fatal_thunk` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `asm:vecspr::_zclip` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:whlrast::_skybox_op_helper` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `asm:whlrast::_sprite_fill_line_records` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.
- `asm:whlrast::_sprite_fill_line_records_alt` — This pinned routine is not linked as native code; only an isolated original-machine routine probe can reach it.

### audio (40)

- `adapted_c:game/audioeng.c::nopsub_27220` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `adapted_c:game/audioeng.c::nopsub_2726C` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `adapted_c:game/audioeng.c::nopsub_272B0` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `adapted_c:game/audioeng.c::nopsub_27489` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `adapted_c:game/uires.c::msoundtog` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `adapted_c:lib/audioply.c::set_audio_voice_parameter` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `adapted_c:lib/audiores.c::audio_load_driver` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `adapted_c:lib/audiores.c::audio_pause_playback` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `adapted_c:lib/audiores.c::audio_set_chunk_callback` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `adapted_c:lib/audiores.c::audiodrv_atexit` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `adapted_c:lib/audiores.c::restore_audio_volume` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/ad15_driver.c::ad15_driver_36` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::pc15_frequency_table` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::pc15_sample_irq` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::pc15_sample_reload` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `native_helper:port/audio.c::pc15_sample_stream_from_record` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::pc15_scaled_delta` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::pc15_start_sample_stream` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_backend_name` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_driver_03` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_driver_1b` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_driver_36` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_driver_39` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_driver_3c` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_driver_3f` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_driver_42` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_driver_entry_33_supported` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_pc15_sample_byte_count` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_pc15_sample_irq_count` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_pc15_sample_reload_hash` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `native_helper:port/audio.c::port_audio_pc15_unsupported_sample_count` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/audio.c::port_audio_silent_load` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `native_helper:port/pc_speaker.c::effective_reload` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `native_helper:port/pc_speaker.c::port_pc_speaker_divisor` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/pc_speaker.c::port_pc_speaker_gate` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/pc_speaker.c::port_pc_speaker_render_s16` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/platform.c::port_audio_dispatch` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:port/trace.c::port_trace_audio_publication` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:tools/porting/port_include/stunts_api_views.h::stunts_port_audio_init_chunk_7_view` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `native_helper:tools/porting/port_include/stunts_api_views.h::stunts_port_send_audio_stop_event_value_view` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.

### boundary record.c (5)

- `native_helper:port/boundary_record.c::bytes_hash` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/boundary_record.c::port_boundary_graphics_corner` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/boundary_record.c::port_boundary_graphics_row9` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/boundary_record.c::port_boundary_graphics_row9_size` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/boundary_record.c::port_boundary_graphics_source` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### build adapters (3)

- `build_adapter:port/build.py::build_game_objects:16` — `ellipseh` builds a sphere perimeter, but the game has no active `solidellipseh` caller; a sphere-rendering path would be needed.
- `build_adapter:port/build.py::build_game_objects:24` — Reach it when AD15/PC15 installs an audio chunk callback; exercise the longer music/results transition path.
- `build_adapter:port/build.py::build_game_objects:45` — Reach it on the guest fatal-startup or audio-initialization error path; successful scenarios do not request a nonzero exit.

### cleanup.c (1)

- `native_helper:port/cleanup.c::port_guest_exit` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### config.c (1)

- `native_helper:port/config.c::digit` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### diagnostics.c (5)

- `native_helper:port/diagnostics.c::abort_signal` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/diagnostics.c::crash_filter` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/diagnostics.c::crash_writer` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/diagnostics.c::port_diagnostics_directory` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/diagnostics.c::port_diagnostics_trace_path` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### dos.exception (4)

- `host:criterr_interrupt_handler@0x01F35C` — Only a divide-by-zero or critical-error path reaches this DOS handler; ordinary menu/race play does not trigger it.
- `host:init_div0@0x009EE8` — Only a divide-by-zero or critical-error path reaches this DOS handler; ordinary menu/race play does not trigger it.
- `host:intr0_handler@0x009EC9` — Only a divide-by-zero or critical-error path reaches this DOS handler; ordinary menu/race play does not trigger it.
- `host:resetcriterr@0x01F3BC` — Only a divide-by-zero or critical-error path reaches this DOS handler; ordinary menu/race play does not trigger it.

### dos.file (10)

- `host:_lseek@0x01DEAE` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.
- `host:_write@0x01DF28` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.
- `host:filesize@0x01FEA5` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.
- `host:filesizea@0x01FE82` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.
- `host:filesizez@0x01FE94` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.
- `host:loadfileata@0x020AD0` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.
- `host:savefile@0x0224FA` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.
- `host:unpacksize@0x01FF49` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.
- `host:unpacksizea@0x01FF26` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.
- `host:unpacksizez@0x01FF38` — Reach it through track/replay load or save; a missing file or failed write exercises its error path.

### dos.memory (5)

- `host:__myalloc@0x01D154` — Reach it during resource allocation or memory exhaustion; normal startup does not hit the failure path.
- `host:_brkctl@0x01E222` — Reach it during resource allocation or memory exhaustion; normal startup does not hit the failure path.
- `host:initmemsize@0x02111D` — Reach it during resource allocation or memory exhaustion; normal startup does not hit the failure path.
- `host:mmgr_alloc_resmem@0x02107A` — Reach it during resource allocation or memory exhaustion; normal startup does not hit the failure path.
- `host:sub_2E290@0x01E290` — Reach it during resource allocation or memory exhaustion; normal startup does not hit the failure path.

### dos.runtime (8)

- `host:__NMSG_WRITE@0x01D129` — Reach it through the named C runtime startup/exit path; game scenarios do not call this unmapped helper.
- `host:_raise@0x01E5AA` — Reach it through the named C runtime startup/exit path; game scenarios do not call this unmapped helper.
- `host:_signal@0x01E67C` — Reach it through the named C runtime startup/exit path; game scenarios do not call this unmapped helper.
- `host:getbiostick@0x01EAD4` — Reach it through the named C runtime startup/exit path; game scenarios do not call this unmapped helper.
- `host:libsub_quit_to_dos@0x01CE03` — Reach it through the named C runtime startup/exit path; game scenarios do not call this unmapped helper.
- `host:locate_sound_fatal@0x020FA9` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `host:start@0x01CC62` — Reach it through the named C runtime startup/exit path; game scenarios do not call this unmapped helper.
- `host:sub_2CE4A@0x01CE4A` — Reach it through the named C runtime startup/exit path; game scenarios do not call this unmapped helper.

### files and game data (8)

- `native_helper:port/file.c::port_fs_exists` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `native_helper:port/file.c::port_fs_seek` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `native_helper:port/file.c::port_runtime_set_save_root` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `native_helper:port/resource.c::file_load_binary_nofatal_thunk` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `native_helper:port/resource.c::signed_hotspot_offset` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `native_helper:tools/porting/port_include/stunts_api_views.h::stunts_port_read_file_with_retry_view` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `semantic_macro:tools/porting/host/compat.h::__loadds:56` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::loadds:54` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.

### game logic (4)

- `adapted_c:game/carsim.c::calccoll` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `adapted_c:game/uires.c::mdosexit` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `adapted_c:game/uires.c::mmusictog` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `adapted_c:game/uires.c::msystempause` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### graphics (25)

- `adapted_c:game/render.c::logo` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `adapted_c:game/uires.c::msetrender` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/render/edge_chain_tessellator.c::line_strip` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/render/renderer_camera.c::append_unsupported` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/render/renderer_camera.c::point_outcode` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/render/renderer_camera.c::reference_rectcode` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/render/renderer_camera_scene.c::port_camera_scene_reference_only` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/render/renderer_camera_scene.c::port_camera_scene_reject` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/render/renderer_modern.c::port_modern_draw_projected` — Reach it with --renderer=modern during a race scene containing the corresponding wheel geometry.
- `native_helper:port/render/renderer_scene.c::port_render_stream` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/render/triangulate2d.c::port_triangulate_status_name` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/render/viewport.h::port_viewport_cockpit_mouse` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `native_helper:port/render/viewport.h::port_viewport_mouse` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `native_helper:port/sprite.c::raster_shift_right_signed` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/sprite.c::sphere_build_ring` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/sprite.c::sprite_copy_2_to_1_2` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/sprite_aux.c::shape2d_combine_runs` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/sprite_aux.c::signed_word` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/sprite_aux.c::source_view` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/video.c::dump_bmp` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/video.c::port_video_debug_capture` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/video.c::port_video_debug_capture_enabled` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:port/video.c::port_video_mouse_to_logical` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `native_helper:port/video.c::port_video_read_status_1` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `native_helper:tools/porting/port_include/stunts_api_views.h::stunts_port_locate_shape_fatal_shape_view` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.

### input (9)

- `adapted_c:game/uires.c::do_joystick_resource_text` — Reach it from track/replay load or save; a missing asset exercises the error path.
- `adapted_c:game/uires.c::mkeyon` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `adapted_c:game/uires.c::mmouseon` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `native_helper:port/input.c::port_input_dos_scan` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `native_helper:port/input.c::port_input_handle_event` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `native_helper:port/input.c::port_input_queue_event` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `native_helper:port/input_script.c::port_input_script_phase_value` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `native_helper:port/sdl_host.c::mouse_to_logical` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `native_helper:port/vga_timing.h::port_vga_input_status_1` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.

### input.joystick (1)

- `host:joybuttons@0x0205C8` — Reach it with the corresponding keyboard, mouse, or joystick device path in a menu or race.

### input.keyboard (7)

- `host:input_read_char_or_joystick@0x0204B6` — Reach it with the corresponding keyboard, mouse, or joystick device path in a menu or race.
- `host:interpretkey@0x020404` — Reach it with the corresponding keyboard, mouse, or joystick device path in a menu or race.
- `host:kb_exit_handler@0x020883` — Reach it with the corresponding keyboard, mouse, or joystick device path in a menu or race.
- `host:kb_int16_handler@0x0209A5` — Reach it with the corresponding keyboard, mouse, or joystick device path in a menu or race.
- `host:kb_int9_handler@0x0208C6` — Reach it with the corresponding keyboard, mouse, or joystick device path in a menu or race.
- `host:nextkey@0x020A35` — Reach it with the corresponding keyboard, mouse, or joystick device path in a menu or race.
- `host:purgekey@0x020A68` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.

### input.mouse (6)

- `host:mouse_get_position@0x0269B0` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `host:mouse_hide_cursor@0x026A0E` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `host:mouse_show_cursor@0x0269E4` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `host:setmousepos@0x026A2C` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `host:setmousex@0x026A9A` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.
- `host:setmousey@0x026ACA` — Reach it with the named keyboard, mouse, or joystick path in menus or a race.

### main.c (4)

- `native_helper:port/main.c::parse_run_ms` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/main.c::port_guest_set_exit_status` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/main.c::port_guest_unwind` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/main.c::port_stub_fail` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### memory (3)

- `native_helper:port/memory.c::mmgr_alloc_farmem` — Candidate route: allocation, overlap and exhausted-memory cases; the required caller or operand case is not present in the selected runs.
- `native_helper:port/memory.c::port_far_add` — Candidate route: allocation, overlap and exhausted-memory cases; the required caller or operand case is not present in the selected runs.
- `native_helper:port/memory.c::port_memory_stats` — Candidate route: allocation, overlap and exhausted-memory cases; the required caller or operand case is not present in the selected runs.

### other (1)

- `native_helper:tools/porting/port_include/stunts_api_views.h::stunts_port_call_read_line_view` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### platform (2)

- `native_helper:port/sdl_host.c::port_sdl_sleep_until` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.
- `native_helper:port/sdl_host.c::toggle_fullscreen` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### port macros (16)

- `semantic_macro:tools/porting/host/compat.h::__fastcall:58` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::__interrupt:53` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::__pascal:50` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::_cdecl:46` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::_far:42` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::_fastcall:57` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::_huge:44` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::_interrupt:52` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::_near:43` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::_pascal:49` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::cdecl:45` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/host/compat.h::pascal:48` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/port_include/stunts_structs_target.h::STUNTS_LAYOUT_ASSERT:696` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/port_include/stunts_types.h::_far:58` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/port_include/stunts_types.h::_huge:64` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.
- `semantic_macro:tools/porting/port_include/stunts_types.h::_near:61` — This qualifier/declaration macro expands at compile time and emits no standalone runtime instructions.

### runtime library (1)

- `adapted_c:lib/sphhelp.c::solidellipseh` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### semantic contracts (5)

- `semantic_contract:high-risk-uncovered-driver-and-timer` — Reach it with AD15/PC15 playback; the device-specific branch needs that hardware option.
- `semantic_contract:msc-ctype-byte-data` — No selected scenario reached this mapped function; inspect its caller and add the matching menu/error/hardware case.
- `semantic_contract:original-replay-goldens` — Reach it after entering replay, then pause, seek, or save the replay.
- `semantic_contract:pc15-sample-trace` — Candidate route: the contract names its own required operands; the required caller or operand case is not present in the selected runs.
- `semantic_contract:vga-raster-status-model` — Candidate route: the contract names its own required operands; the required caller or operand case is not present in the selected runs.

### software_interrupt (3)

- `host:nopsub_19DFF@0x009DFF` — The old DOS service has no mapped native entry; its scenario requires a direct service adapter.
- `host:nopsub_19E09@0x009E09` — The old DOS service has no mapped native entry; its scenario requires a direct service adapter.
- `host:nopsub_19E13@0x009E13` — The old DOS service has no mapped native entry; its scenario requires a direct service adapter.

### timer.audio (3)

- `host:audio_stop_unk@0x020268` — Reach it from the matching timer/audio callback or hardware timing option.
- `host:tickcount@0x022782` — Reach it from the matching timer/audio callback or hardware timing option.
- `host:timer_intr_callback@0x020329` — Reach it from the matching timer/audio callback or hardware timing option.

### timing (6)

- `native_helper:port/test_seed.c::port_test_random_wait_read_count` — Reach it during timer callbacks or a manual-clock wait in startup, a menu, or a race.
- `native_helper:port/test_seed.c::port_test_random_wait_status` — Reach it during timer callbacks or a manual-clock wait in startup, a menu, or a race.
- `native_helper:port/timer.c::port_timer_game_enabled` — Reach it during timer callbacks or a manual-clock wait in startup, a menu, or a race.
- `native_helper:port/timer.c::port_timer_mark_game_enabled` — Reach it during timer callbacks or a manual-clock wait in startup, a menu, or a race.
- `native_helper:port/timer.c::port_timer_tick_count` — Reach it during timer callbacks or a manual-clock wait in startup, a menu, or a race.
- `native_helper:tools/porting/port_include/stunts_api_views.h::stunts_port_timer_copy_counter_split_view` — Reach it during timer callbacks or a manual-clock wait in startup, a menu, or a race.

### trace.c (1)

- `native_helper:port/trace.c::port_trace_debug_capture` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### vga timing.h (1)

- `native_helper:port/vga_timing.h::port_vga_phase_dots` — Candidate route: startup/race/UI candidate; inspect the named caller before choosing a scenario; the required caller or operand case is not present in the selected runs.

### video.memory (2)

- `host:clearscreen@0x0232A8` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:mmgr_init_video_reserve@0x0210FE` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.

### video.mode (5)

- `host:set_bios_mode3@0x0203D8` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:video_add_exithandler@0x0225AE` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:video_on_exit@0x0225D6` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:video_set_mode4@0x02005E` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `host:video_set_mode7@0x020120` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.

### video.retrace (2)

- `host:vertsync@0x022FEE` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:vsyncstate@0x022FFC` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.

### video.surface (18)

- `host:andpackf@0x023B02` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `host:clippixwindow@0x02320E` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:draw_filled_lines@0x0246BC` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:draw_patterned_lines@0x024B96` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:draw_unknown_lines@0x023344` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:drawcline@0x0233C0` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:orpackf@0x0242F6` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `host:readf@0x02477E` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `host:scalefxy@0x025E08` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:setup_mcgawnd1@0x02A958` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `host:shape_op_explosion@0x0247DC` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:sprite_free_wnd@0x0224AA` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:sprite_make_wnd@0x024C0C` — Reach it while drawing the matching startup/menu/race scene; clipped and transition cases need a matching scene.
- `host:sprite_putimage@0x023BDA` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:sub_34526@0x024526` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:sub_345BC@0x0245BC` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:sub_35B76@0x025B76` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
- `host:sub_35C4E@0x025C4E` — Reach it during a matching VGA mode, framebuffer transition, or legacy drawing path in a menu/race scene.
