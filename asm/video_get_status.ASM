_TEXT segment word public 'CODE'
assume cs:_TEXT
public _video_get_status
_video_get_status proc far
    mov dx,03DAh
    in al,dx
    and al,08h
    xor ah,ah
    ret
    db 0
_video_get_status endp
_TEXT ends
end
