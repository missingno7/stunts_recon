# Provenance and reference use

## Locked references

- Empires reconstruction, branch `historical-exact-oracle-v1`, commit `c0a672d5daba74256aefa596e0b78f205d2b169d`. Local checkout HEAD was different, but the requested architecture files were checked against the exact branch and identical. Source: https://github.com/missingno7/empires_reconstruction/tree/historical-exact-oracle-v1
- Canonical Restunts, commit `5c38f258f482e7d28f1ccce73da8992dccbbee89`. Source: https://github.com/4d-stunts/restunts
- DSI format cross-reference, dstien/stunpack, commit `ee616c4b3bfa122d44f56782121b6dabe9504ecb`. Source: https://github.com/dstien/stunpack
- EXEPACK algorithm/reference, viiri/exepack, commit `a42f2c8fb1018feeb2c32fc7f7b1f01240d824c4`. Source: https://github.com/viiri/exepack ; upstream COPYING declares CC0.

`layout/references.json` stores source identities. `recovery/asset-identity.json` hashes every supplied asset. The supplied directory contains launchers/replays/tracks beyond the original core; it is not claimed to be authenticated original installation media. Only the four immutable MCGA input hashes establish this project target.

## Adapted code

`tools/mz.py` and `tools/omf.py` preserve Empires source, whose original file hashes are recorded in the reference lock. Empires' OMF reader was extracted from PortForge `pf_match.py`; its recorded original SHA-256 is `19107ce197169349aeb6758cb85facb577e26423db0ebe7e62cc6eb0e889a96d`. `object_probe.py` adapts Empires' `reconstruct.read_object` framing/checksum contract and adds stricter rejection. No license file was found in the pinned Empires tree; no new license is asserted over that upstream code.

The factory's lifecycle and explicit ownership derive from Empires, but its compact-model binder and specialized runtime queue were not reused as Stunts implementations. The simple DSI decoder is newly written from format evidence; it is not a translation of stunpack's optimized prefix-table implementation. Stunpack's upstream license is GPL-2.0-or-later. EXEPACK backward decoding follows the documented format and the CC0 reference's runout/length semantics.

Restunts asmorig/game.asm/game2.asm, IDC scripts, C sources, segment includes/status and replay harness were studied as evidence. **asmorig is not pristine:** `anders.idc` rewrites CRT instructions independently of the port switch, suppresses an environment initializer and adjusts assembly encodings. This is why source agreement is never the byte oracle. Semantic hypotheses for the two rectangle functions came from `c/math.c`; the far resource getter was derived from the mapped instructions. None establishes original authorship or original TU grouping.

## Historical tools

MSC5.10 tools were first obtained from Microsoft's own MS-DOS archive, commit `2d04cacc5322951f187bb17e017c12920ac8ebe2`: https://github.com/microsoft/MS-DOS . Its CL/C1/C1L/C2/C3/LINK files are identical to those in the complete archived distribution.

Complete MSC5.00 and MSC5.10 distributions came from PCjs disk images at `jeffpar/pcjs-miscdisks` commit `bd4cc85928f2291c8d08e610038a1dd70b94ac6c`. Every image/member was checked against archive metadata and recorded with SHA-256 in `recovery/msc500-provenance.json` and `recovery/msc510-provenance.json`. Original disk documentation: https://www.pcjs.org/software/pcx86/lang/microsoft/c/5.10/ . The user explicitly authorized compiler downloads. Downloaded binary tools remain ignored.

Runner: `C:/tools/msdos/msdos.exe`, ReC98 P0281, SHA-256 `f7f6cb0a3e816c5edb13112d327c1bddbf7463fe7bf9a005ca1eb5317751bd02`. Independent compiler parity used DOSBox-X; independent unpack parity used the pinned Restunts UNP with DOSBox. Receipts record exact runner/tool hashes and commands.

## Research coverage and remaining assumptions

Empires' reconstruction, module/TU probes, candidate checker, production builder/plan, promotion, frontier audit, grinder handover and both role guides were reviewed. Reused lessons: exact extent/public/fixup checks; ordered relocation preservation; input fingerprints; single-writer promotion; fresh acceptance; blocker memory. Rejected assumptions include fixed `_TEXT`, compact-model far calls, Turbo C, fixed header size and accepting partial data probes.

Restunts segment counts are source evidence, not original linker-object counts. Function names and C groupings remain semantic metadata. Startup, genuine ASM origin, original linker and toolchain version are still open. `docs/current/` is authoritative; archived scripts and narratives under `docs/history/` are not current status.
