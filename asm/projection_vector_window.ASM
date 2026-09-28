.8086
VECTOR struc
vx dw ?
vy dw ?
vz dw ?
VECTOR ends
S012_TEXT segment word public 'CODE'
S012_TEXT ends
DGROUP group _DATA, CONST, _BSS
assume ds:DGROUP
_DATA segment word public 'DATA'
public _projection_half_width
public _projection_half_height
public _projection_left_origin
public _projection_top_origin
public _projection_center_x
public _projection_center_y
public _projection_x_scale
public _projection_y_scale
public _projection_x_angle
public _projection_y_angle
public _window_release_order_error_msg
_projection_half_width     dw 160
_projection_half_height     dw 100
_projection_left_origin     dw 0
_projection_top_origin     dw 0
_projection_center_x     dw 160
_projection_center_y     dw 100
_projection_x_scale     dw 0
_projection_y_scale     dw 0
_projection_x_angle     dw 0
_projection_y_angle     dw 0
_window_release_order_error_msg     db 87
    db 105
    db 110
    db 100
    db 111
    db 119
    db 32
    db 82
    db 101
    db 108
    db 101
    db 97
    db 115
    db 101
    db 100
    db 32
    db 79
    db 117
    db 116
    db 32
    db 111
    db 102
    db 32
    db 79
    db 114
    db 100
    db 101
    db 114
    db 13
    db 10
    db 0
    db 0
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
assume cs:S012_TEXT, ds:DGROUP
extrn _cosfast:far
extrn _polang:far
extrn _sinfast:far
extrn _mmgr_release:far
extrn _fatal_error:far
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_322C0
public _nopsub_322DF
public _set_projection
public _vector_to_point
public _sprite_free_window
_nopsub_322C0 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     _projection_left_origin, ax
    add     ax, _projection_half_width
    mov     _projection_center_x, ax
    mov     ax, [bp+arg_2]
    mov     _projection_top_origin, ax
    add     ax, _projection_half_height
    mov     _projection_center_y, ax
    pop     bp
    retf
_nopsub_322C0 endp
_nopsub_322DF proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_0]
    mov     _projection_x_angle, ax
    mov     ax, [bp+arg_2]
    mov     _projection_y_angle, ax
    jmp     short projection_join_01
    db 144
_nopsub_322DF endp
_set_projection proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    arg_4 = word ptr 10
    arg_6 = word ptr 12

    push    bp
    mov     bp, sp
    push    si
    push    di
    mov     ax, [bp+arg_0]
    xor     dh, dh
    mov     dl, ah
    mov     ah, al
    xor     al, al
    mov     cx, 3
projection_loop_head_01:
    shl     ax, 1
    rcl     dx, 1
    loop    projection_loop_head_01
    mov     cx, 168h
    div     cx
    shr     ax, 1
    mov     _projection_x_angle, ax
    mov     ax, [bp+arg_2]
    xor     dh, dh
    mov     dl, ah
    mov     ah, al
    xor     al, al
    mov     cx, 3
projection_loop_head_02:
    shl     ax, 1
    rcl     dx, 1
    loop    projection_loop_head_02
    mov     cx, 168h
    div     cx
    shr     ax, 1
    mov     _projection_y_angle, ax
projection_join_01:
    mov     ax, [bp+arg_4]
    shr     ax, 1
    mov     _projection_half_width, ax
    add     ax, _projection_left_origin
    mov     _projection_center_x, ax
    mov     ax, [bp+arg_6]
    shr     ax, 1
    mov     _projection_half_height, ax
    add     ax, _projection_top_origin
    mov     _projection_center_y, ax
    push    _projection_x_angle
    call _cosfast
    add     sp, 2
    mul     _projection_half_width
    mov     si, ax
    mov     di, dx
    push    _projection_x_angle
    call _sinfast
    add     sp, 2
    mov     cx, ax
    mov     dx, di
    mov     ax, si
    div     cx
    mov     _projection_x_scale, ax
    mov     bx, _projection_y_angle
    or      bx, bx
    jz      short projection_branch_path_01
    push    _projection_y_angle
    call _cosfast
    add     sp, 2
    mul     _projection_half_height
    mov     si, ax
    mov     di, dx
    push    _projection_y_angle
    call _sinfast
    add     sp, 2
    mov     cx, ax
    mov     dx, di
    mov     ax, si
    div     cx
    mov     _projection_y_scale, ax
    pop     di
    pop     si
    pop     bp
    retf
projection_branch_path_01:
    mov     bx, ax
    shr     bx, 1
    shr     bx, 1
    shr     bx, 1
    sub     ax, bx
    shr     bx, 1
    sub     ax, bx
    mov     _projection_y_scale, ax
    push    _projection_half_height
    push    ax
    call _polang
    add     sp, 4
    mov     _projection_y_angle, ax
    pop     di
    pop     si
    pop     bp
    retf
_set_projection endp
_vector_to_point proc far
    var_8000 = byte ptr -32768
     s = byte ptr 0
     r = byte ptr 2
    arg_vec = word ptr 6
    arg_point2dout = word ptr 8

    push    bp
    mov     bp, sp
    push    di
    push    si
    mov     si, [bp+arg_vec]
    mov     di, [bp+arg_point2dout]
    mov     cx, [si+VECTOR.vz]
    or      cx, cx
    jle     short vector_point_branch_path_04
    mov     ax, [si+VECTOR.vx]
    or      ax, ax
    jl      short vector_point_branch_path_02
    mul     _projection_x_scale
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short vector_point_branch_path_01
    inc     bx
vector_point_branch_path_01:
    cmp     cx, bx
    jle     short vector_point_branch_path_06
    div     cx
    add     ax, _projection_center_x
    jo      short vector_point_branch_path_05
    mov     [di], ax
    jmp     short vector_point_join_01
    db 144
vector_point_branch_path_02:
    neg     ax
    mul     _projection_x_scale
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short vector_point_branch_path_03
    inc     bx
vector_point_branch_path_03:
    cmp     cx, bx
    jle     short vector_point_branch_path_07
    div     cx
    neg     ax
    add     ax, _projection_center_x
    jo      short vector_point_branch_path_05
    mov     [di], ax
    jmp     short vector_point_join_01
    db 144
vector_point_branch_path_04:
    mov     ax, 32768
    mov     [di], ax
    mov     [di+2], ax
    jmp     short vector_point_loop_head_01

    db 144
vector_point_branch_path_05:
    or      ax, ax
    jge     short vector_point_branch_path_07
vector_point_branch_path_06:
    mov     ax, 7D00h
    mov     [di], ax
    jmp     short vector_point_join_01

    db 144
vector_point_branch_path_07:
    mov     ax, 8300h
    mov     [di], ax
vector_point_join_01:
    mov     ax, [si+VECTOR.vy]
    or      ax, ax
    jl      short vector_point_branch_path_09
    mul     _projection_y_scale
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short vector_point_branch_path_08
    inc     bx
vector_point_branch_path_08:
    cmp     cx, bx
    jle     short vector_point_branch_path_12
    div     cx
    neg     ax
    add     ax, _projection_center_y
    jo      short vector_point_branch_path_11
    mov     [di+2], ax
vector_point_loop_head_01:
    pop     si
    pop     di
    pop     bp
    retf
vector_point_branch_path_09:
    neg     ax
    mul     _projection_y_scale
    mov     bx, dx
    shl     bx, 1
    or      ax, ax
    jge     short vector_point_branch_path_10
    inc     bx
vector_point_branch_path_10:
    cmp     cx, bx
    jle     short vector_point_branch_path_13
    div     cx
    add     ax, _projection_center_y
    jo      short vector_point_branch_path_11
    mov     [di+2], ax
    jmp     short vector_point_loop_head_01
vector_point_branch_path_11:
    or      ax, ax
    jl      short vector_point_branch_path_13
vector_point_branch_path_12:
    mov     ax, -7D00h
    mov     [di+2], ax
    jmp     short vector_point_loop_head_01
vector_point_branch_path_13:
    mov     ax, 7D00h
    mov     [di+2], ax
    jmp     short vector_point_loop_head_01
_vector_to_point endp
_sprite_free_window proc far
    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     es, word ptr [bp+8]
    mov     di, word ptr [bp+6]
    mov     ds, word ptr es:[di+2]
    mov     si, word ptr es:[di]
    mov     ax, word ptr [si+2]
    add     ax, 0Fh
    shl     ax, 1
    mov     dx, ax
    add     dx, di
    mov     bx, word ptr cs:[62C4h]
    cmp     bx, dx
    jnz     short window_free_error_path_01
    sub     bx, ax
    mov     word ptr cs:[62C4h], bx
    push    ds
    push    si
    mov     ax, ss
    mov     ds, ax
    call    far ptr _mmgr_release
    add     sp, 4
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
window_free_error_path_01:
    mov     ax, ss
    mov     ds, ax
    lea     ax, ds:[_window_release_order_error_msg]
    push    ax
    call    far ptr _fatal_error
_sprite_free_window endp
S012_TEXT ends
end
