# Broader ordinary-C research cohort screening

This is a **research-readiness** review, not original object/TU proof or promotion eligibility. The baseline at selection was 26 strict game-C functions / 1,118 bytes; 0 CHEAP, 7 MEDIUM, 576 SUPERVISOR. The current blocker census has 349 SUPERVISOR tasks lacking complete card instruction evidence and 59 whose card has only that capability blocker. Those 59 are not assumed otherwise ready because `tools/triage.py` returns before examining instruction-dependent obligations.

All four original suggestions had verified mapped coordinate intervals and no finding from `audit_function_extents.py` single-entry traversal. That audit cannot prove the absence of external ingress. The complete research disassembly in `recovery/blocker-census.json` and the original assembly were inspected for ABI, flow, calls, relocations and data. Restunts `c_sources` locations were checked for a definition rather than assumed to be one.

| Task | Research disposition | Evidence and limitations |
|---|---|---|
| `rect_is_adjacent` (`load_169d0`, 130 B) | **Selected** | Pristine 57-instruction interval has near rectangle pointers at BP+6/+8, far word return, direct internal branches, no calls/MZ relocations/globals. Restunts `math.c:447` supplies a semantic definition, but its second adjacency disjunct repeats `r2->right == r2->left`, contrary to the pristine left/right branch; source is not historical authority. Original `seg006.asm:3366–3439` has a named far procedure. Complete object/TU ownership remains unknown. |
| `mat_multiply` (`load_229f2`, 128 B) | **Selected** | Pristine 60-instruction interval has three near matrix pointers at BP+6/+8/+10, far return, direct internal branches/LOOP, no calls/MZ relocations/globals. Target uses signed 16×16 IMUL and shifts DX:AX left twice. Restunts `math.c:183` is a semantic definition but its `(long)` multiply may select a runtime helper; original `seg012.asm:9120–9192` has a named far procedure. |
| `init_rect_arrays` (`load_0a096`, 94 B) | **Rejected for this pilot** | Pristine code copies four-word blocks between multiple absolute/indexed DGROUP locations. The current `restunts.c` hit is a caller, not a definition. The data blocks and their original ownership/extent are not established; the missing instruction packet hides those dependencies. |
| `file_decomp_rle_seq` (`load_20ccf`, 170 B) | **Rejected for this pilot** | `seg012.asm` labels it a *near* procedure inside the decompression flow. Its first instruction reads BP-18 from an already-live frame and it uses near RETs. This is not an independent far C function contribution to test in isolation; original extent/TU and caller frame need resolution. |
| `heapsort_by_order` (`load_26be8`, 150 B) | **Selected replacement** | Pristine 66-instruction named far procedure has three near arguments (count, heap, data), signed array operations, direct branches, no calls/MZ relocations/globals. Restunts `heapsort.c` explicitly says its implementation was adapted in 2014 and calls `heapify`; the target has no calls. Use target semantics and treat that C as a lead only. |
| `file_get_shape2d` (`load_2265b`, 95 B) | **Selected replacement** | Existing reviewed pristine entry/extent/CFG overlay covers all 44 instructions, far resource pointer at BP+6/+8, signed index BP+10 and normalized far DX:AX result. No calls, branches, data globals or MZ relocations. Existing production interval blocker remains. One production candidate (114 B, `__AHSHIFT` fixup), two private explicit-linear candidates (138/146 B) and four subsequent version-profile file-only recompiles are preparation history, not a three-try proof of source exhaustion. |
| `copy_paras_reverse` (`load_211d5`, 83 B) | **Considered, rejected** | Pristine STD/CLD, segment manipulation and REP MOVSW make assembly/intrinsic origin more plausible than an ordinary C pilot. |

The initial four selected target intervals total 503 bytes; the later
`mat_mul_vector` follow-on adds 260 bytes. The new tasks do not need production
recipes or reviewed overlays to enter the isolated `research_batch.py` lane:
it validates each current card and hashes the pristine interval itself. This
distinction leaves the default queue and promotion gates unchanged. A read-only
focused packet utility in this cohort captures the instruction-dependent
blockers without entering workflow-hashed `tools/` during active batches.

Preparation accounting before new batches: one full repository validation; nine original/replacement candidate context reads and single-entry audits; complete research disassembly/Restunts definition review for the selected and rejected cases; prior `file_get_shape2d` history reconstructed as 3 source hypotheses/3 compiler processes before the new profile study, plus four file-only profile-study compiler processes. The current cohort's 256-process ceiling begins at zero and is tracked separately in `ledger.json`. Supervisor and worker token/billing metadata are unavailable from these local receipts; do not infer a numeric model-weighted cost or causal speedup without that usage evidence.

After the first recovery, the focused packet screened 24 further 81–200-byte
SUPERVISOR cards whose only *old card* capability label was missing instruction
evidence. Twenty-two exposed calls, unregistered global/data addresses,
CS-relative storage, hardware instructions or external flow once decoded.
`kb_reg_callback` (81 B) and `sub_37470` (110 B) had no hard triage blocker but
retain indexed-data/source ownership risks; the former also has an odd extent.
Neither has an independently established ordinary-C semantic definition in this
review. They are not replacements on packet output alone. This is a bounded
screen, not a complete audit of all 59 cards or a claim of historical origin.

The same packet then screened 17 cards above 220 bytes from that old-label
population. Four had no newly exposed triage blocker:
`mat_mul_vector` (260 B), `subst_hillroad_track` (287 B, odd extent),
`carState_rc_op` (444 B), and `init_carstate_from_simd` (618 B). The other
13 exposed calls, globals, hardware operations, indirect/outside flow or
CS-relative storage. `mat_mul_vector` had the smallest complete clean interval
and an independently located semantic definition in Restunts `math.c:137`.
The original named far procedure, 122/122 reachable pristine instructions,
no calls/relocations/globals/inside entries, and a finding-free single-entry
extent audit support isolated research. Its Restunts C is not historical source.
The foreman recorded a 16-process follow-on allocation from unused
`mat_multiply` capacity before any `mat_mul_vector` batch.

A later complete pass over the **58 remaining** SUPERVISOR cards with exactly
that old missing-packet label found six with no packet-level blockers or risks:
`mat_multiply`, `heapsort_by_order`, `file_decomp_rle_seq`, `mat_mul_vector`,
`carState_rc_op`, and `init_carstate_from_simd`. Four more had no hard blocker
but indexed-data or odd-extent risks. The other 48 exposed hard obligations.
The clean packet result for `file_decomp_rle_seq` is a useful negative control:
its enclosing BP frame/near-entry issue required original ASM review beyond
local CFG. `carState_rc_op` (444 B) has only a Restunts caller here, leaving
semantic source uncertain. `init_carstate_from_simd` (618 B) has a named far
procedure, a finding-free extent audit, 225 reachable instructions plus one
NOP, no calls/relocations/globals, and a Restunts `restunts.c:371` semantic
definition dominated by field initialization. A 24-process research share was
recorded from unused rectangle capacity before any batch for it. The source
is a lead, not historical authority or proof of its original TU.
