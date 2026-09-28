# Stunts 1.1 asset file formats (source-backed / sample-validated)

This is a byte-layout note for the original asset set, not a claim that every payload has been semantically decoded. `LE` means little-endian. “Source” citations point into the read-only reference checkout `build/references/restunts/src/restunts/`. Executable validation uses the repository parser at `tools/porting/format_reference.py`; its complete per-file findings are in `build/porting/asset-validation.json` (generated, not committed).

## Coverage and evidence levels

The checked-in asset inventory contains 212 files. The validator catalogued all 212: 189 used an extension-specific parser/opaque-runtime handler; 23 ancillary files were inventoried by extension and size only. It reported zero errors. Specific parsed counts: PRE 9, P3S 14, PVS 46, PES 2, RES 13, TRK 41, HIG 41, RPL 1, FNT 3, KMS 4, SFX 1, VCE 8. It checks 71,340 adjacent terrain edges and byte-round-trips every original TRK, HIG, and RPL (83/83 identical). These are executable facts about the supplied originals, not universal claims about third-party files.

The full asset census is: `.BAT` 1, `.CMN` 1, `.COD` 4, `.COM` 2, `.DAT` 1, `.DIF` 3, `.DRV` 4, `.EXE` 3, `.FNT` 3, `.HDR` 4, `.HIG` 41, `.HTM` 1, `.ICO` 2, `.JPG` 1, `.KMS` 4, `.MD` 1, `.P3S` 14, `.PES` 2, `.PLB` 1, `.PRE` 9, `.PVS` 46, `.RES` 13, `.RPL` 1, `.SFX` 1, `.TRK` 41, `.VCE` 8. `.DRV`/`.PLB` are handled as opaque runtime blobs; `.DAT` is inventoried as opaque setup data. `.XVS`, `.ESH`, `.VSH`, `.3SH`, `.DSF`, and `.DVC` are source fallback routes but have no sample files; `.TD` and `.CKM` are absent and no reader was identified.

Evidence tags used below:

- **Source-derived**: exact fields/control flow are visible in the reference source or disassembly cited.
- **Sample-validated**: the Python reader checks the observed layout across originals; this is not by itself proof that the game interprets all fields the same way.
- **Unknown**: do not rely on the interpretation in a port until another source or executable check resolves it.

## Common resource archive directory

**Source-derived layout** for `.RES` and the decompressed payload of `.PRE`, `.P3S`, `.PVS`, and `.PES`; **sample-validated** for these archives and for the audio samples below.

| Offset | Width | Meaning |
|---:|---:|---|
| `0x00` | 4 | Declared size (`uint32 LE`). |
| `0x04` | 2 | Resource count `N` (`uint16 LE`). |
| `0x06` | `4*N` | Ordered resource names, each exactly four bytes. |
| `0x06+4*N` | `4*N` | Ordered `uint32 LE` payload offsets. |
| `0x06+8*N` | variable | Payload area. Offset zero is relative to this byte. |

The game computes the payload base as `6 + 8*N`, then adds the selected offset; requested names shorter than four characters are padded with ASCII spaces. There is no length field per entry. A chunk's boundary is inferred from the next greater directory offset; the final chunk reaches the file end. Repeated offsets can alias the same start. The runtime lookup does not compare the declared-size word against the physical file length. (Source: `c/memmgr.c:590-623`; shape lookup independently uses the same base/offset formula at `c/shape2d.c:226-237`.)

All 13 sampled `.RES` archives have declared size equal to file size. Audio `.KMS`/`.SFX` samples also do; 7 of 8 `.VCE` samples have a different declared-size value. The VCE directory and offsets parse, but the declared-size word's alternate convention is **unknown**; do not use it as the VCE physical length. Audio files are handed to the driver as opaque buffers by the runtime (`c/fileio.c:1051-1057`, `asmorig/seg027.asm:1425-1549`). Their common directory is sample-validated, not a C-side parsing contract.

## Compression envelope, RLE, and VLE

The packed stream uses a four-byte per-pass header: `type:u8`, `output_size:u24 LE`. Type `1` is RLE; type `2` is VLE. If the first byte has bit 7 set, its low seven bits are the number of passes and bytes 1–3 give the final decoded size; each decoded pass becomes the next pass's input. If bit 7 is clear, byte 0 is the sole pass type. The pass loop and wrapper interpretation are in `c/fileio.c:26-33,825-897`. The original code sizes allocation from the wrapper/header but returns the decoded pass length; the reference parser also checks declared output lengths while decoding.

### RLE pass (`type=1`)

| Pass-relative offset | Width | Meaning |
|---:|---:|---|
| `0x00` | 1 | type = 1 |
| `0x01` | 3 | decoded byte count, `u24 LE` |
| `0x04` | 3 | sequence-stage source byte count, `u24 LE` |
| `0x07` | 1 | reserved; source says always zero |
| `0x08` | 1 | escape count `E`; bit 7 skips sequence expansion; low 7 bits are `E` |
| `0x09` | `E` | ordered escape-byte table |
| `0x09+E` | rest | sequence-stage source, then byte-run stage input |

When the skip bit is clear, only the next `src_count` bytes enter sequence expansion. Escape-table entry 1 is the sequence marker. An ordinary byte copies once. Marker, literal bytes, marker, `count` expands the literal sequence to exactly `count` copies (the source writes the first copy while scanning and repeats it `count-1` more times). The resulting sequence then enters byte-run decoding. When the skip bit is set, sequence expansion is bypassed and byte-run decoding starts at the post-table byte. The reserved byte is not interpreted.

For byte runs, an ordinary non-escape byte copies literally. Escape table entry number `k` (one-based) encodes: `k=1`: following `u8 count, u8 value`; `k=3`: following `u16 LE count, u8 value`; all other `k`: following `u8 value` repeated `k-1` times. Decoding stops after the declared output count. Source: `c/fileio.c:35-41,539-680`. The scratch decoder caps a decoded pass at 32 MiB and rejects truncated runs, impossible chunk bounds, and output overruns.

### VLE pass (`type=2`)

After the 4-byte common pass header: byte 4 contains `W` in low seven bits and additive flag in bit 7; then `W` bytes give symbol counts for code widths 1 through `W`; then the alphabet bytes in width order; then the code bitstream. `W` is at most 16 and the alphabet has the sum of those counts (up to 256) entries. Bits are consumed most-significant first. At each code width `w`, the canonical next code range has `count[w]` entries; after assigning them, the next range base is `(base + count) * 2`. For decoded alphabet byte `a`, non-additive output is `a`; additive output is a running 8-bit sum modulo 256. This is equivalent to the short-width lookup tables and long-width escape recurrence in `c/fileio.c:21-24,683-822`; specifically source state is `esc1[i]=alphabet_count_before-j`, `esc2[i]=j+count[i]`, then `j=2*(j+count[i])`.

The parser decodes every packed original in the sample set, including 62 multi-pass files and all 71 VLE passes whose tables include widths above 8 bits. Three RLE passes set the skip-sequence flag; every observed RLE reserved byte is zero. No supplied VLE pass sets the additive flag, so additive decoding is source-derived but not sample-exercised. The parser consumes only enough bits to produce the declared output length; unused end padding is not format data. Exact encoder/canonical padding policy is not specified because the game only needs a decoder here.

## 2D shape archives, flips, palette expansion

A resource whose payload is a `SHAPE2D` begins with a 16-byte packed header:

| Offset | Width | Meaning |
|---:|---:|---|
| `0x00` | 2 | width, `u16 LE` |
| `0x02` | 2 | height, `u16 LE` |
| `0x04` | 2 | unknown word 1 |
| `0x06` | 2 | unknown word 2 |
| `0x08` | 2 | x origin/position |
| `0x0A` | 2 | y origin/position |
| `0x0C` | 1 | attribute/plane color 0 (`unk3`) |
| `0x0D` | 1 | attribute/plane color 1 (`unk4`) |
| `0x0E` | 1 | flip/plane mask (`unk5`) |
| `0x0F` | 1 | suppression/flags (`unk6`) |

This packed declaration is `c/shape2d.h:4-16`; resource entries are addressed by the common archive directory. Ordinary PVS bitmap data is `width*height` bytes. PES and ESH paths can carry four contiguous `width*height` planes; those are not to be mistaken for one flat displayed image.

**PVS unflip:** if `unk6 & 0xF0` is nonzero, the original loader skips unflip. Otherwise `f=(unk5>>4)`: 0 means unchanged; 1 transposes from source index `x*height+y` into row-major target `y*width+x`; 2 reconstructs alternating row halves as implemented at `c/shape2d.c:244-303`; 3 calls the unhandled/fatal path; 4–15 are left as-is by that C branch. The validator reproduces the in-range copy for mode 2; the C scratch routine can write one discarded row past the target for odd heights, but its final copy-back only covers `height` rows. All 46 PVS originals pass parsing/unflip length checks.

**PES planar unflip:** with the same `unk6` suppression test, the upper nibble `unk5>>4` selects planes 0–3. Selected planes are transposed independently; each plane's address advances by `width*height`, including unselected planes (`c/shape2d.c:345-385`). **ESH expansion:** output width is source width times 8. The packed source byte is expanded MSB-first to eight horizontal output pixels. The initial output pixel is `unk4>>4`; in plane order, the low nibbles of `unk3..unk6` are ORed into each set source bit until the first zero plane color (`c/shape2d.c:387-457`). If the archive contains `!MGA`, its 16 bytes immediately after its 16-byte shape header are a 16-entry pixel-index remap. The loader expands first, then applies `pixel=map[pixel]` to every expanded pixel (`c/shape2d.c:491-540`). The `!MGA` source path is proven, but no sample PES/ESH asset here contains the map chunk; mapping content and real-file coverage remain unproven.

`.PVS` is compressed then unflipped; `.XVS` is compressed without an unflip; `.PES` is compressed, planar-unflipped and expanded; `.ESH` is raw then expanded/remapped; `.VSH` is raw opaque. These dispatch branches are explicit at `c/shape2d.c:543-601`. There are no `.XVS`, `.ESH`, or `.VSH` samples in this asset directory; `.VSH` contents are unknown.

### Palette bytes

A `!pal` resource is a shape chunk: skip its first 16 bytes, then there are 256 RGB triplets (768 bytes). Startup takes exactly these 0x300 bytes (`src/init_main.c:246-261`) and passes them to BIOS INT 10h, AH=10h/AL=12h, the block DAC-register call (`build/references/restunts/src/restunts/asm/seg012.asm:13349-13371`). This establishes triplet order and count. The source does not provide an SDL-specific channel conversion or validate the component range; preserve the raw bytes and validate the target DAC convention separately.

## 3D shape archive

`.P3S` is a compressed common archive; `.3SH` is its raw fallback (`c/fileio.c:1030-1048`). Within each shape chunk:

| Region | Length | Meaning |
|---|---:|---|
| Header | 4 | `numverts:u8, numprimitives:u8, numpaints:u8, reserved:u8` |
| Vertex records | `6*numverts` | Three signed 16-bit LE coordinates per vertex (`VECTOR {short x,y,z}`) |
| Cull table 1 | `4*numprimitives` | opaque per-primitive culling record |
| Cull table 2 | `4*numprimitives` | opaque per-primitive culling record |
| Primitive stream | variable | one record per primitive, as below |

Offsets and record strides are used by `c/shape3d.c:82-90`; header fields by `c/shape3d.h:18-23`; vertex type by `c/math.h:13-15`. Primitive record = `type:u8, flags:u8, numpaints material-index bytes, vertex-index bytes`. The number of vertex indices depends on `type`; exact 16-entry table from `asmorig/dseg.asm:14230-14261`:

| Type | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Vertex-index count | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 2 | 6 | 3 | 0 | 0 |
| Renderer primitive class | 0 | 5 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 2 | 3 | 4 | 0 | 0 |

The renderer reads type, flags, materials, then skips/reads the index list from these tables (`c/shape3d.c:723-775`). The meaning of all flag bits, cull-table bytes, and coordinate units is not specified here. All 14 sampled `.P3S` files and every shape record in them pass bounds and vertex-index checks; there are no `.3SH` samples.

### Car tuning `simd` resource

Each of the 11 sampled `CAR*.RES` files contains a `simd` directory entry of exactly 776 bytes. The game copies `sizeof(struct SIMD)` bytes from that chunk, then replaces the final `aerorestable` pointer with a runtime arena pointer (`c/restunts.c:718-730`). The packed C structure in `c/externs.h:140-171` yields this record layout (integer members are 16-bit in the original build):

| Offset | Width | Field |
|---:|---:|---|
| `0x000` | 1 | `num_gears` (`char`) |
| `0x001` | 1 | `simd_unk` |
| `0x002` | 2 | `car_mass` (`short`) |
| `0x004` | 2 | `braking_eff` |
| `0x006` | 2 | `idle_rpm` |
| `0x008` | 2 | `downshift_rpm` |
| `0x00A` | 2 | `upshift_rpm` |
| `0x00C` | 2 | `max_rpm` |
| `0x00E` | 14 | `gear_ratios[7]` (`u16`) |
| `0x01C` | 28 | `knob_points[7]` (`POINT2D`, two 16-bit ints each) |
| `0x038` | 2 | `aero_resistance` |
| `0x03A` | 1 | `idle_torque` |
| `0x03B` | 104 | `torque_curve[104]` |
| `0x0A3` | 1 | `field_A3` |
| `0x0A4` | 2 | `grip` |
| `0x0A6` | 14 | `field_A6[7]` |
| `0x0B4` | 2 | `sliding` |
| `0x0B6` | 8 | `surface_grip[4]` |
| `0x0BE` | 10 | `simd_unk3[10]` |
| `0x0C8` | 8 | `collide_points[2]` |
| `0x0D0` | 2 | `car_height` |
| `0x0D2` | 24 | `wheel_coords[4]` (`VECTOR`, three 16-bit shorts each) |
| `0x0EA` | 62 | `steeringdots[62]` |
| `0x128` | 4 | `spdcenter` (`POINT2D`) |
| `0x12C` | 2 | `spdnumpoints` |
| `0x12E` | 208 | `spdpoints[208]` |
| `0x1FE` | 4 | `revcenter` (`POINT2D`) |
| `0x202` | 2 | `revnumpoints` |
| `0x204` | 256 | `revpoints[256]` |
| `0x304` | 4 | `aerorestable` far pointer slot; replaced after load |

Total is `0x308` bytes. The C layout/size is corroborated by the fixed sample chunk length; the 4-byte pointer slot is not a portable on-disk address. The parser verifies all 11 `simd` spans are exactly 776 bytes but does not assign meanings to unknown fields or interpret chart arrays.

## Tracks `.TRK`

Exact file length is `0x70A` (1802) bytes: element plane `[0x000,0x385)` and terrain plane `[0x385,0x70A)`, each 901 bytes. Each plane contains a row-major 30×30 grid (900 bytes) plus one trailer byte. The editor read/write calls use exactly `0x70A` bytes (reference disassembly `asmorig/seg009.asm:2199-2205,2291-2300`); `c/restunts.c:262-335` allocates separate 0x385-byte element and terrain map areas. Element trailer `[0x384]` is passed to skybox loading (`c/restunts.c:801`); terrain trailer meaning is unknown.

Grid cell `(x,y)` maps to `element[30*(29-y)+x]` for the main track element map and `terrain[30*y+x]` for terrain (`c/restunts.c:236-247`; element and terrain fetches in `asmorig/seg004.asm:4303-4338`). Thus the element plane's row direction is reversed in the gameplay map while terrain rows are forward. These are unsigned byte code planes, not packed bits.

Terrain code is an index into four 20-byte edge-connection tables: E→W `(0,0,0,0,0,0,1,2,1,3,0,2,3,0,0,1,1,3,2,0)`; W→E `(0,0,0,0,0,0,1,2,0,3,1,0,0,3,2,2,3,1,1,0)`; N→S `(0,0,0,0,0,0,1,1,5,0,4,5,0,0,4,1,5,4,1,0)`; S→N `(0,0,0,0,0,0,1,0,5,1,4,0,5,4,0,5,1,1,4)`. The track checker requires adjoining edges to match; source tables and checker are `asmorig/dseg.asm:13506-13584` and `asmorig/seg004.asm:4077-4258`. Code 6 is explicitly called hilltop (`seg004.asm:4311-4316`). Other terrain names/physics meaning are not asserted. Across 41 originals, parser checks 71,340 internal horizontal/vertical adjacency pairs with zero mismatch; it does not claim edge-boundary sentinel checks.

Element bytes below `0xB6` index the track-object catalog (the rendered element can comprise several shapes; `c/externs.h:173-199`, `c/frame.c:36`); runtime normalizes values `0xB6..0xFC` to code 4 and `0xFD..0xFF` to 0 (`asmorig/seg004.asm:4339-4356`). Exact special start/orientation markers observed by the track setup routine: `01,86,93` → angle `0x000`; `87,94,B3` → `0x200`; `88,95,B4` → `0x100`; `89,96,B5` → `0x300` (`asmorig/seg004.asm:4264-4295,4358-4384`). Other element-byte semantics are catalog driven and are not exhaustively renamed here. All 41 originals meet exact file size and serialize back identically.

## Replays `.RPL`

A replay is `GAMEINFO[0x1A] + track-map[0x70A] + input[N]`, hence exact length `0x724 + N`. The 26-byte packed header is:

| Offset | Width | Meaning |
|---:|---:|---|
| `0x00` | 4 | player car identifier bytes |
| `0x04` | 1 | player material |
| `0x05` | 1 | player transmission |
| `0x06` | 1 | opponent type |
| `0x07` | 4 | opponent car identifier bytes |
| `0x0B` | 1 | opponent material |
| `0x0C` | 1 | opponent transmission |
| `0x0D` | 9 | track name bytes |
| `0x16` | 2 | frames per second, `u16 LE` |
| `0x18` | 2 | recorded input/frame count, `u16 LE` |

Fields/packed size are in `c/externs.h:11-24`. Then at `0x1A`, 901 element bytes plus 901 terrain bytes in `.TRK` plane order; at `0x724`, one input byte per recorded frame. The assembly writer adds `0x724` to `game_recordedframes` and writes that many bytes (`asmorig/seg005.asm:1967-1998`); the reader copies the first 13 words into `GAMEINFO` (`seg005.asm:1925-1965`). The sample is 12,474 bytes with 10,646 frame bytes. All map/event boundaries and `0x724+N` length are validated by the parser, and the sample round-trips byte-identically. The separate semantic C writer is not used as evidence for this layout because its implementation does not include the embedded map consistently.

The input byte is the recorded digital control mask. Bit mapping supported by the control paths:

| Bit | Meaning |
|---:|---|
| 0 | accelerate |
| 1 | brake |
| 2 | steer right |
| 3 | steer left |
| 4 | shift up |
| 5 | shift down |
| 6–7 | no meaning established |

The low two bits are consumed as accelerator/brake states (`c/statecar.c:410-430`); bits 2–3 select steering (`c/state.c:47-49`); bits 4–5 trigger shifts (`c/statecar.c:80-88`). Keyboard/joystick masks are composed in `asmorig/seg012.asm:4010-4068`; A/Z also set bits 4/5 (`asmorig/seg005.asm:1365-1388`). This byte does not capture every mouse-mode analog steering side buffer; those are separately stored by the input path (`seg005.asm:1348-1367`).

## High scores `.HIG`

File length `0x16C` = seven records of `0x34` (52) bytes. The writer copies 26 words per record and writes exactly `0x16C` bytes (`asmorig/seg000.asm:2305-2326,2919-2965`). Original 41 files all parse and round-trip byte-identically. The source evidence establishes record count, stride, and raw bytes but not a reliable field-by-field schema. The 52 record bytes remain opaque; do not parse strings/numbers by visual sample patterns.

## Fonts `.FNT`

The reader uses a 16-bit height at `0x0E`, fixed glyph width at `0x10`, variable-width flag at `0x14`, and a 256-entry `u16 LE` relative offset table at `[0x16,0x216)`. Offset zero means absent glyph. Nonzero offsets point to glyph data in the same font segment. For variable width, glyph begins with a `u8 width`; fixed-width glyphs omit this byte and use `u16` width at `0x10`. Each glyph then stores `height * ceil(width/8)` bitmap bytes, one row at a time, MSB leftmost. These reads are visible in `asmorig/seg012.asm:8870-8902,11082-11190`; font height setup is at `asmorig/seg008.asm:4336-4354`. Header bytes not listed remain unknown.

All three originals have height 8. `FONTDEF.FNT` and `FONTN.FNT` are variable width (111 and 101 nonzero pointers); `FONTLED.FNT` is fixed width 6 (17 glyph pointers). The parser verifies every nonzero pointer and bitmap extent for all three files.

## Audio resource containers and raw runtime payloads

The game requests song `.KMS`, optional/legacy `.DSF` then `.SFX` for effects, and optional `.DVC` then `.VCE` for voice (`asmorig/seg027.asm:1564-1720`). The extension selection and direct-file/compressed-file fallbacks are at `seg027.asm:1425-1549`. The driver receives the resulting song/voice buffers (`c/fileio.c:1051-1057`). The supplied `.KMS`, `.SFX`, and `.VCE` files all sample-validate as the common directory layout above: KMS 4 files/1 chunk each, SFX 1 file/9 chunks, VCE 8 files/8–25 chunks. `.DSF` and `.DVC` are not present. Chunk payloads, the audio directory's declared-size convention for VCE, and container codec/sample encodings are driver-level **unknowns**.

`.DRV` files are loaded as raw driver binaries; `.PLB` (sample `MT32.PLB`) is an opaque patch-bank blob passed toward the driver. No internal byte schema is asserted. `.DAT` in this asset set is `SETUP.DAT`, used by the separate loader/setup path according to `assets/README.md`, not a runtime Restunts format established by the inspected source. `.CKM` and `.TD` are absent; no matching file reader was identified in the inspected Restunts source. The runtime's `trackdata` arena is not evidence of a `.TD` file.

## Remaining asset types not specified as game data

`assets/README.md` identifies `.HDR` as a 30-byte MZ-style reconstructed-program header, `.COD` as a custom-packed video code overlay, `.DIF` as its diff/relocation patch, `.CMN` as common segment, and `LOAD.EXE` as the loader that reconstructs/runs the program. These asset roles are documented, but this work did not inspect the external loader implementation or derive their byte grammars. They are not parsed as Restunts `fileio` gameplay formats. `.BAT`, `.COM`, `.EXE`, `.HTM`, `.ICO`, `.JPG`, and `.MD` are catalogued but not assigned a gameplay format here. The 23 such ancillary inventory entries and their exact file list/sizes appear in `build/porting/asset-validation.json`.

## Reproduce validation

From the repository root:

```powershell
python tools/porting/format_reference.py
```

The script reads `assets/` only and writes `build/porting/asset-validation.json`. Acceptance for this spec run: 212 catalogued, 189 typed/opaque-handler files, 23 ancillary inventory-only, zero parser errors; TRK terrain adjacency checks 71,340/71,340; structural parse/reassembly round-trips 83/83 identical (`.TRK` 41, `.HIG` 41, `.RPL` 1). These checks preserve raw planes/records/header-map-input slices; they do not constitute semantic encoders for those formats. No archive or audio writer was established.
