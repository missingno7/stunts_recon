.8086
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
_timer_get_delta proc far
    mov bx, word ptr ds:[_last_timer_callback_counter]
    mov cx, word ptr ds:[previous_timer_counter_hi]
    cli
    mov ax, word ptr ds:[_timer_callback_counter]
    mov dx, word ptr ds:[_timer_callback_counter+2]
    sti
    mov word ptr ds:[_last_timer_callback_counter], ax
    mov word ptr ds:[previous_timer_counter_hi], dx
    sub ax, bx
    sbb dx, cx
    retf
_timer_get_delta endp
; timer_reset
_timer_reset proc far
    xor     ax, ax
    mov     word ptr _timer_callback_counter, ax
    mov     word ptr _timer_callback_counter+2, ax
    retf
_timer_reset endp
; timer_copy_counter
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
