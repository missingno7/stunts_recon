# `rect_intersect` first-condition test

Pristine extent `[0x165EC,0x16670)`: 132 bytes, SHA-256
`8412f5a967e641d12d29d3b5810169ae46cec443e34d64d8c36966fa67064617`.
The reviewed overlay decodes all 56 instructions with two returns and complete
branch reachability. There are no calls, external data operands, or MZ
relocations in this contribution.

Restunts `math.c:382` offers the four in-place rectangle clamps and five early
tests, but its first condition reads `r1->right <= r1->left`. The pristine first
group is `cmp [bx+2],ax; jge continuation; mov ax,1; retf`, so equality
continues and only `right < left` returns 1. The other four early comparisons
agree with that C at the machine level. The candidate changes only the first
condition from this source hint.

Prediction before the production compiler call: under pinned MSC 5.10 medium,
the corrected C will emit the 132-byte two-return body, with the first `jge`
branch at relative `0x0E`, the shared return-one block at `0x10`, four
conditional in-place clamps, and `sub ax,ax; pop si; pop bp; retf` at the end.
Falsifier: any different complete extent, different first branch/return
topology, register/argument allocation, or unresolved object contribution.
The production grinder budget is three source attempts; this is the first.

The first fresh `grind.py` attempt returned `FAST_PASS_ONLY`: the full
132-byte no-fixup contribution matched. Serial promotion freshly recompiled
the source and verified both staged and canonical complete images as
`HYBRID_EXACT`. The accepted source is `src/rect_intersect.c`; receipts are
`recovery/attempts/rect_intersect/0001/report.json`,
`recovery/attempts/rect_intersect/0002/report.json`, and
`recovery/promotions/rect_intersect.json`. This proves exact contribution
ownership, not unique historical C spelling.
