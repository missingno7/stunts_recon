.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
sprite2 equ 5F3Eh
extrn _sprite_set_1_from_argptr:far
public _sprite_copy_2_to_1
_sprite_copy_2_to_1 proc far
    mov ax, seg _TEXT
    push ax
    mov ax, offset sprite2
    push ax
    call _sprite_set_1_from_argptr
    add sp, 4
    retf
    db 0
_sprite_copy_2_to_1 endp
_TEXT ends
end
