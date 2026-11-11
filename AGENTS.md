# Stunts 1.1 reconstruction and port

Read README.md. This tree rebuilds the original MCGA executable byte for byte from C, assembly and the pinned Microsoft runtime, and builds a native SDL3 port from the same sources.

## The invariant

The original assets and `layout/oracle.lock.json` are immutable. Everything else may change (sources, recipes, names, tools, docs) as long as `python tools/validate.py` still reports the exact image with its ordered relocations. A change to `src/`, `asm/`, `recipes/` or `layout/` is proven by the gate, never assumed neutral: local identifier names affect MSC stack-slot order and communal names affect link placement.

Never trim objects, mask acceptance mismatches, ignore inconvenient OMF records, embed original machine code as C, patch final binaries, or regenerate expected bytes from candidates. Preserve complete object extents and declarations, every supported fixup, exact ordered relocations, independently grounded bindings and accepted ownership.

## Stages

Work proceeds in this order; do not start a later stage while an earlier one is open unless asked.

1. **Source side.** Settle remaining origin and attribution questions (ASM versus compiled C, compiler versions, natural link order) with one bounded investigation each, then document what stays unknown. Replace placeholder names. Prune superseded sources, recipes, campaign-era tools, tests and docs.
2. **Faithful port.** The SDL3 port reproduces original behaviour; the indexed 320x200 renderer is its visual oracle.
3. **Enhanced side.** Native rendering grows from understanding the game's scene and renderer, not from further interception of the DOS pipeline.

## Changing sources

Use `tools/context.py FUNCTION`, an isolated directory under `build/workers/NAME/`, `tools/search.py`, and `tools/promote.py` (`--batch` for many candidates). Canonical source, recipe, ownership and acceptance-tool changes have one writer at a time. Run the complete `tools/validate.py` at acceptance and tooling boundaries, not for every hypothesis; it needs Capstone 5.0.3 and Unicorn 2.1.4 in `build/python` (see README).

Keep original facts, source hypotheses, tooling limits and acceptance requirements distinct. Similarity and normalization are diagnostic only. Report C, ASM, pinned runtime and raw bytes separately; raw is currently zero and must stay zero.

## Port

Port code lives in `port/` and its tests in `tests/test_sdl3_*`. Establish a routine's widths, signedness, spans and aliases from the locked machine code before choosing a host representation, and compare against the original with `tools/porting/diffharness`. The faithful renderer stays available and exact when the enhanced renderer changes.

## Workers

Use GPT 6.1 Sol (`gpt-6.1-sol`, `high`) for investigations, shared technical obstacles, cross-function synthesis and integration. Use GPT-6 Luna (`gpt-6-luna`, `xhigh`) for mechanical or tightly bounded tasks. The supervising session takes the hardest blockers itself instead of re-delegating them. Each worker owns its hypotheses and scratch workspace under `build/workers/NAME/`. Keep handoffs to the result, the concrete obstacle and the smallest useful evidence references.

Continue while there is a useful next investigation; stop at a solution, a concrete missing dependency, or when none remains. New hashes alone are not progress. Respect explicit user budgets, cancellation and platform limits.

## Repository

Work lands on `portable-sdl3`; commit verified steps, push only when asked. Do not commit binaries or generated objects. Preserve ignored assets, pinned tools, reference checkouts and local-only inputs; never run blanket `git clean -fdx`. Keep durable source fragments small and generated data under ignored `build/`.
