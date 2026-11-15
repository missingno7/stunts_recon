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
| Semantic contracts | All 25 existing contracts, with authority, tests and finite claim limits preserved |

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

```powershell
python tools/porting/semantic_audit.py --inventory-only --work build/workers/portclock/step7/audit-frozen
python tools/porting/port_layer.py --audit build/workers/portclock/step7/audit-frozen/report.json --comparison build/workers/portclock/step7/race-ad15-final/comparison.json --comparison build/workers/portclock/step7/race-pc15-final/comparison.json
```

[Race findings](step-divergences.md) separate measurements, fixes, attribution
leads and unexercised work. Remaining ABI/span review and opponent, crash, replay,
editor, dialog and high-score captures are further work, not a whole-port PASS.
