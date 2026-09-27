.8086
MATRIX struc
_11 dw ?
_21 dw ?
_31 dw ?
_12 dw ?
_22 dw ?
_32 dw ?
_13 dw ?
_23 dw ?
_33 dw ?
MATRIX ends
VECTOR struc
vx dw ?
vy dw ?
vz dw ?
VECTOR ends
SHAPE2D struc
s2d_width dw ?
s2d_height dw ?
s2d_unk1 dw ?
s2d_unk2 dw ?
s2d_pos_x dw ?
s2d_pos_y dw ?
s2d_unk3 db ?
s2d_unk4 db ?
s2d_unk5 db ?
s2d_unk6 db ?
SHAPE2D ends
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
dseg segment byte public 'STUNTSD'
dseg ends
assume cs:S012_TEXT, ds:DGROUP
extrn _fontdefseg:word
extrn _word_4031E:word
extrn _spritefunc:word
extrn _imagefunc:word
extrn _callbackflags2:byte
extrn _file_get_shape2d:far
extrn _draw_patterned_lines:far
extrn _preRender_line:far
extrn _loc_3180A:near
public _font_op
public _font_op2
public _preRender_patterned
public _nopsub_328C9
public _nopsub_328DB
public _mat_mul_vector
public _mat_multiply
public _mat_invert
public _file_unflip_shape2d
public _file_decomp_vle
public _nopsub_32FEE
public _video_get_status
S012_TEXT segment word public 'CODE'

_font_op proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_4 = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     dx, [bp+arg_4]
    or      dx, dx
    jnz     short loc_3284A
    xor     ax, ax
    jmp     short loc_32882
    db 144
_font_op endp
_font_op2 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    xor     dx, dx
loc_3284A:
    mov     es, _fontdefseg
    mov     si, [bp+arg_0]
    xor     ax, ax
    mov     cl, es:14h
    mov     di, es:10h
loc_3285D:
    mov     bl, [si]
    or      bl, bl
    jz      short loc_32882
    inc     si
    xor     bh, bh
    shl     bx, 1
    mov     bx, es:[bx+16h]
    or      bx, bx
    jz      short loc_3285D
    or      cl, cl
    jz      short loc_3287B
    mov     bl, es:[bx]
    xor     bh, bh
    mov     di, bx
loc_3287B:
    add     ax, di
    dec     dx
    jz      short loc_32882
    jmp     short loc_3285D
loc_32882:
    pop     di
    pop     si
    pop     bp
    retf
_font_op2 endp
_preRender_patterned proc far
    var_A = byte ptr -10
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_6]
    mov     [bp+var_A], 1
    jmp     short loc_328A8
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_6]
    mov     [bp+var_A], 0
loc_328A8:
    mov     ax, [bp+arg_0]
    mov     _word_4031E, ax
    mov     ax, [bp+arg_2]
    mov     [bp+arg_0], ax
    mov     ax, [bp+arg_4]
    mov     [bp+arg_2], ax
    mov     ax, offset _draw_patterned_lines
    mov     _spritefunc, ax
    mov     ax, offset _preRender_line
    mov     _imagefunc, ax
    jmp     _loc_3180A
_preRender_patterned endp
_nopsub_328C9 proc far
    var_A = byte ptr -10
     s = byte ptr 0
     r = byte ptr 2
    arg_6 = byte ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    lea     si, [bp+arg_6]
    mov     [bp+var_A], 1
    jmp     short loc_328A8
_nopsub_328C9 endp
_nopsub_328DB proc far
    var_A = byte ptr -10
     s = byte ptr 0
     r = byte ptr 2
    arg_6 = byte ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    lea     si, [bp+arg_6]
    mov     [bp+var_A], 0
    jmp     short loc_328A8
    ; align 2
    db 0
_nopsub_328DB endp
_mat_mul_vector proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_inpVector = word ptr 6
    arg_matrix = word ptr 8
    arg_outVector = word ptr 10

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     bx, [bp+arg_matrix]
    mov     si, [bp+arg_inpVector]
    mov     di, [bp+arg_outVector]
    xor     dx, dx
    mov     ax, [bx+MATRIX._11]
    or      ax, ax
    jz      short loc_32914
    mov     cx, [si+VECTOR.vx]
    or      cx, cx
    jz      short loc_32914
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
loc_32914:
    mov     [di+VECTOR.vx], dx
    mov     ax, [bx+MATRIX._12]
    or      ax, ax
    jz      short loc_32930
    mov     cx, [si+VECTOR.vy]
    or      cx, cx
    jz      short loc_32930
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vx], dx
loc_32930:
    mov     ax, [bx+MATRIX._13]
    or      ax, ax
    jz      short loc_3294A
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jz      short loc_3294A
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vx], dx
loc_3294A:
    xor     dx, dx
    mov     ax, [bx+MATRIX._21]
    or      ax, ax
    jz      short loc_32963
    mov     cx, [si+VECTOR.vx]
    or      cx, cx
    jz      short loc_32963
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
loc_32963:
    mov     [di+VECTOR.vy], dx
    mov     ax, [bx+MATRIX._22]
    or      ax, ax
    jz      short loc_32981
    mov     cx, [si+VECTOR.vy]
    or      cx, cx
    jz      short loc_32981
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+2], dx
loc_32981:
    mov     ax, [bx+MATRIX._23]
    or      ax, ax
    jz      short loc_3299C
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jz      short loc_3299C
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vy], dx
loc_3299C:
    xor     dx, dx
    mov     ax, [bx+MATRIX._31]
    or      ax, ax
    jz      short loc_329B5
    mov     cx, [si+VECTOR.vx]
    or      cx, cx
    jz      short loc_329B5
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
loc_329B5:
    mov     [di+VECTOR.vz], dx
    mov     ax, [bx+MATRIX._32]
    or      ax, ax
    jz      short loc_329D3
    mov     cx, [si+VECTOR.vy]
    or      cx, cx
    jz      short loc_329D3
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vz], dx
loc_329D3:
    mov     ax, [bx+MATRIX._33]
    or      ax, ax
    jz      short loc_329EE
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jz      short loc_329EE
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vz], dx
loc_329EE:
    pop     di
    pop     si
    pop     bp
    retf
_mat_mul_vector endp
_mat_multiply proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_rightMatrix = word ptr 6
    arg_leftMatrix = word ptr 8
    arg_outputMatrix = word ptr 10

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, [bp+arg_rightMatrix]
    mov     bx, [bp+arg_leftMatrix]
    mov     di, [bp+arg_outputMatrix]
    mov     cx, 9
loc_32A03:
    xor     dx, dx
    mov     ax, [si+MATRIX._11]
    or      ax, ax
    jz      short loc_32A1A
    cmp     [bx+MATRIX._11], 0
    jz      short loc_32A1A
    imul    [bx+MATRIX._11]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
loc_32A1A:
    mov     [di+MATRIX._11], dx
    mov     ax, [si+MATRIX._21]
    or      ax, ax
    jz      short loc_32A36
    cmp     [bx+MATRIX._12], 0
    jz      short loc_32A36
    imul    [bx+MATRIX._12]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+MATRIX._11], dx
loc_32A36:
    mov     ax, [si+MATRIX._31]
    or      ax, ax
    jz      short loc_32A50
    cmp     [bx+MATRIX._13], 0
    jz      short loc_32A50
    imul    [bx+MATRIX._13]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+MATRIX._11], dx
loc_32A50:
    add     di, 2
    cmp     cx, 7
    jz      short loc_32A66
    cmp     cx, 4
    jz      short loc_32A66
    add     bx, 2
    loop    loc_32A03
    pop     di
    pop     si
    pop     bp
    retf
loc_32A66:
    sub     bx, 4
    add     si, 6
    loop    loc_32A03
    pop     di
    pop     si
    pop     bp
    retf
_mat_multiply endp
_mat_invert proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_mat_in = word ptr 6
    arg_mat_out = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, [bp+arg_mat_in]
    mov     di, [bp+arg_mat_out]
    mov     cx, 9
    cmp     si, di
    jnz     short loc_32AA3
    mov     ax, [si+MATRIX._21]
    xchg    ax, [si+MATRIX._12]
    mov     [si+MATRIX._21], ax
    mov     ax, [si+MATRIX._31]
    xchg    ax, [si+MATRIX._13]
    mov     [si+MATRIX._31], ax
    mov     ax, [si+MATRIX._32]
    xchg    ax, [si+MATRIX._23]
    mov     [si+MATRIX._32], ax
    pop     di
    pop     si
    pop     bp
    retf
loc_32AA3:
    mov     ax, [si+MATRIX._11]
    mov     [di+MATRIX._11], ax
    mov     ax, [si+MATRIX._21]
    mov     [di+MATRIX._12], ax
    mov     ax, [si+MATRIX._31]
    mov     [di+MATRIX._13], ax
    mov     ax, [si+MATRIX._12]
    mov     [di+MATRIX._21], ax
    mov     ax, [si+MATRIX._22]
    mov     [di+MATRIX._22], ax
    mov     ax, [si+MATRIX._32]
    mov     [di+MATRIX._23], ax
    mov     ax, [si+MATRIX._13]
    mov     [di+MATRIX._31], ax
    mov     ax, [si+MATRIX._23]
    mov     [di+MATRIX._32], ax
    mov     ax, [si+MATRIX._33]
    mov     [di+MATRIX._33], ax
    pop     di
    pop     si
    pop     bp
    retf
    ; align 2
    db 0
fliphandlers     dw offset fliptype0
    dw offset fliptype1
    dw offset fliptype2
_mat_invert endp
_file_unflip_shape2d proc far
    var_height = word ptr -14
    var_width = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_numshapes = word ptr -6
    var_counter = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_memchunkofs = word ptr 6
    arg_memchunkseg = word ptr 8
    arg_mempagesofs = word ptr 10
    arg_mempagesseg = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    cld
    mov     ax, [bp+arg_memchunkseg]
    mov     ds, ax
    mov     ax, [bp+arg_memchunkofs]
    mov     si, ax
    mov     ax, [bp+arg_mempagesseg]
    mov     [bp+var_A], ax
    mov     ax, [bp+arg_mempagesofs]
    mov     [bp+var_8], ax
    mov     bx, [si+4]      ; number of shapes in binary
    mov     [bp+var_2], bx
    mov     [bp+var_counter], 0
loc_32B0D:
    push    [bp+var_counter]
    push    [bp+arg_memchunkseg]
    push    [bp+arg_memchunkofs]
    mov     ax, seg dseg
    mov     ds, ax
    call    _file_get_shape2d; get shape in binary at index
    add     sp, 6
    mov     [bp+var_numshapes], ax
    mov     si, ax
    mov     ds, dx
    mov     di, [bp+var_8]
    mov     es, [bp+var_A]
    mov     al, [si+SHAPE2D.s2d_unk6]; unk6 = is flipped
    and     al, 0F0h
    jnz     short loc_32B4E
    mov     al, [si+SHAPE2D.s2d_unk5]; (unk4 >> 4) = fliptype, 1..3
    shr     al, 1
    shr     al, 1
    shr     al, 1
    shr     al, 1
    jz      short loc_32B4E
    cmp     al, 4
    jb      short loc_32B5F
    mov     ax, 1
    jmp     short loc_32B58
    ; align 2
    db 144
loc_32B4E:
    inc     [bp+var_counter]
    dec     [bp+var_2]
    jg      short loc_32B0D
    xor     ax, ax
loc_32B58:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_32B5F:
    mov     dx, [si+SHAPE2D.s2d_width]
    mov     [bp+var_width], dx
    mov     dx, [si+SHAPE2D.s2d_height]
    mov     [bp+var_height], dx
    xor     dx, dx
    xor     ah, ah
    mov     bx, ax
    dec     bx
    shl     bx, 1
    jmp     cs:fliphandlers[bx]; jump to flip type handler
fliptype0:
    mov     bx, si
    add     bx, 10h
    add     bx, dx
    mov     cx, [bp+var_width]
loc_32B82:
    mov     al, [bx]
    stosb
    add     bx, [bp+var_height]
    loop    loc_32B82
    inc     dx
    cmp     dx, [bp+var_height]
    jnz     short fliptype0
    jmp     loc_32C15
fliptype1:
    mov     bx, dx
    shr     bx, 1
    add     bx, 10h
    add     bx, si
    mov     cx, [bp+var_width]
loc_32B9F:
    mov     al, [bx]
    stosb
    add     bx, [bp+var_height]
    loop    loc_32B9F
    add     di, [bp+var_width]
    inc     dx
    inc     dx
    cmp     dx, [bp+var_height]
    jb      short fliptype1
    mov     dx, 1
    mov     di, [bp+var_8]
    add     di, [bp+var_width]
loc_32BBA:
    mov     bx, dx
    add     bx, [bp+var_height]
    shr     bx, 1
    add     bx, 10h
    add     bx, si
    mov     cx, [bp+var_width]
loc_32BC9:
    mov     al, [bx]
    stosb
    add     bx, [bp+var_height]
    loop    loc_32BC9
    add     di, [bp+var_width]
    inc     dx
    inc     dx
    cmp     dx, [bp+var_height]
    jb      short loc_32BBA
    jmp     short loc_32C15
    ; align 2
    db 144
fliptype2:
    mov     bx, dx
    shr     bx, 1
    add     bx, 10h
    add     bx, [bp+var_numshapes]
    mov     cx, [bp+var_width]
    mov     si, [bp+var_height]
    shr     si, 1
    adc     si, 0
loc_32BF3:
    mov     al, [bx]
    stosb
    add     bx, si
    loop    loc_32BF3
    inc     dx
    cmp     dx, [bp+var_height]
    jz      short loc_32C15
    mov     cx, [bp+var_width]
    mov     si, [bp+var_height]
    shr     si, 1
loc_32C08:
    mov     al, [bx]
    stosb
    add     bx, si
    loop    loc_32C08
    inc     dx
    cmp     dx, [bp+var_height]
    jnz     short fliptype2
loc_32C15:
    mov     di, [bp+var_numshapes]
    mov     ax, ds
    mov     es, ax
    add     di, 10h
    mov     si, [bp+var_8]
    mov     ds, [bp+var_A]
    mov     ax, [bp+var_width]
    mul     [bp+var_height]
    mov     cx, ax
    shr     cx, 1
    jb      short loc_32C36
    rep movsw
    jmp     loc_32B4E
loc_32C36:
    rep movsw
    movsb
    jmp     loc_32B4E
vle_esc1     dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
    dw 0
vle_esc2     db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
_file_unflip_shape2d endp
_file_decomp_vle proc far
    var_lengths = byte ptr -528
    var_symbols = byte ptr -272
    var_codoff = word ptr -14
    var_codlen = word ptr -10
    var_lenhi = word ptr -8
    var_lenlo = word ptr -6
    var_lenlefthi = word ptr -4
    var_lenleftlo = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_srcoff = word ptr 6
    arg_srcseg = word ptr 8
    arg_dstoff = word ptr 10
    arg_dstseg = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    di
    push    si
    push    ds
    cld
    sub     sp, 200h
    mov     ds, [bp+arg_srcseg]
    mov     si, [bp+arg_srcoff]
    mov     ax, [si+1]
    mov     [bp+var_lenleftlo], ax
    mov     [bp+var_lenlo], ax
    mov     al, [si+3]
    xor     ah, ah
    mov     [bp+var_lenlefthi], ax
    mov     [bp+var_lenhi], ax
    mov     al, [si+4]
    xor     ah, ah
    mov     [bp+var_codlen], ax
    and     ax, 7Fh
    add     si, 5
    mov     [bp+var_codoff], si
    lea     cx, _callbackflags2+72h
    shl     ax, 1
    mov     di, ax
    xor     bx, bx
    xor     dx, dx
gen_esc:
    mov     cs:vle_esc1[bx], cx
    shl     dx, 1
    sub     cs:vle_esc1[bx], dx
    lodsb
    xor     ah, ah
    add     dx, ax
    add     cx, ax
    or      ax, ax
    jz      short $+2
    mov     word ptr cs:vle_esc2[bx], dx
    add     bx, 2
    cmp     bx, di
    jl      short gen_esc
    sub     cx, 425Ch
    mov     ax, cs
    mov     es, ax
    mov     di, 425Ch
    rep movsb
    push    si
    mov     bx, [bp+var_codlen]
    mov     bh, bl
    cmp     bh, 8
    jle     short loc_32DFE
    mov     bh, 8
loc_32DFE:
    xor     si, si
    mov     bl, 1
    mov     cl, 80h ; ' '
    mov     di, cs:vle_esc1
loc_32E09:
    xchg    si, [bp+var_codoff]
    lodsb
    xchg    si, [bp+var_codoff]
    mov     dh, al
    or      al, al
    jz      short loc_32E29
loc_32E16:
    mov     ch, cl
    mov     dl, cs:[di]
    inc     di
loc_32E1C:
    mov     [bp+si+var_symbols], dl
    mov     [bp+si+var_lengths], bl
    inc     si
    dec     ch
    jnz     short loc_32E1C
loc_32E29:
    dec     dh
    jg      short loc_32E16
    shr     cl, 1
    inc     bl
    cmp     bl, bh
    jle     short loc_32E09
pad_lens:
    mov     [bp+si+var_lengths], 40h ; '@'
    inc     si
    cmp     si, 100h
    jnz     short pad_lens
    pop     si
    mov     bx, si
    mov     es, [bp+arg_dstseg]
    mov     di, [bp+arg_dstoff]
    mov     dl, [bx]
    inc     bx
    mov     dh, [bx]
    inc     bx
    mov     ah, 8
    test    [bp+var_codlen], 80h
    jz      short loc_32E5C
    jmp     loc_32F27
loc_32E5C:
    mov     si, dx
    and     si, 0FFh
    mov     cl, [bp+si+var_lengths]
    cmp     cl, 8
    ja      short len_gt_8bit
    mov     al, [bp+si+var_symbols]
    stosb
    cmp     cl, ah
    jle     short loc_32E7F
    xchg    cl, ah
    rol     dx, cl
    xchg    cl, ah
    sub     cl, ah
    jmp     short loc_32EA0
    db 144
loc_32E7F:
    rol     dx, cl
    sub     ah, cl
    or      di, di
    jz      short loc_32E94
loc_32E87:
    dec     [bp+var_lenleftlo]
    jnz     short loc_32E5C
    dec     [bp+var_lenlefthi]
    jge     short loc_32E5C
    jmp     loc_32F16
loc_32E94:
    mov     di, es
    add     di, 1000h
    mov     es, di
    xor     di, di
    jmp     short loc_32E87
loc_32EA0:
    mov     dh, [bx]
    inc     bx
    jz      short loc_32EA9
loc_32EA5:
    mov     ah, 8
    jmp     short loc_32E7F
loc_32EA9:
    mov     bx, ds
    add     bx, 1000h
    mov     ds, bx
    xor     bx, bx
    jmp     short loc_32EA5
len_gt_8bit:
    push    si
    push    bp
    mov     si, bx
    mov     bp, 0Eh
    mov     bh, dh
    xor     dh, dh
    xor     ch, ch
loc_32EC2:
    cmp     ah, 0
    jz      short loc_32F01
loc_32EC7:
    cmp     ch, 0
    jz      short loc_32ED4
    mov     dh, bh
    mov     bx, si
    pop     bp
    pop     si
    jmp     short loc_32E7F
loc_32ED4:
    shl     bh, 1
    rcl     dx, 1
    sub     ah, 1
    add     bp, 2
    cmp     dx, word ptr cs:vle_esc2[bp]
    jnb     short loc_32EC2
    add     dx, cs:vle_esc1[bp]
    mov     bp, dx
    mov     al, cs:[bp+0]
    stosb
    mov     dh, bh
    mov     cl, ah
    rol     dx, cl
    mov     cl, 8
    sub     cl, ah
    mov     ah, 0
    mov     ch, 1
    jmp     short loc_32EC2
loc_32F01:
    mov     bh, [si]
    inc     si
    jz      short loc_32F0A
loc_32F06:
    mov     ah, 8
    jmp     short loc_32EC7
loc_32F0A:
    mov     si, ds
    add     si, 1000h
    mov     ds, si
    xor     si, si
    jmp     short loc_32F06
loc_32F16:
    mov     ax, [bp+var_lenlo]
    mov     dx, [bp+var_lenhi]
    add     sp, 200h
    pop     ds
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
loc_32F27:
    mov     si, dx
    and     si, 0FFh
    mov     cl, [bp+si+var_lengths]
    cmp     cl, 8
    ja      short loc_32F83
    mov     al, [bp+si+var_symbols]
    add     al, ch
    stosb
    cmp     cl, ah
    jle     short loc_32F4C
    xchg    cl, ah
    rol     dx, cl
    xchg    cl, ah
    sub     cl, ah
    jmp     short loc_32F6E
    ; align 2
    db 144
loc_32F4C:
    rol     dx, cl
    sub     ah, cl
    or      di, di
    jz      short loc_32F62
loc_32F54:
    mov     ch, al
    dec     [bp+var_lenleftlo]
    jnz     short loc_32F27
    dec     [bp+var_lenlefthi]
    jge     short loc_32F27
    jmp     short loc_32F16
loc_32F62:
    mov     di, es
    add     di, 1000h
    mov     es, di
    xor     di, di
    jmp     short loc_32F54
loc_32F6E:
    mov     dh, [bx]
    inc     bx
    jz      short loc_32F77
loc_32F73:
    mov     ah, 8
    jmp     short loc_32F4C
loc_32F77:
    mov     bx, ds
    add     bx, 1000h
    mov     ds, bx
    xor     bx, bx
    jmp     short loc_32F73
loc_32F83:
    push    si
    push    bp
    mov     si, bx
    mov     bp, 0Eh
    mov     bh, dh
    xor     dh, dh
    mov     [bp+var_codoff], 0
loc_32F93:
    cmp     ah, 0
    jz      short loc_32FD8
loc_32F98:
    cmp     [bp+var_codoff], 0
    jz      short loc_32FA6
    mov     dh, bh
    mov     bx, si
    pop     bp
    pop     si
    jmp     short loc_32F4C
loc_32FA6:
    shl     bh, 1
    rcl     dx, 1
    sub     ah, 1
    add     bp, 2
    cmp     dx, cs:[bp+423Ch]
    jnb     short loc_32F93
    add     dx, cs:[bp+421Ch]
    mov     bp, dx
    mov     al, cs:[bp+0]
    add     al, ch
    stosb
    mov     dh, bh
    mov     cl, ah
    rol     dx, cl
    mov     cl, 8
    sub     cl, ah
    mov     ah, 0
    mov     [bp+var_codoff], 1
    jmp     short loc_32F93
loc_32FD8:
    mov     bh, [si]
    inc     si
    jz      short loc_32FE1
loc_32FDD:
    mov     ah, 8
    jmp     short loc_32F98
loc_32FE1:
    mov     si, ds
    add     si, 1000h
    mov     ds, si
    xor     si, si
    jmp     short loc_32FDD
    ; align 2
    db 0
_file_decomp_vle endp
_nopsub_32FEE proc far

    mov     dx, 3DAh
loc_32FF1:
    in      al, dx          ; Video status bits:
    test    al, 8
    jnz     short loc_32FF1
loc_32FF6:
    in      al, dx          ; Video status bits:
    test    al, 8
    jz      short loc_32FF6
    retf
_nopsub_32FEE endp
_video_get_status proc far

    mov     dx, 3DAh
    in      al, dx          ; Video status bits:
    and     al, 8
    xor     ah, ah
    retf
_video_get_status endp

    db 0
S012_TEXT ends
end
