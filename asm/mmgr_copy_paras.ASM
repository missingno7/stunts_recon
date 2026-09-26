.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _mmgr_copy_paras
_mmgr_copy_paras proc far
    push bp
    mov bp, sp
    push ds
    push si
    push di
    cld
    mov ds, word ptr [bp+6]
    mov es, word ptr [bp+8]
    mov bx, word ptr [bp+0Ah]
copy_loop:
    or bx, bx
    jz copy_done
    mov cx, 8000h
    mov ax, bx
    sub bx, 1000h
    jnb copy_full
    add bx, 1000h
    shl bx, 1
    shl bx, 1
    shl bx, 1
    mov cx, bx
    xor bx, bx
copy_full:
    xor si, si
    xor di, di
    rep movsw
    mov ax, ds
    add ax, 1000h
    mov ds, ax
    mov ax, es
    add ax, 1000h
    mov es, ax
    jmp short copy_loop
copy_done:
    pop di
    pop si
    pop ds
    pop bp
    retf
_mmgr_copy_paras endp
_TEXT ends
end
