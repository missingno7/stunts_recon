# Current generated status

Full image: **HYBRID_EXACT**. Matching C: **252 bytes**; raw initialized: **199023 bytes**.

Mapped 235 / 619 procedures; 38 source code segments and 25 proven segment frames.

Classification counts are conservative evidence counts, not a complete code/data partition. See status.json for unknown bytes.

Queue: CHEAP=0, MEDIUM=9, SUPERVISOR=599.

Compiler: MSC5.0/5.1 medium-model optimized, stack checking off; production pins MSC5.1; unique version/flags not proven.

Blockers:

- Only external DGROUP offset16 binding is supported; far/self-relative linking and complete TU layout remain open
- QuickC BAKPAT rejected; full compiler/version fingerprint remains open
- Archived CRT startup checksum anomaly and general runtime linking remain unresolved
- Unmapped functions and internal code/data ambiguity remain
