.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _aSFileError:byte
DSEG ends
extrn _fatal_error:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _file_read, _file_read_nofatal, _file_read_fatal
_file_read proc far
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2
    arg_fatal = word ptr 12

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    mov     ax, [bp+arg_fatal]
    mov     [bp+var_fatal], ax
    jmp     short read_body
    ; align 2
    db 144

_file_read endp
_file_read_nofatal proc far
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    mov     [bp+var_fatal], 0
    jmp     short read_body
    db 144

_file_read_nofatal endp
_file_read_fatal proc far
    var_fatal = word ptr -8
    var_curseg = word ptr -6
    var_curoff = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_dstoff = word ptr 8
    arg_dstseg = word ptr 10

    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    mov     [bp+var_fatal], 1
read_body:
    mov     ax, [bp+arg_dstoff]
    mov     [bp+var_curoff], ax
    mov     ax, [bp+arg_dstseg]
    mov     [bp+var_curseg], ax
    mov     dx, [bp+arg_filename]
    xor     al, al
    mov     ah, 3Dh
    int     21h             ; DOS - 2+ - OPEN DISK FILE WITH HANDLE
    jb      short error
    mov     [bp+var_filehandle], ax
readloop:
    mov     ds, [bp+var_curseg]
    mov     dx, [bp+var_curoff]
    mov     cx, 4000h
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Fh
    int     21h             ; DOS - 2+ - READ FROM FILE WITH HANDLE
    jb      short error
    add     [bp+var_curseg], 400h
    cmp     ax, 4000h
    jz      short readloop
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h             ; DOS - 2+ - CLOSE A FILE WITH HANDLE
    mov     ax, [bp+arg_dstoff]
    mov     dx, [bp+arg_dstseg]
    pop     ds
    mov     sp, bp
    pop     bp
    retf
error:
    cmp     [bp+var_fatal], 0
    jnz     short fatal
    xor     ax, ax
    xor     dx, dx
    pop     ds
    mov     sp, bp
    pop     bp
    retf
fatal:
    mov     ax, offset _aSFileError; "%s FILE ERROR\r"
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    push    ax
    call    far ptr _fatal_error

_file_read_fatal endp
S012_TEXT ends
end
