INCLUDE include\platform_hw.inc
.model medium
name asm012_137138
SPRITE struc
    sprite_bitmapptr dd ?
    sprite_unk1 dw ?
    sprite_unk2 dw ?
    sprite_unk3 dw ?
    sprite_lineofs dw ?
    sprite_left dw ?
    sprite_right dw ?
    sprite_top dw ?
    sprite_height dw ?
    sprite_pitch dw ?
    sprite_unk4 dw ?
    sprite_width2 dw ?
    sprite_left2 dw ?
    sprite_widthsum dw ?
SPRITE ends
SPRITE_LEFT2_OFFSET equ 26
SPRITE_WIDTHSUM_OFFSET equ 28
SPRITE_TOP_OFFSET equ 16
SPRITE_HEIGHT_OFFSET equ 18
_DATA segment word public 'DATA'
public _line_pattern_bits, _prerender_auxiliary_arg, _spritefunc, _imagefunc
_line_pattern_bits dw 0
_prerender_auxiliary_arg dw 0
_spritefunc dw 0
    dw seg S012_TEXT
_imagefunc dw 0
    dw seg S012_TEXT
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _sprite1:word
extrn _draw_filled_lines:far
extrn _preRender_line:far
extrn _draw_line_related:far
extrn _draw_line_related_alt:far
spritefunc equ _spritefunc
imagefunc equ _imagefunc
draw_filled_lines equ _draw_filled_lines
preRender_line equ _preRender_line
draw_line_related equ _draw_line_related
draw_line_related_alt equ _draw_line_related_alt
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _loc_3180A
public _preRender_default
public _preRender_default_alt
public _skybox_op_helper
public _preRender_wheel_helper4
public _preRender_helper
public _preRender_helper2
public _preRender_helper3
public _nopsub_31F39
public _nopsub_31F55
_preRender_default label far
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender default.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_default proc far
    var_A = byte ptr -10
     s = byte ptr 0
     r = byte ptr 2
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+var_A], 1
    jmp     short pre_default_alt_join_01
preRender_default endp
_preRender_default_alt label far
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender default alt.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_6 (word ptr 10).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_default_alt proc far
    var_A = byte ptr -10
     s = byte ptr 0
     r = byte ptr 2
    arg_6 = word ptr 10
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+var_A], 0
pre_default_alt_join_01:
    mov     ax, offset draw_filled_lines
    mov     spritefunc, ax
    mov     ax, offset preRender_line
    mov word ptr imagefunc, ax
    mov     si, [bp+arg_6]
    jmp     short _loc_3180A
preRender_default_alt endp
_skybox_op_helper label far
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by skybox op helper.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
skybox_op_helper proc far
    var_A = byte ptr -10
     s = byte ptr 0
     r = byte ptr 2
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+var_A], 1
    jmp     short wheel_raster_join_01
skybox_op_helper endp
_preRender_wheel_helper4 label far
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender wheel helper4.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_color (word ptr 6); arg_vertlinecount (word ptr 8); arg_vertlines (byte ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES, SS; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_wheel_helper4 proc far
    var_7D0 = word ptr -2000
    var_798 = word ptr -1944
    var_3D8 = word ptr -984
    var_18 = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = byte ptr -12
    var_A = byte ptr -10
    var_8 = word ptr -8
    var_vertlineptr = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_color = word ptr 6
    arg_vertlinecount = word ptr 8
    arg_vertlines = byte ptr 10
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+var_A], 0
wheel_raster_join_01:
    mov     ax, offset draw_filled_lines
    mov     spritefunc, ax
    mov     ax, offset preRender_line
    mov word ptr imagefunc, ax
    lea     si, [bp+arg_vertlines]
_loc_3180A:
    mov     [bp+var_vertlineptr], si
    mov     cx, [bp+arg_vertlinecount]
    mov     ax, cx
    dec     ax
    shl     ax, 1
    shl     ax, 1
    add     ax, si
    mov     [bp+var_8], ax
    cld
    mov     ax, ss
    mov     es, ax
    lea     ax, [bp+var_798]
    mov     [bp+var_18], ax
    mov     ax, word ptr cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    mov     [bp+var_2], ax
    mov     ax, word ptr cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    dec     ax
    mov     [bp+var_4], ax
    mov     ax, [si+2]
    mov     [bp+var_E], ax
    mov     [bp+var_12], ax
    mov     bx, [si]
    mov     dx, bx
    mov     [bp+var_10], si
    mov     [bp+var_14], si
    add     si, 4
    dec     cx
    jnz     short wheel_raster_loop_head_01
    push    [bp+arg_color]
    mov     si, [bp+var_vertlineptr]
    push    word ptr [si+2]
    push    word ptr [si]
    push    word ptr [si+2]
    push    word ptr [si]
    call    dword ptr imagefunc
    add     sp, 0Ah
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
wheel_raster_loop_head_01:
    mov     ax, [si+2]
    cmp     ax, [bp+var_E]
    jg      short wheel_raster_branch_path_01
    mov     [bp+var_E], ax
    mov     [bp+var_10], si
wheel_raster_branch_path_01:
    cmp     ax, [bp+var_12]
    jle     short wheel_raster_branch_path_02
    mov     [bp+var_12], ax
    mov     [bp+var_14], si
wheel_raster_branch_path_02:
    mov     ax, [si]
    cmp     ax, bx
    jge     short wheel_raster_branch_path_03
    mov     bx, ax
wheel_raster_branch_path_03:
    cmp     ax, dx
    jle     short wheel_raster_branch_path_04
    mov     dx, ax
wheel_raster_branch_path_04:
    add     si, 4
    loop    wheel_raster_loop_head_01
    cmp     dx, [bp+var_2]
    jl      short wheel_raster_branch_path_08
    cmp     bx, [bp+var_4]
    jge     short wheel_raster_branch_path_08
    mov     ax, [bp+var_12]
    cmp     ax, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jl      short wheel_raster_branch_path_08
    mov     cx, [bp+var_E]
    cmp     cx, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short wheel_raster_branch_path_08
    mov     [bp+var_C], 0
    cmp     dx, [bp+var_4]
    jg      short wheel_raster_branch_path_05
    cmp     bx, [bp+var_2]
    jl      short wheel_raster_branch_path_05
    cmp     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short wheel_raster_branch_path_05
    cmp     cx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short wheel_raster_branch_path_06
wheel_raster_branch_path_05:
    mov     [bp+var_C], 1
wheel_raster_branch_path_06:
    cmp     ax, cx
    jz      short wheel_raster_branch_path_07
    cmp     dx, bx
    jnz     short wheel_raster_branch_path_09
wheel_raster_branch_path_07:
    push    [bp+arg_color]
    mov     si, [bp+var_vertlineptr]
    push    ax
    push    dx
    push    cx
    push    bx
    call    dword ptr imagefunc
    add     sp, 0Ah
wheel_raster_branch_path_08:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
wheel_raster_branch_path_09:
    lea     si, [bp+var_7D0]
    mov     di, [bp+var_10]
wheel_raster_loop_head_02:
    mov     ax, [di+2]
    mov     cx, [di]
    add     di, 4
    cmp     di, [bp+var_8]
    jbe     short wheel_raster_branch_path_10
    mov     di, [bp+var_vertlineptr]
wheel_raster_branch_path_10:
    mov     dx, [di+2]
    cmp     dx, ax
    jle     short wheel_raster_branch_path_12
    push    si
    push    dx
    push    word ptr [di]
    push    ax
    push    cx
    cmp     [bp+var_C], 0
    jz      short wheel_raster_branch_path_11
    call    draw_line_related
    mov     [bp+var_16], di
    call near ptr preRender_helper
    jmp     short wheel_raster_join_02
    db 144
wheel_raster_branch_path_11:
    call    draw_line_related_alt
    mov     [bp+var_16], di
    call near ptr preRender_helper2
wheel_raster_join_02:
    mov     di, [bp+var_16]
    add     sp, 0Ah
wheel_raster_branch_path_12:
    cmp     di, [bp+var_14]
    jnz     short wheel_raster_loop_head_02
    mov     di, [bp+var_10]
wheel_raster_loop_head_03:
    mov     ax, [di+2]
    mov     cx, [di]
    sub     di, 4
    cmp     di, [bp+var_vertlineptr]
    jnb     short wheel_raster_branch_path_13
    mov     di, [bp+var_8]
wheel_raster_branch_path_13:
    mov     dx, [di+2]
    cmp     dx, ax
    jle     short wheel_raster_branch_path_15
    push    si
    push    dx
    push    word ptr [di]
    push    ax
    push    cx
    cmp     [bp+var_C], 0
    jz      short wheel_raster_branch_path_14
    call    draw_line_related
    jmp     short wheel_raster_join_03
    db 144
wheel_raster_branch_path_14:
    call    draw_line_related_alt
wheel_raster_join_03:
    mov     [bp+var_16], di
    call near ptr preRender_helper3
    mov     di, [bp+var_16]
    add     sp, 0Ah
wheel_raster_branch_path_15:
    cmp     di, [bp+var_14]
    jnz     short wheel_raster_loop_head_03
    mov     ax, [bp+var_12]
    cmp     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jl      short wheel_raster_branch_path_16
    mov     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    dec     ax
wheel_raster_branch_path_16:
    mov     bx, [bp+var_E]
    cmp     bx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short wheel_raster_branch_path_17
    mov     bx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
wheel_raster_branch_path_17:
    sub     ax, bx
    jle     short wheel_raster_branch_path_18
    inc     ax
    push    [bp+arg_color]
    push    ax
    push    bx
    shl     bx, 1
    lea     ax, [bp+var_3D8]
    add     ax, bx
    push    ax
    lea     ax, [bp+var_798]
    add     ax, bx
    push    ax
    call    dword ptr spritefunc
    add     sp, 0Ah
wheel_raster_branch_path_18:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
preRender_wheel_helper4 endp
_preRender_helper label near
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender helper.
; ABI: NEAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, CX, DX, SI, DI, BP, CS; balanced explicit saves/restores observed: CX, DI; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_helper proc near
    var_18 = word ptr -24
     s = byte ptr 0
     r = byte ptr 2
    mov     cx, [si+14h]
    or      cx, cx
    jle     short edge_clear_branch_path_01
    mov     di, [si+6]
    mov     dx, [si+4]
    add     dx, 8000h
    adc     di, 0
    sub     di, cx
    shl     di, 1
    add     di, [bp+var_18]
    mov     ax, word ptr cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    push    cx
    push    di
    rep stosw
    pop     di
    pop     cx
    add     di, 3C0h
    dec     ax
    rep stosw
edge_clear_branch_path_01:
    mov     cx, [si+18h]
    or      cx, cx
    jle     short edge_clear_branch_path_02
    mov     di, [si+6]
    mov     dx, [si+4]
    add     dx, 8000h
    adc     di, 0
    sub     di, cx
    shl     di, 1
    add     di, [bp+var_18]
    mov     ax, word ptr cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    push    cx
    push    di
    rep stosw
    pop     di
    pop     cx
    add     di, 3C0h
    dec     ax
    rep stosw
edge_clear_branch_path_02:
    mov     cx, [si+16h]
    or      cx, cx
    jle     short edge_clear_branch_path_03
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+var_18]
    mov     ax, word ptr cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    push    cx
    push    di
    rep stosw
    pop     di
    pop     cx
    add     di, 3C0h
    dec     ax
    rep stosw
edge_clear_branch_path_03:
    mov     cx, [si+1Ah]
    or      cx, cx
    jle     short preRender_helper2
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+var_18]
    mov     ax, word ptr cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    push    cx
    push    di
    rep stosw
    pop     di
    pop     cx
    add     di, 3C0h
    dec     ax
    rep stosw
preRender_helper endp
_preRender_helper2 label near
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender helper2.
; ABI: NEAR entry; source return form: RET mnemonic for NEAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, CS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_helper2 proc near
    var_18 = word ptr -24
     r = byte ptr 0
    mov     cx, [si+0Eh]
    or      cx, cx
    jle     short slope_fill_mode_case_02
    mov     di, [si+6]
    shl     di, 1
    add     di, [bp+var_18]
    mov     bl, [si+12h]
    xor     bh, bh
    shl     bx, 1
    jmp     cs:sprite_line_fill_mode_dispatch[bx]
sprite_line_fill_mode_dispatch     dw offset slope_fill_mode_case_02
    dw offset slope_fill_mode_case_02
    dw offset slope_fill_mode_case_01
    dw offset slope_fill_mode_case_03
    dw offset slope_fill_mode_case_04
    dw offset slope_fill_mode_case_05
    dw offset slope_fill_mode_case_06
    dw offset slope_fill_mode_case_07
    dw offset slope_fill_mode_case_08
    dw offset slope_fill_mode_case_02
slope_fill_mode_case_01:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_fill_loop_head_01:
    mov     [di+3C0h], ax
    stosw
    loop    slope_fill_loop_head_01
slope_fill_mode_case_02:
    retn
slope_fill_mode_case_03:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_fill_loop_head_02:
    mov     [di+3C0h], ax
    stosw
    dec     ax
    loop    slope_fill_loop_head_02
    retn
slope_fill_mode_case_04:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_fill_loop_head_03:
    mov     [di+3C0h], ax
    stosw
    inc     ax
    loop    slope_fill_loop_head_03
    retn
slope_fill_mode_case_05:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_fill_loop_head_04:
    mov     [di+3C0h], ax
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    slope_fill_loop_head_04
    retn
slope_fill_mode_case_06:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_fill_loop_head_05:
    mov     [di+3C0h], ax
    stosw
    add     dx, bx
    adc     ax, 0
    loop    slope_fill_loop_head_05
    retn
slope_fill_mode_case_07:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_fill_loop_head_06
    add     di, 2
slope_fill_loop_head_06:
    mov     [di+3C0h], ax
slope_fill_loop_head_07:
    add     dx, bx
    jnb     short slope_fill_branch_path_01
    stosw
    dec     ax
    loop    slope_fill_loop_head_06
    sub     di, 2
    retn
slope_fill_branch_path_01:
    dec     ax
    loop    slope_fill_loop_head_07
    inc     ax
    mov     [di], ax
    retn
slope_fill_mode_case_08:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_fill_loop_head_08
    add     di, 2
slope_fill_loop_head_08:
    mov     [di], ax
slope_fill_loop_head_09:
    add     dx, bx
    jnb     short slope_fill_branch_path_02
    mov     [di+3C0h], ax
    add     di, 2
    inc     ax
    loop    slope_fill_loop_head_08
    sub     di, 2
    retn
slope_fill_branch_path_02:
    inc     ax
    loop    slope_fill_loop_head_09
    dec     ax
    mov     [di+3C0h], ax
    retn
preRender_helper2 endp
_preRender_helper3 label near
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender helper3.
; ABI: NEAR entry; source return form: RET mnemonic for NEAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, CS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
preRender_helper3 proc near
    var_18 = word ptr -24
    var_C = byte ptr -12
    var_A = byte ptr -10
     r = byte ptr 0
    mov     cx, [si+0Eh]
    or      cx, cx
    jle     short slope_raster_branch_path_02
    mov     di, [si+6]
    shl     di, 1
    add     di, [bp+var_18]
    mov     bl, [si+12h]
    xor     bh, bh
    shl     bx, 1
    cmp     [bp+var_A], 0
    jnz     short slope_raster_branch_path_01
    jmp     cs:sprite_line_merge_mode_dispatch[bx]
slope_raster_branch_path_01:
    jmp     cs:sprite_line_merge_mode_alt[bx]
sprite_line_merge_mode_alt     dw offset slope_fill_mode_case_02
    dw offset slope_fill_mode_case_02
    dw offset slope_raster_mode_case_01
    dw offset slope_raster_mode_case_02
    dw offset slope_raster_mode_case_03
    dw offset slope_raster_mode_case_04
    dw offset slope_raster_mode_case_05
    dw offset slope_raster_mode_case_06
    dw offset slope_raster_mode_case_07
    dw offset slope_fill_mode_case_02
slope_raster_mode_case_01:
    mov     ax, [si+2]
slope_raster_loop_head_01:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_03
    cmp     [di], ax
    jle     short slope_raster_branch_path_04
    stosw
    loop    slope_raster_loop_head_01
slope_raster_branch_path_02:
    jmp     slope_raster_join_01
slope_raster_branch_path_03:
    mov     [di+3C0h], ax
slope_raster_branch_path_04:
    add     di, 2
    loop    slope_raster_loop_head_01
    jmp     slope_raster_join_01
slope_raster_mode_case_02:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_02:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_05
    cmp     [di], ax
    jle     short slope_raster_branch_path_06
    stosw
    dec     ax
    loop    slope_raster_loop_head_02
    jmp     slope_raster_join_01
slope_raster_branch_path_05:
    mov     [di+3C0h], ax
slope_raster_branch_path_06:
    add     di, 2
    dec     ax
    loop    slope_raster_loop_head_02
    jmp     slope_raster_join_01
slope_raster_mode_case_03:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_03:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_07
    cmp     [di], ax
    jle     short slope_raster_branch_path_08
    stosw
    inc     ax
    loop    slope_raster_loop_head_03
    jmp     slope_raster_join_01
slope_raster_branch_path_07:
    mov     [di+3C0h], ax
slope_raster_branch_path_08:
    add     di, 2
    inc     ax
    loop    slope_raster_loop_head_03
    jmp     slope_raster_join_01
slope_raster_mode_case_04:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_raster_loop_head_04:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_09
    cmp     [di], ax
    jle     short slope_raster_branch_path_10
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_04
    jmp     slope_raster_join_01
slope_raster_branch_path_09:
    mov     [di+3C0h], ax
slope_raster_branch_path_10:
    add     di, 2
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_04
    jmp     slope_raster_join_01
slope_raster_mode_case_05:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_raster_loop_head_05:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_11
    cmp     [di], ax
    jle     short slope_raster_branch_path_12
    stosw
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_05
    jmp     slope_raster_join_01
slope_raster_branch_path_11:
    mov     [di+3C0h], ax
slope_raster_branch_path_12:
    add     di, 2
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_05
    jmp     slope_raster_join_01
slope_raster_mode_case_06:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_raster_loop_head_06
    add     di, 2
slope_raster_loop_head_06:
    cmp     [di+3C0h], ax
    jge     short slope_raster_loop_head_07
    mov     [di+3C0h], ax
slope_raster_loop_head_07:
    add     dx, bx
    jnb     short slope_raster_branch_path_14
    cmp     [di], ax
    jle     short slope_raster_branch_path_13
    mov     [di], ax
slope_raster_branch_path_13:
    add     di, 2
    dec     ax
    loop    slope_raster_loop_head_06
    sub     di, 2
    jmp     slope_raster_join_01
slope_raster_branch_path_14:
    dec     ax
    loop    slope_raster_loop_head_07
    inc     ax
    cmp     [di], ax
    jle     short slope_raster_loop_head_08
    mov     [di], ax
slope_raster_loop_head_08:
    jmp     slope_raster_join_01
slope_raster_mode_case_07:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_raster_loop_head_09
    add     di, 2
slope_raster_loop_head_09:
    cmp     [di], ax
    jle     short slope_raster_loop_head_10
    mov     [di], ax
slope_raster_loop_head_10:
    add     dx, bx
    jnb     short slope_raster_branch_path_16
    cmp     [di+3C0h], ax
    jge     short slope_raster_branch_path_15
    mov     [di+3C0h], ax
slope_raster_branch_path_15:
    add     di, 2
    inc     ax
    loop    slope_raster_loop_head_09
    sub     di, 2
    jmp     slope_raster_join_01
slope_raster_branch_path_16:
    inc     ax
    loop    slope_raster_loop_head_10
    dec     ax
    cmp     [di+3C0h], ax
    jge     short slope_raster_loop_head_08
    mov     [di+3C0h], ax
    jmp     slope_raster_join_01
sprite_line_merge_mode_dispatch     dw offset slope_fill_mode_case_02
    dw offset slope_fill_mode_case_02
    dw offset slope_raster_mode_case_08
    dw offset slope_raster_mode_case_09
    dw offset slope_raster_mode_case_10
    dw offset slope_raster_mode_case_11
    dw offset slope_raster_mode_case_12
    dw offset slope_raster_mode_case_13
    dw offset slope_raster_mode_case_14
    dw offset slope_fill_mode_case_02
slope_raster_mode_case_08:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_11:
    cmp     [di], ax
    jg      short slope_raster_branch_path_17
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_18
    add     di, 2
    loop    slope_raster_loop_head_11
    jmp     slope_raster_join_01
slope_raster_branch_path_17:
    rep stosw
    jmp     slope_raster_join_01
slope_raster_branch_path_18:
    add     di, 3C0h
    rep stosw
    jmp     slope_raster_join_01
slope_raster_mode_case_09:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_12:
    cmp     [di], ax
    jg      short slope_raster_loop_head_13
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_19
    add     di, 2
    dec     ax
    loop    slope_raster_loop_head_12
    jmp     slope_raster_join_01
slope_raster_loop_head_13:
    stosw
    dec     ax
    loop    slope_raster_loop_head_13
    jmp     slope_raster_join_01
slope_raster_branch_path_19:
    add     di, 3C0h
slope_raster_loop_head_14:
    stosw
    dec     ax
    loop    slope_raster_loop_head_14
    jmp     slope_raster_join_01
slope_raster_mode_case_10:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
slope_raster_loop_head_15:
    cmp     [di], ax
    jg      short slope_raster_loop_head_16
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_20
    add     di, 2
    inc     ax
    loop    slope_raster_loop_head_15
    jmp     slope_raster_join_01
slope_raster_loop_head_16:
    stosw
    inc     ax
    loop    slope_raster_loop_head_16
    jmp     slope_raster_join_01
slope_raster_branch_path_20:
    add     di, 3C0h
slope_raster_loop_head_17:
    stosw
    inc     ax
    loop    slope_raster_loop_head_17
    jmp     slope_raster_join_01
slope_raster_mode_case_11:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_raster_loop_head_18:
    cmp     [di], ax
    jg      short slope_raster_loop_head_19
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_21
    add     di, 2
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_18
    jmp     slope_raster_join_01
slope_raster_loop_head_19:
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_19
    jmp     slope_raster_join_01
slope_raster_branch_path_21:
    add     di, 3C0h
slope_raster_loop_head_20:
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    slope_raster_loop_head_20
    jmp     slope_raster_join_01
slope_raster_mode_case_12:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
slope_raster_loop_head_21:
    cmp     [di], ax
    jg      short slope_raster_loop_head_22
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_22
    add     di, 2
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_21
    jmp     slope_raster_join_01
slope_raster_loop_head_22:
    stosw
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_22
    jmp     slope_raster_join_01
slope_raster_branch_path_22:
    add     di, 3C0h
slope_raster_loop_head_23:
    stosw
    add     dx, bx
    adc     ax, 0
    loop    slope_raster_loop_head_23
    jmp     slope_raster_join_01
slope_raster_mode_case_13:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_raster_loop_head_24
    add     di, 2
slope_raster_loop_head_24:
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_24
    add     dx, bx
    jnb     short slope_raster_branch_path_23
    cmp     [di], ax
    jg      short slope_raster_branch_path_26
    add     di, 2
slope_raster_branch_path_23:
    dec     ax
    loop    slope_raster_loop_head_24
    jmp     slope_raster_join_01
slope_raster_branch_path_24:
    add     di, 3C0h
slope_raster_loop_head_25:
    stosw
slope_raster_loop_head_26:
    dec     ax
    add     dx, bx
    jnb     short slope_raster_branch_path_25
    loop    slope_raster_loop_head_25
    jmp     short slope_raster_join_01
    db 144
slope_raster_branch_path_25:
    loop    slope_raster_loop_head_26
    jmp     short slope_raster_join_01
    db 144
slope_raster_loop_head_27:
    add     dx, bx
    jnb     short slope_raster_branch_path_27
slope_raster_branch_path_26:
    stosw
    dec     ax
    loop    slope_raster_loop_head_27
    jmp     short slope_raster_join_01
    db 144
slope_raster_branch_path_27:
    dec     ax
    loop    slope_raster_loop_head_27
    inc     ax
    mov     [di], ax
    jmp     short slope_raster_join_01
    db 144
slope_raster_mode_case_14:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short slope_raster_loop_head_28
    add     di, 2
slope_raster_loop_head_28:
    cmp     [di], ax
    jg      short slope_raster_branch_path_31
    add     dx, bx
    jnb     short slope_raster_branch_path_28
    cmp     [di+3C0h], ax
    jl      short slope_raster_branch_path_29
    add     di, 2
slope_raster_branch_path_28:
    inc     ax
    loop    slope_raster_loop_head_28
    jmp     short slope_raster_join_01
    db 144
slope_raster_branch_path_29:
    add     di, 3C0h
slope_raster_loop_head_29:
    mov     [di], ax
slope_raster_loop_head_30:
    inc     ax
    add     dx, bx
    jnb     short slope_raster_branch_path_30
    add     di, 2
    loop    slope_raster_loop_head_29
    dec     ax
    mov     [di], ax
    jmp     short slope_raster_join_01
    db 144
slope_raster_branch_path_30:
    loop    slope_raster_loop_head_30
    add     di, 2
    dec     ax
    mov     [di], ax
    jmp     short slope_raster_join_01
    db 144
slope_raster_loop_head_31:
    add     dx, bx
    jnb     short slope_raster_branch_path_32
slope_raster_branch_path_31:
    stosw
slope_raster_branch_path_32:
    inc     ax
    loop    slope_raster_loop_head_31
    jmp     short slope_raster_join_01
    db 144
slope_raster_join_01:
    cmp     [bp+var_C], 0
    jnz     short slope_raster_branch_path_33
    retn
slope_raster_branch_path_33:
    mov     cx, [si+14h]
    or      cx, cx
    jle     short slope_raster_branch_path_34
    mov     di, [si+6]
    mov     dx, [si+4]
    add     dx, 8000h
    adc     di, 0
    sub     di, cx
    shl     di, 1
    add     di, [bp+var_18]
    mov     ax, word ptr cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    rep stosw
slope_raster_branch_path_34:
    mov     cx, [si+18h]
    or      cx, cx
    jle     short slope_raster_branch_path_35
    mov     di, [si+6]
    mov     dx, [si+4]
    add     dx, 8000h
    adc     di, 0
    sub     di, cx
    shl     di, 1
    add     di, [bp+var_18]
    add     di, 3C0h
    mov     ax, word ptr cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    dec     ax
    rep stosw
slope_raster_branch_path_35:
    mov     cx, [si+16h]
    or      cx, cx
    jle     short slope_raster_branch_path_36
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+var_18]
    mov     ax, word ptr cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    rep stosw
slope_raster_branch_path_36:
    mov     cx, [si+1Ah]
    or      cx, cx
    jle     short slope_raster_return_01
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+var_18]
    add     di, 3C0h
    mov     ax, word ptr cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    dec     ax
    rep stosw
slope_raster_return_01:
    retn
preRender_helper3 endp
_nopsub_31F39 label far
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 31F39.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (byte ptr 10).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
nopsub_31F39 proc far
    var_7D0 = word ptr -2000
    var_798 = byte ptr -1944
    var_3D8 = byte ptr -984
    var_18 = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = byte ptr -12
    var_A = byte ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = byte ptr 10
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+var_A], 1
    mov     ax, offset draw_filled_lines
    mov     spritefunc, ax
    mov     ax, 49A0h
    mov word ptr imagefunc, ax
    jmp     short line_raster_join_01
    db 144
nopsub_31F39 endp
_nopsub_31F55 label near
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 31F55. Its internal flow uses line_raster_join_01, line_raster_branch_path_01.
; ABI: NEAR entry; source return form: RETF mnemonic for NEAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (byte ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES, SS; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
nopsub_31F55 proc near
    var_A = byte ptr -10
     s = byte ptr 0
     r = byte ptr 2
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     [bp+var_A], 0
    mov     ax, offset draw_filled_lines
    mov     spritefunc, ax
    mov     ax, 49A0h
    mov word ptr imagefunc, ax
    var_7D0 = word ptr -2000
    var_798 = byte ptr -1944
    var_3D8 = byte ptr -984
    var_18 = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = byte ptr -12
    var_A = byte ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = byte ptr 10
line_raster_join_01:
    lea     si, [bp+arg_4]
    mov     [bp+var_6], si
    mov     cx, [bp+arg_2]
    mov     ax, cx
    dec     ax
    shl     ax, 1
    add     ax, si
    mov     [bp+var_8], ax
    cld
    mov     ax, ss
    mov     es, ax
    lea     ax, [bp+var_798]
    mov     [bp+var_18], ax
    mov     ax, word ptr cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    mov     [bp+var_2], ax
    mov     ax, word ptr cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    dec     ax
    mov     [bp+var_4], ax
    mov     di, [si]
    mov     ax, [di+6]
    sub     ax, [di+14h]
    sub     ax, [di+18h]
    mov     [bp+var_E], ax
    mov     ax, [di+0Ah]
    add     ax, [di+16h]
    add     ax, [di+1Ah]
    mov     [bp+var_12], ax
    mov     [bp+var_7D0], 3
    mov     bx, [di+2]
    mov     dx, [di+8]
    cmp     bx, dx
    jle     short line_raster_branch_path_01
    xchg    bx, dx
line_raster_branch_path_01:
    mov     [bp+var_10], si
    mov     [bp+var_14], si
    add     si, 2
    dec     cx
    jnz     short line_raster_loop_head_01
    mov     ax, [bp+arg_0]
    mov     [bp+di+arg_0], ax
    push    di
    call    dword ptr imagefunc
    add     sp, 2
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_raster_loop_head_01:
    mov     di, [si]
    mov     ax, [di+6]
    sub     ax, [di+14h]
    sub     ax, [di+18h]
    cmp     ax, [bp+var_E]
    jg      short line_raster_branch_path_03
    jnz     short line_raster_branch_path_02
    test    [bp+var_7D0], 1
    jz      short line_raster_branch_path_04
line_raster_branch_path_02:
    mov     [bp+var_E], ax
    mov     [bp+var_10], si
    or      [bp+var_7D0], 1
    jmp     short line_raster_branch_path_04
    db 144
    var_7D0 = word ptr -2000
    var_798 = byte ptr -1944
    var_3D8 = byte ptr -984
    var_18 = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = byte ptr -12
    var_A = byte ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = byte ptr 10
line_raster_branch_path_03:
    and     [bp+var_7D0], 2
line_raster_branch_path_04:
    mov     ax, [di+0Ah]
    add     ax, [di+16h]
    add     ax, [di+1Ah]
    cmp     ax, [bp+var_12]
    jl      short line_raster_branch_path_07
    jnz     short line_raster_branch_path_06
line_raster_branch_path_05:
    test    [bp+var_7D0], 2
    jnz     short line_raster_branch_path_08
line_raster_branch_path_06:
    mov     [bp+var_12], ax
    mov     [bp+var_14], si
    or      [bp+var_7D0], 2
    jmp     short line_raster_branch_path_08
    db 144
    var_7D0 = word ptr -2000
    var_798 = byte ptr -1944
    var_3D8 = byte ptr -984
    var_18 = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = byte ptr -12
    var_A = byte ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = byte ptr 10
line_raster_branch_path_07:
    and     [bp+var_7D0], 1
line_raster_branch_path_08:
    mov     ax, [di+2]
    cmp     ax, bx
    jge     short line_raster_branch_path_09
    mov     bx, ax
line_raster_branch_path_09:
    cmp     ax, dx
    jle     short line_raster_branch_path_10
    mov     dx, ax
line_raster_branch_path_10:
    mov     ax, [di+8]
    cmp     ax, bx
    jge     short line_raster_branch_path_11
    mov     bx, ax
line_raster_branch_path_11:
    cmp     ax, dx
    jle     short line_raster_branch_path_12
    mov     dx, ax
line_raster_branch_path_12:
    add     si, 2
    loop    line_raster_loop_head_01
    cmp     dx, [bp+var_2]
    jl      short line_raster_branch_path_16
    cmp     bx, [bp+var_4]
    jge     short line_raster_branch_path_16
    mov     ax, [bp+var_12]
    cmp     ax, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jl      short line_raster_branch_path_16
    mov     cx, [bp+var_E]
    cmp     cx, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short line_raster_branch_path_16
    mov     [bp+var_C], 0
    cmp     dx, [bp+var_4]
    jg      short line_raster_branch_path_13
    cmp     bx, [bp+var_2]
    jl      short line_raster_branch_path_13
    cmp     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short line_raster_branch_path_13
    cmp     cx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short line_raster_branch_path_14
line_raster_branch_path_13:
    mov     [bp+var_C], 1
line_raster_branch_path_14:
    cmp     ax, cx
    jz      short line_raster_branch_path_15
    cmp     dx, bx
    jnz     short line_raster_branch_path_17
line_raster_branch_path_15:
    mov     si, [bp+var_6]
    mov     di, [si]
    push    di
    call    dword ptr imagefunc
    add     sp, 2
line_raster_branch_path_16:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_raster_branch_path_17:
    mov     di, [bp+var_10]
line_raster_loop_head_02:
    mov     si, [di]
    cmp     byte ptr [si+12h], 2
    jl      short line_raster_branch_path_19
    mov     [bp+var_16], di
    cmp     [bp+var_C], 0
    jz      short line_raster_branch_path_18
    call near ptr preRender_helper
    jmp     short line_raster_join_02
    db 144
    var_7D0 = word ptr -2000
    var_798 = byte ptr -1944
    var_3D8 = byte ptr -984
    var_18 = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
    var_E = word ptr -14
    var_C = byte ptr -12
    var_A = byte ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = byte ptr 10
line_raster_branch_path_18:
    call near ptr preRender_helper2
line_raster_join_02:
    mov     di, [bp+var_16]
line_raster_branch_path_19:
    cmp     di, [bp+var_14]
    jz      short line_raster_branch_path_20
    add     di, 2
    cmp     di, [bp+var_8]
    jbe     short line_raster_loop_head_02
    mov     di, [bp+var_6]
    jmp     short line_raster_loop_head_02
line_raster_branch_path_20:
    mov     di, [bp+var_10]
line_raster_loop_head_03:
    sub     di, 2
    cmp     di, [bp+var_6]
    jnb     short line_raster_branch_path_21
    mov     di, [bp+var_8]
line_raster_branch_path_21:
    cmp     di, [bp+var_14]
    jz      short line_raster_branch_path_22
    mov     si, [di]
    cmp     byte ptr [si+12h], 2
    jl      short line_raster_loop_head_03
    mov     [bp+var_16], di
    call near ptr preRender_helper3
    mov     di, [bp+var_16]
    jmp     short line_raster_loop_head_03
line_raster_branch_path_22:
    mov     ax, [bp+var_12]
    cmp     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jl      short line_raster_branch_path_23
    mov     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    dec     ax
line_raster_branch_path_23:
    mov     bx, [bp+var_E]
    cmp     bx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short line_raster_branch_path_24
    mov     bx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
line_raster_branch_path_24:
    sub     ax, bx
    jle     short line_raster_branch_path_25
    inc     ax
    push    [bp+arg_0]
    push    ax
    push    bx
    shl     bx, 1
    lea     ax, [bp+var_3D8]
    add     ax, bx
    push    ax
    lea     ax, [bp+var_798]
    add     ax, bx
    push    ax
    call    dword ptr spritefunc
    add     sp, 0Ah
line_raster_branch_path_25:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
nopsub_31F55 endp
S012_TEXT ends
end
