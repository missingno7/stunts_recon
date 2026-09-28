# Stunts 1.1 MCGA historical reconstruction

This workspace replaces explicit raw regions of the original MCGA image with byte-exact historical compiler and runtime output. `HYBRID_EXACT` means the complete hybrid executable and ordered relocations match the immutable oracle. It does not mean the entire program has been recovered from source.

## Current status

The final seg007 object is now reconstructed as C with the pinned MSC 6.00 non-A register-gated profile; the unreferenced dword at `[199484,199488)` is accepted at the head of the adjacent ASM module containing the copyright data. The ownership manifest has zero unresolved initialized bytes and zero raw BSS bytes. Full `python tools/validate.py` passes all 689 tests, 90 independent DOSBox-X contribution checks, fresh whole-image equality, and the BSS real-link gate. `python tools/validate.py --image` reports zero image mismatch bytes with relocation set, bank order, and packed image all exact. A fresh `histbuild.py` reports zero raw-debt OMF objects; its separate library-order diagnostic still has a relocation-order residual.

## Setup

Use Python 3.10+ on Windows. Keep original `MCGA.HDR`, `EGA.CMN`, `MCGA.DIF`, and `MCGA.COD` under ignored `assets/`. The hash-pinned Microsoft C distributions and MS-DOS Player paths are in `layout/toolchain.json`; provisioning provenance is in `evidence/msc500-provenance.json`, `evidence/msc510-provenance.json`, and `evidence/msc600-provenance.json`. The local extraction helper is `toolchain/extract_pcjs.py`. Do not commit game or compiler binaries.

Install the diagnostic decoder with `python -m pip install --target build/python capstone==5.0.3`. Evidence checkouts and commit/file identities are recorded in `layout/references.json`; Restunts resides at `build/references/restunts`. Keep that checkout, its reference executable, and other ignored local inputs. They cannot be restored from this repository's Git history.

Full validation independently recompiles active C and reassembles active ASM under DOSBox-X at `C:/tools/dosbox-x/dosbox-x.exe` (hash-identical copy of `C:/DOSBox-X`); all compiles run under MS-DOS Player at `C:/tools/nmlgcdos/msdos.exe`. Configure local runner paths explicitly when moving machines, preserving pinned tool identities. The pinned MASM 5.10 ASM profile is a reproduction choice, not an attribution of the original assembler. No verification command updates the byte oracle.

## Historical build

Run the complete historical toolchain pipeline from the repository root with:

```powershell
python tools/histbuild.py
```

Each run freshly compiles every accepted C contribution from `src/` with its
recipe's pinned CL profile, assembles accepted ASM modules with pinned MASM,
builds the game library in image-derived order, lets LINK search the pinned
`MLIBCR.LIB`/`LIBH.LIB` for runtime members, then runs one LINK 3.65
(`/ST:8000`) and one EXEPACK. A zero-segment EXTDEF root keeps every reviewed
game and runtime library contribution in the link through normal LINK library
search. It compares the linked image, relocation
set/order, header and packed executable to the locked oracle and prints the
separate C, ASM, pinned-runtime, LINK-fill and raw-debt totals. Raw OMF debt
inputs are named `Dnnn.OBJ` and listed with their source ranges in the report.

Detailed logs, fresh compiler commands, raw-debt inventory, the DOS-side
`MAKEFILE`/response bundle and `report.json` are written under a unique
`build/histbuild/<run>/` directory. The generated NMAKE bundle reruns GAME.LIB
construction, LINK and EXEPACK from that directory. Image-derived
  ordering leaves a measured order residual: the independent build matches
  2,023 of 2,588 relocation positions (bank 0 is exact; banks 1–3 differ).
  The report gives per-bank counts and first member-order divergences. The
separate `python tools/validate.py --image` acceptance diagnostic uses the
oracle-derived order and checks the byte-exact full executable.

## Everyday work

```powershell
python tools/context.py --list rect
python tools/context.py rect_adjust_from_point
python tools/context.py rect_adjust_from_point --asm --callers --symbols
# Write self-contained C into your own build/workers/NAME/ directory.
python tools/search.py build/workers/NAME/candidate.c --function rect_adjust_from_point
# Repeat or pass several candidate files; compiler failures are useful evidence.
python tools/promote.py FUNCTION build/workers/NAME/candidate.c --verify-only
python tools/promote.py FUNCTION build/workers/NAME/candidate.c
# Several candidates, one transaction (each verified alone, one union whole-image build):
python tools/promote.py --batch build/workers/NAME/batch.txt [--verify-only]
python tools/validate.py
# Diagnostic real LINK 3.65 + EXEPACK rebuild (not a gate):
python tools/validate.py --image
# Library-search experiment against the pinned MLIBCR.LIB/LIBH.LIB (diagnostic):
python tools/reallink.py --order library --runtime libraries --library-order combined --game-order address
# Image-derived game member order (DGROUP data anchors; no relocation-table input):
python tools/reallink.py --order library --runtime libraries --library-order combined --game-order image
# Real link with a candidate composed as promotion would (BSS storage is accepted only when this places it):
python tools/reallink.py --tag NAME --stage FUNCTION build/workers/NAME/candidate.c build/workers/NAME/candidate.recipe.json
# Pinned runtime members (integ37: also data-only members, `module_form: data-only`),
# word-alignment / DOSSEG-lead LINK_FILL rows or (integ36) RUNTIME_STORAGE ownership
# updates, from a JSON list; runs the staged real link that must place all linked storage:
python tools/promote_runtime.py build/workers/NAME/rows.json [--verify-only]
# Communals (integ39): the whole c_common unit is one atomic batch, `COMMUNAL UNIT.json`
# plus every republished declaring TU; judge any link directory without publishing:
python tools/communal_unit.py dry-run build/commfit/stage/TAG --base-dir build/commfit/reallink/link
# Diagnostic communal-name solver (data under build/commfit/):
python tools/linkorder.py --run
python tools/commfit.py solve --inventory INVENTORY.json
python tools/stage_link.py --solution build/commfit/out/solution.json --tag TAG
```

For a reviewed assembly extent, use a self-contained MASM source and an explicit recipe with `"kind": "asm"`, `"profile": "masm510-game"`, `"assembler_flags": ["/Mx", "/I."]`, complete OMF declarations, FIXUPPs, and ordered relocations:

```powershell
python tools/search.py build/workers/NAME/candidate.ASM --function FUNCTION --recipe build/workers/NAME/candidate.recipe.json
python tools/promote.py FUNCTION build/workers/NAME/candidate.ASM --recipe build/workers/NAME/candidate.recipe.json --verify-only
```

Predict local BP homes with `python tools/slotorder.py --source src/copy_string.c --function copy_string`, or suggest names with `python tools/slotorder.py --names first second third`.
The same helper predicts explicit `register` SI/DI assignment in declaration order with `--registers si:index di:source`; BP homes still follow the independent identifier hash rule. Supply `--other-register-uses` when generated code also saves SI or DI.

For CODE record-cut and ordered relocation diagnostics, run `python tools/cut_simulator.py INPUT.json OUTPUT.json` with ordered members, instruction boundaries, fixups, and relocation sites.

Map candidate translation units with `python tools/tubench.py --map`, then compare a whole C source per member with `python tools/tubench.py SOURCE --tu ID` or `--interval START END`.
`tubench.py` includes reviewed function-boundary overlays and diagnoses TU-owned `_DATA`/`CONST` placements only when original code operands agree on one base and the complete initialized payload matches the image. A member with no outgoing near calls may be verified as a standalone source when its full object and bindings match; members using TU-owned data must include that data in the contribution.

Context reads the function inventory directly. `--history` expands local search observations; `--raw` shows underlying evidence. Search accepts standalone source, a pinned profile, and an optional `--recipe` for strict binding diagnostics. Full source files also permit small family/TU context hypotheses; their original grouping remains unknown. Search has no eligibility gate, attempt counter, or global workflow fingerprint. Its frozen source/environment and detailed compiler reports live under ignored `build/search/`.

Promotion accepts an existing recipe or an explicit `--recipe PATH`. Without either, it constructs a no-fixup recipe from independently mapped function boundaries. Contributions with fixups require a recipe using the tested binding modes and independently grounded symbols. A missing production capability does not prevent scratch investigation. No general linker or original TU reconstruction is implied.

Run complete validation at acceptance or tooling-change boundaries. Ordinary source iterations only need search. Optional low-level tools include `audit_function_extents.py` for bounded CFG questions, `inspect_object.py` for explicitly diagnostic OMF inspection, and `import_restunts.py` to reproduce the checked inventory from pinned references and reviewed overlays. These are evidence tools, not task-control commands.

## State and proof

`layout/manifest.json` is the sole ownership authority. Its active `recipes/` entries configure complete C contributions from `src/` and ASM contributions from tracked `asm/`; runtime owners carry pinned library/member/record policies directly. `layout/oracle.lock.json`, symbol bindings, reviewed function overlays, and `evidence/` supply independent facts. Reports under `build/` are derived and never confer ownership. Raw BSS debt is one reviewed placeholder per object (`evidence/bss-partition.json`), linked where that object's `_BSS` would be; `tools/communal_order.py`, `tools/msc_static_model.py` and the commfit solver (`tools/commfit.py`, `linkorder.py`, `stage_link.py`, `maxkeep.py`) are diagnostic naming aids only. The LINK c_common communals are accepted only as one whole unit (`tools/communal_unit.py`).

Promotion freezes the candidate, recompiles it, verifies its full contribution, then recompiles the entire staged hybrid image. It protects existing ownership, complete declarations, fixups, and exact ordered relocation obligations. The single publisher checks stable inputs, journals source/recipe/manifest writes, and freshly verifies the canonical image. `--verify-only` exercises the complete staged gate without changing canonical files. Validation also runs focused negative/isolation/transaction tests and the independent compiler backend.

After an interruption, `python tools/promote.py --recover` obtains the same OS lock and rolls back the journal. It refuses to overwrite conflicting user edits. Resolve any reported conflict against `build/publication.json`, then retry recovery and validation. The persistent `build/promotion.lock` file is harmless: the operating system releases its lock when a process exits. Do not delete a live publisher's journal or lock.

Program-wide public names (one per address) are recorded in `layout/names-registry.json`; `tools/namefit.py` checks that every accepted translation unit stays exact under them. Recovered C, matching ASM, pinned runtime (code and owned runtime data), and unresolved/raw bytes are reported separately by validation. See [technical scope](docs/acceptance.md), [selected compiler observations](evidence/compiler-notes.md), [toolchain hypothesis register](evidence/toolchain-hypotheses.json), and [migration results](MIGRATION.md).
