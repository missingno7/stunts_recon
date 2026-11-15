# Archived replay fixtures

The older ArtifactV2 fixture readers remain for provenance and file-format
checks. Their live interpreter build/record/replay commands are retired.
They are not the original-game runtime reference for the current port.

Use [the DOSBox-X observer](../../docs/porting/runtime-oracle.md) to regenerate
original-game input, sound-driver, frame and simulation observations. The
observer mounts scratch copies of the immutable assets and executes the locked
original MCGA image. Existing historical snapshots are not refreshed from the
port or silently relabelled as DOSBox-X captures.
