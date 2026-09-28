.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _callbacks:byte
extrn _callbackflags:byte
extrn _callbackflags2:byte
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_reg_callback, _nopsub_304AF
_kb_reg_callback proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_2]
    mov     cx, 40h ; '@'
    mov     bx, offset _callbacks
    mov     dx, [bp+arg_4]
callback_find_slot_loop:
    cmp     [bx], ax
    jnz     short callback_continue_slot_search
    cmp     [bx+2], dx
    jz      short callback_derive_key_flag_index
callback_continue_slot_search:
    cmp     word ptr [bx+2], 0
    jz      short callback_install_empty_slot
    add     bx, 4
    loop    callback_find_slot_loop
    pop     bp
    retf
callback_install_empty_slot:
    mov     [bx], ax
    mov     [bx+2], dx
callback_derive_key_flag_index:
    mov     ax, 41h ; 'A'
    sub     ax, cx
callback_store_normal_key_flag:
    mov     bx, [bp+arg_0]
    or      bl, bl
    jz      short callback_store_extended_flag
    cmp     bx, 7Fh ; ''
    jg      short callback_return_unmapped_key
    mov     _callbackflags[bx], al
callback_return_unmapped_key:
    pop     bp
    retf
callback_store_extended_flag:
    mov     bl, bh
    xor     bh, bh
    cmp     bx, 84h ; ''
    jg      short callback_return_unmapped_key
    mov     _callbackflags2[bx], al
    pop     bp
    retf

_kb_reg_callback endp
_nopsub_304AF proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    xor     ax, ax
    jmp     short callback_store_normal_key_flag

_nopsub_304AF endp
_TEXT ends
end
