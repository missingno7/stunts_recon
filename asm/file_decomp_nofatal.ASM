.8086
S012_TEXT segment byte public 'CODE'
assume cs:S012_TEXT
public _file_decomp_nofatal
_file_decomp_nofatal proc far
    push bp
    mov bp, sp
    sub sp, 0Ch
    push si
    push di
    mov word ptr [bp-0Ch], 0
    decomp_body_target equ $+16
    jmp short decomp_body_target
    db 144
_file_decomp_nofatal endp
S012_TEXT ends
end
