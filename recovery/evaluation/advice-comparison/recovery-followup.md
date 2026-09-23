# Bounded recovery follow-up

## nopsub_326BA, attempt 1

**Prediction before compile:** A far pointer to a record containing six header bytes followed by `unsigned long` entries should load ES:SI from the first argument, scale the word index by four, read the entry at base + 6 + index*4, then store its two words through the near output pointer. The compiler should preserve SI and end with `retf`.

**Falsifier:** Any output with a different stride or base offset, incorrect parameter loads, added calls/fixups, or a contribution length that does not match the 36-byte interval disproves this source model for the pinned profile.

**Outcome:** `FAILED` (34 bytes versus the required 36). The candidate used `LES BX,[BP+6]`, held the scaled index in `SI`, and used `BX` for the output pointer. It preserved the far indexed read and output stores, but did not reproduce the reference register/stack sequence.

**Diagnostic observation affecting next experiment:** The compact islands identify early far-pointer materialization as the key ordering difference: the indexed struct access computes index in `SI` before `LES BX`. The next experiment expresses the byte address explicitly as `source + 6 + index*4`, then performs one far `unsigned long` load, to test whether that source expression changes pointer materialization and index registers.

**Family view versus ordinary islands:** The `REGISTER_ROLE` family grouped the two islands but reported contradictory SI/BX mappings across pointer, index and output roles. It added no actionable distinction beyond the ordinary islands.

## nopsub_326BA, attempt 2

**Prediction before compile:** Casting the explicit byte offset `6 + index*4` to a far `unsigned long` pointer should let the compiler form the reference's `ES:[SI+BX+6]` indexed access from a far byte base and the scaled index. It may still choose `LES`; a candidate with `LES` and 34 bytes or any other changed address/return behavior falsifies this register-allocation hypothesis.

**Falsifier:** Any differing stride, offset, parameter load, or nonmatching 36-byte contribution disproves this source form for the pinned profile.

**Outcome:** `FAILED` (52 bytes versus the required 36). The compiler created a four-byte stack temporary, added separately to the source offset and segment, then loaded through the temporary. This source form does not preserve an efficient indexed far access.

**Diagnostic observation affecting next experiment:** The compact island shows the local spill and separated far-pointer arithmetic are the major divergence. The final experiment keeps the far pointer in the original parameter and moves the six-byte adjustment inside the indexed lvalue, testing whether it avoids temporary materialization.

**Family view versus ordinary islands:** Register-role groupings remain contradicted and add no actionable distinction over the island's call-frame/segment and extra-instruction evidence.

## nopsub_326BA, attempt 3

**Prediction before compile:** The second source form made a far pointer temporary and emitted stack locals plus separate offset and segment calculations. This form keeps the original far byte pointer intact and applies the six-byte header adjustment in the indexed lvalue; it should avoid the temporary and return to a compact indexed far load.

**Falsifier:** Any stack spill, separated far-pointer arithmetic, incorrect `index*4` access, or nonmatching 36-byte contribution falsifies this form under the pinned profile.

**Outcome:** `FAILED` (34 bytes versus the required 36); the grinder identified the object as identical to attempt 1. The source edit changed the spelling but produced the same effective compiler output.

**Diagnostic observation affecting next experiment:** The compact diagnosis returned to the same two islands and the same `LES BX`/SI-index sequence as attempt 1. This shows the header adjustment inside the indexed lvalue is optimized back to the same effective object.

**Family view versus ordinary islands:** Same contradicted register-role family as attempt 1; it adds no actionable information over the ordinary islands.

## copy_string, attempt 1

**Prediction before compile:** With a near destination argument and far source argument, the `do` loop should copy one source byte, increment the destination argument and the local far source pointer, then test the next source byte. On exit it should write a final NUL. The far-pointer local should produce the four-byte stack slot, `LES` loads, and frame cleanup visible in the 56-byte target.

**Falsifier:** A pre-test loop, different argument order/address spaces, no local far-pointer increment, or a contribution different from the 56-byte interval disproves this source form for the pinned profile.

**Outcome:** `FAILED` (40 bytes versus the required 56). The compiler increments the far source argument directly at `[BP+8]`; it did not materialize the local pointer needed by the reference.

**Diagnostic observation affecting next experiment:** The compact islands show the target uses a far pointer at `[BP-4]` for both the character read and terminator check, while the candidate uses `[BP+8]`; the candidate also increments that argument in place. The next experiment introduces a distinct local far source cursor and increments it while preserving the same do-while behavior.

**Family view versus ordinary islands:** The family view grouped a `-4 -> 8` BP-displacement relation, consistent with the islands' missing local-pointer materialization. The ordinary islands already show the concrete operations; grouping is supporting observation, not proof of object identity.

**Retrospective clarification:** The attempt 1 candidate did not declare a separate local source cursor; the forecast that the compiler would materialize one was incorrect. Attempt 2 explicitly introduced that local.

## copy_string, attempt 2

**Prediction before compile:** Copying the far source parameter to a local `char far *current` and incrementing `current` should materialize a four-byte stack slot, use `LES` from `[BP-4]` for both the copied character and next-byte test, and preserve the target's final NUL store and 56-byte extent.

**Falsifier:** If the optimizer eliminates the local and increments the parameter at `[BP+8]`, or if loop/return lowering changes the 56-byte contribution, this source hypothesis fails.

**Outcome:** `FAILED` (56 bytes, but bound payload differs). The explicit local source pointer reproduces the target frame setup and cleanup. The candidate evaluates the postfix increments early, however: it increments destination before loading/storing the byte and increments the local source before loading it.

**Diagnostic observation affecting next experiment:** The compact islands and paired instruction streams pinpoint statement-expression ordering: candidate increments at offsets `0x16` and `0x1c` precede the source read and destination write, while target increments at `0x1e` and `0x21` follow the write. The next experiment uses a plain store followed by separate increment statements to impose that sequence.

**Family view versus ordinary islands:** No supported difference family was present in the compact report. The ordinary island view was sufficient to identify the operation ordering.

## copy_string, attempt 3

**Prediction before compile:** A plain `*destination = *current` statement followed by separate `++destination` and `++current` statements should emit the reference order: read through the local far pointer, store through the destination argument, increment destination and local source, then test the next byte.

**Falsifier:** Any increment before the byte load/store, incorrect next-byte test, or nonmatching 56-byte contribution disproves this statement-order hypothesis.

**Outcome:** `FAST_PASS_ONLY`. The complete 56-byte contribution matches exactly under the pinned profile. Strict production acceptance and promotion remain for the root agent.

**Diagnostic observation affecting next experiment:** No mismatch remained; strict byte equality closed the bounded experiment.

**Family view versus ordinary islands:** No post-pass family interpretation was needed; exact complete bytes are the deciding evidence.

## Root acceptance

After reviewing the current packet, candidate and recipe, root ran `grind.py promote copy_string` with unchanged source. Fresh compilation and complete-image construction passed: `PROMOTED`, `HYBRID_EXACT`, durable report `recovery/attempts/copy_string/0004/report.json`. This recovers 56 C bytes. The family view receives no incremental recovery credit: its grouping supported an observation already available in the islands, and the decisive statement-order repair needed no supported family.

Final validation is recorded separately in `docs/current/validation.json`; promotion is not a substitute for that check. `nopsub_326BA` remains budget-blocked and the camera block was never reopened.
