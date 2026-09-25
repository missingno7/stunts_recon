.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn sprite1:byte
public _sprite_copy_both_to_arg
_sprite_copy_both_to_arg proc far
 push bp
 mov bp,sp
 push ds
 push di
 push si
 lea si,sprite1
 mov di,word ptr [bp+6]
 mov ax,ds
 mov es,ax
 mov ax,cs
 mov ds,ax
 cld
 mov cx,1Eh
 rep movsw
 pop si
 pop di
 pop ds
 pop bp
 ret
_sprite_copy_both_to_arg endp
_TEXT ends
end
