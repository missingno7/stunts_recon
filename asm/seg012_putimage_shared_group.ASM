.8086
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_33B98
public _sprite_putimage_and_alt
public _sprite_putimage
_nopsub_33B98 proc far
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
    sub     ax, [si+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    sub     ax, [si+6]
    mov     [bp+var_4], ax
    jmp     short loc_33BF5
    ; align 2
    db 144
_nopsub_33B98 endp
_sprite_putimage_and_alt proc far
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
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short loc_33BF5
    ; align 2
    db 144
_sprite_putimage_and_alt endp
_sprite_putimage proc far
    var_E = word ptr -14
    var_bitmap = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    shapey = word ptr -4
    shapex = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+8]
    mov     [bp+shapex], ax
    mov     ax, [si+0Ah]
    mov     [bp+shapey], ax
loc_33BF5:
    mov     bx, [bp+shapey]
    mov     cx, [si+2]
    lea     dx, [si+10h]
    mov     ax, bx
    cmp     ax, word ptr cs:[_sprite1+16]
    jl      short loc_33C1B
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+18]
    jle     short loc_33C42
    sub     cx, ax
    jg      short loc_33C42
loc_33C14:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_33C1B:
    mov     bx, word ptr cs:[_sprite1+16]
    add     ax, cx
    sub     ax, bx
    jle     short loc_33C14
    xchg    ax, cx
    sub     ax, cx
    mov     [bp+var_8], dx
    mul     word ptr [si+0]
    mov     dx, [bp+var_8]
    add     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+18]
    jle     short loc_33C42
    sub     cx, ax
    jle     short loc_33C14
loc_33C42:
    mov     [bp+var_bitmap], dx
    mov     [bp+var_A], cx
    mov     [bp+shapey], bx
    mov     cx, [si+0]
    xor     dx, dx
    mov     bx, [bp+shapex]
    mov     ax, bx
    cmp     ax, word ptr cs:[_sprite1+12]
    jl      short loc_33C6D
    add     ax, cx
    sub     ax, word ptr cs:[_sprite1+14]
    jl      short loc_33C94
    sub     cx, ax
    jle     short loc_33C14
    mov     dx, ax
    jmp     short loc_33C94
    db 144
loc_33C6D:
    mov     bx, word ptr cs:[_sprite1+12]
    add     ax, cx
    sub     ax, bx
    jle     short loc_33C14
    mov     si, cx
    sub     si, ax
    add     [bp+var_bitmap], si
    mov     si, word ptr cs:[_sprite1+14]
    sub     si, bx
    jle     short loc_33C14
    cmp     ax, si
    jl      short loc_33C8E
    mov     ax, si
loc_33C8E:
    xchg    cx, dx
    sub     dx, ax
    add     cx, ax
loc_33C94:
    or      cx, cx
    jle     short loc_33CD8
    mov     [bp+var_6], cx
    mov     [bp+var_8], dx
    mov     [bp+shapex], bx
    cld
    mov     es, word ptr word ptr cs:[_sprite1+2]
    mov     bx, [bp+shapey]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+shapex]
    mov     si, [bp+var_bitmap]
    mov     dx, [bp+var_A]
    mov     bx, word ptr cs:[_sprite1+20]
    sub     bx, [bp+var_6]
    cmp     [bp+var_8], 0
    jz      short loc_33CDB
loc_33CCB:
    mov     cx, [bp+var_6]
    rep movsb
    add     si, [bp+var_8]
    add     di, bx
    dec     dx
    jg      short loc_33CCB
loc_33CD8:
    jmp     loc_33C14
loc_33CDB:
    mov     ax, [bp+var_6]
    shr     ax, 1
    mov     [bp+var_E], ax
    jnb     short loc_33CF5
    jz      short loc_33D02
loc_33CE7:
    mov     cx, [bp+var_E]
    rep movsw
    movsb
    add     di, bx
    dec     dx
    jg      short loc_33CE7
    jmp     loc_33C14
loc_33CF5:
    mov     cx, [bp+var_E]
    rep movsw
    add     di, bx
    dec     dx
    jg      short loc_33CF5
    jmp     loc_33C14
loc_33D02:
    movsb
    add     di, bx
    inc     bx
    dec     dx
    jg      short loc_33D02
    jmp     loc_33C14
_sprite_putimage endp
S012_TEXT ends
end
