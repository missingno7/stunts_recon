.8086
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _cosfast:far
extrn _polang:far
extrn _sinfast:far
public _polradius2d
_polradius2d proc far
    arg_z = word ptr 6
    arg_y = word ptr 8
    push bp
    mov bp, sp
    push word ptr [bp+arg_y]
    push word ptr [bp+arg_z]
    call far ptr _polang
    add sp, 4
    or ax, ax
    jge short when_greater_or_equal
    neg ax
when_greater_or_equal:
    cmp ax, 100h
    jl short polradius2d_when_less
    sub ax, 200h
    neg ax
polradius2d_when_less:
    cmp ax, 80h
    jg short polradius2d_when_greater
    push ax
    call far ptr _cosfast
    add sp, 2
    mov bx, ax
    mov dx, word ptr [bp+arg_y]
    or dx, dx
    jge short polradius2d_return_if_at_least
    neg dx
polradius2d_return_if_at_least:
    xor ax, ax
    sar dx, 1
    rcr ax, 1
    sar dx, 1
    rcr ax, 1
    div bx
    pop bp
    retf
polradius2d_when_greater:
    push ax
    call far ptr _sinfast
    add sp, 2
    mov bx, ax
    mov dx, word ptr [bp+arg_z]
    or dx, dx
    jge short return_if_at_least_2
    neg dx
return_if_at_least_2:
    xor ax, ax
    sar dx, 1
    rcr ax, 1
    sar dx, 1
    rcr ax, 1
    div bx
    pop bp
    retf
_polradius2d endp
S012_TEXT ends
end
