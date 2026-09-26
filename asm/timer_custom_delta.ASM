.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _timer_custom_delta
_timer_custom_delta proc far
    push bp
    mov bp, sp
    mov bx, word ptr [bp + 6]
    mov cx, word ptr [bp + 8]
    cli
    mov ax, word ptr ds:[4108h]
    mov dx, word ptr ds:[410Ah]
    sti
    sub ax, bx
    sbb dx, cx
    pop bp
    retf
_timer_custom_delta endp
_TEXT ends
end
