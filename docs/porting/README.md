# Porting documentation

This index collects compatibility contracts and source/image evidence for the Stunts 1.1 MCGA port. The inventory is planning documentation; it does not implement replacement services.

- [Platform boundary inventory](platform-boundary.md): DOS, BIOS, input, timer, audio, memory, and indexed video-surface boundaries, with observed behavior and unresolved coverage gaps.
- [Structured platform boundary data](platform-boundary.json): machine-readable entries, caller leads, confidence, and evidence locations for the same inventory.

- [Asset formats](formats.md): observed resource/archive layouts, evidence strength, and sample validation; parser output is generated under `build/porting/`.
- [Audio drivers and banks](audio-drivers.md): `.DRV` far-call vectors, observed hardware paths, VCE/KMS/SFX layouts, and open ABI details.
- [Runtime model](runtime-model.md): startup, simulation, input, replay, timing and rendering behavior, with superseded editor/audio claims corrected.
- [Modes and built-in editor](modes-and-editor.md): menu transitions, track-editor loop, placement/validation, save flow, race setup and replay selection.

The parsers are read-only with respect to `assets/`. Run `python tools/porting/format_reference.py assets --report build/porting/asset-validation.json` and `python tools/porting/audio_bank.py --verify-all --json build/porting/audio-bank-validation.json`; both generated reports stay under ignored `build/`.
