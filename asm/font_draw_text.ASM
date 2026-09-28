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
assume cs:S012_TEXT,ds:DGROUP
extrn _fontdefseg:word
extrn _sprite1:byte
public _font_draw_text
SPRITE1 struc
sprite_bitmapptr dw ?
sprite_bitmapseg dw ?
sprite_field4 dw ?
sprite_field6 dw ?
sprite_field8 dw ?
sprite_lineofs dw ?
sprite_fieldC dw ?
sprite_fieldE dw ?
sprite_field10 dw ?
sprite_field12 dw ?
sprite_field14 dw ?
sprite_field16 dw ?
sprite_field18 dw ?
sprite_field1A dw ?
sprite_field1C dw ?
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
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, CS, DS, ES, SS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _fontdefseg, _sprite1, fh_advance, fh_bgcolor, fh_fgcolor, fh_glyphs, fh_height, fh_left, fh_lineheight, fh_proportional, fh_rowbytes, fh_x, fh_y.
; Globals written: fh_advance, fh_rowbytes, fh_x, fh_y.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_font_draw_text proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    push bp
    mov bp, sp
    sub sp, 4
    push ds
    push si
    push di
    mov ds, ds:[_fontdefseg]
    mov ax, [bp+arg_2]
    mov ds:[fh_x], ax
    mov ax, [bp+arg_4]
    mov ds:[fh_y], ax
    jmp short font_text_when_condition
    db 90h
    push bp
    mov bp, sp
    sub sp, 4
    push ds
    push si
    push di
    mov ds, ds:[_fontdefseg]
font_text_when_condition:
    cld
    mov es, word ptr cs:[_sprite1+SPRITE1.sprite_bitmapptr+2]
font_text_draw_loop:
    mov si, [bp+arg_0]
    mov bl, ss:[si]
    cmp bl, 0
    jz short font_text_return_if_zero
    mov al, bl
    inc [bp+arg_0]
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
font_text_when_nonzero:
    mov ax, ds:[fh_x]
    mov [bp+var_2], ax
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
    mov byte ptr ds:[fh_rowbytes], al
font_text_when_zero_2:
    mov al, byte ptr ds:[fh_fgcolor]
    mov ah, byte ptr ds:[fh_bgcolor]
    mov cx, ds:[fh_height]
    mov [bp+var_4], cx
    mov bx, ds:[fh_y]
    shl bx, 1
    add bx, cs:[_sprite1+SPRITE1.sprite_lineofs]
font_text_draw_loop_2:
    mov di, cs:[bx]
    add di, [bp+var_2]
    mov dh, byte ptr ds:[fh_rowbytes]
font_text_draw_loop_3:
    mov cx, 8
    mov dl, [si]
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
    dec [bp+var_4]
    jg short font_text_draw_loop_2
    mov ax, ds:[fh_advance]
    add ds:[fh_x], ax
    jmp font_text_draw_loop
_font_draw_text endp
S012_TEXT ends
end
