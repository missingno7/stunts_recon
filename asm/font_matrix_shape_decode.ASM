.8086
INCLUDE include\platform_hw.inc
name asm012_141362
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
extrn _fontdef_default:byte
_DATA segment word public 'DATA'
public _fontdefseg, _vector_op_base_z
_fontdefseg dw seg _fontdef_default
_vector_op_base_z dw 10
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
dseg segment byte public 'STUNTSD'
dseg ends
assume cs:S012_TEXT, ds:DGROUP
extrn _line_pattern_bits:word
extrn _spritefunc:word
extrn _imagefunc:word
extrn _callbackflags2:byte
extrn _file_get_shape2d:far
extrn _draw_patterned_lines:far
extrn _preRender_line:far
extrn _prerender_shared_setup:near
public _font_op
public _font_op2
public _preRender_patterned
public _nopsub_328C9
public _nopsub_328DB
public _mat_vec
public _mat_multiply
public _mat_invert
public _file_unflip_shape2d
public _file_decomp_vle
public _nopsub_32FEE
public _video_get_status
S012_TEXT segment word public 'CODE'

; -----------------------------------------------------------------------------
; Purpose: Dispatch one font or glyph drawing operation.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_4 (word ptr 8).
; Register ABI: touches AX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
    jnz     short font_op2_branch_path_01
    xor     ax, ax
    jmp     short font_op2_branch_path_03
    db 144
_font_op endp
; -----------------------------------------------------------------------------
; Purpose: Dispatch a font operation using the alternate glyph path.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, ES; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _fontdefseg.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_font_op2 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    xor     dx, dx
font_op2_branch_path_01:
    mov     es, _fontdefseg
    mov     si, [bp+arg_0]
    xor     ax, ax
    mov     cl, es:14h
    mov     di, es:10h
font_op2_loop_head_01:
    mov     bl, [si]
    or      bl, bl
    jz      short font_op2_branch_path_03
    inc     si
    xor     bh, bh
    shl     bx, 1
    mov     bx, es:[bx+16h] ;/* PLATFORM(video): accesses display memory through ES. */
    or      bx, bx
    jz      short font_op2_loop_head_01
    or      cl, cl
    jz      short font_op2_branch_path_02
    mov     bl, es:[bx] ;/* PLATFORM(video): accesses display memory through ES. */
    xor     bh, bh
    mov     di, bx
font_op2_branch_path_02:
    add     ax, di
    dec     dx
    jz      short font_op2_branch_path_03
    jmp     short font_op2_loop_head_01
font_op2_branch_path_03:
    pop     di
    pop     si
    pop     bp
    retf
_font_op2 endp
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender patterned.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 8);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _imagefunc, _line_pattern_bits, _spritefunc.
; -----------------------------------------------------------------------------
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
    jmp     short patterned_loop_head_01
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_6]
    mov     [bp+var_A], 0
patterned_loop_head_01:
    mov     ax, [bp+arg_0]
    mov     _line_pattern_bits, ax
    mov     ax, [bp+arg_2]
    mov     [bp+arg_0], ax
    mov     ax, [bp+arg_4]
    mov     [bp+arg_2], ax
    mov     ax, offset _draw_patterned_lines
    mov     _spritefunc, ax
    mov     ax, offset _preRender_line
    mov     _imagefunc, ax
    jmp     _prerender_shared_setup
_preRender_patterned endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 328C9.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_6 (word ptr 12).
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
    jmp     short patterned_loop_head_01
_nopsub_328C9 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 328DB.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_6 (word ptr 12).
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
    jmp     short patterned_loop_head_01
    ; align 2
    db 0
_nopsub_328DB endp
; -----------------------------------------------------------------------------
; Purpose: Multiply a matrix by a vector using the supplied fixed-point values.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_inpVector (word ptr 6); arg_matrix (word ptr 8); arg_outVector (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_mat_vec proc far
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
    jz      short matrix_vec_branch_path_01
    mov     cx, [si+VECTOR.vx]
    or      cx, cx
    jz      short matrix_vec_branch_path_01
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
matrix_vec_branch_path_01:
    mov     [di+VECTOR.vx], dx
    mov     ax, [bx+MATRIX._12]
    or      ax, ax
    jz      short matrix_vec_branch_path_02
    mov     cx, [si+VECTOR.vy]
    or      cx, cx
    jz      short matrix_vec_branch_path_02
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vx], dx
matrix_vec_branch_path_02:
    mov     ax, [bx+MATRIX._13]
    or      ax, ax
    jz      short matrix_vec_branch_path_03
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jz      short matrix_vec_branch_path_03
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vx], dx
matrix_vec_branch_path_03:
    xor     dx, dx
    mov     ax, [bx+MATRIX._21]
    or      ax, ax
    jz      short matrix_vec_branch_path_04
    mov     cx, [si+VECTOR.vx]
    or      cx, cx
    jz      short matrix_vec_branch_path_04
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
matrix_vec_branch_path_04:
    mov     [di+VECTOR.vy], dx
    mov     ax, [bx+MATRIX._22]
    or      ax, ax
    jz      short matrix_vec_branch_path_05
    mov     cx, [si+VECTOR.vy]
    or      cx, cx
    jz      short matrix_vec_branch_path_05
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+2], dx
matrix_vec_branch_path_05:
    mov     ax, [bx+MATRIX._23]
    or      ax, ax
    jz      short matrix_vec_branch_path_06
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jz      short matrix_vec_branch_path_06
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vy], dx
matrix_vec_branch_path_06:
    xor     dx, dx
    mov     ax, [bx+MATRIX._31]
    or      ax, ax
    jz      short matrix_vec_branch_path_07
    mov     cx, [si+VECTOR.vx]
    or      cx, cx
    jz      short matrix_vec_branch_path_07
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
matrix_vec_branch_path_07:
    mov     [di+VECTOR.vz], dx
    mov     ax, [bx+MATRIX._32]
    or      ax, ax
    jz      short matrix_vec_branch_path_08
    mov     cx, [si+VECTOR.vy]
    or      cx, cx
    jz      short matrix_vec_branch_path_08
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vz], dx
matrix_vec_branch_path_08:
    mov     ax, [bx+MATRIX._33]
    or      ax, ax
    jz      short matrix_vec_branch_path_09
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jz      short matrix_vec_branch_path_09
    imul    cx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+VECTOR.vz], dx
matrix_vec_branch_path_09:
    pop     di
    pop     si
    pop     bp
    retf
_mat_vec endp
; -----------------------------------------------------------------------------
; Purpose: Multiply two fixed-point matrices.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_leftMatrix (word ptr 8); arg_outputMatrix (word ptr 10); arg_rightMatrix (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
matrix_mul_loop_head_01:
    xor     dx, dx
    mov     ax, [si+MATRIX._11]
    or      ax, ax
    jz      short matrix_mul_branch_path_01
    cmp     [bx+MATRIX._11], 0
    jz      short matrix_mul_branch_path_01
    imul    [bx+MATRIX._11]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
matrix_mul_branch_path_01:
    mov     [di+MATRIX._11], dx
    mov     ax, [si+MATRIX._21]
    or      ax, ax
    jz      short matrix_mul_branch_path_02
    cmp     [bx+MATRIX._12], 0
    jz      short matrix_mul_branch_path_02
    imul    [bx+MATRIX._12]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+MATRIX._11], dx
matrix_mul_branch_path_02:
    mov     ax, [si+MATRIX._31]
    or      ax, ax
    jz      short matrix_mul_branch_path_03
    cmp     [bx+MATRIX._13], 0
    jz      short matrix_mul_branch_path_03
    imul    [bx+MATRIX._13]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     [di+MATRIX._11], dx
matrix_mul_branch_path_03:
    add     di, 2
    cmp     cx, 7
    jz      short matrix_mul_branch_path_04
    cmp     cx, 4
    jz      short matrix_mul_branch_path_04
    add     bx, 2
    loop    matrix_mul_loop_head_01
    pop     di
    pop     si
    pop     bp
    retf
matrix_mul_branch_path_04:
    sub     bx, 4
    add     si, 6
    loop    matrix_mul_loop_head_01
    pop     di
    pop     si
    pop     bp
    retf
_mat_multiply endp
; -----------------------------------------------------------------------------
; Purpose: Invert the fixed-point matrix in the supplied workspace.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_mat_in (word ptr 6); arg_mat_out (word ptr 8).
; Register ABI: touches AX, CX, SI, DI, BP, SP; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
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
    jnz     short matrix_inv_branch_path_01
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
matrix_inv_branch_path_01:
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
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file unflip shape2d.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_memchunkofs (word ptr 6); arg_memchunkseg (word ptr 8); arg_mempagesofs (word ptr 10);
;   arg_mempagesseg (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
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
unflip_shape_loop_head_01:
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
    jnz     short unflip_shape_loop_head_02
    mov     al, [si+SHAPE2D.s2d_unk5]; (unk4 >> 4) = fliptype, 1..3
    shr     al, 1
    shr     al, 1
    shr     al, 1
    shr     al, 1
    jz      short unflip_shape_loop_head_02
    cmp     al, 4
    jb      short unflip_shape_branch_path_01
    mov     ax, 1
    jmp     short unflip_shape_join_01
    ; align 2
    db 144
unflip_shape_loop_head_02:
    inc     [bp+var_counter]
    dec     [bp+var_2]
    jg      short unflip_shape_loop_head_01
    xor     ax, ax
unflip_shape_join_01:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
unflip_shape_branch_path_01:
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
unflip_shape_loop_head_03:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, [bp+var_height]
    loop    unflip_shape_loop_head_03
    inc     dx
    cmp     dx, [bp+var_height]
    jnz     short fliptype0
    jmp     unflip_shape_branch_path_02
fliptype1:
    mov     bx, dx
    shr     bx, 1
    add     bx, 10h
    add     bx, si
    mov     cx, [bp+var_width]
unflip_shape_loop_head_04:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, [bp+var_height]
    loop    unflip_shape_loop_head_04
    add     di, [bp+var_width]
    inc     dx
    inc     dx
    cmp     dx, [bp+var_height]
    jb      short fliptype1
    mov     dx, 1
    mov     di, [bp+var_8]
    add     di, [bp+var_width]
unflip_shape_loop_head_05:
    mov     bx, dx
    add     bx, [bp+var_height]
    shr     bx, 1
    add     bx, 10h
    add     bx, si
    mov     cx, [bp+var_width]
unflip_shape_loop_head_06:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, [bp+var_height]
    loop    unflip_shape_loop_head_06
    add     di, [bp+var_width]
    inc     dx
    inc     dx
    cmp     dx, [bp+var_height]
    jb      short unflip_shape_loop_head_05
    jmp     short unflip_shape_branch_path_02
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
unflip_shape_loop_head_07:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, si
    loop    unflip_shape_loop_head_07
    inc     dx
    cmp     dx, [bp+var_height]
    jz      short unflip_shape_branch_path_02
    mov     cx, [bp+var_width]
    mov     si, [bp+var_height]
    shr     si, 1
unflip_shape_loop_head_08:
    mov     al, [bx]
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     bx, si
    loop    unflip_shape_loop_head_08
    inc     dx
    cmp     dx, [bp+var_height]
    jnz     short fliptype2
unflip_shape_branch_path_02:
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
    jb      short unflip_shape_branch_path_03
    rep movsw
    jmp     unflip_shape_loop_head_02
unflip_shape_branch_path_03:
    rep movsw
    movsb
    jmp     unflip_shape_loop_head_02
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
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file decomp vle.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_dstoff (word ptr 10); arg_dstseg (word ptr 12); arg_srcoff (word ptr 6); arg_srcseg
;   (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _callbackflags2.
; -----------------------------------------------------------------------------
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
    jle     short vle_decode_branch_path_01
    mov     bh, 8
vle_decode_branch_path_01:
    xor     si, si
    mov     bl, 1
    mov     cl, 80h ; ' '
    mov     di, cs:vle_esc1
vle_decode_loop_head_01:
    xchg    si, [bp+var_codoff]
    lodsb
    xchg    si, [bp+var_codoff]
    mov     dh, al
    or      al, al
    jz      short vle_decode_branch_path_02
vle_decode_loop_head_02:
    mov     ch, cl
    mov     dl, cs:[di]
    inc     di
vle_decode_loop_head_03:
    mov     [bp+si+var_symbols], dl
    mov     [bp+si+var_lengths], bl
    inc     si
    dec     ch
    jnz     short vle_decode_loop_head_03
vle_decode_branch_path_02:
    dec     dh
    jg      short vle_decode_loop_head_02
    shr     cl, 1
    inc     bl
    cmp     bl, bh
    jle     short vle_decode_loop_head_01
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
    jz      short vle_decode_loop_head_04
    jmp     vle_decode_loop_head_12
vle_decode_loop_head_04:
    mov     si, dx
    and     si, 0FFh
    mov     cl, [bp+si+var_lengths]
    cmp     cl, 8
    ja      short len_gt_8bit
    mov     al, [bp+si+var_symbols]
    stosb
    cmp     cl, ah
    jle     short vle_decode_loop_head_05
    xchg    cl, ah
    rol     dx, cl
    xchg    cl, ah
    sub     cl, ah
    jmp     short vle_decode_join_01
    db 144
vle_decode_loop_head_05:
    rol     dx, cl
    sub     ah, cl
    or      di, di
    jz      short vle_decode_branch_path_03
vle_decode_loop_head_06:
    dec     [bp+var_lenleftlo]
    jnz     short vle_decode_loop_head_04
    dec     [bp+var_lenlefthi]
    jge     short vle_decode_loop_head_04
    jmp     vle_decode_loop_head_11
vle_decode_branch_path_03:
    mov     di, es
    add     di, 1000h
    mov     es, di
    xor     di, di
    jmp     short vle_decode_loop_head_06
vle_decode_join_01:
    mov     dh, [bx]
    inc     bx
    jz      short vle_decode_branch_path_04
vle_decode_loop_head_07:
    mov     ah, 8
    jmp     short vle_decode_loop_head_05
vle_decode_branch_path_04:
    mov     bx, ds
    add     bx, 1000h
    mov     ds, bx
    xor     bx, bx
    jmp     short vle_decode_loop_head_07
len_gt_8bit:
    push    si
    push    bp
    mov     si, bx
    mov     bp, 0Eh
    mov     bh, dh
    xor     dh, dh
    xor     ch, ch
vle_decode_loop_head_08:
    cmp     ah, 0
    jz      short vle_decode_branch_path_06
vle_decode_loop_head_09:
    cmp     ch, 0
    jz      short vle_decode_branch_path_05
    mov     dh, bh
    mov     bx, si
    pop     bp
    pop     si
    jmp     short vle_decode_loop_head_05
vle_decode_branch_path_05:
    shl     bh, 1
    rcl     dx, 1
    sub     ah, 1
    add     bp, 2
    cmp     dx, word ptr cs:vle_esc2[bp]
    jnb     short vle_decode_loop_head_08
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
    jmp     short vle_decode_loop_head_08
vle_decode_branch_path_06:
    mov     bh, [si]
    inc     si
    jz      short vle_decode_branch_path_07
vle_decode_loop_head_10:
    mov     ah, 8
    jmp     short vle_decode_loop_head_09
vle_decode_branch_path_07:
    mov     si, ds
    add     si, 1000h
    mov     ds, si
    xor     si, si
    jmp     short vle_decode_loop_head_10
vle_decode_loop_head_11:
    mov     ax, [bp+var_lenlo]
    mov     dx, [bp+var_lenhi]
    add     sp, 200h
    pop     ds
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
vle_decode_loop_head_12:
    mov     si, dx
    and     si, 0FFh
    mov     cl, [bp+si+var_lengths]
    cmp     cl, 8
    ja      short vle_decode_branch_path_10
    mov     al, [bp+si+var_symbols]
    add     al, ch
    stosb
    cmp     cl, ah
    jle     short vle_decode_loop_head_13
    xchg    cl, ah
    rol     dx, cl
    xchg    cl, ah
    sub     cl, ah
    jmp     short vle_decode_join_02
    ; align 2
    db 144
vle_decode_loop_head_13:
    rol     dx, cl
    sub     ah, cl
    or      di, di
    jz      short vle_decode_branch_path_08
vle_decode_loop_head_14:
    mov     ch, al
    dec     [bp+var_lenleftlo]
    jnz     short vle_decode_loop_head_12
    dec     [bp+var_lenlefthi]
    jge     short vle_decode_loop_head_12
    jmp     short vle_decode_loop_head_11
vle_decode_branch_path_08:
    mov     di, es
    add     di, 1000h
    mov     es, di
    xor     di, di
    jmp     short vle_decode_loop_head_14
vle_decode_join_02:
    mov     dh, [bx]
    inc     bx
    jz      short vle_decode_branch_path_09
vle_decode_loop_head_15:
    mov     ah, 8
    jmp     short vle_decode_loop_head_13
vle_decode_branch_path_09:
    mov     bx, ds
    add     bx, 1000h
    mov     ds, bx
    xor     bx, bx
    jmp     short vle_decode_loop_head_15
vle_decode_branch_path_10:
    push    si
    push    bp
    mov     si, bx
    mov     bp, 0Eh
    mov     bh, dh
    xor     dh, dh
    mov     [bp+var_codoff], 0
vle_decode_loop_head_16:
    cmp     ah, 0
    jz      short vle_decode_branch_path_12
vle_decode_loop_head_17:
    cmp     [bp+var_codoff], 0
    jz      short vle_decode_branch_path_11
    mov     dh, bh
    mov     bx, si
    pop     bp
    pop     si
    jmp     short vle_decode_loop_head_13
vle_decode_branch_path_11:
    shl     bh, 1
    rcl     dx, 1
    sub     ah, 1
    add     bp, 2
    cmp     dx, cs:[bp+423Ch]
    jnb     short vle_decode_loop_head_16
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
    jmp     short vle_decode_loop_head_16
vle_decode_branch_path_12:
    mov     bh, [si]
    inc     si
    jz      short vle_decode_branch_path_13
vle_decode_loop_head_18:
    mov     ah, 8
    jmp     short vle_decode_loop_head_17
vle_decode_branch_path_13:
    mov     si, ds
    add     si, 1000h
    mov     ds, si
    xor     si, si
    jmp     short vle_decode_loop_head_18
    ; align 2
    db 0
_file_decomp_vle endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 32FEE. Its internal flow uses noop_entry3_loop_head_01, noop_entry3_loop_head_02.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches DX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses a VGA/MCGA display register port. */
; -----------------------------------------------------------------------------
_nopsub_32FEE proc far

    mov     dx, R5HW_VIDEO_STATUS_PORT
noop_entry3_loop_head_01:
    in      al, dx ; Video status bits: /* PLATFORM(video): accesses a VGA/MCGA display register port. */
    test    al, 8
    jnz     short noop_entry3_loop_head_01
noop_entry3_loop_head_02:
    in      al, dx ; Video status bits: /* PLATFORM(video): accesses a VGA/MCGA display register port. */
    test    al, 8
    jz      short noop_entry3_loop_head_02
    retf
_nopsub_32FEE endp
; -----------------------------------------------------------------------------
; Purpose: Read the video status register and return its status bits.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches DX; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): accesses a VGA/MCGA display register port. */
; -----------------------------------------------------------------------------
_video_get_status proc far

    mov     dx, R5HW_VIDEO_STATUS_PORT
    in      al, dx ; Video status bits: /* PLATFORM(video): accesses a VGA/MCGA display register port. */
    and     al, 8
    xor     ah, ah
    retf
_video_get_status endp

    db 0
S012_TEXT ends
end
