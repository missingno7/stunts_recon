.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn _file_get_shape2d:far
public _file_load_shape2d_expand
_file_load_shape2d_expand proc far
    var_dstshapeoff = word ptr -16
    var_length = word ptr -14
    var_srcshapeoff = word ptr -12
    var_srcshapeseg = word ptr -10
    var_srcdataoff = word ptr -8
    var_offsets = word ptr -6
    var_shapecounter = word ptr -4
    var_shapecount = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_memchunkoff = word ptr 6
    arg_memchunkseg = word ptr 8
    arg_mempagesoff = word ptr 10
    arg_mempagesseg = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    di
    push    si
    push    ds
    mov     ds, [bp+arg_memchunkseg]
    mov     si, [bp+arg_memchunkoff]
    mov     es, [bp+arg_mempagesseg]
    mov     di, [bp+arg_mempagesoff]
    mov     cx, [si+4]
    mov     [bp+var_shapecount], cx
    shl     cx, 1
    add     cx, 1
    add     si, 4
    add     di, 4
    rep movsw
    xor     ax, ax
loc_35FCC:
    mov     [bp+var_shapecounter], ax
    mov     [bp+var_offsets], di
    mov     es:[di], ax
    mov     es:[di+2], ax
loc_35FD9:
    mov     ax, [bp+var_shapecounter]
    cmp     ax, [bp+var_shapecount]
    jl      short loc_35FE8
    pop     ds
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
loc_35FE8:
    push    ax
loc_35FE9:
    push    [bp+arg_memchunkseg]
loc_35FEC:
    push    [bp+arg_memchunkoff]
    mov     ax, ss
    mov     ds, ax
    call    _file_get_shape2d
    add     sp, 6
    mov     ds, dx
    mov     si, ax
    mov     [bp+var_srcshapeseg], dx
    mov     [bp+var_srcshapeoff], ax
    mov     es, [bp+arg_mempagesseg]
    mov     di, [bp+var_offsets]
    mov     bx, es:[di]
    mov     cx, es:[di+2]
    add     di, 4
    mov     [bp+var_offsets], di
    mov     ax, [si]
    mul     word ptr [si+2]
    mov     [bp+var_length], ax
    shl     ax, 1
    shl     ax, 1
    shl     ax, 1
    add     ax, 10h
    add     ax, bx
    adc     dx, cx
    mov     bx, [bp+var_shapecounter]
    inc     bx
    cmp     bx, [bp+var_shapecount]
    jge     short loc_36040
    mov     es:[di], ax
    mov     es:[di+2], dx
    jmp     short loc_3605B
    ; align 2
    db 144
loc_36040:
    mov     bx, [bp+var_shapecount]
    shl     bx, 1
    shl     bx, 1
    shl     bx, 1
    add     bx, 6
    add     ax, bx
    adc     dx, 0
    mov     di, [bp+arg_mempagesoff]
    mov     es:[di], ax
    mov     es:[di+2], dx
loc_3605B:
    push    [bp+var_shapecounter]
    push    [bp+arg_mempagesseg]
    push    [bp+arg_mempagesoff]
    mov     ax, ss
    mov     ds, ax
    call    _file_get_shape2d
loc_3606D:
    add     sp, 6
loc_36070:
    mov     es, dx
    mov     di, ax
    mov     [bp+var_dstshapeoff], ax
    mov     ds, [bp+var_srcshapeseg]
    mov     cx, 6
    rep movsw
    mov     si, [bp+var_srcshapeoff]
loc_36082:
    mov     di, [bp+var_dstshapeoff]
    mov     ax, [si]
    shl     ax, 1
    shl     ax, 1
    shl     ax, 1
    mov     es:[di], ax
    mov     al, [si+0Dh]
loc_36093:
    shr     al, 1
    shr     al, 1
    shr     al, 1
    shr     al, 1
    mov     ah, al
    add     di, 10h
loc_360A0:
    mov     cx, [bp+var_length]
    cmp     cx, 0
    jle     short loc_360EF
    cmp     cx, 1F40h
    jg      short loc_360EF
    shl     cx, 1
    shl     cx, 1
    rep stosw
loc_360B4:
    add     si, 10h
    mov     [bp+var_srcdataoff], si
    xor     bx, bx
loc_360BC:
    mov     si, [bp+var_srcshapeoff]
    mov     ah, [bx+si+0Ch]
    and     ah, 0Fh
    jz      short loc_360EF
    mov     di, [bp+var_dstshapeoff]
    add     di, 10h
    mov     si, [bp+var_srcdataoff]
loc_360D0:
    mov     dx, [bp+var_length]
loc_360D3:
    lodsb
    mov     cx, 8
loc_360D7:
    shl     al, 1
    jnb     short loc_360DE
    or      es:[di], ah
loc_360DE:
    inc     di
    loop    loc_360D7
    dec     dx
    jg      short loc_360D3
    mov     [bp+var_srcdataoff], si
    shl     ah, 1
    inc     bx
    cmp     bx, 4
    jl      short loc_360BC
loc_360EF:
    inc     [bp+var_shapecounter]
    jmp     loc_35FD9
    ; align 2
    db 0
_file_load_shape2d_expand endp
_TEXT ends
end
