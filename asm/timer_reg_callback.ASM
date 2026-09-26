.model medium
DGROUP group dseg
dseg segment byte public 'STUNTSD'
assume ds:dseg
extrn _timerintr:dword
extrn _aNoRoomLeftOnTimerInterru:byte
dseg ends
_TEXT segment word public 'CODE'
assume cs:_TEXT, ds:dseg
extrn _fatal_error:far
public _timer_reg_callback
_timer_reg_callback proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     cx, 5
    mov     bx, offset _timerintr
loc_302B3:
    cmp     word ptr [bx+2], 0
    jz      short loc_302C7
    add     bx, 4
    loop    loc_302B3
    mov     ax, offset _aNoRoomLeftOnTimerInterru; "NO ROOM LEFT ON TIMER INTERRUPT ROUTINE"...
    push    ax
    call    far ptr _fatal_error
loc_302C7:
    mov     ax, [bp+arg_0]
    mov     [bx], ax
    mov     word ptr [bx+2], 0
    mov     ax, [bp+arg_2]
    mov     [bx+2], ax
    mov     word ptr [bx+6], 0
    pop     bp
    retf
_timer_reg_callback endp
_TEXT ends
end
