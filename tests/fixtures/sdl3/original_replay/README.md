# Original DOSBox-X selection and race references

The directory name is retained for test compatibility. All seven indexed frames
were newly captured from the locked original MCGA executable using DOSBox-X.
Each sidecar/manifest entry contains the complete sealed receipt and checkpoint
label; the payload is 64000 indices plus 768 DAC6 bytes. No legacy interpreter
capture or native image is a source.

Use the observer build from the parent README. For each row, run:

```powershell
python tools/porting/scenario_oracle.py SCENARIO --out build/workers/portclock/new-SCENARIO --dosbox build/workers/portclock/observer9/dosbox-x.exe --audio ad15 --ticks 4000
python tools/porting/pack_sdl3_reference.py build/workers/portclock/new-SCENARIO/original NAME=CHECKPOINT --out build/workers/portclock/new-selection
```

| Scenario | NAME=CHECKPOINT |
|---|---|
| startup | main-menu=menu |
| selection | car-countach=car-countach, car-lancia=car-lancia, track-default=track-default |
| opponent | opponent-clock=opponent-clock |
| manual-crash | race-driving=race-driving, race-result=race-result |

Pass multiple `NAME=CHECKPOINT` arguments separated by spaces, not commas. New
manifests are lists of the generated sidecar objects. Never generate expectations
from port captures. Selection tests compare named AD15 checkpoints directly.
PC15's Lancia pose remains an explicitly open animation-phase comparison; its
own original reference is preserved in the scoped scenario cases.
