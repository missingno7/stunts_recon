.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_32738
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 32738.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_32738 proc far
    push bp
    mov bp, sp
    mov dx, word ptr [bp+8]
    mov ax, word ptr [bp+6]
    div word ptr [bp+0Ah]
    pop bp
    retf
_nopsub_32738 endp
S012_TEXT ends
end
