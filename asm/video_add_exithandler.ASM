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
extrn _add_exit_handler:far
public _video_add_exithandler
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
    mov ax, 3BB6h
    push ax
    call far ptr _add_exit_handler
    add sp, 4
loc_225D5:
    retf
_video_add_exithandler endp
S012_TEXT ends
end
