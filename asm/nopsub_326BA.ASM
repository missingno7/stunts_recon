.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _nopsub_326BA
_nopsub_326BA proc far
    push bp
    mov bp, sp
    push si
    mov es, word ptr [bp+8]
    mov si, word ptr [bp+6]
    mov bx, word ptr [bp+0Ah]
    shl bx, 1
    shl bx, 1
    mov ax, word ptr es:[bx+si+6]
    mov dx, word ptr es:[bx+si+8]
    mov si, word ptr [bp+0Ch]
    mov word ptr [si], ax
    mov word ptr [si+2], dx
    pop si
    pop bp
    retf
_nopsub_326BA endp
_TEXT ends
end
