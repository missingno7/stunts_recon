# SDL3 indexed-screen references

These fixtures were packed from PortForge `.pfidx` captures. Each `.idxz`
contains 64,000 palette indices followed by the 768 authored RGB6 palette
bytes, compressed with zlib. The JSON sidecar records the original PortForge
capture identity and checksums. No image conversion, tolerance, or pixel mask
is used.

The references were captured by PortForge from two replays: checkpoints 300
and 600 in `build/indexed_oracle_intro`, and checkpoint 1200 in
`build/palette_cld1_audit`. To repack those captures, run the documented
packer with these source files:

```powershell
Set-Location D:\Prog\stunts_sdl3
python tools/porting/pack_sdl3_reference.py --out tests/fixtures/sdl3 `
  splash=D:\Games\DOS\dos_recosystem\stunts_forged\build\indexed_oracle_intro\checkpoint_000000000300.pfidx `
  title=D:\Games\DOS\dos_recosystem\stunts_forged\build\indexed_oracle_intro\checkpoint_000000000600.pfidx `
  menu=D:\Games\DOS\dos_recosystem\stunts_forged\build\palette_cld1_audit\checkpoint_000000001200.pfidx
```

The startup replay fingerprint is
`8f82ed91295f9abe7d3529b3426c1db1f0389a6bdcd91c192421e659d6d25da0` for
both startup captures; the menu replay fingerprint is
`7d00ee2bc87a49974f4402ca0b60a4c49ac2b46f927dd34abc6e3d4a3ce9ead4`.
If recapturing PFIDX files, use PortForge's `scripts/indexed_frame_capture.py`
with those same replays and verify the fingerprints, source-file hashes, and
compressed payload hashes recorded in the sidecars.

The menu image is checkpoint 1200 from the existing Port Forge audit replay.
Its in-game cursor overlaps the selected "Let's Drive" outline. The game code
draws the cursor through `mouse_draw_transparent` when input coordinates
change, and draws the magenta outline for selected button 0; this is software
game output, not the host OS cursor. The equivalent SDL capture enters the
menu with Enter and moves the logical pointer to (208,189). The matching
capture is `build/sdl3/m1b-menu-aligned/frame-000016.fbr`.

To compare runtime captures, set `STUNTS_SDL3_CAPTURE_DIR` to the directory
holding the splash/title captures and `STUNTS_SDL3_MENU_CAPTURE_DIR` to the
menu capture directory, then run `python -m unittest tests.test_sdl3_framebuffer -v`.
