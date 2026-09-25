# Selected compiler observations

These are observations from earlier experiments, not acceptance rules or investigation gates. Retrieve their full provenance with `git show 303324cd6d3d310059cd129f41dd7ba399b457bc:PATH`. Fresh search should test whether they still apply to its exact source/profile context.

- `is_facing_camera`: declaration order explained a paired BP-home permutation for one source/profile. A byte-result cross-test emitted 198 bytes with AL/CBW but retained a different zero-return topology from the 200-byte original. A trailing `goto` alone did not move that epilogue in pinned boolean-return fixtures. Provenance: `recovery/experiments/is_facing_camera/declaration-order-20260923.md`; `recovery/compiler-evidence/boolean-return-placement/`.
- `rect_adjust_from_point`: a register-coordinate candidate emitted 76 bytes with only two BP-2 versus BP-6 store displacements remaining. Moving the temporary declaration ahead of the register coordinates repeated the output. Provenance: `recovery/experiments/rect-adjust-register-20260923.md`.
- `file_get_shape2d`: a huge-pointer hypothesis emitted 114 bytes plus an `__AHSHIFT` fixup; explicit linear-address hypotheses removed the helper but emitted 138/146 bytes against 95 original bytes. Provenance: `recovery/experiments/file-get-shape2d-source-20260923.md`.
- `pad_id`: a long-copy hypothesis reproduced far AX/DX loads; a register index selected SI but retained a two-byte frame and NOP (58 versus 54 bytes), identically under MSC 5.00/5.10. Provenance: `recovery/experiments/pad-id-20260923.md`.
- `vector_op_unk`: a RETF followed by another apparent prologue occurs within the imported extent. Use the extent audit and original references before treating that interval as one complete contribution. Provenance: `recovery/experiments/empires-method-transfer-20260923.md`.

The compact search context points to the original source/assembly evidence. The checkpoint is historical retrieval only; no old ledger or supervisor permission is required to investigate any of these functions.
