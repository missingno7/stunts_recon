# Batch 001 worker summary

## Completed functions

- `audioresource_copy_n_bytes` (`load_29c84`, 70 bytes): 3 charged failures, no promotion. Outputs were 36 bytes (ordinary loop), 70 bytes with wrong pointer/register shape (local far-pointer cursors), and 38 bytes (register far-pointer parameters). Target uses DS:SI and ES:DI, preserves SI/DI, writes both full far pointers back, and uses LOOP. Diagnosis unavailable. Reports: `recovery/attempts/audioresource_copy_n_bytes/0001..0003/report.json`.
- `file_get_res_shape_count` (`load_2264a`, 17 bytes): 3 charged failures, no promotion. The exact associated C expression emitted 12 bytes via LES BX; a register far-pointer local spilled and emitted 30 bytes; typed far parameter repeated the 12-byte output. Nine complete instructions form the odd-length extent; odd size alone does not establish assembly origin. Reports: `recovery/attempts/file_get_res_shape_count/0001..0003/report.json`.
- `unknown_libname_1` (`load_1e066`, 18 bytes): 3 charged failures, no promotion. The direct near-pointer body had exact size but used CMP then delayed BX load. Register local and register parameter selected SI and required save/restore; target uses BX. Unknown IDA name does not establish purpose. A register-pressure explanation is unsupported and untested; it is not a reopen recommendation. Reports: `recovery/attempts/unknown_libname_1/0001..0003/report.json`.
- `toupper` (`load_270ba`, 24 bytes): one hypothesis compile produced FAST; `grind.py promote` reported PROMOTED after strict fresh whole-image checks. Inclusive `[a,z]` with signed int parameter/return matches every target byte. Associated repository source uses `< 'z'`; provenance remains unresolved. Reports: `recovery/attempts/toupper/0001..0002/report.json`.
- `mmgr_copy_paras` (`load_2118d`, 72 bytes): one charged attempt was blocked before the compiler because `#include <dos.h>` hit unsupported preprocessor closure. Queue marked it SUPERVISOR; skipped. Report: `recovery/attempts/mmgr_copy_paras/0001/report.json`.
- `sub_35DE6` (`load_25de6`, 34 bytes): skipped with no candidate or compiler invocation after finding target LEA base 0x72a8 corresponds to public assembly label `incnums`, absent from reviewed `layout/data-symbols.json`. A C global reference would require unsupported external data binding.
- `file_load_shape2d_palmap_init` (`load_2acb0`, 39 bytes): skipped with no candidate or compiler invocation. Associated source references extern `palmap`; target stores to indexed `palmap[bx]`, but `palmap` is absent from reviewed data symbols and candidate relocations are not supported by the current no-fixup recipe.

## Run accounting

- Initial routine tasks processed or skipped: 7 of 7.
- Explicit reasoning rounds: 13 (rounds 1–10 compiler hypotheses; round 11 mmgr precompiler blocker; rounds 12–13 symbol-binding reviews).
- Hypothesis compiler experiments: 10. The mmgr blocker was charged by workflow but invoked no compiler. The two symbol-binding skips invoked no compiler.
- Promotions: 1 (`toupper`). Promotion compiles/checks tracked separately.
- Infrastructure events: 2 post-archive `refresh()` calls raised WinError 5 while replacing unrelated cards during `audioresource_copy_n_bytes`; reports remained durable. A standalone refresh succeeded before work continued. No compiler rerun was used to refresh.
- Root supplied initial validation; no baseline or final full validation was rerun.
- Source candidate files modified: `recovery/candidates/audioresource_copy_n_bytes.c`, `recovery/candidates/file_get_res_shape_count.c`, `recovery/candidates/unknown_libname_1.c`, `recovery/candidates/toupper.c`, and `recovery/candidates/mmgr_copy_paras.c`. `toupper` was promoted; the others remain experimental or blocked.

Detailed hypothesis and skip evidence with round IDs and analysis levels are in `new-attempts.jsonl`.
