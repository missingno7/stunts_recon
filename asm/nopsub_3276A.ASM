.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
extrn _projection_y_scale:word
S012_TEXT segment word public 'CODE'
public _nopsub_3276A
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 3276A.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2 (word ptr 8).
; Register ABI: touches AX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: _projection_y_scale.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_3276A proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8

    push    bp
    mov     bp, sp
    mov     ax, _projection_y_scale
    mul     [bp+arg_0]
    div     [bp+arg_2]
    pop     bp
    retf
_nopsub_3276A endp
S012_TEXT ends
end
