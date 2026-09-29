.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _preRender_helper2
_preRender_helper2 proc near
    mov cx, word ptr [si+0Eh]
    or cx, cx
    jle short locret_31AA3
    mov di, word ptr [si+6]
    shl di, 1
    add di, word ptr [bp-18h]
    mov bl, byte ptr [si+12h]
    xor bh, bh
    shl bx, 1
    jmp cs:off_31A82[bx]
off_31A82 label word
    dw offset locret_31AA3
    dw offset locret_31AA3
    dw offset loc_31A96
    dw offset loc_31AA4
    dw offset loc_31AB3
    dw offset loc_31AC2
    dw offset loc_31AE1
    dw offset loc_31B00
    dw offset loc_31B2C
    dw offset locret_31AA3
loc_31A96:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
loc_31A9C:
    mov word ptr [di+3C0h], ax
    stosw
    loop loc_31A9C
locret_31AA3:
    ret
loc_31AA4:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
loc_31AAA:
    mov word ptr [di+3C0h], ax
    stosw
    dec ax
    loop loc_31AAA
    ret
loc_31AB3:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
loc_31AB9:
    mov word ptr [di+3C0h], ax
    stosw
    inc ax
    loop loc_31AB9
    ret
loc_31AC2:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
    mov dx, word ptr [si]
    add dx, 8000h
    adc ax, 0
    mov bx, word ptr [si+0Ch]
loc_31AD4:
    mov word ptr [di+3C0h], ax
    stosw
    sub dx, bx
    sbb ax, 0
    loop loc_31AD4
    ret
loc_31AE1:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
    mov dx, word ptr [si]
    add dx, 8000h
    adc ax, 0
    mov bx, word ptr [si+0Ch]
loc_31AF3:
    mov word ptr [di+3C0h], ax
    stosw
    add dx, bx
    adc ax, 0
    loop loc_31AF3
    ret
loc_31B00:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
    mov bx, word ptr [si+0Ch]
    mov dx, word ptr [si+4]
    add dx, 8000h
    jnb short loc_31B15
    add di, 2
loc_31B15:
    mov word ptr [di+3C0h], ax
loc_31B19:
    add dx, bx
    jnb short loc_31B25
    stosw
    dec ax
    loop loc_31B15
    sub di, 2
    ret
loc_31B25:
    dec ax
    loop loc_31B19
    inc ax
    mov word ptr [di], ax
    ret
loc_31B2C:
    mov cx, word ptr [si+0Eh]
    mov ax, word ptr [si+2]
    mov bx, word ptr [si+0Ch]
    mov dx, word ptr [si+4]
    add dx, 8000h
    jnb short loc_31B41
    add di, 2
loc_31B41:
    mov word ptr [di], ax
loc_31B43:
    add dx, bx
    jnb short loc_31B55
    mov word ptr [di+3C0h], ax
    add di, 2
    inc ax
    loop loc_31B41
    sub di, 2
    ret
loc_31B55:
    inc ax
    loop loc_31B43
    dec ax
    mov word ptr [di+3C0h], ax
    ret
_preRender_helper2 endp
_TEXT ends
end
