_TEXT segment word public 'CODE'
assume cs:_TEXT
public _timer_get_counter
_timer_get_counter proc far
    cli
    mov ax,word ptr ds:[4108h]
    mov dx,word ptr ds:[410Ah]
    sti
    ret
_timer_get_counter endp
_TEXT ends
end
