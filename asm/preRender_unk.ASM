.8086
DGROUP group DSEG
DSEG segment byte public 'STUNTSD'
assume ds:DSEG
extrn _word_4031E:word
extrn _word_40320:word
extrn _spritefunc:word
extrn _imagefunc:word
DSEG ends
extrn _draw_unknown_lines:near
extrn _preRender_line:near
extrn _loc_3180A:near
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _preRender_unk, _nopsub_2F424
_preRender_unk proc far
    var_A = byte ptr -10
     s = byte ptr 0
     r = byte ptr 2
    arg_2 = word ptr 6
    arg_4 = word ptr 8
    arg_6 = word ptr 12
    arg_8 = word ptr 14

    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_8]
    mov     [bp+var_A], 1
    jmp     short loc_2F3FD
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_8]
    mov     [bp+var_A], 0
loc_2F3FD:
    mov     ax, [bp+arg_2]
    mov     _word_4031E, ax
    mov     ax, [bp+0Ah]
    mov     _word_40320, ax
    mov     ax, [bp+arg_4]
    mov     [bp+arg_2], ax
    mov     ax, [bp+arg_6]
    mov     [bp+arg_4], ax
    mov     ax, offset _draw_unknown_lines
    mov     _spritefunc, ax
    mov     ax, offset _preRender_line
    mov     _imagefunc, ax
    jmp     _loc_3180A

_preRender_unk endp
_nopsub_2F424 proc far
    var_A = byte ptr -10
     s = byte ptr 0
     r = byte ptr 2
    arg_8 = byte ptr 14

    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    lea     si, [bp+arg_8]
    mov     [bp+var_A], 0
    jmp     short loc_2F3FD

_nopsub_2F424 endp
_TEXT ends
end
