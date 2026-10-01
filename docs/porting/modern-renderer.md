# Resolution-independent rendering

The faithful renderer remains the indexed 320Ă—200 visual oracle. The locked
historical sources, resource bytes, projection arithmetic and raster remain
authoritative. Host adaptations belong in `port/` and generated port overlays.

## Presentation contract

The grid aspect is 8:5; the intended physical display is 4:3. Pixels have 5:6
width:height. `port/render/viewport.h` fits a centred 4:3 rectangle inside the
actual SDL render-output size. It permits fractional scaling and downscaling;
nearest-neighbour sampling retains the indexed pixel art. The default window
is 960Ă—720 and requests high pixel density. Mouse events are converted from
window coordinates to render-output pixels before applying the same rectangle's
inverse. Bars clamp to the logical edges. Minimized/zero-size surfaces map to
the origin. No game projection or framebuffer byte is changed.

`tests/test_sdl3_viewport.py` checks exact reference rectangles and every logical
pixel centre across 4:3, wide, portrait, fractional HiDPI and small outputs.
The existing framebuffer/F12 capture checks still exercise raw 320Ă—200 pixels.

## Milestones

1. Correct faithful presentation and mouse mapping.
2. Record the ordered semantic stream at `polyinfo()` without bypassing the
   faithful rasterizer; compare it against the locked shape fixtures.
3. Render those commands with native output triangles and explicit 2D layers.
4. Carry recovered camera-space vertices into native projection.
5. Extend horizontal visibility around the central 4:3 composition.
6. Close special primitives, clipping and screen-space pattern semantics.
7. Validate historical equality, semantic fixtures and simulation equivalence.

Later milestone claims require executable evidence; this document does not
confer renderer acceptance.

## Ordered semantic boundary

`port/render/legacy_adapter.py` instruments only the generated host view of
`polyinfo()`. It copies each record immediately before the existing dispatch.
The faithful calls and reset still run. Commands contain pool index, linked
ordinal, signed depth, primitive type, material, ordered projected points,
selection clip, both resolved colours, wheel colours, pattern class and word.
A zero-word stipple still has a command, while the faithful renderer suppresses
its draw. Sphere point 1 holds a radius. No pointers into game scratch survive.

The exact shape-pipeline test now checks this stream against the independently
executed locked DOS pool and draw chain for 1,124 fixtures: all 194 resources,
the existing 736 wheel camera/object rotations, and 194 additional material
fixtures covering solid, stipple, two-colour and zero-word suppression. Full
64 KiB framebuffer, pool and chain equality still pass in those fixtures.
The production semantic audit also passes after this instrumentation.

## Native backend and layers

The maintained SDL 3.4.16 `gpu` SDL_Renderer runs ordered `SDL_RenderGeometry`
triangles with SDL's own GPU shaders. A supported SDL driver is the fallback.
No GPU API reaches game code. `--renderer=projected` is the intermediate
projected-command backend; JSON `renderer` accepts `faithful` or `modern`.
Faithful remains the default during integration.

Patterns are absolute 8×2 reference-screen masks: high byte on even scanlines,
low byte on odd scanlines, bit 7 minus x modulo 8. Triangles are clipped into
reference tiles and use a nearest-sampled alpha mask. Stipple holes preserve
previous paint; two-colour dither selects foreground/background. Pattern
pixels scale with the corrected reference display, including at odd sizes.
Painter order is retained, without a depth buffer.

A stock geometry audit found eight simple concave faces, so unconditional fans
are unsafe. Deterministic ear clipping handles winding, duplicate/collinear
vertices and concavity. Invalid crossing contours are explicit errors, never
silently dropped. Sphere disks and line ribbons use native triangles; wheels
retain the recovered procedural points and three palette colours.

The guest captures the indexed background before `polyinfo` and tracks explicit
2D writes after it. The final indexed image and write mask form an alpha UI
layer. Equal-colour writes remain opaque; frame differencing would lose them.
The offscreen MCGA row copy carries the mask to the display. Menus remain exact
indexed screens. The host publishes complete layers and command copies under
the existing image mutex and renders only those snapshots. PIT, simulation,
input, publication cadence and replay code are unchanged.

Focused probes check native stipple/dither coverage, painter order, RGB6,
concavity, and explicit UI provenance. All 1,124 DOS shape differential fixtures
remain exact with layer instrumentation present.

## Camera-space projection

The generated `trans_op` observer sits after selection of `mview`, fixed-word
position and visibility masks, before the legacy rectangle rejects. It repeats
`mat_vec` and word additions synchronously; its copied XYZ words cannot mutate
simulation. Scene commands own the widened coordinates, projection and palette
interpretation. The host never reads game globals. Semantic structs explicitly
use pack 8 because the generated game's pack-2 ABI otherwise moves double fields.

`set_projection(xangle,yangle,width,height)` takes half-angles in degrees,
converted by integer floor(degrees*1024/360), and uses recovered fast sin/cos.
The renderer captures the resulting centers (`projectiondata5/8`) and focal
scales (`projectiondata9/10`). It evaluates `cx+X*fx/Z, cy-Y*fy/Z` in doubles.
At 960×720, x scales by 3 and y by 3.6, preserving the intended 4:3 display.
For wider windows at the same height, those scales stay constant; only the
horizontal clip expands. Dashboard views use the original height-dependent
half-angle and center (e.g. 35,151/6,320,151); they are not replaced with an
estimated uniform field of view. Roof/dashboard assets occlude a full world
layer in the central composition, so side areas show world behind that UI.

The z=12 near plane and fixed-word intersections are retained initially.
Separate integer reference projection reproduces clipped adjacent-point
suppression and facing decisions. Captured accepted POLYINFO depth/count
override computed reference values. Sorting is local to each original shape
call; its forced insertion flags and scene call order are preserved. A global
Z sort would be incorrect.

The expanded 1,124-fixture differential now checks camera command type,
material, depth, painter chain and independently projected reference points
against the locked DOS records, alongside unchanged full frame/pool equality.
Native probes prove fractional edge placement and identical central 4:3
composition at 960×720, 1280×720 and 1680×720; UI does not extend sideways.
`--window=WIDTHxHEIGHT --hidden --render-capture-dir=PATH` supports automated
native capture. It writes a native BMP and exact indexed FBR for the same
published frame, with output size and both command counts in a sidecar.

Track candidate enumeration is still the recovered 23-tile footprint at this
milestone. Horizontal clip expansion alone cannot add tiles never submitted;
this is the next integration boundary, not a completed widescreen claim.
