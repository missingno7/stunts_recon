# SDL3 semantic audit

Run `python tools/porting/semantic_audit.py`. The default compiler is the
i686 MinGW compiler used by `port/build.py`; `--gcc` selects another installation.
The command freshly prepares the production overlays and writes
`build/porting/semantic-audit/report.json` and `report.md`. `--inventory-only`
omits compiler checks and cannot establish layout coverage.

This is a first milestone, not a proof of whole-game equivalence. The frozen
historical image, accepted C/ASM, and oracle lock remain the authority. The audit
does not edit them. Detailed generated inventories live under ignored build
output; the small maintained inputs are [semantic-contracts.json](../../port/semantic-contracts.json)
and [semantic-spans.json](../../port/semantic-spans.json).

## What is scanned

The audit uses the existing declaration parser and the same production
transformation path as the executable builder. It scans all 38 accepted C
translation units, every native `port/*.c` provider, and native headers with
inline functions. It records original and transformed signatures, source
hashes, changed bodies, aggregate names, and identifier-based global dependency
candidates. Local shadows and conditional definitions still need review; these
source relationships are not a compiler call graph.

All 622 historical inventory rows receive an explicit mapping entry. Joins use
exact historical names and names-registry bindings at the exact routine start.
A name match is a candidate route, not a proof. Missing extents, duplicate
definitions, and unmapped routines remain explicit. The inventory includes
historical ASM entry names, even when no current C route can be established.

The risk inventory reports shifts, narrowing casts, bare host-width scalar
spellings, word sentinel comparisons, offset/overlay names, unprototyped call
casts, and translated fractional-carry code. These are actionable source
locations and snippets, not automatically diagnosed bugs. The bounded span
pass connects indexed pointer parameters to named caller objects and checks
direct global subscripts. Complex bounds, unknown aggregate sizes and pointer
provenance remain unresolved candidates. It never derives an object's extent
from the gap before the next symbol.

Bound inference follows enclosing loops and preceding assignments. Reusing
an index in a later small loop cannot inherit an earlier loop's maximum;
an unknown assignment clears the prior fact. Negative tests cover that case.
Fresh compiler pointer/ABI diagnostics remain visible in the risk report,
including source-selected declarations whose warnings are still compatibility
debt. A selected `vector_op_unk2` return-width contract now also applies to
every production TU, instead of leaving its central declaration implicit.

## Hard contracts and evidence

Fresh i686 compilation checks more than 600 assertions across the actual
source-selected views. These cover signed byte/word/dword widths, unsigned
scalars, numeric near offsets, pointer size, native aggregate sizes and field
offsets, wheel group/origin extents, the reciprocal dword table, the explicit
polygon sentinel extent, and selected native function prototypes. Producer
and consumer views retain separate checks, including scene/track/audio records.
The target's packed records and translated native pointers have different
contracts; a native pointer is not blindly required to have DOS near width.

Two registered span contracts derive their historical facts from parsed
declarations and frozen symbol bindings, then inspect fresh production output:

| Contract | Historical dependence | Required native representation |
|---|---|---|
| Wheel inputs | 24 vectors read through the first six-vector global; six words through one vector origin | Gather all four named groups and both origins into complete contiguous inputs |
| Polygon list head | Index 400 accesses the adjacent reset-marker word after 400 links | Own element 400 explicitly and alias the marker to it |

Known mistakes have negative regression tests. Removing wheel staging,
removing the sentinel element, or changing DOS signedness fails the audit.
A synthetic unregistered caller/span mismatch is reported as a candidate
instead of silently being called safe. Dangling evidence paths and duplicate
contract identifiers also fail validation.

The coverage registry names representative risky routines and exact observable
contracts: matrix alias/write order, gathered wheel inputs, font/header state,
Boolean sprite operations, line/polygon raster pixels, selected word arithmetic
instruction windows, and the stock model transform/queue/frame pipeline.
Existing tests use `tools/porting/diffharness` and locked-machine execution.
Generated angles, clipping cases, signed boundaries, deliberate nonadjacency,
seeded vectors, full memory buffers, and complete frames exercise these units
without requiring manual game states.

The report distinguishes:

- `static/layout`: only the registered layout, source or model invariant.
- `differential-oracle`: live locked-machine comparison for the registered
  finite cases and observables. An expression or composite path is labelled
  accordingly; it does not independently certify its enclosing function.
- `indirect-invariant`: fixed oracle-derived vectors or traces, with authority
  and limitations recorded. These are weaker than executing the oracle now.
- `compile-only`: fresh compiler coverage without a behavioral claim.
- `unverified/high-risk`: no adequate behavioral claim for that routine or a
  risky transformation still requiring review.
- `end-to-end`: recorded frames/replays, without per-routine attribution.

Coverage labels can overlap. A routine can compile successfully while still
having an unverified risky transformation. A finite differential claim does
not suppress other risk locations. Standalone scanning validates contracts and
evidence references; it does **not** rerun every referenced behavioral test.
Normal `tools/validate.py` runs the tests and publishes the audit summary in
its acceptance report, retaining these claim boundaries.

## Applying this to the next transformation

Start with the frozen routine's observable contract and its callers. Establish
the complete read/write span, scalar operations, aggregate views, ABI, aliases
and hardware assumptions before selecting a host representation. Keep the
machine facts separate from the representation hypothesis.

Add or extend a small named contract when a representation assumption must
hold. Add a falsifying probe that reproduces the old error class: separate
globals, toggle a font alias, exercise a minimum word, switch an unsigned
sentinel, or use a slope whose final carry changes the endpoint. Compare the
smallest useful memory/state boundary against the locked routine. Record the
authority, input domain, observables and claim limit in the registry.

Use the report to find other instances of that transformation. Do not promote
an entire translation unit from compile coverage because one expression or
render path matches. Unknown routes and unresolved candidates are the next
investigation list. Replay traces, crash dumps and F12 captures then provide
the final integration layer, rather than the primary translation check.

One new investigation distinguished a true span dependency from a data-domain
condition. `clear_invalid_track_tiles` can syntactically read row-table element
30, which aliases the DOS mouse-button word. The frozen startup reverses the
row table, so its logical final row is raw TRK row zero. All 41 locally installed
tracks exclude vertical parent pieces there and horizontal parents in column
29; the original editor also rejects placement beyond row/column 28. This is
bounded corpus/source evidence for valid maps, not a proof for malformed custom
tracks. The candidate stays visible. The ignored local corpus is not a new
clean-clone test dependency, and the port does not invent a new sentinel value.
