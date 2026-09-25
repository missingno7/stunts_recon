.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _putpixel_single_maybe
_putpixel_single_maybe proc far
 push bp
 mov bp,sp
 push si
 push di
 mov ax,word ptr [bp+6]
 cmp ax,word ptr cs:[5F2Ch]
 jl short reject_pixel
 cmp ax,word ptr cs:[5F2Eh]
 jge short reject_pixel
 mov ax,word ptr [bp+8]
 cmp ax,word ptr cs:[5F30h]
 jl short reject_pixel
 cmp ax,word ptr cs:[5F32h]
 jl short write_pixel
reject_pixel:
 pop di
 pop si
 pop bp
 ret
 push bp
 mov bp,sp
 push si
 push di
write_pixel:
 mov di,word ptr [bp+6]
 mov si,word ptr [bp+8]
 shl si,1
 add si,word ptr cs:[5F2Ah]
 add di,word ptr cs:[si]
 mov es,word ptr cs:[5F22h]
 mov ax,word ptr [bp+0Ah]
 mov byte ptr es:[di],al
 pop di
 pop si
 pop bp
 ret
 db 0
_putpixel_single_maybe endp
_TEXT ends
end
