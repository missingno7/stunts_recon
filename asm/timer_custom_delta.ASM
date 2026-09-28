.8086
INCLUDE include\platform_hw.inc
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _timer_callback_counter:dword
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _timer_custom_delta
; -----------------------------------------------------------------------------
; Purpose: Implement the timer service identified by timer custom delta.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _timer_callback_counter.
; Globals written: none identified.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; -----------------------------------------------------------------------------
_timer_custom_delta proc far
    push bp
    mov bp, sp
    mov bx, word ptr [bp + 6]
    mov cx, word ptr [bp + 8]
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov ax, word ptr ds:[_timer_callback_counter]
    mov dx, word ptr ds:[_timer_callback_counter+2]
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    sub ax, bx
    sbb dx, cx
    pop bp
    retf
_timer_custom_delta endp
S012_TEXT ends
end
