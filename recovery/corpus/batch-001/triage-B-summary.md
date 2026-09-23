triage-B covered 12 supervisor cases plus exact controls indices 3-4; source, promotion, recipe, tool, card, queue and layout files were not changed.
Three explicit higher-level blockers: load_272f4 needs MZ relocation binding; load_2327f needs CS-relative storage binding; load_20883 requires hardware/interrupt review.
Named but unregistered data facts: g_kevinrandom_seed is six bytes at 0xa1da-0xa1df; putsign tests word_428A2 at 0x7132; keyboard restore reads four old-vector words at 0x43d8-0x43de; preRender_patterned writes dispatch pointers and word_4031E. These remain mapping/review gaps unless unsupportedness is separately demonstrated.
load_20404 has a verified 90-byte extent but no complete packet decode; its listing shows callbackflags lookup and an indirect far callback.
load_22886 has dispatch setup, two prologue paths and an external jump whose targets/storage need reconciliation.
load_1637a is 8 bytes over 76, but current diagnosis lacks matched source/recipe/engine identities and an artifact path; cause remains unknown.
parse_shape2d and run_menu are partial mappings with four and five label mismatches; vec_normalInnerProduct and transformed_shape_add_for_sort have no image address anchor despite named source proc/endp listings.
Next-level candidates: restore label-to-image diagnostics for parse_shape2d/run_menu; locate unique byte anchors for the two unmapped helpers; reconcile source-backed DGROUP addresses; supervisor-review relocation, CS-relative, and interrupt representation.
Both controls match frozen promotion-record hashes; stored UNIT.C/OBJ hashes and staged/canonical full-build fingerprints agree. oracle.lock independently pins the 200000-byte load image. This is historical receipt parity, not fresh build validation or a blind replay claim.
No triage finding authorizes source experiments or promotion; only control evidence is complete with existing capabilities.
