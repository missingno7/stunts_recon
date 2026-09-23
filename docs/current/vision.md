# Project vision

The historical recovery build aims for exact machine-code reproduction.
The later SDL3 port will be derived from recovered source, not mixed into the matching source during recovery.

“Recovered original source” means a historically plausible source tree reproducing the original program exactly. It does not mean unavailable original comments, local variable names, formatting, or original translation-unit names have been recovered.

The active target is the pristine Stunts 1.1 MCGA core before the documented crack and before audio-driver integration. EGA/CGA/Tandy and dynamically loaded drivers are outside this matching target.

Production consumes only owners listed in `layout/manifest.json` and `layout/production-plan.json`. Matching C is emitted afresh by a pinned historical compiler. Known runtime modules come from pinned libraries. All remaining image bytes are explicitly raw-owned, including data not yet represented by historical source. No inline byte blobs, C byte arrays, altered expected bytes, compiler-object patching, or final-image patching may substitute for recovery.

Keep matching source historically shaped. A future separate modern target may introduce SDL3, wider types and portable timing/rendering interfaces without changing the historical proof.
