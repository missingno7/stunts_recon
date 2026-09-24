# Current candidate OMF shape census, 2026-09-24

`tools/candidate_omf_census.py` now compares each complete locally archived compiler `UNIT_TEXT` object against its verified pristine interval. It omits only the exact byte ranges declared by complete candidate FIXUPP records, counts all other byte agreements, and records full public/other-segment status. Masked operands remain unresolved binding obligations. The report is research-only and cannot open a queue task or authorize promotion.

The fresh current-SUPERVISOR archive contains 82 `.OBJ` files, 64 distinct task/object payloads across 28 tasks, and no parser errors. Sixty objects have a different `UNIT_TEXT` extent from their target. Four have equal extent:

| Task | Nonfixup bytes equal | Remaining blocker |
|---|---:|---|
| `unload_skybox` | 4 / 18 | Source codegen/branches and several data references |
| `set_frame_callback` | 16 / 16 | Two data addresses and a callback code-pointer offset/base pair; five complete ordered fixups |
| `audioresource_copy_n_bytes` | 11 / 70 | Source codegen difference despite equal extent; no fixups |
| `file_load_shape2d_nofatal` | 9 / 16 | Missing pristine `PUSH CS`, self-relative call context and trailing NOP |

Only `set_frame_callback` is a complete single-public, zero-other-contribution, nonfixup-byte-exact candidate among the sampled supervisor objects. Its code-pointer binding and data ownership remain outside proven production capabilities. A general binder expansion would currently have at most this one observed source-shaped candidate; it would not repair the 60 wrong extents. Candidate archive frequency is not an estimate of original OMF mode frequency.

This reorders the next experiments toward source/TU context and mapping. `recovery/build-topology.json` has compatible runs but no proven historical same-object pair or boundary. The bounded frozen-body audio-toggle context probe tests whether a repeated near-call pattern responds to predecessor definitions, without claiming historical membership or production acceptance. The remaining mapping residuals need separate code/data classification before any boundary is promoted.

## Effective-output collapse added on the next pass

The census now hashes complete effective object contributions: segment declarations and initialized bytes, public order, externals, and ordered FIXUPP records. It excludes OMF module names, record packing, and checksums. This is a research grouping, not proof of link equivalence. In the current 64 distinct task/object payloads, 60 effective outputs remain. Four two-object groups collapse under `load_09dc6`, `load_15f2e`, `load_2264a`, and `load_29c84`; each group retains its raw object SHAs and target/nonfixup comparison in `recovery/candidate-omf-census.json`. A negative test ensures changing a fixup target or public offset changes the effective signature. Reclassification regenerates the groups from the current archive.

A separate review of archived compiler-profile fixtures found no positive object/component signature for MSC 5.0 versus 5.1. Seven exact functions and the exact 114-byte rectangle pair have the same complete bound contributions under both versions; the rectangle pair also has the same publics, segment definitions, and fixups. The current topology has no proven original same-object pair or boundary, so no component-level alternate profile is assigned. This avoids reopening blind global flag sweeps.
