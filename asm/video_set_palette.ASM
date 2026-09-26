.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _video_set_palette
_video_set_palette proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    mov     ax, ds
loc_346A8:
    mov     es, ax
    mov     bx, [bp+arg_0]
    mov     dx, [bp+arg_4]
    mov     cx, [bp+arg_2]
    mov     ah, 10h
    mov     al, 12h
    int     10h             ; - VIDEO - SET BLOCK OF DAC REGISTERS (EGA, VGA/MCGA)
    pop     bp
    retf
    ; align 2
    db 0
_video_set_palette endp
_TEXT ends
end
