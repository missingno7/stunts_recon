# Port-only declaration headers

**PORTING ONLY: NOT PART OF THE MATCHING BUILD.** These headers express a shared host-facing view of recovered declarations and source-selected aggregate layouts. They are not included by `search.py`, `promote.py`, historical recipes, or the matching build.

`stunts_types.h` maps target scalar widths and documents erased segmented qualifiers. `stunts_structs.h` selects per-translation-unit aggregate layouts and includes `stunts_structs_target.h`, the measured target-width schemas with compile-time size and member-offset assertions. `AGGREGATE_VIEW_MAP.md` maps source views to those schemas. `stunts_constants.h` collects shared object-like constants. `stunts_decls.h` centralizes machine-evidence prototypes and globals, while retaining explicit per-TU layout selectors. `seg007_arith.h` supports the host-only translation of the two 8086 arithmetic blocks in `obj_seg007.c`. `declaration-evidence.json` records the machine/source basis and known unresolved declarations.

The integrated set combines Y1's aggregate schema and view map with Y2's central declaration update. It is self-contained for `python tools/porting/host_probe.py --mode compat` and `--mode strict-central`. Generated configs, source overlays, and compiler objects are written under ignored `build/porting/host-probe/`.
