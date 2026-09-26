.model medium
DGROUP group dseg
dseg segment byte public 'STUNTSD'
assume ds:dseg
extrn _timerintr:dword
dseg ends
_TEXT segment word public 'CODE'
assume cs:_TEXT, ds:dseg
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
loc_302ED:
    cmp     [bx], ax
    jnz     short loc_302F6
    cmp     [bx+2], dx
    jz      short loc_302FD
loc_302F6:
    add     bx, 4
    loop    loc_302ED
    pop     bp
    retf
loc_302FD:
    cli
    dec     cx
    jz      short loc_30311
loc_30301:
    mov     ax, [bx+4]
    mov     [bx], ax
    mov     ax, [bx+6]
    mov     [bx+2], ax
    add     bx, 4
    loop    loc_30301
loc_30311:
    mov     word ptr [bx], 0
    mov     word ptr [bx+2], 0
    sti
    pop     bp
    retf
_timer_remove_callback endp
_TEXT ends
end
