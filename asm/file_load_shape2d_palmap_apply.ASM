.8086
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
extrn _file_get_shape2d:far
public _file_load_shape2d_palmap_apply
_file_load_shape2d_palmap_apply proc far
    shapecounter = word ptr -4
    var_shapecount = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_memchunkoff = word ptr 6
    arg_memchunkseg = word ptr 8
    arg_palmap = word ptr 10

    push    bp
    mov     bp, sp
    sub     sp, 4
    push    di
    push    si
    mov     es, [bp+arg_memchunkseg]
    mov     di, [bp+arg_memchunkoff]
    mov     si, [bp+arg_palmap]
    mov     cx, es:[di+4]
    mov     [bp+var_shapecount], cx
    xor     ax, ax
    mov     [bp+shapecounter], ax
shape_palmap_wait_loop:
    mov     ax, [bp+shapecounter]
    cmp     ax, [bp+var_shapecount]
    jl      short shape_palmap_when_less
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
shape_palmap_when_less:
    push    ax
    push    [bp+arg_memchunkseg]
    push    [bp+arg_memchunkoff]
    call    _file_get_shape2d
    add     sp, 6
    mov     es, dx
    mov     di, ax
    mov     ax, es:[di+0]
    mul     word ptr es:[di+2]
    mov     cx, ax
    add     di, 10h
    xor     bx, bx
shape_palmap_fill_loop:
    mov     bl, es:[di]
    mov     al, [bx+si]
    stosb
    loop    shape_palmap_fill_loop
    inc     [bp+shapecounter]
    jmp     short shape_palmap_wait_loop
    ; align 2
    db 0
_file_load_shape2d_palmap_apply endp
S012_TEXT ends
end
