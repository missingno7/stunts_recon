.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn _draw_filled_lines:far
extrn _preRender_sphere_helper:far
extrn _putpixel_single_maybe:far
public _preRender_sphere
_preRender_sphere proc far
    push bp
    mov bp, sp
    sub sp, 79Ah
    push si
    push di
    mov dx, word ptr [bp + 0Ah]
    mov ax, dx
    shr ax, 1
    shr ax, 1
    sub dx, ax
    shr ax, 1
    shr ax, 1
    add dx, ax
    or dx, dx
    jg loc_23096
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
loc_23096:
    mov word ptr [bp - 6], dx
    mov bx, dx
    shr dx, 1
    jne loc_230B6
    push word ptr [bp + 0Ch]
    push word ptr [bp + 8]
    push word ptr [bp + 6]
    call far ptr _putpixel_single_maybe
    add sp, 6
loc_230B0:
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
loc_230B6:
    sub bx, dx
    mov ax, word ptr cs:[5F3Ah]
    mov word ptr [bp - 4], ax
    mov ax, word ptr cs:[5F3Ch]
    dec ax
    mov word ptr [bp - 2], ax
    mov cx, word ptr [bp + 8]
    mov ax, cx
    sub ax, dx
    cmp ax, word ptr cs:[5F32h]
    jge loc_230B0
    mov word ptr [bp - 14h], ax
    add cx, bx
    cmp cx, word ptr cs:[5F30h]
    jle loc_230B0
    mov dx, bx
    mov ax, dx
    shr ax, 1
    shr ax, 1
    add dx, ax
    mov cx, word ptr [bp + 6]
    mov ax, cx
    sub ax, dx
    cmp ax, word ptr [bp - 2]
    jg loc_230B0
    add cx, dx
    cmp cx, word ptr [bp - 4]
    jl loc_230B0
    cmp bx, word ptr ds:[3C56h]
    jl loc_23148
    mov ax, word ptr [bp + 6]
    mov word ptr [bp - 12h], ax
    mov word ptr [bp - 0Eh], ax
    mov dx, word ptr [bp + 0Ah]
    shr dx, 1
    add ax, dx
    mov word ptr [bp - 0Ah], ax
    mov ax, word ptr [bp + 8]
    mov word ptr [bp - 10h], ax
    mov word ptr [bp - 8], ax
    mov dx, word ptr [bp - 6]
    shr dx, 1
    add ax, dx
    mov word ptr [bp - 0Ch], ax
    mov dx, word ptr [bp + 0Ch]
    lea ax, [bp - 12h]
    add sp, 780h
    push dx
    push ax
    call far ptr _preRender_sphere_helper
    add sp, 4
    sub sp, 780h
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
loc_23148:
    shl bx, 1
    mov ax, word ptr [bx + 3C58h]
    mov word ptr [bp - 16h], ax
    mov ax, word ptr [bp - 6]
    mov word ptr [bp - 1Ah], ax
    dec ax
    shl ax, 1
    mov word ptr [bp - 18h], ax
    lea si, [bp - 3DAh]
    lea di, [bp - 79Ah]
loc_23165:
    mov bx, word ptr [bp - 16h]
    mov dl, byte ptr [bx]
    inc word ptr [bp - 16h]
    xor dh, dh
    mov bx, word ptr [bp - 18h]
    mov ax, word ptr [bp + 6]
    sub ax, dx
    cmp ax, word ptr [bp - 2]
    jg loc_231AD
    cmp ax, word ptr [bp - 4]
    jge loc_23184
    mov ax, word ptr [bp - 4]
loc_23184:
    mov word ptr [si], ax
    mov word ptr [bx + si], ax
    mov ax, word ptr [bp + 6]
    add ax, dx
    cmp ax, word ptr [bp - 4]
    jl loc_231AD
    cmp ax, word ptr [bp - 2]
    jle loc_2319A
    mov ax, word ptr [bp - 2]
loc_2319A:
    mov word ptr [di], ax
    mov word ptr [bx + di], ax
    add si, 2
    add di, 2
    sub word ptr [bp - 18h], 4
    jge loc_23165
    jmp loc_231C0
loc_231AD:
    inc word ptr [bp - 14h]
    sub word ptr [bp - 1Ah], 2
    sub word ptr [bp - 18h], 4
    jge loc_23165
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
loc_231C0:
    xor dx, dx
    mov ax, word ptr cs:[5F30h]
    sub ax, word ptr [bp - 14h]
    jle loc_231D9
    sub word ptr [bp - 1Ah], ax
    shl ax, 1
    mov dx, ax
    mov ax, word ptr cs:[5F30h]
    mov word ptr [bp - 14h], ax
loc_231D9:
    mov ax, word ptr [bp - 14h]
    add ax, word ptr [bp - 1Ah]
    sub ax, word ptr cs:[5F32h]
    jle loc_231E9
    sub word ptr [bp - 1Ah], ax
loc_231E9:
    push word ptr [bp + 0Ch]
    push word ptr [bp - 1Ah]
    push word ptr [bp - 14h]
    lea ax, [bp - 79Ah]
    add ax, dx
    push ax
    lea ax, [bp - 3DAh]
    add ax, dx
    push ax
    call far ptr _draw_filled_lines
    add sp, 0Ah
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
_preRender_sphere endp
_TEXT ends
end
