# Separate model/configuration check

After the four-function presentation comparison, the user requested Luna/high for grinding and suggested token/time measurements. Two fresh Luna/high contexts received the identical rectangle and pointer Y packets, prompt and four-read allowance previously used by Astra/medium. No other results or rationale were shown. Recorded calls stayed within their packets. This is a separate comparison of **model plus reasoning setting**, not additional evidence for the presentation A/B.

| Packet | Configuration | Total tokens | Uncached input | Output (including reasoning) | Wall seconds | Additional reads |
|---|---|---:|---:|---:|---:|---:|
| Rectangle | Astra/medium | 99,304 | 17,318 | 578 | 31.731 | 2 |
| Rectangle | Luna/high | 153,751 | 18,996 | 1,635 | 44.154 | 3 |
| Pointer | Astra/medium | 92,801 | 11,761 | 528 | 29.373 | 2 |
| Pointer | Luna/high | 103,020 | 8,334 | 2,270 | 54.907 | 2 |

Luna/high was slower and used more total tokens in both selected runs. Pointer uncached input was lower, so individual usage measures do not all move together. No price comparison is made; tokens and latency are not monetary cost.

Quality also differs. On rectangle, Luna abstained because the source cause was unproven, while Astra proposed a concrete register-parameter test with an explicit falsifier. A hypothesis need not already be proven to justify a bounded experiment on an unblocked task. Luna's abstention did not demonstrate equivalent next-experiment productivity. On pointer, both proposed explicit one-bit shifts, but Astra specified separate statements, an exact expected size and the independent SUB/XOR discrepancy. Luna's wording did not distinguish nested expressions from separate statements, referred to an "existing xor dx,dx" although the candidate uses SUB, and weakened its falsifier to movement toward the target. These are limitations beyond broad category agreement.

Only two packets, one run per configuration, selected after the first comparison. Not randomized, not a benchmark of source grinding, and no robust model ranking follows. `model-check-proposals.md` preserves verbatim replies; `usage-model-check.json` preserves actual telemetry and its definitions. User preference remains Luna/high for delegated grinding.

## Ordinary grinding telemetry, separate workload

One Luna/high agent then completed two bounded source tasks, six durable source attempts: `nopsub_326BA` failed three times and stopped at its budget; `copy_string` failed twice then reached FAST_PASS_ONLY. The run took 542.571 seconds (9.04 minutes), used 2,269,388 cumulative total tokens, of which 2,153,984 were cached input, 99,126 uncached input and 16,278 output. Output includes 8,942 reasoning tokens. There were 37 model tool calls. These cumulative inputs repeatedly include the same context; they are not unique context size.

Parent review, fresh promotion and final validation are excluded. No matching Astra grinding run exists, so this cannot show that Luna is cheaper, faster or equally effective at grinding. The actual source result is useful nonetheless: root freshly promoted the exact 56-byte `copy_string` contribution via `grind.py`, report `recovery/attempts/copy_string/0004/report.json`. Diagnostic causality is not claimed; ordinary islands supplied the decisive local-pointer and increment-order observations.
