.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _parse_shape2d_helper
_parse_shape2d_helper proc far
    push bp
    mov bp, sp
    mov ax, word ptr [bp+8]
    xor dx, dx
    shl ax, 1
    rcl dx, 1
    shl ax, 1
    rcl dx, 1
    shl ax, 1
    rcl dx, 1
    shl ax, 1
    rcl dx, 1
    add ax, word ptr [bp+6]
    adc dx, 0
    pop bp
    retf
_parse_shape2d_helper endp
_TEXT ends
end
