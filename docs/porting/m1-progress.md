# SDL3 port M1 progress

## Resource and allocation boundary

The first M1 slice implements the startup resource path in `port/resource.c`:

- `file_decomp` translates the validated RLE/VLE wrapper, RLE sequence/run,
  and canonical VLE logic from `tools/porting/format_reference.py` (`decompress`,
  `decompress_rle`, and `decompress_vle`). It bounds inputs and decoded spans
  to 32 MiB and retains each complete decoded allocation.
- `file_get_shape2d` follows the indexed archive offset calculation in
  `asm/file_get_shape2d.ASM`; `locate_shape_*` follows the four-byte entry scan
  in `asm/locate_entries.ASM`. `file_unflip_shape2d` and
  `file_unflip_shape2d_pes` use the validated transforms in
  `tools/porting/format_reference.py` (`unflip_pvs` and `unflip_pes`).
- The shape-load thunks forward to the existing `file_load_shape2d*` C
  implementations. `port/memory.c` now resolves registered byte extents,
  paragraph counts, normalized DOS chunk names, release, and in-place resize
  within reserved segment spans. The existing `mmgr_alloc_resbytes` C routine
  remains authoritative and retains its one-extra-paragraph behavior.

`python port/build.py build` passed all 38 game-object compiles and linked the
strict i686 SDL3 executable with 95 function stubs and 4 data stubs, down from
117 and 4 at the M0 baseline. A 4-second guest run loaded the startup resources
and palette, then stopped at `sprite_make_window`; this confirms the first
unsupported boundary in execution order, not a rendered-screen match.

The generated inventory is `build/sdl3/stub-inventory.json` and is refreshed by
the port build. The startup trace reports `sprite_make_window` as the next
service to implement.

## Screen oracle status

No M1 framebuffer has been matched yet. The available
`D:\Games\DOS\dos_recosystem\stunts_forged\pf_stunts_screen.ppm` capture is a
race frame and cannot serve as the title or menu reference. The local
Port-Forge capture executable and saved boot/replay artifacts remain available
for producing named title and menu reference frames.
