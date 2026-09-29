# Historical freeze: `historical-exact-oracle-v1`

This record defines the historical source and proof boundary intended for tag `historical-exact-oracle-v1`. The SDL3 port is a separate line of work derived from this frozen historical base; port-only adapters remain outside the historical acceptance source and recipes.

## Identity

| Item | Value |
|---|---|
| Freeze commit | the commit tagged `historical-exact-oracle-v1` (`git rev-parse historical-exact-oracle-v1^{commit}`) |
| Intended tag | `historical-exact-oracle-v1` |
| Immutable oracle lock SHA-256 | `9F40E20B5A1855DE03760DF68D14AE5455E019172B81173BA76DD0FBA4699804` (`layout/oracle.lock.json`) |
| Locked packed MZ | 199,125 bytes; SHA-256 `f57ac2775b6156b0413775eb61a1c48743f64248f72d13ca970dc645029eb41d` |
| Locked unpacked MZ | 210,384 bytes; SHA-256 `1adb8259b3f6634062b94826e1f167649d9f2deeb6cedc9989127fc37e9ce615` |
| Locked load image | 200,000 bytes; SHA-256 `4fc3340e2dc5d139a95a9a92278ef09902003d2c6857e5d9cdf0112870f3789e` |
| Fresh validated unpacked MZ | 210,384 bytes; SHA-256 matches the locked unpacked MZ |

## Toolchain

The matching compiler profile is Microsoft C 5.10 medium model (`/AM /O /Gs`). The final `seg007` C contribution uses the independently accepted MSC 6.00 medium `/Zi` profile (`/AM /Os /Oe /Og /Gs /Zi`). Tracked matching assembly uses pinned MASM 5.10 (`/Mx /I.`). Validation uses the pinned MS-DOS Player and the hash-identified independent DOSBox-X runner. The separate LINK 3.65 + EXEPACK diagnostic uses tool identities and profiles from `layout/toolchain.json`.

Key executable hashes and full profile file catalogs are recorded in [`layout/toolchain.json`](../layout/toolchain.json) and summarized with the gate receipts in [`freeze_gate_results.md`](evidence/freeze-campaign/integ55-freeze-gate-results.md). The profile used for a source contribution is a reproduction constraint, not proof that it was the unique original compiler or assembler.

## Freeze validation

`HYBRID_EXACT` means that the complete accepted hybrid image and its exact ordered MZ relocation obligations match the immutable oracle. The fresh `python tools/validate.py` gate passed 720 tests, all 90 independent DOSBox-X contribution checks, the whole-image comparison, all 2,588 ordered relocations, and the BSS/runtime placement gate. `python tools/validate.py --image` repeated that gate and added a real LINK 3.65 + EXEPACK diagnostic: zero image mismatch bytes, exact relocation set, exact bank order, exact packed image, and zero alias shims.

The oracle-ordered image equality is distinct from the natural historical-link hypothesis. `python tools/histbuild.py` freshly compiled 38 C and 52 ASM contributions and linked/packed 147 units, with zero raw-debt OMF objects, but its combined-library module-order reconstruction places only 2,023 of 2,588 relocation positions in oracle order (bank 0 exact; banks 1–3 retain the residual). The combined-library arrangement is an explicit reconstruction assumption: `GAME.LIB` starts with pinned `MLIBCR.LIB`, reconstructed game modules follow image-derived order, and pinned `LIBH.LIB` is searched separately by LINK 3.65. This is not a recovered original LINK command. The exact command durations, hashes, ownership counts and test receipts are in [`freeze_gate_results.md`](evidence/freeze-campaign/integ55-freeze-gate-results.md).

## State of the source

Fresh ownership reports separate the historical output by origin:

| Contribution | Bytes |
|---|---:|
| Matching C code + owned C data | 154,618 |
| Matching ASM code + owned ASM data | 36,552 |
| Pinned runtime code + owned runtime data | 8,768 |
| LINK fill | 56 |
| Unresolved initialized bytes | 0 |

The BSS real-link gate placed all accepted object, common and pinned-runtime storage. It reports 7,363 accepted object BSS bytes, 14,944 accepted communal bytes, 38 accepted runtime BSS bytes and 13 LINK-fill bytes; raw object/runtime/common/fill BSS debt is zero. `histbuild.py` independently reports zero raw initialized bytes, zero raw BSS bytes and zero raw-debt OMF objects. Byte ownership and exact output do not imply that all behavior has been recovered as C: matching ASM and pinned runtime remain separately identified.

## Port-only readiness checks

The freeze run passed the target aggregate layout tests (3), state-model smoke tests (2), ASM migration completeness tests (2), and runtime trace normalizer regression tests (3). Following the port-only declaration/adapter fix, both host GCC modes (`compat` and `strict-central`) compile 38/38 sources for syntax and object output. The integ55 gate receipts remain preserved as the earlier historical run; this follow-up changes only porting declarations and adapters. These compile-only results do not mean the game has been ported or linked for a host.

## What this freeze proves

- The accepted C, ASM, pinned-runtime and data contributions together reproduce the full locked image and exact ordered relocation obligations under the strict oracle gate.
- Complete object extents, accepted declarations/fixups, preserved ownership, whole-image equality and BSS placement pass fresh validation.
- The supported code contributions also pass the independent DOSBox-X compiler backend checks.
- The current C/ASM/runtime/raw ownership split is explicit, with no unresolved initialized bytes or raw BSS debt.

## What this freeze does not prove

- It does not recover the original translation-unit grouping, a general original LINK response, unique original compiler version, or arbitrary binding/frame modes. The natural-order LINK experiment remains a separate hypothesis with an ordered-relocation residual.
- It does not mean every routine or stored field has a recovered semantic name, nor that every input, mode, renderer path or device edge is behaviorally understood.
- The Port Forge v7 trace evidence covers bounded workloads only: one sampled 20 Hz race, observed timer deadlines, frame-counter deltas, pause/resume and guest audio batches. It does not cover 10 Hz, crash/airborne states, retrace phase or broad DOS/SDL conformance. See [the runtime oracle](porting/runtime-oracle.md).
- It is not an SDL3 port. Host dispatch, segment:offset resolution, SDL subsystem work and trace emission remain port implementation tasks.

Remaining evidence uncertainty is listed in [unresolved.md](unresolved.md). Unknown data and semantics stay opaque when complete original extents and behavior have been preserved.

## Reproduction

From the repository root, with immutable original assets and the pinned tools supplied locally:

```powershell
python tools/validate.py
python tools/validate.py --image
python tools/histbuild.py
python -m unittest discover -s tests -p test_port_header_layout.py -v
python -m unittest discover -s tests -p test_asm_migration_completeness.py -v
python -m unittest discover -s tests -p test_runtime_trace_normalizer.py -v
python tools/porting/host_probe.py --mode compat
python tools/porting/host_probe.py --mode strict-central
```

`validate.py --image` proves the real-link result only within its oracle-derived order; `histbuild.py` reports the separate natural-link assumption. Neither command changes the oracle lock.

## Reproducibility correction

The frozen checkout's test suite had hidden dependencies on ignored worker proposals, recipes and source files, an already-generated `build/oracle/load-image.bin`, and libraries addressed relative to the repository instead of the pinned toolchain paths. Those inputs could exist in a developer tree while being absent from a clean clone.

The tests now use small tracked historical fixtures or derive the expected contribution from the accepted manifest. Pinned Restunts facts used by the gate are available as compact, identity-checked excerpts; a full `build/references/restunts` checkout remains useful to optional research tools. Toolchain archive reads resolve through `layout/toolchain.json`. `validate.py` creates `build/` and writes the derived oracle image before the tests run. Test temporary directories no longer assume `build/` already exists.

The clean-clone proof below copies only the original `assets/`, places the required Capstone 5.0.3 package in ignored `build/python/`, and resolves the pinned compiler/assembler/runners through `layout/toolchain.json`. No worker archive, candidate queue, ledger, reference checkout or prior oracle output is required by the test gate.

| Clean-clone command | Receipt |
|---|---|
| `python tools/validate.py --image` | Pending fresh-clone run |
| `python tools/porting/host_probe.py --mode compat` | Pending fresh-clone run |
| `python tools/porting/host_probe.py --mode strict-central` | Pending fresh-clone run |

## Why ports start here

The historical tag is the immutable reference for future ports: it retains the original segmented ABI, integer widths, object/data extents, exact relocations, legacy rendering behavior and unknown bytes without normalizing them to host assumptions. A `portable-sdl3` branch should derive from the tag, keep host adapters separate, and compare its runtime traces against the documented observations while preserving the coverage limits in [unresolved.md](unresolved.md).

## Final pre-tag gate (exact tagged tree)

Run by the supervisor on the exact tree committed and tagged, after the port-only adapter fix, with `build/reallink/cache`, `build/search/candidates` and `build/search/environments` moved aside first:

| Command | Result |
|---|---|
| `python tools/validate.py --image` | PASS in 9 min 5 s: 721 tests; `HYBRID_EXACT`; 90/90 independent DOSBox-X contribution checks; real LINK 3.65 + EXEPACK image equal (0 mismatch bytes), relocation set and bank order equal, packed image equal, 0 alias shims; `UNRESOLVED_RAW` 0; BSS/runtime gate PLACED (raw object/runtime/communal/fill BSS 0) |
| `python tools/porting/host_probe.py --mode compat` | 38/38 syntax and object compiles |
| `python tools/porting/host_probe.py --mode strict-central` | 38/38 syntax and object compiles |

The earlier full fresh gate (including `validate.py`, `histbuild.py` and the focused port tests) is recorded in [integ55-freeze-gate-results.md](evidence/freeze-campaign/integ55-freeze-gate-results.md); campaign evidence is under [evidence/freeze-campaign/](evidence/freeze-campaign/).
