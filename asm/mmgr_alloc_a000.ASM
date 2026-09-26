.8086
_TEXT segment word public 'CODE'
extrn _mmgr_alloc_resmem:far
_TEXT ends
assume cs:_TEXT
_TEXT segment word public 'CODE'
public _mmgr_alloc_a000
_mmgr_alloc_a000 proc far
    mov     ax, 0A000h
    push    ax
    call    _mmgr_alloc_resmem
    add     sp, 2
    retf
_mmgr_alloc_a000 endp
_TEXT ends
end
