.8086
S012_TEXT segment byte public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _resendptr1:word
extrn _resendptr2:word
extrn _mmgr_path_to_name:far
S012_TEXT segment byte public 'CODE'
public _nopsub_31429
_nopsub_31429 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     si, _resendptr1
    push    [bp+arg_0]
    call    _mmgr_path_to_name
    add     sp, 2
    mov     di, ax
loc_3143F:
    xor     bx, bx
    cmp     word ptr [si+10h], 0
    jz      short loc_3146D
loc_31447:
    mov     al, [bx+di]
    or      al, al
    jz      short loc_3145A
    cmp     al, [bx+si]
    jnz     short loc_31464
    inc     bx
    cmp     bx, 0Ch
    jl      short loc_31447
    jmp     short loc_31475

    db 144
loc_3145A:
    cmp     byte ptr [bx+si], 2Eh
    jz      short loc_31475
    cmp     byte ptr [bx+si], 0
    jz      short loc_31475
loc_31464:
    add     si, 12h
    cmp     si, _resendptr2
    jb      short loc_3143F
loc_3146D:
    xor     dx, dx
    xor     ax, ax
    pop     di
    pop     si
    pop     bp
    retf
loc_31475:
    mov     ax, 1
    pop     di
    pop     si
    pop     bp
    retf
_nopsub_31429 endp
S012_TEXT ends
end
