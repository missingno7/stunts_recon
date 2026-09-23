# Supervisor handover

The bounded grinder workflow is established; see `readiness.md`, generated status and validation receipts. The project remains an early hybrid reconstruction, with general linking and most code recovery open.

Next priorities:

1. Extend the bounded medium-model binder beyond proven external DGROUP offset16 target-frame fixups. Twelve historical LINK differentials now cover both compiler versions, frame alignment and encoded addends. Test far calls, near self-relative references, other frame methods, nonzero target displacements, segment aliases and ordered relocation obligations against actual MSC objects before enabling them. See `data-binding.md`. Do not use Empires' caller-frame shortcut.
2. Establish original TU/segment contribution relationships from relocated call frames, segment/group/public metadata and multi-function compilation. Restunts segNNN and modern C files are hypotheses only. The rectangle pair has a repeatable 114-byte full-contribution TU probe; remaining TU and segment context is unproven.
3. The three `LIBH` long-arithmetic helpers are now bound (305 bytes); eight historical LINK experiments prove their exact ordered-record semantics. Preserve the default overlap rejection and pinned member/record policies (`library-binding.md`). Next audit the startup COMENT checksum anomaly and extend runtime identification; do not disable checksum checks globally.
4. Support QuickC BAKPAT with format-grounded tests, or continue rejecting it. Compare versions using discriminating real functions. MSC5.0 and5.1 are both exact on current matches; neither the FAQ nor one lucky match proves 5.1 uniquely.
5. Expand fully mapped functions from the current inventory and classify unknown initialized bytes. The 38 imported source code segments are not yet an original TU count. Avoid claiming a total raw-code count until code/data boundaries are complete.

Refresh and validate after changes:

```text
python tools/import_restunts.py
python tools/validate.py
```

The importer needs the pinned Restunts checkout and Capstone5.0.3 (`python -m pip install --target build/python capstone==5.0.3`). Repository/source hashes must agree with `layout/references.json`. The oracle and production build do not need Restunts binaries or Capstone.

`layout/toolchain.json` pins local compiler directories and the MS-DOS Player runner. When moving machines, reconfigure paths explicitly while preserving hashes, then rerun both compiler and independent runner checks. Never copy proprietary tools into tracked source. Complete download provenance is stored under recovery; local extraction helper is `toolchain/extract_pcjs.py`.

Source promotion is serial. Parallel work may inventory/research reference bytes and toolchains, but must not mutate production ownership concurrently. Preserve user files. Current status is generated under `docs/current`; old experiments belong under `docs/history` or recovery receipts.
