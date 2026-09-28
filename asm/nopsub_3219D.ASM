.8086
S012_TEXT segment byte public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _sprintf:far
extrn _nopsub_3215A:far
extrn _nopsub_3216C:far
extrn _byte_4032A:byte
extrn _byte_4032B:byte
extrn _byte_4032C:byte
extrn _word_4032E:word
extrn _word_40330:word
extrn _word_40332:word
extrn _word_40334:word
S012_TEXT segment byte public 'CODE'
public _nopsub_3219D
_nopsub_3219D proc far
    pop ax
    mov word ptr _word_40332, ax
    pop ax
    mov word ptr _word_40334, ax
    mov ax, 4BC6h
    push ax
    call far ptr _sprintf
    add sp, 2
    mov byte ptr _byte_4032B, 1
    mov ax, word ptr _word_4032E
    mov word ptr _word_40330, ax
begin_terminal_output:
    mov ax, word ptr _word_40334
    push ax
    mov ax, word ptr _word_40332
    push ax
    push si
    mov ax, 0B000h
    mov es, ax
    mov si, 4BC6h
    mov dh, byte ptr _byte_4032C
read_next_terminal_character:
    mov dl, byte ptr [si]
    inc si
    or dl, dl
    jz short finish_terminal_output
    cmp dl, 20h
    jl short check_terminal_control_code
    mov bx, word ptr _word_40330
    cmp bx, 0FA0h
    jl short write_terminal_character
    call far ptr _nopsub_3216C
    mov bx, 0F00h
    mov word ptr _word_40330, bx
write_terminal_character:
    mov word ptr es:[bx], dx
    add word ptr _word_40330, 2
    jmp short read_next_terminal_character
finish_terminal_output:
    cmp byte ptr _byte_4032B, 0
    jz short restore_terminal_cursor
    mov ax, word ptr _word_40330
    mov word ptr _word_4032E, ax
restore_terminal_cursor:
    pop si
    retf
check_terminal_control_code:
    cmp dl, 0Ah
    jnz short check_terminal_backspace
    cmp word ptr _word_40330, 0F00h
    jle short advance_terminal_row
    call far ptr _nopsub_3216C
    mov word ptr _word_40330, 0F00h
    jmp short read_next_terminal_character
advance_terminal_row:
    mov ax, word ptr _word_40330
    mov bx, 0A0h
    div bl
    xor ah, ah
    mul bl
    add ax, 0A0h
    mov word ptr _word_40330, ax
    jmp short read_next_terminal_character
check_terminal_backspace:
    cmp dl, 8
    jnz short check_terminal_clear_screen
    cmp word ptr _word_40330, 0
    jz short read_next_terminal_character
    sub word ptr _word_40330, 2
    jmp short read_next_terminal_character
check_terminal_clear_screen:
    cmp dl, 0Ch
    jnz short check_terminal_tab
    call far ptr _nopsub_3215A
    mov word ptr _word_40330, 0
    jmp read_next_terminal_character
check_terminal_tab:
    cmp dl, 9
    jnz short continue_terminal_output
    add word ptr _word_40330, 10h
    and word ptr _word_40330, 0FFF0h
continue_terminal_output:
    jmp read_next_terminal_character
restore_terminal_entry_state:
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
    mov ax, 4BC6h
    push ax
    call far ptr _sprintf
    sub sp, 2
    mov byte ptr _byte_4032B, 0
    jmp begin_terminal_output
set_terminal_row:
    push bp
    mov bp, sp
    mov ax, word ptr [bp+6]
    cmp ax, 0
    jge short clamp_terminal_row_minimum
    xor ax, ax
clamp_terminal_row_minimum:
    cmp ax, 18h
    jle short store_terminal_row
    mov ax, 18h
store_terminal_row:
    mov byte ptr _byte_4032A, al
    pop bp
    retf
_nopsub_3219D endp
S012_TEXT ends
end
