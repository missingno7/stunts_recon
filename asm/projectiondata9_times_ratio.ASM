.8086
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _projection_x_scale:word
S012_TEXT segment word public 'CODE'
public _projectiondata9_times_ratio
_projectiondata9_times_ratio proc far
    push bp
    mov bp, sp
    mov ax, word ptr DGROUP:_projection_x_scale
    mul word ptr [bp+6]
    div word ptr [bp+8]
    pop bp
    retf
_projectiondata9_times_ratio endp
S012_TEXT ends
end

