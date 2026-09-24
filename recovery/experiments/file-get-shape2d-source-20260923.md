# `file_get_shape2d` source test

The pristine contribution is `[0x2265B,0x226BA)`, 95 bytes, SHA-256
`89258c68d62b065a0ae09f36b9bb7c75c3a4f365b6313aad5d54ac7a7400cc05`.
The reviewed overlay checks all 44 decoded instructions, complete reachability,
and adjacent boundary anchors. There are no calls, branches, external data
references, or MZ relocations. The last byte is `retf`; the next function's
prologue begins immediately.

The candidate follows the Restunts `shape2d.c:226` source hint. Machine-level
prediction before compilation: pinned MSC 5.10 medium will emit a full
95-byte contribution with the count and index both scaled by four, the count
scaled again for the data area, an inline 32-bit offset addition, four
`shl/rcl` pairs for segment-to-linear conversion, then four `shr/rcr` pairs
and a DX:AX normalized huge-pointer return. Falsifier: another length,
different scaling/carry/normalization, a helper call, unresolved fixup, or
extra pad. The first production attempt uses one of the card's three source
attempts and cannot be promoted without strict whole-image verification.

The first grinder attempt failed strict extent: the complete output was 114
bytes, with an unresolved `__AHSHIFT` fixup and a 14-byte stack frame rather
than the pristine 95-byte inline body. The full object, compiler receipt, and
diagnosis are in `recovery/attempts/file_get_shape2d/0001/`.

Two independent research-only alternatives were frozen before compiling in
`build/private/research-plans/file-get-shape2d-linear-20260923/manifest.json`.
`file-get-shape2d-linear-cast-20260923.c` reinterprets the far pointer as a
packed long and explicitly linearizes it; `file-get-shape2d-linear-union-20260923.c`
accesses the offset and segment through a union. Each had a predicted
helper-free inline conversion and a falsifier. The hard ceiling was two
compiler processes; two ran. Both removed external fixups, but emitted
distinct 138-byte and 146-byte contributions, each with only six byte-exact
aligned target instructions. The batch and original objects/logs/diagnostics
are under `build/private/research-batches/file_get_shape2d/c0cd384a2f46/`.

These results distinguish huge-pointer helper lowering from explicit
linearization, but none reproduces the 95-byte code. The function remains
raw-owned. Its original source construction or TU/compiler context is still
unknown; a new production attempt needs a source-level mechanism predicted
to account for the inline conversion and complete extent. No object
trimming, operand patching, or fixup masking is authorized.
