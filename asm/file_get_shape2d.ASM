.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _file_get_shape2d
_file_get_shape2d proc far
 push bp
 mov bp,sp
 push si
 mov es,[bp+8]
 mov si,word ptr [bp+6]
 mov ax,word ptr es:[si+4]
 shl ax,1
 shl ax,1
 mov bx,word ptr [bp+10]
 shl bx,1
 shl bx,1
 add bx,ax
 add bx,6
 shl ax,1
 add ax,6
 mov cx,es
 xor dx,dx
 shl cx,1
 rcl dx,1
 shl cx,1
 rcl dx,1
 shl cx,1
 rcl dx,1
 shl cx,1
 rcl dx,1
 add ax,cx
 adc dx,0
 add ax,word ptr es:[bx]
 adc dx,word ptr es:[bx+2]
 mov bx,ax
 and bx,0Fh
 shr dx,1
 rcr ax,1
 shr dx,1
 rcr ax,1
 shr dx,1
 rcr ax,1
 shr dx,1
 rcr ax,1
 mov dx,ax
 mov ax,bx
 pop si
 pop bp
 ret
_file_get_shape2d endp
_TEXT ends
end
