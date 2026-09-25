.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn sprite1:byte
public _sprite_copy_arg_to_both
_sprite_copy_arg_to_both proc far
 push bp
 mov bp,sp
 push ds
 push di
 push si
 mov si,word ptr [bp+6]
 mov ax,cs
 mov es,ax
 lea di,sprite1
 cld
 mov cx,1Eh
 rep movsw
 pop si
 pop di
 pop ds
 pop bp
 ret
_sprite_copy_arg_to_both endp
_TEXT ends
end
