.8086
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _cos_fast:far
extrn _polarAngle:far
extrn _sin_fast:far
public _polarRadius2D
_polarRadius2D proc far
    arg_z = word ptr 6
    arg_y = word ptr 8
    push bp
    mov bp, sp
    push word ptr [bp+arg_y]
    push word ptr [bp+arg_z]
    call far ptr _polarAngle
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
    call far ptr _cos_fast
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
    call far ptr _sin_fast
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
_polarRadius2D endp
S012_TEXT ends
end
