# Stunts 1.1 MCGA platform boundary inventory

**Status:** evidence inventory for SDL3 port planning; no replacement code.

> **Audio-driver correction:** the INT 60h/61h/62h rows below remain unresolved software-interrupt sites in the game image; they do not describe the loaded audio-driver interface. P3a traced the four `.DRV` images and their callers to direct far-call vectors at offsets from the loaded image base. See [audio-drivers.md](audio-drivers.md); keep the unrelated interrupt providers and contracts marked unknown.

## Scope and method

Indexed routines with direct DOS/BIOS/interrupt/port access and routines dereferencing the game sprite surface that can target A000 video memory. Ordinary game far pointers are out of scope unless they cross this platform boundary. Evidence documentation only; no replacement implementation.

Addresses below are image load offsets. Callers are reverse edges from evidence/functions.json, mostly source_semantic_only, so they are lead lists rather than a machine-level complete call graph. An empty caller list does not rule out callbacks/indirect calls. Every row has image and source evidence paths in platform-boundary.json. Apparent I/O opcodes were checked against source bodies because inline data can decode as instructions.

## Rendering surface contract

- `_sprite1` and `_sprite2` begin at A000:0000, pitch 320, dimensions 320x200; line offsets are y*320. The visible extent is 64,000 bytes (0xFA00). Window descriptors can instead own heap buffers.
- Mode 13h is indexed 320x200; palette changes use BIOS INT 10h AX=1012h. Use an SDL indexed surface, preserve pitch/clipping, exact AND/OR and transparency rules, and separate borrowed display storage from owned windows.
- `video_clear_color` performs 64,000 STOSW operations (128,000 byte writes) with 16-bit DI wrap; do not silently treat it as a 64,000-byte fill.

## dos / exception

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x009EC9-0x009EE8` | `intr0_handler` | IVT vector 0 / IRET  -  Records saved AX and CS:IP, advances saved IP by two bytes, then IRETs; this skips a 2-byte divide fault instruction. | none indexed; indirect possible | Use an explicit arithmetic error path or bounded exception bridge with the same recovery state. | high structure; recovery is instruction-size-sensitive |
| `0x009EE8-0x009F12` | `init_div0` | INT 21h AH=35h/25h AL=00h  -  Gets and saves old INT 0 vector, installs intr0_handler through DS:DX. Called by main startup. | ported_stuntsmain_ | Install/restore a process-local divide-error policy around legacy execution. | high |
| `0x01F35C-0x01F377` | `criterr_interrupt_handler` | INT 24h handler / IRET  -  Calls registered far callback and returns its AL decision with IRET. | none indexed; indirect possible | Use synchronous file-error callback with identical action values. | high |
| `0x01F377-0x01F3BC` | `set_criterr_handler` | IVT INT 24h; INT 21h AH=25h  -  Stores far callback, saves current INT 24h vector from IVT 0000:0090h, installs handler and exit callback. | init_main | Map DOS critical-error choices to explicit filesystem errors; preserve callback AL result and restore prior hook. | high |
| `0x01F3BC-0x01F3DA` | `criterr_exithandler` | INT 21h AH=25h AL=24h  -  Restores saved INT 24h vector on exit. | none indexed; indirect possible | Unregister the host compatibility error hook at shutdown. | high |

## dos / file

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x01DEAE-0x01DF28` | `_lseek` | INT 21h AX=4200h/4201h/4202h  -  BX=handle, CX:DX=signed displacement, AL=BOF/current/EOF origin; DX:AX is resulting offset. | __flsbuf | Map DOS origins to host seek, preserve signed 32-bit offset and error mapping. | high; pinned CRT |
| `0x01DF28-0x01E052` | `_write` | INT 21h AH=40h  -  Writes CX bytes from DS:DX to DOS handle BX, including CRT device/short-write handling. | __flsbuf, _fflush | Use host descriptor writes preserving handle, count, short-write and errno behavior. | high; pinned CRT |
| `0x01FE82-0x01FE94` | `file_paras` | INT 21h open/seek/close  -  Thin entry into paragraph-size query; result is ceil(file bytes/16). | file_decomp_fatal, file_load_binary_nofatal | Use host file size and retain paragraph rounding if callers depend on it. | high |
| `0x01FE94-0x01FEA5` | `file_paras_nofatal` | INT 21h open/seek/close  -  Nonfatal paragraph-size query wrapper. | none indexed; indirect possible | Preserve the recoverable error/result convention. | high |
| `0x01FEA5-0x01FF26` | `file_paras_fatal` | INT 21h AH=3Dh/42h/3Eh  -  Opens read-only, seeks EOF then BOF (AX=4200h,CX:DX=0), closes, returns ceil(size/16); errors call fatal_error. | none indexed; indirect possible | Use host metadata; preserve fatal path and paragraph rounding. | high |
| `0x01FF26-0x01FF38` | `file_decomp_paras` | INT 21h open/read/close  -  Thin entry to first-four-byte decompressed-size query; returns ceil(header size/16). | file_decomp_fatal | Parse the same size header and paragraph units. | high |
| `0x01FF38-0x01FF49` | `file_decomp_paras_nofatal` | INT 21h open/read/close  -  Nonfatal first-four-byte decompressed-size query. | none indexed; indirect possible | Preserve header parsing and recoverable errors. | high |
| `0x01FF49-0x01FFD4` | `file_decomp_paras_fatal` | INT 21h AH=3Dh/3Fh/3Eh  -  Opens, reads four-byte size header, closes, returns ceil(size/16); fatal on failure. | none indexed; indirect possible | Use host streams; preserve size header, paragraph rounding, and fatal errors. | high |
| `0x01FFD4-0x02002E` | `file_find` | INT 21h AH=1Ah/4Eh  -  Sets DTA to DS:406Ah; find-first uses CX=6 and DS:DX pattern, copies found path/name to shared result buffer. | ensure_file_exists, file_combine_and_find, file_load_shape2d, load_tracks_menu_shapes, loop_game, ported_stuntsmain_ | Implement DOS wildcard/attribute enumeration and preserve shared result format/lifetime. | high |
| `0x02002E-0x020044` | `file_find_next` | INT 21h AH=1Ah/4Fh  -  Sets same DTA and advances DOS wildcard search. | file_find_next_alt | Continue compatible host enumeration and filtering. | high |
| `0x020AD0-0x020AE0` | `file_read` | INT 21h AH=3Dh/3Fh/3Eh  -  Thin far-destination file-read entry. | file_decomp_fatal, file_load_binary_nofatal | Adapt host reads to segmented destination spans and preserve return pointer convention. | high |
| `0x020AE0-0x020AEF` | `file_read_nofatal` | INT 21h AH=3Dh/3Fh/3Eh  -  Nonfatal reader; core reads 0x4000-byte chunks and advances far destination segment by 0x0400 paragraphs. | sub_29A86 | Keep segmented-buffer progression and recoverable error behavior. | high |
| `0x020AEF-0x020B62` | `file_read_fatal` | INT 21h AH=3Dh/3Fh/3Eh  -  Opens read-only; reads CX=0x4000 (16 KiB) chunks into DS:DX, increments destination segment by 0x0400 paragraphs per chunk, closes; fatal on error. | file_load_replay, load_tracks_menu_shapes, ported_stuntsmain_, run_tracks_menu | Use host stream plus segmented-buffer adapter; preserve cleanup and fatal behavior. | high |
| `0x0224FA-0x02250B` | `file_write_nofatal` | INT 21h AH=3Ch/40h/3Eh/41h  -  Creates/truncates (CX=0), writes 0x4000-byte chunks from far source, advances segment by 0x0400 paragraphs; closes and deletes partial file on error. | none indexed; indirect possible | Preserve overwrite, far-span traversal, short-write and partial-file cleanup semantics. | high |
| `0x02250B-0x0225AE` | `file_write_fatal` | INT 21h AH=3Ch/40h/3Eh/41h  -  Fatal wrapper for 16 KiB far-source writes; deletes partial output before fatal_error. | file_write_replay, highscore_write_a, highscore_write_b, load_tracks_menu_shapes | Use host file APIs with matching overwrite, cleanup and fatal behavior. | high |

## dos / memory

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x01D154-0x01D1B6` | `__myalloc` | INT 21h AH=4Ah  -  Resizes PSP-owned DOS block for CRT environment/heap setup. | __setenvp | Use host environment/allocator setup; remove PSP block ownership. | high; pinned CRT |
| `0x01E222-0x01E290` | `_brkctl` | INT 21h AH=48h  -  Allocates BX DOS paragraphs (16-byte units), returns segment AX or DOS failure; CRT brk path. | __amallocbrk | Back CRT allocation with host pages and convert paragraph units consistently. | high; pinned CRT |
| `0x01E290-0x01E2E6` | `sub_2E290` | INT 21h AH=4Ah  -  Resizes DOS block identified by ES to BX paragraphs. | _brkctl | Resize corresponding host allocation through CRT compatibility layer. | high; pinned CRT |
| `0x02107A-0x0210F1` | `mmgr_alloc_resmem` | INT 21h AH=30h/62h/48h/4Ah  -  Queries DOS/PSP, allocates 0x64 paragraphs (1600 bytes), resizes DOS block to caller segment boundary, records resource arena bounds/chunk metadata. | mmgr_alloc_a000, nopsub_310FE | Use host resource arena with paragraph alignment and explicit ownership; preserve caller-visible boundary. | high machine behavior; boundary intent needs callers |
| `0x02111D-0x021157` | `nopsub_3111D` | INT 21h AH=48h  -  Allocates requested BX paragraphs (retries allocation on failure), stores arena start/end and initializes chunk list. | none indexed; indirect possible | Allocate 16*BX bytes in host resource arena with compatible error handling. | high |

## dos / runtime

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x01CC62-0x01CDEC` | `start` | INT 21h AH=30h/4Ah/35h/25h/44h/4Ch; INT 20h  -  CRT startup checks DOS version, resizes PSP block, checks standard handles, installs INT 0, calls stuntsmain, exits via DOS. | none indexed; indirect possible | Replace PSP/startup services with host process setup while preserving startup gates and exit status. | high; pinned CRT |
| `0x01CE03-0x01CE4A` | `libsub_quit_to_dos` | INT 21h AH=3Eh/4Ch  -  Closes runtime-tracked handles, restores vectors, exits to DOS; called by abort/raise. | _abort, _raise | Close host handles and unwind hooks before process exit. | high; pinned CRT |
| `0x01CE4A-0x01CE77` | `sub_2CE4A` | INT 21h AH=25h  -  Restores saved INT 0 and optionally another saved vector during CRT shutdown. | libsub_quit_to_dos | Restore compatibility hooks from host cleanup stack. | high; pinned CRT |
| `0x01D129-0x01D154` | `__NMSG_WRITE` | INT 21h AH=40h, BX=2  -  Writes counted runtime diagnostic to DOS stderr handle 2. | __FF_MSGBANNER, __nullcheck, _abort, start | Write the same byte count to host stderr and map errors. | high; pinned CRT |
| `0x01E40C-0x01E48C` | `_int86` | Generated INT n / RETF stub  -  Runtime reads input words AX/BX/CX/DX/SI/DI at offsets 0..0Ah; writes those registers and carry result at output+0Ch. INT 25h/26h use special stack-return stub. Mouse C declares only four words: storage extent beyond 8 bytes is unresolved. | mouse_get_state, mouse_init, mouse_set_minmax, mouse_set_pixratio, mouse_set_position, nopsub_36A9A, nopsub_36ACA | Use typed host service calls; define/validate full register-frame extent for remaining BIOS-compatible calls. | high runtime offsets; C storage extent uncertain |
| `0x01E5AA-0x01E63C` | `_raise` | INT 23h; INT 21h AH=4Ch  -  Invokes DOS Ctrl-C vector then exits; called from abort. | _abort | Map to host signal/termination policy and preserve game cleanup. | high; pinned CRT |
| `0x01E67C-0x01E71F` | `_signal` | INT 21h AH=35h/25h AL=23h  -  Gets or sets DOS INT 23h handler. | none indexed; indirect possible | Map Ctrl-C registration to host signal handling. | high; pinned CRT |
| `0x01EAD4-0x01EADE` | `sub_2EAD4` | CLI/STI  -  Disables interrupts around shared-state mutation, then re-enables; called by add-value/state helpers. | set_add_value, sub_2EB07, sub_2EB1E | Use host atomic/lock only if these operations can race; retain reentrancy semantics. | medium: state meaning opaque |
| `0x020FA9-0x02107A` | `locate_sound_fatal` | INT 20h fallback  -  Searches resource table for four-character sound id and computes far address; after fatal_error, INT 20h terminates if it returns. | nopsub_36826 | Resolve resource id and raise/return explicit fatal resource error; do not depend on INT 20h. | high for fallback; path is indirect |

## input / joystick

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x0205C8-0x0205FC` | `nopsub_305C8` | IN 201h  -  When joystick active, reads 201h, inverts/masks active-low button bits 30h, combines keyboard flags 10h/20h; no full axis measurement. | none indexed; indirect possible | Map gamepad buttons to same logical bits and combine keyboard state. | high |
| `0x0205FC-0x0207B4` | `get_joy_flags` | IN/OUT 201h; CLI/STI  -  Samples active-low buttons, writes sample to 201h to start RC timing, polls X/Y lines up to 0xFA0 loop iterations, smooths/calibrates to direction bits 1/2/4/8 and buttons 10h/20h. Unit is CPU loops, not time. | do_joy_restext, get_kb_or_joy_flags, input_checking, nopsub_304B6 | Map SDL axes/buttons to same bit contract and calibrated thresholds; do not treat 4000 loops as fixed duration. | high structure; timing CPU-dependent |

## input / keyboard

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x020404-0x02045E` | `kb_parse_key` | CLI/STI  -  Converts BIOS key word via callback/translation state protected by interrupt reentrancy guard. | kb_get_char, nopsub_304B6 | Map SDL events to same logical actions and callback order. | high |
| `0x0204B6-0x020519` | `nopsub_304B6` | INT 16h AH=01h/00h  -  Checks BIOS key availability, consumes AX word when present, parses it, then can fall through to combined input. | none indexed; indirect possible | Translate host events to same legacy key word/no-key convention. | high |
| `0x020519-0x020538` | `kb_get_char` | INT 16h AH=01h/00h  -  Checks BIOS keyboard and consumes key AX if present, then calls kb_parse_key. | input_checking, run_game | Preserve ASCII/scan mapping and consume semantics. | high |
| `0x020812-0x020883` | `kb_init_interrupt` | IVT INT 09h/16h; PIC 21h  -  Masks IRQ0/1, saves/replaces vectors 9 at IVT 24h/26h and 16h at 58h/5Ah, clears 0x5A scan-state bytes, restores PIC mask, registers exit callback. | init_main | Use SDL events and preserve scan state/query semantics without vector hooks. | high |
| `0x020883-0x0208C6` | `kb_exit_handler` | IVT INT 09h/16h; PIC 21h; BDA 40:17  -  Masks IRQ0/1, restores saved vectors, clears high modifier nibble at 40:17, restores old PIC mask. | ported_stuntsmain_ | Unregister host event hooks and reset modifier state. | high |
| `0x0208C6-0x0209A5` | `kb_int9_handler` | IN 60h/61h; OUT 61h/20h; IRQ1 IRET  -  Reads scan code 60h, pulses bit 7 at 61h, tracks make/break and shift/ctrl/alt maps, queues BIOS-style key words, EOIs PIC 20h and IRETs. | none indexed; indirect possible | Translate SDL key events into identical scan/key/modifier queue and overflow behavior. | high |
| `0x0209A5-0x020A0D` | `kb_int16_handler` | Replacement INT 16h; CLI/STI  -  Implements BIOS AH=00h dequeue, AH=01h peek, AH=02h modifiers using game queue. | none indexed; indirect possible | Expose same query API in host keyboard layer without interrupt installation. | high |
| `0x020A21-0x020A35` | `kb_read_char` | INT 16h AH=01h/00h  -  Nonblocking BIOS read; returns AX=0 when no key. | do_joy_restext | Return zero when host event queue is empty; else compatible key word. | high |
| `0x020A35-0x020A44` | `kb_checking` | INT 16h AH=01h  -  Non-consuming key-available test; zero when absent; normalizes AH when ASCII exists. | kb_shift_checking1, kb_shift_checking2 | Host non-consuming predicate with same return shape. | high |
| `0x020A68-0x020A77` | `kb_check` | INT 16h AH=01h/00h  -  Consumes key words, skipping zero-ASCII/extended-only entries until usable key. | do_joy_restext | Preserve skip policy at call boundary. | high |

## input / mouse

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x0268AA-0x0268D2` | `mouse_set_pixratio` | INT 33h AX=0Fh CX=xratio DX=yratio  -  Sets mickeys-per-pixel ratios; init uses 16:16. | mouse_init | Convert host mouse motion to same logical pixel deltas. | high |
| `0x0268D2-0x02695C` | `mouse_init` | INT 15h AX=C201h; INT 33h AX=0/7/8/0Fh  -  Calls INT 15h C201h (other input regs unstated; service purpose uncertain), then INT 33h reset. If driver indicates present, stores button count, x scale=1 only at width 320, sets [0,width-1]x[0,height-1], ratio 16:16, cache FFFFh. | init_main | Use SDL mouse availability and preserve extents, scaling, button mask and initial cache. | high except INT 15h purpose uncertain |
| `0x02695C-0x0269B0` | `mouse_set_minmax` | INT 33h AX=7/8  -  Sets X limits CX=xmin<<scale,DX=xmax<<scale; sets Y limits CX=ymin,DX=ymax. | mouse_init, mouse_minmax_position | Clamp host pointer to equivalent game coordinates with 320-wide X scaling. | high |
| `0x0269B0-0x0269E4` | `mouse_get_position` | INT 33h AX=3  -  Returns BX buttons, CX X shifted down by scale, DX Y; updates shared state. | none indexed; indirect possible | Return host button mask and game-pixel coords with same scaling. | high |
| `0x0269E4-0x026A0E` | `mouse_show_cursor` | INT 33h AX=1  -  Reference-counted show; invokes driver on transition to visible. | none indexed; indirect possible | Preserve nested show/hide behavior in host cursor layer. | high |
| `0x026A0E-0x026A2C` | `mouse_hide_cursor` | INT 33h AX=2  -  Decrements show count and hides at zero. | none indexed; indirect possible | Preserve nested cursor visibility behavior. | high |
| `0x026A2C-0x026A60` | `mouse_set_position` | INT 33h AX=4 CX=x<<scale DX=y  -  Warps driver cursor to game position with X scaling. | mouse_minmax_position | Warp/clamp host pointer or update logical pointer consistently with capture mode. | high |
| `0x026A60-0x026A9A` | `mouse_get_state` | INT 33h AX=3  -  Returns BX button mask, CX scaled X, DX Y through output pointers. | input_checking, replay_unk2, run_game | Return same host button and game-coordinate values. | high |
| `0x026A9A-0x026ACA` | `nopsub_36A9A` | INT 33h AX=7  -  Sets X limits after shifting args right by mouse scale, opposite mouse_set_minmax left shift. | none indexed; indirect possible | Resolve this asymmetry before unifying the range API. | uncertain: inconsistent scale direction |
| `0x026ACA-0x026AF2` | `nopsub_36ACA` | INT 33h AX=8  -  Sets Y limits directly from caller CX/DX. | none indexed; indirect possible | Use same logical Y range in host input layer. | high |

## software_interrupt

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x009DFF-0x009E09` | `nopsub_19DFF` | INT 61h  -  Invokes INT 61h without setting registers; service/provider and register contract unknown. | none indexed; indirect possible | Identify the installed provider or reproduce its intended service before removing this call. | uncertain: owner and purpose unknown |
| `0x009E09-0x009E13` | `nopsub_19E09` | INT 60h  -  Invokes INT 60h without setting registers; service/provider and register contract unknown. | none indexed; indirect possible | Identify the installed provider or prove this path inert before replacing it. | uncertain: owner and purpose unknown |
| `0x009E13-0x009E21` | `nopsub_19E13` | INT 62h  -  Invokes INT 62h without setting registers; service/provider and register contract unknown. | none indexed; indirect possible | Identify the installed provider or prove this path inert before replacing it. | uncertain: owner and purpose unknown |

## timer / audio

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x0201A0-0x020268` | `timer_setup_interrupt` | PIT 43h/40h; PPI 61h; PIC 21h; IVT IRQ0  -  Default DX=2E9Ch (11932), divider globals=5. Clears speaker gate bits at 61h; writes B6h to 43h (channel 2, mode 3, LSB/MSB), masks IRQ0/1, saves/installs IVT vector 8, unmasks, then writes DL/DH to 40h (channel 0). Command/data mismatch is observed. Conditional on channel0 divisor 11932 at 1,193,182 Hz: ~100.015 Hz raw, ~20.003 Hz every-five callback cadence; effective rate uncertain. | init_main | Use host monotonic scheduler; preserve callback logical cadence only after timing validation. Do not reproduce the mismatched PIT programming. | high sequence; rate conditional |
| `0x020268-0x0202AA` | `audio_stop_unk` | IVT IRQ0; PIC 21h; PIT 40h; PPI 61h  -  If vector still owned, masks IRQ0/1, restores prior vector, unmasks, writes zero twice to PIT 40h, clears speaker bits 0-1 at 61h. | init_main, ported_stuntsmain_ | Stop host callbacks and silence related host sound; restore scheduler ownership. | high; reload effect mode-dependent |
| `0x0202AA-0x0202DE` | `timer_reg_callback` | Timer callback list  -  Registers callback/context for timer_intr_callback; no port operation in this member. | audio_add_driver_timer, audio_load_driver, set_frame_callback | Register with host scheduler preserving order and reentrancy. | high indirect boundary |
| `0x0202DE-0x02031D` | `timer_remove_callback` | CLI/STI  -  Unlinks callback/context pair from timer list with interrupts disabled around mutation. | audio_remove_driver_timer, audiodrv_atexit, remove_frame_callback | Synchronized scheduler unregister with no callback running after teardown. | high |
| `0x020329-0x0203BA` | `timer_intr_callback` | IRQ0; OUT 20h EOI; CLI/STI/IRET  -  IRQ0 updates raw counter, decrements divider, chains old BIOS handler on cadence or sends EOI, runs queued callbacks every fifth tick, updates callback counter, IRETs. | none indexed; indirect possible | Dispatch callbacks from monotonic time at same logical units; preserve counter and chain/EOI observable behavior only if needed. | high structure; inferred cadence depends setup |
| `0x022778-0x022782` | `timer_get_counter` | CLI/STI  -  Atomically reads 32-bit callback counter to DX:AX; unit is callback ticks, not milliseconds. | get_super_random, nopsub_30A77, nopsub_30A97, timer_compare_dx, timer_copy_counter, timer_get_counter_unk, timer_wait_for_dx | Return equivalent monotonic logical ticks or migrate consumers consistently to elapsed time. | high |
| `0x022782-0x02279A` | `timer_custom_delta` | CLI/STI  -  Returns current callback count minus supplied 32-bit count. | none indexed; indirect possible | Compute elapsed logical ticks using monotonic host time. | high |
| `0x02279A-0x0227B7` | `timer_get_delta` | CLI/STI  -  Returns ticks since prior snapshot. | setup_intro, timer_get_delta_alt | Preserve delta units or convert all callers consistently. | high |

## video / memory

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x0210F1-0x0210FE` | `mmgr_alloc_a000` | Calls mmgr_alloc_resmem with segment A000h  -  Establishes resource boundary at video segment A000h. | init_main | Treat display aperture as borrowed surface, not heap allocation. | high |
| `0x0210FE-0x02111D` | `nopsub_310FE` | Passes A000h to mmgr_alloc_resmem  -  Calls resource allocator for A000h then subtracts caller paragraph count from stored end boundary. | none indexed; indirect possible | Keep display aperture reservation separate from host heap; preserve requested reservation if used. | high machine behavior; intent inferred |
| `0x0232A8-0x0232C0` | `video_clear_color` | A000:DI; REP STOSW  -  ES=A000h, DI=0, CX=FA00h words; stores AX pattern 64,000 times (128,000 byte writes) with 16-bit DI wrap. Not a 64,000-byte/pixel fill. | set_bios_mode3, video_on_exit, video_set_mode_13h | Confirm caller color width; preserve AX word-pattern and wrapping behavior until proved visually equivalent to pixel fill. | high machine behavior; intended pixel semantics uncertain |

## video / mode

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x02005E-0x0200B6` | `video_set_mode4` | BDA 40:10; INT 10h AX=0004; OUT 3BFh/3B8h/3B4h/3B5h; B800:0000  -  Updates BDA equipment-word bits, selects BIOS mode 4, sets 3BF/3B8 adapter and 12 CRTC registers via index/data, clears 0x8000 bytes at B800:0000, writes 8Ah to 3B8. | init_main | Use indexed 320x200 four-color surface; preserve mode/palette transition and legacy text state. | high |
| `0x020120-0x020180` | `video_set_mode7` | BDA 40:10; INT 10h AX=0007; OUT 3BFh/3B8h/3B4h/3B5h; B800:0000  -  If flag clear delegates mode 3; else updates BDA bits, sets adapter/CRTC, clears 0x8000 bytes at B800:0000, writes 28h to 3B8, invokes mode 7. | ported_stuntsmain_ | Represent text/monochrome fallback in host UI or preserve mode-state compatibility. | high |
| `0x0203D8-0x020404` | `set_bios_mode3` | BDA 40:10; INT 10h AX=0003 and AH=0Bh  -  Clears color 0, updates BDA equipment bits, sets BIOS text mode 3 and background/border color 0. | video_set_mode7 | Restore host UI mode/background consistently. | high |
| `0x0225AE-0x0225D6` | `video_add_exithandler` | INT 10h AH=0Fh; BDA 40:10  -  Captures BIOS mode and BDA equipment word and registers video_on_exit. | video_set_mode_13h | Capture host display and palette state before game mode. | high |
| `0x0225D6-0x02260E` | `video_on_exit` | BDA 40:10; INT 10h AH=00h/0Bh; optional A000  -  Restores BDA and saved video mode; clears A000 if saved equipment bits equal 30h; sets background 0. | none indexed; indirect possible | Restore prior host display state and free mode-owned surfaces. | high; indirect exit callback |
| `0x023816-0x023848` | `video_set_mode_13h` | BDA 40:10; INT 10h AX=0013/AH=0Bh  -  Registers exit handler, changes BDA equipment bits, selects background 0 and BIOS mode 13h (320x200 indexed 256), clears color 0. | init_main | Create/select indexed 320x200 display surface and save prior host display state. | high |

## video / palette

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x0246A3-0x0246BC` | `video_set_palette` | INT 10h AX=1012h, BX=start, CX=count, ES:DX=RGB triples  -  Calls BIOS set palette block; game call uses BX=0,CX=0x100 (768 RGB bytes). DAC component bit depth depends on adapter; VGA convention is 6 bits. | load_palandcursor | Load same indexed palette/order; preserve adapter-to-display component scaling. | high call contract; bit depth adapter-dependent |

## video / retrace

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x022FEE-0x022FFC` | `nopsub_32FEE` | IN 3DAh bit 3  -  Spins until retrace bit 3 clears, then sets (next vertical-retrace edge); no timeout. | none indexed; indirect possible | Use SDL vsync/frame pacing with bounded shutdown behavior. | high |
| `0x022FFC-0x023006` | `video_get_status` | IN 3DAh  -  Returns AL&8 as AX (0 or 8), current retrace status. | random_wait | Provide equivalent display phase only if caller requires it. | high |

## video / surface

| Address | Routine | Interface and exact behavior | Indexed callers | Replacement contract | Confidence |
|---|---|---|---|---|---|
| `0x012D2E-0x013702` | `setup_car_shapes` | sprite1 descriptor / far pointer  -  Uses sprite1 geometry/pointer while preparing car shapes; the surface may be A000h when MCGA descriptor is active. | free_player_cars, run_game, setup_player_cars | Use typed surface metadata and separate shape parsing from presentation storage. | medium: setup includes descriptor and image references |
| `0x018DC8-0x018E04` | `mouse_draw_opaque` | sprite1 far bitmap pointer  -  Mouse cursor save/draw path accesses the current sprite surface for opaque pixels. | input_checking, mouse_draw_opaque_check | Use host cursor overlay or save-under buffer preserving opaque pixel semantics. | medium |
| `0x018E04-0x018E90` | `mouse_draw_transparent` | sprite1 far bitmap pointer  -  Mouse cursor path accesses current sprite surface with transparent pixels. | input_checking, mouse_draw_transparent_check | Use host cursor overlay/save-under preserving transparent pixels. | medium |
| `0x0224AA-0x0224FA` | `sprite_free_wnd` | sprite window descriptor / far heap pointer  -  Releases a sprite window and allocated bitmap/line-offset data; descriptors may instead point at borrowed A000 surface. | end_hiscore, free_player_cars, load_tracks_menu_shapes, run_car_menu, run_intro_looped, run_menu, run_opponent_menu, run_tracks_menu, setup_car_shapes, setup_intro, sub_275C6 | Free owned host window buffers, never borrowed display storage. | high |
| `0x02320E-0x02327F` | `nopsub_3320E` | sprite1 bitmap descriptor  -  Checks current sprite bitmap descriptor state; no confirmed pixel write. | none indexed; indirect possible | Keep typed surface-available check and explicit borrowed/owned distinction. | medium |
| `0x0232C0-0x023330` | `sprite_clear_1_color` | sprite1 far bitmap pointer  -  Fills current clip rectangle, duplicates byte color to AX, advances rows using pitch. | do_sinking, draw_track_preview, end_hiscore, init_main, intro_op, load_intro_resources, run_car_menu, run_opponent_menu, run_option_menu, run_tracks_menu, show_dialog, skybox_op, skybox_op_helper2, sprite_copy_2_to_1_clear, sprite_copy_wnd_to_1_clear | Fill clipped indexed surface with same 8-bit palette index and pitch. | high |
| `0x023344-0x0233C0` | `draw_unknown_lines` | sprite1 far bitmap pointer  -  Writes line/pixel spans using sprite line offsets, pitch and clip bounds. | none indexed; indirect possible | Preserve integer rasterization, clipping and row stride. | high |
| `0x0233C0-0x023578` | `putpixel_line1_maybe` | sprite1 far bitmap pointer  -  Rasterizes line spans/pixels into sprite1 through its segment and line table. | preRender_line | Preserve integer line rasterization and clipping. | high |
| `0x0235D2-0x02367A` | `sprite_1_unk` | sprite1 far bitmap pointer  -  Reads/writes current sprite1 bitmap; exact operation name unresolved. | do_fileselect_dialog, do_joy_restext, draw_button, load_tracks_menu_shapes, loop_game, mouse_track_op | Preserve exact pixel operation after body/caller classification. | uncertain operation label; surface touch established |
| `0x02367A-0x023742` | `sprite_1_unk3` | sprite1 far bitmap pointer  -  Blits image data to current sprite surface using pitch and line offsets. | sprite_blit_to_video | Preserve row format, pitch and clipping. | high |
| `0x023742-0x023816` | `font_draw_text` | sprite1 far bitmap pointer  -  Renders glyph rows from font tables into current bitmap. | draw_button, end_hiscore, highscore_text_unk, hiscore_draw_text, intro_draw_text, run_car_menu, run_opponent_menu, run_tracks_menu, security_check | Preserve bitmap glyphs, spacing and indexed writes. | high |
| `0x023890-0x0239FA` | `sprite_putimage_and` | sprite1 far bitmap pointer  -  Composites shape with destination AND/mask semantics. | draw_2DtrackMap, mouse_draw_transparent | Use exact bitwise AND, not alpha blend. | high |
| `0x023A1E-0x023AC0` | `putpixel_iconMask` | sprite1 far bitmap pointer  -  Applies icon mask pixels to current destination surface. | draw_2DtrackMap, load_tracks_menu_shapes, preRender_icons | Preserve exact bitwise mask operation. | high |
| `0x023B02-0x023B98` | `shape2d_render_bmp_as_mask` | sprite1 far bitmap pointer  -  ANDs decoded bitmap-mask data into destination pixels. | run_game, setup_car_shapes | Preserve packed mask format and per-pixel AND. | high |
| `0x023BDA-0x023D0C` | `sprite_putimage` | sprite1 far bitmap pointer  -  Copies decoded shape rows into current bitmap. | end_hiscore, load_intro_resources, mouse_draw_opaque, run_car_menu, setup_intro, setup_mcgawnd1, sprite_blit_to_video, sub_19F14 | Preserve shape row encoding, clipping and destination stride. | high |
| `0x023D4E-0x023DBE` | `sprite_shape_to_1_alt` | sprite1 far bitmap pointer  -  Copies/transforms shape pixels into sprite1 via active descriptor. | load_intro_resources, run_car_menu, run_intro, run_menu | Preserve row transform and clip behavior. | medium: operation inferred from body/context |
| `0x023E00-0x023E90` | `shape2d_op_unk` | sprite1 far bitmap pointer  -  Decodes shape2d run stream and writes/combines runs into active surface. | loop_game, setup_car_shapes | Preserve signed run controls and destination pixel operation. | medium |
| `0x023ED2-0x024060` | `shape2d_op_unk3` | sprite1 far bitmap pointer  -  Alternate shape2d stream decoder reads/writes the active surface. | setup_car_shapes | Preserve encoded runs and exact destination operation. | medium |
| `0x024084-0x024212` | `sprite_putimage_or` | sprite1 far bitmap pointer  -  OR-composites source image bits into destination. | draw_2DtrackMap, mouse_draw_transparent, setup_car_shapes | Use exact bitwise OR on indexed pixels. | high |
| `0x024212-0x0242F6` | `putpixel_iconFillings` | sprite1 far bitmap pointer  -  Writes icon filling pixels using sprite line offsets/pitch. | draw_2DtrackMap, load_tracks_menu_shapes, preRender_icons | Preserve indexed writes and clipping. | high |
| `0x0242F6-0x0243B0` | `shape2d_op_unk4` | sprite1 far bitmap pointer  -  Decodes signed shape runs and ORs source bytes into destination pixels. | run_game, setup_car_shapes | Preserve signed run lengths, row wraps and bitwise OR. | high |
| `0x0243B0-0x024526` | `sprite_putimage_transparent` | sprite1 far bitmap pointer  -  Copies shape pixels while skipping transparency sentinel; honors clip bounds and pitch. | draw_ingame_text, run_car_menu | Preserve transparency sentinel and clipping exactly. | high |
| `0x024526-0x0245BC` | `sub_34526` | sprite1 far bitmap pointer  -  Unnamed helper reads/writes through active sprite1 pointer. | run_opponent_menu | Retain exact pixel memory operation after caller review. | uncertain operation label; surface touch established |
| `0x0245BC-0x0246A3` | `sub_345BC` | sprite1 far bitmap pointer  -  Unnamed helper uses sprite1 descriptor/bitmap; source shows alternate-entry/data complexity. | do_fileselect_dialog, do_savefile_dialog, load_tracks_menu_shapes, loop_game, read_line_helper2, run_game, show_dialog | Preserve exact operation/shared entries; resolve boundary before combining. | uncertain role and boundary |
| `0x0246BC-0x024736` | `draw_filled_lines` | sprite1 far bitmap pointer  -  Draws filled spans through ES:DI, advancing by sprite pitch. | preRender_sphere | Preserve span geometry, pitch and clip bounds. | high |
| `0x02477E-0x0247DC` | `sprite_clear_shape` | sprite1 far bitmap pointer  -  Copies rectangle from sprite1 bitmap to far shape/save buffer using line offsets and REP MOVSB. | load_intro_resources | Implement surface readback/save-under with identical row geometry. | high |
| `0x0247DC-0x024B0C` | `shape_op_explosion` | sprite1 far bitmap pointer  -  Scales/clips explosion shape and writes it into sprite1. Inline descriptor/table bytes decode as apparent I/O under linear disassembly; source marks them as data. | update_frame | Preserve scaling, clipping, encoding and blend; render to indexed surface. | high surface; apparent I/O is false decode |
| `0x024B96-0x024C0C` | `draw_patterned_lines` | sprite1 far bitmap pointer  -  Writes patterned line spans via current surface pitch/clip geometry. | none indexed; indirect possible | Preserve pattern phase, clipping and indexed pixels. | high |
| `0x024C0C` | `sprite_make_wnd` | sprite descriptors + far memory allocator  -  Creates window descriptor, allocates width*height storage, sets pitch=width and constructs line offsets. Storage may be heap, not A000. Index start 0x024C0C; no verified end field. | end_hiscore, load_palandcursor, load_tracks_menu_shapes, run_car_menu, run_intro_looped, run_menu, run_opponent_menu, run_tracks_menu, setup_car_shapes, setup_intro, setup_mcgawnd1, setup_mcgawnd2, setup_player_cars, sub_274B0 | Use explicit owned host buffers with dimensions/pitch/clip and distinguish from borrowed display surface. | medium: indexed end missing |
| `0x025B26-0x025B76` | `putpixel_single_maybe` | sprite1 far bitmap pointer  -  Plots one pixel through current line-offset/position state. | get_a_poly_info, intro_op, preRender_sphere, run_car_menu | Plot same clipped indexed color at equivalent coordinates. | high |
| `0x025B76-0x025C4E` | `sub_35B76` | sprite1 far bitmap pointer  -  Unnamed helper reads/writes active sprite surface. | read_line_helper, sub_3702E | Preserve exact copy/fill after caller review. | uncertain operation label; surface touch established |
| `0x025C4E-0x025DC8` | `sub_35C4E` | sprite1 and sprite2 far bitmap pointers  -  Copies pixels between two sprite surfaces; either may be A000 or allocated backing storage. | run_game, setup_intro | Use separate explicit source/destination surfaces, ownership and pitches. | high |
| `0x025E08-0x025F48` | `sub_35E08` | sprite1 far bitmap pointer  -  Blits source pixels into sprite1 while skipping transparent sentinel FFh. | none indexed; indirect possible | Preserve FFh transparency and clip semantics. | high |
| `0x02A958-0x02A9A0` | `setup_mcgawnd1` | sprite descriptors / A000 selection  -  Initializes MCGA-oriented window/surface descriptor and rendering geometry. | run_game, setup_intro | Select indexed 320x200 host surface and initialize dimensions/pitch, with no physical segments. | medium: check setup flags during port |

## Confirmed hardware/service boundary

- Ports found: 3BFh/3B8h/3B4h/3B5h adapter setup; 3DAh retrace; PIT 40h/43h; PPI/speaker 61h; PIC 20h/21h; keyboard 60h/61h; joystick 201h; A000h/B800h video memory.
- BIOS/interrupt services: INT 10h video/palette; INT 15h C201h (mouse path, purpose uncertain); INT 16h keyboard; INT 21h filesystem/memory/vector services; INT 20h termination fallback; INT 23h/24h and vector 0 hooks.
- Not found in reviewed indexed source/image: direct DAC writes 3C0h-3C9h, port 64h, AdLib 388h, direct Tandy/PCjr sound I/O, or an EXE self-read. External audio-driver behavior and gaps below remain unknown.
- `security_check` at 0x0044CF reads misc/string resources; this is not evidence of reading the EXE.

## Coverage gaps

- **seg012 [0x01F436,0x01FDDE) (2472 bytes), blocker class 8:** Unmapped. Linear scan shows candidate INSB 0x01F48C, OUT 0x01F490, INT3 0x01F4C0/0x01F4C1; boundaries and ownership unproved. Next: Resolve complete member/function boundaries and inspect semantic entries before assigning hardware/disk behavior.
- **seg012 [0x022A72,0x022AE2) (112 bytes), blocker class 8:** Unmapped range; no source-confirmed platform op in linear scan. Next: Resolve if exhaustive whole-image coverage is required.
- **seg012 [0x024CE4,0x025AF6) (3602 bytes), blocker class 5:** Unmapped range overlaps large sprite_make_wnd source extent; image function index has no verified end for sprite_make_wnd. Next: Resolve ownership/member boundary before treating all bytes as one function.
- **seg017 [0x026AF2,0x026AF4) (2 bytes), blocker class 8:** Two unmapped bytes after mouse range helper; no platform operation assigned. Next: Resolve only for strict whole-image exhaustiveness.
- **seg027 [0x028570,0x02863C) (204 bytes), blocker class 8:** Unmapped portion of audio-loader object; no direct platform contract assigned. Next: Review audio-loader for delegated driver/port behavior when object boundary is resolved.

## False-positive disassembly exclusions

- **0x00F7D2 build_track_object:** Linear decode OUTSB; source procedure has no port instruction there.
- **0x019A54 file_load_resource:** Linear decode INSB; source procedure has no port instruction there.
- **0x01ECDE draw_line_related:** Linear decode IN AX,5Dh; source body has no I/O instruction at that location.
- **0x0226F4 sin_fast; 0x02365C sprite_1_unk; 0x0241F0 sprite_putimage_or; 0x0242B6 putpixel_iconFillings; 0x0244E6 sprite_putimage_transparent; 0x02469C sub_345BC; 0x024A2F shape_op_explosion; 0x025D35-0x025D37 sub_35C4E:** Linear decoder emits port/string/INT1-like instructions in code/data mixtures or ordinary drawing bodies. Do not treat those as I/O without a confirmed instruction boundary and source operation.

## Evidence and toolchain

- Image function records and boundaries: `evidence/functions.json`; per-entry source and reference-listing anchors are in `platform-boundary.json`.
- This inventory preserves unresolved object-range and boundary concerns in the entries whose source/image evidence remains incomplete.
- No compiler/profile experiment was performed; no toolchain-hypotheses proposal.
