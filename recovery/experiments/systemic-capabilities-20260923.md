# Systemic recovery pass, 2026-09-23

## Fresh baseline and proof scope

`python tools/validate.py` passed before this pass: hybrid load image exact, independent active-C parity exact, 140 then-current tests, 834 matching game-C bytes, 725 pinned-library bytes, and queue `CHEAP=0`, `MEDIUM=0`, `SUPERVISOR=602`. The worktree already contained extensive uncommitted research and promotions; none was discarded. The sibling repositories and `ai_decomp` were read only.

`python tools/reconstruction_factory.py reclassify` is now the repeatable current-state command. It refreshes cards and writes `recovery/blocker-census.json` plus `recovery/build-topology.json`. Both declare research-only authority and retain every current supervisor row. No category changes task control, attempt budgets, source, manifest ownership or the production acceptance path.

## Measured population and unlock leverage

Of 602 current supervisor rows, 220 have verified exact extents covering 18,268 bytes. The other 382 have uncertain extents (272 partial, 110 with no address anchor); their apparent lengths are excluded from byte totals. Categories overlap:

The census also assigns a conservative primary gate per row: 382 extent, 127 MZ/OMF-mode unknown, 17 instruction evidence, 16 data/global, 12 local-source or untested, nine ABI, eight TU-context candidates, seven external-jump extent candidates, two CS-data, two other binding candidates, and 20 `UNKNOWN` where multiple plausible causes remain. These are workflow roots or research candidates, not historical source diagnoses.

| Directly observed or current gate | Tasks | Verified target bytes | Smallest discriminating step |
|---|---:|---:|---|
| Unverified extent | 382 | unknown | Investigate repeated label-span mismatch families without relaxing importer equality. |
| Missing card instruction evidence | 143 | 15,624 | Decode pinned pristine bytes and review CFG/ownership; all 143 now have complete *research* linear decodes. |
| MZ relocation, OMF mode unknown | 127 | 13,478 | Compile representative unchanged full sources and inspect complete OMF/FIXUPP plus historical LINK differential. |
| Direct far or indirect call candidate | 121 | 13,120 | Review target/frame identities and candidate object modes before binder expansion. |
| TU/object context candidate from near calls | 35 | 3,656 | Frozen-body predecessor/order probe and complete-unit check. |
| Unregistered data/global operand | 34 | 1,241 | Independently review DGROUP address and data owner. |

The 127 MZ tasks have 328 relocation sites. Pristine linear decode places 311 on far-CALL operands, six on far-JMP operands and 11 on other operands. It finds 74 direct near calls and 13 indirect calls. MZ relocations do **not** disclose the original OMF frame or target method. Thus a generic new binder mode has no measured population yet; implementing one from those 328 sites would be speculative. The existing reviewed far-CALL mode remains limited to complete single-public, zero-addend target-frame objects.

Direct far calls span 115 distinct load targets. Forty-eight target addresses coincide with verified mapped function starts; 17 of those have calls from at least two distinct current caller tasks (52 call sites). Mapped entry is not original PUBDEF proof. The reviewed code-target registry previously contained only pinned runtime `__aFlmul`; this pass adds `_kb_call_readchar_callback` as a **binding alias**, not a historical name claim. Its five-byte mapped extent and two independent relocated caller sites are checked by `tools/code_symbols.py`. The registry now covers every far target address in three current supervisor tasks, but their source/object blockers remain. This is a capability increment, not 121 released tasks.

## Implemented experiments

- `tools/blocker_census.py` creates a row for every supervisor task, preserves historical blockers, independently decodes each verified pristine extent, records call/relocation instruction contexts, and never assigns OMF modes from an MZ entry. The `reclassify` command reruns it after a factory change.
- `tools/build_topology.py` reports 234 verified functions, 223 with an imported far-call-anchored frame, 44 contiguous same-frame candidate runs covering 163 functions, and 385 direct call edges. It records zero proven original same-object pairs or object boundaries. The exact 114-byte rectangle pair remains a *compatible* TU probe, not a historical TU proof.
- `tools/tu_context.py` freezes a target source hash, varies predecessor/declaration/order snippets and profile, archives complete source/object/OMF metadata, and compares public-bounded target bytes/fixups/extent and other publics. `recovery/tu-context/rectangle-order.json` is a negative control: isolated, predecessor and successor compilations all preserve the exact 62-byte target. `recovery/tu-context/synthetic-call-context.json` is a positive tool control: the same target body changes bytes and fixup form when an external helper becomes a static same-TU predecessor.
- The real `recovery/tu-context/flush-stdin-external-vs-local.json` probe holds `flush_stdin` source fixed. External helper: 10 target bytes, pointer32 far-CALL target-frame fixup. Static predecessor: 10 target bytes at a different public offset, self-relative offset16 near-CALL fixup and MSC local-symbol records. The 11-byte pristine target still differs (`cmp ax,0` versus emitted `or ax,ax`), so there is no exact source or TU claim. The synthetic and real local variants initially exposed strict reader rejection of `LEXTDEF`/`LPUBDEF`; `compile_source(..., research_local_symbols=True)` now permits those complete records for research while default production parsing still rejects them. A negative test checks that separation.
- The new raw code-target resolver path checks mapped target identity/hash/raw ownership and two distinct verified relocated callers before a binding alias can supply an address. A negative test removes one relocation and confirms rejection. No game-C source or manifest owner was promoted.
- A component-level profile control compiled the exact 114-byte two-public rectangle TU under pinned MSC 5.00 and 5.10. Both produced the same exact code, publics, SEGDEF declarations and empty fixups; complete OMF file hashes differed. `recovery/experiments/profile-rectangle-20260923.json` preserves the receipts. This component supplies no evidence for assigning a different production profile, so the pinned global profile remains.

## Priority after the initial census

1. Obtain complete candidate OMF objects for a representative direct far-call family with reviewed target/frame identity. Compare historical LINK output, including ordered relocations, before asserting an additional binder mode. The small `flush_stdin` caller and mapped callback are a bounded probe pair, but source shape still differs by one byte.
2. Test bounded multi-public/same-TU sources with the frozen-body runner and full contribution accounting. Static same-TU calls expose local-symbol and self-relative offset16 records; production support requires a separate reviewed complete-unit binder and historical LINK fixtures. Do not turn the research parser option into production admission.
3. Group the 272 partial mappings by exact label-span failure, retaining all importer equality checks. No common boundary repair was demonstrated by the current sample; 110 no-anchor rows need an independent anchor source.

At this initial stage strict game-C recovery remained 834 bytes in 14 functions and the queue remained at 602 supervisor tasks. Subsequent importer work changed this state; the current result follows.

## Repeated importer mechanisms and current state

`tools/partial_mapping_census.py` exposed repeated first disagreements between the Restunts listing and pinned Capstone 16-bit decode. Narrow, byte-qualified corrections in `tools/restunts_base.py` and `tools/x86_16_encoding.py` recognize only literal `db 144` as a one-byte NOP, unprefixed `98`/`99` as source `cbw`/`cwd`, and exact unprefixed A4–AF string opcodes as source bare string instructions. All other `db` literals and prefixed forms remain unsupported. Every originally verified interval retained the same start, end and SHA-256, and all new intervals still pass source label interval, operand, unique-boundary and pristine-byte checks. Focused positive and negative tests cover each correction.

| Stage | Verified mappings | Partial | No anchor | Queue |
|---|---:|---:|---:|---|
| Baseline | 234 | 275 | 110 | 602 supervisor, 0 medium |
| `db 144` | 372 | 186 | 61 | 595 supervisor, 7 medium |
| `cbw` / `cwd` | 452 | 113 | 54 | 595 supervisor, 7 medium |
| Bare string opcodes | 497 | 72 | 50 | 592 supervisor, 9 medium |

The final mapping adds 263 verified functions and 112,875 bytes of code *evidence*, not recovered C or ASM. Of the remaining 72 partial rows, 42 first fail on `db` versus decoded `add`, often an indication of zero-filled embedded data. General `db 0` acceptance would conceal code/data uncertainty. `tools/audit_function_extents.py` flags 17 possible multi-entry or embedded-data intervals among the broader mapped set; none of the nine released medium tasks overlaps those findings.

The refreshed census has 592 supervisor rows, 470 with verified extents totaling 130,560 target bytes and 122 with uncertain extents. Primary gates: 296 binding/OMF-mode unknown, 122 boundary/extent, 58 missing reviewed instruction evidence, 23 data/global ownership, 20 boundary candidates, 14 TU/object candidates, 12 local-source/untested, nine ABI, two binding candidates, two CODE-data ownership, and 34 unknown. These are research categories, not proved historical causes. Overlapping families: 296 tasks / 113,709 bytes have MZ relocation contexts but unknown candidate OMF modes; 353 / 126,843 lack reviewed card instructions but now have pristine research decodes; 288 / 113,189 have far or indirect call binding candidates; 135 / 73,628 have near-call TU candidates; 55 / 1,930 have data/global candidates. Upper-bound task counts must not be read as promised production unlocks.

Pristine decode observes 2,161 direct far CALLs, 439 direct near CALLs and 46 indirect calls in current verified supervisor extents. Relocation contexts include 2,161 far CALL operands, six far JMP operands, and 42 other instruction operands. There are 346 distinct far-call load targets; only two binding target addresses are independently reviewed, and three supervisor tasks have every far-call address reviewed. Complete candidate OMF modes and historical LINK differential fixtures remain the next binding discriminator. MZ relocation sites alone cannot authorize a production binder extension.

`tools/build_topology.py` now reports 497 verified functions, 492 frame anchors, 56 contiguous candidate runs covering 464 functions, and 2,600 direct call edges. It still records zero proven historical same-object pairs and zero proven object boundaries. The MSC 5.00/5.10 rectangle two-public control produced identical 114 code bytes, public order, segment declarations and fixups, so it gives no reason to change the pinned global 5.1 profile.

Four newly released tasks passed serial FAST and fresh whole-image promotion: `nullsub_1` (2 bytes), `nullsub_2` (2), `get_0` (4), and `audioresource_get_word` (20). Each compiled as a complete single-public object with no fixups or extra initialized contributions. `python tools/validate.py` then passed 148 tests, oracle and exact build verification, independent DOSBox-X code/binding checks for all 18 active functions, and exact whole-image acceptance. Strict game-C recovery is now **18 functions / 862 bytes**, zero game ASM bytes, and 725 pinned-library bytes. Final queue: **CHEAP=0, MEDIUM=5, SUPERVISOR=592**. Nine tasks became grindable through the mapping repair; four were strictly promoted. The larger systemic gain is 112,875 target bytes with newly testable mappings.

The bounded Luna/high batches also tested `copy_paras_reverse`, `kb_reg_callback`, `mmgr_path_to_name`, `flagchar`, `nopsub_37750`, and `sub_38156` without an exact contribution. These remain research results, not failed grinder attempts or loosened eligibility. In particular, source-shape variants for `mmgr_path_to_name` emitted 56–68 bytes against its 32-byte target; further local spellings without a new register/storage or TU-context mechanism have low expected value. `copy_paras_reverse` likewise kept a string-instruction gap. Continue systemic OMF/TU investigations before spending broad grinder budgets. Keep `is_facing_camera` parked unless new systemic evidence changes its blocker.
