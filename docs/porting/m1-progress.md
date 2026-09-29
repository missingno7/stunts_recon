# SDL3 port M1 progress

## Implemented startup path

The port now loads startup archive entries, parses the validated RLE/VLE
resource formats, applies the shape transforms, and resolves the resulting
far pointers through the segmented memory model. The decompressor follows
`tools/porting/format_reference.py` (`decompress`, `decompress_rle`, and
`decompress_vle`); archive lookup and file loading follow
`asm/file_get_shape2d.ASM`, `asm/locate_entries.ASM`, and `asm/file_read.ASM`.
`port/memory.c` now reuses freed real-mode spans, retains full allocation
extents, handles paragraph allocations and normalized chunk names, and reports
free paragraphs. Its call boundaries are documented in
`asm/mmgr_alloc_call_group.ASM`, `asm/mmgr_alloc_pages.ASM`,
`asm/mmgr_free_entries.ASM`, and `asm/mmgr_get_ofs_diff` within
`asm/mmgr_get_ofs_diff.ASM`.

The host palette and indexed 320x200 framebuffer are active. The palette path
uses `asm/video_set_palette.ASM` and
`asm/file_load_shape2d_palmap_apply.ASM`. Sprite-window setup, row offsets,
clipping, opaque drawing, boolean blits, and font drawing have C implementations
beside the SDL host; source references are
`asm/patterned_lines_windows.ASM`, `asm/seg012_shape_to_1_group.ASM`,
`asm/seg012_putimage_shared_group.ASM`, `asm/sprite_clear_1_color.ASM`,
`asm/font_draw_text.ASM`, `asm/font_entries.ASM`, and
`asm/line_sprite_shape_render.ASM`. The `sprite_1_unk3` row sampler is a
semantic C translation of `asm/seg012_sprite_1_unk_group.ASM` lines 146 to 165
and 191 to 291. Its evidence context has no strict recipe, so this translation is
not claimed as historically accepted source.

`port/timer.c` drives the game callback list from the 99.998 Hz host tick and
uses the callback/counter rules in `asm/timer_reg_callback.ASM`,
`asm/timer_remove_callback.ASM`, `asm/timer_get_delta.ASM`, and
`asm/timer_get_counter.ASM`. The SDL keyboard adapter updates DOS scan state
and BIOS-style key reads using the interfaces in
`asm/keyboard_interrupt_runtime.ASM`, `asm/kb_read_char.ASM`, and
`asm/input_keyboard_joystick_services.ASM`. An Enter key event was observed
reaching the game and taking startup into the menu path. DOS audio driver
images remain unloaded as executable code; the silent adapter keeps resource
loading available without calling a DOS driver.

## Build and rendered frames

`python port/build.py build` passes all 38 game C object compiles and links 16
host objects. The generated inventory is **57 function stubs and 3 data
stubs**, down from the M0 inventory of **117 function stubs and 4 data
stubs**. The complete `tools/validate.py` suite was not run; this worktree is
known to have six checks that depend on ignored `build/workers` files.

The startup window can display the Broderbund frame, Stunts title art, and the
main menu after Enter. Captures are kept under ignored `build/sdl3/` output.
Comparisons against Port Forge originals show:

| SDL capture | Port Forge reference | Indexed pixel differences | Palette differences |
|---|---|---:|---:|
| Frame 4 | `checkpoint_000000000300.pfidx` | 811 / 64,000 | 0 / 768 bytes |
| Frame 8 | `checkpoint_000000000600.pfidx` | 3,439 / 64,000 | 0 / 768 bytes |
| Menu frame 12 | `intro-oracle-menu.png` after RGB6 expansion | 12,424 / 64,000 RGB pixels | not applicable |

No screen is pixel-exact yet, so no screen-equality regression has been added.
The title-frame differences all fall at columns `x mod 5 = 4`; 12,049 of the
menu's 12,424 differing RGB pixels fall at the same column phase. This follows
the five-pixel source advance in the `sprite_1_unk3` skip tables, but the
remaining background/state relationship to the fully drawn Port Forge frames
is not yet established. The current captures therefore do not prove a final
title or menu match.

With no key, the first unresolved execution boundary is the `set_projection`
stub. Its assembly is `asm/projection_vector_window.ASM`; `context.py` reports
verified instruction boundaries but no strict recipe or independently
verified caller. Main-menu key input avoids that boundary in the observed
Enter path, but additional menu actions and a pixel-exact menu remain open.

The next acceptance steps are to establish the equivalent menu render state,
resolve the fifth-column mismatch without changing the `sprite_1_unk3` ASM
semantics, continue the startup path from `set_projection`, and add a pixel
comparison regression for each screen once it matches exactly.
