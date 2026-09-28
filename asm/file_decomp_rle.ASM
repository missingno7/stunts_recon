.8086
.8086
extrn _copy_paras_reverse:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _file_decomp_rle, _file_decomp_rle_single, _file_decomp_rle_seq
_file_decomp_rle proc far
    var_lenlo = word ptr -26
    var_lenhi = word ptr -24
    var_16 = word ptr -22
    var_14 = word ptr -20
    var_esclen = word ptr -18
     s = byte ptr 0
     r = byte ptr 2
    arg_srcoff = word ptr 6
    arg_srcseg = word ptr 8
    arg_dstoff = word ptr 12
    arg_dstseg = word ptr 14

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    sub     sp, 116h
    cld
    mov     ax, ds
    mov     es, ax
    mov     si, [bp+arg_srcoff]
    mov     ds, [bp+arg_srcseg]
    mov     ax, [si+1]
    mov     [bp+var_lenlo], ax
    mov     al, [si+3]
    xor     ah, ah
    mov     [bp+var_lenhi], ax
    add     si, 4           ; Skip header + unused byte
    mov     cx, 8
    lea     di, [bp+var_16] ; Read 8 words
    rep movsw
    mov     si, [bp+arg_srcoff]; Reset source pos
    add     si, 9
    mov     dx, [bp+var_esclen]

    and     dx, 7Fh         ; Escape codes length mask

    add     si, dx
    mov     [bp+arg_srcoff], si; Skip escape codes
    cmp     byte ptr [bp+var_esclen], 80h ; ''; Skip seq pass flag
    ja      short skip_seq_pass
    call near ptr _file_decomp_rle_seq
    mov     [bp+var_16], ax
    mov     [bp+var_14], dx
    mov     si, ax          ; paras = len / 16
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    test    si, 0Fh
    jz      short no_remainder
    inc     ax
no_remainder:
    push    ax              ; paras
    mov     bx, [bp+arg_dstseg]
    sub     bx, ax
    add     bx, [bp+arg_dstoff]
    mov     [bp+arg_srcseg], bx
    push    bx
    push    [bp+arg_dstoff]
    call far ptr _copy_paras_reverse
    add     sp, 6
    xor     si, si
    mov     [bp+arg_srcoff], si
skip_seq_pass:
    call near ptr _file_decomp_rle_single
    mov     ax, [bp+var_lenlo]; Return length
    mov     dx, [bp+var_lenhi]
    add     sp, 116h
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf

_file_decomp_rle endp
_file_decomp_rle_single proc near
    mov     cx, 80h ; ''
    lea     di, [bp-11Ch]
    mov     ax, ss
    mov     es, ax
    mov     ds, ax
    xor     ax, ax
    rep stosw
    mov     cl, [bp-12h]

    and     cx, 7Fh

    lea     si, [bp-11h]
    lea     di, [bp-11Ch]
    xor     ax, ax
    xor     bh, bh
write_decoded_repeat_run:
    inc     ax
    mov     bl, [si]
    mov     [bx+di], al
    inc     si
    loop    write_decoded_repeat_run
    mov     ds, word ptr [bp+8]
    mov     si, [bp+6]
    mov     es, word ptr [bp+0Ch]
    xor     di, di
    mov     dx, [bp-16h]
    mov     ax, [bp-14h]
    mov     [bp-1Ch], ax
check_compressed_input_wrap:
    cmp     si, 8000h
    jbe     short check_output_buffer_wrap
    jmp     short wrap_compressed_source
    db 144
check_output_buffer_wrap:
    cmp     di, 8000h
    jbe     short read_rle_control_byte
    jmp     short wrap_output_buffer
    ; align 2
    db 144
read_rle_control_byte:
    lodsb
    xor     ah, ah
    lea     bx, [bp-11Ch]
    add     bx, ax
    mov     cl, ss:[bx]
    test    cl, 0FFh
    jnz     short decode_rle_run_length
    stosb
    dec     dx
    jnz     short check_compressed_input_wrap
    cmp     word ptr [bp-1Ch], 0
    jz      short advance_output_segment
    dec     word ptr [bp-1Ch]
    jmp     short check_compressed_input_wrap
decode_rle_run_length:
    xor     ch, ch
    cmp     cl, 1
    jz      short decode_rle_literal_run
    cmp     cl, 3
    jz      short decode_rle_repeated_run
    dec     cx
    lodsb
    rep stosb
    sub     dx, 2
adjust_source_segment:
    mov     ax, [bp-1Ch]
    sbb     ax, 0
    mov     [bp-1Ch], ax
    or      ax, dx
    jz      short advance_output_segment
    jmp     short check_compressed_input_wrap
advance_output_segment:
    mov     dx, es
    sub     dx, [bp+0Ch]
    mov     ax, 10h
    mul     dx
    add     ax, di
    adc     dx, 0
    retn
decode_rle_literal_run:
    lodsb
    mov     cl, al
    lodsb
    rep stosb
    sub     dx, 3
    jmp     short adjust_source_segment
decode_rle_repeated_run:
    lodsb
    mov     cl, al
    lodsb
    mov     ch, al
    lodsb
    rep stosb
    sub     dx, 4
    jmp     short adjust_source_segment
wrap_compressed_source:
    sub     si, 8000h
    mov     ax, ds
    add     ax, 800h
    mov     ds, ax
    jmp     check_output_buffer_wrap
wrap_output_buffer:
    sub     di, 8000h
    mov     ax, es
    add     ax, 800h
    mov     es, ax
    jmp     read_rle_control_byte

_file_decomp_rle_single endp
_file_decomp_rle_seq proc near
    cmp     byte ptr [bp-12h], 1; _file_decomp_rle::var_esclen
    jnz     short has_codes
    retn
has_codes:
    mov     ds, word ptr [bp+8]; _file_decomp_rle::arg_srcseg
    mov     si, [bp+6]      ; _file_decomp_rle::arg_srcoff
    mov     es, word ptr [bp+0Ch]; _file_decomp_rle::var_dstoff
    xor     di, di
    mov     bx, [bp-16h]
    mov     ax, [bp-14h]
    mov     [bp-1Ch], ax
    mov     ah, [bp-10h]
check_sequence_input_wrap:
    cmp     si, 8000h
    ja      short wrap_sequence_input
check_sequence_output_wrap:
    cmp     di, 8000h
    ja      short wrap_sequence_output
read_rle_sequence_byte:
    lodsb
    cmp     al, ah
    jz      short read_next_sequence_byte
    stosb
    dec     bx
    jnz     short check_sequence_input_wrap
    cmp     word ptr [bp-1Ch], 0
    jz      short advance_output_segment_2
    dec     word ptr [bp-1Ch]
    jmp     short check_sequence_input_wrap
wrap_sequence_input:
    sub     si, 8000h
    mov     dx, ds
    add     dx, 800h
    mov     ds, dx
    jmp     short check_sequence_output_wrap
wrap_sequence_output:
    sub     di, 8000h
    mov     dx, es
    add     dx, 800h
    mov     es, dx
    jmp     short read_rle_sequence_byte
advance_output_segment_2:
    mov     dx, es
    sub     dx, [bp+0Ch]
    mov     ax, 10h
    mul     dx
    add     ax, di
    adc     dx, 0
    retn
read_next_sequence_byte:
    mov     dx, di
compare_sequence_control_byte:
    lodsb
    cmp     al, ah
    jz      short read_sequence_repeat_count
    stosb
    dec     bx
    jnz     short compare_sequence_control_byte
    dec     word ptr [bp-1Ch]
    jmp     short compare_sequence_control_byte
read_sequence_repeat_count:
    lodsb
    push    si
    push    ds
    mov     cx, es
    mov     ds, cx
    mov     si, dx
    mov     cx, di
    sub     cx, dx
    mov     dl, al
    dec     dl
copy_sequence_repeat_run:
    push    cx
    push    si
    push    ds
    rep movsb
    pop     ds
    pop     si
    pop     cx
    dec     dl
    jnz     short copy_sequence_repeat_run
    pop     ds
    pop     si
    sub     bx, 3
    sbb     word ptr [bp-1Ch], 0
    mov     dx, [bp-1Ch]
    or      dx, bx
    jz      short advance_output_segment_2
    jmp     check_sequence_input_wrap

_file_decomp_rle_seq endp
S012_TEXT ends
end
