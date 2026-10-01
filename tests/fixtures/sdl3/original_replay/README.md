# Original-interpreter replay frames

Seven `*.idxz` files contain exactly 64,000 mode 13h indices followed by 768
six-bit VGA DAC values, compressed with zlib. `manifest.json` and individual
sidecars identify the ArtifactV2 replay, its bound base snapshot, original game
program, canonical schema, semantic boundary, last named checkpoint, guest
state, and SHA-256 digests. They are original DOS-interpreter frames, not
screenshots or frames rendered by the native port. The decoder validates every
stored page and the complete 1 MiB memory fingerprint before extracting A0000.

The read-only source root is the `stunts_forged` reference checkout. With it at
`D:/Games/DOS/dos_recosystem/stunts_forged`, run from the repository root:

```powershell
$source = 'D:/Games/DOS/dos_recosystem/stunts_forged'
$out = 'build/workers/replay_oracle/reproduction'
python tools/porting/replay_oracle_probe.py --source-root $source --output-dir $out --replay rec_20260820_204801 --to 800 --label car-countach --build-v13 --compiler C:/msys64/mingw64/bin/g++.exe
python tools/porting/replay_oracle_probe.py --source-root $source --output-dir $out --replay rec_20260827_013957 --to 800 --label car-lancia
python tools/porting/replay_oracle_probe.py --source-root $source --output-dir $out --replay rec_20260813_035233 --to 7000 --label main-menu --v13-runner "$out/pf_dos_session_v13_391f1dd.exe"
python tools/porting/replay_oracle_probe.py --source-root $source --output-dir $out --replay rec_20260813_035233 --to 7600 --label race-driving --v13-runner "$out/pf_dos_session_v13_391f1dd.exe"
python tools/porting/replay_oracle_probe.py --source-root $source --output-dir $out --replay rec_20260813_035233 --to 9000 --label race-result --v13-runner "$out/pf_dos_session_v13_391f1dd.exe"
python tools/porting/replay_menu_branch.py --source-root $source --output-dir "$out/branches" --target track-default
python tools/porting/replay_menu_branch.py --source-root $source --output-dir "$out/branches" --target opponent-clock
python tools/porting/replay_oracle_probe.py --source-root $source --output-dir $out --replay "$out/branches/track-default.pfreplay.json" --to 800 --label track-default
python tools/porting/replay_oracle_probe.py --source-root $source --output-dir $out --replay "$out/branches/opponent-clock.pfreplay.json" --to 800 --label opponent-clock
```

The probe refuses to overwrite previous output. Each receipt's `raw` file can
be packed with `tools/porting/pack_replay_oracle_fixture.py` into a new fixture
directory. The receipt and manifest carry hashes for direct comparison with
these checked-in frames. `--to` is an exclusive boundary count, so 800 denotes
guest state after boundary ordinal 799. When a verified replay ends outside
mode 13h, the probe records only guest state and no indexed frame.

`main-menu`, `car-countach`, `car-lancia`, `race-driving`, and `race-result`
are unmodified corpus replays. The `track-default` and `opponent-clock`
artifacts are controlled branches of `rec_20260827_013930`: all input events
before occurrence 450 are copied, except the mouse press/release at occurrences
264 and 270. The original Drive coordinates `(0.5302083333333333,
0.7541666666666667)` become Track `(0.68, 0.5)` or Opponent `(0.125,
0.72)`. The generator records each as a new 450-frame ArtifactV2 with a
checkpoint every 20 frames. Each branch replay passed 21 original-interpreter
canonical comparisons with zero mismatches through boundary 800. The branch
generator reproduced both recorded branch file SHA-256 values exactly.

The v13 reference source is the preserved `port_forge/build/history/pf-391f1dd`
tree, compiled with its C++17/O2/Wall/Wextra/Werror flags. The current reference
runner is v14 and cannot verify v13 canonical records. Using the wrong replay
base can change copy-protection answers and gives misleading screen coverage;
the probe resolves each replay's declared bound snapshot. These fixtures cover
selected stable UI and race states, not every corpus boundary or the complete
guest transition sequence. Native comparisons need matching semantic state and
input history before a full-frame pixel comparison is meaningful.
