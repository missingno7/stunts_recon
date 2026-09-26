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
extrn _word_4031E:word
extrn _word_40320:word
public _nopsub_33330
public _draw_unknown_lines
sprite1 equ 5F20h
sprite_lineofs equ 0Ah
sprite_bitmapptr equ 0
_nopsub_33330 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_A = word ptr 16
    arg_C = word ptr 18

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_C]
    mov     ds:[_word_4031E], ax
    mov     ax, [bp+arg_A]
    mov     byte ptr ds:[_word_40320], al
    jmp     short loc_33349
    db 144
_nopsub_33330 endp
_draw_unknown_lines proc far
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
loc_33349:
    cld
    mov     si, [bp+arg_4]
    test    si, 1
    jnz     short loc_3335B
    mov     ax, ds:[_word_4031E]
    xchg    ah, al
    mov     ds:[_word_4031E], ax
loc_3335B:
    mov     si, [bp+arg_4]
    shl     si, 1
    add     si, cs:[sprite1+sprite_lineofs]
    mov     ax, word ptr cs:[sprite1+sprite_bitmapptr+2]
    mov     es, ax
    mov     al, [bp+arg_8]
loc_3336E:
    mov     bx, [bp+arg_2]
    mov     dx, [bx]
    mov     bx, [bp+arg_0]
    mov     bx, [bx]
    mov     ah, byte ptr ds:[_word_4031E]
    mov     cl, bl
    and     cl, 7
    rol     ah, cl
    mov     cx, dx
    mov     dl, byte ptr ds:[_word_40320]
    sub     cx, bx
    inc     cx
    jle     short loc_3339A
    mov     di, cs:[si]
    add     di, bx
loc_33393:
    rol     ah, 1
    jb      short loc_333B8
    stosb
    loop    loc_33393
loc_3339A:
    add     [bp+arg_2], 2
    add     [bp+arg_0], 2
    add     si, 2
    mov     dx, ds:[_word_4031E]
    xchg    dh, dl
    mov     ds:[_word_4031E], dx
    dec     [bp+arg_6]
    jg      short loc_3336E
    pop     di
    pop     si
    pop     bp
    retf
loc_333B8:
    mov     es:[di], dl
    inc     di
    loop    loc_33393
    jmp     short loc_3339A
_draw_unknown_lines endp
_TEXT ends
end

