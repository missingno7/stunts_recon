# Measured near-match diagnostic pass

Starting HEAD: `98be0015e3354f54d7b045a136d783965d1eaaa5`, initially clean. The implementation in this pass is a working-tree change on that ref; no newer commit is implied by historical receipts. Fresh baseline validation passed 122 tests, all nine independent active-C compilations and HYBRID_EXACT; its preserved receipt is `recovery/evaluation/near-match-families/baseline-validation.json`.

The feature set is frozen. A subsequent [four-function isolated comparison](../../recovery/evaluation/advice-comparison/README.md) against the existing 98be001 island view found mostly neutral proposal choices and a less complete rectangle hypothesis from the new view. Actual run logs show lower aggregate tokens/time, concentrated in that rectangle run; the pointer run was slower. This is a tentative proposal-overhead signal, not proof of improved accepted recovery per unit of work. Ordinary source attempts and the negative MSC return-placement fixtures are reported separately.

## Scope and changes

The existing decoder, deterministic anchor/edit alignment, comparison levels, fixup normalization, strict probe and durable information path were sufficient. They were not replaced. No new source-search lane, compiler flags, binder rules, acceptance logic, ownership or attempt budget was introduced.

Three focused changes were justified:

1. Cross-island BP-displacement and explicit register-role patterns. All aligned uses of affected operands, including unchanged uses, check the proposed one-to-one mapping; width conflicts, reuse and opaque implicit effects contradict it. Each family retains scope, mapping, all supporting pair/operand sites, contradictions, confidence basis and links to original islands. Contradicted patterns remain visible but cover no residuals. This is an observed operand relation, not dataflow, source-object identity or a causal repair.
2. Conservative evidence separation. A reproduced `mov ax,ds` / `mov ax,es` pair used to be labelled REGISTER_ALLOCATION; segment/frame roles now stay separate. Branch correspondence requires a destination in a protected byte anchor for strong evidence; structurally paired destinations are uncertain. Identical relative bytes at different positions retain a separate destination observation when correspondence is unproven. Implicit registers reported by Capstone are retained. Its missing XLAT effects were reproduced and now prevent a register-family claim across that instruction; this is not a claim that the decoder is a complete semantic model.
3. Routine presentation uses the families and residuals, archived strict failure, unresolved symbols and freshness first. `context.py NAME --diagnosis --islands` expands original localization; full artifacts retain every site. Newer unavailable diagnostics are explicitly listed instead of silently replacing their strict result with an older summary. No second frontend was added.

## Local corpus and provenance

`recovery/evaluation/near-match-families/cases.json` records the pre-implementation selection; `results.json` records the same identity-checked inputs before/after. Seven records cover four distinct functions: current canary, two failed rectangle forms plus its accepted form, two parse_shape2d_helper failures, and accepted parse_shape2d_helper3. Five failure artifacts are not five independent functions. The third parse-helper attempt was withheld from diagnostic design; its complete output turns out identical to the first, so it is a weak held-out control, not independent generalization evidence.

Every case records pristine target/extent, preserved source and object identities, profile/flags, available bound bytes, recipe/toolchain context and historical strict status. Available complete artifacts were reused, with no missing bytes manufactured. Rectangle prior_03 preserves compiler-staged CRLF source matching its staged identity, not the original input-byte identity. Whole historical dependency snapshots are not reconstructed from incomplete receipts. Current toolchain/recipe identities are recorded separately. Baseline independent compilation provides fresh accepted-source controls; archived successful rectangle research is not relabelled as a fresh promotion.

The canary baseline was reproduced by re-analysis: 200/194 bytes, 87/82 linear instructions, 63 target instructions in 10 byte anchors and nine islands. NOPs are included. Its one supported BP mapping is `-12 -> -8`, `-10 -> -6`, `-8 -> -12`, `-6 -> -10`, all word accesses: 11 supporting sites across islands 1, 2, 3, 4 and 6, zero contradictions among aligned uses. This does not prove original variable identity, frame accounting, or declaration order.

Before: nine island descriptions repeatedly report slots, branch layout and calls. After:

```text
Strict failure: EXTENT_MISMATCH, 200/194 bytes
BP displacement permutation: 11 sites, five islands, no observed contradiction
Two branch changes reach corresponding byte-anchored destinations
Two unresolved __aFlmul operand fields remain binding obligations
Eight ungrouped observations: two unresolved branch destinations,
AL versus AX result, and five missing instructions including CBW/NOPs
```

All original islands remain accessible. The result-region observations are not eight asserted independent repairs. No missing instruction, literal, symbol, width or control-flow difference is hidden by the family. Original preservation ranges are unchanged. No EPILOGUE_OR_RETURN_LOWERING cause is asserted; the return is in the subsequent exact suffix.

Rectangle prior_02 previously repeated six BP-4/BP-2 byte accesses across four islands. The new view shows one displacement family and the separate `sub sp,4` / `sub sp,2` residual. Prior_01 additionally has a five-site SI/BX pattern, absent SI save/restore, NOP alignment and uncertain branches. The historically accepted local-register form has no observations. The parameter qualifier alone therefore did not complete this recovery. Historical prior_01/prior_02 source comparison is controlled for that qualifier, but prior_03 also changes declaration/symbol spelling: the whole accepted solution is not proof of a unique original C spelling.

## Measurements and negative controls

The fixed evaluation driver `python tools/evaluate_near_match.py --after` performs no compilation or search. It reads the baseline implementation from the pinned Git ref. Nine warm same-process measurements include decode, alignment, grouping, compacting and formatting, excluding artifact I/O. UTF-8 bytes and lines are measured; no token count or recovery-time speedup is claimed. Full before/after text and summaries are retained per case.

| Case | Lines before/after | UTF-8 bytes before/after | Median ms before/after |
|---|---:|---:|---:|
| Canary | 46 / 17 | 3123 / 1423 | 10.961 / 10.545 |
| Rectangle plain | 17 / 15 | 1427 / 1180 | 14.548 / 15.661 |
| Rectangle register parameter | 25 / 8 | 1665 / 742 | 4.931 / 6.110 |
| Rectangle accepted | 8 / 6 | 563 / 571 | 2.316 / 1.619 |
| Parse failure | 13 / 13 | 1009 / 951 | 1.673 / 1.728 |
| Parse held out | 13 / 13 | 1009 / 951 | 1.651 / 4.291 |
| Parse accepted | 8 / 6 | 546 / 554 | 1.616 / 1.765 |

Some accepted output is slightly larger; the parse case gains no supported family. The purpose is to separate correlated operands from independent residuals, not force compression. Full evidence is still needed to assess source lifetimes, paired-word object types, weakly aligned branches or TU causes.

Focused tests cover genuinely different branch targets; identical encoding with different destination; weak destination correspondence; wrong immediate/symbol; segment/frame registers; access widths; positive BP displacements; inconsistent mapping and unchanged reused homes; implicit-effect barriers; repeated code; incomplete decoding; and unavailable, crashing or deliberately misleading diagnostics. None of those controls becomes a supported false family or changes strict rejection. Complete diagnostic observation coverage is checked, including compact/index/default-context flow.

## Recovery questions exercised

Predictions and falsifiers were recorded in `predictions.json` before the fresh tooling probe and archived-bound comparison. Outcomes:

- One unchanged-source canary compilation retained its complete candidate hash/fixups and failed EXTENT_MISMATCH. The final engine re-analyzed that same checked object after a negative-control fix; it did not compile another source variant. `fresh-canary-result.json` links both records. Source budget remains two, with the existing supervisor block unreleased.
- Comparing identity-checked historical bound research bytes against the unbound view removes exactly two unresolved-call observations. Every non-fixup byte, the BP mapping and all eight residual instruction pairs remain unchanged (`bound-canary/result.json`). This is fresh diagnostic analysis of historical binding research, not fresh binding acceptance; extent still fails.
- Rectangle history confirms the separation of register and BP/frame effects described above. This is retrospective evidence after reading the successful source, not an independent prediction. Complete artifacts made recompilation unnecessary.

Reporting became clearer. No new causal source hypothesis was established and no new function was promoted. Tooling changes and correlated slot observations alone do not justify reopening the blocked canary. The next useful source-recovery question is independent MSC 5.x evidence for the byte-result/CBW branch sequence and how that interacts with the paired word homes; archived char-result, declaration-order, register and flag negatives must be consulted before proposing a new bounded experiment.

Follow-up: two isolated MSC 5.1 fixtures tested ordinary early return against an explicit trailing zero-return label, with predictions recorded before compilation. Their complete 38-byte contributions are identical; the optimizer moves both zero paths first. This refutes the simple label-placement hypothesis without a new canary source variant. See `recovery/compiler-evidence/boolean-return-placement/README.md` and its source/command/object receipts. The block and source budget remain unchanged.

## Selective remote evidence

Only source/doc files were read through GitHub, never cloned/built or used as Stunts dependencies. Full refs and inspected file hashes are in `remote-sources.json` beside the local evaluation. These are remotely reported historical cases, not locally reproduced predictions.

- [SimAnt grinder lessons](https://github.com/missingno7/simantw_recon/blob/1397e995c7b3856daed112d162e3d57bf73f6d91/docs/grinder-lessons.md), its `codegen_diff.py` and near-exact lane: IsItFood's clean body still had wrong segment identity; YellowHelp required the correct declaration binding while an expression rewrite introduced a temporary. This motivated the Stunts segment-register negative control and keeping unresolved symbols/strict failure above body patterns. No MSC 7/Win16 selector machinery or lane threshold was imported.
- [Icy Tower compiler-context evidence](https://github.com/missingno7/icytower_recon/blob/af421c9f9a18d583f799adc82bab25f06ce36980/docs/compiler-context-evidence.md), `stack_diagnostics.py` and `instruction_alignment.py`: omitted peers affected scratch pushes without changing the target body, while type/flag alternatives failed. Its declaration/type evidence deliberately does not prove stack allocation. Stunts gets the same caution about causal claims, not GCC's mechanism or DWARF assumptions.
- Empires inspected ref `9f9adaf8c5c2b503eeecee7aa1291df5097ef77c`; historical files were checked against frozen `historical-exact-oracle-v1` at `873d1df0f505601d760c6880cdea0c6ae3d81405`. Closure-frontier, STARTUP.C and the GAME recipe are byte-identical across those refs. No portable code was used.

Three Empires retrospectives distinguish body and non-body questions:

1. [Sound display-mode repair](https://github.com/missingno7/empires_reconstruction/commit/051d322d76b203eadab79202b6e2d8f562d53615): evidence was a word compare against a char-declared object and a branch; the change replaced two inline-ASM instructions with a word-view C equality and existing control structure. Minimal useful clue: preserve access width and branch polarity separately. Neighboring compare/goto forms reportedly grew instead; this was not a general rule for all conditionals.
2. [GAME TU recipe](https://github.com/missingno7/empires_reconstruction/blob/873d1df0f505601d760c6880cdea0c6ae3d81405/recipes/modules/C_3A75_4A93.json): TURNLOOP/LVLDRV's assembler-route dependence was explained by neighboring BOOTSEED inline `sti` in one TU, rather than inventing per-function flags. Minimal clue: body mismatch plus compilation-route/context dependence. That Turbo C mechanism is not evidence for MSC 5.x.
3. [MUSIC history](https://github.com/missingno7/empires_reconstruction/blob/9f9adaf8c5c2b503eeecee7aa1291df5097ef77c/docs/history/exact-c-recovery.md): matching code bytes coexisted with wrong ordered relocations because inline ASM selected the assembler route. Rewriting the cached-frequency routine as C restored native descending fixup order. Minimal clue: code equality and ordered relocation failure must remain separate. Stunts already enforces that distinction and needed no binder extension.

The small selected Stunts corpus cannot establish that most remaining functions need small fixes. The diagnostic implementation itself changed no ownership. Subsequent bounded recovery promoted `copy_string`; its decisive observations were available in the existing islands, so it does not establish an incremental recovery benefit for the family summary. See the separate comparison and recovery records linked above. Final validation is recorded in `docs/current/validation.json`; no historical receipt substitutes for that check.
