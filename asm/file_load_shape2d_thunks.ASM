.8086
S012_TEXT segment word public 'CODE'
S012_TEXT ends
assume cs:S012_TEXT
extrn _file_load_shape2d_res_fatal:far
extrn _file_load_shape2d_res_nofatal:far
extrn _file_load_shape2d_res:far
extrn _parse_shape2d:far
extrn _file_load_shape2d_fatal:far
extrn _file_load_shape2d_nofatal:far
extrn _file_load_shape2d:far
public _file_load_shape2d_res_fatal_thunk
public _load_shape2d_res_nofatal_thunk
public _file_load_shape2d_res_thunk
public _parse_shape2d_thunk
public _file_load_shape2d_fatal_thunk
public _load_shape2d_nofatal_thunk
public _file_load_shape2d_thunk
S012_TEXT segment word public 'CODE'

_file_load_shape2d_res_fatal_thunk proc far

    jmp     _file_load_shape2d_res_fatal
_file_load_shape2d_res_fatal_thunk endp
_load_shape2d_res_nofatal_thunk proc far

    jmp     _file_load_shape2d_res_nofatal
_load_shape2d_res_nofatal_thunk endp
_file_load_shape2d_res_thunk proc far

    jmp     _file_load_shape2d_res
_file_load_shape2d_res_thunk endp
_parse_shape2d_thunk proc far

    jmp     _parse_shape2d
_parse_shape2d_thunk endp
_file_load_shape2d_fatal_thunk proc far

    jmp     _file_load_shape2d_fatal
_file_load_shape2d_fatal_thunk endp
_load_shape2d_nofatal_thunk proc far

    jmp     _file_load_shape2d_nofatal
_load_shape2d_nofatal_thunk endp
_file_load_shape2d_thunk proc far

    jmp     _file_load_shape2d
    ; align 2
    db 0
_file_load_shape2d_thunk endp

S012_TEXT ends
end
