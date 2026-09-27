.8086
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _file_decomp
_file_decomp proc far
    push bp
    mov bp, sp
    sub sp, 0Ch
    push si
    push di
    mov ax, [bp+8]
    mov [bp-0Ch], ax
    decomp_body_target equ $+32
    jmp short decomp_body_target
    db 144
_file_decomp endp
S012_TEXT ends
end
