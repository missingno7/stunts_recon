# Factory architecture and limits

`tools/mz.py` and `tools/omf.py` are adapted from the pinned Empires factory. The standalone MZ parser and full OMF segment/group/public/fixup metadata were reusable. Empires' compact-model binder, fixed segment assumptions, Turbo C settings and specialized runtime grinder were deliberately not transplanted. The source provenance chain is in `provenance.md`.

## Implemented path

1. `oracle.py verify`: reconstruct and verify immutable oracle; never updates its lock.
2. `import_restunts.py`: refresh address/semantic evidence with pinned source hashes and full binary comparison anchors.
3. `prepare_candidate.py`: create a bounded recipe only from a verified machine extent and a separately written C hypothesis.
4. `toolchain_probe.py`: validate every tool/header/library hash or run a research compiler experiment.
5. `probe_module.py` / `check_candidate.py`: compile fresh `UNIT.C` in a unique directory, read strict OMF, compare the entire emitted contribution, publics, complete fixup obligations, relocation absence, length and bytes.
6. `check_candidate.py --promote`: scope check; FAST; fresh staged full-image construction; publish source/recipe/ownership; fresh canonical full-image construction; promotion receipt. A single-writer lock serializes promotion; failures roll back staged source and canonical metadata.
7. `build_exact.py verify`: freshly compile every C owner, reopen/hash each pinned library, concatenate explicit owner contributions, construct synthetic MZ metadata, compare every load byte and ordered relocation pair, and check inputs again. No accepted object cache is read.
8. `reconstruction_factory.py refresh/next`: capability-based queue, explicit card paths, interval-aware blockers and generated status. Construction and workflow fingerprints separately invalidate stale results.
9. `grind.py`: archived hypotheses, structured diagnostics, duplicate rejection, bounded budgets and explicit reopen epochs; CLI promotion uses the same control state.
10. `validate.py`: full tests, fresh whole-image build, active-only independent compilation, prior-ownership and queue/card audits in one command.

`src/` is active recovered C. `recovery/candidates/` and `recovery/blocked/` are inactive. `layout/production-plan.json` lists ordered consumed inputs and must agree with the ownership manifest. Raw regions are direct oracle owners, visibly distinct from source recovery. The reconstructed packed EXE is verified by the oracle; production builds the exact **canonical unpacked** MZ, not a historical linker/EXEPACK rerun.

## Supported OMF production subset

The generic reader exposes SEGDEF, GRPDEF, PUBDEF, EXTDEF, COMENT, LEDATA and full FIXUPP frame/target/addend metadata. The strict wrapper verifies record framing/checksums, MODEND, unique segment names, initialized coverage, public bounds and fixup bounds. It rejects unsupported records, duplicate initialized writes, BAKPAT, COMDEF, 32-bit records and holes rather than masking evidence.

Current C production accepts one complete text contribution, one public at offset zero, and no additional data/BSS. It supports fixup-free objects or explicitly reviewed external DGROUP offset16 bindings. External declarations must agree with the applicable binding mode; unbound dependencies are never ignored. Preprocessor directives and inline ASM/raw emission are blocked until their dependency closure is supported. Compiler alignment belongs to the owned contribution: rectangle containment owns 51 function bytes plus the actual emitted one-byte NOP. No object is trimmed.

Pinned runtime production accepts entire exact library CODE contributions with checked publics/external declarations, no other owned data/BSS, no fixups and no intersecting MZ relocations. Active module counts and byte totals are generated in `status.json`. Three helpers use ordered LEDATA overwrites (retf changed to retf 8); exact member/record policies and eight historical LINK experiments establish the supported behavior. Default object reading still rejects overlaps; see `library-binding.md`. Library promotion uses the same serial FAST/staged/canonical discipline as C promotion.

## Still incomplete

A general medium-model FIXUPP binder is not implemented. The bounded external DGROUP offset16 subset is supported and checked against historical LINK; see `data-binding.md`. Far call frames, other frame methods, nonzero target displacements, aliases, startup and general cross-module layout remain blocked. `probe_tu.py` reports inspection metadata by default; with a reviewed target it compares the entire contribution and emits **TU_EXTENT_MATCH_ONLY**. The 114-byte combined rectangle pair matches with publics at offsets 0 and 62. This proves coexistence in a TU, not its original complete boundaries, and does not link a complete historical program. There is no matching-ASM emitter or exact-data authoring backend yet; those owner kinds fail closed.

The full hybrid is exact by construction and comparison, with raw ownership reported explicitly in generated status. Complete executable/data classification is unresolved. The current monotonic metric counts active source/library ownership, not imported assembly or equivalent replay behavior.
