# `rect_adjust_from_point` register allocation probes

Authority: bounded, isolated supervisor research on the existing interval
blocker. The pristine `[0x1637A,0x163C6)` contribution is 76 bytes, SHA-256
`7a66ad05b0867d74d0feb0da20b8c216595645babc1f7607739adc239af8e16a`.
No production attempt, recipe, or manifest was changed by these probes.

Prior `endurance-001` trials compiled the Restunts-style source to 84 bytes,
then ordinary cached `px`/`py` locals to 86 bytes. The latter used BP locals
instead of the pristine SI/DI coordinate roles. Each new probe had a frozen
machine-level prediction, falsifier, and one-compiler-process ceiling before
compilation. One process ran per probe under pinned `msc510-medium`.

| Source | Effective output | Result |
|---|---|---|
| `rect-adjust-register-20260923.c` | 76 bytes, SHA-256 `73cce6806ee80cfb2f36b85b667b91fc76afad83e7ea63d64c9822615c93db85` | Explicit register coordinates selected SI/DI and preserved the target 6-byte frame, four conditional stores, and all 33 instruction positions. Only two bytes differ: the two temp-store displacements at relative `0x1E` and `0x3A` are `FE` (BP-2), while pristine is `FA` (BP-6). No fixups. |
| `rect-adjust-temp-first-20260923.c` | Same 76-byte SHA-256 and effective output | Moving the temp declaration before both register coordinates changed no machine output. It falsified a simple declaration-order repair for this source/profile. |

The automatic diagnosis groups the two differences as one consistent
BP-2→BP-6 operand pattern over the complete 33-instruction scope, with 31
byte-exact instructions. This is a machine observation. It does not establish
which original source declaration, allocator state, or TU context produced
BP-6. A mere stack-slot patch or deliberately unused storage would not be an
accepted C recovery. The function remains blocked until a source or
compiler-context explanation predicts and produces the complete 76 bytes.

Detailed frozen plans, original objects, compiler logs, receipts, and full
diagnostics are local under
`build/private/research-plans/rect-adjust-register-20260923/` and
`build/private/research-plans/rect-adjust-temp-first-20260923/`.
