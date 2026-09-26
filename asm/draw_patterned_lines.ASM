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
public _draw_patterned_lines
_draw_patterned_lines proc far
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
loc_34B9B:
    cld
    mov     si, [bp+arg_4]
    test    si, 1
    jnz     short loc_34BAD
    mov     ax, ds:[_word_4031E]
    xchg    ah, al
    mov     ds:[_word_4031E], ax
loc_34BAD:
    mov     si, [bp+arg_4]
    shl     si, 1
    add     si, word ptr cs:[5F2Ah]
    mov     ax, word ptr word ptr cs:[5F22h]
    mov     es, ax
    mov     al, [bp+arg_8]
loc_34BC0:
    mov     bx, [bp+arg_2]
    mov     dx, [bx]
    mov     bx, [bp+arg_0]
    mov     bx, [bx]
    mov     ah, byte ptr ds:[_word_4031E]
    mov     cl, bl
    and     cl, 7
    rol     ah, cl
    mov     cx, dx
    sub     cx, bx
    inc     cx
    jle     short loc_34BE8
    mov     di, cs:[si]
    add     di, bx
loc_34BE1:
    rol     ah, 1
    jnb     short loc_34C06
    stosb
    loop    loc_34BE1
loc_34BE8:
    add     [bp+arg_2], 2
    add     [bp+arg_0], 2
    add     si, 2
    mov     dx, ds:[_word_4031E]
    xchg    dh, dl
    mov     ds:[_word_4031E], dx
    dec     [bp+arg_6]
    jg      short loc_34BC0
    pop     di
    pop     si
    pop     bp
    retf
loc_34C06:
    inc     di
    loop    loc_34BE1
    jmp     short loc_34BE8
    ; align 2
    db 0
_draw_patterned_lines endp
_TEXT ends
end
