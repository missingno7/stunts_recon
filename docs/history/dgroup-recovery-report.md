# DGROUP recovery pass

Three additional game-C functions are now production-owned: `nopsub_378AE` (14 bytes), `nopsub_378BC` (14 bytes), and `audio_enable_flag2` (6 bytes). Each went through FAST, a fresh staged full-image build, and a fresh canonical build. Their object files contain real external data fixups; the new bounded binder resolves them from independently established DGROUP addresses. Full object declarations and ordered fixups are reviewed recipe obligations. No object trimming, raw-byte C or final executable patching is used.

The original relocated startup proves DGROUP at load coordinate 0x2B770. Two recovered array references point into startup-cleared BSS beyond the stored image. These addresses are explicit symbol facts, not additional initialized-data ownership. See [binding evidence](data-binding.md).

Validation: **56 tests passed**, including fresh historical compiler/linker tests and full hybrid reconstruction. Twelve historical LINK differential fixtures pass across MSC5.00/5.10, two data alignments, and three encoded addends. Both compiler versions reproduce all six recovered functions. Independent DOSBox-X compilation reproduces all six complete bound contributions. Oracle, both toolchain profiles, full build and independent-runner input snapshots agree. Independent parity now invalidates stale success before a new run and verifies unchanged inputs before publishing.

| Metric | Current |
|---|---:|
| Full-image acceptance | HYBRID_EXACT |
| Matching C | 174 bytes / 6 functions |
| Matching ASM | 0 bytes |
| Runtime identified / production-owned | 725 / 420 bytes |
| Raw initialized bytes | 199406 |
| Confirmed raw executable minimum | 18957 bytes |
| Exact raw executable total | Unknown |
| Mapped / imported functions | 235 / 619 |
| Imported code segments / proven frames | 38 / 25 |
| Code / data / library / unknown classification | 18826 / 21968 / 725 / 158481 bytes |
| CHEAP / MEDIUM / SUPERVISOR queue | 0 / 18 / 595 |

The immutable MCGA oracle is unchanged:

| Representation | Bytes | SHA-256 |
|---|---:|---|
| Canonical packed MZ | 199125 | `f57ac2775b6156b0413775eb61a1c48743f64248f72d13ca970dc645029eb41d` |
| Canonical unpacked MZ | 210384 | `1adb8259b3f6634062b94826e1f167649d9f2deeb6cedc9989127fc37e9ce615` |
| Load image | 200000 | `4fc3340e2dc5d139a95a9a92278ef09902003d2c6857e5d9cdf0112870f3789e` |

All 2588 ordered relocation entries remain exact. This is an early hybrid, not complete source recovery. Production pins MSC5.10 `/AM /O /Gs`; the historical version and exact option set remain ambiguous. Far/self-relative binding, other frames/displacements, original TU layout, QuickC BAKPAT, overlapping library LEDATA and unresolved classification remain open. No compiler download is blocking the supported subset.

Continue with the generated queue; the next MEDIUM card is `load_207d2`, `sub_307D2`, 17 bytes. It is a research candidate, not a promised match.

```powershell
$py = 'C:\Users\Jiri\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $py tools/reconstruction_factory.py next --tier MEDIUM
& $py tools/oracle.py verify
& $py tools/build_exact.py verify
& $py -m unittest discover -s tests -v
& $py tools/crosscheck_runner.py
& $py tools/reconstruction_factory.py refresh
```

The earlier first-pass narrative is preserved under `docs/history/first-pass-report.md`; current status and handover documents reflect this pass.
