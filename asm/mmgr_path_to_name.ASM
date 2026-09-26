.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _mmgr_path_to_name
_mmgr_path_to_name proc far
    push bp
    mov bp, sp
    push si
    mov si, word ptr [bp+6]
    mov bx, si
    cld
path_scan:
    lodsb
    or al, al
    jz path_done
    cmp al, 3Ah
    jz path_component
    cmp al, 5Ch
    jnz path_scan
path_component:
    mov bx, si
    jmp short path_scan
path_done:
    mov ax, bx
    pop si
    pop bp
    retf
_mmgr_path_to_name endp
_TEXT ends
end
