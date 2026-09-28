.8086
INCLUDE include\platform_hw.inc
name asm012_154486
DGROUP group _DATA
assume ds:DGROUP, ss:DGROUP
_DATA segment word public 'DATA'
source_row_fraction_accumulator dw 0
_DATA ends
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
SPRITE struc
sprite_bitmapptr dw ?
sprite_bitmapseg dw ?
sprite_reserved4 dw ?
sprite_reserved6 dw ?
sprite_reserved8 dw ?
sprite_lineofs dw ?
sprite_left dw ?
sprite_right dw ?
sprite_top dw ?
sprite_height dw ?
sprite_pitch dw ?
sprite_reserved16 dw ?
sprite_width2 dw ?
sprite_left2 dw ?
sprite_widthsum dw ?
SPRITE ends
extrn _sprite1:word
extrn _sprite2:word
public _draw_filled_rect
public _clear_rect
public _incnums
public _sub_35DC8
public _sub_35DE6
public _sub_35E08
public _loc_35ED9
; -----------------------------------------------------------------------------
; Purpose: Draw a filled rectangle using the scaled sprite blitter.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (byte ptr 14).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_draw_filled_rect proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12
    arg_8 = byte ptr 14

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     dx, cs:_sprite1+SPRITE.sprite_left
    mov     ax, dx
    sub     ax, [bp+arg_0]
    jle     short check_rectangle_right_edge
    mov     [bp+arg_0], dx
    sub     [bp+arg_4], ax
    jle     short return_empty_rectangle
check_rectangle_right_edge:
    mov     ax, [bp+arg_0]
    add     ax, [bp+arg_4]
    sub     ax, cs:_sprite1+SPRITE.sprite_right
    jle     short check_rectangle_top_edge
    sub     [bp+arg_4], ax
    jle     short return_empty_rectangle
check_rectangle_top_edge:
    mov     ax, cs:_sprite1+SPRITE.sprite_top
    sub     ax, [bp+arg_2]
    jle     short check_rectangle_bottom_edge
    sub     [bp+arg_6], ax
    jle     short return_empty_rectangle
    mov     ax, cs:_sprite1+SPRITE.sprite_top
    mov     [bp+arg_2], ax
check_rectangle_bottom_edge:
    mov     ax, [bp+arg_2]
    add     ax, [bp+arg_6]
    mov     bx, cs:_sprite1+SPRITE.sprite_height
    sub     ax, bx
    jle     short draw_clipped_rectangle
    sub     [bp+arg_6], ax
    jle     short return_empty_rectangle
    jmp     short draw_clipped_rectangle
    db 144
return_empty_rectangle:
    jmp     short return_after_rectangle_draw
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    push    si
    push    di
draw_clipped_rectangle:
    cld
    cmp     [bp+arg_4], 0
    jle     short return_empty_rectangle
    cmp     [bp+arg_6], 0
    jle     short return_empty_rectangle
    mov     es, word ptr cs:_sprite1+SPRITE.sprite_bitmapptr+2
    mov     al, [bp+arg_8]
    mov     ah, al
    mov     dx, [bp+arg_0]
    mov     si, [bp+arg_6]
    mov     bx, [bp+arg_2]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE.sprite_lineofs
    mov     di, cs:[bx]
    add     di, [bp+arg_0]
    mov     dx, cs:_sprite1+SPRITE.sprite_pitch
    sub     dx, [bp+arg_4]
    mov     cx, [bp+arg_4]
handle_odd_rectangle_width:
    sar     cx, 1
    mov     [bp+arg_4], cx
    jb      short draw_odd_rectangle_width
    jle     short return_after_rectangle_draw
draw_rectangle_rows:
    mov     cx, [bp+arg_4]
write_rectangle_word_pixels:
    xor     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    write_rectangle_word_pixels
    add     di, dx
    dec     si
    jg      short draw_rectangle_rows
return_after_rectangle_draw:
    pop     di
    pop     si
    pop     bp
    retf
draw_odd_rectangle_width:
    jz      short draw_single_pixel_rows
    jl      short return_after_rectangle_draw
    inc     dx
draw_odd_rectangle_rows:
    mov     cx, [bp+arg_4]
write_odd_rect_word_pixels:
    xor     es:[di], ax ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    inc     di
    loop    write_odd_rect_word_pixels
    xor     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, dx
    dec     si
    jg      short draw_odd_rectangle_rows
    jmp     short return_after_rectangle_draw
draw_single_pixel_rows:
    inc     dx
write_single_pixel_rows:
    xor     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, dx
    dec     si
    jg      short write_single_pixel_rows
    jmp     short return_after_rectangle_draw
    ; align 2
    db 0
_draw_filled_rect endp
; -----------------------------------------------------------------------------
; Purpose: Clear a rectangle in the active sprite or video buffer.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12); arg_8
;   (byte ptr 14).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_clear_rect proc far
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
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
    arg_8 = word ptr 14

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    si
    push    di
    push    ds
    cld
    mov     ax, word ptr cs:_sprite1.sprite_bitmapptr+2
    mov     es, ax
    mov     ax, word ptr cs:_sprite2.sprite_bitmapptr+2
    mov     ds, ax
    mov     ax, [bp+arg_0]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_4]
    mov     ax, [bp+arg_2]
    shl     ax, 1
    mov     cx, ax
    add     ax, cs:_sprite2.sprite_lineofs
    mov     [bp+var_4], ax
    add     cx, cs:_sprite1.sprite_lineofs
    mov     ax, [bp+arg_8]
    mov     bx, ax
    add     ax, [bp+arg_0]
    cwd
    idiv    cs:_sprite1.sprite_width2
    sub     dx, [bp+arg_0]
    shl     ax, 1
    add     cx, ax
    add     [bp+var_2], dx
    mov     [bp+var_6], cx
    mov     dx, [bp+arg_6]
copy_sprite_scanline:
    mov     cx, [bp+arg_4]
    mov     bx, [bp+var_4]
    mov     si, cs:[bx]
    add     si, [bp+arg_0]
    mov     bx, [bp+var_6]
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    rep movsb
    add     [bp+var_4], 2
    add     [bp+var_6], 2
    dec     dx
    jg      short copy_sprite_scanline
    pop     ds
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
_incnums    db 0
    db 1
    db 2
    db 3
    db 4
    db 5
    db 6
    db 7
    db 8
    db 9
    db 10
    db 11
    db 12
    db 13
    db 14
    db 15
    db 16
    db 17
    db 18
    db 19
    db 20
    db 21
    db 22
    db 23
    db 24
    db 25
    db 26
    db 27
    db 28
    db 29
    db 30
    db 31
    db 32
    db 33
    db 34
    db 35
    db 36
    db 37
    db 38
    db 39
    db 40
    db 41
    db 42
    db 43
    db 44
    db 45
    db 46
    db 47
    db 48
    db 49
    db 50
    db 51
    db 52
    db 53
    db 54
    db 55
    db 56
    db 57
    db 58
    db 59
    db 60
    db 61
    db 62
    db 63
    db 64
    db 65
    db 66
    db 67
    db 68
    db 69
    db 70
    db 71
    db 72
    db 73
    db 74
    db 75
    db 76
    db 77
    db 78
    db 79
    db 80
    db 81
    db 82
    db 83
    db 84
    db 85
    db 86
    db 87
    db 88
    db 89
    db 90
    db 91
    db 92
    db 93
    db 94
    db 95
    db 96
    db 97
    db 98
    db 99
    db 100
    db 101
    db 102
    db 103
    db 104
    db 105
    db 106
    db 107
    db 108
    db 109
    db 110
    db 111
    db 112
    db 113
    db 114
    db 115
    db 116
    db 117
    db 118
    db 119
    db 120
    db 121
    db 122
    db 123
    db 124
    db 125
    db 126
    db 127
    db 128
    db 129
    db 130
    db 131
    db 132
    db 133
    db 134
    db 135
    db 136
    db 137
    db 138
    db 139
    db 140
    db 141
    db 142
    db 143
    db 144
    db 145
    db 146
    db 147
    db 148
    db 149
    db 150
    db 151
    db 152
    db 153
    db 154
    db 155
    db 156
    db 157
    db 158
    db 159
    db 160
    db 161
    db 162
    db 163
    db 164
    db 165
    db 166
    db 167
    db 168
    db 169
    db 170
    db 171
    db 172
    db 173
    db 174
    db 175
    db 176
    db 177
    db 178
    db 179
    db 180
    db 181
    db 182
    db 183
    db 184
    db 185
    db 186
    db 187
    db 188
    db 189
    db 190
    db 191
    db 192
    db 193
    db 194
    db 195
    db 196
    db 197
    db 198
    db 199
    db 200
    db 201
    db 202
    db 203
    db 204
    db 205
    db 206
    db 207
    db 208
    db 209
    db 210
    db 211
    db 212
    db 213
    db 214
    db 215
    db 216
    db 217
    db 218
    db 219
    db 220
    db 221
    db 222
    db 223
    db 224
    db 225
    db 226
    db 227
    db 228
    db 229
    db 230
    db 231
    db 232
    db 233
    db 234
    db 235
    db 236
    db 237
    db 238
    db 239
    db 240
    db 241
    db 242
    db 243
    db 244
    db 245
    db 246
    db 247
    db 248
    db 249
    db 250
    db 251
    db 252
    db 253
    db 254
    db 255
_clear_rect endp
copy_bytes equ 100h
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper sub 35DC8.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sub_35DC8 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push bp
    mov bp, sp
    push ds
    push si
    push di
    mov cx, copy_bytes
    mov ds, [bp+arg_2]
    mov si, [bp+arg_0]
    mov ax, cs
    mov es, ax
    mov di, offset _incnums
    cld
    rep movsb
    pop di
    pop si
    pop ds
    pop bp
    retf
_sub_35DC8 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper sub 35DE6. Its internal flow uses clear_copy_direction_flag, copy_bytes_to_increment_table.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12).
; Register ABI: touches AX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sub_35DE6 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12
    push bp
    mov bp, sp
    push ds
    push si
    push di
    mov cx, [bp+arg_2]
    mov ds, [bp+arg_6]
    mov si, [bp+arg_0]
    mov ax, cs
    mov es, ax
    lea di, _incnums[si]
    mov si, [bp+arg_4]
clear_copy_direction_flag:
    cld
copy_bytes_to_increment_table:
    rep movsb
    pop di
    pop si
    pop ds
    pop bp
    retf
_sub_35DE6 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper sub 35E08. Its internal flow uses allocate_sprite_scale_locals, prepare_sprite_dimensions.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (byte ptr 14); arg_shapeptr (dword ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES, SS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: source_row_fraction_accumulator.
; Globals written: source_row_fraction_accumulator.
; -----------------------------------------------------------------------------
_sub_35E08 proc far
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
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
    arg_8 = word ptr 14

    push    bp
    mov     bp, sp
allocate_sprite_scale_locals:
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
prepare_sprite_dimensions:
    mov     si, [bp+arg_2]
    mov     cx, [bp+arg_6]
    mov     ax, [si+4]
    imul    [bp+arg_0]
    sub     cl, ah
    sbb     ch, dl
    mov     [bp+var_2], cx
    mov     cx, [bp+arg_8]
    mov     ax, [si+6]
    imul    [bp+arg_0]
    sub     cl, ah
    sbb     ch, dl
    mov     [bp+var_4], cx
    jmp     short validate_sprite_dimensions
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short validate_sprite_dimensions
    ; align 2
    db 144
return_invalid_sprite_size:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     si, [bp+arg_2]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
validate_sprite_dimensions:
    cmp     [bp+arg_0], 2
    jb      short return_invalid_sprite_size
    cld
    mov     ax, [si+2]
    mul     [bp+arg_0]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short return_invalid_sprite_size
    mov     [bp+var_8], ax
    mov     ax, [si]
    mov     [bp+var_A], ax
    mul     [bp+arg_0]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short return_invalid_sprite_size
    mov     [bp+var_6], ax
    mov     dx, cs:_sprite1.sprite_pitch
    sub     dx, ax
    mov     [bp+var_C], dx
    add     si, 10h
    mov     dx, 1
    xor     ax, ax
    mov     [bp+var_14], ax
    mov     [bp+var_12], ax
    div     [bp+arg_0]
    mov     dx, ax
    mov     al, ah
    xor     ah, ah
    shr     ax, 1
    jz      short save_sprite_source_origin
    add     si, ax
    mov     cx, ax
step_sprite_source_rows:
    add     si, [bp+var_A]
    loop    step_sprite_source_rows
save_sprite_source_origin:
    mov     [bp+var_E], si
    mov     [bp+var_10], dx
    var_14 = word ptr -20
    var_12 = word ptr -18
    var_10 = word ptr -16
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
    arg_shapeptr = dword ptr 8
    arg_6 = word ptr 12
    arg_8 = word ptr 14
_loc_35ED9:
    mov     ax, word ptr cs:_sprite1.sprite_bitmapptr+2
    mov     es, ax
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, cs:_sprite1.sprite_lineofs
    mov     di, cs:[bx]
    add     di, [bp+var_2]
    mov     dx, [bp+var_10]
    mov     ax, [bp+var_12]
    mov     ss:source_row_fraction_accumulator, ax
begin_scaled_sprite_row:
    mov     cx, [bp+var_6]
    mov     bx, [bp+var_14]
draw_scaled_sprite_pixels:
    mov     al, [si]
    cmp     al, 0FFh
    jz      short advance_scaled_sprite_pixel
    mov     es:[di], al
advance_scaled_sprite_pixel:
    inc     di
    add     bx, dx
    mov     al, bh
    xor     ah, ah
    add     si, ax
    xor     bh, bh
    loop    draw_scaled_sprite_pixels
    dec     [bp+var_8]
    jle     short return_after_sprite_draw
    add     di, [bp+var_C]
    mov     si, [bp+var_E]
    add     ss:source_row_fraction_accumulator, dx
    mov     cl, byte ptr ss:source_row_fraction_accumulator+1
    or      cl, cl
    jz      short begin_scaled_sprite_row
    xor     ch, ch
skip_source_rows_for_scale:
    add     si, [bp+var_A]
    loop    skip_source_rows_for_scale
    mov     byte ptr ss:source_row_fraction_accumulator+1, 0
    mov     [bp+var_E], si
    jmp     short begin_scaled_sprite_row
return_after_sprite_draw:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
    ; align 2
    db 0
_sub_35E08 endp
S012_TEXT ends
end
