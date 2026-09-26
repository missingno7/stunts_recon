.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn video_clear_color:far
public _video_on_exit
_video_on_exit proc far
    mov ax, 40h
    mov es, ax
    mov al, byte ptr ds:[4C83h]
    mov byte ptr es:[10h], al
    mov ah, 0
    mov al, byte ptr ds:[4C82h]
    int 10h
    mov ax, 40h
    mov es, ax
    mov al, byte ptr ds:[4C83h]
    mov byte ptr es:[10h], al
    and al, 30h
    cmp al, 30h
    jne loc_22606
    xor ax, ax
    push ax
    call far ptr video_clear_color
    add sp, 2
loc_22606:
    mov ah, 0Bh
    mov bx, 0
    int 10h
    retf
_video_on_exit endp
_TEXT ends
end
