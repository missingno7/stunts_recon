# `rect_union` source and data evidence

The pristine contribution is `[0x16572,0x165EC)`, 122 bytes, SHA-256
`1a3c53b717bb6c23be598af0963dbe58fd02c55a5b25d15b885b335fb3077e34`.
`layout/function-evidence.json` checks all 47 decoded instructions against the
image, the two boundary anchors, every branch destination, and complete
reachability. The source listing at Restunts `seg006.asm:2759-2818` agrees with
the four extrema and final right-edge normalization, but its symbolic operand
labels alone are not address proof.

The independent pristine initialization sequence in `init_main` starts at load
coordinate `0x29F27`: `C7 06 58 96 01 00` writes 1 to DS:`9658`, immediately
followed at `0x29F2D` by `C7 06 64 9D FF FF`, writing `FFFF` to DS:`9D64`.
Restunts `seg031.asm:237-238` labels these consecutive writes
`video_flag2_is1` and `video_flag3_isFFFF`; `dseg.asm:36307,38067` declares
both as words. The target `rect_union` reads DS:`9658` twice and DS:`9D64`
once in the matching normalization expression. This paired writer plus the
separate reader and widths supports the two symbol identities. Their original
DGROUP load addresses, `0x34DC8` and `0x354D4`, derive from the independently
relocated frame `0x2B770`, not candidate operand bytes. Both lie in the
startup-cleared BSS interval `[0x30D3A,0x36490)`.

Two source forms and falsifiers were frozen in
`build/private/research-plans/rect_union-20260923/manifest.json` before
compilation. The first explicit `if/else` form emitted 166 bytes. The second
conditional-expression form emitted exactly 122 bytes; all six raw differences
from the pristine extent are the three zero-valued offset16 fixup payloads at
relative offsets 94, 109, and 114. Its 47 decoded instructions align with the
target. The research batch is archived under
`build/private/research-batches/rect_union/e5c840e13d37/`. The reviewed
recipe records the complete object declarations and ordered fixups. A fresh
`grind.py` FAST probe matched the full contribution, and serial promotion
recompiled the source and verified both staged and canonical whole images.
The accepted source is `src/rect_union.c`; the promotion receipt is
`recovery/promotions/rect_union.json`. This establishes exact game C ownership
for these 122 bytes, not unique original source text.
