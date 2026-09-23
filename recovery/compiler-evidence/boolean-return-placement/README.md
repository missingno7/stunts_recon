# MSC 5.1 return-block placement probe

This is independent compiler evidence, not another is_facing_camera source attempt. The candidate, recipe, production ownership and blocker were untouched. Prediction and falsifier were recorded before the two compilations in `prediction.json`.

The archived canary cast-result trial (`recovery/experiments/is_facing_camera/prior_04`) already emits AL selection plus CBW, but places the early-zero return before the arithmetic. The target has a separate trailing zero-return epilogue. This suggests asking about return-block placement independently from type allocation.

`early_return.c` uses an ordinary early return. `trailing_zero.c` moves that return behind the byte-converted long comparison with an explicit `goto`. Both are complete one-public, fixup-free contributions compiled under pinned `msc510-medium`, `/AM /O /Gs`; commands, source/staged/object identities and full decoding are in their JSON receipts. No compiler or object bytes are committed.

**Prediction refuted:** both emit identical 38-byte contributions. The explicit label does not force a trailing epilogue: MSC canonicalizes it into the same leading zero-return block. Both emit AL selection and CBW, but neither emits the target's two NOPs between the unconditional branch and false-value materialization. This is evidence for these two complete fixtures only, not proof that all goto/control-flow spellings are equivalent in MSC.

A read-only scan found the exact `mov al,1; jmp +4; nop; nop; sub al,al; cbw` byte sequence only at load 0x15FDE, inside the independently reviewed canary extent. No second source example was obtained. A unique byte pattern does not prove assembly origin or a compiler version.

Consequences: do not spend a canary attempt on a simple trailing-label rewrite. No new evidence justifies reopening its blocked source family. Retain the byte-result/return-topology question, but seek an independently supported source/context distinction before another complete-function experiment. The two fixtures are a negative compiler experiment, not a recovered game function.
