.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _sprite1_unknown2
public _sprite_1_unk
public _sprite_1_unk3
; -----------------------------------------------------------------------------
; Purpose: Apply the legacy sprite-1 operation exposed at this entry.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_2 (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, BX, DX, SI, DI, BP, SP, CS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sprite1_unknown2 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 6
    arg_4 = word ptr 8
    arg_6 = word ptr 10
    arg_8 = word ptr 12

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     dx, word ptr cs:[_sprite1+12]
    mov     ax, dx
    sub     ax, [bp+arg_2]
    jle     short unknown2_when_less_or_equal
    mov     [bp+arg_2], dx
    sub     [bp+arg_6], ax
    jle     short sprite1_unknown2_draw_loop
unknown2_when_less_or_equal:
    mov     ax, [bp+arg_2]
    add     ax, [bp+arg_6]
    sub     ax, word ptr cs:[_sprite1+14]
    jle     short unknown2_when_less_or_equal_2
    sub     [bp+arg_6], ax
    jle     short sprite1_unknown2_draw_loop
unknown2_when_less_or_equal_2:
    mov     ax, word ptr cs:[_sprite1+16]
    sub     ax, [bp+arg_4]
    jle     short unknown2_when_less_or_equal_3
    sub     [bp+arg_8], ax
    jle     short sprite1_unknown2_draw_loop
    mov     ax, word ptr cs:[_sprite1+16]
    mov     [bp+arg_4], ax
unknown2_when_less_or_equal_3:
    mov     ax, [bp+arg_4]
    add     ax, [bp+arg_8]
    mov     bx, word ptr cs:[_sprite1+18]
    sub     ax, bx
    jle     short sprite_1_unk_shared_path
    sub     [bp+arg_8], ax
    jle     short sprite1_unknown2_draw_loop
    jmp     short sprite_1_unk_shared_path
    db 144
sprite1_unknown2_draw_loop:
    jmp     short sprite_1_unk_draw_loop
    ; align 2
    db 144
_sprite1_unknown2 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite 1 unk.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 6); arg_4 (word ptr 8); arg_6 (word ptr 10); arg_8
;   (word ptr 12).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the
;   listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_sprite_1_unk proc far
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
sprite_1_unk_shared_path:
    cld
    cmp     [bp+arg_4], 0
    jle     short sprite1_unknown2_draw_loop
    cmp     [bp+arg_6], 0
    jle     short sprite1_unknown2_draw_loop
    mov     es, word ptr word ptr cs:[_sprite1+2]
    mov     al, [bp+arg_8]
    mov     ah, al
    mov     dx, [bp+arg_0]
    mov     si, [bp+arg_6]
    mov     bx, [bp+arg_2]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1+10]
    mov     di, cs:[bx]
    add     di, [bp+arg_0]
    mov     dx, word ptr cs:[_sprite1+20]
    sub     dx, [bp+arg_4]
    mov     cx, [bp+arg_4]
    sar     cx, 1
    mov     [bp+arg_4], cx
    jb      short sprite_1_unk_when_below
    jle     short sprite_1_unk_draw_loop
sprite_1_unk_fill_loop:
    mov     cx, [bp+arg_4]
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    add     di, dx
    dec     si
    jg      short sprite_1_unk_fill_loop
sprite_1_unk_draw_loop:
    pop     di
    pop     si
    pop     bp
    retf
sprite_1_unk_when_below:
    jz      short sprite_1_unk_when_zero
    jl      short sprite_1_unk_draw_loop
    inc     dx
sprite_1_unk_fill_loop_2:
    mov     cx, [bp+arg_4]
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, dx
    dec     si
    jg      short sprite_1_unk_fill_loop_2
    jmp     short sprite_1_unk_draw_loop
sprite_1_unk_when_zero:
    inc     dx
sprite_1_unk_draw_loop_2:
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    add     di, dx
    dec     si
    jg      short sprite_1_unk_draw_loop_2
    jmp     short sprite_1_unk_draw_loop
    ; align 2
    db 0
sprite_scale_pattern_by_row     db 11
    db 5
    db 8
    db 2
    db 10
    db 4
    db 7
    db 1
    db 9
    db 3
    db 6
    db 0
sprite_run_pre_skip_by_phase     db 1
    db 3
    db 0
    db 2
sprite_run_post_skip_by_phase     db 3
    db 1
    db 4
    db 2
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     es, word ptr word ptr cs:[_sprite1+2]
    mov     ax, [bp+0Ch]
    mov     [bp-2], ax
    mov     ax, [bp+0Eh]
    jmp     short sprite_1_unk3_when_condition
    ; align 2
    db 144
_sprite_1_unk endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite 1 unk3.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 6); arg_4 (word ptr 8).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_sprite_1_unk3 proc far
    var_16 = word ptr -22
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

    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     es, word ptr word ptr cs:[_sprite1+2]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
sprite_1_unk3_when_condition:
    shl     ax, 1
    add     ax, word ptr cs:[_sprite1+10]
    mov     [bp+var_4], ax
    add     ax, [si+2]
    add     ax, [si+2]
    mov     [bp+var_6], ax
    mov     ax, [si]
    mov     [bp+var_8], ax
    mov     bx, 0Ch
    mul     bx
    mov     [bp+var_A], ax
    add     si, 10h
    mov     [bp+var_C], si
    mov     [bp+var_E], 0Bh
sprite_1_unk3_draw_loop:
    mov     di, [bp+var_E]
    mov     cl, cs:sprite_scale_pattern_by_row[di]
    xor     ch, ch
    mov     ax, [bp+var_8]
    mul     cx
    mov     [bp+var_10], ax
    shl     cx, 1
    add     cx, [bp+var_4]
    mov     [bp+var_12], cx
    mov     si, [bp+var_C]
    add     si, [bp+var_10]
    mov     bx, [bp+arg_4]
    mov     [bp+var_16], bx
sprite_1_unk3_draw_loop_2:
    mov     di, [bp+var_12]
    cmp     di, [bp+var_6]
    jnb     short sprite_1_unk3_when_not_below
    mov     di, cs:[di]
    add     di, [bp+var_2]
    mov     cx, [bp+var_8]
    mov     [bp+var_14], si
    xor     ah, ah
    mov     bx, [bp+var_16]
sprite_1_unk3_draw_loop_3:
    and     bx, 3
    mov     al, cs:sprite_run_pre_skip_by_phase[bx]
    sub     cx, ax
    jle     short unk3_when_less_or_equal
    add     si, ax
    add     di, ax
    mov     al, [si]
    mov     es:[di], al ;/* PLATFORM(video): accesses display memory through ES. */
    mov     al, cs:sprite_run_post_skip_by_phase[bx]
    add     si, ax
    add     di, ax
    sub     cx, ax
    inc     bx
    jmp     short sprite_1_unk3_draw_loop_3
unk3_when_less_or_equal:
    inc     [bp+var_16]
    add     [bp+var_12], 18h
    mov     si, [bp+var_14]
    add     si, [bp+var_A]
    jmp     short sprite_1_unk3_draw_loop_2
sprite_1_unk3_when_not_below:
    inc     [bp+arg_4]
    dec     [bp+var_E]
    jge     short sprite_1_unk3_draw_loop
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_sprite_1_unk3 endp
S012_TEXT ends
end
