.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _video_set_mode4
_video_set_mode4 proc far
 push di
 mov byte ptr ds:[40EAh],1
 mov ax,40h
 mov es,ax
 mov ax,word ptr es:[10h]
 and ax,0FFCFh
 or ax,20h
 mov word ptr es:[10h],ax
 mov ah,0
 mov al,4
 int 10h
 mov al,3
 mov dx,3BFh
 out dx,al
 mov al,2
 mov dx,3B8h
 out dx,al
 mov cx,0Ch
 mov dx,3B4h
 xor bx,bx
mode4_loop:
 mov al,bl
 out dx,al
 inc dx
 mov al,byte ptr ds:[bx+40ECh]
 out dx,al
 dec dx
 inc bx
 loop mode4_loop
 cld
 mov ax,0B800h
 mov es,ax
 xor di,di
 mov cx,4000h
 xor ax,ax
 rep stosw
 mov al,8Ah
 mov dx,3B8h
 out dx,al
 pop di
 ret
 db 0
_video_set_mode4 endp
_TEXT ends
end
