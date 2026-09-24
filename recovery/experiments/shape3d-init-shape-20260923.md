# Shape initialization source probe

Authorization: user requested the next best recovery steps. One production grinder task has three source attempts. The verified pristine interval is `load_1955a`, `[103770,103942)`, 172 bytes, with no MZ relocations. `layout/function-evidence.json` records a reviewed 69-instruction linear CFG: one entry, one `retf`, no branch, call, or padding. Restunts `asmorig/seg008.asm:3898-3973` supplies names and structure hints; the original image supplies the machine behavior.

## Hypothesis 1 (before compilation)

The Restunts sequence of header-to-structure assignments and pointer offsets is expressible by MSC 5.10 medium-model C. The destination paint count must be an 8-bit field: pristine `19582` stores only `AL` to offset 8, whereas Restunts declares a 16-bit field. The candidate keeps the observed field offsets and assignment order.

Prediction: the compiler emits a 172-byte complete code contribution with one far return, repeated `LES SI,[BP+6]` for the first three header reads, no fixups, and stores to destination offsets `0,6,8,2,14,18,10` in that order. Pointer offsets use 16-bit offset arithmetic and preserve the input segment.

Falsifier: different complete extent, calls/fixups, field widths or order, or any original byte mismatch. A partial opcode resemblance does not establish acceptance.

Outcome: attempt `0001` emitted 190 bytes and 76 instructions. The compact diagnostic aligned 62 target instructions and localized the principal difference to a four-byte stack frame storing `hdr`: the three original `LES SI,[BP+6]` reads became `LES SI,[BP-4]`. Extra frame teardown and a trailing NOP followed. This disproves the local-alias source form, while supporting the field and arithmetic structure.

## Hypothesis 2 (before compilation)

Accessing the three header bytes directly through the far argument eliminates the compiler-created local far pointer. Prediction: no `SUB SP,4`, no writes to `[BP-4]`/`[BP-2]`, and the three `LES SI` sites use `[BP+6]`; the complete extent contracts toward 172 bytes and the trailing epilogue matches. Falsifier: the compiler still materializes a local alias, changes the preserved arithmetic anchors, or fails any complete byte/fixup obligation.

Outcome: attempt `0002` returned `FAST_PASS_ONLY`: the untouched complete compiler contribution is 172 bytes, byte-identical to the original interval, with no fixups or relocations. Promotion still requires a fresh staged and canonical whole-image build.

Attempt `0003` promoted the full 172-byte contribution. Fresh `tools/validate.py` passed the immutable oracle, full hybrid image, 140 tests, and independent DOSBox-X compilation for all 14 active C functions. This proves the current C contribution, not that Restunts captured the unique historical source or original translation-unit layout. The direct source field widths and emitted offsets are independently checked against pristine bytes.
