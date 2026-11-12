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

Alt+Enter (including keypad Enter and either Alt key) toggles desktop borderless
fullscreen on the current monitor for both renderers. SDL restores the windowed
geometry on exit. The shortcut is consumed by the host, including repeats and
the Enter release after Alt has been released. Fullscreen and resize events
invalidate the host presentation; the game continues through its usual frame
publication path. `tests/test_sdl3_fullscreen.py` checks desktop mode, monitor
bounds, restored geometry and key routing with SDL's dummy and Windows backends;
the Windows check also verifies the native window has no caption or resize frame.

`tests/test_sdl3_viewport.py` checks exact reference rectangles and every logical
pixel centre across 4:3, wide, portrait, fractional HiDPI and small outputs.
The existing framebuffer/F12 capture checks still exercise raw 320×200 pixels.

## Milestones

1. Correct faithful presentation and mouse mapping.
2. Record the ordered semantic stream at `DRAW_OBJECTS()` without bypassing the
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
`DRAW_OBJECTS()`. It copies each record immediately before the existing dispatch.
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

The guest captures the indexed background before `DRAW_OBJECTS` and tracks explicit
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

The generated `ADD_OBJECT_TO_BUFFER` observer sits after selection of `mview`, fixed-word
position and visibility masks, before the legacy rectangle rejects. It repeats
`mat_vec` and word additions synchronously; its copied XYZ words cannot mutate
simulation. Scene commands own the widened coordinates, projection and palette
interpretation. The host never reads game globals. Semantic structs explicitly
use pack 8 because the generated game's pack-2 ABI otherwise moves double fields.

`setprojectp(xangle,yangle,width,height)` takes half-angles in degrees,
converted by integer floor(degrees*1024/360), and uses recovered fast sin/cos.
The renderer captures the resulting centers (`projectiondata5/8`) and focal
scales (`projectiondata9/10`). It evaluates `cx+X*fx/Z, cy-Y*fy/Z` in doubles.
At 960×720, x scales by 3 and y by 3.6, preserving the intended 4:3 display.
For wider windows at the same height, those scales stay constant; only the
horizontal clip expands. Dashboard views use the original height-dependent
half-angle and center (e.g. 35,151/6,320,151); they are not replaced with an
estimated uniform field of view. The world renders behind the cockpit assets.
In widescreen modern mode, the roof and dashboard bands scale uniformly from
their corrected 4:3 display size until they span the output width. Their inner
edges stay at the published windscreen top/bottom: the enlarged roof extends
above the viewport and the dashboard extends below it, where SDL crops them.
This preserves the visible vertical extent and the world projection. Transparent
cockpit pixels remain transparent; UI within the windscreen retains its original
placement. Mouse hit testing in the widened bands inverts that same transform
using the displayed snapshot, so clickable HUD controls follow their artwork.
At 4:3 and narrower, the original whole-UI composition is retained.

The z=12 near plane and fixed-word intersections are retained initially.
Separate integer reference projection reproduces clipped adjacent-point
suppression and facing decisions. Captured accepted POLYINFO depth/count
override computed reference values. Sorting is local to each original shape
call; its forced insertion flags and scene call order are preserved. A global
Z sort would be incorrect.

The expanded 1,124-fixture differential now checks camera command type,
material, depth, painter chain and independently projected reference points
against the locked DOS records, alongside unchanged full frame/pool equality.
Native probes prove fractional edge placement and identical central world
projection at 960×720, 1280×720 and 1680×720. A separate cockpit probe checks
uniform scale-to-width, outward cropping, alpha holes, fixed windscreen
clearance and unchanged in-world HUD placement, including fractional and
portrait output sizes.
The 2026-10-03 cockpit overscan run passed 837 tests without skips, all 90
independent DOSBox-X contribution checks, fresh `HYBRID_EXACT` equality and the
BSS/runtime link gate. A real GPU capture at 1680×720 verified the full-width
cockpit composition. The live cross-aspect tests retained identical matching
GAMESTATE bytes and authoritative indexed frames/palettes.
`--window=WIDTHxHEIGHT --hidden --render-capture-dir=PATH` supports automated
native capture. It writes a native BMP and exact indexed FBR for the same
published frame, with output size and both command counts in a sidecar.

## Wider scene submission and regression closure

Changing projection alone cannot reveal track objects outside the recovered
23-tile submission table. `tile_adapter.py` generates a capture-only copy of
`drawworld` in the host overlay. It uses the same track decoding, instance
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
XYZ words, projection, paint and flags; the faithful call supplies its
current backlight override and each accepted descriptor
receives the original depth/count. Unsupported records, capacity failures or
an unbound reference instance stop modern mode with a diagnostic and failure
status. GPU submission failures do the same. Modern mode never substitutes the
faithful world; indexed menus and explicit UI assets remain compositor layers.

The wrapper compares all 0x460 GAMESTATE bytes before and after every widened
capture. `tests/test_sdl3_renderer_equivalence.py` also drives the same seeded
race with held throttle through 160 simulation boundaries in faithful 4:3 and
modern 4:3, 16:9 and 21:9. At least 100 matching live-frame state snapshots and
90 matching faithful framebuffers/palettes must be byte-identical in every run.
No timing, physics or authoritative input code is changed.

Native pixel probes require the central world to be identical at 960, 1280
and 1680 output widths with constant height. The cockpit covers the full width
within its original visible vertical bands. The production camera converter's
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

## Camera-switching fixes and native background

The first expanded-pass implementation failed when an external camera exposed
car materials. The pass left `backlightovr8` at 46 before the faithful pass began;
its first cloud then had identical XYZ, projection, paint and flags but a
different override from its cached instance. The wrapper now saves/restores
that latch. A longer replay of the supplied debug inputs also exposed a
47-versus-46 difference caused by expanded object ordering. Geometry binding
now adopts the faithful call's backlight override: material context is an
attribute of the submitted instance, not its geometric identity. XYZ/resource/
projection matching remains exact. A focused command test proves both override
directions resolve the faithful material/palette ID. A paired seeded test
switches F1/F2/F3/F4, enters replay and seeks, checking every matching
live GAMESTATE byte and the indexed framebuffer in stable camera intervals.

The expanded tile footprint can reach side-border neighbours while a hidden
player or absent opponent has horizontal sentinel -1. A neighbour also equal
to -1 must not select that car or read its unset wheel-row local. The generated
wide clone adds explicit visible-car guards; the recovered faithful predicates
remain unchanged. Both supplied minidumps identify a null `shapeptr` dereference
inside `ADD_OBJECT_TO_BUFFER`, but their caller frames do not identify the originating
model. The host overlay also turns any remaining null submission into an
explicit scene error before dereferencing it, rather than a hard crash or an
omitted primitive. The edge guards address a proven invalid source path; the
original crash frame cannot be reconstructed from those dumps alone.

Native mode no longer samples the pre-world 320x200 background or replicates
its edge columns. The guest copies the four original scenery panels, their
palette IDs, camera height, pitch/roll matrix, yaw and recovered projection
into the published snapshot. The panorama uses the original ordered panel
starts 0, 320, 512 and 832, clipped to its 1024-unit periodic extent. Source
`u = center_x + 512 - yaw` is the original forward sample. Native geometry
maps that atlas to a cylinder with angular direction
`theta = (u - center_x - 512 + yaw) * 2*pi/1024`, then applies the captured
column-major Q14 pitch/roll matrix. Vertical height uses the recovered focal
scale and the original 15000-unit horizon distance. Original bitmap content
is retained with nearest-neighbour sampling; no new scenery texture is invented. Small
angular/vertical cells and native frustum clipping avoid the DOS scanline
rotation approximation. Sky/ground bands use the camera world-up halfplane,
including straight-up/down and rolled views. The angular projection is an
intentional modern improvement over the original linear panorama scroll.

Clouds retain their original shape resources, animation angle, material and
fixed transforms. Their native position keeps the circular placement computed
before the DOS `rot_ofs.z = 15000` overwrite. A separate copied native XYZ bank
uses that translation; the exact DOS XYZ bank still binds the faithful
instance. Native clipping admits only clouds in front of the camera. The
original forced-depth projection and every faithful pixel remain unchanged.

Focused native pixel probes verify distinct side panorama content, yaw sign,
90-degree roll, straight-up/down cameras and rejection of an absent sky
snapshot. A presentation probe injects scene/GPU errors and verifies that no
faithful world is published or presented after those errors.

Dirty-rectangle copies of the world window must carry its UI provenance, even
when their row width is less than 320. Treating those partial MCGA publications
as opaque sprite assets leaked indexed world rectangles over native triangles.
The compositor now recognizes aligned copies into the actual video buffer at
any row width. Generic asset blits remain opaque. Native comparison captures
also write a black/white UI-mask BMP, making that boundary directly inspectable.
Frame equality tests require a following simulation frame before comparing a
captured render, because a publication stop can interrupt the final dirty-copy
sequence before the full frame reaches MCGA.

The supplied 3,170-event debug input session was replayed to its normal exit
on the SDL GPU backend without a scene rejection or crash. The final build
passed all 833 tests with no skips, 90 independent DOSBox-X compiler checks,
fresh HYBRID_EXACT whole-image equality with 2,588 ordered relocations, and
the real-link BSS/runtime gates. Historical ownership remains unchanged:
C 154,618 bytes, ASM 36,552 bytes, pinned runtime 8,768 bytes, and zero raw
initialized or BSS bytes. These are finite regression results, not a claim
that every possible runtime scene has been exhaustively covered.

## Native table continuation at track corners

Three subsequent debug sessions stopped at the null-shape guard. Instrumented
replay of the shortest session resolved the submission to a boundary fence:
`fence_codes[1] = 215`, LOD 2, at track column 29 and row 0. The original
`trklst[215]` expression is a DOS data-layout alias. The locked symbol map puts
`trklst` at load address 186376 for 215 fourteen-byte records, `scene2` directly
after it at 189386 for 19 records, and `scene3` at 189652 for 13 records.
Thus index 215 selects `scene2[0]`, whose original models are `g_shapes3d[108]`
and `g_shapes3d[109]`. The native globals were separate and padded; reading
past `trklst` selected neither model.

The generated host consumer now resolves this index space explicitly across
the three tables, for both faithful and modern passes and the track preview.
It preserves every source initializer, geometry choice, LOD and material.
Unknown indices fail explicitly. This also covers overlay indices 222–225.
The new scene-table regression compares model pointers, rotation and all six
scalar attributes for all 247 rows against the immutable DOS load image.
No historical source, declaration, ownership or expected image is changed.

The JSON renderer setting is available in older user configs by adding
`"renderer": "modern"` or `"renderer": "faithful"`; a command-line renderer
argument overrides it. Existing files remain preserved rather than being
automatically rewritten. Native panorama sampling defaults to nearest-neighbour.
