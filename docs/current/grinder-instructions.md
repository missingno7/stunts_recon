# Routine grinder workflow

Run `python tools/validate.py` when taking over the repository and before a handoff. It verifies the oracle and pinned tools, runs all tests, reconstructs the entire image, independently recompiles active C under DOSBox-X, checks previous promotions still own their bytes, refreshes the queue and audits current cards. MS-DOS Player at `C:/tools/nmlgcdos/msdos.exe` is the current compiler/LINK runner; `layout/toolchain.json` pins its hash. Use the bundled Python path in README if Python is not on PATH.

`docs/current/status.json` is generated ownership/queue status. `validation.json` records a completed validation and its fingerprints; it is not proof that later edits were tested. The default validation includes independent parity. `--no-independent` explicitly records that check as NOT_RUN.

## Pick and prepare

```text
python tools/reconstruction_factory.py next
python tools/context.py FUNCTION_ID
```

Selection chooses CHEAP first, then MEDIUM, and never silently selects SUPERVISOR. Context accepts that stable ID or exact name and follows the queue's card path. Use `--asm`, `--callers`, `--globals`, `--history` and `--full` for explicit expansion. Old unreferenced card files are not current tasks. A MEDIUM task still needs a C hypothesis and reviewed recipe; CHEAP means prerequisites are supported, not that the source matches.

Check disassembly, source evidence, risks and `attempts_remaining`. Odd extent is an alignment risk, not proof of assembly origin. External jumps, calls, CS-relative storage, missing global-address evidence and unverified boundaries belong to supervisor work. Do not make a candidate look eligible by renaming it or changing its machine interval.

Write an ordinary C hypothesis under `recovery/candidates/NAME.c`, then:

```text
python tools/prepare_candidate.py load_XXXXX --source recovery/candidates/NAME.c
python tools/reconstruction_factory.py refresh
python tools/grind.py attempt NAME --hypothesis "What source choice this tests"
```

Use the stable ID and semantic name from the card. Preparation requires verified boundaries, a wholly raw-owned extent, and no active interval block. Review the generated recipe before refreshing. Refresh acknowledges the current tool/layout/evidence baseline; it must not be used to disguise out-of-scope changes.

Default recipes accept no fixups. A supervisor must establish external DGROUP addresses and review complete object declarations/fixups before enabling that supported binding mode; see `data-binding.md`. No target addresses or expected bytes are learned from a mismatching compiler object.

The selected larger-function path also supports reviewed zero-addend external pointer32 far CALLs with target frames. Code publics come from pinned library ownership and an independent pristine frame anchor. Ordered source relocations are checked separately from the hybrid header. Other call modes and multi-public production remain blocked. See `workflow-canary.md` before extending a recipe.

## Iterate or promote

`grind.py` archives each source, hypothesis, compiler log, recipe, object identity and available mismatch diagnostics under `recovery/attempts/NAME/NNNN/`. It refreshes cards after recording the result.

- **FAST_PASS_ONLY:** complete contribution matches; production has not changed.
- **FAILED:** inspect the compact mismatch summary first. Preserve exact anchors; form the next justified hypothesis around the local islands, their evidence and explicit realignment. Check complete size/fixups and distinguish codegen evidence from binding/TU problems. Edit only that candidate source. Do not refresh after an ordinary source-only edit; the card deliberately permits it.
- Unequal lengths include unbound instruction alignment with unresolved operand fields marked. Equal lengths that bind but differ include a bound-payload diagnosis plus separate object evidence. Neither normalization nor structural similarity counts as exact acceptance. Full length and byte equality remain mandatory.
- **ERROR:** fix the toolchain, source-scope or environment problem. Infrastructure errors do not spend source-attempt budget.
- **Blocked:** stop this task. Unsupported objects/binding escalate immediately; three failed hypotheses exhaust the budget. Identical hypotheses are refused without recompiling. Different source with identical object output is identified in the report.

To promote a FAST match:

```text
python tools/grind.py promote NAME --hypothesis "Promote the complete contribution proven by FAST"
python tools/validate.py
```

Promotion freshly recompiles FAST and both staged/canonical complete images under a serial writer lock. It checks exact expected input fingerprints and workflow controls, rolls back failed metadata changes, and refreshes the queue. No test-total or documentation edits are needed after promotion. Independent parity consumes active manifest recipes only; failed candidates do not become production tests.

`check_candidate.py` remains a compatible CLI but routes through the same ledger. `probe_module.py` and `toolchain_probe.py` are research diagnostics, not promotion commands or a way to bypass budgets.

## Read local mismatch evidence

Default `context.py FUNCTION_ID` automatically includes the latest bounded `match_diagnosis`: archived strict status, supported operand families, contradictions and scope, corresponding anchored branch destinations, unresolved fixup symbols, ungrouped residuals, exact anchor ranges, source/recipe/engine freshness, and full artifact identity/path. A newer observation without a summary is explicitly listed. `context.py FUNCTION_ID --diagnosis` prints the concise text view. Reports use half-open byte and instruction ranges relative to each contribution. Counts include the entire linear decode, including alignment NOPs; an anchored percentage is not semantic correctness or a reachable-instruction count.

Read the families and independent residuals first. `--diagnosis --islands` expands the original island view; open the referenced full JSON or use `--asm` only if needed. The full JSON holds both decoded streams, every aligned pair, all observation sites, and contradictions. Grinder attempts copy it durably beside `report.json`; the report and derived index carry bounded summaries without instruction arrays. `--history` expands prior evidence; comparable diagnostics can show lost exact target ranges. Truncation is explicit; regression comparison requires matching engine, target and comparison mode and a complete current anchor summary. These observations never decide acceptance.

Distinguish three claims: decoded operand differences; supported grouping of those observations; a source explanation tested by a controlled compiler experiment. Only the last can justify a causal source claim, and it still cannot replace strict acceptance. A BP correspondence is not evidence for a missing local or even a local-versus-parameter assignment. Register patterns exclude segment/frame roles and retain access widths; implicit effects exposed by the pinned decoder are recorded, with a conservative XLAT barrier for its known missing effects. No liveness or global register-renaming proof is claimed. Structural branch destinations are uncertain; even identical relative bytes can reach different destinations after insertion. Fixup normalization never establishes the intended symbol or whole contribution.

Preserve exact regions as evidence, not immutable C-line boundaries: declaration types, allocation and TU context can affect distant instructions. Before compiling, record the predicted machine-level change and what would falsify it. Existing repeated-output experiments and blockers still apply. Tooling/grouping improvements alone do not reopen a blocked source family. The measured small Stunts corpus and remote case-study limitations are in `docs/current/near-match-families.md`.

Unique four-instruction sequences of at least eight bytes seed deterministic monotonic alignment. Local bounded edit alignment connects anchors; register/stack substitutions are operand evidence, not source-level explanations. Tiny repeated epilogues are not independent anchors. Byte equality, decoded equality, structural similarity and unresolved fixup normalization stay separate. Diagnostic errors cannot turn a strict probe failure into a pass.

For supervisor tooling verification of an unchanged candidate, `python tools/probe_module.py NAME --supervisor-diagnostic` freshly compiles and archives a failure under `recovery/diagnostics`, then refreshes context. It never promotes, spends a source attempt or reopens a blocker. Do not use it for unbudgeted source trials. An extent mismatch exits with failure even when its diagnostic is useful.

## Supervisor re-entry

After identifying concrete new evidence or adding a tested capability:

```text
python tools/grind.py reopen NAME --reason "New evidence or tested capability that changes the previous blocker"
```

This records a new epoch and preserves all earlier sources/reports. Interval blocks follow the original bytes across names; reopen the original blocked task rather than using a renamed alias. Reopening does not waive capability checks, stale recipe mapping, oracle identity or full-image acceptance.

Never trim compiler output, hide raw bytes in C/ASM, patch objects/final images, or change the oracle lock. Do not delete a writer lock blindly after interruption: inspect its process and any partially changed source/manifest, restore a consistent state, then validate. Parallel agents may perform read-only research; all source/layout promotion remains serial.
