# Replay regression boundary

Restunts' `src/restunts/repldump/repldump.c` creates a 16-bit frame count followed by raw GAMESTATE records after `input_do_checking(1)` and `update_gamestate()`. It initializes replay state, RNG, track and cars at the historical 20 FPS cadence. Its original-assembly harness writes `.BIN`; its ported-C harness writes `.BNI`.

This can later support one pinned replay corpus across historical harness, intermediate cleanup and SDL3 port. Record replay/track/car hashes, initial RNG/state, compiler build identity and state-schema version. Compare fields through an explicit serializer once host layouts differ.

Replay equality complements machine-byte equality. It does not validate pristine machine bytes, nor prove that a modified driver-integrated harness is the original executable. No replay harness or SDL work is implemented in this pass; keep that implementation behind the historical source boundary.
