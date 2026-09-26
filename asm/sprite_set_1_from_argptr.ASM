.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
sprite1 equ 5F20h
public _sprite_set_1_from_argptr
_sprite_set_1_from_argptr proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    cld
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     cx, 0Fh
    mov     ax, cs
    mov     es, ax
    mov     di, offset sprite1
    rep movsw
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
_sprite_set_1_from_argptr endp
_TEXT ends
end
