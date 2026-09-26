.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _timer_get_delta
_timer_get_delta proc far
    mov bx, word ptr ds:[4E8Ah]
    mov cx, word ptr ds:[4E8Ch]
    cli
    mov ax, word ptr ds:[4108h]
    mov dx, word ptr ds:[410Ah]
    sti
    mov word ptr ds:[4E8Ah], ax
    mov word ptr ds:[4E8Ch], dx
    sub ax, bx
    sbb dx, cx
    retf
_timer_get_delta endp
_TEXT ends
end
