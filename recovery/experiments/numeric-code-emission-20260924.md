# Numeric CODE emission mapping, 2026-09-24

## Change

The importer now parses one unsigned numeric `db` or `dw` value, with an optional source label, as an exact raw emission. The parser refuses expressions, symbol offsets, lists, DUP forms, negative values, other widths and values that overflow the directive. Unlabeled `db 144` retains the existing reviewed NOP interpretation. Every imported byte still has to agree with the pristine image through the complete source stream and interval decode. The importer records these source declarations as raw emissions, but a separate entry may decode the same bytes as instructions. No code/data classification follows.

This enables source-context boundary mapping across embedded numeric data. `init_div0`'s eleven trailing bytes at `byte_19F07` match the oracle; `parse_shape2d_helper2`'s four `dw 0` words at `word_2F354` through `word_2F35A` match the oracle. The latter are CODE-segment data used by adjacent critical-error routines, not C body instructions. The source grammar and exact-byte fixtures have positive and negative tests.

## Measured delta from commit c4b4726

| Imported state | Before | After |
| --- | ---: | ---: |
| Partial unmapped | 23 | 15 |
| No address anchor | 7 | 2 |
| Exact source-emission coordinates, CFG pending | 62 | 71 |
| Exact instruction-anchor intervals | 527 | 531 |

Thirteen additional procedures have exact emission extents, totaling 3,506 bytes: nine mixed emission/code intervals (3,268 bytes) and four instruction-only intervals (238 bytes) reached through the now verified surrounding source stream. No previously exact interval changed its start, end or SHA-256. The fresh blocker census moved `BOUNDARY_OR_EXTENT` from 30 to 17 and `EMISSION_BYTES_CFG_REVIEW` from 56 to 65. SUPERVISOR remains 576, MEDIUM 8, CHEAP 0. No C function or byte was promoted.

`sub_39088` is a concrete overlap control: its `byte_3930E db 131; db 126; db 240` bytes are a branch target and begin a valid instruction sequence when entered there. It must never be automatically classified as data. The fresh single-entry CFG audit is now folded into blocker reclassification: 18 SUPERVISOR tasks spanning 7,512 mapped bytes show nonpadding instructions unreachable from the named entry under direct-flow traversal. This is a candidate ownership/entry blocker, not proof that the bytes are dead. The newly mapped `font_set_unk` source PROC contains four successive prologue/RETF bodies in its 112-byte interval. Its byte and instruction alignment does not prove a single-entry function or original TU/public structure. It remains SUPERVISOR and needs explicit ownership/CFG review before any recipe. `criterr_interrupt_handler` and `set_criterr_handler` are interrupt/runtime forms, likewise not ordinary C canaries. The exact emission status of `parse_shape2d_helper2` preserves its trailing data ownership question.

## Limits and next experiment

The new parser establishes source-declared emission bytes and, where the full stream agrees, interval coordinates. It cannot prove CFG reachability, whether a CODE literal belongs to a separate historical object, a compiler profile, or any production binder mode. A shared CFG probe should decode around raw emission spans, detect direct/public entry points into them, resolve exact indirect table targets, and refuse unresolved index bounds and external entries. Source/manifest promotion still requires the existing strict whole-image path.

Verification: `tools/validate.py` passed 171 tests, independently recompiled the 25 active C functions, confirmed `HYBRID_EXACT`, and checked current cards and queue.
