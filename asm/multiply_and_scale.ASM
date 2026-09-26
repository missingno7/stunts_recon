.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _multiply_and_scale
_multiply_and_scale proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    imul    [bp+arg_2]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    adc     dx, 0
    mov     ax, dx
    pop     bp
    retf
_multiply_and_scale endp
_TEXT ends
end
