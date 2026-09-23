# Compiler fingerprint

Production pins Microsoft C 5.10 with `/c /AM /O /Gs`, invoked by the hash-pinned ReC98 MS-DOS Player. Complete Microsoft C 5.00 and 5.10 distributions and their bundled QuickC 1.00/1.01 are available under ignored `toolchain/`. Every extracted executable, header, library and startup object is SHA-256 pinned in `layout/toolchain.json`.

The larger canary pass is recorded in `workflow-canary.md`. Both MSC versions emit the same 194-byte strongest canary hypothesis, which does not match its 200-byte pristine extent. Explicit `/Ox`, `/Od`, `/Or`, `/Os` experiments and the adjacent-function TU probe did not establish a matching profile. Production flags remain unchanged; unique version/flags remain unproven. Two-call historical LINK fixtures and same-TU near-call fixtures agree under both linkers.

The following fixed examples establish compiler-family compatibility; current production ownership is generated in `status.json`:

| Function | Load offset | Owned bytes | Construct |
|---|---:|---:|---|
| rect_is_overlapping | 0x1695E | 62 | signed rectangle comparisons, near pointers, multiple returns |
| rect_is_inside | 0x1699C | 52 | short-circuit boolean conjunction; includes one emitted alignment NOP |
| audioresource_get_dword | 0x284FA | 26 | explicit far pointer, 32-bit return, local variable stores |
| nopsub_378AE | 0x278AE | 14 | unsigned byte-array load through DGROUP |
| nopsub_378BC | 0x278BC | 14 | unsigned byte-array load through DGROUP |
| audio_enable_flag2 | 0x273B2 | 6 | byte store through DGROUP |
| nopsub_26552 | 0x16552 | 32 | signed-long absolute value with conditional returns |

These seven examples were promoted through FAST and fresh whole-image acceptance. MSC 5.00 produces identical complete bound contributions for these seven examples; `/Ox` also matches the initial three fixup-free functions. This establishes compatible compiler-family code generation, not a uniquely proven version or optimization profile. The Restunts FAQ states Microsoft C 5.10 and its runtime signatures support that family, but neither overrides the ambiguity in experiments.

Far function returns and near default rectangle pointers support a medium-model hypothesis. An explicitly qualified source in another memory model could reproduce the same ABI. Frame-pointer behavior, register use, signed comparisons and the far data getter are demonstrated locally; global signed-char behavior, switch generation, structure copying, original TU grouping, linker version and assembler choice remain unproven.

The initial RPM C hypothesis emits `__aFulmul`, whereas the oracle uses direct MUL and a different frame/register arrangement. A sign-function hypothesis failed across 28 MSC/QuickC profile combinations. These are archived blockers, not evidence to relax the oracle. QuickC emits BAKPAT records that the permissive inherited reader would ignore; production now rejects those objects.

MS-DOS Player and independent hidden DOSBox-X produced equal MSC5.10 code/fixups for a bounded sign fixture. Independent DOSBox-X checks compile all active manifest C functions and compare their complete bytes and binding obligations (`recovery/promoted-runner-parity.json`). These are bounded runner checks, not a blanket emulator-correctness claim. UNP required DOSBox; MS-DOS Player's zero exit status on its failed run was insufficient.

Runtime inventory identifies 725 normalized exact bytes. Production library ownership is generated in `status.json` rather than inferred from identification receipts. Exact record policies support `ldiv`, `lmul`, `uldiv` (305 bytes), verified unchanged against both historical linkers individually and together. The recovered 32-byte long-absolute-value routine differs from the pinned archive's `labs.c` implementation; it is not claimed as an identified library member. Archived startup `crt0` also has a checksum anomaly and is not accepted. The range-based `toupper` helper matches freshly compiled C but is not the archived library's ctype-table implementation; it has not been promoted or misclassified as a proven library module.

See `toolchain-fingerprint.json`, `recovery/*-experiments.json`, `recovery/runner-parity.json`, and `recovery/library-evidence.json` for exact experiment evidence.

The two rectangle functions also compile together into the exact contiguous 114-byte contribution, with publics at offsets 0 and 62. See `recovery/tu/rectangle-pair.json`; full original TU grouping remains unproven.
