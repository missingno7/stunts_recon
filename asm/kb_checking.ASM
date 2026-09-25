_TEXT segment word public 'CODE'
assume cs:_TEXT
public _kb_checking
_kb_checking proc far
    mov ah,1
    int 16h
    jnz have_key
    xor ax,ax
have_key:
    or al,al
    jz done
    xor ah,ah
done:
    ret
_kb_checking endp
_TEXT ends
end
