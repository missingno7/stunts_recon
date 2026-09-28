.8086
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
public aSFileError_0
aSFileError_0     db 37
    db 115
    db 32
    db 70
    db 73
    db 76
    db 69
    db 32
    db 69
    db 82
    db 82
    db 79
    db 82
    db 13
    db 0
    db 0
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _fatal_error:far
S012_TEXT segment word public 'CODE'
public _file_write_nofatal
public _file_write_fatal
_file_write_nofatal proc far
    var_fatal = word ptr -4
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short _file_write
    db 144
_file_write_nofatal endp
_file_write_fatal proc far
    var_errno = word ptr -6
    var_fatal = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_srcoff = word ptr 8
    arg_srcseg = word ptr 10
    arg_lenl = word ptr 12
    arg_lenh = word ptr 14

    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 1
_file_write:
    mov     dx, [bp+arg_filename]
    xor     cx, cx
    mov     ah, 3Ch
    int     21h             ; DOS - 2+ - CREATE A FILE WITH HANDLE (CREAT)
    jb      short loc_32570
    mov     [bp+var_filehandle], ax
loc_32527:
    cmp     [bp+arg_lenl], 0
    jnz     short loc_32533
    cmp     [bp+arg_lenh], 0
    jz      short loc_3259E
loc_32533:
    mov     cx, 4000h
    mov     ax, [bp+arg_lenl]
    sub     [bp+arg_lenl], cx
    sbb     [bp+arg_lenh], 0
    jnb     short loc_32553
    mov     cx, [bp+arg_lenl]
    mov     [bp+arg_lenl], 0
    mov     [bp+arg_lenh], 0
    add     cx, 4000h
loc_32553:
    mov     ds, [bp+arg_srcseg]
    mov     dx, [bp+arg_srcoff]
    mov     bx, [bp+var_filehandle]
    mov     ah, 40h
    int     21h             ; DOS - 2+ - WRITE TO FILE WITH HANDLE
    jb      short loc_32570
    cmp     ax, cx
    jnz     short loc_3256D
    add     [bp+arg_srcseg], 400h
    jmp     short loc_32527
loc_3256D:
    mov     ax, 1
loc_32570:
    mov     [bp+var_errno], ax
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h             ; DOS - 2+ - CLOSE A FILE WITH HANDLE
    mov     ax, ss
    mov     ds, ax
    mov     dx, [bp+arg_filename]
    mov     ah, 41h
    int     21h             ; DOS - 2+ - DELETE A FILE (UNLINK)
    mov     ax, [bp+var_errno]
    cmp     [bp+var_fatal], 0
    jnz     short loc_325A7
    mov     ax, ss
    mov     ds, ax
    mov     ax, offset DGROUP:aSFileError_0; "%s FILE ERROR\r"
    push    [bp+arg_filename]
    push    ax
    call    far ptr _fatal_error
loc_3259E:
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h             ; DOS - 2+ - CLOSE A FILE WITH HANDLE
    xor     ax, ax
loc_325A7:
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_file_write_fatal endp
S012_TEXT ends
end
