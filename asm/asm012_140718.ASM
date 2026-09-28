.8086
_DATA segment word public 'DATA'
_currentvideomode db 0
_byte_403F3 db 0
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
assume ds:DGROUP
extrn _add_exit_handler:far
extrn _video_clear_color:far
public _video_add_exithandler
public _video_on_exit
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP

; video_add_exithandler
assume cs:S012_TEXT, ds:DGROUP
_video_add_exithandler proc far
    cmp byte ptr ds:[_currentvideomode], 0
    jne loc_225D5
    mov ah, 0Fh
    int 10h
    mov byte ptr ds:[_currentvideomode], al
    mov ax, 40h
    mov es, ax
    mov al, byte ptr es:[10h]
    mov byte ptr ds:[_byte_403F3], al
    push cs
    mov ax, offset _video_on_exit
    push ax
    call far ptr _add_exit_handler
    add sp, 4
loc_225D5:
    retf
_video_add_exithandler endp

; video_on_exit
assume cs:S012_TEXT, ds:DGROUP
_video_on_exit proc far
    mov ax, 40h
    mov es, ax
    mov al, byte ptr ds:[_byte_403F3]
    mov byte ptr es:[10h], al
    mov ah, 0
    mov al, byte ptr ds:[_currentvideomode]
    int 10h
    mov ax, 40h
    mov es, ax
    mov al, byte ptr ds:[_byte_403F3]
    mov byte ptr es:[10h], al
    and al, 30h
    cmp al, 30h
    jne loc_22606
    xor ax, ax
    push ax
    call far ptr _video_clear_color
    add sp, 2
loc_22606:
    mov ah, 0Bh
    mov bx, 0
    int 10h
    retf
_video_on_exit endp

S012_TEXT ends
end
