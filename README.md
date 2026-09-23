# Stunts 1.1 MCGA historical reconstruction

This repository reconstructs the supplied MCGA core from original assets, then gradually replaces explicit raw regions with byte-exact historical compiler/library output. It does not yet reconstruct the entire program from source. No SDL work belongs in this tree during recovery.

Current recovery totals and fresh acceptance are generated in the linked status files. The bounded grinder loop is established; broader linker/TU work remains explicit supervisor work. See [current status](docs/current/status.md), [machine status](docs/current/status.json), and [handover](docs/current/grinder-instructions.md).

## Run locally

Python 3.10+ is sufficient for construction/tests. Importing Restunts evidence additionally uses pinned Capstone 5.0.3 in `build/python`.

On this machine, Python is bundled with Codex:

```powershell
$py = 'C:\Users\Jiri\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
& $py tools/validate.py
& $py tools/reconstruction_factory.py next
& $py tools/context.py FUNCTION_ID
```

With Python on PATH, use the same commands prefixed by `python`.

Context automatically shows compact mismatch islands and exact anchors when an archived diagnosis exists. Use `python tools/context.py FUNCTION_ID --diagnosis` for a concise text view, then consult the referenced full artifact if needed. Preserve exact anchors while investigating local differences; diagnostic similarity never relaxes byte/fixup acceptance.

`assets/`, `toolchain/`, `build/` and references are ignored. Keep your original files there; no game or compiler binary is tracked. `layout/oracle.lock.json` has no automatic update command. Verification never learns new expected bytes.

## Reading order

Routine work starts with the function packet above and [grinder instructions](docs/current/grinder-instructions.md). The following documents are deeper references, not a requirement to read the whole project on every task. See [the implemented workflow/canary pass](docs/current/workflow-canary.md) for measurements and the remaining larger-function mismatch.

1. [Vision and source claims](docs/current/vision.md)
2. [Oracle and coordinate systems](docs/current/oracle.md)
3. [Provenance](docs/current/provenance.md)
4. [Compiler fingerprint](docs/current/toolchain-fingerprint.md)
5. [Factory boundaries](docs/current/architecture.md)
6. [Grinding readiness](docs/current/readiness.md)
7. [Supervisor handover](docs/current/supervisor-instructions.md)

The original MCGA bytes are the only byte oracle. Restunts supplies structural and semantic evidence. A PASS means the exact current hybrid image matches, including ordered relocation entries; raw-owned bytes remain recovery debt.
