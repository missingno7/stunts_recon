.8086
extrn _byte_3F880:byte
extrn _byte_3F881:byte
extrn _byte_3F88C:byte
extrn _dword_3F874:word
extrn _timerintr:word
extrn _word_2F876:word
extrn _word_2F892:word
extrn _word_3F882:word
extrn _word_3F884:word
extrn _word_3F886:word
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _word_3F884:word
extrn _word_3F886:word
extrn _word_3F882:word
extrn _byte_3F880:byte
DSEG ends
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _add_exit_handler:far
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
 mov word ptr ds:[_word_3F884],5
 mov word ptr ds:[_word_3F886],5
 mov byte ptr ds:[_byte_3F880],0
 mov byte ptr ds:[_byte_3F881],1
 jmp shared_setup
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov word ptr ds:[_word_3F884],ax
 mov word ptr ds:[_word_3F886],ax
 mov dx,word ptr [bp+6]
 mov byte ptr ds:[_byte_3F880],0
 mov byte ptr ds:[_byte_3F881],1
 jmp shared_setup
 push bp
 mov bp,sp
 mov dx,1
 xor ax,ax
 idiv word ptr [bp+6]
 mov word ptr ds:[_word_3F884],ax
 mov word ptr ds:[_word_3F886],ax
 mov dx,word ptr [bp+6]
 mov word ptr ds:[_word_3F882],64h
 mov byte ptr ds:[_byte_3F880],1
shared_setup:
 cli
 mov byte ptr ds:[_byte_3F88C],0
 mov word ptr ds:[_timerintr],0
 mov word ptr ds:[_word_2F892],0
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
 mov word ptr ds:[_dword_3F874],ax
setup_after_old_off:
 mov ax,word ptr es:[22h]
 mov bx,cs
 cmp ax,bx
 je setup_after_old_seg
 mov word ptr ds:[_word_2F876],ax
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
 call far ptr _add_exit_handler
 add sp,4
 pop bp
 ret

_timer_setup_interrupt endp
S012_TEXT ends
end
