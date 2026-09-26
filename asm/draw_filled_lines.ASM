.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _draw_filled_lines
_draw_filled_lines proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_x1arr = word ptr 6
    arg_x2arr = word ptr 8
    arg_y = word ptr 10
    arg_numlines = word ptr 12
    arg_color = word ptr 14

    push    bp
    mov     bp, sp
    sub     sp, 2
    push    si
    push    di
    cmp     [bp+arg_numlines], 0
    jz      short loc_3470C ; if arg6 == 0, return
    cld
    mov     si, [bp+arg_y]
    shl     si, 1
    add     si, word ptr cs:[5F2Ah]
    mov     ax, word ptr word ptr cs:[5F22h]
    mov     es, ax
    mov     ax, [bp+arg_color]
    mov     ah, al
loc_346E0:
    mov     bx, [bp+arg_x2arr]
    mov     cx, [bx]
    mov     bx, [bp+arg_x1arr]
    mov     bx, [bx]
    sub     cx, bx
    inc     cx
    jle     short loc_346FC
    mov     di, cs:[si]     ; si = offset in lineofs table, di = y offset in bitmapptr
    add     di, bx
    test    cx, 0FFF8h
    jnz     short loc_34712 ; jump if (cx & 0xFFF8), ie jump if cx >= 8.
    rep stosb               ; rep count = cx = (arg2->member0 - arg0->member0) + 1
loc_346FC:
    add     [bp+arg_x2arr], 2
    add     [bp+arg_x1arr], 2
    add     si, 2
    dec     [bp+arg_numlines]
    jg      short loc_346E0
loc_3470C:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_34712:
    test    di, 1           ; looks like its figuring out which alignment optimizations to use
    jnz     short loc_34725
    shr     cx, 1
    jb      short loc_34720
    rep stosw
    jmp     short loc_346FC
loc_34720:
    rep stosw
    stosb
    jmp     short loc_346FC
loc_34725:
    shr     cx, 1
    jb      short loc_34730
    dec     cx
    stosb
    rep stosw
    stosb
    jmp     short loc_346FC
loc_34730:
    stosb
    rep stosw
    jmp     short loc_346FC
    ; align 2
    db 0
_draw_filled_lines endp
_TEXT ends
end
