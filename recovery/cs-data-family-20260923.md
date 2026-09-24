# CS-relative data family audit

Read-only audit, 2026-09-23. The queued verified inventory contains four other
CS-data contributions besides blocked `sub_35DE6`. This is reuse potential,
not a tested compiler or binder capability. Full local analysis is at
`build/private/cs-table-family/audit.md`.

| Task | Bytes | Pristine machine evidence | Scope |
|---|---:|---|---|
| `sub_35DE6` | 34 | `mov ax,cs; mov es,ax; lea di,[si+72A8h]; rep movsb` | Indexed CS destination; four prior isolated trials rejected ordinary near/far data access and the tested named based-segment forms. |
| `sprite_copy_both_to_arg` | 32 | `lea si,[5F20h]; mov ax,cs; mov ds,ax; rep movsw` | CS source at `5F20h`. |
| `sprite_copy_arg_to_both` | 28 | `mov ax,cs; mov es,ax; lea di,[5F20h]; rep movsw` | CS destination at `5F20h`. |
| `sprite_set_1_size` | 41 | Six explicit `cs:[5F2C/2E/30/32/3A/3C]` word stores | Fixed nearby CS fields. |
| `criterr_exithandler` | 30 | Explicit `cs:[0934h]` and `cs:[0936h]` word reads | Separate address area and interrupt semantics. |

The three sprite cases form the closest address-area cluster. The Restunts
`sprite1` and `incnums` labels are source hints; original TU, segment and
symbol bindings remain unproven. The existing `external-dgroup-offset16-v1`
binder cannot resolve a CS frame. A next useful discriminator requires an
independently mapped CS word and an evidence-backed C declaration/TU that
causes pinned MSC 5.10 to emit resolvable CS-frame OMF metadata. The current
triage records explicit CS access for the two string-copy cases; their `LEA`
constants are address expressions, not DGROUP reads.
