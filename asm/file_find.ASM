.8086
.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _dtatransbuffer:byte
extrn _foundfile:byte
extrn _foundfilepath:byte
extrn _foundfileptr:word
DSEG ends
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _file_find, _file_find_next
_file_find proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, offset _dtatransbuffer
    mov     ah, 1Ah
    int     21h             ; DOS - SET DISK TRANSFER AREA ADDRESS
    mov     dx, [bp+arg_0]
    mov     cl, 6
    mov     ah, 4Eh
    int     21h             ; DOS - 2+ - FIND FIRST ASCIZ (FINDFIRST)
    jb      short _file_find_err
    mov     di, offset _foundfile
    mov     _foundfileptr, di
    mov     si, [bp+arg_0]
    mov     cx, 57h ; 'W'
    mov     ax, ds
    mov     es, ax
loc_2FFFD:
    lodsb
    stosb
    cmp     al, 0
    jz      short _file_find_ok
    cmp     al, 3Ah ; ':'
    jz      short loc_3000B
    cmp     al, 5Ch ; '\'
    jnz     short loc_3000F
loc_3000B:
    mov     _foundfileptr, di
loc_3000F:
    loop    loc_2FFFD
_file_find_ok:
    mov     ax, ds
    mov     es, ax
    mov     si, offset _foundfilepath
    mov     di, _foundfileptr
    mov     cx, 0Dh
    cld
    rep movsb
    mov     ax, offset _foundfile
loc_30025:
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
_file_find_err:
    xor     ax, ax
    jmp     short loc_30025

_file_find endp
_file_find_next proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, offset _dtatransbuffer
    mov     ah, 1Ah
    int     21h             ; DOS - SET DISK TRANSFER AREA ADDRESS
    mov     ah, 4Fh
    int     21h             ; DOS - 2+ - FIND NEXT ASCIZ (FINDNEXT)
    jb      short _file_find_err
    jmp     short _file_find_ok
    ; align 2
    db 0

_file_find_next endp
_TEXT ends
end
