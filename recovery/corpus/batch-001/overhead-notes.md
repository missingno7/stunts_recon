# Partial token-accounting checkpoint

This checkpoint counts root token events at/after supervisor-start timestamp 2026-09-23T13:03:13.647Z (recorded ordinal 3398) through the latest checkpoint, including later root steering turns. It includes direct `corpus_*` and `endurance_*` workers plus every descendant session in those worker trees. Root events are summed; each worker session contributes its latest cumulative usage. Model totals remain separate. The checkpoint is partial and should be rerun after all workers and final collection complete.

`api_equivalent_cost_scenarios` applies `pricing-reference.json` per request. These Standard/Fast totals are published API-equivalent scenarios, not actual Codex charges; the recorded service tier is unavailable. Models without published rates in the reference remain unpriced.

Tool-call categories are counts only. The serialized logs expose orchestration calls for this checkpoint; low-level exec/compiler calls are not represented as counted function_call events, so their counts are unavailable rather than zero. Supervisor handoff text is reported by character count only, with no causal token or dollar allocation.

Reasoning tokens are reported as a subset of output tokens. Uncached input is computed as input minus cached input; cached and cache-write input remain separate.
