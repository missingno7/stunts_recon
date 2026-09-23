# First-pass validation report

The repository is initialized and the end-to-end matching-C milestone is achieved. This remains a hybrid reconstruction, not a complete source recovery or original-linker reproduction.

## Oracle identity

Target: supplied Stunts 1.1 MCGA core, before crack and before driver integration.

| Representation | Bytes | SHA-256 |
|---|---:|---|
| Canonical packed MZ | 199125 | `f57ac2775b6156b0413775eb61a1c48743f64248f72d13ca970dc645029eb41d` |
| Canonical unpacked MZ | 210384 | `1adb8259b3f6634062b94826e1f167649d9f2deeb6cedc9989127fc37e9ce615` |
| Pristine load image | 200000 | `4fc3340e2dc5d139a95a9a92278ef09902003d2c6857e5d9cdf0112870f3789e` |

All 2588 ordered relocation entries agree with independent UNP/DOSBox output. The decoded 200000-byte core differs from the pinned cracked Restunts reference only at the documented crack byte. Packed header padding and unpacked MZ metadata are explicitly canonical/reconstructed; see oracle.md.

## Results

- Full fresh hybrid acceptance: **HYBRID_EXACT**.
- Tests: **44 passed** (standard-library unittest), including fresh real compiler and library construction, real far-call OMF parsing, negative records, scope and stale-input failures.
- Imported procedures: 619; mapped functions: 235 (234 importer extents plus containment proven by compiler output).
- Source code segments: 38; source data segments: 3; stack segments: 1; independently proven code frames: 25.
- Matching C: **140 bytes**, three genuine game functions. Matching ASM: **0 bytes**.
- Runtime identified: **725 bytes**; production-bound runtime: **420 bytes**, nine complete pinned modules.
- Raw initialized image: **199440 bytes**. Confirmed raw executable minimum: **18991 bytes**; exact raw executable total remains unknown.
- Queue: **CHEAP 0 / MEDIUM 20 / SUPERVISOR 596**.

| Promoted function | Bytes |
|---|---:|
| rect_is_overlapping | 62 |
| rect_is_inside | 52 (includes 1 alignment NOP) |
| audioresource_get_dword | 26 |

Each promotion ran FAST, a fresh staged full construction, then a fresh canonical full construction. All three also match under independent DOSBox-X compilation. The rectangle pair reproduces its entire 114-byte contiguous contribution in one TU, but original complete TU boundaries remain unproven.

Disjoint classification evidence: **18826 code / 21968 data / 725 library / 158481 unknown bytes**. This conservatism prevents raw data/code ambiguity from being hidden in recovery totals.

## Compiler and remaining blockers

Production uses pinned **Microsoft C 5.10, /AM /O /Gs**. Microsoft C5.00 also matches all three accepted functions; /Ox also matches. Compiler-family/memory-model compatibility is demonstrated across two game areas and different constructs, but a unique historical compiler version or option set is not proven. No required compiler download is currently missing; complete MSC5.00/5.10 and bundled QuickC1.00/1.01 are local. QuickC2.x remains an optional untested hypothesis.

Open work: general medium-model fixup binding; original TU/linker layout; QuickC BAKPAT; three runtime helpers with overlapping LEDATA; startup checksum anomaly; unresolved function boundaries; ASM origins; complete code/data classification. The current supported subset rejects these cases instead of weakening acceptance.

## Continue

With Python on PATH (or the bundled interpreter path in README):

```text
python tools/oracle.py verify
python tools/build_exact.py verify
python tools/reconstruction_factory.py refresh
python tools/reconstruction_factory.py next --tier MEDIUM
python tools/toolchain_probe.py experiment --task rect_is_overlapping --profile msc500-medium
python tools/probe_tu.py recovery/tu/rectangle-pair.c --profile msc510-medium --target recovery/tu/rectangle-pair-target.json
```

Read the selected bounded card and grinder-instructions.md before creating a candidate recipe. `--promote` is only for a fresh FAST-passing candidate with reviewed scope. No SDL work was introduced.

References: Empires `c0a672d5daba74256aefa596e0b78f205d2b169d`; Restunts `5c38f258f482e7d28f1ccce73da8992dccbbee89`. All source/tool/asset provenance is in layout and recovery JSON. Original assets and downloaded binaries are ignored, and no proprietary binary is tracked.
