.model medium

dseg segment byte public 'STUNTSD'
    extrn _old_intr0_handler:dword
    extrn _word_3BE30:word
    extrn _word_3BE32:word
    extrn _g_kevinrandom_seed:byte
dseg ends

S002_TEXT segment byte public 'CODE'
    assume cs:S002_TEXT
    assume es:nothing, ss:nothing, ds:dseg
    public _update_rpm_from_speed
    public _nopsub_19DE8
    public _nopsub_19DFF
    public _nopsub_19E09
    public _nopsub_19E13
    public _init_kevinrandom
    public _get_kevinrandom_seed
    public _get_kevinrandom
    public _intr0_handler
    public _init_div0
    public _byte_19F07
_update_rpm_from_speed proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_currRpm = word ptr 6
    arg_speed = word ptr 8
    arg_gearRatio = word ptr 10
    arg_isChangingGear = word ptr 12
    arg_idleRpm = word ptr 14

    push    bp
    mov     bp, sp
    push    bp
    mov     cx, [bp+arg_currRpm]
    mov     ax, [bp+arg_speed]
    cmp     [bp+arg_isChangingGear], 0
    jnz     short loc_19DDB
    mul     [bp+arg_gearRatio]
    mov     cx, dx
loc_19DDB:
    cmp     cx, [bp+arg_idleRpm]
    jnb     short loc_19DE3
    mov     cx, [bp+arg_idleRpm]
loc_19DE3:
    mov     ax, cx
    pop     bp
    pop     bp
    retf
_update_rpm_from_speed endp
_nopsub_19DE8 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
loc_19DE9:
    mov     bp, sp
    push    bp
loc_19DEC:
    xor     ax, ax
loc_19DEE:
    mov     bx, [bp+arg_0]
loc_19DF1:
    or      bx, bx
loc_19DF3:
    jz      short loc_19DFC
loc_19DF5:
    jl      short loc_19DFB
loc_19DF7:
    inc     ax
loc_19DF8:
    pop     bp
    pop     bp
locret_19DFA:
    retf
loc_19DFB:
    dec     ax
loc_19DFC:
    pop     bp
    pop     bp
locret_19DFE:
    retf
_nopsub_19DE8 endp
_nopsub_19DFF proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
loc_19E00:
    mov     bp, sp
loc_19E02:
    mov     ax, [bp+arg_0]
loc_19E05:
    int     61h             ; reserved for user interrupt
    pop     bp
locret_19E08:
    retf
_nopsub_19DFF endp
_nopsub_19E09 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
loc_19E0A:
    mov     bp, sp
loc_19E0C:
    mov     ax, [bp+arg_0]
    int     60h
    pop     bp
    retf
_nopsub_19E09 endp
_nopsub_19E13 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
loc_19E14:
    mov     bp, sp
loc_19E16:
    push    bp
    push    si
    mov     si, [bp+arg_0]
loc_19E1B:
    int     62h             ; reserved for user interrupt
    pop     si
loc_19E1E:
    pop     bp
    pop     bp
    retf
_nopsub_19E13 endp
_init_kevinrandom proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
loc_19E24:
    push    bp
    mov     bx, [bp+arg_0]
    mov     al, [bx]
loc_19E2A:
    mov     _g_kevinrandom_seed, al
    mov     al, [bx+1]
loc_19E30:
    mov     _g_kevinrandom_seed+1, al
loc_19E33:
    mov     al, [bx+2]
loc_19E36:
    mov     _g_kevinrandom_seed+2, al
    mov     al, [bx+3]
    mov     _g_kevinrandom_seed+3, al
    mov     al, [bx+4]
    mov     _g_kevinrandom_seed+4, al
    mov     al, [bx+5]
    mov     _g_kevinrandom_seed+5, al
    pop     bp
loc_19E4C:
    pop     bp
    retf
_init_kevinrandom endp
_get_kevinrandom_seed proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    push    bp
    mov     bx, [bp+arg_0]
    mov     al, _g_kevinrandom_seed
    mov     [bx], al
    mov     al, _g_kevinrandom_seed+1
    mov     [bx+1], al
    mov     al, _g_kevinrandom_seed+2
    mov     [bx+2], al
    mov     al, _g_kevinrandom_seed+3
    mov     [bx+3], al
    mov     al, _g_kevinrandom_seed+4
    mov     [bx+4], al
loc_19E72:
    mov     al, _g_kevinrandom_seed+5
    mov     [bx+5], al
    pop     bp
    pop     bp
    retf
_get_kevinrandom_seed endp
_get_kevinrandom proc far

    mov     al, _g_kevinrandom_seed+5
loc_19E7E:
    add     al, _g_kevinrandom_seed+4
loc_19E82:
    mov     _g_kevinrandom_seed+4, al
loc_19E85:
    add     al, _g_kevinrandom_seed+3
loc_19E89:
    mov     _g_kevinrandom_seed+3, al
loc_19E8C:
    add     al, _g_kevinrandom_seed+2
loc_19E90:
    mov     _g_kevinrandom_seed+2, al
    add     al, _g_kevinrandom_seed+1
loc_19E97:
    mov     _g_kevinrandom_seed+1, al
    add     al, _g_kevinrandom_seed
    mov     _g_kevinrandom_seed, al
    inc     _g_kevinrandom_seed+5
    jnz     short loc_19EC3
    inc     _g_kevinrandom_seed+4
    jnz     short loc_19EC3
    inc     _g_kevinrandom_seed+3
    jnz     short loc_19EC3
    inc     _g_kevinrandom_seed+2
    jnz     short loc_19EC3
    inc     _g_kevinrandom_seed+1
    jnz     short loc_19EC3
    inc     _g_kevinrandom_seed
loc_19EC3:
    mov     al, _g_kevinrandom_seed
    xor     ah, ah
    retf
_get_kevinrandom endp
_intr0_handler proc far
     s = byte ptr 0
     r = byte ptr 2

    push    bp
    mov     bp, sp
    push    ds
    mov     ax, seg dseg
loc_19ED0:
    mov     ds, ax
loc_19ED2:
    mov     ax, [bp+4]
loc_19ED5:
    mov     _word_3BE30, ax
loc_19ED8:
    mov     ax, [bp+2]
    mov     _word_3BE32, ax
loc_19EDE:
    inc     ax
    inc     ax
loc_19EE0:
    mov     [bp+2], ax
loc_19EE3:
    xor     ax, ax
    pop     ds
    pop     bp
    iret
_intr0_handler endp
_init_div0 proc far

    push    ds
loc_19EE9:
    mov     ah, 35h ; '5'
loc_19EEB:
    mov     al, 0
    int     21h             ; DOS - 2+ - GET INTERRUPT VECTOR
loc_19EEF:
    mov     word ptr _old_intr0_handler+2, es
loc_19EF3:
    mov     word ptr _old_intr0_handler, bx
loc_19EF7:
    mov     dx, seg S002_TEXT
loc_19EFA:
    mov     ds, dx
loc_19EFC:
    mov     dx, offset _intr0_handler
loc_19EFF:
    mov     ah, 25h ; '%'
loc_19F01:
    mov     al, 0
    int     21h             ; DOS - SET INTERRUPT VECTOR
    pop     ds
locret_19F06:
    retf
_byte_19F07     label byte
    push    ds
    lds     dx, _old_intr0_handler
    mov     ah, 25h
    mov     al, 0
    int     21h
_init_div0 endp
tail_19F13:
    pop ds
    retf
S002_TEXT ends
end
