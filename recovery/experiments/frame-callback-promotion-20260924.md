# Strict callback registration recovery, 2026-09-24

`set_frame_callback` (`load_1255a`, pristine `[75098,75126)`) is now a
28-byte `MATCHING_C` hybrid replacement. The source is
`recovery/candidates/set_frame_callback.c`; the reviewed recipe is
`recipes/set_frame_callback.json`. The fresh MSC 5.10 `/AM /O /Gs` object is
372 bytes with SHA-256
`fd3f9f5921964435405c6e30e51b599f3d67bf6621f0b320caaf3cab5c8e0758`,
identical to the archived research object. Its complete `UNIT_TEXT` is 28
bytes, with one public at zero and zero DATA/CONST/BSS contribution.

The complete OMF FIXUPP order is two DGROUP offsets, one far-CALL pointer,
and the callback code-pointer offset/base pair in the actual record sequence:
`offset16@24`, `pointer32@15`, `base16@10`, `loader-offset16@7`,
`offset16@2`. The bounded `external-frame-callback-v1` binder pins the full
four SEGDEF declarations, group, public, externals, zero-addend object
skeleton, fixup indices/frames/targets/order, reviewed symbol identities,
and original ordered MZ relocation entries at 75115 then 75108. It rejects
extra contribution, unsupported modes, changed declarations, reordered or
missing fixups, altered symbols, and wrong relocation coordinates. The
historical LINK differential and its limits are in
`recovery/experiments/frame-link-differential-20260924.md`.

Address aliases are independently checked against the pristine oracle:
`_frame_callback` is mapped at 75158 and independently named by the relocated
MOV offset/segment pair in `remove_frame_callback` at 75140;
`_timer_reg_callback` retains its two relocated far-call anchors. DGROUP is
anchored at `0x2B770`; `_byte_442E4` is BSS at `0x342E4`, with pristine
comparison/increment references in `frame_callback`; `_word_46468` is BSS at
`0x36468`, with pristine accesses in `frame_callback` and separate
`update_frame`. The two-byte word fits before BSS end `0x36490`.

`recovery/attempts/set_frame_callback/0001/report.json` records fresh
`FAST_PASS_ONLY`; `0002/report.json` records serial `PROMOTED` after staging
and fresh whole-image exact acceptance. No raw bytes were inserted into C,
neither objects nor the final image were patched, and candidate fixups were not
masked. Original historical PUBDEF spelling, original private-data owner,
and TU membership remain unproved. This is a byte-exact hybrid replacement,
not proof of those source-history claims. The research probes did not measure
a recovery speedup.

Final `python tools/validate.py` passed 178 tests and independently recompiled
all 26 active C functions with complete binding obligations. Strict C recovery
rose from 25 functions / 1,090 bytes to 26 / 1,118 bytes; raw initialized
bytes fell from 198,185 to 198,157. The full image remains `HYBRID_EXACT`.
The queue moved from CHEAP 0 / MEDIUM 7 / SUPERVISOR 577 to
CHEAP 0 / MEDIUM 7 / SUPERVISOR 576. The shared-base placement diagnostic
remains research-only and did not reopen the incomplete two-public or
private-DATA objects.
