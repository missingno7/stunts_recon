.8086
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
    jmp     short loc_340BD ; goto _sprite_putimage_or
    ; align 2
    db 144
_sprite_putimage_or_alt endp
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
    jmp     short loc_340BD
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
loc_340BD:
    mov     bx, [bp+var_4]
    mov     cx, [si+2]
    lea     dx, [si+10h]
    mov     ax, bx
    cmp     ax, cs:_sprite1+SPRITE_TOP
    jl      short loc_340E3
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_HEIGHT
    jle     short loc_3410A
    sub     cx, ax
    jg      short loc_3410A
loc_340DC:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_340E3:
    mov     bx, cs:_sprite1+SPRITE_TOP
    add     ax, cx
    sub     ax, bx
    jle     short loc_340DC
    xchg    ax, cx
    sub     ax, cx
    mov     [bp+var_8], dx
    mul     word ptr [si]
    mov     dx, [bp+var_8]
    add     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_HEIGHT
    jle     short loc_3410A
    sub     cx, ax
    jle     short loc_340DC
loc_3410A:
    mov     [bp+var_C], dx
    mov     [bp+var_A], cx
    mov     [bp+var_4], bx
    mov     cx, [si]
    xor     dx, dx
    mov     bx, [bp+var_2]
    mov     ax, bx
    cmp     ax, cs:_sprite1+SPRITE_LEFT
    jl      short loc_34135
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_RIGHT
    jl      short loc_3415C
    sub     cx, ax
    jle     short loc_340DC
    mov     dx, ax
    jmp     short loc_3415C
    db 144
loc_34135:
    mov     bx, cs:_sprite1+SPRITE_LEFT
    add     ax, cx
    sub     ax, bx
    jle     short loc_340DC
    mov     si, cx
    sub     si, ax
    add     [bp+var_C], si
    mov     si, cs:_sprite1+SPRITE_RIGHT
    sub     si, bx
    jle     short loc_340DC
    cmp     ax, si
    jl      short loc_34156
    mov     ax, si
loc_34156:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
loc_3415C:
    or      cx, cx
    jle     short loc_341A5
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
loc_34185:
    mov     bx, cs:_sprite1+SPRITE_PITCH
    sub     bx, [bp+var_6]
    cmp     [bp+var_8], 0
    jz      short loc_341A8
loc_34193:
    mov     cx, [bp+var_6]
loc_34196:
    lodsb
    or      es:[di], al
    inc     di
    loop    loc_34196
    add     si, [bp+var_8]
    add     di, bx
    dec     dx
    jg      short loc_34193
loc_341A5:
    jmp     loc_340DC
loc_341A8:
    mov     ax, [bp+var_6]
    shr     ax, 1
    mov     [bp+var_E], ax
    jnb     short loc_341CC
    jz      short loc_341DF
    inc     bx
loc_341B5:
    mov     cx, [bp+var_E]
loc_341B8:
    lodsw
    or      es:[di], ax
    inc     di
    inc     di
    loop    loc_341B8
    lodsb
    or      es:[di], al
    add     di, bx
    dec     dx
    jg      short loc_341B5
    jmp     loc_340DC
loc_341CC:
    mov     cx, [bp+var_E]
loc_341CF:
    lodsw
    or      es:[di], ax
    inc     di
    inc     di
    loop    loc_341CF
    add     di, bx
    dec     dx
    jg      short loc_341CC
    jmp     loc_340DC
loc_341DF:
    inc     bx
loc_341E0:
    lodsb
    or      es:[di], al
    add     di, bx
    inc     bx
    dec     dx
    jg      short loc_341E0
    jmp     loc_340DC
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
    jmp     short loc_3424B
    ; align 2
    db 144
_sprite_putimage_or endp
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
    jmp     short loc_3424B
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
loc_3424B:
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
    jnb     short loc_34295
    jz      short loc_342A7
    inc     bx
loc_3427A:
    mov     cx, [bp+var_6]
loc_3427D:
    lodsw
    or      es:[di], ax
    inc     di
    inc     di
    loop    loc_3427D
    lodsb
    or      es:[di], al
    add     di, bx
    dec     dx
    jg      short loc_3427A
loc_3428E:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_34295:
    mov     cx, [bp+var_6]
loc_34298:
    lodsw
    or      es:[di], ax
    inc     di
    inc     di
    loop    loc_34298
    add     di, bx
    dec     dx
    jg      short loc_34295
    jmp     short loc_3428E
loc_342A7:
    inc     bx
loc_342A8:
    lodsb
    or      es:[di], ax
    add     di, bx
    dec     dx
    jg      short loc_342A8
    jmp     short loc_3428E
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
    jmp     short loc_34311
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
    jmp     short loc_34311
    ; align 2
    db 144
_putpixel_iconFillings endp
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
loc_34311:
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
loc_3433C:
    lodsb
    or      al, al
    jg      short loc_3434A
    jl      short loc_3436A
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_3434A:
    mov     cl, al
    xor     ch, ch
    lodsb
loc_3434F:
    or      es:[di], al
    inc     di
    dec     dx
    jg      short loc_34366
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
loc_34366:
    loop    loc_3434F
    jmp     short loc_3433C
loc_3436A:
    neg     al
    mov     cl, al
    xor     ch, ch
loc_34370:
    lodsb
    or      es:[di], al
    inc     di
    dec     dx
    jg      short loc_34388
    add     [bp+var_10], 2
    mov     bx, [bp+var_10]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_6]
loc_34388:
    loop    loc_34370
    jmp     short loc_3433C
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
    jmp     short loc_343E9
    ; align 2
    db 144
_shape2d_op_unk4 endp
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
    jmp     short loc_343E9
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
loc_343E9:
    mov     bx, [bp+var_shapey]
    mov     cx, [si+SHAPE2D.s2d_height]
    lea     dx, [si+(size SHAPE2D)]
    mov     ax, bx
    cmp     ax, cs:_sprite1+SPRITE_TOP
    jl      short loc_3440F
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_HEIGHT
    jle     short loc_34436
    sub     cx, ax
    jg      short loc_34436
loc_34408:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_3440F:
    mov     bx, cs:_sprite1+SPRITE_TOP
    add     ax, cx
    sub     ax, bx
    jle     short loc_34408
    xchg    ax, cx
    sub     ax, cx
    mov     [bp+var_bmpptr], dx
    mul     [si+SHAPE2D.s2d_width]
    mov     dx, [bp+var_bmpptr]
    add     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_HEIGHT
    jle     short loc_34436
    sub     cx, ax
    jle     short loc_34408
loc_34436:
    mov     [bp+var_bmpptr], dx
    mov     [bp+var_A], cx
    mov     [bp+var_shapey], bx
    mov     cx, [si+SHAPE2D.s2d_width]
    xor     dx, dx
    mov     ax, [bp+var_shapex]
    mov     bx, ax
    cmp     ax, cs:_sprite1+SPRITE_LEFT
    jl      short loc_34461
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_RIGHT
    jl      short loc_34488
    sub     cx, ax
    jle     short loc_34408
    mov     dx, ax
    jmp     short loc_34488
    db 144
loc_34461:
    mov     bx, cs:_sprite1+SPRITE_LEFT
    add     ax, cx
    sub     ax, bx
    jle     short loc_34408
    mov     si, cx
    sub     si, ax
    add     [bp+var_bmpptr], si
    mov     si, cs:_sprite1+SPRITE_RIGHT
    sub     si, bx
    jle     short loc_34408
    cmp     ax, si
    jl      short loc_34482
    mov     ax, si
loc_34482:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
loc_34488:
    or      cx, cx
    jle     short loc_344DC
    mov     [bp+var_6], cx
    mov     [bp+var_8], dx
    mov     [bp+var_shapex], bx
    cld
    mov     es, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
    mov     bx, [bp+var_shapey]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     di, cs:[bx]
loc_344A8:
    add     di, [bp+var_shapex]
    mov     si, [bp+var_bmpptr]
    mov     dx, cs:_sprite1+SPRITE_PITCH
    sub     dx, [bp+var_6]
    xor     bh, bh
    mov     ah, 0FFh
loc_344BA:
    mov     cx, [bp+var_6]
loc_344BD:
    lodsb
    mov     bl, al
    mov     al, cs:_incnums[bx]
    cmp     al, ah
    jz      short loc_344CF
    stosb
    loop    loc_344BD
    jmp     short loc_344D2
    db 144
loc_344CF:
    inc     di
    loop    loc_344BD
loc_344D2:
    add     si, [bp+var_8]
    add     di, dx
    dec     [bp+var_A]
    jg      short loc_344BA
loc_344DC:
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
    jmp     short loc_34541
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
    jmp     short loc_34541
    ; align 2
    db 144
_sprite_putimage_transparent endp
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
loc_34541:
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
loc_3456E:
    mov     cx, [bp+var_6]
loc_34571:
    lodsb
    mov     bl, al
    mov     al, cs:_incnums[bx]
    cmp     al, 0FFh
    jz      short loc_34583
    stosb
    loop    loc_34571
    jmp     short loc_34586
    db 144
loc_34583:
    inc     di
    loop    loc_34571
loc_34586:
    add     di, dx
    dec     [bp+var_8]
    jg      short loc_3456E
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
    jmp     short loc_345E5
    ; align 2
    db 144
_release_shape_resources endp
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
    jmp     short loc_345E5
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    ds
    push    si
    push    di
    mov     ds, _fontdefseg
loc_345E5:
    cld
    mov     es, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
loc_345EB:
    mov     si, [bp+arg_0]
    mov     bl, ss:[si]
    or      bl, bl
    jz      short loc_3461F
    mov     al, bl
    inc     [bp+arg_0]
    xor     bh, bh
    shl     bx, 1
    add     bx, fh_glyphs
    mov     si, [bx]
    cmp     si, 0
    jnz     short loc_34626
    cmp     al, 0Dh
    jz      short loc_34610
    cmp     al, 0Ah
    jnz     short loc_345EB
loc_34610:
    mov     ax, ds:[fh_left]
    mov     word ptr ds:[fh_x], ax
    mov     ax, word ptr ds:[fh_lineheight]
    add     word ptr ds:[fh_y], ax
    jmp     short loc_345EB
loc_3461F:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_34626:
    mov     ax, word ptr ds:[fh_x]
    mov     [bp+var_2], ax
    cmp     byte ptr ds:[fh_proportional], 0
    jz      short loc_34647
    mov     al, [si]
    xor     ah, ah
    inc     si
    mov     word ptr ds:[fh_advance], ax
    add     ax, 7
    shr     ax, 1
    shr     ax, 1
    shr     ax, 1
    mov     byte ptr ds:[fh_rowbytes], al
loc_34647:
    mov     al, byte ptr ds:[fh_fgcolor]
    mov     ah, byte ptr ds:[fh_bgcolor]
    mov     cx, word ptr ds:[fh_height]
    mov     [bp+var_4], cx
    mov     bx, word ptr ds:[fh_y]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
loc_34660:
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dh, byte ptr ds:[fh_rowbytes]
loc_3466A:
    mov     cx, 8
    mov     dl, [si]
loc_3466F:
    shl     dl, 1
    jnb     short loc_3468C
    stosb
    loop    loc_3466F
    inc     si
    dec     dh
    jg      short loc_3466A
loc_3467B:
    inc     bx
    inc     bx
    dec     [bp+var_4]
    jg      short loc_34660
    mov     ax, word ptr ds:[fh_advance]
    add     word ptr ds:[fh_x], ax
    jmp     loc_345EB
loc_3468C:
    mov     es:[di], ah
    inc     di
    loop    loc_3466F
    inc     si
    dec     dh
    jg      short loc_3466A
    jmp     short loc_3467B
    ; align 2
    db 0
    push    bp
    mov     bp, sp
    mov     ax, [bp+0Ch]
    jmp     short loc_346A8
    db 144
_draw_text_at endp
_video_set_palette proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    mov     ax, ds
loc_346A8:
    mov     es, ax
    mov     bx, [bp+arg_0]
    mov     dx, [bp+arg_4]
    mov     cx, [bp+arg_2]
    mov     ah, 10h
    mov     al, 12h
    int     10h             ; - VIDEO - SET BLOCK OF DAC REGISTERS (EGA, VGA/MCGA)
    pop     bp
    retf
    ; align 2
    db 0
_video_set_palette endp

S012_TEXT ends
end
