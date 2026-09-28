.8086
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
dseg segment byte public 'STUNTSD'
extrn aLocateshape4_4sShapeNotF:byte
extrn aLocatesound4_4sSoundNotF:byte
dseg ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:dseg
extrn _fatal_error:far
S012_TEXT segment word public 'CODE'
public _locate_shape_nofatal
public _locate_shape_fatal
public _locate_sound_fatal
_locate_shape_nofatal proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    xor     dx, dx
    jmp     short _alt_locate_resource
    db 144
_locate_shape_nofatal endp
_locate_shape_fatal proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, 1
    jmp     short _alt_locate_resource
    db 144
_locate_shape_fatal endp
_locate_sound_fatal proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10

    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, 2
_alt_locate_resource:
    cld
    mov     ds, [bp+arg_2]
    mov     si, [bp+arg_0]
    mov     ax, seg dseg
    mov     es, ax
    mov     di, [bp+arg_4]
    mov     cx, 4
    mov     bx, 0
locate_sound_search_loop:
    cmp     byte ptr es:[bx+di], 0
    jz      short locate_sound_search_loop_5
    inc     bx
    loop    locate_sound_search_loop
locate_sound_search_loop_2:
    mov     ax, [si+4]
    or      ax, ax
    jz      short _end_of_locate
    add     si, 6
    mov     bx, si
locate_sound_search_loop_3:
    mov     si, bx
    mov     di, [bp+arg_4]
    mov     cx, 4
locate_sound_search_loop_4:
    cmpsb
    jnz     short locate_sound_when_nonzero
    loop    locate_sound_search_loop_4
    jmp     short _found_resource
    ; align 2
    db 144
locate_sound_when_nonzero:
    cmp     byte ptr [si-1], 0
    jnz     short locate_sound_when_nonzero_2
    cmp     byte ptr es:[di-1], 20h ; ' '
    jz      short _found_resource
locate_sound_when_nonzero_2:
    add     bx, 4
    dec     ax
    jge     short locate_sound_search_loop_3
_end_of_locate:
    cmp     dx, 1
    jl      short locate_sound_when_less
    jg      short locate_sound_when_greater
    mov     dx, offset aLocateshape4_4sShapeNotF; "locateshape - %-4.4s SHAPE NOT FOUND\r\n"
    jmp     short locate_sound_error_path
    ; align 2
    db 144
locate_sound_when_greater:
    mov     dx, offset aLocatesound4_4sSoundNotF; "locatesound - %-4.4s SOUND NOT FOUND\r\n"
locate_sound_error_path:
    mov     ax, seg dseg
    mov     ds, ax
    push    [bp+arg_4]
    push    dx
    call    far ptr _fatal_error
    int     20h             ; DOS - PROGRAM TERMINATION
locate_sound_when_less:
    xor     ax, ax
    jmp     short sound_return_if_condition
    ; align 2
    db 144
locate_sound_search_loop_5:
    mov     byte ptr es:[bx+di], 20h ; ' '
    inc     bx
    loop    locate_sound_search_loop_5
    jmp     short locate_sound_search_loop_2
_found_resource:
    mov     si, [bp+arg_0]
    mov     ax, [si+4]
    shl     ax, 1
    shl     ax, 1
    add     bx, ax
    shl     ax, 1
    add     ax, 6
    mov     cx, ds
    xor     dx, dx
    shl     cx, 1
    rcl     dx, 1
    shl     cx, 1
    rcl     dx, 1
    shl     cx, 1
    rcl     dx, 1
    shl     cx, 1
    rcl     dx, 1
    add     ax, cx
    adc     dx, 0
    add     ax, [bx]
    adc     dx, [bx+2]
    mov     bx, ax

    and     bx, 0Fh

    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    mov     dx, ax
    mov     ax, bx
sound_return_if_condition:
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
_locate_sound_fatal endp
S012_TEXT ends
end
