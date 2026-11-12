; ASMDOC(font_shapes) Module load_23742. Current module name accurately names its one public transparent glyph
; renderer;
; ASMDOC(font_shapes) retain its alternate internal prologue too. Public callable entries: _font_draw_text.
; ASMDOC(font_shapes) Contract offsets use BP after the standard prologue; FAR return IP/CS occupy BP+2/+4.
; Original source
; ASMDOC(font_shapes) comments remain verbatim and may contain older incomplete summaries; these added contracts
; describe
; ASMDOC(font_shapes) transitive shared exits.
.8086
INCLUDE include\platform_hw.inc
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
extrn _fontdefseg:word
extrn _sprite1:byte
public _fortextxy
SPRITE1 struc
sprite_bitmapptr dw ?
sprite_bitmapseg dw ?
sprite_field4 dw ?
sprite_field6 dw ?
sprite_field8 dw ?
sprite_lineofs dw ?
sprite_left dw ?
sprite_right dw ?
sprite_top dw ?
sprite_bottom dw ?
sprite_pitch dw ?
sprite_field16 dw ?
sprite_width2 dw ?
sprite_left2 dw ?
sprite_widthsum dw ?
SPRITE1 ends
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
; -----------------------------------------------------------------------------
; Purpose: Draw a string by looking up and rendering each glyph.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8); arg_4 (word ptr 10).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, CS, DS, ES, SS; balanced explicit saves/restores observed:
; none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _fontdefseg, _sprite1, fh_advance, fh_bgcolor, fh_fgcolor, fh_glyphs, fh_height, fh_left,
; fh_lineheight, fh_proportional, fh_rowbytes, fh_x, fh_y.
; Globals written: fh_advance, fh_rowbytes, fh_x, fh_y.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC(font_shapes) Contract _font_draw_text (proposed _font_draw_text): Draw NUL-terminated legacy-byte glyphs
; ASMDOC(font_shapes) transparently into active sprite1 at explicit coordinates.
; ASMDOC(font_shapes) FAR cdecl; RETF, caller removes arguments. Stack offsets below are from BP after PUSH BP /
; MOV BP,SP:
; ASMDOC(font_shapes) saved BP +0, return IP +2, return CS +4; first argument +6. Balanced SP on return.
; ASMDOC(font_shapes) inputs: [BP+6] near text offset interpreted through SS (not current DS); this argument
; word is advanced
; ASMDOC(font_shapes) in place; [BP+8] u16 initial x; [BP+10] u16 initial y; Incoming DS addresses DGROUP so
; _fontdefseg can
; ASMDOC(font_shapes) be loaded; source font thereafter addressed by that segment; Font header: +0 fg low byte,
; +2 bg (read
; ASMDOC(font_shapes) but not written to pixels), +4 line-left x, +8/+0A cursor, +0C rowbytes, +0E height, +10
; advance, +12
; ASMDOC(font_shapes) lineheight, +14 proportional byte, +16 glyph offsets
; ASMDOC(font_shapes) outputs: Active sprite1 bitmap receives foreground for set glyph bits only; clear bits
; leave destination
; ASMDOC(font_shapes) untouched; Font cursor x,y updated; unknown glyph skipped; absent CR or LF resets x to
; font +4 and
; ASMDOC(font_shapes) increments y by font +12; Proportional glyph reads width byte, updates font advance and
; ceil(width/8)
; ASMDOC(font_shapes) rowbytes, then paints every bit of all row bytes (including padding)
; ASMDOC(font_shapes) preserves: BP; SI; DI; DS; CS; SS; SP (balanced)
; ASMDOC(font_shapes) clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; ASMDOC(font_shapes) globals_read: _fontdefseg; CS:_sprite1+2 bitmap segment, CS:_sprite1+10 line-table offset and
; ASMDOC(font_shapes) CS:line_table[2*y]; selected font header/glyphs; SS:text bytes
; ASMDOC(font_shapes) globals_written: selected font +8/+0A cursor, +10 advance and +0C rowbytes for
; proportional glyphs;
; ASMDOC(font_shapes) SS:[BP+6] text pointer argument incremented until NUL; selected sprite1 bitmap segment
; indexed by line
; ASMDOC(font_shapes) offset+x
; ASMDOC(font_shapes) services: none in this routine.
; ASMDOC(font_shapes) Reachability: Retained accepted-source caller or address reference; see reachability_evidence.
; ASMDOC(font_shapes) No clipping or background store. Bitmap base-offset word is not read; line-table entries supply
; ASMDOC(font_shapes) destination offsets.
; ASMDOC(font_shapes) Rows/rowbytes loops are entered before decrement checks; positive height and rowbytes are
; ASMDOC(font_shapes) well-formed-input requirements. DH rowbytes uses signed JG after decrement.
; ASMDOC(font_shapes) Unlabelled alternate prologue at original line 77 skips x/y initialization and continues
; the stored font
; ASMDOC(font_shapes) cursor; not a public export.
_fortextxy proc far
    local_rows_left = word ptr -4
    local_glyph_x = word ptr -2
    arg_text_offset = word ptr 6
    arg_initial_x = word ptr 8
    arg_initial_y = word ptr 10
    push bp
    mov bp, sp
    sub sp, 4
    push ds
    push si
    push di
    mov ds, ds:[_fontdefseg]
    mov ax, [bp+arg_initial_x]
    mov ds:[fh_x], ax
    mov ax, [bp+arg_initial_y]
    mov ds:[fh_y], ax
    jmp short font_text_when_condition
    even ; word-align the following entry
; ASMDOC(font_shapes) Unlabelled continuation FAR-style prologue: text at BP+6, cursor retained from font
; header; standard
; ASMDOC(font_shapes) DS/SI/DI saves and shared transparent core. The normal entry jumps over it.
    push bp
    mov bp, sp
    sub sp, 4
    push ds
    push si
    push di
    mov ds, ds:[_fontdefseg]
; ASMDOC(font_shapes) Shared continuation starts here. The earlier unlabeled prologue preserves existing cursor
; x,y instead of
; ASMDOC(font_shapes) assigning explicit arguments.
font_text_when_condition:
    cld
    mov es, word ptr cs:[_sprite1+SPRITE1.sprite_bitmapptr+2]
font_text_draw_loop:
    mov si, [bp+arg_text_offset]
    mov bl, ss:[si]
    cmp bl, 0
    jz short font_text_return_if_zero
    mov al, bl
    inc [bp+arg_text_offset]
    xor bh, bh
    shl bx, 1
    add bx, fh_glyphs
    mov si, [bx]
    cmp si, 0
    jnz short font_text_when_nonzero
    cmp al, 0Dh
    jz short font_text_when_zero
    cmp al, 0Ah
    jnz short font_text_draw_loop
font_text_when_zero:
    mov ax, ds:[fh_left]
    mov ds:[fh_x], ax
    mov ax, ds:[fh_lineheight]
    add ds:[fh_y], ax
    jmp short font_text_draw_loop
font_text_return_if_zero:
    pop di
    pop si
    pop ds
    mov sp, bp
    pop bp
    retf
; ASMDOC(font_shapes) Glyph offset is nonzero: optionally consume proportional width, then draw full MSB-first
; row bytes
; ASMDOC(font_shapes) transparently.
font_text_when_nonzero:
    mov ax, ds:[fh_x]
    mov [bp+local_glyph_x], ax
    cmp byte ptr ds:[fh_proportional], 0
    jz short font_text_when_zero_2
    mov al, [si]
    xor ah, ah
    inc si
    mov ds:[fh_advance], ax
    add ax, 7
    shr ax, 1
    shr ax, 1
    shr ax, 1
    mov ds:[fh_rowbytes], al
font_text_when_zero_2:
    mov al, byte ptr ds:[fh_fgcolor]
    mov ah, byte ptr ds:[fh_bgcolor]
    mov cx, ds:[fh_height]
    mov [bp+local_rows_left], cx
    mov bx, ds:[fh_y]
    shl bx, 1
    add bx, cs:[_sprite1+SPRITE1.sprite_lineofs]
font_text_draw_loop_2:
    mov di, cs:[bx]
    add di, [bp+local_glyph_x]
    mov dh, ds:[fh_rowbytes]
font_text_draw_loop_3:
    mov cx, 8
    mov dl, [si]
; ASMDOC(font_shapes) Set glyph bits overwrite foreground; clear bits only advance DI. AH background is loaded
; but never
; ASMDOC(font_shapes) stored.
font_text_draw_loop_4:
    shl dl, 1
    jnb short font_text_when_not_below
    mov es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
font_text_when_not_below:
    inc di
    loop font_text_draw_loop_4
    inc si
    dec dh
    jg short font_text_draw_loop_3
    inc bx
    inc bx
    dec [bp+local_rows_left]
    jg short font_text_draw_loop_2
    mov ax, ds:[fh_advance]
    add ds:[fh_x], ax
    jmp font_text_draw_loop
_fortextxy endp
S012_TEXT ends
end
