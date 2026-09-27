.8086
_DATA segment word public 'DATA'
_DATA ends
_BSS segment word public 'BSS'
_BSS ends
CONST segment word public 'CONST'
CONST ends
DGROUP group CONST,_BSS,_DATA
extrn _kb_readchar_callback:word
extrn _readchar_callback_seg:word
extrn _timer_copy_unk:word
extrn _word_305F8:word
extrn _timer_get_counter:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT, ds:DGROUP
public _kb_call_readchar_callback
public _kb_read_char
public _kb_checking
public _nopsub_kb_set_readchar_callback
public _nopsub_kb_get_readchar_callback
public _flush_stdin
public _kb_check
public _nopsub_30A77
public _nopsub_30A97
_kb_call_readchar_callback proc far
    call    dword ptr ds:[_kb_readchar_callback]
    retf
_kb_call_readchar_callback endp
_kb_read_char proc far

    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jnz     short loc_30A2A
    xor     ax, ax
    retf
loc_30A2A:
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    or      al, al
    jz      short locret_30A34
    xor     ah, ah
locret_30A34:
    retf
_kb_read_char endp
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
_nopsub_kb_set_readchar_callback proc far
    push    bp
    mov     bp, sp
    mov     ax, [bp+6]
    mov     word ptr ds:[_kb_readchar_callback], ax
    mov     ax, [bp+8]
    mov     word ptr ds:[_readchar_callback_seg], ax
    pop     bp
    retf
_nopsub_kb_set_readchar_callback endp
_nopsub_kb_get_readchar_callback proc far
    mov     ax, word ptr ds:[_kb_readchar_callback]
    mov     dx, word ptr ds:[_readchar_callback_seg]
    retf
_nopsub_kb_get_readchar_callback endp
_flush_stdin proc far
    call    far ptr _kb_call_readchar_callback
    cmp     ax, 0
    jz      short _flush_stdin
    retf
_flush_stdin endp
_kb_check proc far

    mov     ah, 1
    int     16h             ; KEYBOARD - CHECK BUFFER, DO NOT CLEAR
    jnz     short loc_30A71
    xor     ax, ax
    retf
loc_30A71:
    mov     ah, 0
    int     16h             ; KEYBOARD - READ CHAR FROM BUFFER, WAIT IF EMPTY
    jmp     short near ptr _kb_check
_kb_check endp
_nopsub_30A77 proc far
    call    far ptr _kb_call_readchar_callback
    cmp     ax, 0
    jnz     short locret_30A96
    call    far ptr _timer_get_counter
    cmp     dx, word ptr ds:[_word_305F8]
    jb      short _nopsub_30A77
    ja      short loc_30A94
    cmp     ax, word ptr ds:[_timer_copy_unk]
    jb      short _nopsub_30A77
loc_30A94:
    xor     ax, ax
locret_30A96:
    retf
_nopsub_30A77 endp
_nopsub_30A97 proc far
    push    bp
    mov     bp, sp
    sub     sp, 4
    call    far ptr _timer_get_counter
    add     ax, [bp+6]
    adc     dx, [bp+8]
    mov     [bp-4], ax
    mov     [bp-2], dx
loc_30AAE:
    call    far ptr _kb_call_readchar_callback
    cmp     ax, 0
    jnz     short loc_30ACB
    call    far ptr _timer_get_counter
    cmp     dx, [bp-2]
    jb      short loc_30AAE
    ja      short loc_30AC9
    cmp     ax, [bp-4]
    jb      short loc_30AAE
loc_30AC9:
    xor     ax, ax
loc_30ACB:
    mov     sp, bp
    pop     bp
    retf
    db 0
_nopsub_30A97 endp
S012_TEXT ends
end
