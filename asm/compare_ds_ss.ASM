.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _compare_ds_ss
_compare_ds_ss proc far

    xor     ax, ax
    mov     bx, ss
    mov     dx, ds
    cmp     bx, dx
    jnz     short locret_30328
    inc     ax
locret_30328:
    retf
_compare_ds_ss endp
_TEXT ends
end
