; ASMDOC Module review: Tentative name replaced by clipped indexed-pixel operation; one complete module retained.
; ASMDOC Scope: code-derived contracts below supersede broad legacy summaries without deleting them. No instructions,
; ASMDOC declarations, old comments or line endings changed.
; ASMDOC Public routines: _putpixel_single_maybe. Stack offsets are post-prologue BP offsets.
.8086
INCLUDE include\platform_hw.inc
extrn _sprite1:byte
S012_TEXT segment word public 'CODE'
assume cs:S012_TEXT
public _putpixel_single_maybe
; -----------------------------------------------------------------------------
; Purpose: Write one pixel to the selected video page.
; ABI: FAR entry; source return form: RET mnemonic for FAR entry; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, SI, DI, BP, SP, CS, ES; balanced explicit saves/restores observed: DI, SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; /* PLATFORM(video): accesses display memory through ES. */
; -----------------------------------------------------------------------------
; ASMDOC Contract _putpixel_single_maybe; proposed name _sprite_putpixel_clipped
; ASMDOC Purpose: Plot one low-byte color only if signed x/y lie inside sprite1 half-open bounds.
; ASMDOC Convention: 16-bit far cdecl; caller removes arguments; offsets below are from BP after PUSH BP / MOV BP,SP (entry SP
; ASMDOC offsets are two bytes smaller).
; ASMDOC Inputs: [BP+06] signed x; [BP+08] signed y; [BP+0A] color word (AL only).
; ASMDOC Outputs: Accepted pixel writes ES: (CS:row_offsets[y]+x); rejected pixel writes nothing; On accepted path AX=color and
; ASMDOC ES=bitmap segment; reject AX is last tested coordinate.
; ASMDOC Preserves: BP; SI; DI; DS; SS; CS; SP balanced except consumed far return; BX; CX; DX.
; ASMDOC Clobbers: AX; ES only on accepted path; arithmetic flags; DF unchanged.
; ASMDOC Globals read: _sprite1 left(+0C),right(+0E),top(+10),bottom(+12),CS row-table pointer(+0A),bitmap segment(+2); CS row
; ASMDOC offsets.
; ASMDOC Globals written: sprite1 backing pixel on accepted path.
; ASMDOC Services: none.
; ASMDOC Dead: false. Reachability: Direct callers asm/vector_sphere_sprite_ops.ASM:143 and src/obj_seg006.c:690.
; ASMDOC Edge cases: RET spelling is inside PROC FAR (MASM far return), not evidence of a near ABI. Unlabeled unchecked prologue
; ASMDOC at lines 35..38 is skipped by normal control flow; accepted path jumps to write_pixel at 39.
_putpixel_single_maybe proc far
 push bp
 mov bp,sp
 push si
 push di
 mov ax,word ptr [bp+6]
 cmp ax,word ptr cs:[_sprite1+12]
 jl short reject_pixel
 cmp ax,word ptr cs:[_sprite1+14]
 jge short reject_pixel
 mov ax,word ptr [bp+8]
 cmp ax,word ptr cs:[_sprite1+16]
 jl short reject_pixel
 cmp ax,word ptr cs:[_sprite1+18]
 jl short write_pixel
reject_pixel:
 pop di
 pop si
 pop bp
 ret
; ASMDOC Unexported unchecked prologue lies after rejected-pixel far return; ordinary accepted path jumps over it to write_pixel.
 push bp
 mov bp,sp
 push si
 push di
; ASMDOC Accepted path uses signed half-open clip bounds tested above; row table is in CS and descriptor bitmap offset is not
; ASMDOC separately added.
write_pixel:
 mov di,word ptr [bp+6]
 mov si,word ptr [bp+8]
 shl si,1
 add si,word ptr cs:[_sprite1+10]
 add di,word ptr cs:[si]
 mov es,word ptr cs:[_sprite1+2]
 mov ax,word ptr [bp+0Ah]
 mov byte ptr es:[di],al ;/* PLATFORM(video): accesses display memory through ES. */
 pop di
 pop si
 pop bp
 ret
 db 0
_putpixel_single_maybe endp
S012_TEXT ends
end
