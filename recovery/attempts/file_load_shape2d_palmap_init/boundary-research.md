# Palmap initialization boundary review

## Finding

No 40-byte boundary correction is justified by the current evidence. Preserve the independently mapped 39-byte procedure and the failed complete-contribution trial. The NOP immediately after it is present in the pristine image, but the strongest existing segment-level source evidence places that byte before the next procedure, as its alignment prefix.

## Evidence

- `recovery/cards/file_load_shape2d_palmap_init.json` and `recovery/restunts-inventory.json` identify `file_load_shape2d_palmap_init` at load offset `175280` (`0x2ACB0`), with 39 instruction bytes ending after the `retf` at `175318` (`0x2ACD6`); exclusive end is `175319` (`0x2ACD7`). The matching Restunts source is `build/references/restunts/src/restunts/asmorig/seg034.asm`, lines 404–441: `retf`, `file_load_shape2d_palmap_init endp`, then `seg034 ends`; there is no following alignment directive in seg034.
- The pristine image has byte `90` at `175319` (`0x2ACD7`); the following procedure begins at `175320` (`0x2ACD8`). The next source segment is separately declared byte-aligned `seg035`, class `STUNTSC`, in `seg035.inc` and `seg035.asm`.
- `build/references/restunts/src/restunts/asmorig/seg035.asm`, lines 54–56, explicitly places `db 144` under `; align 2` immediately before `file_load_shape2d_res_fatal proc far`. Its first procedure's verified start is `175320`. This source byte and following procedure start agree with the pristine `90` gap and `0x2ACD8` entry. The pinned Restunts reference hashes are in `layout/references.json`.
- The isolated pinned MSC5.1 compile produced a 40-byte complete `UNIT_TEXT`. The one `_palmap` offset16 fixup is exactly the predicted site (`+23`); the existing binder derives `0x5472` from the independent DGROUP mapping. Applying the existing binding function read-only to all 40 bytes gives an exact match for pristine `[175280,175319)`, followed by `90`. Receipt and full frozen diagnostic are preserved in `recovery/attempts/file_load_shape2d_palmap_init/0001/`.

## Policy and disposition

`docs/current/architecture.md` says compiler alignment belongs to the owned contribution and prohibits trimming. `docs/current/readiness.md` and `docs/current/provenance.md` warn that a procedure's RETF or Restunts source grouping alone does not prove original contribution/TU boundaries; `asmorig` is not pristine. Therefore the matching extra NOP cannot be added to palmap's expected extent just to fit the isolated compiler object. The existing single-public binder has no mechanism to detach it, and the raw interval remains unresolved. No manifest, inventory boundary, recipe extent, or oracle bytes were changed in this review.

The missing proof for a different mapping is original contribution/TU ownership of `0x2ACD7` sufficient to supersede the current 39-byte verified procedure extent. Until then, retain 39 bytes and do not promote this isolated 40-byte object. Any future remedy must use existing complete-contribution rules; a multi-public/TU acceptance path would require separately authorized capability work.
