# Recommendations for Stunts historical freeze and first port

Read-only methodology review for the Stunts 1.1 MCGA campaign. This is a proposal for the freeze record and later port boundary; it makes no new historical acceptance claim.

## 1. Freeze definition and document shape — ADOPT, with a Stunts-specific scope statement

Use a tracked `docs/freeze.md` (or equivalent) as a versioned claim record. Kegg’s `docs/freeze.md` is a strong structure: **Identity**, **Toolchain**, **Freeze validation**, **State of source**, **Intentional historical material**, and **Verification-chain notes**. Pair it with an `unresolved.md` catalog. Include:

- tag and validated source commit; original input hash/size; pinned tools, profiles, runners, and provenance;
- exact source/object/runtime/raw ownership totals and what `HYBRID_EXACT` means;
- a fresh, uncached whole-image command/result, ordered relocation result, independent compiler/backend result, and focused negative-control test result;
- remaining limits, deliberately retained opaque names/types/ASM/raw ownership, and what the tag does not prove;
- unresolved entries that separate **PROVEN machine/layout facts** from **HYPOTHESIS/name guesses**, with evidence path, freeze/port consequence, A–F campaign class, and next useful evidence.

Keep the README summary short and link to the full record. The unresolved catalog should not be a generic “todo” list: copy Kegg’s useful pattern of stating what is established, what remains unknown, and why preserving an unknown is more faithful than assigning a semantic name.

**Stunts gate caveat:** state separately that `validate.py --image` uses the locked, oracle-derived order and proves byte equality under that order. `histbuild.py` is a separate natural-link hypothesis whose library/order reconstruction still has an ordered-relocation residual. Do not turn the former into a claim that the historical LINK command/member order has been recovered. README.md and docs/acceptance.md already make this distinction; the freeze record should preserve it prominently.

## 2. Exact-build gate conventions — ADOPT the proof chain, not Kegg’s command spelling

Kegg’s tag records a fresh-clone run, hash verification for each launched tool, a whole-image `--fresh` build with zero cache hits, a second DOSBox-X compile path, parser/negative/isolation tests, and exact image SHA. For Stunts, write down the equivalent existing commands and receipts rather than copying `--fresh` as a requirement by name:

1. verify pinned local tool identities and immutable oracle identity;
2. run the complete validation at the freeze/tooling boundary, with fresh source compilation and no object-cache reuse;
3. require the supported contributions, complete extents/declarations, fixups, all 2,588 ordered MZ relocations, and complete hybrid image to satisfy the existing strict gate;
4. preserve independent DOSBox-X contribution parity and negative/isolation/transaction controls;
5. run from a clean clone or isolated fresh checkout with locally supplied immutable assets/tools, then archive command and report identities.

Publish **two results** if needed: “strict oracle-ordered image equality” and “natural historical-link reproduction.” A cache-backed development pass, object count, zero raw debt, or image match with oracle-supplied ordering cannot stand in for the latter. Kegg also notes its own limits (retail media not independently found; runner DLL/config not pinned); Stunts should list comparable remaining assumptions instead of hiding them behind the tag.

## 3. First-port boundary — ADOPT the separation and per-function map; extend existing Stunts inventory

Keep `PORT_BUILD`, `tools/porting/port_include`, and any future SDL3 implementation outside historical `src/`, `asm/`, `recipes/`, and ownership/oracle state. Kegg’s migration map is useful as a *proposal*: stable function identity by object/address, caller/callee and consumed/produced state, then `KEEP` / `ADAPT` / `REIMPLEMENT` with an explicit hardware seam. Empires demonstrates the stronger practical split: frozen historical source as oracle; portable game logic behind subsystem headers; SDL included only in the SDL platform layer; separately built/tested product.

Stunts already has useful foundations in `docs/porting/README.md`, `platform-boundary.md`, `state-model.md`, `formats.md`, and host-probe artifacts. Extend those inventories into a reviewed port map rather than starting a parallel status system. Keep each claim scoped: a syntax probe is not a port, a function replay is not subsystem coverage, and a playable shell is not historical exactness.

For a software renderer, adopt the indexed-page approach used in Kegg/Empires: model the game-visible page/layout/clip/palette behavior, preserve six-bit DAC values and page ownership, and convert indices only at presentation. Use real-game captures or deterministic draw traces as golden fixtures; screenshots alone are weak evidence. Build rendering, input, timer, audio, and file services as separate seams. Preserve simulation cadence and event order independently of host refresh.

For trace comparison, record a fixed startup/config/input script and compare canonical checkpoints at named gameplay or service boundaries, including relevant state, palette/page output, and timed effects. The PortForge review warns that generic frame ordinals, raw PCs, and host presentation callbacks are not universal semantic input points. Let the platform adapter own machine/device time and safe points; let the game profile identify meaningful input boundaries. Keep replay/session identity and snapshots bound together if resumable traces are later added.

Do **not** infer broad port conformance from a single clean replay. `kegg_forged/README.md` deliberately describes its current 100-frame selected regression as narrow and says the project is not DOS-PM conformant; `native/README.md` makes unrecovered paths explicit `NativeGap`s and forbids hidden oracle continuation. Adopt those fail-closed labels and coverage statements for any future Stunts port.

## 4. ABI and segmented pointers — ADOPT explicit boundaries; DO NOT copy flat-pointer erasure

Kegg’s `port/include/watcom/watcom_compat.h` and `i86.h` erase `near`/`far` only because Kegg is Watcom DOS/4GW flat 32-bit. Its fixed virtual selector, `FP_SEG`/`FP_OFF` macros, and low-memory shadow are a platform-specific compatibility model, not a template for Stunts’ 16-bit medium model. Empires likewise flattens its port’s far pointers in a documented 8086 adaptation; that is not evidence Stunts segment:offset values are ordinary host pointers.

For Stunts, keep segment and offset as explicit values whenever they cross stored data, driver calls, interrupts, or runtime callbacks. Resolve them through a bounded host-side address-space/service adapter only after the pointer’s segment convention and referenced extent are established. Near data pointers may map to typed host objects where independently proven. Retain target integer widths, packing, argument widths/order, return convention, and register-block layouts at adapter edges. Continue to forbid treating a segment:offset pair as a native pointer or using one global’s address plus an addend to reach another object.

## 5. Names and type recovery — ADOPT provenance and confidence separation

Use Kegg’s readable-name rule: descriptive names only where behavior is supported; retain address-derived `f_...` / `g_...` spellings for unknown roles. Keep a type catalogue with measured total size, field offsets, access widths, evidence path, and confidence. Record proven TU-specific views when code/layout evidence requires them; do not force one convenient “clean” declaration across incompatible source contexts. Kegg’s documented `EnemyProjectile.animation_sequence` per-unit `int` view is a useful example of preserving a byte cursor instead of imposing pointer-scaled arithmetic.

Use separate labels for original facts, source reconstruction, and semantics. Pre2’s proof policy distinguishes historical spelling, evidence-derived facts, reconstructed semantic names, and unknowns, and separately tiers routine/range/object/link/image proof. Tales’ type evidence warns that an access width or nearby extension does not by itself prove signedness, pointer kind, variable extent, or semantic meaning. Apply that restraint to Stunts: retain complete aggregate extents and opaque tails; highest observed access is not object size. Names constrained to preserve OMF/BSS layout remain reconstruction aliases, not recovered historical names; document the fitting constraint.

## 6. Sibling evidence — what transfers

- **Kegg recon:** strongest historical-freeze template. Tag `historical-exact-clean-v1` points to a freeze-record commit whose parent is the validated source commit; the record claims the byte-identical `KE.EXE`, fresh link, zero raw debt, and still lists remaining names, gotos, and provenance limits.
- **Kegg forged / PortForge review:** adopt explicit maturity, coverage, input-boundary, and gap reporting. Do not adopt a universal `step_frame`/frame-ordinal replay model; the review documents real timing, snapshot, and presentation-driven progression gaps.
- **Empires reconstruction:** closest DOS graphics/ABI port analogy: a separate portable tree, fixed-width DOS aliases, real 8-bpp indexed software renderer, virtual tick model, and graphics fixtures captured from original 8086 routines. Its flat-pointer normalization is conditional to its own model.
- **Icy Tower rerecon:** good claim hygiene: its `FREEZE.md` reports 251 exact functions and 2 behaviorally certified `EQUIVALENT` functions separately; `PORT.md` says the SDL branch makes no matching claim. Do not merge behavioral equivalence with Stunts’ strict byte-exact status.
- **Icy Tower recon, SimAntW, Tales, Pre2:** useful partial-work models, not whole-program freeze precedents. Their proof ladders distinguish function/range/object/image scope; type tools keep width evidence separate from inferred meaning; Pre2 explicitly says a 343-byte code-range proof is not an object or executable proof.
- **Overkill forged:** currently labels itself oracle-only and describes legacy port findings as inputs requiring revalidation. Treat adjacent port work as evidence, not conformance authority.

## Stunts-specific next step — ADOPT

Draft the freeze record from the present strict validation receipts and current README/doc scope; include the natural-link relocation-order residual as a separate unresolved/assumption entry. Keep using the existing platform-boundary/state/resource inventories for a later SDL3 design. Before port implementation, acquire bounded interactive traces for the unresolved timer, retrace, audio callback, and input behaviors already listed in `docs/porting/README.md`; static host compilation and source call maps do not settle those runtime contracts.
