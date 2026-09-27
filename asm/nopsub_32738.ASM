.8086
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_32738
_nopsub_32738 proc far
    push bp
    mov bp, sp
    mov dx, word ptr [bp+8]
    mov ax, word ptr [bp+6]
    div word ptr [bp+0Ah]
    pop bp
    retf
_nopsub_32738 endp
S012_TEXT ends
end
