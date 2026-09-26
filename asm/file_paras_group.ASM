.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _aSFileError_1:byte
DSEG ends
extrn _fatal_error:far
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _file_paras
public _file_paras_nofatal
public _file_paras_fatal
_file_paras proc far
    var_fatal = word ptr -6
    var_length = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     ax, [bp+arg_fatal]
    mov     [bp+var_fatal], ax
    jmp     short paras_body
    ; align 2
    db 144
_file_paras endp
_file_paras_nofatal proc far
    var_6 = word ptr -6
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+var_6], 0
    jmp     short paras_body
    db 144
_file_paras_nofatal endp
_file_paras_fatal proc near
    var_fatal = word ptr -6
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 1
    var_fatal = word ptr -6
    var_length = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
paras_body:
    mov     dx, [bp+arg_filename]
    mov     ah, 3Dh ; '='
    xor     al, al
    int     21h             ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
    jnb     short loc_2FEDD
    cmp     [bp+var_fatal], 0
    jnz     short loc_2FECD
    xor     ax, ax
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_2FECD:
    mov     ax, offset _aSFileError_1; "%s FILE ERROR"
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    push    ax
    call    far ptr _fatal_error
loc_2FEDD:
    mov     [bp+var_filehandle], ax
    mov     bx, ax
    sub     cx, cx
    sub     dx, dx
    mov     ax, 4202h
    int     21h             ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
    push    dx
    push    ax
    sub     dx, dx
    mov     ax, 4200h
    int     21h             ; DOS - 2+ - MOVE FILE READ/WRITE POINTER (LSEEK)
    pop     ax
    pop     dx
    xor     bx, bx
    test    ax, 0Fh
    jz      short loc_2FF00
    mov     bx, 1
loc_2FF00:
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    add     ax, bx
    mov     [bp+var_length], ax
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h             ; DOS - 2+ - CLOSE A FILE WITH HANDLE
    mov     ax, [bp+var_length]
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_file_paras_fatal endp
_TEXT ends
end
