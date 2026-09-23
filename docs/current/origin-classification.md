# Origin and byte classification

Source representation and historical origin are different facts. Restunts asmorig contains disassembly of compiler output, runtime code, possible handwritten assembler, data and later script edits. A difficulty reproducing an opcode is not evidence of assembly authorship. A C match proves a source representation, not the unique original source or compiler.

`layout/manifest.json` is the authority for active source/library/raw ownership. Generated `docs/current/status.json` reports matching C, matching ASM, identified and bound runtime bytes, raw initialized bytes and the conservative confirmed raw-executable minimum. Do not copy current recovery totals into hand-maintained documents or tests.

The classification layer distinguishes checked code outside identified libraries, initialized DGROUP evidence, identified runtime and unknown bytes. Its disjoint totals cover the 200000-byte initialized image. Unknown bytes are not automatically data. BSS symbol coordinates beyond that image are logical addresses and add no initialized ownership.

The exact total of unresolved executable bytes remains unknown until all code/data boundaries are proven. This differs from the exact raw initialized-byte count. Identifying a library does not itself promote it; active binding must independently satisfy its complete object and oracle obligations. The pinned overlapping-record helpers are described in `library-binding.md`.

Mapped-function totals combine fully anchored importer extents and independently accepted C contributions. Imported segment names/counts are Restunts source structure, not proven original translation units. Source provenance, mapping confidence and readiness are separate fields; queue capability blockers do not classify historical origin.

See `layout/classification.json`, `recovery/restunts-inventory.json`, `recovery/library-evidence.json` and generated status. Future matching-ASM claims require independent compiler/library, hardware or authorship evidence.
