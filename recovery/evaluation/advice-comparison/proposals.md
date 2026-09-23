# Isolated proposal transcripts

Verbatim task-complete responses from eight fresh GPT-6 Astra / medium contexts. Conditions, identical work allowance and packet hashes are in design.json. No source compilations were permitted.

## camera X

Abstain from a source experiment: the packet explicitly blocks further attempts without independent new evidence, and the supplied diagnosis/history does not establish such evidence.

The motivating observations are two distinct discrepancies: exchanged 32-bit BP-relative local slots and a return sequence using `AL` plus `CBW`. The target’s `+0xB0..+0xB9` contains `mov al,1`, a jump, two NOPs, `sub al,al`, and `cbw`; the candidate materializes the result in `AX`. A consistent slot exchange does not establish declaration order as its cause.

Checked historical alternatives include a `char` temporary and `register char` temporary (both 200 bytes with the same non-target digest), a `char` function return (unchanged 194-byte candidate), a cast of the Boolean result (198 bytes), Boolean/ternary variants, renamed locals, separate declarations, register hints, and compiler/optimization changes. Consequently, suggesting a char temporary again would repeat existing evidence; matching the 200-byte extent alone would prove nothing.

No new machine-level change is justified for compilation, so no source-experiment prediction or falsifier is asserted. In particular, the char-temporary history does not show that its emitted instructions fix the target’s return sequence or preserve the anchors. The two unresolved `__aFlmul` pointer fixups also remain binding obligations.

Additional reads: `history.json` twice and `full.json` twice—**4/4** permitted reads. The initial combined output was truncated; rereads inspected compact history fields and the full-diagnostic schema. No edits or compilation performed.

## camera Y

Abstain from a source experiment: the packet explicitly blocks attempts without independent new evidence, and none was established.

The motivating differences are an 11-site exchange of two 32-bit BP slots and the target’s byte-valued result sequence (`mov al,1`, `sub al,al`, `cbw`) versus the candidate’s AX result. The consistent slot mapping identifies `dx1`/`dy0` storage differences; it does not establish which source construct caused them.

History already tests char/register-char temporaries (200 bytes, identical hashes), a char return (unchanged 194 bytes), result casting (198 bytes), boolean/conditional forms, register hints, renaming, declaration splitting, long temporaries, and compiler/optimization alternatives. Equal extent from the char temporary is insufficient evidence of instruction agreement. Two `__aFlmul` fixups also remain unresolved.

No compilation is proposed, so machine-change prediction and falsifier are inapplicable. Further authorization would require independent evidence explaining the slot allocation or distinguishing the archived char-temporary output at instruction level.

Additional reads: `history.json` twice and `full.json` twice—4 total. No edits or compilation.

## rectangle X

Propose one conditional source experiment: introduce `register struct POINT2D *q = p;` before `char flag`, replacing each `p->` access with `q->`; leave types, conditions and return unchanged. Execute only after current queue/budget authorization.

- **Observation:** Target consistently uses SI for the near pointer, saves/restores SI, allocates four frame bytes and accesses the byte flag at `[bp-4]`. Candidate uses BX, two frame bytes and `[bp-2]`. This supports testing an explicit register-local pointer with a retained frame slot; it does not establish that source cause.
- **Prediction before compilation:** `push si; mov si,[bp+6]` replaces the BX setup; all four coordinate comparisons address SI; `pop si` appears before frame teardown. The stronger hypothesis predicts `sub sp,4` and flag accesses at `[bp-4]`. The added initial byte should consume the candidate NOP at `+0x17`, preserving realignment at `+0x18` and the exact anchor `[+0x35,+0x3B)`; SI restoration should consume terminal padding.
- **Falsifier:** Continued BX addressing falsifies the register-selection explanation. SI selection with a two-byte frame and `[bp-2]` falsifies the stronger retained-slot explanation, even if similarity improves.
- **Evidence checked:** Full instruction streams explain the extra/missing instructions as SI preservation versus padding. Comparisons remain signed 16-bit; flag operations remain byte-width and return uses `cbw` in both. Bound clip addresses agree with the four external fixups; no normalization excuses a mismatch. `history.json` is empty, so there is no archived experiment supporting this hypothesis.
- **Additional reads:** `full.json` twice, `history.json` once: **3/4**. No edits or compilation performed; any eventual acceptance still requires fresh whole-image promotion.

## rectangle Y

Propose one conditional source experiment: change only the parameter to `register struct POINT2D *p`, retaining `char flag`, control flow, and pinned compiler flags. Execution requires current queue/budget authorization.

- **Observation:** Target keeps the near pointer in SI, saves/restores SI, reserves four local bytes, and accesses the byte flag at `[bp-4]`; candidate uses BX, reserves two bytes, and accesses `[bp-2]`. Explicit register allocation could explain this coupled allocation pattern; the operand grouping alone does not establish it.
- **Prediction before compilation:** Pointer load and four comparisons switch BX→SI; `push si`/`pop si` appear. If register allocation also explains the frame, `sub sp,2` becomes `sub sp,4` and all six flag references move to `[bp-4]`, retaining byte width and `cbw`. The inserted prologue byte should absorb the candidate NOP at offset 23, changing the early branch encodings to target values while preserving destinations 24 and 42. The epilogue restore should absorb the terminal padding NOP. Preserve exact anchors `[0,3)` and `[53,59)` and the four bound `_clip` addresses/addends.
- **Falsifier:** Unchanged BX allocation refutes the register-parameter explanation. SI allocation with unchanged frame/flag displacement refutes its proposed explanation of the *coupled* mismatch, even if it improves matching.
- **Checked evidence:** Both supported families have zero recorded contradictions; frame accesses are byte locals, pointer/comparison operands are words, globals are bound, and the residual save/restore/padding differences are compatible with this prediction. `history.json` is empty, so no prior supporting or failed experiment is established. No strict acceptance claim follows from similarity or this proposal.
- **Additional reads:** `full.json` once and `history.json` once: **2/4**. No compilation or edits.

## pointer X

Propose one source experiment, conditional on current queue/budget authorization: replace the four-bit shift with four separate one-bit shifts of an `unsigned long` temporary:

```c
unsigned long value = (unsigned long)segment;
value <<= 1;
value <<= 1;
value <<= 1;
value <<= 1;
return value + offset;
```

- **Observation:** The target contains four consecutive `shl ax,1; rcl dx,1` pairs; the candidate implements the same shift through `cl=4`, one pair, `dec cl`, and `jne`.
- **Prediction:** Separate statements will produce four inline shift pairs, removing the counter initialization, decrement, and branch. With register retention, the candidate becomes 32 bytes and the unchanged `add/adc/pop/retf` suffix begins at `+0x18`. Expect `sub dx,dx` may remain; this experiment does not explain the target’s `xor dx,dx`.
- **Falsifier:** Retention of the counted loop—or substitution of spills, helper calls, or different arithmetic instead of four inline pairs—falsifies this specific lowering prediction.
- **Evidence checked:** Full instruction streams, operand widths, exact prefix/suffix, and absence of fixups support a local shift-lowering experiment rather than a binding repair. History is empty; no prior hypotheses or blockers are recorded. Exact acceptance remains unproven.
- **Additional reads:** `full.json` and `history.json`, once each: **2 additional file reads**. No execution or edits performed.

## pointer Y

Propose one conditional source experiment: replace the return expression with:

```c
unsigned long value = (unsigned long)segment;
value <<= 1;
value <<= 1;
value <<= 1;
value <<= 1;
return value + offset;
```

Motivation: the target has four consecutive `shl ax,1` / `rcl dx,1` pairs; the candidate implements the same 32-bit shift through a CL-counted loop. Separate one-bit operations may select straight-line code generation.

Prediction before compilation: remove `mov cl,4`, `dec cl`, and `jne`; emit four shift/rotate pairs without spills. Preserve the six-byte prefix and eight-byte addition/return suffix, moving the suffix to offset 24 and producing 32 bytes. This hypothesis does **not** explain the independent `sub dx,dx` versus `xor dx,dx` difference; predict that `sub` may remain.

Falsifier: persistence of the counted loop, or spills/other arithmetic replacing the predicted four pairs, rejects this source-shape explanation. A remaining zeroing-opcode mismatch still prevents exact acceptance.

Checked complete instruction streams, operand widths, exact anchors, empty fixup lists, and empty history. No prior experimental support or explicit interval blocker appears in the packet. Execution remains conditional on fresh queue/budget authorization; nothing was compiled or edited.

Additional reads: `full.json` once and `history.json` once — **2/4** allowed.

## run_count X

Abstain from a source experiment: the 46-byte candidate exactly matches all 18 target instructions, with zero mismatch islands and no fixup normalization. No source-level mismatch hypothesis is justified.

The next validation, subject to current queue/budget authorization, is fresh whole-image acceptance with unchanged source and the pinned compiler profile. Prediction: the function remains exactly 46 bytes and byte-identical throughout `+0x0..+0x2E`. Any differing byte or extent falsifies reproducibility of this local match; a whole-image failure would invalidate promotion even if this function remains exact.

Checked source, identical target/candidate hashes, complete byte anchor, empty fixup list, `FAST_PASS_ONLY`, and reported history count of zero. Additional files read: none (0/4).

## run_count Y

Abstain from a source experiment. Target and candidate are identical 46-byte streams with matching SHA-256; diagnosis reports all 18 instructions preserved, zero mismatch islands, zero residuals, and no fixups.

No source-codegen hypothesis warrants a predicted machine-level change: any changed payload would break the existing exact match. The checked source, identities, diagnosis, and reported history count of zero provide no unresolved source difference to test or falsify.

The remaining question is whole-image acceptance. Subject to current queue authorization, fresh full-image validation should preserve these 46 bytes and satisfy extent/binding obligations. Any byte, extent, or binding failure would falsify promotion readiness, despite the local match. `FAST_PASS_ONLY` does not establish acceptance.

Additional files read: none; additional read count: 0.
