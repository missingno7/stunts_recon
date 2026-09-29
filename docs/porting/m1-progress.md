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

## Sprite, timer, random, and font boundary

The next port slice adds the host representations for DOS `sprite1`/`sprite2`
and MCGA windows in `port/sprite.c`, retaining the 16-byte shape header, full
pixel extent, row offsets, pitch, and clip bounds. Opaque shape drawing,
sprite-window allocation/release, descriptor selection/copy, and active-buffer
clear follow `asm/sprite_make_wnd.ASM`, `asm/patterned_lines_windows.ASM`,
`asm/seg012_shape_to_1_group.ASM`, `asm/seg012_putimage_shared_group.ASM`, and
`asm/sprite_clear_1_color.ASM`. Valid in-bounds operations retain the original
indexed bytes; invalid spans unwind rather than crossing their registered
allocation.

`port/timer.c` now dispatches the game's five-slot callback list from the
99.998 Hz host tick, preserves callback order, gates dispatch while the game
marks input as pushed, and implements the 32-bit elapsed/counter reads from
`asm/timer_get_delta.ASM` and `asm/timer_get_counter.ASM`. `port/random.c`
translates the six-byte seed carry logic from `asm/obj_seg002.ASM`.
`port/font.c` selects loaded font records and translates the legacy font
header, glyph-width, and bitplane loops from `asm/font_draw_text.ASM`,
`asm/font_entries.ASM`, and `asm/line_sprite_shape_render.ASM`; the static
fallback remains the complete 1408-byte record in `src/fardata_11039.c`.

The strict SDL3 build now links with 76 function stubs and 3 data stubs,
compared with the M0 inventory of 117 functions and 4 data symbols. A five
second run passes resource/palette loading, cursor-window setup, timer
calibration, random wait, and font selection, then reaches the unresolved
`cosfast` service. No original frame has been captured or compared yet, and no
title/menu screen is claimed as rendered.

The diagnostic `search.py` run for the whole `asm/obj_seg002.ASM` module stopped
at the documented unsupported `COMDEF` communal allocation rule before
comparison (`build/search/5a7954d4-fa05-4237-a9a7-b7758d6dc81b/report.json`).
This tooling obstacle does not affect the C translation path or alter the
accepted ASM/source authority.

## Screen oracle status

No M1 framebuffer has been matched yet. The available
`D:\Games\DOS\dos_recosystem\stunts_forged\pf_stunts_screen.ppm` capture is a
race frame and cannot serve as the title or menu reference. The local
Port-Forge capture executable and saved boot/replay artifacts remain available
for producing named title and menu reference frames.
