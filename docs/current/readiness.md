# What is ready for grinding

The routine loop is established for **verified leaf contributions with supported binding**. It checks candidate eligibility, source-only scope, immutable targets, complete object extents/declarations/fixups, budgets, fresh whole-image promotion, rollback, independent active-code recompilation and queue freshness. Generated status and validation receipts carry current fingerprints; evergreen instructions do not hard-code recovery totals.

This is not yet a project-wide “just grind every function” state. The queue distinguishes work supported today from supervisor research. Consult generated `status.json` and each row's capability blockers rather than interpreting a short function as easy C.

## Evidence that the process works

- `parse_shape2d_helper`: three alternative ordinary-C expressions failed complete-extent checks. Sources, compiler receipts and diagnostics were archived. An unchanged retry was refused; the third failure moved the task to SUPERVISOR automatically.
- `parse_shape2d_helper3`: the first hypothesis matched the complete 46-byte contribution. The same workflow promoted it through fresh staged and canonical builds. Tests passed without editing hard-coded recovery or compiler-receipt counts.

Regression tests cover budget exhaustion, unsupported-feature escalation, interval blocks after renaming, legacy blockers, reopen epochs, stale workflow fingerprints, stale recipe mappings, symbol-error diagnostics, canonical-write interference and rollback. Independent compilation ignores inactive hypotheses and checks every active C owner.

## Remaining gates for broad grinding

1. **General medium-model binding.** Support far calls/pointers, self-relative references, additional frame methods, target displacements and exact ordered MZ relocations. Each mode needs historical LINK differential and negative tests before its queue category is unlocked. Current external DGROUP offset16 support is narrower.
2. **Original contribution and TU boundaries.** Establish compiler alignment, shared tails/external jumps and multi-function contribution layout. A source function's RETF is not automatically its full emitted object extent. Persistent opcode/frame differences need discriminating compiler/profile evidence rather than endless syntax variations.
3. **Reviewed global/data dependencies.** Add original symbol/frame evidence for blocked globals; develop a reviewed proposal workflow for richer data/BSS ownership and includes. Addresses are currently explicit in `layout/data-symbols.json`; register-relative offsets are not blindly assumed to be globals.
4. **Expand verified mapping.** Many imported procedures lack complete boundaries or code/data separation. Prove coordinates before preparing recipes. Restunts segment names and materialized BSS are not original layout authority.

QuickC BAKPAT, the archived CRT checksum anomaly, matching-ASM origin and data-emission backends remain specialized research. They need not block safe leaf-function grinding, and their rejection must remain explicit.

## Reproducible handoff

Run `python tools/validate.py`, read generated `validation.json` and `status.json`, then use `reconstruction_factory.py next`. Follow `grinder-instructions.md` without hand-editing counts, test expectations or production ownership. Provisioning remains hash-pinned in `layout/toolchain.json` and `layout/oracle.lock.json`; downloaded binaries and generated objects stay outside tracked source.
