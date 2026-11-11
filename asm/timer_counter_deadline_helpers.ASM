; ASMDOC[timers] Module: 32-bit callback-clock delta/reset and shared/local unsigned deadline operations. Deadline
; ASMDOC[timers]   comparisons are full high/low lexicographic comparisons, not DX register parameters.
; ASMDOC[timers] Authoritative contracts below refine older retained comments; original source bytes are untouched.
; ASMDOC[timers] Stack offsets BP+6/BP+8 refer to saved-BP FAR frame; entry SP+4/SP+6 refer to original FAR frame.
; ASMDOC[timers] Data exports: _timer_copy_unk: shared deadline dword; _timer_copy_target_high: EQU alias at shared
; ASMDOC[timers]   deadline+2; _line_input_screen_rect: word near pointer owned here but unused by these routines
.8086
INCLUDE include\platform_hw.inc
name asm012_141210
DGROUP group CONST,_BSS,_DATA
assume ds:DGROUP
_DATA segment word public 'DATA'
public _timer_deadline_ticks, _timer_copy_target_high, _line_input_font_header
; ASMDOC[timers] Block: Exported data: shared deadline dword and high-word EQU alias. line_input_screen_rect is
; ASMDOC[timers]   separate storage, not code.
_timer_deadline_ticks dd 0
_timer_copy_target_high equ _timer_deadline_ticks+2
_last_timer_callback_counter dd 0
previous_timer_counter_hi equ _last_timer_callback_counter+2
_line_input_font_header dw 0
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
extrn _timer_callback_counter:dword
extrn _timer_get_counter:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _elapsedticks
public _resettick
public _setticks
public _waitticks
public _testticks
public _timedwait

; elapsedticks
; -----------------------------------------------------------------------------
; Purpose: Compute the elapsed timer delta between stored counter values.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _last_timer_callback_counter, _timer_callback_counter.
; Globals written: _last_timer_callback_counter.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_get_delta (proposed name _timer_get_delta): Snapshot callback counter under CLI/STI,
; ASMDOC[timers]   update previous snapshot and return modular 32-bit elapsed delta.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; no stack arguments
; ASMDOC[timers] outputs: DX:AX=(current-_last_timer_callback_counter) modulo 2^32; BX:CX hold old low:high words;
; ASMDOC[timers]   IF=1.
; ASMDOC[timers] preserves: SI DI BP DS ES unmodified.
; ASMDOC[timers] clobbers: AX BX CX DX flags; IF forced enabled
; ASMDOC[timers] globals_read: _last_timer_callback_counter; _timer_callback_counter
; ASMDOC[timers] globals_written: _last_timer_callback_counter
; ASMDOC[timers] services: CLI/STI only around current read; previous read/store outside masked interval
; ASMDOC[timers] reachability: Called by setup_intro and timer_get_delta_alt.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_elapsedticks proc far
; ASMDOC[timers] Block: Previous snapshot load/store are outside critical section; only current low/high read is
; ASMDOC[timers]   IRQ-masked.
    mov bx, word ptr ds:[_last_timer_callback_counter]
    mov cx, word ptr ds:[previous_timer_counter_hi]
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov ax, word ptr ds:[_timer_callback_counter]
    mov dx, word ptr ds:[_timer_callback_counter+2]
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov  word ptr ds:[_last_timer_callback_counter], ax
    mov word ptr ds:[previous_timer_counter_hi], dx
    sub ax, bx
    sbb dx, cx
    retf
_elapsedticks endp
; resettick
; -----------------------------------------------------------------------------
; Purpose: Reset the timer counter state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _timer_callback_counter.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_reset (proposed name _timer_reset): Zero callback counter in two unprotected word
; ASMDOC[timers]   stores; retain last delta snapshot and elapsed counter.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; no stack arguments
; ASMDOC[timers] outputs: AX=0; callback counter=0 (subject to interrupt interleaving); arithmetic flags from XOR.
; ASMDOC[timers] preserves: BX CX DX SI DI BP DS ES unmodified.
; ASMDOC[timers] clobbers: AX arithmetic flags
; ASMDOC[timers] globals_read:
; ASMDOC[timers] globals_written: _timer_callback_counter low/high words
; ASMDOC[timers] services:
; ASMDOC[timers] reachability: No known direct, address-taken, vector or callback reference in active
; ASMDOC[timers]   sources/recipes; no fallthrough from preceding RETF. Treat as unused public, not removable bytes.
; ASMDOC[timers] dead: true (static known-program reachability; never permission to trim extent)
_resettick proc far
; ASMDOC[timers] Block: Reset performs two unmasked word stores and leaves previous-delta snapshot unchanged.
    xor     ax, ax
    mov     word ptr _timer_callback_counter, ax
    mov     word ptr _timer_callback_counter+2, ax
    retf
_resettick endp
; setticks
; -----------------------------------------------------------------------------
; Purpose: Copy the current timer counter into the caller-provided value.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_ticksh (word ptr 8);
; arg_ticksl (word ptr 6).
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _timer_copy_unk.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_copy_counter (proposed name _timer_copy_counter): Set shared deadline to current
; ASMDOC[timers]   callback counter plus supplied 32-bit duration modulo 2^32.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: SS:[BP+6] duration low word (entry SP+4); SS:[BP+8] duration high word (entry SP+6);
; ASMDOC[timers]   DS=DGROUP
; ASMDOC[timers] outputs: DX:AX=deadline; shared _timer_copy_unk stores it; IF=1 through timer_get_counter.
; ASMDOC[timers] preserves: BP restored; BX CX SI DI DS ES unmodified by this body or timer_get_counter.
; ASMDOC[timers] clobbers: AX DX arithmetic flags IF
; ASMDOC[timers] globals_read: _timer_callback_counter via _timer_get_counter
; ASMDOC[timers] globals_written: _timer_copy_unk low word; _timer_copy_target_high alias
; ASMDOC[timers] services: FAR _timer_get_counter
; ASMDOC[timers] reachability: Line editor/audio wait paths establish shared deadline.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_setticks proc far
    arg_ticksl = word ptr 6
    arg_ticksh = word ptr 8

    push    bp
    mov     bp, sp
; ASMDOC[timers] Block: Store current+duration as shared deadline, modulo 2^32. This is not a caller-provided
; ASMDOC[timers]   destination pointer.
    call    _timer_get_counter
    add     ax, [bp+arg_ticksl]
    adc     dx, [bp+arg_ticksh]
    mov     word ptr _timer_deadline_ticks, ax
    mov     word ptr _timer_copy_target_high, dx
    pop     bp
    retf
_setticks endp
; waitticks
; -----------------------------------------------------------------------------
; Purpose: Wait until the timer counter reaches the requested high-word value.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack
; arguments referenced.
; Register ABI: touches AX, DX; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: _timer_copy_unk.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_wait_for_dx (proposed name _timer_wait_for_dx): Busy-wait until current unsigned
; ASMDOC[timers]   callback counter is at least the full shared deadline.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; no stack/register arguments; incoming DX ignored
; ASMDOC[timers] outputs: DX:AX=first observed counter >= unsigned deadline; IF=1. No wrap-aware signed-distance
; ASMDOC[timers]   comparison.
; ASMDOC[timers] preserves: BX CX SI DI BP DS ES unmodified locally and by timer_get_counter.
; ASMDOC[timers] clobbers: AX DX flags IF
; ASMDOC[timers] globals_read: _timer_copy_unk low/high; _timer_callback_counter via _timer_get_counter
; ASMDOC[timers] globals_written:
; ASMDOC[timers] services: Repeated FAR _timer_get_counter; busy wait without yield
; ASMDOC[timers] reachability: Audio wait callers; backward branches target same FAR public entry without changing
; ASMDOC[timers]   return frame.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_waitticks proc far
    call    _timer_get_counter
; ASMDOC[timers] Block: Unsigned lexicographic high/low comparison against shared dword; incoming DX is not an
; ASMDOC[timers]   argument.
    cmp     dx, word ptr _timer_copy_target_high
    jb      short near ptr _waitticks
    ja      short return_when_deadline_reached
    cmp     ax, word ptr _timer_deadline_ticks
    jb      short near ptr _waitticks
return_when_deadline_reached:
    retf
_waitticks endp
; testticks
; -----------------------------------------------------------------------------
; Purpose: Compare the timer counter high word with the requested value.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX, DS; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: _timer_copy_unk.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_compare_dx (proposed name _timer_compare_dx): Return Boolean current unsigned
; ASMDOC[timers]   callback counter >= full shared deadline.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: DS=DGROUP; no stack/register arguments; incoming DX ignored
; ASMDOC[timers] outputs: AX=1 when counter>=deadline, else 0; DX=current counter high word; IF=1; flags not a
; ASMDOC[timers]   uniform Boolean result.
; ASMDOC[timers] preserves: BX CX SI DI BP DS ES unmodified locally and by timer_get_counter.
; ASMDOC[timers] clobbers: AX DX flags IF
; ASMDOC[timers] globals_read: _timer_copy_unk low/high; _timer_callback_counter via _timer_get_counter
; ASMDOC[timers] globals_written:
; ASMDOC[timers] services: FAR _timer_get_counter
; ASMDOC[timers] reachability: Line editor and audio scheduling callers.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_testticks proc far
; ASMDOC[timers] Block: Return AX Boolean for full unsigned counter>=deadline; flags are path-dependent.
 call far ptr _timer_get_counter
 cmp dx,word ptr ds:[_timer_copy_target_high]
 jb zero_result
 ja one_result
 cmp ax, word ptr ds:[_timer_deadline_ticks]
 jb zero_result
one_result:
 mov ax,1
 ret
zero_result:
 xor ax,ax
 ret
_testticks endp
; timedwait
; -----------------------------------------------------------------------------
; Purpose: Implement the timer service identified by timer get counter unk.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC[timers] Contract _timer_get_counter_unk (proposed name _timer_wait_ticks): Compute a private 32-bit deadline
; ASMDOC[timers]   = current+duration modulo 2^32 and busy-wait with unsigned full-dword comparisons.
; ASMDOC[timers] convention: FAR caller-cleanup stack call; RETF (MASM RET in a FAR PROC also returns far).
; ASMDOC[timers] inputs: SS:[BP+6] duration low word (entry SP+4); SS:[BP+8] duration high word (entry SP+6);
; ASMDOC[timers]   DS=DGROUP
; ASMDOC[timers] outputs: DX:AX=first observed counter>=local deadline; four local stack bytes released; IF=1.
; ASMDOC[timers] preserves: BP restored; BX CX SI DI DS ES unchanged locally and by timer_get_counter.
; ASMDOC[timers] clobbers: AX DX arithmetic flags IF
; ASMDOC[timers] globals_read: _timer_callback_counter via _timer_get_counter
; ASMDOC[timers] globals_written:
; ASMDOC[timers] services: FAR _timer_get_counter repeatedly; no yield
; ASMDOC[timers] reachability: Game/menu pacing callers. Uses local [BP-4]/[BP-2], so does not overwrite shared
; ASMDOC[timers]   deadline.
; ASMDOC[timers] dead: false (static known-program reachability; never permission to trim extent)
_timedwait proc far
    push bp
    mov bp, sp
    sub sp, 4
    call far ptr _timer_get_counter
    add ax, [bp+6]
    adc dx, [bp+8]
    mov [bp-4], ax
    mov [bp-2], dx
; ASMDOC[timers] Block: Private stack deadline at BP-4:BP-2 avoids modifying the shared deadline. Ordinary unsigned
; ASMDOC[timers]   compare is not wrap-distance logic.
wait_for_counter:
    call far ptr _timer_get_counter
    cmp dx, [bp-2]
    jb wait_for_counter
    ja counter_reached
    cmp ax, [bp-4]
    jb wait_for_counter
counter_reached:
    mov sp, bp
    pop bp
    retf
    db 0 ; retained zero fill at this boundary
_timedwait endp
S012_TEXT ends
end
