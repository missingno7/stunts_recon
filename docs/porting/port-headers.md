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

`strict-central` writes under `build/porting/host-probe/strict-central/` and removes local prototype/extern views. Integrated result: **24/38 syntax passes and 24/38 object passes**; the 14 remaining failures are Class 2 source declaration, target-width, signature, or aggregate-view disagreements listed below ([report](../../build/porting/host-probe/strict-central/report.md), [raw diagnostics](../../build/porting/host-probe/strict-central/results.json)). This audits source consistency and does not claim the current sources are ready to link against the central declarations. All modes are compile-only and produce host objects unrelated to accepted historical objects.

`tests/test_port_header_layout.py` compiles `stunts_structs.h` when GCC is present. Including it evaluates the generated full schema `sizeof`/`offsetof` assertions; the test also pins representative GAMESTATE, SHAPE2D, and AUDIOCHUNK sizes and GAMESTATE member offsets. It skips when GCC is unavailable.

## Remaining strict-central Class 2 differences

Each row groups the strict-central compiler errors by source TU and symbol/view. The exact locations and compiler messages are retained in the linked raw report. Function-call and global-view disagreements recorded in the Y2 notes remain source-porting work; the header set does not alter matching sources.

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

## Independently bound declarations

Y2 resolved the former Class 4 binding questions in the central header: `kbormouse` is a byte at `0x2B8F8`, `audiodriverbinary` is a four-byte far pointer at `0x3060A`, and `primidxcounttab` is a 16-byte table at `0x2EA62`. `word_349A2` is the segment word at offset `+2` within `gamerptrs` at `0x349A0`, not a separate object. The evidence anchors are in [Y2 PORT_NOTES](../../build/workers/Y2/PORT_NOTES.md) and `stunts_decls.h`; the shared declarations do not invent an independent symbol binding for `word_349A2`.

Aggregate names are not guaranteed to denote one layout across recovered translation units. `track_object` and audio records retain source-selected views where the source definitions establish them. The measured map preserves four size families separately: GAMESTATE 1014/1120, SHAPE2D 12/14/16, SPRITE 8/30, and WheelRect 12/16. `track_constants_module.c:494` remains an intentionally opaque 22-byte SHAPE3D view where no field structure is available.
