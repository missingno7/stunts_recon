# Exact CODE-segment offset tables, 2026-09-24

## Capability

`tools/table_offset_probe.py` now inventories complete Restunts `off_<IDA> dw offset loc/locret_<IDA>` groups, including contiguous continuation words. It requires one independently derived code-segment paragraph from mapped far calls, unique source definitions for the table and every target, target offsets in that frame, exact full table bytes against the pristine load image, no MZ relocation inside the table, and an independently verified numeric label immediately after the table. Unsupported names, addends, missing definitions, unresolved frames, mismatching bytes, and unbracketed tables remain refusals.

`tools/restunts_base.py` consumes only bracketed exact tables as two-byte raw source emissions, then reruns the full source-label mapping. The ordinary instruction status and 527 previously exact instruction intervals did not drift. Complete mapped functions with a table carry `BOUNDARIES_AND_EMISSION_BYTES_VERIFIED` and remain CFG-unreviewed. `tools/reconstruction_factory.py reclassify` regenerates the table census before the blocker census, keeping both current with the source inventory and oracle.

## Measured result

| Measure | Before | After |
| --- | ---: | ---: |
| Partial imported procedures | 33 | 23 |
| Exact emission-coordinate procedures | 52 | 62 |
| Exact instruction-anchor procedures | 527 | 527 |
| SUPERVISOR queue | 576 | 576 |
| MEDIUM queue | 8 | 8 |
| CFG-review blocker tasks | 46 | 56 |
| Boundary/extent blocker tasks | 40 | 30 |

The current census finds 27 offset-table groups in 20 task contexts: 14 exact bracketed groups across 13 tasks, totaling 320 verified source-emission bytes. Eight groups are exact but unbracketed, four have unresolved table coordinates, and one has an unresolved target. Three of the 13 tasks retain other mapping defects. The 10 fully remapped procedures are `run_option_menu`, `file_load_resource`, `__output`, `polarAngle`, `preRender_helper2`, `preRender_helper3`, `sin_fast`, `putpixel_line1_maybe`, `sub_38DE6`, and `sub_3945A`.

## Proof limit and next discriminator

These words are CODE-segment data emissions, not proven reachable instructions. This pass does not establish CFG edges through indirect jumps, original TU boundaries, C ownership, or any compiler/binder match. The queue therefore stays conservative. The next shared experiment is a bounded CFG review that treats exact table spans as data, validates all direct branches and indirect table targets, and refuses unresolved control flow. Only a separate production recipe and fresh whole-image proof could make one of these tasks grindable.

Verification: `tools/validate.py` passed 166 tests, independent exact recompilation of 25 C functions, whole-image `HYBRID_EXACT`, and current queue/card checks.
