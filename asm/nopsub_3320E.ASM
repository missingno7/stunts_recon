.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _nopsub_3320E
_nopsub_3320E proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12
    arg_8 = word ptr 14
    arg_A = word ptr 16

    push    bp
    mov     bp, sp
    push    ds
    push    si
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, [si+2]
    cmp     word ptr cs:[5F22h], ax
    jnz     short coordinates_when_nonzero
    mov     ax, [bp+arg_4]
    mov     [si+1Ah], ax
    mov     cs:[5F3Ah], ax
    mov     [si+0Ch], ax
    mov     cs:[5F2Ch], ax
    mov     ax, [bp+arg_6]
    mov     [si+1Ch], ax
    mov     cs:[5F3Ch], ax
    mov     [si+0Eh], ax
    mov     cs:[5F2Eh], ax
    mov     ax, [bp+arg_8]
    mov     [si+10h], ax
    mov     cs:[5F30h], ax
    mov     ax, [bp+arg_A]
    mov     [si+12h], ax
    mov     cs:[5F32h], ax
    pop     si
    pop     ds
    pop     bp
    retf
coordinates_when_nonzero:
    mov     ax, [bp+arg_4]
    mov     [si+1Ah], ax
    mov     [si+0Ch], ax
    mov     ax, [bp+arg_6]
    mov     [si+1Ch], ax
    mov     [si+0Eh], ax
    mov     ax, [bp+arg_8]
    mov     [si+10h], ax
    mov     ax, [bp+arg_A]
    mov     [si+12h], ax
    pop     si
    pop     ds
    pop     bp
    retf
_nopsub_3320E endp
_TEXT ends
end
