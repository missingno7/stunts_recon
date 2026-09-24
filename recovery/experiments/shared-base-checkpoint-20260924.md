# Shared-base checkpoint, 2026-09-24

## Question and evidence

The shared-base finding in `D:/Prog/ai_decomp/research/breakthrough/stunts-topology.md` was obtained on an older, changing Stunts worktree. I re-applied its equations to the current native inventory and candidate OMF census. `tools/shared_base_probe.py` now runs after the candidate census in `reconstruction_factory.py reclassify`. Its output, `recovery/shared-base-census.json`, is research only and pins every input hash. No compiler or linker process was needed for this checkpoint.

A public in candidate segment S at offset O and an independently verified original entry at address A implies `base(S) = A - O`. Multiple publics in S must agree on the base and frame. The tool reports a minimal conflicting pair, a common base, or insufficient evidence. It never combines CODE and DATA segment bases. It reports candidate span and complete anchored function span separately from placement. It does not bind fixups or infer original TU membership.

## Current corpus

The current 64 distinct candidate objects contain three multi-public objects and two objects with a 33-byte initialized private `_DATA` contribution.

| Candidate | Placement result | Separate proof gap |
| --- | --- | --- |
| `update_rpm_from_speed`, two publics | Only one public has an independently mapped entry; insufficient anchors | 170-byte candidate versus 34-byte task; other public/owner unresolved |
| `file_load_shape2d_nofatal`, trial 001 | Contradictory implied CODE bases, 24 bytes apart | Layout rejected for these two entry constraints |
| `file_load_shape2d_nofatal`, trial 002 | Common CODE base 174570, both entries in one frame | Candidate span `[174570, 174594)` is 24 bytes; anchored two-function span `[174570, 175280)` is 710 bytes. Its self-relative offset16 fixup and full linker obligations remain unaccepted. |

Thus the previous two-public conclusion survives on current native evidence, but the placement-consistent object still omits 686 bytes. This is no recovery promotion.

## One discriminating private-DATA experiment

The two `mmgr_get_chunk_size` candidates each emit the same 33-byte private `_DATA` string. A separately mapped `mmgr_free` instruction at load `0x2149B` is `B8 83 47` (`mov ax, 0x4783`), with no MZ relocation in its operand. The independently anchored DGROUP load base is 178032, placing the referenced original string at load 196339. Restunts calls that string `aMemoryManagerB`; this semantic association is a hypothesis, while the instruction bytes and DGROUP base are original-image evidence. The explicit anchor is recorded in `recovery/shared-base-anchors.json`.

At that proposed DATA placement, each candidate string matches the original for 32 bytes. Candidate byte 32 is NUL (`00`); original byte 32 is a space (`20`) because the original message continues. The complete private-data contribution therefore differs under the proposed association. Only one independent DATA reference is available, so the shared DATA base itself remains underconstrained. This comparison does not prove which original TU owned the string.

## Decision

The experiment changes the **local candidate decision**: neither archived `mmgr_get_chunk_size` private-DATA object should be sent to linker/binder work as a placement-only repair. A new source candidate must account for the full original message and the shared failure tail, then be judged on complete code/data extents and ordered fixups. For `file_load_shape2d_nofatal`, the next meaningful test remains a complete two-public contribution, not another placement check on the 24-byte object.

The **project-level next recovery decision is unchanged**. The current results are diagnostics, not a measured recovery speedup: no strict C bytes, queue eligibility, native linker support, or whole-image acceptance changed. Placement consistency is necessary in these proposals and insufficient for strict recovery. `tools/validate.py` remains the acceptance gate.

Verification: `tools/validate.py` passed 173 tests, independent recompilation of the 25 active C functions, `HYBRID_EXACT`, and current queue/card checks.
