# Port-only declaration headers and TU audit

**These headers are PORTING ONLY and are NOT PART OF THE MATCHING BUILD.** They live in `tools/porting/port_include/`; no matching source or recipe includes them. The historical MSC 5.10 profile, accepted object bytes, and image authority are unchanged by these files.

## Header set and mode behavior

`stunts_types.h` maps recovered scalar widths to host fixed-width types and erases segmented qualifiers for the flat host compiler. `stunts_structs.h` provides source-selected aggregate views using `STUNTS_TU_*` selectors. `stunts_constants.h` collects shared object-like constants. `stunts_decls.h` provides the central machine-evidence function/global declarations. `seg007_arith.h` supports a host-only translation of the two original 8086 arithmetic blocks. The declaration evidence is in `declaration-evidence.json`.

Run the bounded GCC probe from the repository root:

```powershell
python tools/porting/host_probe.py --mode compat
python tools/porting/host_probe.py --mode strict-central
```

`compat` writes host-only configs and source overlays under `build/porting/host-probe/compat/`; it syntax-checks and compiles each of the 38 active C translation units to an object. It supplies the central header plus the local declarations, selected aggregate views and adapters needed to compile the current recovered source. Result: 38/38 syntax passes and 38/38 object passes.

`strict-central` writes under `build/porting/host-probe/strict-central/` and removes local prototype/extern views so each TU sees the shared central declarations. It compiles 6/38 TUs; 32 fail on source/target declaration or aggregate-view differences. This is an audit of source consistency, not a claim that those TUs are ready to link with the central declarations. `--mode legacy` retains the earlier compatibility-shim hazard survey documented in [host-probe.md](host-probe.md).

All modes are compile-only. They do not link or execute the game, and their GCC objects are unrelated to accepted historical objects.

## Remaining declaration and aggregate differences

The strict central audit reports **BLOCKER CLASS 2** for the rows below: current recovered per-TU declarations or calls differ from the shared machine-evidence prototype, or their aggregate field view differs. Examples are representative; the generated report and raw diagnostics under `build/porting/host-probe/strict-central/` retain the full compiler feedback.

| Translation unit | Representative residual |
|---|---|
| `audio_make_filename.c` | `audio_make_filename` definition has a different parameter list from the shared declaration. |
| `fardata_11036.c` | `unk_3B1E2` array declaration differs in element type. |
| `file_get_unflip_size.c` | `file_get_unflip_size` return/parameter spelling differs. |
| `file_load_shape2d_expandedsize.c` | `file_load_shape2d_expandedsize` return/pointer declaration differs. |
| `obj_seg000.c` | `waitm_ms`/`g_is_busy` declarations; `file_build_path`, `show_dialog`, and other calls disagree in arity. |
| `obj_seg001_complete.c` | `opponent_op` and car-state prototypes/arity; `track_object.ss_multiTileFlag` field view. |
| `obj_seg003.c` | `update_frame`/`skybox_op_helper2` declarations; `track_object.ss_physicalModel` and `.ss_multiTileFlag`; helper return/arity. |
| `obj_seg004.c` | `bto_auxiliary1` and resource-helper declarations; `track_object.multiTile`, `.physicalModel`, and `.rotation` field names/layout. |
| `obj_seg005.c` | Audio globals and replay/dialog signatures; `file_load_replay` arity. Also references the unbound `kbormouse` global (Class 4 below). |
| `obj_seg006.c` | `calc_sincos80` and rectangle helper prototypes/calls. Also references unbound `primidxcounttab` (Class 4 below). |
| `obj_seg007.c` | `timer_reg_callback`, `pad_id`, and `audio_init_engine` signature/arity differences. |
| `obj_seg008.c` | `file_build_path`, `file_combine_and_find`, and resource helper signatures/arity. Also references unbound `kbormouse` (Class 4 below). |
| `obj_seg009.c` | `track_object.ss_multiTileFlag`; `show_dialog`, `file_build_path`, and other call arities. |
| `obj_seg016_group.c` | Resource lookup and `nopsub_*` declarations differ; `locate_shape_*` call arity. |
| `obj_seg027.c` | `audioresource_*` function parameter views and `debug_printf_text`/runtime calls. |
| `obj_seg028.c` | `AUDIOCHUNK`/`AUDIOVOICE` field views differ (`activeVoices`, `channelNumber`, `modeValue`, `resourceIndex`, `active`, `data`); audio entry signatures. Also references unbound `audiodriverbinary` (Class 4 below). |
| `obj_seg029.c` | `audioresource_compare_chunknames`, `audioresource_get_chunk_index`, `audioresource_find`, and copy helper prototypes. |
| `obj_seg031.c` | File/shape loader declarations and `kb_reg_callback`/resource call arity. |
| `obj_seg032_group.c` | `read_line` declaration and `set_add_value` arity. |
| `obj_seg035_group.c` | Shape-resource loader and memory-manager declarations/arity. |
| `polarRadius3D.c` | `polarRadius3D` definition differs from the shared prototype. |
| `preRender_sphere_helper.c` | `preRender_sphere_helper` definition differs from the shared prototype. |
| `preRender_sphere_helper2.c` | `preRender_sphere_helper2` pointer/aggregate view differs. |
| `preRender_wheel.c` | `preRender_wheel` prototype and `preRender_wheel_helper` arity. |
| `preRender_wheel_helper.c` | `preRender_wheel_helper` pointer/aggregate view differs. |
| `preRender_wheel_helper2.c` | `preRender_wheel_helper2` pointer/aggregate view differs. |
| `preRender_wheel_helper3.c` | `preRender_wheel_helper3` pointer/aggregate view differs. |
| `seg017_mouse_whole.c` | Mouse function/global source spellings differ in scalar widths and pointer forms. |
| `seg024_matrot.c` | `mat_rot_x` and `mat_rot_z` prototypes differ. |
| `seg034_shape2d_group.c` | Shape loader prototypes, `palmap` element declaration, and `file_unflip_shape2d`/resource call arity. |
| `sprite_1_unk4.c` | `sprite_1_unk4` definition differs from the shared prototype. |
| `track_constants_module.c` | Shared array declarations differ in element type/extent (`aBarn`, track-bound tables). |

Aggregate names are not guaranteed to denote one layout across recovered translation units. `track_object` field use differs across `obj_seg001_complete.c`, `obj_seg003.c`, `obj_seg004.c`, and `obj_seg009.c`; the audio source views use distinct `AUDIOCHUNK`/`AUDIOVOICE` and `AudioChunk`/`AudioVoice` tags. The header retains per-TU selectors where source definitions are available. An unconditional struct replacement would erase those distinctions and is not supported by this audit.

## Missing independent bindings

These references remain **BLOCKER CLASS 4** (TU/link/binding infrastructure). `kbormouse` is used by `obj_seg005.c` and `obj_seg008.c`, `audiodriverbinary` by `obj_seg028.c`, and `primidxcounttab` by `obj_seg006.c`. None has a grounded address/type row in the current names registry, state model, or type-inference inventory. The central header does not invent declarations for them. Add a shared declaration only after an independent binding is established.
