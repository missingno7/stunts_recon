.8086
DSEG segment byte public 'STUNTSD'
DSEG ends
_DATA segment word public 'DATA'
_aSFileError db '%s FILE ERROR',13,0
_aSNotPackedFile db '%s NOT PACKED FILE',13,0
_aSInvalidPackTy db '%s INVALID PACK TYPE',13,0,0
aLocateshape4_4sShapeNotF db 'locateshape - %-4.4s SHAPE NOT FOUND',13,10
aLocatesound4_4sSoundNotF db 'locatesound - %-4.4s SOUND NOT FOUND',13,10
_DATA ends
DGROUP group _DATA,DSEG
assume ds:DGROUP
extrn _fatal_error:far
extrn _copy_paras_reverse:far
extrn _mmgr_get_chunk_by_name:far
extrn _file_paras:far
extrn _mmgr_alloc_pages:far
extrn _file_decomp_paras:far
extrn _mmgr_resize_memory:far
extrn _file_decomp_vle:far
public _file_read, _file_read_nofatal, _file_read_fatal
public _file_decomp_rle, _file_decomp_rle_single, _file_decomp_rle_seq
public _file_load_binary
public _file_load_binary_nofatal
public _file_decomp, _file_decomp_nofatal, _file_decomp_fatal
public _locate_shape_nofatal
public _locate_shape_fatal
public _locate_sound_fatal
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP

; file_read
assume cs:S012_TEXT
_file_read proc far
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2
    arg_fatal = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    mov     ax, [bp+arg_fatal]
    mov     [bp+var_fatal], ax
    jmp     short read_body
    ; align 2
    db 144

_file_read endp
_file_read_nofatal proc far
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    mov     [bp+var_fatal], 0
    jmp     short read_body
    db 144

_file_read_nofatal endp
_file_read_fatal proc far
    var_fatal = word ptr -8
    var_curseg = word ptr -6
    var_curoff = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_dstoff = word ptr 8
    arg_dstseg = word ptr 10

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    mov     [bp+var_fatal], 1
read_body:
    mov     ax, [bp+arg_dstoff]
    mov     [bp+var_curoff], ax
    mov     ax, [bp+arg_dstseg]
    mov     [bp+var_curseg], ax
    mov     dx, [bp+arg_filename]
    xor     al, al
    mov     ah, 3Dh
    int     21h             ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
    jb      short error
    mov     [bp+var_filehandle], ax
readloop:
    mov     ds, [bp+var_curseg]
    mov     dx, [bp+var_curoff]
    mov     cx, 4000h
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Fh
    int     21h             ; DOS - 2+ - READ FROM FILE WITH HANDLE
    jb      short error
    add     [bp+var_curseg], 400h
    cmp     ax, 4000h
    jz      short readloop
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h             ; DOS - 2+ - CLOSE A FILE WITH HANDLE
    mov     ax, [bp+arg_dstoff]
    mov     dx, [bp+arg_dstseg]
    pop     ds
    mov     sp, bp
    pop     bp
    retf
error:
    cmp     [bp+var_fatal], 0
    jnz     short fatal
    xor     ax, ax
    xor     dx, dx
    pop     ds
    mov     sp, bp
    pop     bp
    retf
fatal:
    mov     ax, offset DGROUP:_aSFileError; "%s FILE ERROR\r"
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    push    ax
    call    far ptr _fatal_error

_file_read_fatal endp

; file_decomp_rle
assume cs:S012_TEXT
_file_decomp_rle proc far
    var_lenlo = word ptr -26
    var_lenhi = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_esclen = word ptr -18
     s = byte ptr 0
     r = byte ptr 2
    arg_srcoff = word ptr 6
    arg_srcseg = word ptr 8
    arg_dstoff = word ptr 12
    arg_dstseg = word ptr 14

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    sub     sp, 116h
    cld
    mov     ax, ds
    mov     es, ax
    mov     si, [bp+arg_srcoff]
    mov     ds, [bp+arg_srcseg]
    mov     ax, [si+1]
    mov     [bp+var_lenlo], ax
    mov     al, [si+3]
    xor     ah, ah
    mov     [bp+var_lenhi], ax
    add     si, 4           ; Skip header + unused byte
    mov     cx, 8
    lea     di, [bp+var_16] ; Read 8 words
    rep movsw
    mov     si, [bp+arg_srcoff]; Reset source pos
    add     si, 9
    mov     dx, [bp+var_esclen]

    and     dx, 7Fh         ; Escape codes length mask

    add     si, dx
    mov     [bp+arg_srcoff], si; Skip escape codes
    cmp     byte ptr [bp+var_esclen], 80h ; ''; Skip seq pass flag
    ja      short skip_seq_pass
    call near ptr _file_decomp_rle_seq
    mov     [bp+var_16], ax
    mov     [bp+var_14], dx
    mov     si, ax          ; paras = len / 16
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    test    si, 0Fh
    jz      short no_remainder
    inc     ax
no_remainder:
    push    ax              ; paras
    mov     bx, [bp+arg_dstseg]
    sub     bx, ax
    add     bx, [bp+arg_dstoff]
    mov     [bp+arg_srcseg], bx
    push    bx
    push    [bp+arg_dstoff]
    call far ptr _copy_paras_reverse
    add     sp, 6
    xor     si, si
    mov     [bp+arg_srcoff], si
skip_seq_pass:
    call near ptr _file_decomp_rle_single
    mov     ax, [bp+var_lenlo]; Return length
    mov     dx, [bp+var_lenhi]
    add     sp, 116h
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf

_file_decomp_rle endp
_file_decomp_rle_single proc near
    mov     cx, 80h ; ''
    lea     di, [bp-11Ch]
    mov     ax, ss
    mov     es, ax
    mov     ds, ax
    xor     ax, ax
    rep stosw
    mov     cl, [bp-12h]

    and     cx, 7Fh

    lea     si, [bp-11h]
    lea     di, [bp-11Ch]
    xor     ax, ax
    xor     bh, bh
loc_30C1A:
    inc     ax
    mov     bl, [si]
    mov     [bx+di], al
    inc     si
    loop    loc_30C1A
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     es, word ptr [bp+0Ch]
    xor     di, di
    mov     dx, [bp-16h]
    mov     ax, [bp-14h]
    mov     [bp-1Ch], ax
loc_30C36:
    cmp     si, 8000h
    jbe     short loc_30C3F
    jmp     short loc_30CB3
    db 144
loc_30C3F:
    cmp     di, 8000h
    jbe     short loc_30C48
    jmp     short loc_30CC1
    ; align 2
    db 144
loc_30C48:
    lodsb
    xor     ah, ah
    lea     bx, [bp-11Ch]
    add     bx, ax
    mov     cl, ss:[bx]
    test    cl, 0FFh
    jnz     short loc_30C68
    stosb
    dec     dx
    jnz     short loc_30C36
    cmp     word ptr [bp-1Ch], 0
    jz      short loc_30C8A
    dec     word ptr [bp-1Ch]
    jmp     short loc_30C36
loc_30C68:
    xor     ch, ch
    cmp     cl, 1
    jz      short loc_30C9A
    cmp     cl, 3
    jz      short loc_30CA5
    dec     cx
    lodsb
    rep stosb
    sub     dx, 2
loc_30C7B:
    mov     ax, [bp-1Ch]
    sbb     ax, 0
    mov     [bp-1Ch], ax
    or      ax, dx
    jz      short loc_30C8A
    jmp     short loc_30C36
loc_30C8A:
    mov     dx, es
    sub     dx, [bp+0Ch]
    mov     ax, 10h
    mul     dx
    add     ax, di
    adc     dx, 0
    retn
loc_30C9A:
    lodsb
    mov     cl, al
    lodsb
    rep stosb
    sub     dx, 3
    jmp     short loc_30C7B
loc_30CA5:
    lodsb
    mov     cl, al
    lodsb
    mov     ch, al
    lodsb
    rep stosb
    sub     dx, 4
    jmp     short loc_30C7B
loc_30CB3:
    sub     si, 8000h
    mov     ax, ds
    add     ax, 800h
    mov     ds, ax
    jmp     loc_30C3F
loc_30CC1:
    sub     di, 8000h
    mov     ax, es
    add     ax, 800h
    mov     es, ax
    jmp     loc_30C48

_file_decomp_rle_single endp
_file_decomp_rle_seq proc near
    cmp     byte ptr [bp-12h], 1; _file_decomp_rle::var_esclen
    jnz     short has_codes
    retn
has_codes:
    mov     ds, word ptr [bp+8]; _file_decomp_rle::arg_srcseg
    mov     si, [bp+6]      ; _file_decomp_rle::arg_srcoff
    mov     es, word ptr [bp+0Ch]; _file_decomp_rle::var_dstoff
    xor     di, di
    mov     bx, [bp-16h]
    mov     ax, [bp-14h]
    mov     [bp-1Ch], ax
    mov     ah, [bp-10h]
loc_30CED:
    cmp     si, 8000h
    ja      short loc_30D0D
loc_30CF3:
    cmp     di, 8000h
    ja      short loc_30D1B
loc_30CF9:
    lodsb
    cmp     al, ah
    jz      short loc_30D39
    stosb
    dec     bx
    jnz     short loc_30CED
    cmp     word ptr [bp-1Ch], 0
    jz      short loc_30D29
    dec     word ptr [bp-1Ch]
    jmp     short loc_30CED
loc_30D0D:
    sub     si, 8000h
    mov     dx, ds
    add     dx, 800h
    mov     ds, dx
    jmp     short loc_30CF3
loc_30D1B:
    sub     di, 8000h
    mov     dx, es
    add     dx, 800h
    mov     es, dx
    jmp     short loc_30CF9
loc_30D29:
    mov     dx, es
    sub     dx, [bp+0Ch]
    mov     ax, 10h
    mul     dx
    add     ax, di
    adc     dx, 0
    retn
loc_30D39:
    mov     dx, di
loc_30D3B:
    lodsb
    cmp     al, ah
    jz      short loc_30D49
    stosb
    dec     bx
    jnz     short loc_30D3B
    dec     word ptr [bp-1Ch]
    jmp     short loc_30D3B
loc_30D49:
    lodsb
    push    si
    push    ds
    mov     cx, es
    mov     ds, cx
    mov     si, dx
    mov     cx, di
    sub     cx, dx
    mov     dl, al
    dec     dl
loc_30D5A:
    push    cx
    push    si
    push    ds
    rep movsb
    pop     ds
    pop     si
    pop     cx
    dec     dl
    jnz     short loc_30D5A
    pop     ds
    pop     si
    sub     bx, 3
    sbb     word ptr [bp-1Ch], 0
    mov     dx, [bp-1Ch]
    or      dx, bx
    jz      short loc_30D29
    jmp     loc_30CED

_file_decomp_rle_seq endp

; file_load_binary_group
assume cs:S012_TEXT
_file_load_binary proc far
    push bp
    mov bp, sp
    sub sp, 2
    mov ax, [bp+8]
    mov [bp-2], ax
    jmp short common_entry
    nop
_file_load_binary endp
_file_load_binary_nofatal proc far
    push bp
    mov bp, sp
    sub sp, 2
    mov word ptr [bp-2], 0
    jmp short common_entry
    nop
fatal_entry:
    push bp
    mov bp, sp
    sub sp, 2
    mov word ptr [bp-2], 1
common_entry:
    push word ptr [bp+6]
    call far ptr _mmgr_get_chunk_by_name
    add sp, 2
    or dx, dx
    jnz load_complete
    push word ptr [bp-2]
    push word ptr [bp+6]
    call far ptr _file_paras
    add sp, 4
    or ax, ax
    jz empty_file
    push ax
    push word ptr [bp+6]
    call far ptr _mmgr_alloc_pages
    add sp, 4
    push word ptr [bp-2]
    push dx
    push ax
    push word ptr [bp+6]
    call far ptr _file_read
    add sp, 8
load_complete:
    mov sp, bp
    pop bp
    retf
empty_file:
    xor dx, dx
    jmp short load_complete
_file_load_binary_nofatal endp

; file_decomp_group
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
    mov     ax, offset DGROUP:_aSInvalidPackTy; "%s INVALID PACK TYPE\r"
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

; locate_entries
_locate_shape_nofatal proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    xor     dx, dx
    jmp     short _alt_locate_resource
    db 144
_locate_shape_nofatal endp
_locate_shape_fatal proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, 1
    jmp     short _alt_locate_resource
    db 144
_locate_shape_fatal endp
_locate_sound_fatal proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, 2
_alt_locate_resource:
    cld
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, seg dseg
    mov     es, ax
    mov     di, [bp+arg_4]
    mov     cx, 4
    mov     bx, 0
loc_30FC7:
    cmp     byte ptr es:[bx+di], 0
    jz      short loc_31024
    inc     bx
    loop    loc_30FC7
loc_30FD0:
    mov     ax, [si+4]
    or      ax, ax
    jz      short _end_of_locate
    add     si, 6
    mov     bx, si
loc_30FDC:
    mov     si, bx
    mov     di, [bp+arg_4]
    mov     cx, 4
loc_30FE4:
    cmpsb
    jnz     short loc_30FEC
    loop    loc_30FE4
    jmp     short _found_resource
    ; align 2
    db 144
loc_30FEC:
    cmp     byte ptr [si-1], 0
    jnz     short loc_30FF9
    cmp     byte ptr es:[di-1], 20h ; ' '
    jz      short _found_resource
loc_30FF9:
    add     bx, 4
    dec     ax
    jge     short loc_30FDC
_end_of_locate:
    cmp     dx, 1
    jl      short loc_3101F
    jg      short loc_3100C
    mov     dx, offset DGROUP:aLocateshape4_4sShapeNotF; "locateshape - %-4.4s SHAPE NOT FOUND\r\n"
    jmp     short loc_3100F
    ; align 2
    db 144
loc_3100C:
    mov     dx, offset DGROUP:aLocatesound4_4sSoundNotF; "locatesound - %-4.4s SOUND NOT FOUND\r\n"
loc_3100F:
    mov     ax, seg dseg
    mov     ds, ax
    push    [bp+arg_4]
    push    dx
    call    far ptr _fatal_error
    int     20h             ; DOS - PROGRAM TERMINATION
loc_3101F:
    xor     ax, ax
    jmp     short loc_31075
    ; align 2
    db 144
loc_31024:
    mov     byte ptr es:[bx+di], 20h ; ' '
    inc     bx
    loop    loc_31024
    jmp     short loc_30FD0
_found_resource:
    mov     si, [bp+arg_0]
    mov     ax, [si+4]
    shl     ax, 1
    shl     ax, 1
    add     bx, ax
    shl     ax, 1
    add     ax, 6
    mov     cx, ds
    xor     dx, dx
    shl     cx, 1
    rcl     dx, 1
    shl     cx, 1
    rcl     dx, 1
    shl     cx, 1
    rcl     dx, 1
    shl     cx, 1
    rcl     dx, 1
    add     ax, cx
    adc     dx, 0
    add     ax, [bx]
    adc     dx, [bx+2]
    mov     bx, ax

    and     bx, 0Fh

    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    mov     dx, ax
    mov     ax, bx
loc_31075:
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
_locate_sound_fatal endp

S012_TEXT ends
end
