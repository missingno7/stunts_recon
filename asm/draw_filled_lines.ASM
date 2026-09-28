.8086
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
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
    jz      short filled_lines_return_if_zero ; if arg6 == 0, return
    cld
    mov     si, [bp+arg_y]
    shl     si, 1
    add     si, word ptr cs:[_sprite1+10]
    mov     ax, word ptr word ptr cs:[_sprite1+2]
    mov     es, ax
    mov     ax, [bp+arg_color]
    mov     ah, al
filled_lines_draw_loop:
    mov     bx, [bp+arg_x2arr]
    mov     cx, [bx]
    mov     bx, [bp+arg_x1arr]
    mov     bx, [bx]
    sub     cx, bx
    inc     cx
    jle     short filled_lines_draw_loop_2
    mov     di, cs:[si]     ; si = offset in lineofs table, di = y offset in bitmapptr
    add     di, bx
    test    cx, 0FFF8h
    jnz     short filled_lines_when_nonzero ; jump if (cx & 0xFFF8), ie jump if cx >= 8.
    rep stosb               ; rep count = cx = (arg2->member0 - arg0->member0) + 1
filled_lines_draw_loop_2:
    add     [bp+arg_x2arr], 2
    add     [bp+arg_x1arr], 2
    add     si, 2
    dec     [bp+arg_numlines]
    jg      short filled_lines_draw_loop
filled_lines_return_if_zero:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
filled_lines_when_nonzero:
    test    di, 1           ; looks like its figuring out which alignment optimizations to use
    jnz     short filled_lines_when_nonzero_2
    shr     cx, 1
    jb      short filled_lines_when_below
    rep stosw
    jmp     short filled_lines_draw_loop_2
filled_lines_when_below:
    rep stosw
    stosb
    jmp     short filled_lines_draw_loop_2
filled_lines_when_nonzero_2:
    shr     cx, 1
    jb      short filled_lines_when_below_2
    dec     cx
    stosb
    rep stosw
    stosb
    jmp     short filled_lines_draw_loop_2
filled_lines_when_below_2:
    stosb
    rep stosw
    jmp     short filled_lines_draw_loop_2
    ; align 2
    db 0
_draw_filled_lines endp
S012_TEXT ends
end
