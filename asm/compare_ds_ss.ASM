.8086
S012_TEXT segment byte public 'CODE'
assume cs:S012_TEXT
public _compare_ds_ss
_compare_ds_ss proc far

    xor     ax, ax
    mov     bx, ss
    mov     dx, ds
    cmp     bx, dx
    jnz     short compare_ds_ss_early_return
    inc     ax
compare_ds_ss_early_return:
    retf
_compare_ds_ss endp
S012_TEXT ends
end
