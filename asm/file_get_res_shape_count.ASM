.8086
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _file_get_res_shape_count
_file_get_res_shape_count proc far
 push bp
 mov bp,sp
 push si
 mov es,[bp+8]
 mov si,word ptr [bp+6]
 mov ax,word ptr es:[si+4]
 pop si
 pop bp
 ret
_file_get_res_shape_count endp
S012_TEXT ends
end
