# Pinned runtime objects with ordered LEDATA

All twelve identified runtime contributions are now production-bound: **725 bytes**. This includes the previously blocked far long-arithmetic members of the pinned MSC5.10 `LIBH.LIB`:

| Module | Load image start | Complete bytes | Publics |
|---|---:|---:|---|
| ldiv.asm | 0x1E83C | 156 | `__aFldiv` |
| lmul.asm | 0x1E8D8 | 52 | `__aFlmul`, `__aFulmul` (same offset) |
| uldiv.asm | 0x1E9A6 | 97 | `__aFuldiv` |

These archive members intentionally contain overlapping LEDATA records. `ldiv` overwrites byte153, `uldiv` overwrites byte94, and `lmul` overwrites bytes24 and49. Each later record replaces a one-byte RETF with RETF 8 and contributes the additional operand bytes. The original archive member is passed unchanged to LINK and to the parser.

## Independent LINK evidence

`tools/runtime_link_probe.py` extracts the exact module by archive/member hashes, links it under `C:/tools/msdos/msdos.exe`, and checks the complete `_TEXT` segment from LINK's map. Both bundled historical linkers (MSC5.00 and MSC5.10 distributions) reproduce the expected bytes, public addresses and `lmul` alias, with no MZ relocations. Each module is tested separately and all three together: eight experiments, recorded in `recovery/runtime-link-proof.json`. The combined segment is 305 bytes. No arbitrary window inside LINK output is accepted as a complete contribution.

This establishes these specific objects' ordered-write semantics. It does not establish the original game's unique linker version or authorize arbitrary overlapping objects.

## Production gate

Default `read_object` still rejects every overlapping LEDATA record. Only a library owner with `pinned-ordered-ledata-v1` policy can opt in. The policy locks the entire module SHA-256 and every LEDATA record's file offset, segment index, destination offset, payload size/hash, and exact overlap offsets in record order. Production never generates a policy from an observed input. Reviewed candidate policies are in `layout/library-candidates.json`; active owners carry their policies in the manifest.

All existing checks remain: record framing/checksums, supported record kinds, full SEGDEF coverage, no holes/overflow, no BIG/32-bit storage, exact publics/externals, and no other initialized/BSS contribution. Overlapping objects containing any linker fixup are rejected. The complete resolved library segment must match its entire oracle extent and have no intersecting MZ relocation. The archived CRT startup checksum anomaly remains rejected; no checksum exception was added.

`tools/check_library.py ID --promote` reopens and validates the pinned archive for FAST, staged full-image construction and canonical full-image construction under the common serial promotion lock. Library and C promotions derive the exact expected canonical fingerprint before writing metadata, reject unrelated concurrent edits, and roll back metadata on failure. JSON serialization is explicit UTF-8 with LF so predicted fingerprints match the actual writes on every host. Failure tests exercise both canonical-write interference and canonical-build rollback.

## Remaining work

Library provenance no longer leaves any of the currently identified 725 bytes raw-owned. This does not imply all runtime code has been identified. Startup, general runtime fixups, original segment/TU layout and unknown origins remain open. Keep runtime ownership separate from recovered game C and matching handwritten ASM.
