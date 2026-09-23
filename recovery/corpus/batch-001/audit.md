# Batch 001 partial record audit

Snapshot: 2026-09-23T13:26:58+00:00 UTC. This audit used the frozen selection, current attempt ledger and linked reports, Triage A, the archive summary and worker summary. It did not alter sources, recipes, queue state, attempt records or promotion state. Only `audit.json` and this report were written in the batch directory.

The batch ledger has **11 rows / 11 rounds**. Receipts show **10 hypothesis compiler invocations**; **10 workflow attempts were charged**, including the one `mmgr_copy_paras` preprocessing rejection that had no compiler invocation. Nine compiled failures are confirmed across three routines, plus one exact `toupper` FAST result and promotion. Promotion rebuilt the candidate fresh at least three times; those are verification builds, not added hypotheses. This is within the caps of 15 rounds and 50 hypothesis compiles. These three-failure stops reflect the initial short-budget workflow gate, not model exhaustion; later extended Luna research is a separate cohort.

| Function | Target | Emitted outcomes | Ledger verdict |
|---|---:|---|---|
| `audioresource_copy_n_bytes` | 70 | 36, 70, 38 bytes; all strict mismatches | 3 informative negatives; original short-budget gate reached |
| `file_get_res_shape_count` | 17 | 12, 30, 12 bytes; third output repeats first effective code and object | 2 informative negatives, 1 neutral redundant; original short-budget gate reached |
| `unknown_libname_1` | 18 | 18, 24, 18 bytes; equal-size outputs are byte mismatches | 3 informative negatives; original short-budget gate reached |
| `toupper` | 24 | 24 bytes, strict match | FAST pass, then promoted after fresh whole-image checks |
| `mmgr_copy_paras` | 72 | no emitted bytes; preprocessing rejected directives | charged workflow attempt, zero compiler invocations; 2 budget units remain |

**Receipt reconciliation:** all nine compiler failure reports agree with the ledger’s target/emitted lengths and mismatch class; all show empty fixup lists. The repeated `file_get_res_shape_count` output is independently confirmed by identical effective-code and object hashes in reports 0001 and 0003. The three `unknown_libname_1` rows now link to their reports and identities. `toupper` strict acceptance is backed by FAST report 0001 and promotion report 0002; status is therefore `PROMOTED`, while its exact historical source condition remains unknown because repository C uses `< 'z'` and the target is inclusive.

**Information gain:** 8 compiled rows have distinct, prediction-relevant negative outcomes; one row is neutral because a declaration rewrite reproduces the same code and object; `toupper` supports one sufficient exact candidate. The `mmgr_copy_paras` rejection establishes a preprocessor/workflow blocker only. The current corpus does not prove unique historical source causes. The `toupper` row’s “rootcause” is best read as an experimentally sufficient explanation for emitted bytes, not provenance of the original source.

**Blocker overview:** Triage A and B are strictly nonexecuting: together they contain 24 supervisor cases and 5 exact controls. Their evidence spans call/link boundaries and external control flow; MZ relocation, CS-relative, DGROUP and unregistered data mappings; hardware, interrupt and segment-register semantics; historical compiler/source and partial label mappings; source-shape or exhausted-budget questions; and cases with incomplete decode or no image anchor. These are triage observations, not compiler outcomes; see both triage sections in the JSON.

**Data gaps:** the historical archive’s 43 records must remain separate from current-batch trials. The current row ledger omits effective/object/output hashes for most compiler attempts even though receipts provide them; toupper is omitted from that list because its FAST row does record those hashes. The per-row model/reasoning settings are absent, so settings consistency is not mechanically verifiable here. The worker summary was refreshed during the audit; its current hash is in the JSON snapshot.

After this snapshot, the live ledger gained round 12 for `sub_35DE6` (`SKIPPED_BLOCKED`, zero compiler experiments). It is recorded as post-snapshot context in the JSON and excluded from the 11-round counts.

See `audit.json` for row-level receipt-derived hashes, budget charges, repeat identity checks and input hashes. Later endurance-cohort work is outside this snapshot and should reuse these first-batch records without merging its short-budget and extended-run accounting.
