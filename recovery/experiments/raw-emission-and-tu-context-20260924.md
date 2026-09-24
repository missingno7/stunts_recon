# Raw-emission mapping and frozen-body TU context, 2026-09-24

## Exact source-emission mapping

A fresh audit of the previous partial-mapping failures found 42 `db`/pristine-`add` first failures whose source byte equaled the pristine byte. Forty were `align 2; db 0`; linear disassembly had consumed that zero with following code as a false `add`. The importer now treats only literal `db 0` as an exact raw-byte emission, compares the byte in the pristine load image, and resumes instruction decoding at the following coordinate. Other raw directives remain unsupported. A mixed raw/instruction interval is explicitly `BOUNDARIES_AND_EMISSION_BYTES_VERIFIED` with `verified_emission_coordinates_cfg_unreviewed` confidence; it does not enter the verified-instruction work queue or code-byte classification.

Importer statuses changed from 497 instruction-verified / 72 partial / 50 no-address-anchor to 527 instruction-verified / 52 exact-emission-CFG-unreviewed / 33 partial / 7 no-address-anchor. All 497 previously verified intervals retained their exact start, end, and SHA-256. Of the former failures, 39 partial and 13 no-address-anchor cases became mixed-emission intervals; 30 no-address-anchor cases became ordinary instruction-verified intervals. The fresh SUPERVISOR census records 46 active `EMISSION_BYTES_CFG_REVIEW` tasks covering 6,269 byte coordinates. Their code/data CFG status and source viability remain unproved. Queue movement after refresh: SUPERVISOR 585 to 576, MEDIUM 5 to 8, CHEAP stays 0. No C promotion is implied.

The remaining partial-mapping report now has 33 tasks. The former leading `db`/`add` pattern fell from 42 to 3; the rest need specific source/data, opcode, or boundary investigation. `reconstruction_factory.py reclassify` regenerates this report and the candidate OMF census so future supervisor routing uses current evidence.

## Frozen-body TU context

A bounded four-compiler MSC 5.1 experiment held each of two `audio_toggle_flag` target bodies fixed. Only surrounding helper definitions/declarations changed. External declarations yielded 26-byte target windows with two far-call pointer32 fixups; static predecessor definitions yielded 22-byte windows with two self-relative offset16 fixups. The same two effective groups occurred for both targets. Applying the recorded static-call fixups to independently mapped pristine helper addresses, plus the data operand, conditionally yields each 22-byte target body exactly. This is research evidence that TU context changes emission. The local helper stubs and order differ from the mapped component, so neither historical same-object membership nor strict whole-image acceptance follows. Plans, source, objects, and full fixup records are under `build/private/tu-context-input/audio-toggle/` and `build/private/tu-context/`.

## New MEDIUM research

Three tasks enabled by fresh mapping received bounded private source batches under the pinned profile, with no production attempt or source/manifest edit. `nopsub_32738` (14-byte target) produced four effective outputs in five compiles; its nearest ordinary C form is 22 bytes and calls `__aFuldiv`, while the pristine target uses inline word DIV. `multiply_and_scale` (26-byte target) produced six distinct outputs in six compiles; each emitted a 32-bit multiplication helper, while the target uses inline signed word IMUL. `nopsub_3215A` (18-byte target) produced five effective outputs in six compiles; none emitted the target `REP STOSW` sequence or extent. These findings point to compiler idiom/source-width or context research, not source acceptance. Full private reports are under `build/private/research-batches/load_22738/3a9823792d45/`, `load_20044/b8fc650e2b14/`, and `load_2215a/0e312186dbe5/`.

Strict production ownership remains unchanged; raw, C, ASM, and pinned runtime counts must be read from the final generated status and validation receipt.

## CFG audit limit

A follow-up read-only audit found many `align 2; db 0` emissions immediately before `endp`, including `file_find_next` and `video_get_status`, but did not prove all incoming references or predecessor paths for the 52 mixed intervals. Other cases have internal stubs or multiple raw bytes, including `__sigentry` and `file_load_shape2d_expandedsize`. The exact-byte mapping remains the strongest supported automated rule; no interval was promoted to instruction-verified status or reopened by this audit.
