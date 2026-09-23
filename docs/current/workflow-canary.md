# Workflow and larger-function integration pass

The information path and bounded far-call capability are implemented and tested. **The 200-byte canary remains unresolved. Its neighboring 76-byte `rect_compare_point` was promoted through fresh FAST, staged and canonical hybrid builds.** C ownership is now 328 bytes; library ownership remains 725 bytes and raw initialized ownership is 198,947 bytes. Generated `validation.json` records the fresh handoff proof. Production remains an oracle-backed raw-owner hybrid, not natural historical-link closure.

## Baseline and costs

The fresh initial validation at commit `27bc04c` passed 87 tests, eight independent DOSBox-X C checks and HYBRID_EXACT in 20.523 seconds. It owned 252 C bytes and 725 library bytes; 199,023 initialized bytes remained raw. The queue was 0 CHEAP / 9 MEDIUM / 599 SUPERVISOR, with 235 mapped procedures. `recovery/workflow-baseline.json` records blocker families and observed file sizes.

The 608 current baseline cards occupied 11,224,761 bytes. Warm read-only measurements were 7.876 ms for broad inputs (65 files / 556,627 bytes), 6.912 ms for controls (19 paths / 6,384,898 bytes), and 0.572 ms for five attempt reports. These were measured before changes; no baseline full-refresh timing was taken.

`python tools/benchmark_workflow.py` measures **the same current card facts and complete snapshots** serialized inline versus referenced once by hash. See `recovery/workflow-cost.json` for current exact bytes, five-run serialization medians, fresh refresh time, and bounded packet bytes/time. This is an equivalent-evidence comparison, not a comparison against an incomplete summary. No tokenizer or token-usage measurements were taken. The new default packet has more explicit ABI/attempt facts than the old unresolved canary card, so its standalone size is not presented as an equivalent before/after token saving.

Final measured equivalent evidence: **13,502,353 bytes inline vs 2,156,831 bytes shared**, saving 11,345,522 bytes (84.0%). Five warm serialization medians were 84.1 ms vs 36.7 ms. A no-change refresh took 0.676 seconds; the default canary packet was 7,752 UTF-8 bytes in 24.9 ms. These are local timings, not guaranteed speedups across machines.

Fresh handoff validation passed **102 tests**, independently reproduced all **nine active C functions** under DOSBox-X, and preserved HYBRID_EXACT in **26.904 seconds**. It includes more tests and one more active function than the 20.523-second baseline, so those elapsed times are not an equal-work performance comparison. Final queue: 0 CHEAP / 9 MEDIUM / 598 SUPERVISOR; 237 mapped procedures.

## Implemented path

`context.py` resolves exact names or stable IDs through a generated index and follows the actual card path. Default output includes extent confidence, oracle identity, blockers, source excerpt, known ABI/width constraints, profile, latest diagnostic and distinct experiment summaries. `--asm`, `--callers`, `--globals`, `--history`, `--full` explicitly expand evidence. Omitted material and artifact paths/hashes remain visible.

Cards reference immutable content-addressed snapshots. Readers verify hashes; validation resolves each shared snapshot once and rechecks it after the card audit. Unchanged JSON is not rewritten; replacement is atomic. Full scope guards remain conservative.

Three dependency roles are explicit: broad transaction inputs detect out-of-scope promotion edits; production inputs select active recipes/sources while retaining every tool/header/ASM/layout dependency; task inputs and hypothesis keys include compiler/binder/header/symbol evidence without unrelated candidate content. An unrelated candidate edit no longer makes accepted production stale or makes an unchanged failed hypothesis novel. Includes remain rejected until their compilation closure is implemented, and headers are conservatively fingerprinted anyway.

The compact attempt index is a derived search view, not an acceptance authority. Immutable archives and interval-aware controls remain authoritative. Full validation reconciles the view. Archive contents are still hashed conservatively during workflow checks: removing that scan is a deferred optimization, not a claimed speedup. The archive is currently small; its scaling risk is prospective. Controls are loaded once per queue refresh rather than once per function.

Length failures now retain complete emitted sizes, publics/fixups, the first differing instruction and a hashed full disassembly artifact. Raw objects/logs remain in ignored probe directories; archived sources/logs/diagnostics retain their identities. Instruction decoding never supplies accepted bytes. Effective code/fixup deduplication retains profile and dependency context. FAST now rejects a workflow-control mutation during compilation, in addition to the existing serial promotion/rollback checks.

## Canary proof and binding

Pristine `is_facing_camera` is `[0x15F2E,0x15FF6)`, 200 bytes, SHA-256 `6fc29d78db5106925aa06f76eb06ed2f7fdb5a6dfb0e597d1e94b56e0558e929`. `layout/function-evidence.json` proves selected instruction coverage, direct branches, both returns, three unreachable alignment bytes and caller/neighbor anchors without lifting the importer's 80-byte threshold globally. The old interior anchor remains a recorded mapping conflict.

Private-reference provenance and the small useful function cross-reference are in `recovery/forged-crossref.json`. Only derived facts and file identities are recorded; private implementation and execution machinery were not copied. Its declared input identities match our oracle; actual private asset files were absent. Private differential/replay harnesses were not executed.

Two pointer32 CALLs target the pinned `__aFlmul` public at load `0x1E8D8`, not a presumed helper name. Frame `0x1CC50` is supported by a separate pristine relocated call at `0x4E46`. The library owner is freshly reopened and checked. Offset and paragraph halves are computed from these symbol facts, never desired canary operand bytes. The binder checks exact declarations, location, kind/width, frame/target/index, zero displacement/addend, unowned storage and ordered source relocation obligations. Other modes fail closed.

`recovery/far-link-proof.json` records four untouched compiler-object + pinned-library LINK experiments: both compiler/linker versions, both input orders, two calls each. Tests reproduce them. Different input orders exercise nonzero source and target frames. They prove this fixture behavior, not the game's original TU/link topology or full MZ relocation-table derivation.

The game's caller sequences use PUSH CS + near CALL. `recovery/call-topology-proof.json` and its test show MSC already emits that sequence for a known same-TU callee; LINK supplies the self-relative displacement without a far-CALL byte rewrite. This mode remains research-only in production.

The adjacent `rect_compare_point` + canary source compiles into a 270-byte TU with publics at 0 and 76. The canary's relative bytes/fixups are unchanged. This is coexistence evidence, not original TU proof. The four rectangle field addresses were subsequently proven and its standalone 76-byte function was promoted. BSS source ownership and multi-public production remain open. No adapter or sliced contribution was promoted.

The pinned Empires reference `c0a672d5daba74256aefa596e0b78f205d2b169d` was consulted through `docs/relocation-grouping.md` and `docs/exact-structural-link.md`. Its grouping/relocation-order lessons informed the immediate TU probe; its adapters and historical architecture were not imported.

## Actual source experiments and remaining mismatch

Nineteen source/profile trials are archived under `recovery/experiments/is_facing_camera`, followed by an ordinary `grind.py` attempt. Ten early trials were archived retrospectively from their original compiler inputs/receipts; their reports explicitly say they were not freshly rerun during archival. Later predictions were written before compilation. Additional bound-payload and adjacent-TU probes are separately labeled research.

The strongest source remains `recovery/candidates/is_facing_camera.c`. It preserves the important width asymmetry: X operands widen before subtraction, Y subtraction wraps at 16 bits then sign-extends, and products/determinant use the compiler's 32-bit helper. It emits 194 bytes. After binding, the first mismatch is **+0x37**: `mov [bp-8],ax` instead of `mov [bp-0xC],ax`. Its dx1/dy0 allocation differs, followed by the final boolean/return block; the complete target is six bytes longer. `recovery/canary-bound-diagnostic.json` proves both source call operands/relocation sites can be bound while retaining this failure.

Changing return type, explicit byte casts/constants, variable spelling/declaration grouping, predicate polarity, or adding result temporaries did not solve it. Named char/register-char hypotheses produced the same effective object. A stored long temporary increased the frame. MSC5.00 and MSC5.10 agree on the strongest source. `/Od` expands the function; `/Os` and `/Or` shrink it; `/Ox` did not fix the tested temporary variant. The adjacent TU does not change the canary. Explicit register-long and register-far-parameter qualifiers also leave the canary at 194 bytes. These results do not identify a unique original compiler/profile.

The family is explicitly supervisor-blocked, with the original interval retained. The next discriminating work is to seek evidence for paired long-delta storage and byte-result materialization, then test that ordinary-C shape under the existing profile. Do not invent padding, reorder bytes, add a function-specific adapter or repeat arbitrary flag sweeps. General DATA/BSS, headers at compile time, multiple publics and self-relative production remain unsupported.

## Accepted neighboring recovery

The plain classifier source emitted 76 bytes with BX and a two-byte local frame, so equal length alone was insufficient. A register-qualified parameter produced SI but still the wrong local slot. A used local `register POINT2D *p = point` reproduced SI, the four-byte frame and flag at BP-4 with no dummy storage. All 76 bytes and four struct-field fixups match. The source and new field evidence passed the ordinary grinder promotion path, including fresh FAST and both staged/canonical full-image builds. The three research hypotheses and the promotion are archived separately. This useful leaf match does not replace the unresolved non-leaf canary requirement.

## Continue

The local mismatch diagnostic now records a fresh unchanged-source supervisor probe under `recovery/diagnostics/is_facing_camera`; the latest entry is exposed automatically by context. This probe remains `EXTENT_MISMATCH` (200-byte target, 194-byte candidate). It does not consume a source attempt or reopen the family.

Previously the compact report stopped at instruction 25, +0x37 (`[bp-0xC]` versus `[bp-8]`). The aligned report identifies **63 of 87 linear target instructions in 10 byte-exact anchors (72.41%)**, with 82 candidate instructions and nine local islands. The exact prefix is target/candidate +0x0..+0x37. The exact suffix is target +0xB9..+0xC8 versus candidate +0xB3..+0xC2. These counts include alignment NOPs, unlike CFG-reachable counts; they are diagnostic metrics only.

The first island, +0x37..+0x3D in both streams, shows paired BP-0xC/BP-0xA to BP-8/BP-6 stores with high-confidence stack-slot evidence; exact code resumes at +0x3D. Further slot islands recur at +0x45, +0x58, +0x6D and +0x88. The two far CALL operand fields appear as unresolved fixup evidence, never exact anchors. Near branches expose aligned versus unproven destination correspondence. The final island is target +0xAE..+0xB9 versus candidate +0xAE..+0xB3: byte/word result materialization, changed control operations and missing instructions, followed by the exact suffix.

The engine does not label that final island `EPILOGUE_OR_RETURN_LOWERING`: its return instruction belongs to the subsequent exact suffix. It also does not infer a source temporary, register allocation cause, original variable names or semantic branch equivalence. Where operands or alignment do not establish a class, `LOCAL_CODEGEN_UNCLASSIFIED` remains the fallback. The diagnostic does not override the existing bound-call evidence or any complete-contribution requirement.

```text
python tools/context.py is_facing_camera --diagnosis
python tools/context.py is_facing_camera --history
python tools/context.py load_226ba
python tools/reconstruction_factory.py next
python tools/validate.py
```

Use focused probes/tests during exploration. Reopen the canary only with a concrete new-evidence reason. Any eventual promotion still requires exact complete compiler bytes/publics/fixups/relocations, fresh staged and canonical full-image builds, and independent active-source compilation. Existing matches and the immutable oracle have not been relaxed.
