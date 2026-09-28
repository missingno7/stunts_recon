.8086
INCLUDE include\platform_hw.inc
name asm012_149180
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT,ds:DGROUP
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
extrn _fontdefseg:word
extrn _loc_35ED9:near
public _draw_filled_lines
public _nopsub_34736
public _sprite_clear_shape_alt
public _sprite_clear_shape
public _shapeexpl
public _sprite1
public _sprite2
public _font_setup_unknown
public _set_fontdefseg
; -----------------------------------------------------------------------------
; Purpose: Draw filled scan lines from the prepared line descriptors.
; ABI: FAR entry; source return form: RET, RETF mnemonic for FAR entry; stack arguments: arg_color (word ptr 14); arg_numlines (word ptr 12); arg_x1arr (word ptr 6); arg_x2arr
;   (word ptr 8); arg_y (word ptr 10).
; Register ABI: touches AX, BX, CX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
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
    jz      short fill_lines_branch_path_01 ; if arg6 == 0, return
    cld
    mov     si, [bp+arg_y]
    shl     si, 1
    add     si, word ptr cs:[_sprite1.sprite_lineofs]
    mov     ax, word ptr cs:[_sprite1.sprite_bitmapseg]
    mov     es, ax
    mov     ax, [bp+arg_color]
    mov     ah, al
fill_lines_loop_head_01:
    mov     bx, [bp+arg_x2arr]
    mov     cx, [bx]
    mov     bx, [bp+arg_x1arr]
    mov     bx, [bx]
    sub     cx, bx
    inc     cx
    jle     short fill_lines_loop_head_02
    mov     di, cs:[si]     ; si = offset in lineofs table, di = y offset in bitmapptr
    add     di, bx
    test    cx, 0FFF8h
    jnz     short fill_lines_branch_path_02 ; jump if (cx & 0xFFF8), ie jump if cx >= 8.
    rep stosb ; rep count = cx = (arg2->member0 - arg0->member0) + 1 /* PLATFORM(video): writes display or sprite memory through ES:DI. */
fill_lines_loop_head_02:
    add     [bp+arg_x2arr], 2
    add     [bp+arg_x1arr], 2
    add     si, 2
    dec     [bp+arg_numlines]
    jg      short fill_lines_loop_head_01
fill_lines_branch_path_01:
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
fill_lines_branch_path_02:
    test    di, 1           ; looks like its figuring out which alignment optimizations to use
    jnz     short fill_lines_branch_path_04
    shr     cx, 1
    jb      short fill_lines_branch_path_03
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    jmp     short fill_lines_loop_head_02
fill_lines_branch_path_03:
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    jmp     short fill_lines_loop_head_02
fill_lines_branch_path_04:
    shr     cx, 1
    jb      short fill_lines_branch_path_05
    dec     cx
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    jmp     short fill_lines_loop_head_02
fill_lines_branch_path_05:
    stosb ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    jmp     short fill_lines_loop_head_02
    ; align 2
    db 0
_draw_filled_lines endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 34736.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8); arg_4 (word ptr 10);
;   arg_6 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_34736 proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     di, [bp+arg_0]
    mov     ax, [bp+arg_4]
    sub     ax, [di+4]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_6]
    sub     ax, [di+6]
    mov     [bp+var_4], ax
    jmp     short shape_clear_join_01
    ; align 2
    db 144
_nopsub_34736 endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite clear shape alt.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: arg_2 (word ptr 8); arg_4 (word ptr 10); arg_6 (word ptr 12);
;   arg_8 (word ptr 12).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sprite_clear_shape_alt proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 6
    arg_4 = word ptr 8
    arg_6 = word ptr 10
    arg_8 = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_4]
    mov     di, [bp+arg_2]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     [di+8], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    mov     [di+0Ah], ax
    jmp     short shape_clear_join_01
    ; align 2
    db 144
_sprite_clear_shape_alt endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by sprite clear shape.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_sprite_clear_shape proc far
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    ds
    push    si
    push    di
    mov     ds, [bp+arg_2]
    mov     di, [bp+arg_0]
    mov     ax, [di+8]
    mov     [bp+var_2], ax
    mov     ax, [di+0Ah]
    mov     [bp+var_4], ax
shape_clear_join_01:
    mov     ax, [di]
    mov     [bp+var_8], ax
    mov     ax, [di+2]
    mov     [bp+var_A], ax
    mov     bx, [bp+var_4]
    shl     bx, 1
    add     bx, word ptr cs:[_sprite1.sprite_lineofs]
    mov     [bp+var_6], bx
    mov     ds, word ptr cs:[_sprite1.sprite_bitmapseg]
    mov     es, [bp+arg_2]
    add     di, 10h
    cld
    mov     dx, [bp+var_A]
shape_clear_loop_head_01:
    mov     bx, [bp+var_6]
    mov     si, cs:[bx]
    add     si, [bp+var_2]
    mov     cx, [bp+var_8]
    rep movsb
    add     [bp+var_6], 2
    dec     dx
    jg      short shape_clear_loop_head_01
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_sprite_clear_shape endp
; -----------------------------------------------------------------------------
; Purpose: Implement the sprite or 2D-shape operation identified by shapeexpl.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_6 (word ptr 12); arg_8 (word ptr 12); arg_shapeptr (dword ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(video): selects a video-memory segment. */
; -----------------------------------------------------------------------------
_shapeexpl proc far
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

    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+arg_shapeptr+2]
    mov     si, word ptr [bp+arg_shapeptr]
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
    jmp     short shape_explode_join_01
    ; align 2
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+arg_shapeptr+2]
    mov     si, word ptr [bp+arg_shapeptr]
    mov     ax, [bp+arg_6]
    mov     [bp+var_2], ax
    mov     ax, [bp+arg_8]
    mov     [bp+var_4], ax
    jmp     short shape_explode_join_01
    ; align 2
    db 144
shape_explode_loop_head_01:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
    push    bp
    mov     bp, sp
    sub     sp, 16h
    push    ds
    push    si
    push    di
    mov     ds, word ptr [bp+arg_shapeptr+2]
    mov     si, word ptr [bp+arg_shapeptr]
    mov     ax, [si+8]
    mov     [bp+var_2], ax
    mov     ax, [si+0Ah]
    mov     [bp+var_4], ax
shape_explode_join_01:
    cmp     [bp+arg_0], 2
    jb      short shape_explode_loop_head_01
    cld
    mov     ax, [si+2]
    mul     [bp+arg_0]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short shape_explode_loop_head_01
    mov     [bp+var_8], ax
    mov     ax, [si+0]
    mov     [bp+var_A], ax
    mul     [bp+arg_0]
    mov     al, ah
    mov     ah, dl
    or      ax, ax
    jz      short shape_explode_loop_head_01
    mov     [bp+var_6], ax
    mov     dx, cs:_sprite1.sprite_pitch
    sub     dx, ax
    mov     [bp+var_C], dx
    add     si, 10h
    mov     dx, 1
    xor     ax, ax
    mov     [bp+var_12], ax
    mov     [bp+var_14], ax
    div     [bp+arg_0]
    mov     [bp+var_10], ax
    mov     al, ah
    xor     ah, ah
    shr     ax, 1
    jz      short shape_explode_branch_path_01
    add     si, ax
    mov     cx, ax
shape_explode_loop_head_02:
    add     si, [bp+var_A]
    loop    shape_explode_loop_head_02
shape_explode_branch_path_01:
    mov     cx, [bp+var_6]
    mov     ax, [bp+var_2]
    mov     bx, cs:_sprite1.sprite_left2
    cmp     ax, bx
    jge     short shape_explode_branch_path_02
    add     ax, cx
    sub     ax, bx
    jle     short shape_explode_branch_path_06
    mov     [bp+var_2], bx
    mov     [bp+var_6], ax
    xchg    ax, cx
    sub     ax, cx
    mul     [bp+var_10]
    xchg    al, dl
    xor     dh, dh
    mov     [bp+var_14], dx
    xchg    al, ah
    add     si, ax
    mov     ax, bx
shape_explode_branch_path_02:
    add     ax, cx
    sub     ax, cs:_sprite1.sprite_widthsum
    jl      short shape_explode_branch_path_03
    sub     cx, ax
    jle     short shape_explode_branch_path_06
    mov     [bp+var_6], cx
shape_explode_branch_path_03:
    mov     ax, cs:_sprite1.sprite_pitch
    sub     ax, cx
    mov     [bp+var_C], ax
    mov     cx, [bp+var_8]
    mov     ax, [bp+var_4]
    mov     bx, cs:_sprite1.sprite_top
    cmp     ax, bx
    jge     short shape_explode_branch_path_04
    add     ax, cx
    sub     ax, bx
    jle     short shape_explode_branch_path_06
    mov     [bp+var_8], ax
    mov     [bp+var_4], bx
    xchg    ax, cx
    sub     ax, cx
    mul     [bp+var_10]
    xchg    al, dl
    xor     dh, dh
    mov     [bp+var_12], dx
    xchg    al, ah
    mul     [bp+var_A]
    add     si, ax
    mov     ax, bx
shape_explode_branch_path_04:
    add     ax, cx
    sub     ax, cs:_sprite1.sprite_height
    jl      short shape_explode_branch_path_05
    sub     cx, ax
    jle     short shape_explode_branch_path_06
    mov     [bp+var_8], cx
shape_explode_branch_path_05:
    mov     dx, [bp+var_10]
    mov     [bp+var_E], si
    jmp near ptr _loc_35ED9
shape_explode_branch_path_06:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_sprite1 dw 0,0A000h,0,0,0,offset lineoffsets,0,0140h,0,00C8h,0140h,0,0140h,0,0140h ;/* PLATFORM(video): selects a video-memory segment. */
_sprite2 dw 0,0A000h,0,0,0,offset lineoffsets,0,0140h,0,00C8h,0140h,0,0140h,0,0140h ;/* PLATFORM(video): selects a video-memory segment. */
lineoffsets     dw 0
    dw 320
    dw 640
    dw 960
    dw 1280
    dw 1600
    dw 1920
    dw 2240
    dw 2560
    dw 2880
    dw 3200
    dw 3520
    dw 3840
    dw 4160
    dw 4480
    dw 4800
    dw 5120
    dw 5440
    dw 5760
    dw 6080
    dw 6400
    dw 6720
    dw 7040
    dw 7360
    dw 7680
    dw 8000
    dw 8320
    dw 8640
    dw 8960
    dw 9280
    dw 9600
    dw 9920
    dw 10240
    dw 10560
    dw 10880
    dw 11200
    dw 11520
    dw 11840
    dw 12160
    dw 12480
    dw 12800
    dw 13120
    dw 13440
    dw 13760
    dw 14080
    dw 14400
    dw 14720
    dw 15040
    dw 15360
    dw 15680
    dw 16000
    dw 16320
    dw 16640
    dw 16960
    dw 17280
    dw 17600
    dw 17920
    dw 18240
    dw 18560
    dw 18880
    dw 19200
    dw 19520
    dw 19840
    dw 20160
    dw 20480
    dw 20800
    dw 21120
    dw 21440
    dw 21760
    dw 22080
    dw 22400
    dw 22720
    dw 23040
    dw 23360
    dw 23680
    dw 24000
    dw 24320
    dw 24640
    dw 24960
    dw 25280
    dw 25600
    dw 25920
    dw 26240
    dw 26560
    dw 26880
    dw 27200
    dw 27520
    dw 27840
    dw 28160
    dw 28480
    dw 28800
    dw 29120
    dw 29440
    dw 29760
    dw 30080
    dw 30400
    dw 30720
    dw 31040
    dw 31360
    dw 31680
    dw 32000
    dw 32320
    dw 32640
    dw 32960
    dw 33280
    dw 33600
    dw 33920
    dw 34240
    dw 34560
    dw 34880
    dw 35200
    dw 35520
    dw 35840
    dw 36160
    dw 36480
    dw 36800
    dw 37120
    dw 37440
    dw 37760
    dw 38080
    dw 38400
    dw 38720
    dw 39040
    dw 39360
    dw 39680
    dw 40000
    dw 40320
    dw 40640
    dw 40960
    dw 41280
    dw 41600
    dw 41920
    dw 42240
    dw 42560
    dw 42880
    dw 43200
    dw 43520
    dw 43840
    dw 44160
    dw 44480
    dw 44800
    dw 45120
    dw 45440
    dw 45760
    dw 46080
    dw 46400
    dw 46720
    dw 47040
    dw 47360
    dw 47680
    dw 48000
    dw 48320
    dw 48640
    dw 48960
    dw 49280
    dw 49600
    dw 49920
    dw 50240
    dw 50560
    dw 50880
    dw 51200
    dw 51520
    dw 51840
    dw 52160
    dw 52480
    dw 52800
    dw 53120
    dw 53440
    dw 53760
    dw 54080
    dw 54400
    dw 54720
    dw 55040
    dw 55360
    dw 55680
    dw 56000
    dw 56320
    dw 56640
    dw 56960
    dw 57280
    dw 57600
    dw 57920
    dw 58240
    dw 58560
    dw 58880
    dw 59200
    dw 59520
    dw 59840
    dw 60160
    dw 60480
    dw 60800
    dw 61120
    dw 61440
    dw 61760
    dw 62080
    dw 62400
    dw 62720
    dw 63040
    dw 63360
    dw 63680
_shapeexpl endp
; -----------------------------------------------------------------------------
; Purpose: Implement the graphics or geometry helper identified by font setup unknown.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, CX, SI, DI, BP, SP, DS, ES; balanced explicit saves/restores observed: DI, DS, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _fontdefseg.
; Globals written: _fontdefseg.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
_font_setup_unknown proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     es, ds:[_fontdefseg]
    mov     ax, [bp+arg_2]
    xor     ah, ah
    mov     es:2, ax
    mov     ax, [bp+arg_0]
    xor     ah, ah
    mov     es:0, ax
    pop     bp
    retf
    push    bp
    mov     bp, sp
    mov     es, ds:[_fontdefseg]
    mov     ax, [bp+6]
    mov     es:8, ax
    mov     ax, [bp+8]
    mov     es:0Ah, ax
    pop     bp
    retf
    push    bp
    mov     bp, sp
    push    ds
    push    di
    push    si
    xor     si, si
    mov     di, [bp+6]
    mov     ax, ds
    mov     es, ax
    mov     ds, ds:[_fontdefseg]
    cld
    mov     word ptr es:[di+16h], ds ;/* PLATFORM(video): accesses display memory through ES. */
    mov     cx, 0Bh
    rep movsw
    pop     si
    pop     di
    pop     ds
    pop     bp
    retf
    push    bp
    mov     bp, sp
    push    di
    push    si
    mov     si, [bp+6]
    mov     ax, [si+16h]
    mov     ds:[_fontdefseg], ax
    mov     es, ax
    xor     di, di
    cld
    mov     cx, 0Bh
    rep movsw
    pop     si
    pop     di
    pop     bp
    retf
_font_setup_unknown endp
; -----------------------------------------------------------------------------
; Purpose: Implement the video or display setup operation identified by set fontdefseg.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_2 (word ptr 8).
; Register ABI: touches AX, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _fontdefseg.
; -----------------------------------------------------------------------------
_set_fontdefseg proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_2]
    mov     ds:[_fontdefseg], ax
    pop     bp
    retf
    ; align 2
    db 0
_set_fontdefseg endp
S012_TEXT ends
end
