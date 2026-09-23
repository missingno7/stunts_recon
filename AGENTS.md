# Historical reconstruction rules

When delegating source grinding, use GPT-6 Luna with high reasoning (`gpt-6-luna`, `high`). Keep source/manifest promotion single-writer. Controlled diagnostic comparisons must hold the agent model and reasoning setting constant across conditions.

For autonomous research and escalation, follow `docs/current/worker-research.md`. Keep detailed evidence local and handoffs compact. Workflow gates and research-budget stops are not model-capability failures; account for supervisor traffic in total model-weighted cost.

Routine startup: run the baseline validation once, select a task, then use `python tools/context.py FUNCTION_ID`. Expand `--asm`, `--callers`, `--globals`, `--history` or `--full` only as needed. The packet names omitted evidence and raw artifact paths. A stale packet cannot authorize an attempt.

Inspect the automatic compact match diagnosis before full assembly. Preserve byte-exact anchors and investigate the localized mismatch islands and realignment points. `--diagnosis` renders a concise view; expand the referenced full diagnostic only when evidence is insufficient. Check archived source/recipe/engine freshness. Structural similarity and unresolved operand normalization are diagnostic only; distinguish source codegen differences from binding, fixup and translation-unit blockers. History anchor losses are observations, never acceptance authority. Supervisor tooling checks may archive an unchanged-source probe with `probe_module.py NAME --supervisor-diagnostic`; this does not reopen a blocked task or authorize source experiments outside the grinder budget.

Read supported difference families and ungrouped residuals first; `--diagnosis --islands` expands the original localization. A consistent BP/register mapping is an observed operand pattern, not a proven source cause or repair. Check scope, contradictions, widths, segment/frame roles and unresolved symbols. Byte anchors are preservation evidence, not fixed C-line boundaries; inspect lost anchors rather than automatically forbidding a justified experiment. Record a machine-level prediction and falsifier before a source experiment, and distinguish clearer reporting, an experimentally supported explanation, and strict acceptance.

Read README.md and docs/current/supervisor-instructions.md before changing acceptance logic. Original assets and layout/oracle.lock.json are authoritative. Do not regenerate expected bytes after a mismatch. Keep matching source and any future modern build separate.

Never commit assets, downloaded compiler tools, generated EXEs/OBJs, or reference binaries. Use the pinned compiler profile and fresh whole-image acceptance for every promotion. Source/manifest promotion is single-writer. No matching claim may depend on ignored OMF records, masked fixups, object trimming, or final binary patching.

Run the relevant tests, tools/oracle.py verify, tools/build_exact.py verify, and tools/reconstruction_factory.py refresh before reporting current acceptance. Report raw/unknown bytes separately from recovered C, ASM and pinned runtime ownership.

Routine source work must follow docs/current/grinder-instructions.md and use tools/grind.py for durable attempts and promotion. Honor interval blockers and failure budgets; only a documented supervisor reopen starts a new epoch. Use the exact card path returned by the queue. Run tools/validate.py before handoff; it subsumes oracle/build/test/queue checks and independently recompiles active C. Do not hard-code current recovery totals in tests or evergreen instructions.
