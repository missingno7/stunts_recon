# Historical reconstruction rules

Read README.md and docs/current/supervisor-instructions.md before changing acceptance logic. Original assets and layout/oracle.lock.json are authoritative. Do not regenerate expected bytes after a mismatch. Keep matching source and any future modern build separate.

Never commit assets, downloaded compiler tools, generated EXEs/OBJs, or reference binaries. Use the pinned compiler profile and fresh whole-image acceptance for every promotion. Source/manifest promotion is single-writer. No matching claim may depend on ignored OMF records, masked fixups, object trimming, or final binary patching.

Run the relevant tests, tools/oracle.py verify, tools/build_exact.py verify, and tools/reconstruction_factory.py refresh before reporting current acceptance. Report raw/unknown bytes separately from recovered C, ASM and pinned runtime ownership.

Routine source work must follow docs/current/grinder-instructions.md and use tools/grind.py for durable attempts and promotion. Honor interval blockers and failure budgets; only a documented supervisor reopen starts a new epoch. Use the exact card path returned by the queue. Run tools/validate.py before handoff; it subsumes oracle/build/test/queue checks and independently recompiles active C. Do not hard-code current recovery totals in tests or evergreen instructions.
