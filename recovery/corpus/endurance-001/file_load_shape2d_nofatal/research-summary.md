# file_load_shape2d_nofatal endurance result

**Status: unresolved, budget-censored.** Two compiler processes ran under `msc510-medium` with `/AM /O /Gs`. A third research-adapter call failed before launching a compiler because its work-directory name already existed; it consumed no compiler slot. The frozen target is 18 bytes, SHA-256 `8c0aee23fb3b48de8f827aa81776bc9619edb46ef9a326b248d548968486d7cf`.

Trial 1 isolated the far-return wrapper and declared the callee external with its far-pointer return. MSC emitted 20 code bytes and a pointer32 external fixup to `_file_load_shape2d` (effective identity `d6eb7dceb5889480bd893a7f8b995106caf4f2cd7abb35a77c0af9be1319ff2a`). Prologue, argument pushes, cleanup, and return were anchored; call lowering was a far `9A` instead of target `PUSH CS; CALL near`, plus the object had an alignment NOP. Extent and bytes failed.

Trial 2 declared the external callee near. Code length became 18 bytes, but bytes still differed (effective identity `f33bac3690c5f1ecec06bcf61768af2a3152e273ba4faec2713c515fe7fc40d1`). The call became `CALL rel16` with a self-relative `offset16` fixup to `_file_load_shape2d`; there was no `PUSH CS`, and the code ended with a NOP. This establishes that the explicit near declaration changes call mode and that the object carries a same-segment relocation requirement. It does not produce the target's far-call stack setup or resolved +5 displacement.

The target assembly shows `PUSH CS; CALL near` to the immediately following `file_load_shape2d` implementation in the same segment/TU. The matching next question is whether compiling the wrapper with a local far callee definition reproduces the target's internal-call lowering and exact displacement. A controlled same-TU stub candidate was prepared, but not compiled: both actual compiler slots were spent. The unrun source and its prediction are explicitly labeled in the case directory.

This is useful narrowing of a workflow/TU gate, not evidence that Luna cannot solve the source task. Strict extent matched once, but complete bytes did not; unresolved symbol relocation and call mode remain. No exact or acceptance claim is made.

Per-trial artifacts include source and precompile prediction/falsifier, compiler receipt/log, full object/code and OMF declaration/fixup details, target/code bytes, strict comparison, and compact/full frozen-engine diagnostics. Engine SHA-256: `a5df723b30e4581e53434d32dea82798c625c34e84bf55330c56a7f81c23ccc3`. Token usage by model/cache category is unavailable.
