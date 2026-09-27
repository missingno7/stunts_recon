.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _byte_3F85A:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
extrn _set_bios_mode3:far
public _video_set_mode7
_video_set_mode7 proc far
    push di
    cmp byte ptr ds:[_byte_3F85A], 0
    jne loc_2012F
    call far ptr _set_bios_mode3
    pop di
    retf
loc_2012F:
    mov ax, 40h
    mov es, ax
    mov ax, word ptr es:[10h]
    and ax, 0FFCFh
    or ax, 30h
    mov word ptr es:[10h], ax
    mov al, 3
    mov dx, 3BFh
    out dx, al
    mov al, 20h
    mov dx, 3B8h
    out dx, al
    mov cx, 0Ch
    mov dx, 3B4h
    xor bx, bx
loc_20156:
    mov al, bl
    out dx, al
    inc dx
    mov al, byte ptr [bx + 40F8h]
    out dx, al
    dec dx
    inc bx
    loop loc_20156
    cld
    mov ax, 0B800h
    mov es, ax
    xor di, di
    mov cx, 4000h
    xor ax, ax
    rep stosw
    mov al, 28h
    mov dx, 3B8h
    out dx, al
    mov ah, 0
    mov al, 7
    int 10h
    pop di
    retf
_video_set_mode7 endp
S012_TEXT ends
end
