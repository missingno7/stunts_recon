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
not a strict historical match. `sprite_1_unk3` follows the row, skip, and
lane-local phase updates in `asm/seg012_sprite_1_unk_group.ASM`; it also has no
strict recipe. Its SDL port previously advanced SI and DI after each byte
copied by `mov al,[si]` / `mov es:[di],al`. Those instructions do not advance
either register; only the explicit skip/phase updates do. Removing the two
extra increments restores the byte lanes and completes the title lettering.
`tests/test_sdl3_sprite.py` exercises all four phases with the real `prod` and
`titl` assets from SDTITL.PVS.

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
| `m1b-title-check/frame-000004.fbr` | `checkpoint_000000000300.pfidx` (splash) | 0 / 64,000 indexed pixels | 0 / 768 RGB6 bytes |
| `m1b-title-check/frame-000008.fbr` | `checkpoint_000000000600.pfidx` (title) | 0 / 64,000 indexed pixels | 0 / 768 RGB6 bytes |
| `m1b-menu-aligned/frame-000016.fbr` | `checkpoint_000000001200.pfidx` (menu) | 0 / 64,000 indexed pixels | 0 / 768 RGB6 bytes |

The reported frame-4 (811 pixels) and frame-8 (3,439 pixels) `x mod 5 = 4`
mismatches came from the extra SI/DI increments in `sprite_1_unk3`, not a
missing pass in `sprite_blit_to_video`. Both captures now match their
references exactly, including the complete title lettering.

The menu reference already contains its software cursor and magenta button
outline. `run_menu` initializes button 0 ("Let's Drive"),
`mouse_timer_sprite_unknown` draws its outline, and `input_checking` draws the
cursor via `mouse_draw_transparent` after mouse coordinates change. The port
needed the INT 33h X coordinate to honor `mousehorscale` at the DOS boundary;
SDL events remain in logical pixels. With the pointer at logical (208,189),
the `frame-000016.fbr` menu image and RGB6 palette match checkpoint 1200
exactly. Fixtures under `tests/fixtures/sdl3/` store compressed indexed bytes
plus RGB6 palette, source identities, and hashes. The documented
`pack_sdl3_reference.py` script packs the Port Forge PFIDX files without
conversion or tolerance, and `tests/test_sdl3_framebuffer.py` asserts exact
bytes for all three screens. `port_video_present` snapshots direct framebuffer
and palette changes at the host presentation boundary, so cursor and hover
drawing written between explicit publication calls is visible and captured.

## Build, inventory, and next work

`python port/build.py build` passes all 38 active game C object compiles and
links 20 host objects. The generated `build/sdl3/stub-inventory.json` currently
reports **41 function stubs and 3 data stubs**, down from the M0 inventory of
**117 function stubs and 4 data stubs**. Port-host C changes remain separate
from historical object ownership; no original asset, oracle, accepted ownership
row, pinned-runtime row, or raw-byte ownership was changed. The function
stubs fail fast and the three data stubs remain zero-filled. Remaining groups
include file/replay/input/audio and cleanup services, shape operations, and
the 3D raster path (`preRender_line`, `preRender_patterned`,
`preRender_sphere`, `preRender_unk`, `draw_line_related`, `skybox_op_helper`).
They are listed in the generated `build/sdl3/stub-inventory.json`.

The clean `tools/validate.py` acceptance run passed: 725 tests (zero failed or
skipped), independent DOSBox-X parity for 90 contributions, fresh
`HYBRID_EXACT` whole-image equality with 2,588 relocations, and BSS real link
status `PLACED`.

Validated initialized-image ownership bytes:

| Ownership class | Bytes |
|---|---:|
| Matching C code | 137,978 |
| Matching C data | 16,640 |
| Matching ASM code | 30,977 |
| Matching ASM data | 5,575 |
| Pinned runtime code | 7,576 |
| Pinned runtime data | 1,192 |
| Link fill | 56 |
| BSS in image | 6 |
| Unresolved raw bytes | 0 |

The first-screen boundary is now exact. M1 does not implement gameplay. M2
should first land one deterministic race-entry frame: load the chosen track,
initialize the 3D renderer and car state, and enter simulation with the 20 Hz
catch-up step. Keep loading, renderer, vehicle initialization, and tick
advancement as separately traceable boundaries; verify the first frame before
expanding to ongoing race input and frame pacing.
