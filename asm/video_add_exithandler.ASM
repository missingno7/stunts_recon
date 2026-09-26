.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn add_exit_handler:far
public _video_add_exithandler
_video_add_exithandler proc far
    cmp byte ptr ds:[4C82h], 0
    jne loc_225D5
    mov ah, 0Fh
    int 10h
    mov byte ptr ds:[4C82h], al
    mov ax, 40h
    mov es, ax
    mov al, byte ptr es:[10h]
    mov byte ptr ds:[4C83h], al
    push cs
    mov ax, 3BB6h
    push ax
    call far ptr add_exit_handler
    add sp, 4
loc_225D5:
    retf
_video_add_exithandler endp
_TEXT ends
end
