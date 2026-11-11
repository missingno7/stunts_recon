; ASMDOC Module: sprite_rectangle_scaled_blitters. Proposed content name: sprite_rectangle_scaled_blitters. Existing
; ASMDOC module name covers rectangles, mapping-table operations and shared scaled bitmap tail; retain.
; ASMDOC Public routines: _draw_filled_rect, _clear_rect, _sub_35DC8, _sub_35DE6, _sub_35E08, _loc_35ED9.
; ASMDOC Static contract audit: byte/word widths and offsets describe original 8086 paths; inherited generated comments
; ASMDOC below are retained verbatim and may disagree. BP offsets are after PUSH BP/MOV BP,SP. Destination bitmap/table
; ASMDOC memory is caller-selected; no owned routine calls an application callback.
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
; ASMDOC Contract _draw_filled_rect: XOR the clipped rectangle with the low byte of the supplied color.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 i16 x; BP+8 i16 y; BP+10 i16 width; BP+12 i16 height; BP+14 u8 XOR operand (only low byte read);
; ASMDOC clipping mutates x/y/width/height argument words; width then replaced by half-width.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP, DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, flags; DF=0 on accepted dimensions, unchanged on early clipping exit.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, clip left/right +0Ch/+0Eh, top/bottom
; ASMDOC +10h/+12h, pitch +14h; CS row-table words.
; ASMDOC globals_written: Caller x/y/width/height argument words on clipping/draw paths; Bitmap memory selected by
; ASMDOC CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Uses XOR, not assignment fill, and no sprite scaling. Signed wrapped clip arithmetic; empty dimensions
; ASMDOC rejected. Later rows add pitch. Unreachable unclipped prologue after normal early-exit jump..
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
; ASMDOC Dormant unlabeled prologue: unclipped XOR rectangle entry. Named public-entry control flow skips this stub;
; ASMDOC retained extent is intentional.
    push    bp
    mov     bp, sp
    push    si
    push    di
; ASMDOC After clipping, XOR rectangle rows; not assignment fill.
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
; ASMDOC Contract _clear_rect: Copy a rectangle from sprite2 to a linearly displaced position in sprite1.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 i16 source x; BP+8 i16 source y; BP+10 u16 byte width; BP+12 i16 row count; BP+14 i16 linear
; ASMDOC destination displacement; quotient/remainder from signed (x+displacement modulo 65536)/sprite1.width2 select
; ASMDOC destination y+quotient and remainder x.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, flags; DF=0.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, line-table +0Ah, signed width2 +18h; CS:_sprite2 bitmap segment
; ASMDOC +2 and line-table +0Ah; both CS row-table arrays.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Not a color clear. REP MOVSB forward, then source/destination row table cursors +2; no clipping, no
; ASMDOC overlap repair, no dimension validation. Zero/negative height still executes first row. Division can fault on
; ASMDOC zero divisor or quotient overflow..
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
; ASMDOC Forward sprite2-to-sprite1 row copy; row-table cursors advance independently.
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
; ASMDOC PUBLIC DATA _incnums: Identity-initialized byte-to-byte pixel remapping table; full/partial copy entries can
; ASMDOC change it; transparent bitmap paths test mapped value against FFh.
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
; ASMDOC Contract _sub_35DC8: Copy exactly 256 source bytes into the code-segment remapping table.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 source offset; BP+8 source segment; at least 256 readable bytes.
; ASMDOC outputs: CS:_incnums[0..255] overwritten in forward order; AX=CS, CX=0, ES=CS..
; ASMDOC preserves: BP, SP, DS, SI, DI, BX, DX, SS, CS.
; ASMDOC clobbers: AX, CX, ES; DF=0, other arithmetic flags unchanged by body.
; ASMDOC globals_read: none.
; ASMDOC globals_written: CS:_incnums[256].
; ASMDOC services: none.
; ASMDOC dead: True.
; ASMDOC notes: Data in CODE segment is writable. No span checks or overlap buffering; changes data bytes, not
; ASMDOC executable opcodes..
; ASMDOC Proposed routine name: _sprite_copy_increment_table.
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
; ASMDOC Contract _sub_35DE6: Copy an explicit source byte span into the code-segment remapping table at an index.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 destination table index; BP+8 u16 byte count; BP+10 source offset; BP+12 source segment.
; ASMDOC outputs: count bytes copied forward to CS:(_incnums+index modulo 65536); AX=CS, CX=0, ES=CS..
; ASMDOC preserves: BP, SP, DS, SI, DI, BX, DX, SS, CS.
; ASMDOC clobbers: AX, CX, ES; DF=0, other arithmetic flags unchanged by body.
; ASMDOC globals_read: none.
; ASMDOC globals_written: CS:_incnums+index for count bytes; no table-bound check.
; ASMDOC services: none.
; ASMDOC dead: True.
; ASMDOC notes: Count zero copies nothing; caller must keep requested destination within intended storage. Normal host
; ASMDOC adapter deliberately rejects out-of-table spans..
; ASMDOC Proposed routine name: _sprite_update_increment_table.
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
; ASMDOC Contract _sub_35E08: Unclipped Q8 scaled transparent bitmap blit with scaled hotspot.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 Q8 scale (256=1:1; accepted 2..65535); BP+8 source shape offset; BP+10 source segment; BP+12
; ASMDOC i16 destination x; BP+14 i16 destination y; source +0/+2 dimensions, +4/+6 hotspot, bitmap +10h; SS must
; ASMDOC address DGROUP for row fraction accumulator.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF cleared only if draw path reaches CLD.
; ASMDOC globals_read: CS:_sprite1 pitch +14h, bitmap segment +2, row-table +0Ah; first CS row-table word.
; ASMDOC globals_written: SS:source_row_fraction_accumulator; Bitmap memory selected by CS:_sprite1; descriptor fields
; ASMDOC unchanged.
; ASMDOC services: none.
; ASMDOC dead: True.
; ASMDOC notes: No clipping in this public prologue. Dimensions use unsigned MUL(scale) with middle 16 product bits;
; ASMDOC hotspot uses signed IMUL middle bits. scale<2 rejected unsigned; zero scaled dimension rejected. Source
; ASMDOC step=floor(65536/scale), Q8 sampling; initial half-step selects sampling center; raw FFh skips (no incnums
; ASMDOC remap). Shares live tail _loc_35ED9..
; ASMDOC Proposed routine name: _shape2d_scale_blit_hotspot.
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
; ASMDOC Dormant unlabeled prologue: scaled transparent blit at explicit position. Named public-entry control flow
; ASMDOC skips this stub; retained extent is intentional.
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
; ASMDOC Dormant unlabeled prologue: scaled transparent blit at stored position. Named public-entry control flow skips
; ASMDOC this stub; retained extent is intentional.
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
; ASMDOC Unclipped scale setup computes positive dimensions and reciprocal; later public tail is also used by
; ASMDOC shapeexpl.
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
; ASMDOC Contract _loc_35ED9: Shared scaled-transparent bitmap raster continuation, also reached from shapeexpl.
; ASMDOC convention: Public interior near-jump continuation, NOT a callable FAR/near procedure; falls through from
; ASMDOC local setup or JMP from _shapeexpl; shared unwind performs RETF to original caller.
; ASMDOC inputs: Inherited BP frame: [BP-2] x, [-4] y, [-6] positive output width, [-8] positive remaining rows, [-10]
; ASMDOC source width, [-12] destination pitch minus output width, [-14] source row origin, [-16] reciprocal Q8 step,
; ASMDOC [-18] initial vertical fraction, [-20] horizontal fraction; DS:SI current source pixel; SS=DGROUP; stack
; ASMDOC already contains saved DS, SI, DI from SUB SP,16h/PUSH DS/PUSH SI/PUSH DI; BP+2/+4 FAR return.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: Restores original caller BP, SP, DS, SI and DI from inherited frame; does not preserve
; ASMDOC continuation-entry DS/SI/DI values; SS and CS unchanged.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF inherited (normal setup supplies DF=0).
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2 and row-table +0Ah; first CS row-table word;
; ASMDOC SS:source_row_fraction_accumulator.
; ASMDOC globals_written: SS:source_row_fraction_accumulator; Inherited locals [BP-8] remaining rows and [BP-14] source
; ASMDOC row origin; Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Does not perform clipping itself; shapeexpl supplies clipped locals. Pixel AL=FFh leaves destination
; ASMDOC unchanged; otherwise write raw AL. Fraction high byte selects integer source skip then is cleared; source and
; ASMDOC destination arithmetic wrap at 16 bits. Final unwind restores saved DS/SI/DI/BP. Shared data state is
; ASMDOC non-reentrant..
; ASMDOC Proposed routine name: _shape2d_scale_blit_tail.
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
; ASMDOC Horizontal Q8 sampling restarts each row; vertical fraction lives in SS:DGROUP writable data.
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
