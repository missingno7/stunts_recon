.8086
DSEG segment byte public 'STUNTSD'
DSEG ends
_DATA segment word public 'DATA'
_byte_4032A db 0
_byte_4032B db 0
_byte_4032C db 7
db 0
_word_4032E dw 3840
_word_40330 dw 0
_word_40332 dw 0
_word_40334 dw 0
_output_buffer db 120 dup (0)
_DATA ends
DGROUP group _DATA,DSEG
assume ds:DGROUP
extrn _sprintf:far
public _nopsub_3215A
public _nopsub_3216C
public _nopsub_3219D
public _nopsub_322B4
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP

; nopsub_3215A
assume cs:S012_TEXT
_nopsub_3215A proc far
    push di
    mov ax, 0B000h
    mov es, ax
    xor di, di
    mov cx, 07D0h
    xor ax, ax
    cld
    rep stosw
    pop di
    retf
_nopsub_3215A endp

; nopsub_3216C
_nopsub_3216C proc far
     r = byte ptr 0

    push    si
    push    di
    push    ds
    mov     al, _byte_4032A
    mov     bl, 0A0h
    mul     bl
    mov     si, ax
    mov     di, si
    add     si, 0A0h
    mov     ax, 18h
    sub     al, _byte_4032A
    mov     bl, 50h
    mul     bl
    mov     cx, ax
    mov     ax, es
    mov     ds, ax
    cld
    rep movsw
    xor     ax, ax
    mov     cx, 50h
    rep stosw
    pop     ds
    pop     di
    pop     si
    retf
_nopsub_3216C endp

; nopsub_3219D
_nopsub_3219D proc far
    pop ax
    mov word ptr _word_40332, ax
    pop ax
    mov word ptr _word_40334, ax
    mov ax, offset DGROUP:_output_buffer
    push ax
    call far ptr _sprintf
    add sp, 2
    mov byte ptr _byte_4032B, 1
    mov ax, word ptr _word_4032E
    mov word ptr _word_40330, ax
loc_321BC:
    mov ax, word ptr _word_40334
    push ax
    mov ax, word ptr _word_40332
    push ax
    push si
    mov ax, 0B000h
    mov es, ax
    mov si, 4BC6h
    mov dh, byte ptr _byte_4032C
loc_321D1:
    mov dl, byte ptr [si]
    inc si
    or dl, dl
    jz short loc_321FD
    cmp dl, 20h
    jl short loc_3220C
    mov bx, word ptr _word_40330
    cmp bx, 0FA0h
    jl short loc_321F3
    call far ptr _nopsub_3216C
    mov bx, 0F00h
    mov word ptr _word_40330, bx
loc_321F3:
    mov word ptr es:[bx], dx
    add word ptr _word_40330, 2
    jmp short loc_321D1
loc_321FD:
    cmp byte ptr _byte_4032B, 0
    jz short loc_3220A
    mov ax, word ptr _word_40330
    mov word ptr _word_4032E, ax
loc_3220A:
    pop si
    retf
loc_3220C:
    cmp dl, 0Ah
    jnz short loc_3223A
    cmp word ptr _word_40330, 0F00h
    jle short loc_32226
    call far ptr _nopsub_3216C
    mov word ptr _word_40330, 0F00h
    jmp short loc_321D1
loc_32226:
    mov ax, word ptr _word_40330
    mov bx, 0A0h
    div bl
    xor ah, ah
    mul bl
    add ax, 0A0h
    mov word ptr _word_40330, ax
    jmp short loc_321D1
loc_3223A:
    cmp dl, 8
    jnz short loc_3224D
    cmp word ptr _word_40330, 0
    jz short loc_321D1
    sub word ptr _word_40330, 2
    jmp short loc_321D1
loc_3224D:
    cmp dl, 0Ch
    jnz short loc_32260
    call far ptr _nopsub_3215A
    mov word ptr _word_40330, 0
    jmp loc_321D1
loc_32260:
    cmp dl, 9
    jnz short loc_3226F
    add word ptr _word_40330, 10h
    and word ptr _word_40330, 0FFF0h
loc_3226F:
    jmp loc_321D1
loc_32272:
    pop ax
    mov word ptr _word_40332, ax
    pop ax
    mov word ptr _word_40334, ax
    pop dx
    pop ax
    mov bx, 0A0h
    mul bl
    add ax, dx
    mov word ptr _word_40330, ax
    mov ax, offset DGROUP:_output_buffer
    push ax
    call far ptr _sprintf
    sub sp, 2
    mov byte ptr _byte_4032B, 0
    jmp loc_321BC
loc_3229A:
    push bp
    mov bp, sp
    mov ax, word ptr [bp+6]
    cmp ax, 0
    jge short loc_322A7
    xor ax, ax
loc_322A7:
    cmp ax, 18h
    jle short loc_322AF
    mov ax, 18h
loc_322AF:
    mov byte ptr _byte_4032A, al
    pop bp
    retf
_nopsub_3219D endp

; nopsub_322B4
_nopsub_322B4 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     _byte_4032C, al
    pop     bp
    retf

    db 0
_nopsub_322B4 endp

S012_TEXT ends
end
