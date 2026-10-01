# Resolution-independent rendering

The faithful renderer remains the indexed 320×200 visual oracle. The locked
historical sources, resource bytes, projection arithmetic and raster remain
authoritative. Host adaptations belong in `port/` and generated port overlays.

## Presentation contract

The grid aspect is 8:5; the intended physical display is 4:3. Pixels have 5:6
width:height. `port/render/viewport.h` fits a centred 4:3 rectangle inside the
actual SDL render-output size. It permits fractional scaling and downscaling;
nearest-neighbour sampling retains the indexed pixel art. The default window
is 960×720 and requests high pixel density. Mouse events are converted from
window coordinates to render-output pixels before applying the same rectangle's
inverse. Bars clamp to the logical edges. Minimized/zero-size surfaces map to
the origin. No game projection or framebuffer byte is changed.

`tests/test_sdl3_viewport.py` checks exact reference rectangles and every logical
pixel centre across 4:3, wide, portrait, fractional HiDPI and small outputs.
The existing framebuffer/F12 capture checks still exercise raw 320×200 pixels.

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
Faithful remains the safe default; modern is explicitly selectable.

Patterns are absolute 8×2 reference-screen masks: high byte on even scanlines,
low byte on odd scanlines, bit 7 minus x modulo 8. Triangles are clipped into
reference tiles and use a nearest-sampled alpha mask. Stipple holes preserve
previous paint; two-colour dither selects foreground/background. Pattern
pixels scale with the corrected reference display, including at odd sizes.
Painter order is retained, without a depth buffer.

A stock geometry audit found eight simple concave faces, so unconditional fans
are unsafe. Deterministic ear clipping handles winding, duplicate/collinear
vertices and concavity. The audit covers all 194 stock resources and 4,482 primitives: 4,226 filled
faces, 161 lines, one sphere and 94 wheels. It finds 773 nonplanar faces,
34 faces with repeated positions and five proper crossings in dominant-axis
projections of warped faces. Detailed regenerated data goes under ignored
`build/render-audit/`; run `python tools/porting/render_geometry_audit.py`.

Simple contours use ear clipping. Crossing/touching contours and collapsed
faces use continuous edge-chain tessellation, derived from the raster's last
minimum-Y / first maximum-Y chains, source-order overwrite and edge-side
selection. This is deliberately neither a triangle fan nor even-odd fill.
Collapsed contours retain a one-reference-pixel footprint. This continuous
interpretation is not claimed pixel-identical to the integer raster; the
faithful raster remains available for exact coverage and tie decisions.

Sphere disks use 64 native triangles. Lines use a one-reference-pixel ribbon
and pixels use a reference-pixel quad, preserving perceived width as output
resolution grows. Wheels retain the recovered 16-point Q14 angle table,
16 tread quads, two 18-point side faces, one 16-point ring and three palette
colours. Intermediate coordinates stay floating point. The differential
geometry test bounds stable-anchor rounding differences to two reference
pixels and separately checks adjacent-ring seam changes caused by eliminating
integer minimum-row ties. Six actual stock wheels are also executed by the
frozen DOS oracle: their ordered recovered-C face callbacks reproduce every
full-wheel pixel exactly. All 22 zero-area faces in those cases paint pixels,
so the modern contour path never discards them merely for zero area.

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

## Wider scene submission and regression closure

Changing projection alone cannot reveal track objects outside the recovered
23-tile submission table. `tile_adapter.py` generates a capture-only copy of
`update_frame` in the host overlay. It uses the same track decoding, instance
construction, animations, LOD and sorting code. Each of the five forward rows
gets 12 extra lateral cells at each edge, inheriting the edge detail threshold.
All 23 original entries remain an ordered subsequence, including heading 2's
late repeated coordinate. All eight source tables are tested at expansions
0, 1, 2, 4 and 12. Forward visibility/LOD remain the game's bounded policy;
this does not invent distant terrain or an unlimited draw distance.

The widened pass captures selected transforms and returns before the old
pool, raster and UI tail. It caches side geometry independently of window size,
so a resize can immediately reproject the last published scene, including
paused replay. The normal pass then runs and retains every indexed pixel.
Reference instances bind to the cached geometry by resource identity, exact
XYZ words, projection, paint, flags and backlight; each accepted descriptor
receives the original depth/count. Unsupported records, capacity failures or
an unbound reference instance explicitly reject the modern scene and present
the faithful image. They never silently omit geometry.

The wrapper compares all 0x460 GAMESTATE bytes before and after every widened
capture. `tests/test_sdl3_renderer_equivalence.py` also drives the same seeded
race with held throttle through 160 simulation boundaries in faithful 4:3 and
modern 4:3, 16:9 and 21:9. At least 100 matching live-frame state snapshots and
90 matching faithful framebuffers/palettes must be byte-identical in every run.
No timing, physics or authoritative input code is changed.

Native pixel probes require the central 960×720 composition to be identical
at 960, 1280 and 1680 output widths with constant height, and verify side-world
coverage below the central dashboard. The production camera converter's
reference-accepted subsequence is checked against all 1,124 DOS shape fixtures.
The full historical validation remains a separate mandatory gate.

For manual comparison, use `--renderer=modern --window=1680x720 --debug` and
press F12. The diagnostics capture contains the indexed reference plus a
native BMP and matching FBR/sidecar from the next presentation. Frame IDs
identify each pair; the native comparison never reads mutable guest state.
`--render-capture-dir=PATH` provides an automated pair after 30 world
presentations. A real SDL GPU smoke capture at 1680×720 submitted 265 native
camera commands versus 43 legacy projected commands without fallback. Those
captures are diagnostic build output, not a new historical oracle.
