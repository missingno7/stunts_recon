.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn video_clear_color:far
extrn video_add_exithandler:far
public _video_set_mode_13h
_video_set_mode_13h proc far
 call far ptr video_add_exithandler
 mov ax,40h
 mov es,ax
 mov ax,word ptr es:[10h]
 and ax,0FFCFh
 or ax,10h
 mov word ptr es:[10h],ax
 mov ah,0Bh
 mov bx,0
 int 10h
 mov ah,0
 mov al,13h
 int 10h
 xor ax,ax
 push ax
 push ax
 call far ptr video_clear_color
 add sp,4
 ret
_video_set_mode_13h endp
_TEXT ends
end
