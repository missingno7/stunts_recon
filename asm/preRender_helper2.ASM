.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _preRender_helper2
_preRender_helper2 proc near
    mov cx, word ptr [si+0Eh]
    or cx, cx
    jle short prerender_help2_early_return
    mov di, word ptr [si+6]
    shl di, 1
    add di, word ptr [bp-18h]
    mov bl, byte ptr [si+12h]
    xor bh, bh
    shl bx, 1
    jmp cs:prerender_help2_lookup_table[bx]
prerender_help2_lookup_table label word
    dw offset prerender_help2_early_return
    dw offset prerender_help2_early_return
    dw offset prerender_help2_finish
    dw offset prerender_help2_finish_2
    dw offset prerender_help2_finish_3
    dw offset help2_load_source_value
    dw offset help2_load_source_value_2
    dw offset help2_load_source_value_3
    dw offset help2_load_source_value_4
    dw offset prerender_help2_early_return
prerender_help2_finish:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
prerender_help2_fill_loop:
    mov word ptr [di+3C0h], ax
    stosw
    loop prerender_help2_fill_loop
prerender_help2_early_return:
    ret
prerender_help2_finish_2:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
prerender_help2_fill_loop_2:
    mov word ptr [di+3C0h], ax
    stosw
    dec ax
    loop prerender_help2_fill_loop_2
    ret
prerender_help2_finish_3:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
prerender_help2_fill_loop_3:
    mov word ptr [di+3C0h], ax
    stosw
    inc ax
    loop prerender_help2_fill_loop_3
    ret
help2_load_source_value:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
    mov dx, word ptr [si]
    add dx, 8000h
    adc ax, 0
    mov bx, word ptr [si+0Ch]
prerender_help2_fill_loop_4:
    mov word ptr [di+3C0h], ax
    stosw
    sub dx, bx
    sbb ax, 0
    loop prerender_help2_fill_loop_4
    ret
help2_load_source_value_2:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
    mov dx, word ptr [si]
    add dx, 8000h
    adc ax, 0
    mov bx, word ptr [si+0Ch]
prerender_help2_fill_loop_5:
    mov word ptr [di+3C0h], ax
    stosw
    add dx, bx
    adc ax, 0
    loop prerender_help2_fill_loop_5
    ret
help2_load_source_value_3:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
    mov bx, word ptr [si+0Ch]
    mov dx, word ptr [si+4]
    add dx, 8000h
    jnb short prerender_help2_fill_loop_6
    add di, 2
prerender_help2_fill_loop_6:
    mov word ptr [di+3C0h], ax
prerender_help2_fill_loop_7:
    add dx, bx
    jnb short help2_return_if_not_below
    stosw
    dec ax
    loop prerender_help2_fill_loop_6
    sub di, 2
    ret
help2_return_if_not_below:
    dec ax
    loop prerender_help2_fill_loop_7
    inc ax
    mov word ptr [di], ax
    ret
help2_load_source_value_4:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
    mov bx, word ptr [si+0Ch]
    mov dx, word ptr [si+4]
    add dx, 8000h
    jnb short prerender_help2_scan_loop
    add di, 2
prerender_help2_scan_loop:
    mov word ptr [di], ax
prerender_help2_scan_loop_2:
    add dx, bx
    jnb short help2_return_if_not_below_2
    mov word ptr [di+3C0h], ax
    add di, 2
    inc ax
    loop prerender_help2_scan_loop
    sub di, 2
    ret
help2_return_if_not_below_2:
    inc ax
    loop prerender_help2_scan_loop_2
    dec ax
    mov word ptr [di+3C0h], ax
    ret
_preRender_helper2 endp
_TEXT ends
end
