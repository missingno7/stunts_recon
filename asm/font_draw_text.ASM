.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
_TEXT segment word public 'CODE'
assume cs:_TEXT,ds:DGROUP
extrn _fontdefseg:word
extrn _word_3B770:word
extrn _word_3B772:word
extrn _word_3B774:word
extrn _aMsRunTimeLibraryCop:byte
extrn _data_2B77A:byte
extrn _data_2B77C:byte
extrn _word_2B77E:word
extrn _word_2B780:word
extrn _word_2B782:word
extrn _data_2B784:byte
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
sprite1_base equ 5F20h
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
    mov ds:8, ax
    mov ax, [bp+arg_4]
    mov ds:0Ah, ax
    jmp short loc_3376B
    db 90h
    push bp
    mov bp, sp
    sub sp, 4
    push ds
    push si
    push di
    mov ds, ds:[_fontdefseg]
loc_3376B:
    cld
    mov es, word ptr cs:[sprite1_base+SPRITE1.sprite_bitmapptr+2]
loc_33771:
    mov si, [bp+arg_0]
    mov bl, ss:[si]
    cmp bl, 0
    jz short loc_337A6
    mov al, bl
    inc [bp+arg_0]
    xor bh, bh
    shl bx, 1
    add bx, 16h
    mov si, [bx]
    cmp si, 0
    jnz short loc_337AD
    cmp al, 0Dh
    jz short loc_33797
    cmp al, 0Ah
    jnz short loc_33771
loc_33797:
    mov ax, ds:[_word_3B774]
    mov ds:[_aMsRunTimeLibraryCop], ax
    mov ax, ds:[_word_2B782]
    add ds:[_aMsRunTimeLibraryCop+2], ax
    jmp short loc_33771
loc_337A6:
    pop di
    pop si
    pop ds
    mov sp, bp
    pop bp
    retf
loc_337AD:
    mov ax, ds:[_aMsRunTimeLibraryCop]
    mov [bp+var_2], ax
    cmp byte ptr ds:[_data_2B784], 0
    jz short loc_337CE
    mov al, [si]
    xor ah, ah
    inc si
    mov ds:[_word_2B780], ax
    add ax, 7
    shr ax, 1
    shr ax, 1
    shr ax, 1
    mov byte ptr ds:[_data_2B77C], al
loc_337CE:
    mov al, byte ptr ds:[_word_3B770]
    mov ah, byte ptr ds:[_word_3B772]
    mov cx, ds:[_word_2B77E]
    mov [bp+var_4], cx
    mov bx, ds:[_data_2B77A]
    shl bx, 1
    add bx, cs:[sprite1_base+SPRITE1.sprite_lineofs]
loc_337E7:
    mov di, cs:[bx]
    add di, [bp+var_2]
    mov dh, byte ptr ds:[_data_2B77C]
loc_337F1:
    mov cx, 8
    mov dl, [si]
loc_337F6:
    shl dl, 1
    jnb short loc_337FD
    mov es:[di], al
loc_337FD:
    inc di
    loop loc_337F6
    inc si
    dec dh
    jg short loc_337F1
    inc bx
    inc bx
    dec [bp+var_4]
    jg short loc_337E7
    mov ax, ds:[_word_2B780]
    add ds:[_aMsRunTimeLibraryCop], ax
    jmp loc_33771
_font_draw_text endp
_TEXT ends
end
