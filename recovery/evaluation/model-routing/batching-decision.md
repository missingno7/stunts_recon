# Bounded batching comparison: completed

Use small batches when several hypotheses can be justified before seeing compiler results. Retain adaptive sequential investigation when the next hypothesis depends on those results. This one-pair Stunts experiment does not establish a recovery-efficiency advantage for batching and does not justify a batching framework or larger search quota.

## Controlled pair

Two fresh GPT-6 Luna/high contexts started from the same archived `unknown_libname_1` attempt 0001, target, prior evidence, tools and compiler recipe. Before launch, 451 common files matched; only TASK.md differed. Both had ceilings of four hypotheses and eight compiler processes, including strict verification. Both pinned compiler profiles were available. Later pilot repairs and trajectories were excluded. The user separately approved both runs and their data transfer. These counts belong neither to the eight-run model pilot nor the frozen 42/50 endurance cohort.

The sequential worker recorded a prediction and falsifier before each trial and could adapt after each result. The batch worker recorded three complete candidates and predictions before compilation, then used one mechanical helper invocation. The helper used the existing strict checks and returned a compact matrix grouped by effective identity. It changed no production acceptance rules. This tests two work policies, not identical candidate lists or isolated scheduling overhead.

| Observed measure | Adaptive sequential | Bounded batch |
|---|---:|---:|
| Model requests, including all work in the run | 13 | 10 |
| Hypotheses / compiler processes | 4 / 4 | 3 / 3 |
| Unique effective outputs | 4 | 2 |
| Strict exact matches | 0 | 0 |
| Worker wall seconds | 336.000 | 304.890 |
| Trial-helper seconds | 1.893 | 1.475 |
| Input tokens, including cached | 488,523 | 440,617 |
| Cached input tokens | 440,064 | 381,440 |
| Output tokens | 10,816 | 5,385 |
| Standard API-equivalent direct cost | $0.014655 | $0.012425 |

Per-request usage reconciles with each CLI's aggregate. Largest requests were 52,967 and 61,716 input tokens, respectively, below the long-context price threshold. Costs use the recorded [official Standard rates](https://developers.openai.com/api/docs/pricing); reasoning tokens are already included in output. These are estimates, not subscription charges. Request counts are an observable proxy for model round-trips, not measurements of internal reasoning rounds.

Batching used 23% fewer model requests, 9% less wall time and 15% less direct model cost. It also ran 25% fewer candidates and found half as many distinct outputs. Direct cost per unique output was about $0.00366 sequentially versus $0.00621 batched. Neither this denominator nor wall time establishes recovery productivity: outputs differ in information value and neither arm solved the target. Setup, supervision and reporting are excluded from these worker costs; no end-to-end savings are claimed.

## Information gained and limits

Both arms established that a register-qualified formal selects SI with save/restore, rather than the target BX pattern, and that a register local produces a 24-byte contribution against the 18-byte target. The batch's positive-body variant duplicated its first output. Sequential investigation additionally tested an unsigned integer pointer copy (28 bytes with AX/stack storage) and a post-increment condition (22 bytes, argument-slot increment and a second pointer load). These rule out those concrete source variants, not entire source or ABI families.

Sequential stopped at the hypothesis cap: censored evidence. Batch stopped after a repeated output without a proposed new analysis direction: reported local convergence, not proof of exhausted alternatives. Batch output identities are a subset of the sequential identities. The remaining question is how the historical compiler obtained BX reuse without extra spills or saves and with the exact target epilogue. Neither worker proved an external capability blocker or an exact solution.

The retrospective found ten within-run output repetitions among the earlier eight runs, concentrated in the two unresolved functions. It did not show that all 44 historical hypotheses could have been specified upfront. Later optimization, compiler-context and allocation pivots often depended on earlier results. Only an initial 2–4 candidate batch was supported conservatively; a precise fraction of safely batchable historical trials was not established. Stunts has no `96 variants per attempt / 192 candidates per target` machinery. The small experimental runner demonstrates mechanical compilation and deduplication without introducing that infrastructure into production.

The useful operational rule is to batch a small set of independent, evidence-backed alternatives and interpret only unique outcomes; avoid a factorial cross-product unless each interaction is justified. The observed register-formal/local and guard-form alternatives were suitable to test, but the duplicate guard outcome shows that extra combinations can buy little information. Stop or change analysis level when outputs repeat. Keep sequential feedback for hypotheses that require newly observed allocation, extent, fixup or compiler-context evidence.

One selected unresolved target, one run per policy, different candidate selection and stop points, and no strict successes are insufficient to estimate recovery-speed gains or an optimal batch size. Do not expand this pilot solely to obtain a positive result. No SOL run was performed.

## Evidence and validation

[Results](batching-results.json) retain hypotheses, predictions, falsifiers, identity hashes, outcomes and local capsule paths. [Usage](batching-usage.json) retains request-level counts without conversation text. Detailed sources and diagnostics remain in the two local capsules identified there. The [retrospective](batching-retrospective.md) and [primary model report](decision.md) remain separate.

No production source or manifest was promoted. Baseline validation passed 139 tests and independently recompiled all 11 active C functions; after the experiments, current acceptance inputs still matched the HYBRID_EXACT receipt. Production remains 408 C bytes, 0 ASM bytes, 725 pinned runtime bytes, 198,867 raw initialized bytes and 158,205 unknown-classification bytes. These are distinct ownership/classification measures, not additive recovery claims.
