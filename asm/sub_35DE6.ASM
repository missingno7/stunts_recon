.8086
extrn _incnums:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _sub_35DE6
_sub_35DE6 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12
    push bp
    mov bp, sp
    push ds
    push si
    push di
    mov cx, [bp+arg_2]
    mov ds, [bp+arg_6]
    mov si, [bp+arg_0]
    mov ax, cs
    mov es, ax
    lea di, _incnums[si]
    mov si, [bp+arg_4]
loc_35E00:
    cld
loc_35E01:
    rep movsb
    pop di
    pop si
    pop ds
    pop bp
    retf
_sub_35DE6 endp
S012_TEXT ends
end
