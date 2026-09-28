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
    jg prerender_sphere_when_greater
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
prerender_sphere_when_greater:
    mov word ptr [bp - 6], dx
    mov bx, dx
    shr dx, 1
    jne sphere_when_not_equal
    push word ptr [bp + 0Ch]
    push word ptr [bp + 8]
    push word ptr [bp + 6]
    call far ptr _putpixel_single_maybe
    add sp, 6
prerender_sphere_scan_loop:
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
sphere_when_not_equal:
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
    jge prerender_sphere_scan_loop
    mov word ptr [bp - 14h], ax
    add cx, bx
    cmp cx, word ptr cs:[5F30h]
    jle prerender_sphere_scan_loop
    mov dx, bx
    mov ax, dx
    shr ax, 1
    shr ax, 1
    add dx, ax
    mov cx, word ptr [bp + 6]
    mov ax, cx
    sub ax, dx
    cmp ax, word ptr [bp - 2]
    jg prerender_sphere_scan_loop
    add cx, dx
    cmp cx, word ptr [bp - 4]
    jl prerender_sphere_scan_loop
    cmp bx, word ptr ds:[3C56h]
    jl prerender_sphere_when_less
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
prerender_sphere_when_less:
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
prerender_sphere_scan_loop_2:
    mov bx, word ptr [bp - 16h]
    mov dl, byte ptr [bx]
    inc word ptr [bp - 16h]
    xor dh, dh
    mov bx, word ptr [bp - 18h]
    mov ax, word ptr [bp + 6]
    sub ax, dx
    cmp ax, word ptr [bp - 2]
    jg sphere_when_greater_2
    cmp ax, word ptr [bp - 4]
    jge sphere_when_greater_or_equal
    mov ax, word ptr [bp - 4]
sphere_when_greater_or_equal:
    mov word ptr [si], ax
    mov word ptr [bx + si], ax
    mov ax, word ptr [bp + 6]
    add ax, dx
    cmp ax, word ptr [bp - 4]
    jl sphere_when_greater_2
    cmp ax, word ptr [bp - 2]
    jle sphere_when_less_or_equal
    mov ax, word ptr [bp - 2]
sphere_when_less_or_equal:
    mov word ptr [di], ax
    mov word ptr [bx + di], ax
    add si, 2
    add di, 2
    sub word ptr [bp - 18h], 4
    jge prerender_sphere_scan_loop_2
    jmp sphere_when_condition
sphere_when_greater_2:
    inc word ptr [bp - 14h]
    sub word ptr [bp - 1Ah], 2
    sub word ptr [bp - 18h], 4
    jge prerender_sphere_scan_loop_2
    pop di
    pop si
    mov sp, bp
    pop bp
    retf
sphere_when_condition:
    xor dx, dx
    mov ax, word ptr cs:[5F30h]
    sub ax, word ptr [bp - 14h]
    jle sphere_when_less_or_equal_2
    sub word ptr [bp - 1Ah], ax
    shl ax, 1
    mov dx, ax
    mov ax, word ptr cs:[5F30h]
    mov word ptr [bp - 14h], ax
sphere_when_less_or_equal_2:
    mov ax, word ptr [bp - 14h]
    add ax, word ptr [bp - 1Ah]
    sub ax, word ptr cs:[5F32h]
    jle sphere_when_less_or_equal_3
    sub word ptr [bp - 1Ah], ax
sphere_when_less_or_equal_3:
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
