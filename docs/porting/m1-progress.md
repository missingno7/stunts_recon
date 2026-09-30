# SDL3 port M1 progress

## Implemented startup and render services

The startup resource path now loads bounded archive entries, decodes RLE/VLE,
resolves shape records, expands ESH bitplanes, and applies the 16-entry shape
palette map. Expansion follows the layout and MSB-first plane rules in
`tools/porting/format_reference.py::expand_esh` and the operation in
`asm/file_load_shape2d_expand.ASM`. The palette pass follows
`asm/file_load_shape2d_palmap_apply.ASM`. Both assembly references assembled
to their recipe sizes and passed `promote.py --verify-only` with fresh
`HYBRID_EXACT` and ordered-relocation checks (340 bytes and 90 bytes,
respectively). These checks qualify the assembly references; the SDL host C
adapters are separate implementations.

The far-pointer memory manager preserves DOS paragraph allocation, reuse,
normalization, and extents. The palette is held as RGB6 beside a 320x200
indexed framebuffer. Sprite windows, row offsets, clipping, shape composition,
boolean blits, text, and font loading have host implementations. Their source
anchors include `asm/mmgr_alloc_call_group.ASM`,
`asm/mmgr_alloc_pages.ASM`, `asm/mmgr_free_entries.ASM`,
`asm/video_set_palette.ASM`, `asm/patterned_lines_windows.ASM`,
`asm/seg012_shape_to_1_group.ASM`, `asm/seg012_putimage_shared_group.ASM`,
`asm/font_draw_text.ASM`, and `asm/font_entries.ASM`.

`sprite_clear_shape` copies the active sprite rectangle at the SHAPE2D x/y
origin into its backing bitmap. Its 94-byte context (`load_2477e`) has verified
instruction boundaries but no strict recipe or established C owner. A worker
candidate compiled to 148 bytes; `promote.py --verify-only` rejected it because
the complete contribution length differs from the 94-byte target. The SDL
implementation in `port/sprite.c` is therefore recorded as a semantic port,
not a strict historical match. The interlaced `sprite_1_unk3` path follows the
row, skip, and lane-local phase updates in
`asm/seg012_sprite_1_unk_group.ASM`; it also has no strict recipe.

The timer callback list is driven from the 99.998 Hz host tick, and SDL scan
codes update DOS key-down state and the BIOS-style character queue. Enter was
observed reaching the game's `sprite_blit_to_video` full-copy branch during an
interactive capture. `port/polang.c`, `port/projection.c`, `port/matrix.c`, and
`port/vehicle.c` provide the startup geometry and RPM helpers needed by the
intro path. These are host C translations with their individual assembly or
semantic references documented in the source; they do not claim strict
historical ownership.

## Rendered frames and comparisons

The SDL runtime displays the Broderbund splash, Stunts title, the transition
art, and the main-menu scene. Enter advances the intro and reaches the menu.
Captures and trace files are kept in ignored `build/sdl3/` output. Comparisons
against Port Forge indexed framebuffer and RGB6 palette dumps are:

| SDL capture | Port Forge reference | Pixel differences | Palette differences |
|---|---|---:|---:|
| `m1-after-shape-fixcapture/frame-000004.fbr` | `checkpoint_000000000300.pfidx` | 811 / 64,000 indexed pixels | 0 / 768 RGB6 bytes |
| `m1-after-shape-fixcapture/frame-000008.fbr` | `checkpoint_000000000600.pfidx` | 3,439 / 64,000 indexed pixels | 0 / 768 RGB6 bytes |
| `m1-keyburst-capture/frame-000009.fbr` | `palette_cld1_audit/checkpoint_000000001200.pfidx` | 423 / 64,000 indexed pixels | 0 / 768 RGB6 bytes |

The title capture at frame 4 differs only at x mod 5 = 4 (811 pixels, all
black in SDL where the reference contains logo colors). This is consistent
with the four interlaced phases visible in `sprite_blit_to_video` leaving the
fifth column phase unrefreshed in this transition capture. It is a diagnostic
observation, not an established fix: the screen sequence advances after this
capture, and no equivalent stable title checkpoint has yet been matched.

The menu frame 9 has exact palette equality and 423 differing indices, all in
the small region x=105..215, y=119..198. The Port Forge raw framebuffer has a
magenta rectangle and a white mouse cursor over the car there; the rest of the
menu indices are identical. The SDL menu image also has an exact RGB match to
Port Forge's `stunts_forged/build/source-game/intro-native-menu.png`, but that PNG is
diagnostic evidence, not the strict indexed framebuffer-plus-palette oracle.
Neither screen is pixel-exact against an equivalent Port Forge indexed state.
A scan of the ignored `m1-*` captures found 823 unique SDL images; scanning
Port Forge's indexed references found 82 unique images. There was no exact
framebuffer-plus-palette pair among palette-compatible comparisons. No
screen-equality regression has been added, and no mismatch is masked.

## Build, inventory, and next work

`python port/build.py build` passes all 38 active game C object compiles and
links 20 host objects. The generated `build/sdl3/stub-inventory.json` currently
reports **41 function stubs and 3 data stubs**, down from the M0 inventory of
**117 function stubs and 4 data stubs**. Port-host C changes remain separate
from historical object ownership; no original asset, oracle, accepted ownership
row, pinned-runtime row, or raw-byte ownership was changed. The full
`tools/validate.py` suite was not run because this worktree is known to have six
checks that depend on ignored `build/workers` files.

The next visual blockers are to establish an equivalent title checkpoint for
the interlaced capture and a menu reference with the same mouse cursor state as
the SDL framebuffer. The current run reaches the interactive menu without
logging a later unresolved host-service boundary; menu actions beyond the
intro-to-menu path remain unverified.
