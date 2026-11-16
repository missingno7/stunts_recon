# Port-layer inventory and finite coverage

[port-layer.json](../../tools/porting/port-layer.json) is the joined review table
of native substitutions and adaptations, including enhanced-renderer helpers.
Unchanged reconstructed C is outside it; its width/layout adaptations are inside.

| View | Authority and scope |
|---|---|
| ASM replacements | `asm-migration.json`: 241 public entries in 40 reconstructed MASM modules, plus 92 pinned-runtime entries |
| Host services | All 113 `platform-boundary.json` DOS/BIOS, keyboard, mouse, VGA/port-I/O, timer and sound interfaces |
| Adapted C | Fresh `semantic_audit.py` historical/production-overlay join, including observation/input hooks |
| Build adaptations | Rewrite sites in `port/build.py`, adapters and delegated declaration/header/width transforms; source line and enclosing-function hash retained |
| Native helpers/macros | Existing function scanner plus recursive renderer/header helpers and explicit width/qualifier macros |
| Semantic contracts | All 29 scoped contracts, with authority, tests and finite claim limits preserved |

Rows overlap: their sum is not a count of independent DOS APIs. Existing
original-machine differential cases count as finite evidence. Lexical similarity,
compilation or registration alone do not. Every row says `exercised and agrees`,
`exercised and differs`, or `not exercised`. An agreeing row proves its named
bounded cases, never its entire input domain. Driver rows retain ordered-step
differences even when an identical request/return exists in an unordered call set.

The race observes 14 outermost foreground contracts: `intsin`, `intcos`,
`fixedmult`, `intatan`, `inthypot`, `mat_vec`, `transmult`, `zclipline`,
`mysetseeds`, `mygetseeds`, `myrandom`, `normkey`, `normkeyi`, `joystate`.
It also observes defined INT 15h C201/INT 33h register projections and 12 sound
far-entry offsets across AD15/PC15. The 14 routine contracts include bounded
before/after spans, alias offsets and defined returns. Private driver memory,
arbitrary DOS buffers, callback-internal decomposition and the remaining entries
are not newly covered. The step-0 clock-site inventory is linked by hash as
historical evidence; its old line numbers/shared-state model are not current claims.

Step 8 adds eleven optional raster/blit contracts; ten execute in the race's
first 22 frames. Their bounded observations include scalar arguments, vertex
lists, clip words, row 9, and hashes of the dashboard corner/source pixels.
27 real clipped polygon calls additionally run against original machine code.
The refreshed step-9 table has 1423 overlapping rows: 92 with finite agreeing
evidence, seven differing raw ordered sound streams, and 1324 without comparable
coverage. New typed `savefilez` and `instringz` cases cover error returns, line
buffer spans and marker XY; a separate data row covers all 257 MSC ctype bytes.
Scope-specific semantic-contract rows now inherit their named evidence. Full
results-music/replay transition streams are still open, even where a bounded
race sound projection agrees. Unmatched operands never count as passes.

Every remaining row carries `reachability.runtime_entry`, `scenario` and `reason`.
False means no standalone guest entry (build rewrite/macro). Null means runtime
reachability is not established: the suggested scenario/probe is a triage route,
not a fabricated call graph. Missing comparable ABI/span capture does not mean a
function never ran. Pinned MSC routines need a native semantic mapping/direct
machine test; merely replaying a native scenario cannot call the absent MSC entry.
Extract the complete per-row remaining list (1324 entries), without truncation:

```powershell
python -c "import json; r=json.load(open('tools/porting/port-layer.json')); [print(x['id'], json.dumps(x['reachability'])) for x in r['rows'] if not x['exercised']]" > build/workers/portclock/step9/unexercised.txt
python tools/porting/semantic_audit.py --work build/workers/portclock/step9/audit
python tools/porting/port_layer.py --audit build/workers/portclock/step9/audit/report.json --comparison tools/porting/step_oracle_cases/race-boundary-results.json --comparison tools/porting/step_oracle_cases/scenario-results.json
```

The compact result files contain paired comparison counts, not candidate-derived
expected values. Original executable/observer/log seals remain in the corresponding
original fixtures. [Findings](step-divergences.md) distinguish fixed defects,
finite agreeing cases, CPU-dependent observations and still-unproven sound state.
