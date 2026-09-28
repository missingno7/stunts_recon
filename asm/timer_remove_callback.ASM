.model medium
DGROUP group dseg
dseg segment byte public 'STUNTSD'
assume ds:dseg
extrn _timerintr:dword
dseg ends
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:dseg
public _timer_remove_callback
_timer_remove_callback proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     cx, 5
    mov     bx, offset _timerintr
    mov     ax, [bp+arg_0]
    mov     dx, [bp+arg_2]
timer_remove_search_slot_loop:
    cmp     [bx], ax
    jnz     short timer_remove_advance_slot
    cmp     [bx+2], dx
    jz      short timer_remove_disable_interrupts
timer_remove_advance_slot:
    add     bx, 4
    loop    timer_remove_search_slot_loop
    pop     bp
    retf
timer_remove_disable_interrupts:
    cli
    dec     cx
    jz      short timer_remove_clear_tail_slot
timer_remove_shift_slots:
    mov     ax, [bx+4]
    mov     [bx], ax
    mov     ax, [bx+6]
    mov     [bx+2], ax
    add     bx, 4
    loop    timer_remove_shift_slots
timer_remove_clear_tail_slot:
    mov     word ptr [bx], 0
    mov     word ptr [bx+2], 0
    sti
    pop     bp
    retf
_timer_remove_callback endp
S012_TEXT ends
end
