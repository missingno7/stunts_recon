.8086
extrn _sub_38156:far
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _audio_function2
_audio_function2 proc far
    push bp
    mov bp, sp
    push si
    mov ax, 4Ch
    imul word ptr [bp+6]
    mov si, ax
    add si, 6364h
    cmp byte ptr [si], 1
    jne short audio_function2_done
    cmp byte ptr [si+1], 1
    jne short audio_function2_done
    push word ptr [si+12h]
    call far ptr _sub_38156
    pop bx
    mov word ptr [si+12h], 0FFFFh
    mov byte ptr [si+1], 0
    mov byte ptr [si+1Ah], 1
audio_function2_done:
    pop si
    mov sp, bp
    pop bp
    retf
_audio_function2 endp
_TEXT ends
end
