.model medium
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
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _spritefunc:word
extrn _imagefunc:dword
DSEG ends
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
assume cs:S012_TEXT, ds:DSEG
public _loc_3180A
public _preRender_default, _preRender_default_alt, _skybox_op_helper, _preRender_wheel_helper4, _preRender_helper, _preRender_helper2, _preRender_helper3, _nopsub_31F39, _nopsub_31F55
_preRender_default label far
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
    jmp     short loc_317CE
preRender_default endp
_preRender_default_alt label far
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
loc_317CE:
    mov     ax, offset draw_filled_lines
    mov     spritefunc, ax
    mov     ax, offset preRender_line
    mov word ptr imagefunc, ax
    mov     si, [bp+arg_6]
    jmp     short _loc_3180A
preRender_default_alt endp
_skybox_op_helper label far
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
    jmp     short loc_317FB
skybox_op_helper endp
_preRender_wheel_helper4 label far
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
loc_317FB:
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
    jnz     short loc_3186D
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
loc_3186D:
    mov     ax, [si+2]
    cmp     ax, [bp+var_E]
    jg      short loc_3187B
    mov     [bp+var_E], ax
    mov     [bp+var_10], si
loc_3187B:
    cmp     ax, [bp+var_12]
    jle     short loc_31886
    mov     [bp+var_12], ax
    mov     [bp+var_14], si
loc_31886:
    mov     ax, [si]
    cmp     ax, bx
    jge     short loc_3188E
    mov     bx, ax
loc_3188E:
    cmp     ax, dx
    jle     short loc_31894
    mov     dx, ax
loc_31894:
    add     si, 4
    loop    loc_3186D
    cmp     dx, [bp+var_2]
    jl      short loc_318F0
    cmp     bx, [bp+var_4]
    jge     short loc_318F0
    mov     ax, [bp+var_12]
    cmp     ax, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jl      short loc_318F0
    mov     cx, [bp+var_E]
    cmp     cx, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short loc_318F0
    mov     [bp+var_C], 0
    cmp     dx, [bp+var_4]
    jg      short loc_318D3
    cmp     bx, [bp+var_2]
    jl      short loc_318D3
    cmp     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short loc_318D3
    cmp     cx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short loc_318D7
loc_318D3:
    mov     [bp+var_C], 1
loc_318D7:
    cmp     ax, cx
    jz      short loc_318DF
    cmp     dx, bx
    jnz     short loc_318F6
loc_318DF:
    push    [bp+arg_color]
    mov     si, [bp+var_vertlineptr]
    push    ax
    push    dx
    push    cx
    push    bx
    call    dword ptr imagefunc
    add     sp, 0Ah
loc_318F0:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_318F6:
    lea     si, [bp+var_7D0]
    mov     di, [bp+var_10]
loc_318FD:
    mov     ax, [di+2]
    mov     cx, [di]
    add     di, 4
    cmp     di, [bp+var_8]
    jbe     short loc_3190D
    mov     di, [bp+var_vertlineptr]
loc_3190D:
    mov     dx, [di+2]
    cmp     dx, ax
    jle     short loc_3193F
    push    si
    push    dx
    push    word ptr [di]
    push    ax
    push    cx
    cmp     [bp+var_C], 0
    jz      short loc_3192E
    call    draw_line_related
    mov     [bp+var_16], di
    call near ptr preRender_helper
    jmp     short loc_31939
    db 144
loc_3192E:
    call    draw_line_related_alt
    mov     [bp+var_16], di
    call near ptr preRender_helper2
loc_31939:
    mov     di, [bp+var_16]
    add     sp, 0Ah
loc_3193F:
    cmp     di, [bp+var_14]
    jnz     short loc_318FD
    mov     di, [bp+var_10]
loc_31947:
    mov     ax, [di+2]
    mov     cx, [di]
    sub     di, 4
    cmp     di, [bp+var_vertlineptr]
    jnb     short loc_31957
    mov     di, [bp+var_8]
loc_31957:
    mov     dx, [di+2]
    cmp     dx, ax
    jle     short loc_31983
    push    si
    push    dx
    push    word ptr [di]
    push    ax
    push    cx
    cmp     [bp+var_C], 0
    jz      short loc_31972
    call    draw_line_related
    jmp     short loc_31977
    db 144
loc_31972:
    call    draw_line_related_alt
loc_31977:
    mov     [bp+var_16], di
    call near ptr preRender_helper3
    mov     di, [bp+var_16]
    add     sp, 0Ah
loc_31983:
    cmp     di, [bp+var_14]
    jnz     short loc_31947
    mov     ax, [bp+var_12]
    cmp     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jl      short loc_31997
    mov     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    dec     ax
loc_31997:
    mov     bx, [bp+var_E]
    cmp     bx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short loc_319A6
    mov     bx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
loc_319A6:
    sub     ax, bx
    jle     short loc_319C7
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
loc_319C7:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
preRender_wheel_helper4 endp
_preRender_helper label near
preRender_helper proc near
    var_18 = word ptr -24
     s = byte ptr 0
     r = byte ptr 2
    mov     cx, [si+14h]
    or      cx, cx
    jle     short loc_319F9
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
loc_319F9:
    mov     cx, [si+18h]
    or      cx, cx
    jle     short loc_31A25
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
loc_31A25:
    mov     cx, [si+16h]
    or      cx, cx
    jle     short loc_31A46
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
loc_31A46:
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
preRender_helper2 proc near
    var_18 = word ptr -24
     r = byte ptr 0
    mov     cx, [si+0Eh]
    or      cx, cx
    jle     short locret_31AA3
    mov     di, [si+6]
    shl     di, 1
    add     di, [bp+var_18]
    mov     bl, [si+12h]
    xor     bh, bh
    shl     bx, 1
    jmp     cs:off_31A82[bx]
off_31A82     dw offset locret_31AA3
    dw offset locret_31AA3
    dw offset loc_31A96
    dw offset loc_31AA4
    dw offset loc_31AB3
    dw offset loc_31AC2
    dw offset loc_31AE1
    dw offset loc_31B00
    dw offset loc_31B2C
    dw offset locret_31AA3
loc_31A96:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
loc_31A9C:
    mov     [di+3C0h], ax
    stosw
    loop    loc_31A9C
locret_31AA3:
    retn
loc_31AA4:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
loc_31AAA:
    mov     [di+3C0h], ax
    stosw
    dec     ax
    loop    loc_31AAA
    retn
loc_31AB3:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
loc_31AB9:
    mov     [di+3C0h], ax
    stosw
    inc     ax
    loop    loc_31AB9
    retn
loc_31AC2:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
loc_31AD4:
    mov     [di+3C0h], ax
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    loc_31AD4
    retn
loc_31AE1:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
loc_31AF3:
    mov     [di+3C0h], ax
    stosw
    add     dx, bx
    adc     ax, 0
    loop    loc_31AF3
    retn
loc_31B00:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short loc_31B15
    add     di, 2
loc_31B15:
    mov     [di+3C0h], ax
loc_31B19:
    add     dx, bx
    jnb     short loc_31B25
    stosw
    dec     ax
    loop    loc_31B15
    sub     di, 2
    retn
loc_31B25:
    dec     ax
    loop    loc_31B19
    inc     ax
    mov     [di], ax
    retn
loc_31B2C:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short loc_31B41
    add     di, 2
loc_31B41:
    mov     [di], ax
loc_31B43:
    add     dx, bx
    jnb     short loc_31B55
    mov     [di+3C0h], ax
    add     di, 2
    inc     ax
    loop    loc_31B41
    sub     di, 2
    retn
loc_31B55:
    inc     ax
    loop    loc_31B43
    dec     ax
    mov     [di+3C0h], ax
    retn
preRender_helper2 endp
_preRender_helper3 label near
preRender_helper3 proc near
    var_18 = word ptr -24
    var_C = byte ptr -12
    var_A = byte ptr -10
     r = byte ptr 0
    mov     cx, [si+0Eh]
    or      cx, cx
    jle     short loc_31BA8
    mov     di, [si+6]
    shl     di, 1
    add     di, [bp+var_18]
    mov     bl, [si+12h]
    xor     bh, bh
    shl     bx, 1
    cmp     [bp+var_A], 0
    jnz     short loc_31B7F
    jmp     cs:off_31CF7[bx]
loc_31B7F:
    jmp     cs:off_31B84[bx]
off_31B84     dw offset locret_31AA3
    dw offset locret_31AA3
    dw offset loc_31B98
    dw offset loc_31BB7
    dw offset loc_31BDB
    dw offset loc_31BFF
    dw offset loc_31C37
    dw offset loc_31C6F
    dw offset loc_31CB1
    dw offset locret_31AA3
loc_31B98:
    mov     ax, [si+2]
loc_31B9B:
    cmp     [di+3C0h], ax
    jl      short loc_31BAB
    cmp     [di], ax
    jle     short loc_31BAF
    stosw
    loop    loc_31B9B
loc_31BA8:
    jmp     loc_31EB9
loc_31BAB:
    mov     [di+3C0h], ax
loc_31BAF:
    add     di, 2
    loop    loc_31B9B
    jmp     loc_31EB9
loc_31BB7:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
loc_31BBD:
    cmp     [di+3C0h], ax
    jl      short loc_31BCE
    cmp     [di], ax
    jle     short loc_31BD2
    stosw
    dec     ax
    loop    loc_31BBD
    jmp     loc_31EB9
loc_31BCE:
    mov     [di+3C0h], ax
loc_31BD2:
    add     di, 2
    dec     ax
    loop    loc_31BBD
    jmp     loc_31EB9
loc_31BDB:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
loc_31BE1:
    cmp     [di+3C0h], ax
    jl      short loc_31BF2
    cmp     [di], ax
    jle     short loc_31BF6
    stosw
    inc     ax
    loop    loc_31BE1
    jmp     loc_31EB9
loc_31BF2:
    mov     [di+3C0h], ax
loc_31BF6:
    add     di, 2
    inc     ax
    loop    loc_31BE1
    jmp     loc_31EB9
loc_31BFF:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
loc_31C11:
    cmp     [di+3C0h], ax
    jl      short loc_31C26
    cmp     [di], ax
    jle     short loc_31C2A
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    loc_31C11
    jmp     loc_31EB9
loc_31C26:
    mov     [di+3C0h], ax
loc_31C2A:
    add     di, 2
    sub     dx, bx
    sbb     ax, 0
    loop    loc_31C11
    jmp     loc_31EB9
loc_31C37:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
loc_31C49:
    cmp     [di+3C0h], ax
    jl      short loc_31C5E
    cmp     [di], ax
    jle     short loc_31C62
    stosw
    add     dx, bx
    adc     ax, 0
    loop    loc_31C49
    jmp     loc_31EB9
loc_31C5E:
    mov     [di+3C0h], ax
loc_31C62:
    add     di, 2
    add     dx, bx
    adc     ax, 0
    loop    loc_31C49
    jmp     loc_31EB9
loc_31C6F:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short loc_31C84
    add     di, 2
loc_31C84:
    cmp     [di+3C0h], ax
    jge     short loc_31C8E
    mov     [di+3C0h], ax
loc_31C8E:
    add     dx, bx
    jnb     short loc_31CA4
    cmp     [di], ax
    jle     short loc_31C98
    mov     [di], ax
loc_31C98:
    add     di, 2
    dec     ax
    loop    loc_31C84
    sub     di, 2
    jmp     loc_31EB9
loc_31CA4:
    dec     ax
    loop    loc_31C8E
    inc     ax
    cmp     [di], ax
    jle     short loc_31CAE
    mov     [di], ax
loc_31CAE:
    jmp     loc_31EB9
loc_31CB1:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short loc_31CC6
    add     di, 2
loc_31CC6:
    cmp     [di], ax
    jle     short loc_31CCC
    mov     [di], ax
loc_31CCC:
    add     dx, bx
    jnb     short loc_31CE6
    cmp     [di+3C0h], ax
    jge     short loc_31CDA
    mov     [di+3C0h], ax
loc_31CDA:
    add     di, 2
    inc     ax
    loop    loc_31CC6
    sub     di, 2
    jmp     loc_31EB9
loc_31CE6:
    inc     ax
    loop    loc_31CCC
    dec     ax
    cmp     [di+3C0h], ax
    jge     short loc_31CAE
    mov     [di+3C0h], ax
    jmp     loc_31EB9
off_31CF7     dw offset locret_31AA3
    dw offset locret_31AA3
    dw offset loc_31D0B
    dw offset loc_31D31
    dw offset loc_31D5C
    dw offset loc_31D87
    dw offset loc_31DCA
    dw offset loc_31E0D
    dw offset loc_31E61
    dw offset locret_31AA3
loc_31D0B:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
loc_31D11:
    cmp     [di], ax
    jg      short loc_31D23
    cmp     [di+3C0h], ax
    jl      short loc_31D28
    add     di, 2
    loop    loc_31D11
    jmp     loc_31EB9
loc_31D23:
    rep stosw
    jmp     loc_31EB9
loc_31D28:
    add     di, 3C0h
    rep stosw
    jmp     loc_31EB9
loc_31D31:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
loc_31D37:
    cmp     [di], ax
    jg      short loc_31D4A
    cmp     [di+3C0h], ax
    jl      short loc_31D51
    add     di, 2
    dec     ax
    loop    loc_31D37
    jmp     loc_31EB9
loc_31D4A:
    stosw
    dec     ax
    loop    loc_31D4A
    jmp     loc_31EB9
loc_31D51:
    add     di, 3C0h
loc_31D55:
    stosw
    dec     ax
    loop    loc_31D55
    jmp     loc_31EB9
loc_31D5C:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
loc_31D62:
    cmp     [di], ax
    jg      short loc_31D75
    cmp     [di+3C0h], ax
    jl      short loc_31D7C
    add     di, 2
    inc     ax
    loop    loc_31D62
    jmp     loc_31EB9
loc_31D75:
    stosw
    inc     ax
    loop    loc_31D75
    jmp     loc_31EB9
loc_31D7C:
    add     di, 3C0h
loc_31D80:
    stosw
    inc     ax
    loop    loc_31D80
    jmp     loc_31EB9
loc_31D87:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
loc_31D99:
    cmp     [di], ax
    jg      short loc_31DB0
    cmp     [di+3C0h], ax
    jl      short loc_31DBB
    add     di, 2
    sub     dx, bx
    sbb     ax, 0
    loop    loc_31D99
    jmp     loc_31EB9
loc_31DB0:
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    loc_31DB0
    jmp     loc_31EB9
loc_31DBB:
    add     di, 3C0h
loc_31DBF:
    stosw
    sub     dx, bx
    sbb     ax, 0
    loop    loc_31DBF
    jmp     loc_31EB9
loc_31DCA:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     dx, [si]
    add     dx, 8000h
    adc     ax, 0
    mov     bx, [si+0Ch]
loc_31DDC:
    cmp     [di], ax
    jg      short loc_31DF3
    cmp     [di+3C0h], ax
    jl      short loc_31DFE
    add     di, 2
    add     dx, bx
    adc     ax, 0
    loop    loc_31DDC
    jmp     loc_31EB9
loc_31DF3:
    stosw
    add     dx, bx
    adc     ax, 0
    loop    loc_31DF3
    jmp     loc_31EB9
loc_31DFE:
    add     di, 3C0h
loc_31E02:
    stosw
    add     dx, bx
    adc     ax, 0
    loop    loc_31E02
    jmp     loc_31EB9
loc_31E0D:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short loc_31E22
    add     di, 2
loc_31E22:
    cmp     [di+3C0h], ax
    jl      short loc_31E39
    add     dx, bx
    jnb     short loc_31E33
    cmp     [di], ax
    jg      short loc_31E51
    add     di, 2
loc_31E33:
    dec     ax
    loop    loc_31E22
    jmp     loc_31EB9
loc_31E39:
    add     di, 3C0h
loc_31E3D:
    stosw
loc_31E3E:
    dec     ax
    add     dx, bx
    jnb     short loc_31E48
    loop    loc_31E3D
    jmp     short loc_31EB9
    db 144
loc_31E48:
    loop    loc_31E3E
    jmp     short loc_31EB9
    db 144
loc_31E4D:
    add     dx, bx
    jnb     short loc_31E58
loc_31E51:
    stosw
    dec     ax
    loop    loc_31E4D
    jmp     short loc_31EB9
    db 144
loc_31E58:
    dec     ax
    loop    loc_31E4D
    inc     ax
    mov     [di], ax
    jmp     short loc_31EB9
    db 144
loc_31E61:
    mov     cx, [si+0Eh]
    mov     ax, [si+2]
    mov     bx, [si+0Ch]
    mov     dx, [si+4]
    add     dx, 8000h
    jnb     short loc_31E76
    add     di, 2
loc_31E76:
    cmp     [di], ax
    jg      short loc_31EB2
    add     dx, bx
    jnb     short loc_31E87
    cmp     [di+3C0h], ax
    jl      short loc_31E8D
    add     di, 2
loc_31E87:
    inc     ax
    loop    loc_31E76
    jmp     short loc_31EB9
    db 144
loc_31E8D:
    add     di, 3C0h
loc_31E91:
    mov     [di], ax
loc_31E93:
    inc     ax
    add     dx, bx
    jnb     short loc_31EA3
    add     di, 2
    loop    loc_31E91
    dec     ax
    mov     [di], ax
    jmp     short loc_31EB9
    db 144
loc_31EA3:
    loop    loc_31E93
    add     di, 2
    dec     ax
    mov     [di], ax
    jmp     short loc_31EB9
    db 144
loc_31EAE:
    add     dx, bx
    jnb     short loc_31EB3
loc_31EB2:
    stosw
loc_31EB3:
    inc     ax
    loop    loc_31EAE
    jmp     short loc_31EB9
    db 144
loc_31EB9:
    cmp     [bp+var_C], 0
    jnz     short loc_31EC0
    retn
loc_31EC0:
    mov     cx, [si+14h]
    or      cx, cx
    jle     short loc_31EE1
    mov     di, [si+6]
    mov     dx, [si+4]
    add     dx, 8000h
    adc     di, 0
    sub     di, cx
    shl     di, 1
    add     di, [bp+var_18]
    mov     ax, word ptr cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    rep stosw
loc_31EE1:
    mov     cx, [si+18h]
    or      cx, cx
    jle     short loc_31F07
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
loc_31F07:
    mov     cx, [si+16h]
    or      cx, cx
    jle     short loc_31F1D
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+var_18]
    mov     ax, word ptr cs:[_sprite1+SPRITE_LEFT2_OFFSET]
    rep stosw
loc_31F1D:
    mov     cx, [si+1Ah]
    or      cx, cx
    jle     short locret_31F38
    mov     di, [si+0Ah]
    inc     di
    shl     di, 1
    add     di, [bp+var_18]
    add     di, 3C0h
    mov     ax, word ptr cs:[_sprite1+SPRITE_WIDTHSUM_OFFSET]
    dec     ax
    rep stosw
locret_31F38:
    retn
preRender_helper3 endp
_nopsub_31F39 label far
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
    jmp     short loc_31F6E
    db 144
nopsub_31F39 endp
_nopsub_31F55 label near
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
loc_31F6E:
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
    jle     short loc_31FC8
    xchg    bx, dx
loc_31FC8:
    mov     [bp+var_10], si
    mov     [bp+var_14], si
    add     si, 2
    dec     cx
    jnz     short loc_31FE8
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
loc_31FE8:
    mov     di, [si]
    mov     ax, [di+6]
    sub     ax, [di+14h]
    sub     ax, [di+18h]
    cmp     ax, [bp+var_E]
    jg      short loc_32010
    jnz     short loc_32002
    test    [bp+var_7D0], 1
    jz      short loc_32015
loc_32002:
    mov     [bp+var_E], ax
    mov     [bp+var_10], si
    or      [bp+var_7D0], 1
    jmp     short loc_32015
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
loc_32010:
    and     [bp+var_7D0], 2
loc_32015:
    mov     ax, [di+0Ah]
    add     ax, [di+16h]
    add     ax, [di+1Ah]
    cmp     ax, [bp+var_12]
    jl      short loc_3203B
    jnz     short loc_3202D
loc_32025:
    test    [bp+var_7D0], 2
    jnz     short loc_32040
loc_3202D:
    mov     [bp+var_12], ax
    mov     [bp+var_14], si
    or      [bp+var_7D0], 2
    jmp     short loc_32040
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
loc_3203B:
    and     [bp+var_7D0], 1
loc_32040:
    mov     ax, [di+2]
    cmp     ax, bx
    jge     short loc_32049
    mov     bx, ax
loc_32049:
    cmp     ax, dx
    jle     short loc_3204F
    mov     dx, ax
loc_3204F:
    mov     ax, [di+8]
    cmp     ax, bx
    jge     short loc_32058
    mov     bx, ax
loc_32058:
    cmp     ax, dx
    jle     short loc_3205E
    mov     dx, ax
loc_3205E:
    add     si, 2
    loop    loc_31FE8
    cmp     dx, [bp+var_2]
    jl      short loc_320B6
    cmp     bx, [bp+var_4]
    jge     short loc_320B6
    mov     ax, [bp+var_12]
    cmp     ax, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jl      short loc_320B6
    mov     cx, [bp+var_E]
    cmp     cx, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short loc_320B6
    mov     [bp+var_C], 0
    cmp     dx, [bp+var_4]
    jg      short loc_3209D
    cmp     bx, [bp+var_2]
    jl      short loc_3209D
    cmp     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jge     short loc_3209D
    cmp     cx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short loc_320A1
loc_3209D:
    mov     [bp+var_C], 1
loc_320A1:
    cmp     ax, cx
    jz      short loc_320A9
    cmp     dx, bx
    jnz     short loc_320BC
loc_320A9:
    mov     si, [bp+var_6]
    mov     di, [si]
    push    di
    call    dword ptr imagefunc
    add     sp, 2
loc_320B6:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_320BC:
    mov     di, [bp+var_10]
loc_320BF:
    mov     si, [di]
    cmp     byte ptr [si+12h], 2
    jl      short loc_320DC
    mov     [bp+var_16], di
    cmp     [bp+var_C], 0
    jz      short loc_320D6
    call near ptr preRender_helper
    jmp     short loc_320D9
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
loc_320D6:
    call near ptr preRender_helper2
loc_320D9:
    mov     di, [bp+var_16]
loc_320DC:
    cmp     di, [bp+var_14]
    jz      short loc_320EE
    add     di, 2
    cmp     di, [bp+var_8]
    jbe     short loc_320BF
    mov     di, [bp+var_6]
    jmp     short loc_320BF
loc_320EE:
    mov     di, [bp+var_10]
loc_320F1:
    sub     di, 2
    cmp     di, [bp+var_6]
    jnb     short loc_320FC
    mov     di, [bp+var_8]
loc_320FC:
    cmp     di, [bp+var_14]
    jz      short loc_32114
    mov     si, [di]
    cmp     byte ptr [si+12h], 2
    jl      short loc_320F1
    mov     [bp+var_16], di
    call near ptr preRender_helper3
    mov     di, [bp+var_16]
    jmp     short loc_320F1
loc_32114:
    mov     ax, [bp+var_12]
    cmp     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    jl      short loc_32123
    mov     ax, word ptr cs:[_sprite1+SPRITE_HEIGHT_OFFSET]
    dec     ax
loc_32123:
    mov     bx, [bp+var_E]
    cmp     bx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
    jge     short loc_32132
    mov     bx, word ptr cs:[_sprite1+SPRITE_TOP_OFFSET]
loc_32132:
    sub     ax, bx
    jle     short loc_32153
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
loc_32153:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
nopsub_31F55 endp
S012_TEXT ends
end
