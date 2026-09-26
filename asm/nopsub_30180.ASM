.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _word_3F884:word
extrn _word_3F886:word
extrn _word_3F882:word
extrn _byte_3F880:byte
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn add_exit_handler:far
public _nopsub_30180,_timer_setup_interrupt
_nopsub_30180 proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    mov     dx, 2E9Ch
    mov     _word_3F884, 5
    mov     _word_3F886, 5
    mov     _word_3F882, 64h ; 'd'
    mov     _byte_3F880, 1
    jmp     short shared_setup
    ; align 2
    db 144

_nopsub_30180 endp
_timer_setup_interrupt proc far
 push bp
 mov dx,2E9Ch
 mov word ptr ds:[4114h],5
 mov word ptr ds:[4116h],5
 mov byte ptr ds:[4110h],0
 mov byte ptr ds:[4111h],1
 jmp shared_setup
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov word ptr ds:[4114h],ax
 mov word ptr ds:[4116h],ax
 mov dx,word ptr [bp+6]
 mov byte ptr ds:[4110h],0
 mov byte ptr ds:[4111h],1
 jmp shared_setup
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov word ptr ds:[4114h],ax
 mov word ptr ds:[4116h],ax
 mov dx,word ptr [bp+6]
 mov word ptr ds:[4112h],64h
 mov byte ptr ds:[4110h],1
shared_setup:
 cli
 mov byte ptr ds:[411Ch],0
 mov word ptr ds:[4120h],0
 mov word ptr ds:[4122h],0
 sti
 in al,61h
 and al,0FCh
 out 61h,al
 mov al,0B6h
 out 43h,al
 in al,21h
 or al,3
 out 21h,al
 xor ax,ax
 mov es,ax
 cli
 mov ax,word ptr es:[20h]
 cmp ax,1909h
 je setup_after_old_off
 mov word ptr ds:[4104h],ax
setup_after_old_off:
 mov ax,word ptr es:[22h]
 mov bx,cs
 cmp ax,bx
 je setup_after_old_seg
 mov word ptr ds:[4106h],ax
 mov word ptr es:[20h],1909h
 mov word ptr es:[22h],cs
setup_after_old_seg:
 in al,21h
 and al,0FCh
 out 21h,al
 sti
 mov al,dl
 out 40h,al
 mov al,dh
 out 40h,al
 push cs
 mov ax,1848h
 push ax
 call far ptr add_exit_handler
 add sp,4
 pop bp
 ret

_timer_setup_interrupt endp
_TEXT ends
end
