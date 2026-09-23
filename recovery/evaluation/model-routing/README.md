# Model-routing pilot: completed checkpoint

The separately approved two-run batching comparison is also complete: see
[batching-decision.md](batching-decision.md). It reduced direct worker requests
and cost in one pair, but produced fewer distinct outputs and no strict recovery;
it does not establish better recovery throughput.

The existing-data audit is complete; see [audit.md](audit.md) and
[existing-evidence.json](existing-evidence.json). Before this pilot, no existing comparison isolated
Luna versus Astra on strict source recovery. Proposal quality, compiler outcomes,
and strict recovery remain separate outcomes.

The [recorded selection](pilot-design.json) starts with four matched pairs:
copy_string, rect_compare_point, parse_shape2d_helper, and unknown_libname_1.
Both conditions use high reasoning, identical frozen source/evidence/tools, and
ceilings of 15 meaningful hypotheses and 20 actual compiler processes per task,
including strict verification recompiles. The earlier 42/50 recovery cohort is
frozen and separate.

## Prepared state

[prepared-pilot.json](prepared-pilot.json) locates all eight disposable copied
directories and records matching input-manifest hashes within each pair.
Every capsule passed the existing whole-image verifier with game C owned by raw
oracle ranges and pinned runtime ownership retained. Preparation launched no
compiler processes. Later source solutions, Git history, and modern Restunts C
are absent. This is procedural blinding, not an adversarial filesystem boundary.
Caller/TU evidence beyond the supplied packet must be derived from the available
original image and metadata; a missing-evidence stop must be reported as such.

The local trial helper archives source, prediction and falsifier before compiling,
counts actual process launches, preserves full object and structured mismatch
evidence, and invokes the unchanged strict candidate and whole-image checks.
Diagnostic profile/flag probes consume the same allowance but cannot earn strict
credit. Source/manifest changes in these capsules are research outcomes only.

## Run status

The specific transfer and eight matched high/high runs were approved and completed.
Five earlier launch preflights are retained separately from the eight completed
runs; they were access/orchestration failures with no model-token usage. See the
compact report at [decision.md](decision.md), the machine-readable
[results.json](results.json), and the request-level [usage.json](usage.json).

## Accounting and validation

The local collector `build/model-routing-pilot/collect_usage.py` separates primary
trajectories from all preparation/supervision after the cutoff in
[supervisor-start.json](supervisor-start.json), including interrupted preparation.
`build/model-routing-pilot/run-map.json` identifies participating sessions; append
actual primary session IDs only with their observed launch state. Per-request
usage is deduplicated; reasoning is a subset of output. Standard/Fast prices are
alternative API-equivalent scenarios, not Codex invoice charges.

Production acceptance inputs were checked unchanged against the current
HYBRID_EXACT receipt after capsule preparation. The preceding baseline validation
passed 139 tests and independently recompiled all 11 active C functions. Current
production ownership remains 408 C bytes, 0 ASM bytes, 725 pinned runtime bytes,
198,867 raw initialized bytes, and 158,205 unknown-classification bytes. No
production source promotion occurred during this pilot preparation.
