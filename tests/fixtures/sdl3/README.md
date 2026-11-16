# Original DOSBox-X indexed-screen references

Every `.idxz` contains 64000 original mode-13h indices followed by 768 DAC6
palette bytes, compressed with zlib. Sidecars seal the original executable,
observer, scripts, bindings and raw capture logs. No native image is an expectation.
Splash/title payloads happen to be unchanged; their provenance was recaptured.
Menu cursor placement and animated-car checkpoints are now explicitly scripted.

Build the observer and regenerate into fresh scratch, from the repository root:

```powershell
python tools/porting/dosbox_observer/build.py --out build/workers/portclock/observer9
python tools/porting/scenario_oracle.py startup --out build/workers/portclock/new-startup --dosbox build/workers/portclock/observer9/dosbox-x.exe --audio ad15 --ticks 1000
python tools/porting/pack_sdl3_reference.py build/workers/portclock/new-startup/original splash=splash title=title menu=menu --out build/workers/portclock/new-screens
python -m unittest tests.test_sdl3_framebuffer -v
```

`--replace` is required to overwrite references, and still requires a sealed
original capture. The packer never accepts native frames. Tests compare the named
checkpoints, not an image found elsewhere in a run. See
[selection/race references](original_replay/README.md) and
[runtime oracle](../../../docs/porting/runtime-oracle.md).

`startup-seed.json` is a prescribed shared startup-boundary input vector, installed
and observed in the original. Its numeric controls are retained; no claim is made
that its old status-read counts measure a real machine's entropy or speed.
