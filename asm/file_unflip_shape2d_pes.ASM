.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn _file_get_shape2d:far
public _file_unflip_shape2d_pes
_file_unflip_shape2d_pes proc far
    var_val = byte ptr -20
    var_iterations = byte ptr -18
    var_dataptr = word ptr -16
    var_length = word ptr -14
    var_height = word ptr -12
    var_width = word ptr -10
    var_shapeseg = word ptr -8
    var_shapeofs = word ptr -6
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
    sub     sp, 14h
    push    ds
    push    si
    push    di
    mov     ax, [bp+arg_memchunkseg]
    mov     ds, ax
    mov     ax, [bp+arg_memchunkoff]
    mov     si, ax
    mov     bx, [si+4]
    mov     [bp+var_shapecount], bx
    mov     [bp+var_shapecounter], 0
loc_36114:
    push    [bp+var_shapecounter]
    push    [bp+arg_memchunkseg]
    push    [bp+arg_memchunkoff]
    mov     ax, seg dseg
    mov     ds, ax
    call    _file_get_shape2d
    add     sp, 6
    mov     [bp+var_shapeofs], ax
    mov     [bp+var_shapeseg], dx
    mov     si, ax
    mov     ds, dx
    mov     al, [si+0Fh]
loc_36137:
    and     al, 0F0h
    jnz     short loc_3614E
    mov     bl, [si+0Eh]
    shr     bl, 1
    shr     bl, 1
    shr     bl, 1
    shr     bl, 1
    and     bl, 0Fh
    mov     [bp+var_val], bl
    jnz     short loc_3615D
loc_3614E:
    inc     [bp+var_shapecounter]
    dec     [bp+var_shapecount]
    jg      short loc_36114
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_3615D:
    mov     ax, [si+0]
    mov     [bp+var_width], ax
loc_36162:
    mov     dx, [si+2]
    mov     [bp+var_height], dx
    mul     dx
    mov     [bp+var_length], ax
    add     si, 10h
loc_36170:
    mov     [bp+var_dataptr], si
loc_36173:
    mov     [bp+var_iterations], 4
loc_36177:
    shr     [bp+var_val], 1
    jnb     short loc_361AF
loc_3617C:
    mov     es, [bp+arg_mempagesseg]
    mov     di, [bp+arg_mempagesoff]
loc_36182:
    mov     ds, [bp+var_shapeseg]
    xor     dx, dx
loc_36187:
    mov     bx, [bp+var_height]
loc_3618A:
    mov     si, [bp+var_dataptr]
    add     si, dx
    mov     cx, [bp+var_width]
loc_36192:
    mov     al, [si]
    stosb
loc_36195:
    add     si, bx
loc_36197:
    loop    loc_36192
    inc     dx
loc_3619A:
    cmp     dx, bx
    jnz     short loc_3618A
loc_3619E:
    mov     es, [bp+var_shapeseg]
loc_361A1:
    mov     di, [bp+var_dataptr]
loc_361A4:
    mov     ds, [bp+arg_mempagesseg]
loc_361A7:
    mov     si, [bp+arg_mempagesoff]
loc_361AA:
    mov     cx, [bp+var_length]
loc_361AD:
    rep movsb
loc_361AF:
    mov     ax, [bp+var_length]
loc_361B2:
    add     [bp+var_dataptr], ax
loc_361B5:
    dec     [bp+var_iterations]
loc_361B8:
    jnz     short loc_36177
loc_361BA:
    jmp     short loc_3614E
_file_unflip_shape2d_pes endp
_TEXT ends
dseg segment byte public 'STUNTSD'
dseg ends
end

