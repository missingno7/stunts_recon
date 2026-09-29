# Freeze gate results — 2026-09-29

## Freshness and scope

`layout/oracle.lock.json` remained unchanged; SHA-256: `9F40E20B5A1855DE03760DF68D14AE5455E019172B81173BA76DD0FBA4699804`. The locked packed oracle is 199,125 bytes, SHA-256 `f57ac2775b6156b0413775eb61a1c48743f64248f72d13ca970dc645029eb41d`; its unpacked MZ is 210,384 bytes, SHA-256 `1adb8259b3f6634062b94826e1f167649d9f2deeb6cedc9989127fc37e9ce615`; load image is 200,000 bytes, SHA-256 `4fc3340e2dc5d139a95a9a92278ef09902003d2c6857e5d9cdf0112870f3789e`.

Each validation ran as a fresh Python process, so `tools/memo.py`'s evidence cache began empty. Before the first validation, these directories were moved into `build/workers/integ55/cache_backup/`: `build/reallink/cache`, `build/search/candidates`, and `build/search/environments`. After each validation, newly emitted `build/reallink/cache` and `build/search/environments` were moved into a separate backup subdirectory before the next image gate. `build/search/candidates` was not recreated. `histbuild.py` used its fresh unique run directory. No original assets, pinned tool files, or reference checkouts were moved or changed.

## Gate commands and results

| Exact command | Duration | Result |
|---|---:|---|
| `python tools/validate.py` | 528.964 s | PASS; 720/720 tests (suite log: 304.685 s), `HYBRID_EXACT`, 90/90 independent DOSBox-X contribution checks, 2,588 ordered relocations, BSS/runtime storage `PLACED`. |
| `python tools/validate.py --image` | 594.577 s | PASS; 720/720 tests (suite log: 351.635 s), `HYBRID_EXACT`, 90/90 independent checks, BSS/runtime storage `PLACED`. The additional LINK 3.65 + EXEPACK oracle-ordered diagnostic reported `image_equal=true`, 0 mismatch bytes, relocation set exact, bank order exact, packed image exact, 0 alias shims. |
| `python tools/histbuild.py` | 30.370 s | `BUILT_WITH_ORDER_RESIDUAL`; 147 units/objects, C 38/38 and ASM 52/52 freshly built; LINK 3.65 and EXEPACK each ran once; zero raw-debt OMF objects. The image/packed executable differ under the natural-order hypothesis: 2,023/2,588 relocation positions are in oracle order, bank 0 exact, banks 1–3 retain the ordering residual. Report: `build/histbuild/build-20260929T151414814613Z/report.json`. |
| `python -m unittest discover -s tests -p test_port_header_layout.py -v` | 0.322 s | PASS; 3 tests, including target aggregate size/offset compile assertions. |
| `python -m unittest discover -s tests -p test_porting_state_model.py -v` | 0.257 s | PASS; 2 tests. |
| `python -m unittest discover -s tests -p test_asm_migration_completeness.py -v` | 0.264 s | PASS; 2 tests; manifest ASM and pinned-runtime coverage complete. |
| `python -m unittest discover -s tests -p test_runtime_trace_normalizer.py -v` | 0.285 s | PASS; 3 tests, including fixture normalization, in-place overwrite refusal, and malformed-row reporting. |
| `python tools/porting/host_probe.py --mode compat` | 34.401 s | CLI PASS; 38/38 sources pass syntax and object compilation. This is compile-only. |
| `python tools/porting/host_probe.py --mode strict-central` | 11.305 s | CLI completed; 36/38 sources pass syntax and object compilation. The three diagnostics are conflicts in `obj_seg008.c` for `call_read_line` and `read_file_with_retry`, plus `obj_seg028.c` for `send_audio_stop_event` first-argument signedness. These are PORT_BUILD declaration-view issues with F3b-proven machine contracts, not historical gate failures. |

Both normalizer smoke commands also succeeded: `python tools/porting/normalize_portforge_trace.py build/workers/F5c/event_trace.jsonl --out build/workers/integ55/replay-normalized.jsonl` wrote 37,628 rows; the same command with `active_pause_event_trace.jsonl` and `pause-normalized.jsonl` wrote 4,441 rows. They were not separately timed.

## Ownership, image, relocation, and BSS status

Fresh `validate.py` ownership totals: matching C 137,978 code + 16,640 data bytes (154,618 total); matching ASM 30,977 code + 5,575 data bytes (36,552 total); pinned runtime 7,576 code + 1,192 data bytes (8,768 total); LINK fill 56 bytes; unresolved initialized bytes 0. The whole oracle-ordered hybrid image and all 2,588 ordered relocations match.

The real-link BSS/runtime placement gate placed all 12 listed rows. It reports 7,363 accepted object BSS bytes, 14,944 accepted common bytes, 38 accepted pinned-runtime BSS bytes, and 13 LINK-fill bytes. Raw object BSS, raw runtime, raw communal and raw fill counts are all zero. `histbuild.py` separately reports raw initialized bytes 0, raw BSS bytes 0, and raw-debt OMF objects 0.

## Toolchain identities from `layout/toolchain.json`

| Profile/tool | Flags or use | Pinned identity |
|---|---|---|
| `msc510-medium` | `/AM /O /Gs`, canonical medium-model profile | `CL.EXE` `cad40cef732beea3455225fbd62286c664166db7c3f87e2402e22b2e3f107771`; pass binaries `C1.EXE` `8d33c6d87aa74b042b7d7cb01a23241c4cc3b1b8b446b8f1548bd167e3c7bf62`, `C2.EXE` `a513c98bc2ce11bd00901b560252b5727ab9b6bacd9319b9eaba08f9e314b99b`, `C3.EXE` `c1269628534d5deb48171ba9e42f761dcb1c28a0087c73780db0ee2e4352e6fa`. |
| `msc600-medium-zi` | `/AM /Os /Oe /Og /Gs /Zi`, final `seg007` contribution | `CL.EXE` `136a092d68a206430b775f761d33d03ca350c385e1214ec6175f6690084b3598`. |
| Pinned LINK 3.65 / EXEPACK | Historical build and diagnostic pack step | `LINK.EXE` `124a3c800edc16b60f696d35a9dfec798b68b78fe2ca90c5988ea76bdeab1a8a`; `EXEPACK.EXE` `568554322d29e46f79c0c760ac2391cb0689dc7acb9aed79a428194ec6fb83e7`. |
| `masm510-game` | `/Mx /I.` | `MASM.EXE` `1c6286c69b616160b8475ad17453ee872702a2c98075c2159ec792a1c745275f`. |
| MS-DOS Player runner | `-e -v5.00` | `f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02`. |
| Independent DOSBox-X runner | DOSBox-X 2026.08.31 configured identity | `b028a4d328302ea270722dec69f3ec3a10ebf76beba72a51970db33a88df2bf6`. |

The complete file catalogs, provenance and remaining profile identities are in `layout/toolchain.json`; `validate.py` verified the configured profiles at run start.

## Interpretation

`validate.py --image` proves equality under the locked oracle-derived order, including the real LINK 3.65 + EXEPACK image diagnostic. `histbuild.py` tests a distinct natural module/library-order hypothesis and does not reproduce the oracle's ordered relocation table. Its combined-library arrangement is the documented reconstruction assumption described in `README.md` and `docs/acceptance.md`, not a recovered original LINK response.
