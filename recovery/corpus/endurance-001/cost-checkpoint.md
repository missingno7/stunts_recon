# Model-weighted cost checkpoint

Timestamp: 2026-09-23T14:32:11+00:00. Scope: this corpus batch from its recorded supervisor start, including root steering, workers and descendants, final audit and report preparation. Later publication/final-response tokens are not yet observed.

| Model | Uncached input | Cached input | Output (includes reasoning) | Standard API-equivalent | Fast API-equivalent |
|---|---:|---:|---:|---:|---:|
| gpt-6-astra | 236,393 | 16,498,304 | 51,479 | $21.4362 | $42.8724 |
| gpt-6-luna | 1,441,826 | 55,760,384 | 409,120 | $0.9063 | $1.8127 |

Priced subtotal: $22.3425 Standard, $44.6851 Fast. Astra represents 95.94% of that modeled subtotal. One auto-review request is unpriced. No request crosses the published long-context threshold; actual service tier and subscription charges are unavailable. These are alternative price scenarios, not an invoice.

The batch yielded one strict promotion (`toupper`, 24 bytes); the extended cohort yielded none. Do not compare this setup/triage/research/reporting cost with an unmeasured Astra-only or autonomous-Luna counterfactual. Supervisor traffic is included in recorded input/output, but its causal attribution to individual handoffs is unavailable.

The source rates, request-level method and token reconciliation are in `../batch-001/pricing-reference.json`, `../batch-001/usage.json` and `../batch-001/overhead-notes.md`. Published same-tier per-token rates differ by 100×; the different token volumes make aggregate cost ratios different. Detailed session contents are not published.
