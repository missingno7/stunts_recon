# Remaining uncertainties

This is the list of uncertainties that still have a practical consequence. The machine-exact oracle and strict gate are complete; there are no class F freeze blockers. Class A items are genuine historical/runtime evidence gaps; class E items are semantic unknowns that can remain opaque while bytes and extents stay preserved.

## Historical fidelity

- **Original compiler attribution and translation-unit/link recipe (A).** The canonical matching profile is pinned Microsoft C 5.10 medium model with `/AM /O /Gs`, but accepted functions do not distinguish MSC 5.00 from 5.10. Original TU grouping, general LINK response, arbitrary frame/binding modes, and natural module/library order are not recovered. `histbuild.py` reproduces the image and the relocation order through a combined-library link whose member order is taken from the image (README, Historical build); that order is an assumption, not a recovered fact. Evidence: `evidence/toolchain-hypotheses.json`, `README.md` Historical build, `freeze.md`.

## Port-only compile baseline

The GCC `compat` and `strict-central` probes both pass 38/38 active C sources for syntax and object compilation after the port-only declaration/adapter fix. These checks compile host objects only and do not resolve runtime dispatch or execution behavior. Evidence: [port headers](porting/port-headers.md), `build/porting/host-probe/{compat,strict-central}/results.json`.

## Faithful portable runtime

- **DOS timer and interrupt environment (A).** Port Forge v7 measures timer deadlines at about 99.9985 Hz in the captured runs, and the active race selected a 20 Hz game target. This does not identify the inherited real-DOS PIT channel-0 control word, saved INT 8 chain counts, runtime IVT contents, keyboard IRQ counts, or callback totals across DOS configurations. The 10 Hz game target was not selected in these workloads. Evidence: [runtime oracle](porting/runtime-oracle.md), `docs/porting/runtime-model.md`.
- **Presentation and input device traces (A/D).** The pause/resume workload proves that the guest frame counter stops while timer, video and audio publications continue. PortForge sampled `FrameStart` only; it did not measure `VerticalRetraceStart`, per-frame retrace/palette port traffic, BIOS mouse INT 15h C201h service details, or before/live/after vector state. Evidence: `docs/porting/CAPTURE.md`, `docs/porting/platform-boundary.md`.
- **Audio driver runtime ABI (A).** Static caller/vector structure is mapped and game-audio publication batches were counted, but those batches are not host callbacks. The exact runtime driver selection, hardware port traffic, `.DRV` callee cleanup and opaque record semantics are not established. Evidence: `docs/porting/audio-drivers.md`, `docs/porting/runtime-oracle.md`.
- **Unindexed software-service reachability (A/E).** Three routines in `rpmrand` issue INT 60h/61h/62h without setting registers. No indexed C caller or provider contract is known; indirect reachability remains possible. Keep their accepted bytes and calls intact until a provider/path is identified. Evidence: `docs/porting/platform-boundary.md`, `docs/porting/asm-migration.md`.

## Future modern renderer

- **Opaque renderer/format semantics (E).** Some Shape2D fields and `!MGA` palette mapping, Shape3D/P3S flags and SIMD fields, exhaustive terrain codes/trailers, VLE encoding/padding, and several fallback-format grammars lack a complete independent sample/contract. Preserve full records, index bytes and current draw behavior; do not derive object sizes from maximum observed reads. Evidence: `docs/porting/formats.md`, `docs/porting/asm-migration.md`.
- **Renderer entry details (D/E).** A few sprite/pixel operations have known surface reads/writes but their complete operation labels or indirect-entry boundaries remain tentative. An accepted renderer migration still needs byte-range and caller evidence at the exact port boundary. Evidence: `docs/porting/platform-boundary.md`.

## Optional semantic archaeology

- **State and track meaning (E/C).** Some globals and physical units remain unknown; aggregate offset views are incomplete; `build_obj` pairs track tables differently from setup/reference naming. Keep exact object layouts and current bindings. Evidence: `docs/porting/state-model.md`, `docs/porting/formats.md`.
- **Unexercised bytes and absent formats (E).** Supplied replay controls never set bits 6–7, font byte indices do not prove character encoding, HIG byte `0x10` is opaque, VCE records have driver-specific opaque tails, and no `.CKM`/`.TD` sample/reader or several fallback-format samples are available. Preserve the complete bytes/records and round-trip them without assigning guessed meanings. Evidence: `docs/porting/formats.md`.
- **Opaque audio and UI data (E).** Uncalled driver slots, MT-32 patch/event payload meanings, editor/dialog text encoding, and some unnamed pixel-operation roles remain unknown; they are preserved in original resources or exact code. Evidence: `docs/porting/audio-drivers.md`, `docs/porting/modes-and-editor.md`, `docs/porting/asm-migration.md`.

## Port implementation tasks are tracked separately

Host file/resource/audio dispatch hooks, a bounded segment:offset resolver, SDL3 subsystem adapters, and SDL3 trace emission are implementation work; they are listed in [the porting guide](porting/README.md), not treated as unknown facts about the historical image.
