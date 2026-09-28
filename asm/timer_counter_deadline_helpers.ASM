.8086
INCLUDE include\platform_hw.inc
name asm012_141210
DGROUP group CONST,_BSS,_DATA
assume ds:DGROUP
_DATA segment word public 'DATA'
public _timer_copy_unk, _timer_copy_target_high, _line_input_screen_rect
_timer_copy_unk dd 0
_timer_copy_target_high equ _timer_copy_unk+2
_last_timer_callback_counter dd 0
previous_timer_counter_hi equ _last_timer_callback_counter+2
_line_input_screen_rect dw 0
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
assume ds:DGROUP
extrn _timer_callback_counter:dword
extrn _timer_get_counter:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _timer_get_delta
public _timer_reset
public _timer_copy_counter
public _timer_wait_for_dx
public _timer_compare_dx
public _timer_get_counter_unk

; timer_get_delta
; -----------------------------------------------------------------------------
; Purpose: Compute the elapsed timer delta between stored counter values.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _last_timer_callback_counter, _timer_callback_counter.
; Globals written: _last_timer_callback_counter.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; -----------------------------------------------------------------------------
_timer_get_delta proc far
    mov bx, word ptr ds:[_last_timer_callback_counter]
    mov cx, word ptr ds:[previous_timer_counter_hi]
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov ax, word ptr ds:[_timer_callback_counter]
    mov dx, word ptr ds:[_timer_callback_counter+2]
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov word ptr ds:[_last_timer_callback_counter], ax
    mov word ptr ds:[previous_timer_counter_hi], dx
    sub ax, bx
    sbb dx, cx
    retf
_timer_get_delta endp
; timer_reset
; -----------------------------------------------------------------------------
; Purpose: Reset the timer counter state.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _timer_callback_counter.
; -----------------------------------------------------------------------------
_timer_reset proc far
    xor     ax, ax
    mov     word ptr _timer_callback_counter, ax
    mov     word ptr _timer_callback_counter+2, ax
    retf
_timer_reset endp
; timer_copy_counter
; -----------------------------------------------------------------------------
; Purpose: Copy the current timer counter into the caller-provided value.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_ticksh (word ptr 8); arg_ticksl (word ptr 6).
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _timer_copy_unk.
; -----------------------------------------------------------------------------
_timer_copy_counter proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_ticksl = word ptr 6
    arg_ticksh = word ptr 8

    push    bp
    mov     bp, sp
    call    _timer_get_counter
    add     ax, [bp+arg_ticksl]
    adc     dx, [bp+arg_ticksh]
    mov     word ptr _timer_copy_unk, ax
    mov     word ptr _timer_copy_target_high, dx
    pop     bp
    retf
_timer_copy_counter endp
; timer_wait_for_dx
; -----------------------------------------------------------------------------
; Purpose: Wait until the timer counter reaches the requested high-word value.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _timer_copy_unk.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_timer_wait_for_dx proc far
    call    _timer_get_counter
    cmp     dx, word ptr _timer_copy_target_high
    jb      short near ptr _timer_wait_for_dx
    ja      short return_when_deadline_reached
    cmp     ax, word ptr _timer_copy_unk
    jb      short near ptr _timer_wait_for_dx
return_when_deadline_reached:
    retf
_timer_wait_for_dx endp
; timer_compare_dx
; -----------------------------------------------------------------------------
; Purpose: Compare the timer counter high word with the requested value.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _timer_copy_unk.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_timer_compare_dx proc far
 call far ptr _timer_get_counter
 cmp dx,word ptr ds:[_timer_copy_target_high]
 jb zero_result
 ja one_result
 cmp ax,word ptr ds:[_timer_copy_unk]
 jb zero_result
one_result:
 mov ax,1
 ret
zero_result:
 xor ax,ax
 ret
_timer_compare_dx endp
; timer_get_counter_unk
; -----------------------------------------------------------------------------
; Purpose: Implement the timer service identified by timer get counter unk.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_timer_get_counter_unk proc far
    push bp
    mov bp, sp
    sub sp, 4
    call far ptr _timer_get_counter
    add ax, word ptr [bp+6]
    adc dx, word ptr [bp+8]
    mov word ptr [bp-4], ax
    mov word ptr [bp-2], dx
wait_for_counter:
    call far ptr _timer_get_counter
    cmp dx, word ptr [bp-2]
    jb wait_for_counter
    ja counter_reached
    cmp ax, word ptr [bp-4]
    jb wait_for_counter
counter_reached:
    mov sp, bp
    pop bp
    retf
    db 0
_timer_get_counter_unk endp
S012_TEXT ends
end
