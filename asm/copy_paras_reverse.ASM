.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _copy_paras_reverse
_copy_paras_reverse proc far
    push bp
    mov bp, sp
    push ds
    push si
    push di
    std
    mov bx, word ptr [bp+0Ah]
    mov ax, word ptr [bp+6]
    add ax, bx
    mov ds, ax
    mov ax, word ptr [bp+8]
    add ax, bx
    mov es, ax
copy_reverse_loop:
    or bx, bx
    jz copy_reverse_done
    mov cx, 1000h
    sub bx, 1000h
    jnb copy_reverse_full
    add bx, 1000h
    mov cx, bx
    xor bx, bx
copy_reverse_full:
    mov ax, ds
    sub ax, cx
    mov ds, ax
    mov ax, es
    sub ax, cx
    mov es, ax
    shl cx, 1
    shl cx, 1
    shl cx, 1
    mov ax, cx
    shl ax, 1
    dec ax
    dec ax
    mov si, ax
    mov di, ax
    rep movsw
    jmp short copy_reverse_loop
copy_reverse_done:
    cld
    pop di
    pop si
    pop ds
    pop bp
    retf
_copy_paras_reverse endp
_TEXT ends
end
