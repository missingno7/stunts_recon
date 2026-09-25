# Stunts 1.1 MCGA historical reconstruction

This workspace replaces explicit raw regions of the original MCGA image with byte-exact historical compiler and runtime output. `HYBRID_EXACT` means the complete hybrid executable and ordered relocations match the immutable oracle. It does not mean the entire program has been recovered from source.

## Setup

Use Python 3.10+ on Windows. Keep original `MCGA.HDR`, `EGA.CMN`, `MCGA.DIF`, and `MCGA.COD` under ignored `assets/`. The hash-pinned Microsoft C distributions and MS-DOS Player paths are in `layout/toolchain.json`; provisioning provenance is in `evidence/msc500-provenance.json` and `evidence/msc510-provenance.json`. The local extraction helper is `toolchain/extract_pcjs.py`. Do not commit game or compiler binaries.

Install the diagnostic decoder with `python -m pip install --target build/python capstone==5.0.3`. Evidence checkouts and commit/file identities are recorded in `layout/references.json`; Restunts resides at `build/references/restunts`. Keep that checkout, its reference executable, and other ignored local inputs. They cannot be restored from this repository's Git history.

Full validation independently recompiles active C under DOSBox-X at `C:/DOSBox-X/dosbox-x.exe`. Configure local runner paths explicitly when moving machines, preserving pinned compiler identities. No verification command updates the byte oracle.

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
python tools/validate.py
```

Predict local BP homes with `python tools/slotorder.py --source src/copy_string.c --function copy_string`, or suggest names with `python tools/slotorder.py --names first second third`.

Map candidate translation units with `python tools/tubench.py --map`, then compare a whole C source per member with `python tools/tubench.py SOURCE --tu ID` or `--interval START END`.

Context reads the function inventory directly. `--history` expands local search observations; `--raw` shows underlying evidence. Search accepts standalone source, a pinned profile, and an optional `--recipe` for strict binding diagnostics. Full source files also permit small family/TU context hypotheses; their original grouping remains unknown. Search has no eligibility gate, attempt counter, or global workflow fingerprint. Its frozen source/environment and detailed compiler reports live under ignored `build/search/`.

Promotion accepts an existing recipe or an explicit `--recipe PATH`. Without either, it constructs a no-fixup recipe from independently mapped function boundaries. Contributions with fixups require a recipe using the tested binding modes and independently grounded symbols. A missing production capability does not prevent scratch investigation. No general linker or original TU reconstruction is implied.

Run complete validation at acceptance or tooling-change boundaries. Ordinary source iterations only need search. Optional low-level tools include `audit_function_extents.py` for bounded CFG questions, `inspect_object.py` for explicitly diagnostic OMF inspection, and `import_restunts.py` to reproduce the checked inventory from pinned references and reviewed overlays. These are evidence tools, not task-control commands.

## State and proof

`layout/manifest.json` is the sole ownership authority. Its active `recipes/` entries configure complete contributions from `src/`; runtime owners carry pinned library/member/record policies directly. `layout/oracle.lock.json`, symbol bindings, reviewed function overlays, and `evidence/` supply independent facts. Reports under `build/` are derived and never confer ownership.

Promotion freezes the candidate, recompiles it, verifies its full contribution, then recompiles the entire staged hybrid image. It protects existing ownership, complete declarations, fixups, and exact ordered relocation obligations. The single publisher checks stable inputs, journals source/recipe/manifest writes, and freshly verifies the canonical image. `--verify-only` exercises the complete staged gate without changing canonical files. Validation also runs focused negative/isolation/transaction tests and the independent compiler backend.

After an interruption, `python tools/promote.py --recover` obtains the same OS lock and rolls back the journal. It refuses to overwrite conflicting user edits. Resolve any reported conflict against `build/publication.json`, then retry recovery and validation. The persistent `build/promotion.lock` file is harmless: the operating system releases its lock when a process exits. Do not delete a live publisher's journal or lock.

Recovered C, matching ASM, pinned runtime, and unresolved/raw bytes are reported separately by validation. See [technical scope](docs/acceptance.md), [selected compiler observations](evidence/compiler-notes.md), [toolchain hypothesis register](evidence/toolchain-hypotheses.json), and [migration results](MIGRATION.md).
