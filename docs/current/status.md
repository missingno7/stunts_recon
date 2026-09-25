# Current generated status

Full image: **HYBRID_EXACT**. Matching C: **1248 bytes**; raw initialized: **198027 bytes**.

Mapped 531 / 619 procedures; 38 source code segments and 36 proven segment frames.

Classification counts are conservative evidence counts, not a complete code/data partition. See status.json for unknown bytes.

Queue: CHEAP=0, MEDIUM=7, SUPERVISOR=575.

Compiler: MSC5.0/5.1 medium-model optimized, stack checking off; production pins MSC5.1; unique version/flags not proven.

Blockers:

- Supported: external DGROUP offset16, reviewed zero-addend external far CALLs, and the exact callback-pointer object mode; general/self-relative linking and multi-public production remain open
- QuickC BAKPAT rejected; full compiler/version fingerprint remains open
- Archived CRT startup checksum anomaly and general runtime linking remain unresolved
- Unmapped functions and internal code/data ambiguity remain
