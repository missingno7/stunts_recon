.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _video_clear_color
_video_clear_color proc far
    push bp
    mov bp,sp
    push di
    mov ax,0A000h
    mov es,ax
    mov ax,word ptr [bp+6]
    mov di,0
    mov cx,0FA00h
    rep stosw
    pop di
    pop bp
    retf
    db 0
_video_clear_color endp
_TEXT ends
end
