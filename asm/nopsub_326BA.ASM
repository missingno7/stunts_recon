.8086
INCLUDE include\platform_hw.inc
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _nopsub_326BA
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 326BA.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, DX, SI, BP, SP, ES; balanced explicit saves/restores observed: SI; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
_nopsub_326BA proc far
    push bp
    mov bp, sp
    push si
    mov es, word ptr [bp+8]
    mov si, word ptr [bp+6]
    mov bx, word ptr [bp+0Ah]
    shl bx, 1
    shl bx, 1
    mov ax, word ptr es:[bx+si+6]
    mov dx, word ptr es:[bx+si+8]
    mov si, word ptr [bp+0Ch]
    mov word ptr [si], ax
    mov word ptr [si+2], dx
    pop si
    pop bp
    retf
_nopsub_326BA endp
S012_TEXT ends
end
