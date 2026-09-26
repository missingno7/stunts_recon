.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _sprite_set_1_size
_sprite_set_1_size proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     cs:[5F3Ah], ax
    mov     cs:[5F2Ch], ax
    mov     ax, [bp+arg_2]
    mov     cs:[5F3Ch], ax
    mov     cs:[5F2Eh], ax
    mov     ax, [bp+arg_4]
    mov     cs:[5F30h], ax
    mov     ax, [bp+arg_6]
    mov     cs:[5F32h], ax
    pop     bp
    retf
_sprite_set_1_size endp
_TEXT ends
end
