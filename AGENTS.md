# Historical matching work

Read README.md. Use `context.py FUNCTION`, an isolated directory under `build/workers/NAME/`, `search.py`, and `promote.py`. Run complete `validate.py` at meaningful acceptance/tooling boundaries, not for every hypothesis.

Use GPT-6 Luna with extra-high reasoning (`gpt-6-luna`, `xhigh`) for independent function/family investigations. Each worker owns its hypotheses and scratch workspace. Use GPT-6 Sol/high selectively for shared technical obstacles or cross-function synthesis. The migration smoke test explicitly uses Luna/high. Keep handoffs to the result, concrete obstacle, and smallest useful evidence references.

Workers may investigate types, signedness, declarations, register/local allocation, control flow, ABI, callers/callees, compiler behavior, and plausible TU context. Choose individual experiments or useful batches; inspect actual compiler feedback and adapt. No fixed attempt counts, quotas, batch sizes, supervisor checkpoints, research permissions, or reopening ceremonies apply.

Continue while finding a useful next investigation. Progress includes strict matches, narrowed mismatches, distinguished explanations, and useful compiler/context facts. New source/output hashes alone do not prove progress; repeated output may answer a deliberate question. Reassess or stop at a solution, concrete missing dependency, or no useful next investigation. Respect explicit user budgets, cancellation, and platform limits.

Keep original facts, source/context hypotheses, tooling limitations, and strict acceptance requirements distinct. Use compact diagnosis before expanding full disassembly. Preserve exact anchors as evidence, inspect meaningful changes, and state a machine-level prediction and falsifier when useful. Similarity and normalization are diagnostic only; unresolved operands and unsupported records must remain explicit.

Investigations may run concurrently. Canonical source, recipe, ownership and acceptance-tool changes have one writer. Submit exact candidates through `promote.py`; technical acceptance requires no additional permission ceremony. Shared proof changes need focused tests and single-writer integration.

The original assets and `layout/oracle.lock.json` are immutable authority. Preserve complete object extents/declarations, every supported fixup, exact ordered relocations, independently grounded bindings, previously accepted ownership, and fresh whole-image equality. Never trim objects, mask acceptance mismatches, ignore inconvenient OMF records, embed original machine code as C, patch final binaries, or regenerate expected bytes from candidates. Keep modern builds separate.

Raw ownership remains explicit reconstruction debt. Report C, ASM, pinned runtime and raw bytes separately. Preserve ignored assets, pinned tools, reference checkouts, and local-only inputs; never run blanket `git clean -fdx`. Do not commit binaries or generated objects. Context and validation must work without cards, queues, ledgers, or old research archives. Keep new durable source fragments small and detailed generated data under ignored build output.
