; ASMDOC Module: sprite_shape_video_ops. Proposed content name: sprite_shape_video_ops. Existing name describes this
; ASMDOC inseparable mixed bitmap/RLE/text/palette module; retain.
; ASMDOC Public routines: _sprite_putimage_or_alt, _sprite_putimage_or, _putpixel_iconFillings, _shape2d_op_unk4,
; ASMDOC _sprite_putimage_transparent, _release_shape_resources, _draw_text_at, _video_set_palette.
; ASMDOC Static contract audit: byte/word widths and offsets describe original 8086 paths; inherited generated
; comments
; ASMDOC below are retained verbatim and may disagree. BP offsets are after PUSH BP/MOV BP,SP. Destination
; bitmap/table
; ASMDOC memory is caller-selected; no owned routine calls an application callback.
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
_aCopyrightCUnlimitedSoftwareInc_198 db 'Copyright (C) Unlimited Software Inc. 1989,1990.'
    db '  All rights reserved.', 6 dup (0)
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS, dseg
dseg segment byte public 'STUNTSD'
extrn _fontdefseg:word
dseg ends
assume cs:S012_TEXT, ds:DGROUP
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
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments:
; arg_2 (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_putimage_or_alt: Clipped OR bitmap blit with hotspot-adjusted destination.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 source SHAPE2D offset; BP+8 source segment; source header width +0, height +2; packed pixel
; ASMDOC bytes +10h; BP+10 i16 destination x; BP+12 i16 destination y; header +4/+6 signed hotspot words subtracted
; ASMDOC from x/y with 16-bit wrap.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF cleared only if draw path reaches CLD.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, clip left/right +0Ch/+0Eh, top/bottom
; ASMDOC +10h/+12h, pitch +14h; CS row-table words.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Shares sprite_or_join_01 and return with _sprite_putimage_or. Positive source dimensions are an
; ASMDOC expected precondition (height zero can still execute one row). Clips using signed 16-bit branches and wrapped
; ASMDOC coordinates. Source and destination offsets wrap at 16 bits. Width-one, zero-source-skip path increments row
; ASMDOC advance after each row: later destination row strides grow by one. No callbacks, DOS or BIOS calls..
_sprite_putimage_or_alt proc far
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    sub     ax, [si+4]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_destination_y]
    sub     ax, [si+6]
    mov     [bp+local_destination_y], ax
    jmp     short sprite_or_join_01 ; goto _sprite_putimage_or
    even ; word-align the following entry
_sprite_putimage_or_alt endp
; -----------------------------------------------------------------------------
; Purpose: Draw a sprite image using the OR compositing operation.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_putimage_or: Clipped OR bitmap blit at explicit destination.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 source SHAPE2D offset; BP+8 source segment; source header width +0, height +2; packed pixel
; ASMDOC bytes +10h; BP+10 i16 destination x; BP+12 i16 destination y.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF cleared only if draw path reaches CLD.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, clip left/right +0Ch/+0Eh, top/bottom
; ASMDOC +10h/+12h, pitch +14h; CS row-table words.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Positive source dimensions are an expected precondition (height zero can still execute one row). Clips
; ASMDOC using signed 16-bit branches and wrapped coordinates. Source and destination offsets wrap at 16 bits.
; ASMDOC Width-one, zero-source-skip path increments row advance after each row: later destination row strides grow by
; ASMDOC one. No callbacks, DOS or BIOS calls..
_sprite_putimage_or proc far
    local_word_count = word ptr -14
    local_source_cursor = word ptr -12
    local_rows_left = word ptr -10
    local_source_skip_or_cursor = word ptr -8
    local_visible_width = word ptr -6
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_destination_y]
    mov     [bp+local_destination_y], ax
    jmp     short sprite_or_join_01
    even ; word-align the following entry
; ASMDOC Dormant unlabeled prologue: OR at stored shape position. Named public-entry control flow skips this stub;
; ASMDOC retained extent is intentional.
    push    bp
    mov     bp, sp
    sub     sp, 0Eh
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [si+8]
    mov     [bp+local_destination_x], ax
    mov     ax, [si+0Ah]
    mov     [bp+local_destination_y], ax
; ASMDOC Shared clipped OR core: wrappers provide DS:SI and x/y locals; far return restores their saves.
sprite_or_join_01:
    mov     bx, [bp+local_destination_y]
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
    mov     [bp+local_source_skip_or_cursor], dx
    mul     word ptr [si]
    mov     dx, [bp+local_source_skip_or_cursor]
    add     dx, ax
    mov     ax, bx
    add     ax, cx
    sub     ax, cs:_sprite1+SPRITE_HEIGHT
    jle     short sprite_or_branch_path_02
    sub     cx, ax
    jle     short sprite_or_loop_head_01
sprite_or_branch_path_02:
    mov     [bp+local_source_cursor], dx
    mov     [bp+local_rows_left], cx
    mov     [bp+local_destination_y], bx
    mov     cx, [si]
    xor     dx, dx
    mov     bx, [bp+local_destination_x]
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
    nop ; retained code pad at an even offset; EVEN would emit no byte
sprite_or_branch_path_03:
    mov     bx, cs:_sprite1+SPRITE_LEFT
    add     ax, cx
    sub     ax, bx
    jle     short sprite_or_loop_head_01
    mov     si, cx
    sub     si, ax
    add     [bp+local_source_cursor], si
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
    mov     [bp+local_visible_width], cx
    mov     [bp+local_source_skip_or_cursor], dx
    mov     [bp+local_destination_x], bx
    cld
    mov     es, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
    mov     bx, [bp+local_destination_y]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     di, cs:[bx]
    add     di, [bp+local_destination_x]
    mov     si, [bp+local_source_cursor]
    mov     dx, [bp+local_rows_left]
sprite_or_branch_path_06:
    mov     bx, cs:_sprite1+SPRITE_PITCH
    sub     bx, [bp+local_visible_width]
    cmp     [bp+local_source_skip_or_cursor], 0
    jz      short sprite_or_branch_path_08
sprite_or_loop_head_02:
    mov     cx, [bp+local_visible_width]
sprite_or_loop_head_03:
    lodsb
    or      es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    loop    sprite_or_loop_head_03
    add     si, [bp+local_source_skip_or_cursor]
    add     di, bx
    dec     dx
    jg      short sprite_or_loop_head_02
sprite_or_branch_path_07:
    jmp     sprite_or_loop_head_01
sprite_or_branch_path_08:
    mov     ax, [bp+local_visible_width]
    shr     ax, 1
    mov     [bp+local_word_count], ax
    jnb     short sprite_or_loop_head_06
    jz      short sprite_or_branch_path_09
    inc     bx
sprite_or_loop_head_04:
    mov     cx, [bp+local_word_count]
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
    mov     cx, [bp+local_word_count]
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
    db 0 ; retained zero fill at this boundary
; ASMDOC Dormant unlabeled prologue: unclipped icon OR with hotspot subtraction. Named public-entry control flow skips
; ASMDOC this stub; retained extent is intentional.
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
    even ; word-align the following entry
_sprite_putimage_or endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by putpixel iconFillings.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _putpixel_iconFillings: Unclipped OR bitmap blit at explicit destination.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 source SHAPE2D offset; BP+8 source segment; source header width +0, height +2; packed pixel
; ASMDOC bytes +10h; BP+10 i16 destination x; BP+12 i16 destination y.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF=0.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah and pitch +14h; CS row-table words.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Positive width/height expected: loops enter before testing row count and width zero can
; underflow LOOP.
; ASMDOC Width one ORs a word after LODSB; AH=0 so the adjacent byte is read/written but unchanged. No clipping or
; ASMDOC callbacks..
_putpixel_iconFillings proc far
    var_6 = word ptr -6
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [bp+arg_destination_x]
    mov     [bp+local_destination_x], ax
    mov     ax, [bp+arg_destination_y]
    mov     [bp+local_destination_y], ax
    jmp     short icon_fill_join_01
    even ; word-align the following entry
; ASMDOC Dormant unlabeled prologue: unclipped icon OR at stored position. Named public-entry control flow skips this
; ASMDOC stub; retained extent is intentional.
    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [si+8]
    mov     [bp+local_destination_x], ax
    mov     ax, [si+0Ah]
    mov     [bp+local_destination_y], ax
; ASMDOC Unclipped OR rows; width-one special case uses a word OR with AH zero.
icon_fill_join_01:
    mov     bx, [bp+local_destination_y]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     di, cs:[bx]
    add     di, [bp+local_destination_x]
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
    db 0 ; retained zero fill at this boundary
; ASMDOC Dormant unlabeled prologue: RLE OR with hotspot subtraction. Named public-entry control flow skips this stub;
; ASMDOC retained extent is intentional.
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
    even ; word-align the following entry
; ASMDOC Dormant unlabeled prologue: RLE OR at explicit position. Named public-entry control flow skips this stub;
; ASMDOC retained extent is intentional.
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
    even ; word-align the following entry
_putpixel_iconFillings endp
; -----------------------------------------------------------------------------
; Purpose: Process a 2D shape operation variant.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_shapeofs (word ptr 6);
; arg_shapeseg (word ptr 8).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none
; inferred; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _shape2d_op_unk4: Decode signed byte RLE packets and OR pixels at the stored shape position without
; ASMDOC clipping.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 source SHAPE2D offset; BP+8 source segment; source header width +0, height +2; packed pixel
; ASMDOC bytes +10h; header x +8, y +0Ah; stream at +10h: 0=end, +1..127 repeat next byte, -1..-128 literal byte
; count.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF=0.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah; CS row-table words.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Width resets the per-row remainder; each row reloads its CS row-table entry. Shape height is never
; ASMDOC read: only zero packet terminates; no stream/span checks. Negative 80h means 128 literals..
; ASMDOC Proposed routine name: _shape2d_or_rle.
_shape2d_op_unk4 proc far
    var_10 = word ptr -16
    var_E = word ptr -14
    var_6 = word ptr -6
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
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
    mov     [bp+local_destination_x], ax
    mov     ax, [si+SHAPE2D.s2d_pos_y]
    mov     [bp+local_destination_y], ax
; ASMDOC Signed-run decoder; zero stream control terminates, not shape height.
shape_op4_join_01:
    cld
    lea     ax, [si+(size SHAPE2D)]
    mov     [bp+var_E], ax
    mov     ax, [si]
    mov     [bp+var_6], ax
    mov     bx, [bp+local_destination_y]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     [bp+var_10], bx
    mov     di, cs:[bx]
    add     di, [bp+local_destination_x]
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
    add     di, [bp+local_destination_x]
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
    add     di, [bp+local_destination_x]
    mov     dx, [bp+var_6]
shape_op4_branch_path_04:
    loop    shape_op4_loop_head_03
    jmp     short shape_op4_loop_head_01
; ASMDOC Dormant unlabeled prologue: clipped remapped transparent blit with hotspot subtraction. Named public-entry
; ASMDOC control flow skips this stub; retained extent is intentional.
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
    even ; word-align the following entry
_shape2d_op_unk4 endp
; -----------------------------------------------------------------------------
; Purpose: Draw a sprite image while preserving transparent pixels.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_4 (word ptr 8); arg_6
; (word ptr 10).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _incnums, _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _sprite_putimage_transparent: Clipped bitmap blit through the color-remapping table; mapped
; FFh skips
; ASMDOC destination.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 source SHAPE2D offset; BP+8 source segment; source header width +0, height +2; packed pixel
; ASMDOC bytes +10h; BP+10 i16 destination x; BP+12 i16 destination y.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF cleared only if draw path reaches CLD.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, clip left/right +0Ch/+0Eh, top/bottom
; ASMDOC +10h/+12h, pitch +14h; CS row-table words; CS:_incnums[256] indexed by raw source byte.
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Transparency is tested AFTER table lookup; raw FFh can draw if remapped. Positive source
; dimensions are
; ASMDOC an expected precondition (height zero can still execute one row). Clips using signed 16-bit branches and
; ASMDOC wrapped coordinates. Source and destination offsets wrap at 16 bits. No callbacks, DOS or BIOS calls..
_sprite_putimage_transparent proc far
    var_bmpptr = word ptr -12
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_shapey = word ptr -4
    var_shapex = word ptr -2
    shapeptr = dword ptr 6
    arg_destination_x = word ptr 10
    arg_destination_y = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 10h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+shapeptr+2]
    mov     si, word ptr [bp+shapeptr]
    mov     ax, [bp+arg_destination_x]
    mov     [bp+var_shapex], ax
    mov     ax, [bp+arg_destination_y]
    mov     [bp+var_shapey], ax
    jmp     short sprite_alpha_join_01
    even ; word-align the following entry
; ASMDOC Dormant unlabeled prologue: clipped remapped transparent blit at stored position. Named public-entry control
; ASMDOC flow skips this stub; retained extent is intentional.
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
; ASMDOC Clip source/destination rectangle before remapping every source byte; mapped FFh is transparent.
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
    nop ; retained code pad at an even offset; EVEN would emit no byte
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
    nop ; retained code pad at an even offset; EVEN would emit no byte
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
    db 0 ; retained zero fill at this boundary
; ASMDOC Dormant unlabeled prologue: unclipped remapped transparent blit with hotspot subtraction. Named public-entry
; ASMDOC control flow skips this stub; retained extent is intentional.
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
    even ; word-align the following entry
; ASMDOC Dormant unlabeled prologue: unclipped remapped transparent blit at explicit position. Named public-entry
; ASMDOC control flow skips this stub; retained extent is intentional.
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
    even ; word-align the following entry
_sprite_putimage_transparent endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by release shape resources.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed:
; none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _fontdefseg, _incnums, _sprite1.
; Globals written: _aCopyrightCUnlimitedSoftwareInc_198, fh_x, fh_y.
; Global addresses taken: _aCopyrightCUnlimitedSoftwareInc_198.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _release_shape_resources: Unclipped remapped-transparent bitmap blit at shape-stored position.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 source SHAPE2D offset; BP+8 source segment; source header width +0, height +2; packed pixel
; ASMDOC bytes +10h; header x +8, y +0Ah.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF=0.
; ASMDOC globals_read: CS:_sprite1 bitmap segment +2, row-table offset +0Ah, pitch +14h; first CS row-table word;
; ASMDOC CS:_incnums[256].
; ASMDOC globals_written: Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: Does not free or release resources. Positive dimensions required; loops enter before bounds
; tests. Only
; ASMDOC initial destination row uses row table; later rows add pitch. Does not write copyright scratch or font state:
; ASMDOC those instructions belong to an unreachable trailing entry stub..
_release_shape_resources proc far
    var_8 = word ptr -8
    var_6 = word ptr -6
    local_destination_y = word ptr -4
    local_destination_x = word ptr -2
    arg_shape_offset = word ptr 6
    arg_shape_segment = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_shape_segment]
    mov     si, [bp+arg_shape_offset]
    mov     ax, [si+8]
    mov     [bp+local_destination_x], ax
    mov     ax, [si+0Ah]
    mov     [bp+local_destination_y], ax
; ASMDOC Unclipped remapped bitmap core, unrelated to resource ownership or freeing.
shape_release_join_01:
    mov     bx, [bp+local_destination_y]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
    mov     di, cs:[bx]
    add     di, [bp+local_destination_x]
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
    nop ; retained code pad at an even offset; EVEN would emit no byte
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
; ASMDOC Dormant unlabeled prologue: single-character text draw using copyright-tail scratch. Named public-entry
; ASMDOC control flow skips this stub; retained extent is intentional.
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
    mov     ds:[fh_x], ax
    mov     ax, [bp+0Ah]
    mov     ds:[fh_y], ax
    jmp     short draw_text_join_01
    even ; word-align the following entry
_release_shape_resources endp
; -----------------------------------------------------------------------------
; Purpose: Draw or clear the graphics content identified by draw text at.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 6); arg_4 (word ptr 8).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, CS, DS, ES, SS; balanced explicit saves/restores observed:
; none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _fontdefseg, _sprite1, fh_advance, fh_bgcolor, fh_fgcolor, fh_glyphs, fh_height, fh_left,
; fh_lineheight, fh_proportional, fh_rowbytes, fh_x, fh_y.
; Globals written: fh_advance, fh_rowbytes, fh_x, fh_y.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _draw_text_at: Opaque bitmap-font text draw at explicit position; advances font cursor.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 near SS-relative zero-terminated legacy-byte string offset (argument word incremented in place);
; ASMDOC BP+8 i16 x; BP+10 i16 y; entry DS addresses DGROUP:_fontdefseg.
; ASMDOC outputs: No defined scalar return; writes destination pixels..
; ASMDOC preserves: BP, SP (caller argument cleanup), DS, SI, DI, SS, CS.
; ASMDOC clobbers: AX, BX, CX, DX, ES, arithmetic flags; DF=0.
; ASMDOC globals_read: DGROUP:_fontdefseg; Font segment fields fg/bg low bytes +0/+2, left +4, x/y +8/+0Ah, rowbytes
; ASMDOC +0Ch, height +0Eh, advance +10h, line height +12h, proportional byte +14h, glyph offset table +16h and
; ASMDOC bitmaps; CS:_sprite1 bitmap segment +2 and row-table offset +0Ah; CS row-table words.
; ASMDOC globals_written: Font segment x +8 and y +0Ah; proportional glyphs also advance +10h and rowbytes +0Ch;
; Caller
; ASMDOC argument word [BP+6]; Bitmap memory selected by CS:_sprite1; descriptor fields unchanged.
; ASMDOC services: none.
; ASMDOC dead: False.
; ASMDOC notes: MSB-first glyph bits draw fg and clear bits bg; full rowbytes*8 bits drawn. Missing glyph CR or LF
; ASMDOC resets x to left and adds line height; CRLF does that twice. Existing CR/LF glyph suppresses control
; handling.
; ASMDOC No clipping; positive glyph rowbytes/height required..
_draw_text_at proc far
    local_rows_left = word ptr -4
    local_glyph_x = word ptr -2
    arg_text_offset = word ptr 6
    arg_initial_x = word ptr 8
    arg_initial_y = word ptr 10

    push    bp
    mov     bp, sp
    sub     sp, 4
    push    ds
    push    si
    push    di
    mov     ds, _fontdefseg  ; ds = seg039
    mov     ax, [bp+arg_initial_x]
    mov     ds:[fh_x], ax
    mov     ax, [bp+arg_initial_y]
    mov     ds:[fh_y], ax
    jmp     short draw_text_join_01
    even ; word-align the following entry
; ASMDOC Dormant unlabeled prologue: opaque text draw at current font cursor. Named public-entry control flow skips
; ASMDOC this stub; retained extent is intentional.
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    ds
    push    si
    push    di
    mov     ds, _fontdefseg
; ASMDOC Opaque font core: SS-relative string, DS font glyphs, ES target; full bytes include background pixels.
draw_text_join_01:
    cld
    mov     es, word ptr cs:_sprite1+SPRITE_BITMAPPTR+2
draw_text_loop_head_01:
    mov     si, [bp+arg_text_offset]
    mov     bl, ss:[si]
    or      bl, bl
    jz      short draw_text_branch_path_02
    mov     al, bl
    inc     [bp+arg_text_offset]
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
    mov     ds:[fh_x], ax
    mov     ax, ds:[fh_lineheight]
    add     ds:[fh_y], ax
    jmp     short draw_text_loop_head_01
draw_text_branch_path_02:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
draw_text_branch_path_03:
    mov     ax, ds:[fh_x]
    mov     [bp+local_glyph_x], ax
    cmp     byte ptr ds:[fh_proportional], 0
    jz      short draw_text_branch_path_04
    mov     al, [si]
    xor     ah, ah
    inc     si
    mov     ds:[fh_advance], ax
    add     ax, 7
    shr     ax, 1
    shr     ax, 1
    shr     ax, 1
    mov     ds:[fh_rowbytes], al
draw_text_branch_path_04:
    mov     al, byte ptr ds:[fh_fgcolor]
    mov     ah, byte ptr ds:[fh_bgcolor]
    mov     cx, ds:[fh_height]
    mov     [bp+local_rows_left], cx
    mov     bx, ds:[fh_y]
    shl     bx, 1
    add     bx, cs:_sprite1+SPRITE_LINEOFS
draw_text_loop_head_02:
    mov     di, cs:[bx]
    add     di, [bp+local_glyph_x]
    mov     dh, ds:[fh_rowbytes]
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
    dec     [bp+local_rows_left]
    jg      short draw_text_loop_head_02
    mov     ax, ds:[fh_advance]
    add     ds:[fh_x], ax
    jmp     draw_text_loop_head_01
draw_text_branch_path_05:
    mov     es:[di], ah ;/* PLATFORM(video): accesses display memory through ES. */
    inc     di
    loop    draw_text_loop_head_04
    inc     si
    dec     dh
    jg      short draw_text_loop_head_03
    jmp     short draw_text_loop_head_05
    db 0 ; retained zero fill at this boundary
; ASMDOC Dormant unlabeled prologue: palette source with explicit far segment at BP+12. Named public-entry
; control flow
; ASMDOC skips this stub; retained extent is intentional.
    push    bp
    mov     bp, sp
    mov     ax, [bp+0Ch]
    jmp     short palette_join_01
    nop ; retained code pad at an even offset; EVEN would emit no byte
_draw_text_at endp
; -----------------------------------------------------------------------------
; Purpose: Load a block of palette entries through the BIOS video service.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 6); arg_4 (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, BP, SP, DS, ES; balanced explicit saves/restores observed: none
; inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(palette): calls the BIOS palette service. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _video_set_palette: Load a DAC register block through BIOS video service.
; ASMDOC convention: 16-bit FAR cdecl; BP+6 is first argument after PUSH BP; RETF without immediate; caller removes
; ASMDOC arguments.
; ASMDOC inputs: BP+6 u16 first DAC index; BP+8 u16 number of entries; BP+10 near entry-DS offset of 3*count RGB
; bytes;
; ASMDOC entry DS is palette source segment.
; ASMDOC outputs: BIOS DAC palette block updated; no wrapper-defined scalar return..
; ASMDOC preserves: BP and stack explicitly; CS/SS by FAR return; all other preservation depends on BIOS INT 10h
; ASMDOC contract.
; ASMDOC clobbers: Wrapper sets AX=1012h, BX=first, CX=count, DX=source offset, ES=entry DS; flags and interrupt
; ASMDOC register effects delegated to BIOS.
; ASMDOC globals_read: none.
; ASMDOC globals_written: none.
; ASMDOC services: INT 10h AX=1012h, BX first, CX count, ES:DX RGB block.
; ASMDOC dead: False.
; ASMDOC notes: ES is not saved. Wrapper does not modify DS itself; no RGB range checks. Unreachable preceding stub
; ASMDOC accepts source segment at BP+12 then joins palette_join_01..
_video_set_palette proc far
    arg_first_index = word ptr 6
    arg_entry_count = word ptr 8
    arg_rgb_offset = word ptr 10

    push    bp
    mov     bp, sp
    mov     ax, ds
; ASMDOC BIOS block DAC service; AX here supplies source segment before AX becomes 1012h.
palette_join_01:
    mov     es, ax
    mov     bx, [bp+arg_first_index]
    mov     dx, [bp+arg_rgb_offset]
    mov     cx, [bp+arg_entry_count]
    mov     ah, 10h
    mov     al, 12h
    int     R5HW_BIOS_VIDEO_INT
; - VIDEO - SET BLOCK OF DAC REGISTERS (EGA, VGA/MCGA) /* PLATFORM(palette): calls the BIOS palette service. */
    pop     bp
    retf
    db 0 ; retained zero fill at this boundary
_video_set_palette endp

S012_TEXT ends
end
