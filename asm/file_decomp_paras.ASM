.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _aSFileError_1:byte
DSEG ends
extrn _fatal_error:far
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _file_decomp_paras, _file_decomp_paras_nofatal, _file_decomp_paras_fatal
_file_decomp_paras proc far
    var_fatal = word ptr -8
    var_6 = byte ptr -6
    var_5 = word ptr -5
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    push    si
    push    di
    mov     ax, [bp+arg_fatal]
    mov     [bp+var_fatal], ax
    jmp     short decomp_paras_body
    ; align 2
    db 144

_file_decomp_paras endp
_file_decomp_paras_nofatal proc far
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short decomp_paras_body
    db 144

_file_decomp_paras_nofatal endp
_file_decomp_paras_fatal proc near
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 1
    var_fatal = word ptr -8
    var_6 = byte ptr -6
    var_5 = word ptr -5
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
decomp_paras_body:
    mov     dx, [bp+arg_filename]
    mov     dx, [bp+arg_filename]
    mov     ah, 3Dh ; '='
    xor     al, al
    int     21h             ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
    jnb     short loc_2FF68
    jmp     short loc_2FFB5
    ; align 2
    db 144
    var_fatal = word ptr -8
    var_6 = byte ptr -6
    var_5 = word ptr -5
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
loc_2FF68:
    mov     [bp+var_filehandle], ax
    mov     bx, [bp+var_filehandle]
    lea     dx, [bp+var_6]
    mov     cx, 4
    mov     ah, 3Fh
    int     21h             ; DOS - 2+ - READ FROM FILE WITH HANDLE
    cmp     ax, cx
    jnz     short loc_2FFAE
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h             ; DOS - 2+ - CLOSE A FILE WITH HANDLE
    mov     ax, [bp+var_5]
    mov     dx, [bp-3]
    xor     dh, dh
    xor     bx, bx
    test    ax, 0Fh
    jz      short loc_2FF95
    mov     bx, 1
loc_2FF95:
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    add     ax, bx
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_2FFAE:
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h             ; DOS - 2+ - CLOSE A FILE WITH HANDLE
loc_2FFB5:
    cmp     [bp+var_fatal], 0
    jnz     short loc_2FFC4
    xor     ax, ax
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_2FFC4:
    mov     ax, offset _aSFileError_1; "%s FILE ERROR"
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    push    ax
loc_2FFCF:
    call    far ptr _fatal_error

_file_decomp_paras_fatal endp
_TEXT ends
end
