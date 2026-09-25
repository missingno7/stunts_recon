# Workspace simplification â€” 2026-09-25

The prior complete validation passed 179 tests and independent DOSBox-X compilation before changes. Git checkpoint: `checkpoint/pre-simplification-20260925`. Detailed migration receipts are local under ignored `build/migration/`.

The obsolete cards, queue snapshots, task epochs, retry budgets, eligibility/reopen machinery, routing/cost studies, duplicated reports, inactive recipes, and their orchestration/tests were deleted. No active historical archive tree replaces them. Git retains the history; `evidence/compiler-notes.md` retains five useful observations with precise retrieval references.

The retained proof closure consists of the accepted C and recipes; one ownership manifest; immutable byte/toolchain/reference identities; original function/address/relocation facts; reviewed bindings and overlays; complete OMF/runtime reconstruction; compiler diagnostics; and focused technical tests. The inventory remains queryable for all original procedures, including functions that current binders cannot promote. Runtime evidence was reduced to member-identification facts; active ownership and record policies live only in the manifest.

| Measure | Before | After |
|---|---:|---:|
| Accepted C functions | 28 | 28 |
| Matching C bytes | 1,866 | 1,866 |
| Matching ASM bytes | 0 | 0 |
| Pinned runtime bytes | 725 | 725 |
| Explicit raw initialized bytes | 197,409 | 197,409 |

The everyday path is `context.py` â†’ `search.py` â†’ `promote.py`; use `validate.py` at acceptance/tooling boundaries. The former queue/card/grind/reopen/refresh path is gone. Supporting low-level modules retain their technical purpose.

Luna investigations normally use extra-high reasoning and own isolated scratch source, hypothesis selection, adaptation, and useful batches. There are no attempt or compiler quotas, administrative checkpoints, or reopening requirements. Sol/high is reserved for shared technical obstacles. Canonical publication remains serialized, checks current inputs, and needs no additional permission ceremony after strict acceptance.

The migration smoke used Luna/high on unresolved `rect_adjust_from_point`. Four standalone hypotheses were replayed through the new search command. Compiler feedback led from an 84-byte output through a spilled-coordinate variant to a 76-byte register-coordinate variant with 31/33 exact instruction alignments. The remaining two displacement islands use BP-2 where the original uses BP-6; adding unused locals changed the frame rather than fixing the slot. No new strict match or recovery is claimed. See `build/workers/migration-smoke/summary.json` for the compact progression and individual reports.

The function inventory was regenerated from pinned references after old workflow deletion and compared semantically equal: 619 procedures and 781 binary anchors. The frame-callback historical LINK fixture now recompiles source instead of depending on a discarded archived object. The final proof results and footprint follow.

Original assets, pinned compiler/runtime distributions, evidence checkouts and reference executables remain intact. The old recovery tree's 155 ignored/local-only compiler files were moved to `build/migration/local-only-recovery`; none were deleted. The only pre-existing tracked modification was the generated parity receipt, refreshed by the required baseline validation and retained as `build/migration/pre-cleanup-parity.json`. No unrelated source edits were present.

Remaining technical limits: original TU membership, general linking and arbitrary binding/segment modes remain unproven; production still rejects unsupported OMF records, QuickC BAKPAT and local-symbol modes. The strict production subset remains complete contributions. Scratch family/TU hypotheses are observations, not historical grouping claims. `HYBRID_EXACT` includes explicitly raw-owned regions and is not a complete source recovery claim. This migration makes no economic or throughput improvement claim.

## Final verification

`python tools/validate.py --baseline build/migration/baseline.json` passed **135 tests**, with no failures or skips, fresh **HYBRID_EXACT**, and independent DOSBox-X parity for all **28** accepted functions. All **66 ownership intervals**, accepted C source identities, recipe binding/build facts, oracle identities, **2588 ordered MZ relocations**, and runtime ownership were preserved. The complete executable identity is `1adb8259b3f6634062b94826e1f167649d9f2deeb6cedc9989127fc37e9ce615`.

Actual idempotent publication of the existing `rect_union` control passed both staged and canonical full-image checks, left the baseline sources/ownership unchanged, and cleared its journal. This was acceptance-path testing, not new recovery. `--verify-only` also passed. Focused tests cover byte/fixup/declaration corruption, symbol resolution, ownership errors, source/state races, competing writer processes, interrupted publication, conflicting user edits, unsupported-record reporting, and unrelated history/ownership changes.

Two concurrent real searches of existing exact controls passed strict recipe probes in separate compiler work directories while canonical inputs stayed unchanged. Scratch compilation also ran alongside final validation without invalidating it. Context listing, assembly/symbol expansions, and regenerated inventory work with no old process state present. Receipts: `build/validation/report.json`, `build/validation/independent.json`, `build/migration/parallel-search.json`, `build/migration/publication-control.json`, and `build/migration/inventory-regeneration.json`.

The versioned project footprint changed from **2,374 files / 37.83 MB** to **121 files / about 4.55 MB** (existing tracked plus new nonignored project files; ignored assets/tools/reference checkouts/build output excluded). Python tool modules decreased from **53 to 33**; everyday work now uses **four commands**, with no queue/grind/reopen/refresh sequence. `git diff --check` is clean.
