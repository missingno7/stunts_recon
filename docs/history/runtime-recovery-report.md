# Runtime and signed-long recovery pass

This pass replaces **337 additional raw bytes** with verified production contributions:

| Contribution | Bytes | Evidence |
|---|---:|---|
| `nopsub_26552` | 32 | Ordinary signed-long absolute-value C; complete MSC5.00/5.10 output matches |
| `ldiv.asm` | 156 | Complete pinned LIBH member, including ordered LEDATA overwrite |
| `lmul.asm` | 52 | Complete pinned LIBH member and both public aliases |
| `uldiv.asm` | 97 | Complete pinned LIBH member, including ordered LEDATA overwrite |

Each promotion performed FAST, a fresh staged full-image construction, then a fresh canonical construction. The new C routine differs from the pinned runtime archive's `labs.c` implementation; no library provenance is asserted for it. All twelve currently identified library contributions are now bound. Source ownership remains distinct from library ownership and ASM origin.

Historical compilation and LINK use **`C:/tools/msdos/msdos.exe`** as the primary runner. Eight tests linked the untouched arithmetic members, individually and together, with the linkers from both MSC distributions. All complete code extents, publics/aliases and zero-relocation obligations agreed. Explicit per-member hashes and ordered-record policies support these overlaps; default overlap rejection, checksum enforcement and unknown-record rejection remain intact. See [library binding](library-binding.md).

The initial 17-byte `sub_307D2` candidate failed three bounded hypotheses: MSC emitted SUB AH,AH instead of the original XOR AH,AH and added a NOP beyond the target extent. It is archived in `recovery/blocked/sub_307D2.json` and excluded from routine retry. No trimming or opcode replacement was used.

Promotion auditing exposed a canonical-write race. Both C and library promotion now predict the exact final input fingerprint, check it before the canonical build, and verify the returned fingerprint afterward. Regression tests cover unrelated edits during metadata writes and rollback after canonical failure. Explicit UTF-8/LF JSON serialization makes those predicted file hashes portable.

## Current validation

**69 tests passed.** The fresh complete hybrid MZ and all 2588 ordered relocations match the unchanged oracle. Independent DOSBox-X compilation reproduces all seven C contributions and their binding obligations. Both pinned compiler profiles verify, the 114-byte rectangle TU experiment remains exact, and full-build/independent-runner input snapshots agree.

| Metric | Current |
|---|---:|
| Full-image acceptance | HYBRID_EXACT |
| Matching C | 206 bytes / 7 functions |
| Matching ASM | 0 bytes |
| Runtime identified / production-owned | 725 / 725 bytes |
| Raw initialized bytes | 199069 |
| Confirmed raw executable minimum | 18620 bytes |
| Exact raw executable total | Unknown |
| Mapped / imported functions | 235 / 619 |
| Imported code segments / proven frames | 38 / 25 |
| Code / data / library / unknown classification | 18826 / 21968 / 725 / 158481 bytes |
| CHEAP / MEDIUM / SUPERVISOR queue | 0 / 16 / 593 |

The target remains the supplied Stunts 1.1 MCGA core before crack and driver integration. Canonical header padding and reconstructed unpacked metadata remain documented in [oracle construction](oracle.md).

| Representation | Bytes | SHA-256 |
|---|---:|---|
| Canonical packed MZ | 199125 | `f57ac2775b6156b0413775eb61a1c48743f64248f72d13ca970dc645029eb41d` |
| Canonical unpacked MZ | 210384 | `1adb8259b3f6634062b94826e1f167649d9f2deeb6cedc9989127fc37e9ce615` |
| Load image | 200000 | `4fc3340e2dc5d139a95a9a92278ef09902003d2c6857e5d9cdf0112870f3789e` |

Production pins MSC5.10 `/AM /O /Gs`; MSC5.00 also matches all seven C functions. Unique historical version/flags and complete TU layout remain unproven. General far/self-relative binding, other frame methods/displacements, QuickC BAKPAT, archived CRT startup checksum anomalies and unresolved code/data boundaries remain open. No compiler download blocks the supported subset.

## Continue

The next generated MEDIUM card is `load_20a44`, `nopsub_kb_set_readchar_callback`, 17 bytes. Inspect complete-contribution alignment and data fixups before spending its bounded attempt budget. General linker/TU work remains the higher-value supervisor frontier.

```powershell
$py = 'C:\Users\Jiri\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $py tools/reconstruction_factory.py next --tier MEDIUM
& $py tools/runtime_link_probe.py
& $py tools/oracle.py verify
& $py tools/build_exact.py verify
& $py -m unittest discover -s tests -v
& $py tools/crosscheck_runner.py
& $py tools/reconstruction_factory.py refresh
```

Earlier pass narratives are preserved under `docs/history`; this report and generated status describe current production ownership.
