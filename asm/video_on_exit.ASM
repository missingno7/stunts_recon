.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _byte_403F3:byte
extrn _currentvideomode:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
extrn _video_clear_color:far
public _video_on_exit
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
    jne on_exit_return_if_not_equal
    xor ax, ax
    push ax
    call far ptr _video_clear_color
    add sp, 2
on_exit_return_if_not_equal:
    mov ah, 0Bh
    mov bx, 0
    int 10h
    retf
_video_on_exit endp
S012_TEXT ends
end
