.8086
extrn _incnums:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _sub_35DC8
copy_bytes equ 100h
_sub_35DC8 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push bp
    mov bp, sp
    push ds
    push si
    push di
    mov cx, copy_bytes
    mov ds, [bp+arg_2]
    mov si, [bp+arg_0]
    mov ax, cs
    mov es, ax
    mov di, offset _incnums
    cld
    rep movsb
    pop di
    pop si
    pop ds
    pop bp
    retf
_sub_35DC8 endp
S012_TEXT ends
end
