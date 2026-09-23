# rect_adjust_from_point endurance result

**Status: unresolved, budget-censored.** Two compiler processes ran, using `msc510-medium` with `/AM /O /Gs`; no precompiler failure occurred. The frozen target is 76 bytes, SHA-256 `7a66ad05b0867d74d0feb0da20b8c216595645babc1f7607739adc239af8e16a`.

Trial 1 preserved the Restunts C body and added the far function distance observed in the target. The full object was 348 bytes, with one 84-byte `UNIT_TEXT` segment, public `_rect_adjust_from_point`, no fixups, and effective identity `9061afeabf0550f337eb9b9eeeded8b234db42772b780fdbe70ee4daafd9f899`. Strict extent and bytes both failed (84 vs 76). The frozen diagnostic found 22/33 target instructions byte-exact across six anchors; BP prologue/local size and coordinate register use differed.

Trial 2 introduced explicit `px`/`py` locals so the compiler could retain point coordinates across the bounds checks. This produced a new 86-byte class, effective identity `53d6ca6222ff14a728a01df393d1709c54fef24ef39f45d9afebde01680cd7ec`, with no fixups. It grew rather than approached the target. The diagnostic saw fewer anchored target instructions (15/33 across four anchors). Candidate code materialized both coordinates in BP locals and reused AX; target keeps them in SI/DI. Thus the test establishes that ordinary local caching under the pinned profile does not reproduce the observed target allocation.

This is a source/codegen mismatch, with no object-binding or workflow gate identified. The remaining hypotheses include different declarations/lifetimes or TU/compiler context that cause SI/DI allocation. Two controlled experiments do not establish a Luna capability limit; the case is quota-censored. No strict match or acceptance claim is made.

Exact source, prediction/falsifier, compiler receipt/log, complete object, all segment/declaration/fixup metadata, target/code bytes, and compact/full frozen-engine diagnostics are archived per trial. Diagnostic engine SHA-256 is `a5df723b30e4581e53434d32dea82798c625c34e84bf55330c56a7f81c23ccc3`. Token usage by model/cache category is unavailable.
