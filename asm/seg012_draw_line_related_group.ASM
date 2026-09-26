; Draw-line entry pair: the alternate entry selects clipped-output mode by
; changing the shared frame byte before joining the common body.
.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
extrn sprite1:byte
; Semantic declarations: an unsigned far word count and a far array of 50 word offsets.
; Their symbols remain external because the repository has no grounded aliases for them.
extrn _word_2F448:word
extrn _off_2F44A:word
public _draw_line_related_alt
public _draw_line_related

; sprite1 is the independently grounded 30-byte CS object.
SPRITE_TOP equ 10h
SPRITE_HEIGHT equ 12h
SPRITE_LEFT2 equ 1Ah
SPRITE_WIDTHSUM equ 1Ch

; Near output pointer addresses this complete 28-byte record.
LINE_X_FRACTION equ 0
LINE_START_X equ 02h
LINE_Y_FRACTION equ 04h
LINE_START_Y equ 06h
LINE_END_X equ 08h
LINE_END_Y equ 0Ah
LINE_SLOPE equ 0Ch
LINE_LENGTH equ 0Eh
LINE_RESERVED equ 10h
LINE_DIRECTION equ 12h
LINE_STATUS equ 13h
LINE_LEFT_CLIP equ 14h
LINE_RIGHT_CLIP equ 16h
LINE_TOP_CLIP equ 18h
LINE_BOTTOM_CLIP equ 1Ah
LINE_RESULT_SIZE equ 1Ch

_draw_line_related_alt proc far
    clip_mode = byte ptr -4
     s = byte ptr 0
     r = byte ptr 2
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     [bp+clip_mode], 1
    jmp     short shared_line_body
_draw_line_related_alt endp
_draw_line_related proc far
    clip_mode = byte ptr -4
    region_mask = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_startX = word ptr 6
    arg_startY = word ptr 8
    arg_endX = word ptr 10
    arg_endY = word ptr 12
    arg_result = word ptr 14
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     [bp+clip_mode], 0
shared_line_body:
    mov     si, [bp+arg_result]
    mov     word ptr [si+LINE_DIRECTION], 0FFh
    xor     ax, ax
    mov     [si+LINE_X_FRACTION], ax
    mov     [si+LINE_Y_FRACTION], ax
    mov     [si+LINE_LEFT_CLIP], ax
    mov     [si+LINE_RIGHT_CLIP], ax
    mov     [si+LINE_TOP_CLIP], ax
    mov     [si+LINE_BOTTOM_CLIP], ax
    mov     ax, [bp+arg_startY]
    mov     bx, [bp+arg_endY]
    mov     cx, [bp+arg_startX]
    mov     dx, [bp+arg_endX]
    cmp     ax, bx
    jg      short line_eb9c
    mov     [si+LINE_START_X], cx
    mov     [si+LINE_START_Y], ax
    mov     [si+LINE_END_X], dx
line_eb96:
    mov     [si+LINE_END_Y], bx
    jmp     short line_eba8
    nop
line_eb9c:
    mov     [si+LINE_START_X], dx
    mov     [si+LINE_START_Y], bx
    mov     [si+LINE_END_X], cx
    mov     [si+LINE_END_Y], ax
line_eba8:
    jnz     short line_ebad
    jmp     handle_horizontal_line
line_ebad:
    xor     dx, dx
    cmp     [bp+clip_mode], 0
    jnz     short line_ec1a
    mov     ax, [si+LINE_START_Y]
    mov     bx, word ptr cs:[sprite1+SPRITE_TOP]
    mov     cx, word ptr cs:[sprite1+SPRITE_HEIGHT]
    cmp     ax, cx
    jl      short line_ebcb
    mov     dl, 8
    jmp     line_ef9b
line_ebcb:
    cmp     ax, bx
    jge     short line_ebd2
    or      dh, 4
line_ebd2:
    mov     ax, [si+LINE_END_Y]
    cmp     ax, bx
    jge     short line_ebde
    mov     dl, 4
    jmp     line_ef9b
line_ebde:
    cmp     ax, cx
    jl      short line_ebe5
    or      dl, 8
line_ebe5:
    mov     bx, word ptr cs:[sprite1+SPRITE_LEFT2]
    mov     cx, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    mov     ax, [si+LINE_START_X]
    cmp     ax, bx
    jge     short line_ebf9
    or      dh, 2
line_ebf9:
    cmp     ax, cx
    jl      short line_ec00
    or      dh, 1
line_ec00:
    mov     ax, [si+LINE_END_X]
    cmp     ax, bx
    jge     short line_ec0a
    or      dl, 2
line_ec0a:
    cmp     ax, cx
    jl      short line_ec11
    or      dl, 1
line_ec11:
    test    dl, dh
    jz      short line_ec1a
    and     dl, dh
    jmp     line_ef9b
line_ec1a:
    or      dl, dh
    xor     dh, dh
    mov     [bp+region_mask], dx
    mov     cx, [si+LINE_END_Y]
    sub     cx, [si+LINE_START_Y]
    jno     short line_ec2c
line_ec29:
    jmp     recover_overflowed_coordinates
line_ec2c:
    mov     dx, [si+LINE_END_X]
    sub     dx, [si+LINE_START_X]
    jo      short line_ec29
    jnz     short line_ec41
    inc     cx
    mov     [si+LINE_LENGTH], cx
    mov     byte ptr [si+LINE_DIRECTION], 2
    jmp     short dispatch_region_case
    nop
line_ec41:
    jl      short line_ec8f
    cmp     dx, cx
    jb      short line_ec82
    jz      short line_ec88
    mov     byte ptr [si+LINE_DIRECTION], 8
line_ec4d:
    xchg    cx, dx
line_ec4f:
    cmp     cx, word ptr cs:[_word_2F448]
    jge     short line_ec69
    mov     bx, cx
    shl     bx, 1
    mov     bx, word ptr cs:[_off_2F44A+bx]
    add     bx, dx
    add     bx, dx
    mov     ax, cs:[bx]
    jmp     short line_ec76
    nop
line_ec69:
    xor     ax, ax
    div     cx
    mov     bx, cx
    shr     bx, 1
    sub     bx, dx
    adc     ax, 0
line_ec76:
    mov     [si+LINE_SLOPE], ax
    inc     cx
    jo      short line_ec29
    mov     [si+LINE_LENGTH], cx
    jmp     short dispatch_region_case
    nop
line_ec82:
    mov     byte ptr [si+LINE_DIRECTION], 6
    jmp     short line_ec4f
line_ec88:
    mov     byte ptr [si+LINE_DIRECTION], 4
    jmp     short line_eca9
    nop
line_ec8f:
    neg     dx
    jo      short line_ec29
    cmp     dx, cx
    jb      short line_ec9f
    jz      short line_eca5
    mov     byte ptr [si+LINE_DIRECTION], 7
    jmp     short line_ec4d
line_ec9f:
    mov     byte ptr [si+LINE_DIRECTION], 5
    jmp     short line_ec4f
line_eca5:
    mov     byte ptr [si+LINE_DIRECTION], 3
line_eca9:
    inc     cx
    mov     [si+LINE_LENGTH], cx
dispatch_region_case:
    mov     bx, [bp+region_mask]
    shl     bx, 1
    jz      short return_no_intersection
    jmp     cs:region_dispatch_table[bx]
region_dispatch_table     dw offset return_no_intersection
    dw offset dispatch_right_edge_case
    dw offset clip_against_horizontal_edge
    dw offset clip_against_horizontal_edge
    dw offset clip_against_top
    dw offset clip_against_top
    dw offset clip_against_top
    dw offset clip_against_top
    dw offset clip_against_bottom
    dw offset clip_against_bottom
    dw offset clip_against_bottom
    dw offset clip_against_bottom
    dw offset clip_against_top
    dw offset clip_against_top
    dw offset clip_against_top
    dw offset clip_against_top
return_no_intersection:
    xor     ax, ax
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
clip_against_top:
    mov     ax, [si+LINE_START_Y]
    mov     cx, word ptr cs:[sprite1+SPRITE_TOP]
    mov     [si+LINE_START_Y], cx
    sub     cx, ax
    mov     bl, [si+LINE_DIRECTION]
    shl     bx, 1
    jmp     cs:top_status_dispatch_table[bx]
top_status_dispatch_table     dw 0
    dw 0
    dw offset line_ed0a
    dw offset line_ed10
    dw offset line_ed19
    dw offset line_ed22
    dw offset line_ed32
    dw offset line_ed42
    dw offset line_ed7e
line_ed0a:
    sub     [si+LINE_LENGTH], cx
    jmp     line_f13e
line_ed10:
    sub     [si+LINE_START_X], cx
    sub     [si+LINE_LENGTH], cx
    jmp     line_f13e
line_ed19:
    add     [si+LINE_START_X], cx
    sub     [si+LINE_LENGTH], cx
    jmp     line_f13e
line_ed22:
    mov     ax, [si+LINE_SLOPE]
    mul     cx
    sub     [si+LINE_X_FRACTION], ax
    sbb     [si+LINE_START_X], dx
    sub     [si+LINE_LENGTH], cx
    jmp     line_f13e
line_ed32:
    mov     ax, [si+LINE_SLOPE]
    mul     cx
    add     [si+LINE_X_FRACTION], ax
    adc     [si+LINE_START_X], dx
    sub     [si+LINE_LENGTH], cx
    jmp     line_f13e
line_ed42:
    mov     [si+LINE_START_Y], ax
    mov     dx, cx
    xor     ax, ax
    mov     cx, [si+LINE_SLOPE]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    sub     [si+LINE_START_X], ax
    sub     [si+LINE_LENGTH], ax
    jle     short line_ed69
    mul     word ptr [si+LINE_SLOPE]
    add     [si+LINE_Y_FRACTION], ax
    adc     [si+LINE_START_Y], dx
    jmp     line_f13e
line_ed69:
    mov     word ptr [si+LINE_LENGTH], 1
    mov     ax, word ptr cs:[sprite1+SPRITE_TOP]
    mov     [si+LINE_START_Y], ax
    mov     ax, [si+LINE_END_X]
    mov     [si+LINE_START_X], ax
    jmp     line_f13e
line_ed7e:
    mov     [si+LINE_START_Y], ax
    mov     dx, cx
    xor     ax, ax
    mov     cx, [si+LINE_SLOPE]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    add     [si+LINE_START_X], ax
    sub     [si+LINE_LENGTH], ax
    jle     short line_ed69
    mul     word ptr [si+LINE_SLOPE]
    add     [si+LINE_Y_FRACTION], ax
    adc     [si+LINE_START_Y], dx
    jmp     line_f13e
clip_against_bottom:
    mov     cx, [si+LINE_END_Y]
    mov     dx, word ptr cs:[sprite1+SPRITE_HEIGHT]
    dec     dx
    mov     [si+LINE_END_Y], dx
    sub     cx, dx
    mov     bl, [si+LINE_DIRECTION]
    shl     bx, 1
    jmp     cs:bottom_status_dispatch_table[bx]
bottom_status_dispatch_table     dw 0
    dw 0
    dw offset line_edcf
    dw offset line_edd5
    dw offset line_edde
    dw offset line_ede7
    dw offset line_ee0b
    dw offset line_ee2a
    dw offset line_ee53
line_edcf:
    sub     [si+LINE_LENGTH], cx
    jmp     line_f148
line_edd5:
    add     [si+LINE_END_X], cx
    sub     [si+LINE_LENGTH], cx
    jmp     line_f148
line_edde:
    sub     [si+LINE_END_X], cx
    sub     [si+LINE_LENGTH], cx
    jmp     line_f148
line_ede7:
    mov     dx, [si+LINE_LENGTH]
    sub     dx, cx
    mov     [si+LINE_LENGTH], dx
    dec     dx
    mov     ax, [si+LINE_SLOPE]
    mul     dx
    mov     bx, [si+LINE_X_FRACTION]
    mov     cx, [si+LINE_START_X]
    sub     bx, ax
    sbb     cx, dx
    add     bx, 8000h
    adc     cx, 0
    mov     [si+LINE_END_X], cx
    jmp     line_f148
line_ee0b:
    mov     dx, [si+LINE_LENGTH]
    sub     dx, cx
    mov     [si+LINE_LENGTH], dx
    dec     dx
    mov     ax, [si+LINE_SLOPE]
    mul     dx
    add     ax, [si+LINE_X_FRACTION]
    adc     dx, [si+LINE_START_X]
    add     ax, 8000h
    adc     dx, 0
    mov     [si+LINE_END_X], dx
    jmp     line_f148
line_ee2a:
    xor     ax, ax
    sub     ax, [si+LINE_Y_FRACTION]
    sbb     dx, [si+LINE_START_Y]
    jl      short line_ee4f
    mov     cx, [si+LINE_SLOPE]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
line_ee40:
    mov     dx, [si+LINE_START_X]
    sub     dx, ax
    mov     [si+LINE_END_X], dx
    inc     ax
    mov     [si+LINE_LENGTH], ax
    jmp     line_f148
line_ee4f:
    xor     ax, ax
    jmp     short line_ee40
line_ee53:
    xor     ax, ax
    sub     ax, [si+LINE_Y_FRACTION]
    sbb     dx, [si+LINE_START_Y]
    jl      short line_ee4f
    mov     cx, [si+LINE_SLOPE]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    mov     dx, [si+LINE_START_X]
    add     dx, ax
    mov     [si+LINE_END_X], dx
    inc     ax
    mov     [si+LINE_LENGTH], ax
    jmp     line_f148
clip_against_horizontal_edge:
    mov     bl, [si+LINE_DIRECTION]
    shl     bx, 1
    jmp     cs:left_status_dispatch_table[bx]
left_status_dispatch_table     dw 0
    dw 0
    dw offset return_clipped
    dw offset line_ee94
    dw offset line_eead
    dw offset line_eec6
    dw offset line_eefe
    dw offset line_ef31
    dw offset line_ef61
line_ee94:
    mov     cx, word ptr cs:[sprite1+SPRITE_LEFT2]
    mov     ax, [si+LINE_END_X]
    mov     [si+LINE_END_X], cx
    sub     cx, ax
    add     [si+LINE_RIGHT_CLIP], cx
    sub     [si+LINE_LENGTH], cx
    sub     [si+LINE_END_Y], cx
    jmp     line_f196
line_eead:
    mov     ax, [si+LINE_START_X]
    mov     cx, word ptr cs:[sprite1+SPRITE_LEFT2]
    mov     [si+LINE_START_X], cx
    sub     cx, ax
    add     [si+LINE_LEFT_CLIP], cx
    add     [si+LINE_START_Y], cx
    sub     [si+LINE_LENGTH], cx
    jmp     line_f196
line_eec6:
    mov     ax, [si+LINE_X_FRACTION]
    mov     dx, [si+LINE_START_X]
    mov     cx, word ptr cs:[sprite1+SPRITE_LEFT2]
    mov     [si+LINE_END_X], cx
    sub     dx, cx
    jl      short line_eef9
    mov     cx, [si+LINE_SLOPE]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    inc     ax
line_eee4:
    mov     cx, [si+LINE_LENGTH]
    mov     [si+LINE_LENGTH], ax
    sub     cx, ax
    add     [si+LINE_RIGHT_CLIP], cx
    dec     ax
    add     ax, [si+LINE_START_Y]
    mov     [si+LINE_END_Y], ax
    jmp     line_f196
line_eef9:
    mov     ax, 1
    jmp     short line_eee4
line_eefe:
    mov     dx, word ptr cs:[sprite1+SPRITE_LEFT2]
    xor     ax, ax
    sub     ax, [si+LINE_X_FRACTION]
    sbb     dx, [si+LINE_START_X]
    jl      short line_ef2e
    mov     cx, [si+LINE_SLOPE]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    add     [si+LINE_START_Y], ax
    add     [si+LINE_LEFT_CLIP], ax
    sub     [si+LINE_LENGTH], ax
    jle     short return_clipped
    mul     word ptr [si+LINE_SLOPE]
    add     [si+LINE_X_FRACTION], ax
    adc     [si+LINE_START_X], dx
    jmp     line_f196
line_ef2e:
    jmp     short return_clipped
    nop
line_ef31:
    mov     ax, [si+LINE_START_X]
    mov     dx, word ptr cs:[sprite1+SPRITE_LEFT2]
    mov     [si+LINE_END_X], dx
    sub     ax, dx
    mov     cx, ax
    inc     cx
    mov     [si+LINE_LENGTH], cx
    mul     word ptr [si+LINE_SLOPE]
    add     ax, [si+LINE_Y_FRACTION]
    adc     dx, [si+LINE_START_Y]
    add     ax, 8000h
    adc     dx, 0
    mov     ax, [si+LINE_END_Y]
    sub     ax, dx
    mov     [si+LINE_END_Y], dx
    add     [si+LINE_RIGHT_CLIP], ax
    jmp     line_f196
line_ef61:
    mov     cx, [si+LINE_START_X]
    mov     ax, word ptr cs:[sprite1+SPRITE_LEFT2]
    mov     [si+LINE_START_X], ax
    sub     ax, cx
    sub     [si+LINE_LENGTH], ax
    mul     word ptr [si+LINE_SLOPE]
    mov     bx, [si+LINE_Y_FRACTION]
    mov     cx, [si+LINE_START_Y]
    add     ax, bx
    adc     dx, cx
    mov     [si+LINE_Y_FRACTION], ax
    mov     [si+LINE_START_Y], dx
    add     bx, 8000h
    adc     cx, 0
    add     ax, 8000h
    adc     dx, 0
    sub     dx, cx
    add     [si+LINE_LEFT_CLIP], dx
    jmp     line_f196
return_clipped:
    mov     dx, 2
line_ef9b:
    mov     [si+LINE_STATUS], dl
    mov     word ptr [si+LINE_LENGTH], 0
    mov     al, [si+LINE_STATUS]
    test    al, 4
    jz      short line_efbe
    mov     bx, word ptr cs:[sprite1+SPRITE_TOP]
    mov     [si+LINE_START_Y], bx
    mov     word ptr [si+LINE_Y_FRACTION], 0
    dec     bx
    mov     [si+LINE_END_Y], bx
    jmp     short line_f017
    nop
line_efbe:
    test    al, 8
    jz      short line_efd2
    mov     bx, word ptr cs:[sprite1+SPRITE_HEIGHT]
    mov     [si+LINE_START_Y], bx
    mov     word ptr [si+LINE_Y_FRACTION], 0
    jmp     short line_f017
    nop
line_efd2:
    mov     cx, [si+LINE_END_Y]
    cmp     cx, word ptr cs:[sprite1+SPRITE_HEIGHT]
    jl      short line_efe2
    mov     cx, word ptr cs:[sprite1+SPRITE_HEIGHT]
    dec     cx
line_efe2:
    mov     dx, [si+LINE_START_Y]
    mov     bx, [si+LINE_Y_FRACTION]
    add     bx, 8000h
    adc     dx, 0
    cmp     dx, word ptr cs:[sprite1+SPRITE_TOP]
    jge     short line_effb
    mov     dx, word ptr cs:[sprite1+SPRITE_TOP]
line_effb:
    mov     [si+LINE_START_Y], dx
    mov     word ptr [si+LINE_Y_FRACTION], 0
    sub     cx, dx
    dec     dx
    inc     cx
    mov     [si+LINE_END_Y], dx
    test    al, 2
    jz      short line_f014
    add     [si+LINE_RIGHT_CLIP], cx
    jmp     short line_f017
    nop
line_f014:
    add     [si+LINE_BOTTOM_CLIP], cx
line_f017:
    xor     ah, ah
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
dispatch_right_edge_case:
    mov     bl, [si+LINE_DIRECTION]
line_f022:
    xor     bh, bh
    shl     bx, 1
    jmp     cs:right_status_dispatch_table[bx]
right_status_dispatch_table     dw 0
    dw 0
    dw offset line_f03d
    dw offset line_f043
    dw offset line_f05c
    dw offset line_f076
    dw offset line_f0a3
    dw offset line_f0d7
    dw offset line_f110
line_f03d:
    mov     dx, 1
    jmp     line_ef9b
line_f043:
    mov     cx, [si+LINE_START_X]
    mov     ax, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    dec     ax
    mov     [si+LINE_START_X], ax
    sub     cx, ax
    add     [si+LINE_START_Y], cx
    sub     [si+LINE_LENGTH], cx
    add     [si+LINE_TOP_CLIP], cx
    jmp     return_no_intersection
line_f05c:
    mov     cx, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    dec     cx
    mov     ax, [si+LINE_END_X]
    mov     [si+LINE_END_X], cx
    sub     ax, cx
    add     [si+LINE_BOTTOM_CLIP], ax
    sub     [si+LINE_LENGTH], ax
    sub     [si+LINE_END_Y], ax
    jmp     return_no_intersection
line_f076:
    mov     ax, [si+LINE_X_FRACTION]
    mov     dx, [si+LINE_START_X]
    sub     dx, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    inc     dx
    mov     cx, [si+LINE_SLOPE]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    sub     [si+LINE_LENGTH], ax
    jle     short line_f03d
    add     [si+LINE_START_Y], ax
    add     [si+LINE_TOP_CLIP], ax
    mul     word ptr [si+LINE_SLOPE]
    sub     [si+LINE_X_FRACTION], ax
    sbb     [si+LINE_START_X], dx
    jmp     return_no_intersection
line_f0a3:
    mov     dx, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    dec     dx
    mov     [si+LINE_END_X], dx
    xor     ax, ax
    sub     ax, [si+LINE_X_FRACTION]
    sbb     dx, [si+LINE_START_X]
    jl      short line_f03d
    mov     cx, [si+LINE_SLOPE]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    inc     ax
    mov     cx, [si+LINE_LENGTH]
    mov     [si+LINE_LENGTH], ax
    sub     cx, ax
    add     [si+LINE_BOTTOM_CLIP], cx
    dec     ax
    add     ax, [si+LINE_START_Y]
    mov     [si+LINE_END_Y], ax
    jmp     return_no_intersection
line_f0d7:
    mov     ax, [si+LINE_START_X]
    mov     cx, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    dec     cx
    sub     ax, cx
    mov     [si+LINE_START_X], cx
    sub     [si+LINE_LENGTH], ax
    mul     word ptr [si+LINE_SLOPE]
    mov     bx, [si+LINE_Y_FRACTION]
    mov     cx, [si+LINE_START_Y]
    add     ax, bx
    adc     dx, cx
    mov     [si+LINE_Y_FRACTION], ax
    mov     [si+LINE_START_Y], dx
    add     bx, 8000h
    adc     cx, 0
    add     ax, 8000h
    adc     dx, 0
    sub     dx, cx
    add     [si+LINE_TOP_CLIP], dx
    jmp     return_no_intersection
line_f110:
    mov     ax, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    mov     cx, ax
    dec     cx
    mov     [si+LINE_END_X], cx
    sub     ax, [si+LINE_START_X]
    mov     [si+LINE_LENGTH], ax
    dec     ax
    mul     word ptr [si+LINE_SLOPE]
    add     ax, [si+LINE_Y_FRACTION]
    adc     dx, [si+LINE_START_Y]
    add     ax, 8000h
    adc     dx, 0
    mov     ax, [si+LINE_END_Y]
    sub     ax, dx
    mov     [si+LINE_END_Y], dx
    add     [si+LINE_BOTTOM_CLIP], ax
    jmp     return_no_intersection
line_f13e:
    test    [bp+region_mask], 8
    jz      short line_f148
    jmp     clip_against_bottom
line_f148:
    xor     dx, dx
    mov     ax, [si+LINE_START_X]
    cmp     ax, word ptr cs:[sprite1+SPRITE_LEFT2]
    jge     short line_f157
    or      dh, 2
line_f157:
    mov     bx, [si+LINE_X_FRACTION]
    add     bx, 8000h
    adc     ax, 0
    cmp     ax, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    jl      short line_f16a
    or      dh, 1
line_f16a:
    mov     ax, [si+LINE_END_X]
    cmp     ax, word ptr cs:[sprite1+SPRITE_LEFT2]
    jge     short line_f177
    or      dl, 2
line_f177:
    cmp     ax, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    jl      short line_f181
    or      dl, 1
line_f181:
    test    dl, dh
    jz      short line_f18a
    and     dl, dh
    jmp     line_ef9b
line_f18a:
    or      dl, dh
    jz      short line_f1a0
    xor     dh, dh
    mov     [bp+region_mask], dx
    jmp     dispatch_region_case
line_f196:
    test    [bp+region_mask], 1
    jz      short line_f1a0
    jmp     dispatch_right_edge_case
line_f1a0:
    jmp     return_no_intersection
handle_horizontal_line:
    mov     byte ptr [si+LINE_DIRECTION], 1
    cmp     cx, dx
    jnz     short line_f1af
    mov     byte ptr [si+LINE_DIRECTION], 9
line_f1af:
    jle     short line_f1bd
    mov     byte ptr [si+LINE_DIRECTION], 0
    mov     [si+LINE_START_X], dx
    mov     [si+LINE_END_X], cx
    xchg    cx, dx
line_f1bd:
    cmp     [bp+clip_mode], 0
    jnz     short line_f1e4
    mov     bx, word ptr cs:[sprite1+SPRITE_TOP]
    cmp     ax, bx
    jge     short line_f1ed
    mov     al, 4
    mov     [si+LINE_START_Y], bx
    mov     [si+LINE_END_Y], bx
line_f1d4:
    mov     [si+LINE_STATUS], al
    mov     word ptr [si+LINE_LENGTH], 0
    xor     ah, ah
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_f1e4:
    sub     dx, cx
    inc     dx
    mov     [si+LINE_LENGTH], dx
    jmp     return_no_intersection
line_f1ed:
    mov     bx, word ptr cs:[sprite1+SPRITE_HEIGHT]
    cmp     ax, bx
    jl      short line_f200
    mov     al, 8
    mov     [si+LINE_START_Y], bx
    mov     [si+LINE_END_Y], bx
    jmp     short line_f1d4
line_f200:
    mov     ax, dx
    sub     ax, cx
    inc     ax
    mov     [si+LINE_LENGTH], ax
    cmp     dx, word ptr cs:[sprite1+SPRITE_LEFT2]
    jge     short line_f21b
    dec     word ptr [si+LINE_END_Y]
    mov     word ptr [si+LINE_RIGHT_CLIP], 1
    mov     al, 2
    jmp     short line_f1d4
line_f21b:
    cmp     cx, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    jl      short line_f22e
    dec     word ptr [si+LINE_END_Y]
    mov     word ptr [si+LINE_BOTTOM_CLIP], 1
    mov     al, 1
    jmp     short line_f1d4
line_f22e:
    mov     ax, word ptr cs:[sprite1+SPRITE_LEFT2]
    mov     bx, ax
    sub     ax, cx
    jle     short line_f23e
    mov     [si+LINE_START_X], bx
    sub     [si+LINE_LENGTH], ax
line_f23e:
    mov     ax, dx
    mov     bx, word ptr cs:[sprite1+SPRITE_WIDTHSUM]
    dec     bx
    sub     ax, bx
    jle     short line_f250
    sub     [si+LINE_LENGTH], ax
    mov     [si+LINE_END_X], bx
line_f250:
    jmp     return_no_intersection
recover_overflowed_coordinates:
    mov     cx, [si+LINE_END_Y]
    sar     cx, 1
    mov     ax, [si+LINE_START_Y]
    sar     ax, 1
    sub     cx, ax
    sar     cx, 1
    mov     dx, [si+LINE_END_X]
    sar     dx, 1
    mov     ax, [si+LINE_START_X]
    sar     ax, 1
    sub     dx, ax
    sar     dx, 1
line_f26f:
    cmp     word ptr [si+LINE_START_Y], 0C180h
    jg      short line_f2b0
line_f276:
    mov     ax, [si+LINE_START_Y]
    add     ax, cx
    mov     [si+LINE_START_Y], ax
    mov     bx, ax
    sub     ax, word ptr cs:[sprite1+SPRITE_TOP]
    jle     short line_f2ab
    cmp     ax, cx
    jle     short line_f28d
    mov     ax, cx
line_f28d:
    sub     bx, word ptr cs:[sprite1+SPRITE_HEIGHT]
    jle     short line_f298
    sub     ax, bx
    jle     short line_f2ab
line_f298:
    mov     bx, [si+LINE_START_X]
    cmp     bx, word ptr cs:[sprite1+SPRITE_LEFT2]
    jge     short line_f2a8
    add     [si+LINE_LEFT_CLIP], ax
    jmp     short line_f2ab
    nop
line_f2a8:
    add     [si+LINE_TOP_CLIP], ax
line_f2ab:
    add     [si+LINE_START_X], dx
    jmp     short line_f26f
line_f2b0:
    cmp     word ptr [si+LINE_END_Y], 3E80h
    jge     short line_f2d6
    cmp     word ptr [si+LINE_START_X], 0C180h
    jle     short line_f276
    cmp     word ptr [si+LINE_START_X], 3E80h
    jge     short line_f276
    cmp     word ptr [si+LINE_END_X], 0C180h
    jle     short line_f2d6
    cmp     word ptr [si+LINE_END_X], 3E80h
    jge     short line_f2d6
    jmp     line_ebad
line_f2d6:
    mov     ax, [si+LINE_END_Y]
    sub     ax, cx
    mov     [si+LINE_END_Y], ax
    mov     bx, ax
    sub     ax, word ptr cs:[sprite1+SPRITE_HEIGHT]
    inc     ax
    jge     short line_f30f
    neg     ax
    cmp     ax, cx
    jle     short line_f2f0
    mov     ax, cx
line_f2f0:
    sub     bx, word ptr cs:[sprite1+SPRITE_TOP]
    inc     bx
    jge     short line_f2fc
    add     ax, bx
    jle     short line_f30f
line_f2fc:
    mov     bx, [si+LINE_END_X]
    cmp     bx, word ptr cs:[sprite1+SPRITE_LEFT2]
    jge     short line_f30c
    add     [si+LINE_RIGHT_CLIP], ax
    jmp     short line_f30f
    nop
line_f30c:
    add     [si+LINE_BOTTOM_CLIP], ax
line_f30f:
    sub     [si+LINE_END_X], dx
    jmp     short line_f2b0
_draw_line_related endp
_TEXT ends
end
