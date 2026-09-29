# Port-only declaration headers and TU audit

**These headers are PORTING ONLY and are NOT PART OF THE MATCHING BUILD.** They live in `tools/porting/port_include/`; no matching source or recipe includes them. The historical MSC 5.10 profile, accepted object bytes, and image authority are unchanged by these files.

## Header set and mode behavior

`stunts_types.h` maps recovered scalar widths to host fixed-width types and erases segmented qualifiers for the flat host compiler. `stunts_structs.h` selects source-specific aggregate views and includes `stunts_structs_target.h`, the shared measured target-width schemas. `AGGREGATE_VIEW_MAP.md` maps source declarations and members to those schemas. `stunts_constants.h` collects shared object-like constants. `stunts_decls.h` provides central machine-evidence function and global declarations. `seg007_arith.h` supports a host-only translation of the two original 8086 arithmetic blocks. The declaration basis remains in `declaration-evidence.json`.

The integrated header set combines Y1's schema and view map with Y2's central declaration update. Y1 measured 49 schemas, 49 target-size assertions, 508 member-offset assertions, and 130 source-view mappings; Y2 reconciled 168 API declarations and resolved the three previously unbound declarations listed below. See the [Y1 report](../../build/workers/Y1/REPORT.md), [aggregate view map](../../tools/porting/port_include/AGGREGATE_VIEW_MAP.md), [Y2 report](../../build/workers/Y2/REPORT.md), and [Y2 port notes](../../build/workers/Y2/PORT_NOTES.md).

Run the bounded GCC probe from the repository root:

```powershell
python tools/porting/host_probe.py --mode compat
python tools/porting/host_probe.py --mode strict-central
```

`compat` writes host-only configs, source overlays, and compiler objects under `build/porting/host-probe/compat/`. It supplies the central header plus the local declarations, selected aggregate views, and adapters needed by the current recovered source. Integrated result: **38/38 syntax passes and 38/38 object passes** ([report](../../build/porting/host-probe/compat/report.md), [raw results](../../build/porting/host-probe/compat/results.json)).

`strict-central` writes under `build/porting/host-probe/strict-central/` and removes local prototype/extern views. With the `PORT_BUILD` source adapters, the current result is **38/38 syntax passes and 38/38 object passes** ([report](../../build/porting/host-probe/strict-central/report.md), [raw results](../../build/porting/host-probe/strict-central/results.json)). The earlier unadapted baseline was 24/38; the historical declaration/view differences are retained below. Strict-central is still a compile-only audit: it does not link or execute host code, and a pass does not resolve the semantic/runtime residuals listed below. Both probe modes produce host objects unrelated to accepted historical objects.

The strict-central adapters are guarded by `PORT_BUILD` and `STUNTS_PROBE_STRICT_CENTRAL`; the probe writes transformed source only under `build/porting/host-probe/`. The declarations, adapters, and source rewrites are porting aids, not matching-source or historical-profile changes.

`tests/test_port_header_layout.py` compiles `stunts_structs.h` when GCC is present. Including it evaluates the generated full schema `sizeof`/`offsetof` assertions; the test also pins representative GAMESTATE, SHAPE2D, and AUDIOCHUNK sizes and GAMESTATE member offsets. It skips when GCC is unavailable.

## Pre-adapter strict-central Class 2 differences

This table records the 14-TU baseline that produced 24/38 passes before the strict-central overlays. Some rows are now syntax-adapted for host compilation; that does not repair the matching source declarations or establish runtime semantics. The [Y2 port notes](../../build/workers/Y2/PORT_NOTES.md) retain the caller/source evidence.

| Translation unit | Remaining Class 2 symbols and views |
|---|---|
| `fardata_11036.c` | `unk_3B1E2` source element type conflicts with the target-width central declaration. |
| `obj_seg000.c` | `waitm_ms`, `savedptr_ms`, `unused_count`, `pixel_scales` source declarations conflict with central widths/pointers; `locate_shape_fatal` is consumed as a `SHAPE2D` member view while its central return is a generic far resource; `read_file_with_retry` caller supplies 3 arguments versus the 4-argument definition; `call_read_line` callers supply 5 versus 6; `nullsub_2` callers pass resource and selector arguments while the current body is parameterless. |
| `obj_seg001_complete.c` | `track_object.ss_multiTileFlag` alias is absent from the selected aggregate view; `plan_memres` is one `PLANE` object but this TU uses it as a pointer/array; `trackctrpos2`, `row_ctr_zs` conflict with target widths. |
| `obj_seg003.c` | `track_object.ss_physicalModel` and `.ss_multiTileFlag` aliases are absent; `td10checkptr` is viewed as `VECTOR *` here but as flat target words in this source; vector/scalar arithmetic disagrees at world-coordinate expressions; `resbuftext` is an array but assigned as a scalar; `sky_hgt_world`, `rotpr`, `ground_skybox`, `frmexcess`, `maxscnh`, `g_tdist`, `g_skybox_sky_clr`, `tsix` conflict with central target widths/extents. |
| `obj_seg004.c` | `track_object.multiTile`, `.physicalModel`, `.rotation` aliases are absent; `aCar0` is a 76-byte aggregate but this TU indexes it as a byte-matrix view; `g_trackpiecescounter`, `postable`, `r_zp`, `xcols`, `z_ctr_pos` conflict with target widths/extents. |
| `obj_seg005.c` | `g_audio_frms_ix`, `snd_tick_clock`, `g_clocks`, `viewyshift`, `replayrst`, `roofbmphgt_saved`, `dashbmpy_copy`, `rplbarabovehgt`, `dasty`, `dashbmy9`, `rfy5` conflict with central widths; `game_camera_buttons_x1/x2/y1/y2` source arrays use 9 words while target extents are 8/8/7/7. |
| `obj_seg006.c` | `mat_y_rot_angle`, `polygonnumber`, `poly_cursor1`, `facenodeiterator`, `polygon_link_3_list_iter`, `poly_link_listit4`, and pointer globals `mat_copy_clr_lst_ptr`, `g_mat_clrlist_copy_2_ptr`, `material_patlistptr_copy`, `matpatlistcopypointer2` conflict with central target types. |
| `obj_seg007.c` | `send_audio_stop_event` is consumed as a value here while its implementation returns `void`; `audio_tick_divider` conflicts with central target width. |
| `obj_seg008.c` | `g_animphase`, `g_hovercolor_idle`, `fontdefvalue`, `kbjoyflags`, `flagsdown`, `msecoordx`, `pos_y_ms`, `g_mouseyposstacktable`, `g_mousesave_x_tbl` conflict with central target types/extents; this TU also has five-argument `call_read_line` callers against its six-argument definition. |
| `obj_seg009.c` | `track_object.ss_multiTileFlag` alias is absent; `gterrtrk` and `lnoffsets` conflict with central target extents. |
| `obj_seg028.c` | Selected `AUDIOVOICE` view lacks source fields `resourceIndex`, `active`, `data`, `note`, `state16`, `channelNumber`, `resource`; selected `AUDIOCHUNK` view lacks `channelNumber`, `activeVoices`, `modeValue`; `audio_init_chunk` caller supplies 7 arguments versus the 6-argument definition. |
| `obj_seg031.c` | `spritepointermini` and `mouse_ptr_cursor` source pointer declarations conflict with the central target view; `nullsub_2`'s cross-TU parameter view remains unresolved. |
| `obj_seg032_group.c` | `timer_copy_counter` callers split a 32-bit tick delta into two 16-bit arguments; the central/implementation signature takes one 32-bit value. |
| `seg017_mouse_whole.c` | `ms_buttons`, `cursorxposition`, `mouse_api_y` source declarations conflict with central target widths. |

The remaining API-signature notes also include `call_read_line` (six-parameter definition versus five-argument callers), `audio_init_chunk` (six versus seven), and `locate_shape_fatal` (generic far-resource result versus a `SHAPE2D` member view), all Class 2. Full call-site/source evidence is in the [Y2 notes](../../build/workers/Y2/PORT_NOTES.md).

## PORT_BUILD adapter evidence and remaining residuals

Every adapter below is host-only and is excluded from the matching build. The machine anchors and parameter reads are recorded in the [Z1 API context digest](../../build/workers/Z1/api_context_digest.json) and summarized in the [Z1 report](../../build/workers/Z1/REPORT.md). The probe is a syntax/object-compile check only.

| Blocker class | Adapter or unresolved behavior | Evidence and remaining work |
|---|---|---|
| 2 | `call_read_line` caller view | The target `load_190bc` in seg008 (`+0x1C0C`, 126 bytes) reads six parameter words at `BP+6..BP+0x10`; `obj_seg000.c:115` declares the five-argument caller view, call sites are at `:1328` and `:2774`, and `obj_seg008.c:1180` defines six word parameters. The wrapper splits the final `I32` into low/high words. Source migration and whole-object matching remain separate work. |
| 2 | `timer_copy_counter` caller view | Target `load_227c0` in seg012 (`+0x3DA0`, 23 bytes) adds the words at `BP+6` and `BP+8` into `DX:AX`; `obj_seg032_group.c:34,78,123` declares/passes two words. The host view joins them into one `U32`; source migration remains open. |
| 2 | `read_file_with_retry` and `send_audio_stop_event` | `obj_seg000.c:1150` calls `read_file_with_retry` with three arguments while `obj_seg008.c:1548` defines four; no evidence supplies the omitted value. `obj_seg007.c:158` consumes a result from a call to the `void` definition at `obj_seg028.c:489`; no target return-value evidence is available. Their dispatch hooks intentionally have no implementation. |
| 2 | `locate_shape_fatal` result view; `nullsub_2` call view | `load_20f9d` in seg012 (`+0x257D`, 12 bytes) sets `DX=1` and jumps to a generic resource thunk, which does not establish a payload type. The `SHAPE2D` view follows uses at `obj_seg000.c:716,2412-2418,2531-2650`. `nullsub_2` is exactly `retf; nop` (`load_29e54`, seg031 `+0x44`, 2 bytes), with no parameter reads; caller form and canonical source migration remain separate. |
| 4 | Host file/resource/audio dispatch | `stunts_port_read_file_with_retry_dispatch` and `stunts_port_send_audio_stop_event_dispatch` are declared only; a host integration must define policy and linkage. The probe does not link. |
| 5 | `obj_seg005.c` camera-button arrays | Source initializers at lines 1915-1918 each contain nine words, while central target extents at `0x2EA08/0x2EA1A/0x2EA2C/0x2EA3E` are 8/8/7/7 words. The incomplete host extern views only permit compilation; resolve the complete owning object boundaries before assigning matching ownership. |
| 6 | `audio_init_chunk` far pointer | Target `load_27dbc` in seg027 (`+0x0CEC`, 260 bytes) reads seven words, with the far pointer occupying `BP+0xA/+0xC`; `obj_seg028.c:96,203` exposes the seven source expressions and the observed call uses `0:0`. Define `stunts_port_resolve_far_data_pointer` for nonzero segmented addresses before host linking or execution. |

Other PORT_BUILD type/aggregate views cite their basis beside the declarations: `track_object` aliases preserve same-offset members used by `obj_seg001_complete.c`, `obj_seg003.c`, `obj_seg004.c`, and `obj_seg009.c`; audio records use the source-selected fields at `obj_seg028.c:75-76`; `savedptr_ms`, `td10checkptr`, and the cursor globals retain their source-specific pointer views. The source-selected record layouts are mapped in [AGGREGATE_VIEW_MAP.md](../../tools/porting/port_include/AGGREGATE_VIEW_MAP.md), and machine-backed widths/bindings are in [declaration-evidence.json](../../tools/porting/port_include/declaration-evidence.json). These views do not settle the camera extents or implement host runtime services.

## Independently bound declarations

Y2 resolved the former Class 4 binding questions in the central header: `kbormouse` is a byte at `0x2B8F8`, `audiodriverbinary` is a four-byte far pointer at `0x3060A`, and `primidxcounttab` is a 16-byte table at `0x2EA62`. `word_349A2` is the segment word at offset `+2` within `gamerptrs` at `0x349A0`, not a separate object. The evidence anchors are in [Y2 PORT_NOTES](../../build/workers/Y2/PORT_NOTES.md) and `stunts_decls.h`; the shared declarations do not invent an independent symbol binding for `word_349A2`.

Aggregate names are not guaranteed to denote one layout across recovered translation units. `track_object` and audio records retain source-selected views where the source definitions establish them. The measured map preserves four size families separately: GAMESTATE 1014/1120, SHAPE2D 12/14/16, SPRITE 8/30, and WheelRect 12/16. `track_constants_module.c:494` remains an intentionally opaque 22-byte SHAPE3D view where no field structure is available.
