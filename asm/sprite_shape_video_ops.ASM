.8086
INCLUDE include\platform_hw.inc
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
; Font header at DS/ES = _fontdefseg: 0Bh words, then the 256-entry glyph offset table.
FONTHDR struc
fh_fgcolor dw ?
fh_bgcolor dw ?
fh_left dw ?
fh_unk06 dw ?
fh_x dw ?
fh_y dw ?
fh_rowbytes db ?
fh_unk0D db ?
fh_height dw ?
fh_advance dw ?
fh_lineheight dw ?
fh_proportional db ?
fh_unk15 db ?
fh_glyphs dw 256 dup (?)
FONTHDR ends
SPRITE_BITMAPPTR = 00h
SPRITE_LINEOFS = 0Ah
SPRITE_LEFT = 0Ch
SPRITE_RIGHT = 0Eh
SPRITE_TOP = 10h
SPRITE_HEIGHT = 12h
SPRITE_PITCH = 14h
SPRITE_WIDTH2 = 18h
SPRITE_LEFT2 = 1Ah
SPRITE_WIDTHSUM = 1Ch

S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
    dd 3
_aCopyrightCUnlimitedSoftwareInc_198 db 'Copyright (C) Unlimited Software Inc. 1989,1990.  All rights reserved.', 6 dup (0)
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS, dseg
dseg segment byte public 'STUNTSD'
extrn _fontdefseg:word
dseg ends
assume cs:S012_TEXT,ds:DGROUP
extrn _sprite1:byte
extrn _incnums:byte
public _sprite_putimage_or_alt
public _sprite_putimage_or
public _putpixel_iconFillings
public _shape2d_op_unk4
public _sprite_putimage_transparent
public _release_shape_resources
public _draw_text_at
public _video_set_palette
S012_TEXT segment word public 'CODE'

; -----------------------------------------------------------------------------
; Purpose: Draw a sprite image using the alternate OR compositing path.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_2 (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sprite_putimage_or_alt proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 6
    arg_4 = word ptr 8
    arg_6 = word ptr 10
    arg_8 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short sprite_or_join_01 ; goto _sprite_putimage_or
    ; align 2
    db 144
_sprite_putimage_or_alt endp
; -----------------------------------------------------------------------------
; Purpose: Draw a sprite image using the OR compositing operation.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_sprite_putimage_or proc far
    var_E = word ptr -14
    var_C = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    mov     [bp+var_4], ax
    jmp     short sprite_or_join_01
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
sprite_or_join_01:
    mov     bx, [bp+var_4]
    mov     cx, [si+2]
    lea     dx, [si+10h]
    mov     ax, bx
    cmp     ax, cs:_sprite1+SPRITE_TOP
    jl      short sprite_or_branch_path_01
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_HEIGHT
    jle     short sprite_or_branch_path_02
    sub     cx, ax
    jg      short sprite_or_branch_path_02
sprite_or_loop_head_01:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
sprite_or_branch_path_01:
    mov     bx, cs:_sprite1+SPRITE_TOP
    add     ax, cx
    sub     ax, bx
    jle     short sprite_or_loop_head_01
    xchg    ax, cx
    sub     ax, cx
    mov     [bp+var_8], dx
    mul     word ptr [si]
    mov     dx, [bp+var_8]
    add     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_HEIGHT
    jle     short sprite_or_branch_path_02
    sub     cx, ax
    jle     short sprite_or_loop_head_01
sprite_or_branch_path_02:
    mov     [bp+var_C], dx
    mov     [bp+var_A], cx
    mov     [bp+var_4], bx
    mov     cx, [si]
    xor     dx, dx
    mov     bx, [bp+var_2]
    mov     ax, bx
    cmp     ax, cs:_sprite1+SPRITE_LEFT
    jl      short sprite_or_branch_path_03
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_RIGHT
    jl      short sprite_or_branch_path_05
    sub     cx, ax
    jle     short sprite_or_loop_head_01
    mov     dx, ax
    jmp     short sprite_or_branch_path_05
    db 144
sprite_or_branch_path_03:
    mov     bx, cs:_sprite1+SPRITE_LEFT
    add     ax, cx
    sub     ax, bx
    jle     short sprite_or_loop_head_01
    mov     si, cx
    sub     si, ax
    add     [bp+var_C], si
    mov     si, cs:_sprite1+SPRITE_RIGHT
    sub     si, bx
    jle     short sprite_or_loop_head_01
    cmp     ax, si
    jl      short sprite_or_branch_path_04
    mov     ax, si
sprite_or_branch_path_04:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
sprite_or_branch_path_05:
    or      cx, cx
    jle     short sprite_or_branch_path_07
    mov     [bp+var_6], cx
    mov     [bp+var_8], dx
    mov     [bp+var_2], bx
    cld
    mov     es, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     si, [bp+var_C]
    mov     dx, [bp+var_A]
sprite_or_branch_path_06:
    mov     bx, cs:_sprite1+SPRITE_PITCH
    sub     bx, [bp+var_6]
    cmp     [bp+var_8], 0
    jz      short sprite_or_branch_path_08
sprite_or_loop_head_02:
    mov     cx, [bp+var_6]
sprite_or_loop_head_03:
    lodsb
    or      es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    loop    sprite_or_loop_head_03
    add     si, [bp+var_8]
    add     di, bx
    dec     dx
    jg      short sprite_or_loop_head_02
sprite_or_branch_path_07:
    jmp     sprite_or_loop_head_01
sprite_or_branch_path_08:
    mov     ax, [bp+var_6]
    shr     ax, 1
    mov     [bp+var_E], ax
    jnb     short sprite_or_loop_head_06
    jz      short sprite_or_branch_path_09
    inc     bx
sprite_or_loop_head_04:
    mov     cx, [bp+var_E]
sprite_or_loop_head_05:
    lodsw
    or      es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    sprite_or_loop_head_05
    lodsb
    or      es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    dec     dx
    jg      short sprite_or_loop_head_04
    jmp     sprite_or_loop_head_01
sprite_or_loop_head_06:
    mov     cx, [bp+var_E]
sprite_or_loop_head_07:
    lodsw
    or      es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    sprite_or_loop_head_07
    add     di, bx
    dec     dx
    jg      short sprite_or_loop_head_06
    jmp     sprite_or_loop_head_01
sprite_or_branch_path_09:
    inc     bx
sprite_or_loop_head_08:
    lodsb
    or      es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    inc     bx
    dec     dx
    jg      short sprite_or_loop_head_08
    jmp     sprite_or_loop_head_01
    ; align 2
    db 0
    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     ax, [bp+0Ah]
    sub     ax, [si+4]
    mov     [bp-2], ax
    mov     ax, [bp+0Ch]
    sub     ax, [si+6]
    mov     [bp-4], ax
    jmp     short icon_fill_join_01
    ; align 2
    db 144
_sprite_putimage_or endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by putpixel iconFillings.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_putpixel_iconFillings proc far
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [bp+arg_4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    mov     [bp+var_4], ax
    jmp     short icon_fill_join_01
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
icon_fill_join_01:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     es, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
    cld
    mov     dx, [si+2]
    mov     bx, cs:_sprite1+SPRITE_PITCH
    mov     ax, [si]
    sub     bx, ax
    add     si, 10h
    shr     ax, 1
    mov     [bp+var_6], ax
    jnb     short icon_fill_loop_head_04
    jz      short icon_fill_branch_path_01
    inc     bx
icon_fill_loop_head_01:
    mov     cx, [bp+var_6]
icon_fill_loop_head_02:
    lodsw
    or      es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    icon_fill_loop_head_02
    lodsb
    or      es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    dec     dx
    jg      short icon_fill_loop_head_01
icon_fill_loop_head_03:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
icon_fill_loop_head_04:
    mov     cx, [bp+var_6]
icon_fill_loop_head_05:
    lodsw
    or      es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    icon_fill_loop_head_05
    add     di, bx
    dec     dx
    jg      short icon_fill_loop_head_04
    jmp     short icon_fill_loop_head_03
icon_fill_branch_path_01:
    inc     bx
icon_fill_loop_head_06:
    lodsb
    or      es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, bx
    dec     dx
    jg      short icon_fill_loop_head_06
    jmp     short icon_fill_loop_head_03
    ; align 2
    db 0
    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     ax, [bp+0Ah]
    sub     ax, [si+4]
    mov     [bp-2], ax
    mov     ax, [bp+0Ch]
    sub     ax, [si+6]
    mov     [bp-4], ax
    jmp     short shape_op4_join_01
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     ax, [bp+0Ah]
    mov     [bp-2], ax
    mov     ax, [bp+0Ch]
    mov     [bp-4], ax
    jmp     short shape_op4_join_01
    ; align 2
    db 144
_putpixel_iconFillings endp
; -----------------------------------------------------------------------------
; Purpose: Process a 2D shape operation variant.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_shapeofs (word ptr 6); arg_shapeseg (word ptr 8).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_shape2d_op_unk4 proc far
    var_10 = word ptr -16
    var_E = word ptr -14
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_shapeofs = word ptr 6
    arg_shapeseg = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shapeseg]
    mov     si, [bp+arg_shapeofs]
    mov     ax, [si+SHAPE2D.s2d_pos_x]
    mov     [bp+var_2], ax
    mov     ax, [si+SHAPE2D.s2d_pos_y]
    mov     [bp+var_4], ax
shape_op4_join_01:
    cld
    lea     ax, [si+(size SHAPE2D)]
    mov     [bp+var_E], ax
    mov     ax, [si]
    mov     [bp+var_6], ax
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     [bp+var_10], bx
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     ax, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
    mov     es, ax
    mov     si, [bp+var_E]
    mov     dx, [bp+var_6]
shape_op4_loop_head_01:
    lodsb
    or      al, al
    jg      short shape_op4_branch_path_01
    jl      short shape_op4_branch_path_03
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
shape_op4_branch_path_01:
    mov     cl, al
    xor     ch, ch
    lodsb
shape_op4_loop_head_02:
    or      es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    dec     dx
    jg      short shape_op4_branch_path_02
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
shape_op4_branch_path_02:
    loop    shape_op4_loop_head_02
    jmp     short shape_op4_loop_head_01
shape_op4_branch_path_03:
    neg     al
    mov     cl, al
    xor     ch, ch
shape_op4_loop_head_03:
    lodsb
    or      es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    dec     dx
    jg      short shape_op4_branch_path_04
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
shape_op4_branch_path_04:
    loop    shape_op4_loop_head_03
    jmp     short shape_op4_loop_head_01
    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     ax, [bp+0Ah]
    sub     ax, [si+4]
    mov     [bp-2], ax
    mov     ax, [bp+0Ch]
    sub     ax, [si+6]
    mov     [bp-4], ax
    jmp     short sprite_alpha_join_01
    ; align 2
    db 144
_shape2d_op_unk4 endp
; -----------------------------------------------------------------------------
; Purpose: Draw a sprite image while preserving transparent pixels.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_4 (word ptr 8); arg_6 (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _incnums, _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
_sprite_putimage_transparent proc far
    var_bmpptr = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_shapey = word ptr -4
    var_shapex = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    shapeptr = dword ptr 6
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+shapeptr+2]
    mov     si, word ptr [bp+shapeptr]
    mov     ax, [bp+arg_4]
    mov     [bp+var_shapex], ax
    mov     ax, [bp+arg_6]
    mov     [bp+var_shapey], ax
    jmp     short sprite_alpha_join_01
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Ch
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+shapeptr+2]
    mov     si, word ptr [bp+shapeptr]
    mov     ax, [si+SHAPE2D.s2d_pos_x]
    mov     [bp+var_shapex], ax
    mov     ax, [si+SHAPE2D.s2d_pos_y]
    mov     [bp+var_shapey], ax
sprite_alpha_join_01:
    mov     bx, [bp+var_shapey]
    mov     cx, [si+SHAPE2D.s2d_height]
    lea     dx, [si+(size SHAPE2D)]
    mov     ax, bx
    cmp     ax, cs:_sprite1+SPRITE_TOP
    jl      short sprite_alpha_branch_path_01
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_HEIGHT
    jle     short sprite_alpha_branch_path_02
    sub     cx, ax
    jg      short sprite_alpha_branch_path_02
sprite_alpha_loop_head_01:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
sprite_alpha_branch_path_01:
    mov     bx, cs:_sprite1+SPRITE_TOP
    add     ax, cx
    sub     ax, bx
    jle     short sprite_alpha_loop_head_01
    xchg    ax, cx
    sub     ax, cx
    mov     [bp+var_bmpptr], dx
    mul     [si+SHAPE2D.s2d_width]
    mov     dx, [bp+var_bmpptr]
    add     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_HEIGHT
    jle     short sprite_alpha_branch_path_02
    sub     cx, ax
    jle     short sprite_alpha_loop_head_01
sprite_alpha_branch_path_02:
    mov     [bp+var_bmpptr], dx
    mov     [bp+var_A], cx
    mov     [bp+var_shapey], bx
    mov     cx, [si+SHAPE2D.s2d_width]
    xor     dx, dx
    mov     ax, [bp+var_shapex]
    mov     bx, ax
    cmp     ax, cs:_sprite1+SPRITE_LEFT
    jl      short sprite_alpha_branch_path_03
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_RIGHT
    jl      short sprite_alpha_branch_path_05
    sub     cx, ax
    jle     short sprite_alpha_loop_head_01
    mov     dx, ax
    jmp     short sprite_alpha_branch_path_05
    db 144
sprite_alpha_branch_path_03:
    mov     bx, cs:_sprite1+SPRITE_LEFT
    add     ax, cx
    sub     ax, bx
    jle     short sprite_alpha_loop_head_01
    mov     si, cx
    sub     si, ax
    add     [bp+var_bmpptr], si
    mov     si, cs:_sprite1+SPRITE_RIGHT
    sub     si, bx
    jle     short sprite_alpha_loop_head_01
    cmp     ax, si
    jl      short sprite_alpha_branch_path_04
    mov     ax, si
sprite_alpha_branch_path_04:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
sprite_alpha_branch_path_05:
    or      cx, cx
    jle     short sprite_alpha_branch_path_08
    mov     [bp+var_6], cx
    mov     [bp+var_8], dx
    mov     [bp+var_shapex], bx
    cld
    mov     es, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
    mov     bx, [bp+var_shapey]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     di, cs:[bx]
sprite_alpha_branch_path_06:
    add     di, [bp+var_shapex]
    mov     si, [bp+var_bmpptr]
    mov     dx, cs:_sprite1+SPRITE_PITCH
    sub     dx, [bp+var_6]
    xor     bh, bh
    mov     ah, 0FFh
sprite_alpha_loop_head_02:
    mov     cx, [bp+var_6]
sprite_alpha_loop_head_03:
    lodsb
    mov     bl, al
    mov     al, cs:_incnums[bx]
    cmp     al, ah
    jz      short sprite_alpha_branch_path_07
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    loop    sprite_alpha_loop_head_03
    jmp     short sprite_alpha_join_02
    db 144
sprite_alpha_branch_path_07:
    inc     di
    loop    sprite_alpha_loop_head_03
sprite_alpha_join_02:
    add     si, [bp+var_8]
    add     di, dx
    dec     [bp+var_A]
    jg      short sprite_alpha_loop_head_02
sprite_alpha_branch_path_08:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
    ; align 2
    db 0
    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     ax, [bp+0Ah]
    sub     ax, [si+4]
    mov     [bp-2], ax
    mov     ax, [bp+0Ch]
    sub     ax, [si+6]
    mov     [bp-4], ax
    jmp     short shape_release_join_01
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     ax, [bp+0Ah]
    mov     [bp-2], ax
    mov     ax, [bp+0Ch]
    mov     [bp-4], ax
    jmp     short shape_release_join_01
    ; align 2
    db 144
_sprite_putimage_transparent endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by release shape resources.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _fontdefseg, _incnums, _sprite1.
; Globals written: _aCopyrightCUnlimitedSoftwareInc_198, fh_x, fh_y.
; Global addresses taken: _aCopyrightCUnlimitedSoftwareInc_198.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
_release_shape_resources proc far
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
shape_release_join_01:
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, cs:_sprite1+SPRITE_PITCH
    mov     ax, [si]
    mov     [bp+var_6], ax
    sub     dx, ax
    mov     ax, [si+2]
    mov     [bp+var_8], ax
    mov     es, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
    cld
    add     si, 10h
    xor     bx, bx
shape_release_loop_head_01:
    mov     cx, [bp+var_6]
shape_release_loop_head_02:
    lodsb
    mov     bl, al
    mov     al, cs:_incnums[bx]
    cmp     al, 0FFh
    jz      short shape_release_branch_path_01
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    loop    shape_release_loop_head_02
    jmp     short shape_release_join_02
    db 144
shape_release_branch_path_01:
    inc     di
    loop    shape_release_loop_head_02
shape_release_join_02:
    add     di, dx
    dec     [bp+var_8]
    jg      short shape_release_loop_head_01
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    ds
    push    si
    push    di
    mov     ax, [bp+6]
    mov     byte ptr _aCopyrightCUnlimitedSoftwareInc_198+46h, al
    mov     ax, offset DGROUP:_aCopyrightCUnlimitedSoftwareInc_198+46h
    mov     [bp+6], ax
    mov     ds, _fontdefseg
    mov     ax, [bp+8]
    mov     word ptr ds:[fh_x], ax
    mov     ax, [bp+0Ah]
    mov     word ptr ds:[fh_y], ax
    jmp     short draw_text_join_01
    ; align 2
    db 144
_release_shape_resources endp
; -----------------------------------------------------------------------------
; Purpose: Draw or clear the graphics content identified by draw text at.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 6); arg_4 (word ptr 8).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, CS, DS, ES, SS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _fontdefseg, _sprite1, fh_advance, fh_bgcolor, fh_fgcolor, fh_glyphs, fh_height, fh_left, fh_lineheight, fh_proportional, fh_rowbytes, fh_x, fh_y.
; Globals written: fh_advance, fh_rowbytes, fh_x, fh_y.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_draw_text_at proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    sub     sp, 4
    push    ds
    push    si
    push    di
    mov     ds, _fontdefseg  ; ds = seg039
    mov     ax, [bp+arg_2]
    mov     ds:[fh_x], ax
    mov     ax, [bp+arg_4]
    mov     word ptr ds:[fh_y], ax
    jmp     short draw_text_join_01
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    ds
    push    si
    push    di
    mov     ds, _fontdefseg
draw_text_join_01:
    cld
    mov     es, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
draw_text_loop_head_01:
    mov     si, [bp+arg_0]
    mov     bl, ss:[si]
    or      bl, bl
    jz      short draw_text_branch_path_02
    mov     al, bl
    inc     [bp+arg_0]
    xor     bh, bh
    shl     bx, 1
    add     bx, fh_glyphs
    mov     si, [bx]
    cmp     si, 0
    jnz     short draw_text_branch_path_03
    cmp     al, 0Dh
    jz      short draw_text_branch_path_01
    cmp     al, 0Ah
    jnz     short draw_text_loop_head_01
draw_text_branch_path_01:
    mov     ax, ds:[fh_left]
    mov     word ptr ds:[fh_x], ax
    mov     ax, word ptr ds:[fh_lineheight]
    add     word ptr ds:[fh_y], ax
    jmp     short draw_text_loop_head_01
draw_text_branch_path_02:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
draw_text_branch_path_03:
    mov     ax, word ptr ds:[fh_x]
    mov     [bp+var_2], ax
    cmp     byte ptr ds:[fh_proportional], 0
    jz      short draw_text_branch_path_04
    mov     al, [si]
    xor     ah, ah
    inc     si
    mov     word ptr ds:[fh_advance], ax
    add     ax, 7
    shr     ax, 1
    shr     ax, 1
    shr     ax, 1
    mov     byte ptr ds:[fh_rowbytes], al
draw_text_branch_path_04:
    mov     al, byte ptr ds:[fh_fgcolor]
    mov     ah, byte ptr ds:[fh_bgcolor]
    mov     cx, word ptr ds:[fh_height]
    mov     [bp+var_4], cx
    mov     bx, word ptr ds:[fh_y]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
draw_text_loop_head_02:
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dh, byte ptr ds:[fh_rowbytes]
draw_text_loop_head_03:
    mov     cx, 8
    mov     dl, [si]
draw_text_loop_head_04:
    shl     dl, 1
    jnb     short draw_text_branch_path_05
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    loop    draw_text_loop_head_04
    inc     si
    dec     dh
    jg      short draw_text_loop_head_03
draw_text_loop_head_05:
    inc     bx
    inc     bx
    dec     [bp+var_4]
    jg      short draw_text_loop_head_02
    mov     ax, word ptr ds:[fh_advance]
    add     word ptr ds:[fh_x], ax
    jmp     draw_text_loop_head_01
draw_text_branch_path_05:
    mov     es:[di], ah ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    loop    draw_text_loop_head_04
    inc     si
    dec     dh
    jg      short draw_text_loop_head_03
    jmp     short draw_text_loop_head_05
    ; align 2
    db 0
    push    bp
    mov     bp, sp
    mov     ax, [bp+0Ch]
    jmp     short palette_join_01
    db 144
_draw_text_at endp
; -----------------------------------------------------------------------------
; Purpose: Load a block of palette entries through the BIOS video service.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 6); arg_4 (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, BP, SP, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(palette): calls the BIOS palette service. */
; -----------------------------------------------------------------------------
_video_set_palette proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    mov     ax, ds
palette_join_01:
    mov     es, ax
    mov     bx, [bp+arg_0]
    mov     dx, [bp+arg_4]
    mov     cx, [bp+arg_2]
    mov     ah, 10h
    mov     al, 12h
    int     R5HW_BIOS_VIDEO_INT ; - VIDEO - SET BLOCK OF DAC REGISTERS (EGA, VGA/MCGA) /* PLATFORM(palette): calls the BIOS palette service. */
    pop     bp
    retf
    ; align 2
    db 0
_video_set_palette endp

S012_TEXT ends
end
