.8086
S012_TEXT segment word public 'CODE'
S012_TEXT ends
DSEG segment byte public 'STUNTSD'
DSEG ends
DGROUP group DSEG
assume cs:S012_TEXT, ds:DGROUP
extrn _mmgr_get_chunk_by_name:far
extrn _file_decomp_paras:far
extrn _mmgr_alloc_pages:far
extrn _file_paras:far
extrn _file_read:far
extrn _copy_paras_reverse:far
extrn _fatal_error:far
extrn _mmgr_resize_memory:far
extrn _file_decomp_rle:far
extrn _file_decomp_vle:far
extrn _aSInvalidPackTy:byte
public _file_decomp, _file_decomp_nofatal, _file_decomp_fatal
S012_TEXT segment word public 'CODE'
_file_decomp proc far
    var_fatal = word ptr -12
     s = byte ptr 0
     r = byte ptr 2
    arg_fatal = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     ax, [bp+arg_fatal]
    mov     [bp+var_fatal], ax
    jmp     short decomp_body
    db 144
_file_decomp endp
_file_decomp_nofatal proc far
    var_fatal = word ptr -12
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short decomp_body
    db 144
_file_decomp_nofatal endp
_file_decomp_fatal proc far
    var_fatal = word ptr -12
    var_nextsrcseg = word ptr -10
    var_passes = word ptr -8
    var_decomp_paras = word ptr -6
    var_dstseg = word ptr -4
    var_dstoff = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     [bp+var_fatal], 1
decomp_body:
    push    [bp+arg_filename]
    call    _mmgr_get_chunk_by_name
    add     sp, 2
    or      dx, dx
    jz      short fd_not_found
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_30E29:
    jmp     loc_30F0F
fd_not_found:
    push    [bp+var_fatal]
    push    [bp+arg_filename]
    call    _file_decomp_paras
    add     sp, 4
    or      ax, ax
    jz      short loc_30E29
    add     ax, 4
    mov     [bp+var_decomp_paras], ax
    push    ax
    push    [bp+arg_filename]
    call    _mmgr_alloc_pages
    add     sp, 4
    mov     [bp+var_dstoff], ax
    mov     [bp+var_dstseg], dx
    push    [bp+var_fatal]
    push    [bp+arg_filename]
    call    _file_paras
    add     sp, 4
    or      ax, ax
    jz      short loc_30E29
    mov     dx, [bp+var_decomp_paras]
    sub     dx, ax
    add     dx, [bp+var_dstseg]
    push    [bp+var_fatal]
    push    dx
    push    [bp+var_dstoff]
    push    [bp+arg_filename]
    call    _file_read
    add     sp, 8
    or      dx, dx
    jz      short loc_30E29
    mov     [bp+var_passes], 1
    mov     ds, dx
    mov     si, ax
    mov     al, [si]
    test    al, 80h
    jz      short decompress_subfile
    and     ax, 7Fh
    mov     [bp+var_passes], ax
    add     si, 4
decompress_subfile:
    push    [bp+var_decomp_paras]
    push    [bp+var_dstseg]
    push    [bp+var_dstoff]
    push    ds
    mov     bl, [si]
    dec     bl
    jl      short loc_30F0C
    cmp     bl, 2
    jge     short loc_30F0C
    push    si
    xor     bh, bh
    shl     bx, 1
    mov     ax, seg dseg
    mov     ds, ax
    shl     bx, 1
    call    dword ptr cs:compression_type[bx]
    add     sp, 0Ah
    dec     [bp+var_passes]
    jle     short fd_done
    mov     si, ax
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    test    si, 0Fh
    jz      short loc_30EE5
    inc     ax
loc_30EE5:
    push    ax
    mov     bx, [bp+var_decomp_paras]
    sub     bx, ax
    add     bx, [bp+var_dstseg]
    mov     [bp+var_nextsrcseg], bx
    push    bx
    push    [bp+var_dstseg]
    call    _copy_paras_reverse
    add     sp, 6
    mov     ds, [bp+var_nextsrcseg]
    xor     si, si
    jmp     short decompress_subfile
compression_type     dd _file_decomp_rle
    dd _file_decomp_vle
loc_30F0C:
    add     sp, 8
loc_30F0F:
    cmp     [bp+var_fatal], 0
    jnz     short loc_30F1F
    xor     ax, ax
    xor     dx, dx
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_30F1F:
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    mov     ax, offset _aSInvalidPackTy; "%s INVALID PACK TYPE\r"
    push    ax
    call    far ptr _fatal_error
fd_done:
    mov     ax, [bp+var_decomp_paras]
    sub     ax, 4
    push    ax
    push    [bp+var_dstseg]
    push    [bp+var_dstoff]
    call    _mmgr_resize_memory
    add     sp, 6
    mov     ax, [bp+var_dstoff]
    mov     dx, [bp+var_dstseg]
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     ax, [bp+arg_6]
    mov     [bp+var_fatal], ax
    jmp     short loc_30F7E
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short loc_30F7E
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    si
    push    di
    mov     [bp+var_fatal], 1
loc_30F7E:
    push    [bp+var_fatal]
    push    [bp+arg_filename]
    call    _file_decomp_fatal
    add     sp, 4
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf

_file_decomp_fatal endp
S012_TEXT ends
end
