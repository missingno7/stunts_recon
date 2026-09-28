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
    jge short loc_200CD
    neg ax
loc_200CD:
    cmp ax, 100h
    jl short loc_200D7
    sub ax, 200h
    neg ax
loc_200D7:
    cmp ax, 80h
    jg short loc_200FE
    push ax
    call far ptr _cosfast
    add sp, 2
    mov bx, ax
    mov dx, word ptr [bp+arg_y]
    or dx, dx
    jge short loc_200F0
    neg dx
loc_200F0:
    xor ax, ax
    sar dx, 1
    rcr ax, 1
    sar dx, 1
    rcr ax, 1
    div bx
    pop bp
    retf
loc_200FE:
    push ax
    call far ptr _sinfast
    add sp, 2
    mov bx, ax
    mov dx, word ptr [bp+arg_z]
    or dx, dx
    jge short loc_20112
    neg dx
loc_20112:
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
