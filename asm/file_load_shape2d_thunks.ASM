.8086
INCLUDE include\platform_hw.inc
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

; -----------------------------------------------------------------------------
; Purpose: Call the shape resource loader with fatal-error handling enabled.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_load_shape2d_res_fatal_thunk proc far

    jmp     _file_load_shape2d_res_fatal
_file_load_shape2d_res_fatal_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Call the shape resource loader with fatal-error handling disabled.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_load_shape2d_res_nofatal_thunk proc far

    jmp     _file_load_shape2d_res_nofatal
_load_shape2d_res_nofatal_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Forward to the shared shape resource loader.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_load_shape2d_res_thunk proc far

    jmp     _file_load_shape2d_res
_file_load_shape2d_res_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Forward to the shared 2D shape parser.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_parse_shape2d_thunk proc far

    jmp     _parse_shape2d
_parse_shape2d_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Call the shape loader with fatal-error handling enabled.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_load_shape2d_fatal_thunk proc far

    jmp     _file_load_shape2d_fatal
_file_load_shape2d_fatal_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Call the shape loader with fatal-error handling disabled.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_load_shape2d_nofatal_thunk proc far

    jmp     _file_load_shape2d_nofatal
_load_shape2d_nofatal_thunk endp
; -----------------------------------------------------------------------------
; Purpose: Forward to the shared 2D shape loader.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack arguments: no named stack arguments referenced.
; Register ABI: touches none; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_file_load_shape2d_thunk proc far

    jmp     _file_load_shape2d
    ; align 2
    db 0
_file_load_shape2d_thunk endp

S012_TEXT ends
end
