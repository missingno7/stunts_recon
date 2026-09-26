.model medium
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _game_replay_mode:byte
extrn _word_32D02:word
extrn _gameconfig:byte
extrn _byte_449DA:byte
extrn _is_in_replay:byte
extrn _state:byte
extrn _passed_security:byte
extrn _byte_4393C:byte
extrn _framespersec:word
extrn _byte_3B8F2:byte
extrn _byte_3FE00:byte
extrn _mouse_xpos:word
extrn _byte_40D6A:byte
extrn _mouse_butstate:word
extrn _byte_3E85C:byte
extrn _byte_44292:byte
extrn _byte_442EA:byte
extrn _elapsed_time1:word
extrn _word_45D3E:word
extrn _byte_46467:byte
extrn _cvxptr:dword
extrn _td16_rpl_buffer:dword
extrn _byte_442E4:byte
extrn _word_443F4:word
extrn _word_4499C:word
extrn _word_449E4:word
extrn _word_44D1E:word
extrn _byte_44A8A:byte
extrn _word_46468:word
extrn _byte_449E6:byte
extrn _byte_4552F:byte
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT, ds:DSEG
extrn _audio_carstate:far
extrn _compare_ds_ss:far
extrn _sub_18D06:far
extrn _update_crash_state:far
extrn _mouse_get_state:far
extrn _abs:far
extrn _sub_307E3:far
extrn _get_kb_or_joy_flags:far
extrn _kb_get_key_state:far
extrn __aFlmul:far
public _frame_callback, _replay_unk2
_frame_callback proc far

    call    _compare_ds_ss
    or      ax, ax
    jnz     short loc_225A2
    jmp     locret_22696
loc_225A2:
    cmp     _byte_442E4, 0
    jz      short loc_225AC
    jmp     locret_22696
loc_225AC:
    inc     _byte_442E4
    cmp     _byte_442E4, 1
    jz      short loc_225BA
    jmp     loc_22692
loc_225BA:
    inc     _word_443F4
    mov     ax, _word_4499C
    cmp     _word_443F4, ax
    jl      short loc_225FE
    mov     ax, _word_449E4
    cmp     _word_44D1E, ax
    jz      short loc_225FE
    push    _word_443F4
    mov     ax, 22h ; '"'
    imul    _word_44D1E
    add     ax, 97DCh
    push    ax
    call    _sub_18D06
    add     sp, 4
    mov     _word_443F4, 0
    inc     _word_44D1E
    cmp     _word_44D1E, 28h ; '('
    jnz     short loc_225FE
    mov     _word_44D1E, 0
loc_225FE:
    cmp     _byte_449DA, 0
    jz      short loc_22608
    jmp     loc_22692
loc_22608:
    cmp     _byte_46467, 0
    jz      short loc_22612
    jmp     loc_22692
loc_22612:
    cmp     _is_in_replay, 0
    jz      short loc_22620
    cmp     _game_replay_mode, 2
    jz      short loc_22692
loc_22620:
    cmp     _game_replay_mode, 0
    jnz     short loc_2263E
    mov     ax, word ptr _state+13Ah
    cmp     word ptr _state+138h, ax
    jl      short loc_2263E
    mov     _is_in_replay, 1
    call    _audio_carstate
    jmp     short loc_22692
    db 144
    db 144
loc_2263E:
    dec     _byte_44A8A
    jnz     short loc_22692
    mov     al, byte ptr _word_4499C
    mov     _byte_44A8A, al
    inc     _word_46468
    cmp     _game_replay_mode, 2
    jnz     short loc_22688
    mov     al, _byte_449E6
    cbw
    cmp     ax, 2
    jz      short loc_22666
    cmp     ax, 3
    jz      short loc_2267E
    jmp     short loc_22688
    ; align 2
    db 144
loc_22666:
    dec     _byte_4552F
    jnz     short loc_22692
    sub     ax, ax
    push    ax
    push    cs
    call near ptr _replay_unk2
    add     sp, 2
    mov     _byte_4552F, 2
    jmp     short loc_22692
    ; align 2
    db 144
loc_2267E:
    sub     ax, ax
    push    ax
    push    cs
    call near ptr _replay_unk2
    add     sp, 2
loc_22688:
    sub     ax, ax
    push    ax
    push    cs
    call near ptr _replay_unk2
    add     sp, 2
loc_22692:
    dec     _byte_442E4
locret_22696:
    retf
    ; align 2
    db 144
_frame_callback endp

_replay_unk2 proc far
    var_A = word ptr -10
    var_8 = word ptr -8
    var_6 = word ptr -6
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    sub     sp, 0Ah
    push    di
    push    si
    cmp     [bp+arg_0], 0
    jz      short loc_226AC
loc_226A6:
    sub     si, si
    jmp     loc_22817
    ; align 2
    db 144
loc_226AC:
    cmp     _game_replay_mode, 2
    jnz     short loc_226E6
    mov     ax, _word_32D02
    cmp     word ptr _gameconfig+18h, ax
    jbe     short loc_226C6
    inc     _word_32D02
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
loc_226C6:
    cmp     _byte_449DA, 0
    jz      short loc_226D0
    jmp     loc_22985
loc_226D0:
    mov     _is_in_replay, 1
    call    _audio_carstate
loc_226DA:
    mov     _byte_449DA, 1
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
    ; align 2
    db 144
loc_226E6:
    cmp     _byte_449DA, 0
    jnz     short loc_226A6
    cmp     byte ptr _state+3F6h, 0
    jnz     short loc_226A6
    cmp     _game_replay_mode, 1
    jz      short loc_226A6
    cmp     _passed_security, 0
    jnz     short loc_22725
    cmp     _byte_4393C, 0
    jnz     short loc_22725
    mov     ax, _framespersec
    shl     ax, 1
    shl     ax, 1
    cmp     ax, word ptr _state+140h
    jnb     short loc_22725
    sub     ax, ax
    push    ax
    mov     ax, 1
    push    ax
    call    _update_crash_state
    add     sp, 4
loc_22725:
    cmp     _byte_3B8F2, 0
    jnz     short loc_22736
    cmp     _byte_3FE00, 0
    jnz     short loc_22736
    jmp     loc_227E8
loc_22736:
    cmp     _byte_3B8F2, 0
    jz      short loc_2279A
    mov     ax, 0AA5Eh
    push    ax
    mov     ax, 0A9FCh
    push    ax
    mov     ax, 8B78h
    push    ax
    call    _mouse_get_state
    add     sp, 6
    mov     di, _mouse_xpos
    sub     di, 0A0h ; '   '
    push    di              ; int
    call    _abs
    add     sp, 2
    cmp     ax, 12h
    jge     short loc_2276C
    sub     di, di
    jmp     short loc_22779
    ; align 2
    db 144
loc_2276C:
    or      di, di
    jle     short loc_22776
    sub     di, 12h
    jmp     short loc_22779
    ; align 2
    db 144
loc_22776:
    add     di, 12h
loc_22779:
    mov     ax, di
    mov     _byte_40D6A, al
    test    byte ptr _mouse_butstate, 1
    jz      short loc_2278A
    mov     si, 2
    jmp     short loc_227D2
loc_2278A:
    test    byte ptr _mouse_butstate, 2
    jz      short loc_22796
    mov     si, 1
    jmp     short loc_227D2
loc_22796:
    sub     si, si
    jmp     short loc_227D2
loc_2279A:
    call    _sub_307E3
    mov     _byte_40D6A, al
    or      al, al
    jle     short loc_227B0
    cbw
    mov     bx, ax
    mov     al, _byte_3E85C[bx]
    jmp     short loc_227C5
    ; align 2
    db 144
loc_227B0:
    cmp     _byte_40D6A, 0
    jge     short loc_227C8
    mov     al, _byte_40D6A
    cbw
    mov     bx, ax
    neg     bx
    mov     al, _byte_3E85C[bx]
    neg     al
loc_227C5:
    mov     _byte_40D6A, al
loc_227C8:
    call    _get_kb_or_joy_flags
    mov     si, ax
    and     si, 33h
loc_227D2:
    mov     di, _word_32D02
    and     di, 3Fh
    mov     al, _byte_40D6A
    mov     _byte_44292[di], al
    mov     _byte_442EA[di], 1
    jmp     short loc_227EF
    ; align 2
    db 144
loc_227E8:
    call    _get_kb_or_joy_flags
    mov     si, ax
loc_227EF:
    mov     ax, 1Eh
    push    ax
    call    _kb_get_key_state
    add     sp, 2
    or      ax, ax
    jz      short loc_22803
    or      si, WORD PTR 10h
loc_22803:
    mov     ax, 2Ch ; ','
    push    ax
    call    _kb_get_key_state
    add     sp, 2
    or      ax, ax
    jz      short loc_22817
    or      si, WORD PTR 20h
loc_22817:
    mov     ax, 5DCh
    imul    _framespersec
    mov     cx, _word_32D02
    add     cx, _elapsed_time1
    cmp     ax, cx
    ja      short loc_2283C
    sub     ax, ax
    push    ax
    mov     ax, 4
    push    ax
    call    _update_crash_state
    add     sp, 4
    jmp     loc_226DA
loc_2283C:
    cmp     _word_32D02, 2EE0h
    jz      short loc_22847
    jmp     loc_2296C
loc_22847:
    cmp     _elapsed_time1, 0
    jnz     short loc_22866
    cmp     byte ptr _word_45D3E, 0
    jnz     short loc_22866
    mov     byte ptr _word_45D3E, 1
    mov     _byte_46467, 1
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
    ; align 2
    db 144
loc_22866:
    sub     di, di
    jmp     loc_22909
    ; align 2
    db 144
loc_2286C:
    mov     ax, 460h
    cwd
    push    dx
    push    ax
    mov     ax, di
    cwd
    push    dx
    push    ax
    call    __aFlmul
    add     ax, 5A0h
    adc     dx, 0
    add     ax, word ptr _cvxptr
    adc     dx, 0
    mov     cx, 0Ch
    shl     dx, cl
    add     dx, word ptr _cvxptr+2
    mov     es, dx
    mov     bx, ax
    mov     ax, [bp+var_6]
    sub     es:[bx], ax
    mov     ax, 460h
    cwd
    push    dx
    push    ax
    mov     ax, di
    cwd
    push    dx
    push    ax
    call    __aFlmul
    add     ax, 460h
    adc     dx, 0
    add     ax, word ptr _cvxptr
    adc     dx, 0
    mov     cx, 0Ch
    shl     dx, cl
    add     dx, word ptr _cvxptr+2
    mov     cx, 460h
    sub     bx, bx
    push    bx
    push    cx
    mov     cx, ax
    mov     ax, di
    mov     bx, dx
    cwd
    push    dx
    push    ax
    mov     [bp+var_A], cx
    mov     [bp+var_8], bx
    call    __aFlmul
    add     ax, word ptr _cvxptr
    adc     dx, 0
    mov     cx, 0Ch
    shl     dx, cl
    add     dx, word ptr _cvxptr+2
    mov     bx, ax
    mov     es, dx
    mov     ax, [bp+var_A]
    mov     dx, [bp+var_8]
    push    si
    push    di
    mov     di, bx
    mov     si, ax
    push    ds
    mov     ds, dx
    mov     cx, 230h
    repne movsw
    pop     ds
    pop     di
    pop     si
    inc     di
loc_22909:
    mov     ax, 1Eh
    imul    _framespersec
    mov     [bp+var_6], ax
    mov     ax, 2EE0h
    cwd
    mov     cx, [bp+var_6]
    idiv    cx
    dec     ax
    cmp     ax, di
    jle     short loc_22924
    jmp     loc_2286C
loc_22924:
    sub     di, di
    jmp     short loc_2293E
loc_22928:
    mov     bx, [bp+var_A]
    add     bx, word ptr _td16_rpl_buffer
    mov     es, word ptr _td16_rpl_buffer+2
    mov     al, es:[bx+di]
    mov     bx, word ptr _td16_rpl_buffer
    mov     es:[bx+di], al
    inc     di
loc_2293E:
    mov     ax, 1Eh
    imul    _framespersec
    mov     [bp+var_A], ax
    mov     ax, 2EE0h
    sub     ax, [bp+var_A]
    cmp     ax, di
    jg      short loc_22928
    mov     ax, 1Eh
    imul    _framespersec
    mov     [bp+var_A], ax
    sub     _word_32D02, ax
    sub     word ptr _gameconfig+18h, ax
    add     _elapsed_time1, ax
    sub     word ptr _state+140h, ax
loc_2296C:
    mov     bx, _word_32D02
    inc     _word_32D02
    add     bx, word ptr _td16_rpl_buffer
    mov     es, word ptr _td16_rpl_buffer+2
    mov     ax, si
    mov     es:[bx], al
    inc     word ptr _gameconfig+18h
loc_22985:
    pop     si
    pop     di
    mov     sp, bp
    pop     bp
    retf
    ; align 2
    db 144
_replay_unk2 endp
_TEXT ends
end
