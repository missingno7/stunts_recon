# Matching Decompilation: Cookbook and Evidence

Companion to the [operating guidelines](MATCHING_DECOMP_GUIDELINES.md). Consult the relevant section; this is not a mandatory reading list for every worker.

**Evidence baseline: 2026-09-23.** Project examples below are documented repository experiments, not builds independently rerun for this document. Recommendations are working policies, not experimentally proven universal optima. Repository-specific acceptance controls remain authoritative.

## Pricing and accounting

OpenAI Standard API text prices, USD per million tokens, for prompts with **at most 272,000 input tokens**: [P1–P3]

| Billed category | GPT-6 Astra | GPT-6 Luna | Astra / Luna |
|---|---:|---:|---:|
| Uncached input | $10.00 | $0.10 | 100× |
| Cached input | $1.00 | $0.01 | 100× |
| Cache writes | $12.50 | $0.125 | 100× |
| Output | $50.00 | $0.50 | 100× |

The published long-context rates preserve that ratio. Compare like categories, tiers, and regions; recheck pricing when it matters. A 100× unit-price gap is **not** evidence that Luna solves tasks 100× more cheaply, nor a measurement of subscription quota consumption.

Compute cost from actual billed categories for every participating model, plus applicable tool charges. Do not add cached input on top of total input, or reasoning on top of output when those are already included. Record missing telemetry as unknown. The Stunts evaluation explicitly identifies both subset relationships. [T2]

**Economic default:** test whether a cheap model with good tools and sufficient autonomy can complete the task before paying for routine expensive supervision. Optimize verified results per total cost; elapsed time is secondary unless it causes resource or reliability problems. Keep evidence gains separate from recovery credit rather than combining them in an arbitrary score.

## Recipe 1 — Assign a self-contained task

Use the smallest unit that contains the necessary dependency context: a function, related family, or object/TU component. A short function with unsupported references may need more preparation than a larger, well-evidenced body.

```text
TASK: stable ID, baseline identity, owned paths/range, acceptance scope
EVIDENCE: current source; target instructions; types/ABI; fixups/data;
          prior outcomes and contradictions; expandable artifact references
ACTIONS: exact compile/compare commands; permitted research and write scope
BUDGET: explicit cost/probe ceiling; checkpoint and escalation conditions
        hard shared cap versus initial allocations; reallocation authority;
        actual compiler processes versus prelaunch errors and verification
RESULT: candidate + reproducible evidence, or a precise unresolved question
```

Do not make each worker rediscover the repository. Do not omit critical evidence merely to shorten the packet. Keep one worker on a family while its accumulated context remains useful; checkpoint or restart when unrelated history dominates.

Validate every advertised research dependency inside the actual worker directory and execution account, not just the strict acceptance compiler. The Stunts model-routing pilot passed its raw/runtime whole-image baseline while omitting MSC 5.0 files needed by an allowed diagnostic profile. Those precompiler failures are setup defects, not negative compiler evidence or model-capability failures. The corrected capsule builder copies and hash-verifies every advertised pinned profile; existing trial records remain unchanged. Count proposed hypotheses, executed experiments, and unique effective outcomes separately when a setup failure interrupts a trial.

Check the repository's actual transaction closure before parallel edits. In Stunts, every file under `tools/` participates in promotion fingerprints, even a research-only CLI. Stage independent tool development under an ignored private build directory while a source writer is active, then integrate and refresh/validate serially. A new filename alone does not make shared writes independent.

In Stunts endurance-001, an initial allocation was mistakenly treated as a hard stop even though the worker already had shared-budget reallocation authority. Clarifying that authority enabled its own proposed same-TU experiment. This was an assignment/control interpretation problem, not evidence that Astra supplied a necessary source insight. Nine shared slots ultimately remained unused; neither per-case stops nor that incomplete sample establish global hypothesis exhaustion. [T3]

## Recipe 2 — Run a discriminating search

**Observe → predict → generate → compile → compare → deduplicate → update.**

For an allocation mismatch, a bounded matrix might contrast a parameter with an initialized local, with and without a `register` qualifier. Generate and compile the matrix mechanically; let the model interpret outcomes. Each axis needs a reason. Type, signedness, and pointer-model variants are hypotheses, not automatically equivalent transformations.

An experiment is informative when it changes a decision or constrains an explanation. A new object alone is not information gain. A repeated object can refute a proposed source distinction; repeated rediscovery is not progress. Negative conclusions remain limited to the tested compiler, flags, declarations, and context.

**Stop a search direction** when it repeats known outcomes without a new discriminator. **Continue investigation** when new evidence or a different analysis level remains available within budget. Do not turn the three-attempt convention into a capability claim, and do not use a research command to evade an existing block. [T1]

## Recipe 3 — Diagnose the right layer

| Observation | Next discriminating check | Do not conclude |
|---|---|---|
| Candidate is far smaller | Missing branches/calls, initialization, ABI, and optimizer deletion | “Just a stack-frame problem” |
| Similar bytes except registers/slots | Types, lifetimes, parameters versus locals, and a controlled fixture | “Rename variables until it matches” |
| Correct opcodes, wrong operands | Symbol identity, width, addend, frame, data ownership, relocation encoding | “Mask the operands” |
| Small unexplained extent difference | Complete contribution, compiler/linker alignment, shared entries, private data | “Trim or add padding” |
| Unchanged neighbors stop matching | Shared declarations, TU emission order, pools, compiler state | “Keep the fake body that preserved them” |
| Identical inputs produce inconsistent results | Runner, staging, isolation, tool identities, repeated controls | “Try another C expression” |
| Worker stops at a capability gate | Separate safe research from production support; identify missing proof | “Luna cannot solve this” |
| Strict object reader rejects a record | Preserve the failure; inspect a complete generic parse and record semantics in isolation | “The compiler failed” or “generic parsing enables acceptance” |

Prioritize structure and actual references over scalar similarity scores. A branch displacement can stay byte-identical while reaching a different instruction after another block changes. Preserve exact regions as evidence, not untouchable source-line boundaries. [T1, I1]

## Recipe 4 — Escalate information, not transcripts

Suggested routine handoff: **roughly 150–250 words**, not a correctness limit. Attach exceptional details when they change the decision.

```text
RESULT: exact candidate / informative negative / infrastructure-blocked
PROOF: scope, baseline and candidate identities, verifier result
NEW FACTS: decisive observations and their limits
EXHAUSTED: materially different hypotheses already tested
BLOCKER: exact missing fact/capability; contradiction if any
ASK: one decision or next discriminating experiment
ARTIFACTS: paths/hashes and reproduction command
COST: model-specific usage or telemetry reference
```

Put detailed logs and full compiler outputs in artifacts. Generate routine success summaries mechanically where possible. Compression must retain uncertainty and evidence links; a short confident but misleading summary is worse than a necessary longer one.

## Recipe 5 — Give Luna a fair comparison

For an unresolved task, first distinguish **model difficulty**, **insufficient evidence**, **poor tooling**, **budget exhaustion**, and **production-policy restrictions**. More than one may apply.

To compare models, hold starting evidence, tools, permissions, and meaningful budget comparable. To compare workflows, evaluate complete strategies—including preparation, extended Luna work, subsequent Astra reading, and integration. Astra inheriting Luna's experiments is not an independent from-scratch result.

Observe useful resolutions after the previous cutoff, repetitive searches, worker self-correction, and decisive missing evidence. Change one important factor at a time where feasible. Neither a small proposal-only benchmark nor a supervisor's later success establishes that the stronger model was required. Choose the next experiment from current results; do not restart useful work merely to satisfy a fixed sample-size plan. [T2]

## Recipe 6 — Turn repeated work into reusable capability

Aggregate blockers with overlaps intact. Rank a possible capability by tasks it would **actually make eligible**, not all tasks mentioning it. Give a cheap research worker a bounded fixture/proof task when appropriate; use Astra for synthesis when evidence justifies it.

Automate repeated parsing, candidate generation, build staging, comparisons, deduplication, and packet creation before adding more agent narration. Reuse scoped compiler findings with prerequisites and counterexamples. A tool earns its place by improving valid decisions, reducing redundant work, or unblocking recovery—not by producing more diagnostics.

Freeze a useful interface long enough to evaluate it. Do not confuse smaller files, fewer tokens, or additional tests with improved recovery throughput.

Stunts provides a concrete reuse example: three objects from two endurance cases contained B4/LEXTDEF local-symbol records. The strict production reader rejected them, but the existing generic OMF reader and frozen diagnostics exposed the records and fixups without an acceptance change. Same-TU work also encountered B6/LPUBDEF. Supporting static symbols in production still requires visibility-preserving interpretation and independently verified binding; generic decode alone makes no case eligible. Research found a 24-byte whole contribution with relevant call geometry, not an accepted 18-byte function obtained by trimming it. [T3]

For those existing objects, Stunts now provides a narrow reusable command instead of a case-specific inspection script:

```text
python tools/inspect_object.py --object path/to/complete.obj --target path/to/target.bin --evidence path/to/receipt.json --out-dir build/private/object-inspection
```

This command does not compile, bind or promote. Inputs are explicit and fingerprinted; it inventories supported records and visibility, verifies complete initialized coverage of the selected segment, and reuses the existing diagnostics. Output remains `RESEARCH_ONLY`, with the strict B4/B6 rejection intact. It rejects unknown record types, LIDATA, duplicate segment names and incomplete selected-segment coverage rather than presenting a misleading complete result. It is not a general OMF semantic validator. Full reports stay in the private artifact directory; no generated/reference binaries belong in Git.

Likewise, two named-segment declaration spellings rejected by pinned MSC 5.10, together with documentation dating the feature to C6, bound those tested syntax/profile hypotheses. They do not prove that every possible historical implementation requires C6 or that the target could not have been written in C/ASM under the pinned environment. Do not generalize a feature boundary into a language-origin claim. [T3]

## Recipe 7 — Promote without changing the claim

Keep three independent questions visible:

**Does it match? What produced it? How was it integrated?**

A candidate can match while the image still contains raw regions; an exact image can still use a nonhistorical construction process. Record source, authentic ASM, complete library reuse, original assets/data, research scaffolds, and unresolved raw ownership separately.

Freshly verify the entire declared contribution: extent, bytes, relevant private data, symbols, and complete fixup/relocation obligations. Then run the repository's integration checks, revalidate shared dependencies, and publish a receipt tied to the actual inputs. Cache research safely; do not substitute an old receipt for required fresh acceptance. Failed validation invalidates current success, not historical evidence.

A live Stunts example is `file_load_shape2d_palmap_init`: independent symbol evidence enabled the existing DGROUP binding mode, and its first candidate matched the target's 39 bytes after binding but emitted a 40-byte complete object contribution with a trailing NOP. That is a rejected candidate and a compilation-context question, not 39 recovered bytes or permission to trim. Keep this observation scoped to [attempt 0001](../recovery/attempts/file_load_shape2d_palmap_init/0001/report.json); later attempts must retain their own outcomes.

The pristine image also contains that NOP, but the separate following Restunts segment places it before its own procedure. Those segment files are not original linker/TU authority. The [boundary review](../recovery/attempts/file_load_shape2d_palmap_init/boundary-research.md) therefore retained the 39-byte verified procedure rather than expanding the interval merely because 40 bytes would match. A good near-match can reveal a missing ownership proof without authorizing a promotion.

Protect the oracle and acceptance implementation from incidental worker edits. Negative tests must reject plausible wrong candidates. A correct verifier only proves its declared scope; it does not police source provenance by magic. Freeze historical closure before making portable behavior changes. [E1, E2, T1]

## Transferable lessons from the four projects

| Documented observation | General lesson | Limit |
|---|---|---|
| Empires links its source/ASM plan with one TLINK invocation and checks the full image and ordered relocations. [E1, E2] | Track historical construction and source provenance in addition to output equality. | Exact reconstruction does not prove unique original source text. |
| Empires' inline-ASM path changes code generation across a TU; runner faults were independently reproduced. [E3, E4] | A local mismatch may originate in build context or the execution host. | Do not transfer Turbo C or runner-specific findings to other toolchains without a probe. |
| SimAnt compiled 384 candidates in four historical boots; the target still did not match. [S1] | Script controlled search and retain honest negative results. | More variants are not automatically better hypotheses. |
| SimAnt used selector pools/private data to establish same-object relationships, while exact boundaries remained qualified. [S2] | Recover build structure from multiple independent constraints. | Same-object evidence is not unique original-TU proof. |
| Icy Tower's GCC 4.4.1 scratch-register state changes later functions; a renderer candidate had omitted a real path to stay near target size. [I1, I2] | Restore actual structure, test coupled changes, and reject size-driven fabrication. | Compiler-state dependence is demonstrated here, not presumed everywhere. |
| Stunts' four-case presentation test saved 10.3% cumulative tokens but did not establish better recovery; the largest saving accompanied a weaker proposal. [T2] | Measure decision quality and complete workflow cost alongside token reductions. | Proposal-only observations do not establish an autonomous worker's capability ceiling. |
| Stunts endurance used 31 compiles on ten unresolved cases: 21 new effective outputs, eight repeats, two syntax rejections, zero strict promotions. Existing exact controls were not recompiled. [T3] | More runway can expose TU/compiler/tooling evidence without producing recovery; distinguish those outcomes. | This purposeful sample and inherited histories are not a matched model comparison or an optimal-budget estimate. |
| Stunts' corpus cost checkpoint attributed about 96% of the priced API-equivalent subtotal to Astra ($21.44 versus $0.91 for Luna). [T4] | Supervisor traffic can dominate cost even with extensive cheap-worker research. | Setup, triage and reporting are included; no autonomous-Luna or Astra-only counterfactual was measured, and these are not subscription charges. |

## Evidence and sources

Stunts' [bounded batching pair](../recovery/evaluation/model-routing/batching-decision.md)
used 10 model requests versus 13 sequentially and about 15% less direct model cost,
but found two unique outputs versus four and no exact solution in either arm.
Use small batches for independently justified alternatives; retain sequential
feedback when the next hypothesis depends on the last result. Deduplicate outcomes
under the same compiler recipe, and do not confuse fewer requests or cheaper runs
with improved recovery. This one-pair result excludes supervision costs and does
not establish an optimal batch size.

Sources are primary project documentation or official pricing. Historical figures above illustrate mechanisms; they are not live progress dashboards. Reproduce the relevant experiment before relying on its behavior in another project.

- **P1 — OpenAI API pricing:** <https://developers.openai.com/api/docs/pricing> (checked 2026-09-23).
- **P2 — GPT-6 Astra:** <https://developers.openai.com/api/docs/models/gpt-6-astra>.
- **P3 — GPT-6 Luna:** <https://developers.openai.com/api/docs/models/gpt-6-luna>.
- **E1 — Empires construction:** [README at `historical-exact-oracle-v1`](https://github.com/missingno7/empires_reconstruction/blob/historical-exact-oracle-v1/README.md).
- **E2 — Empires closure/provenance:** [closure frontier](https://github.com/missingno7/empires_reconstruction/blob/historical-exact-oracle-v1/docs/current/closure-frontier.md).
- **E3 — Empires TU context:** [TU structure](https://github.com/missingno7/empires_reconstruction/blob/historical-exact-oracle-v1/docs/current/tu-structure.md).
- **E4 — Empires execution environment:** [DOS runner](https://github.com/missingno7/empires_reconstruction/blob/historical-exact-oracle-v1/docs/dos-runner.md).
- **S1 — SimAnt controlled search:** [codegen infrastructure](https://github.com/missingno7/simantw_recon/blob/1397e995c7b3856daed112d162e3d57bf73f6d91/docs/codegen-infrastructure.md).
- **S2 — SimAnt structural evidence:** [build topology](https://github.com/missingno7/simantw_recon/blob/1397e995c7b3856daed112d162e3d57bf73f6d91/docs/build-topology.md).
- **I1 — Icy Tower compiler context:** [TU analysis](https://github.com/missingno7/icytower_recon/blob/af421c9f9a18d583f799adc82bab25f06ce36980/docs/tu-context-analysis.md).
- **I2 — Icy Tower evidence versus size:** [renderer corrections](https://github.com/missingno7/icytower_recon/blob/af421c9f9a18d583f799adc82bab25f06ce36980/docs/attempts/game-main/draw-frame-bar-20260923.md).
- **T1 — Stunts workflow and proof boundaries:** [grinder instructions](https://github.com/missingno7/stunts_recon/blob/0f7406d0ab9bd802a63c19fda48cdffa08ab652c/docs/current/grinder-instructions.md).
- **T2 — Stunts empirical limits/accounting:** [bounded comparison](https://github.com/missingno7/stunts_recon/blob/0f7406d0ab9bd802a63c19fda48cdffa08ab652c/recovery/evaluation/advice-comparison/README.md).
- **T3 — Stunts endurance evidence and limitations:** [final audit](../recovery/corpus/endurance-001/final-audit.md), [case evidence](../recovery/corpus/endurance-001/handoff.md), and [strategy](../recovery/corpus/endurance-001/strategy.md), published at commit `44cc8e1`.
- **T4 — Stunts total model-weighted cost:** [timestamped checkpoint](../recovery/corpus/endurance-001/cost-checkpoint.md) and [token accounting](../recovery/corpus/batch-001/usage.json), published at commit `44cc8e1`. The observation ends at its timestamp and excludes later publication work.
