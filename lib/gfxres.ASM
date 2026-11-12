; ASMDOC REVIEW: graphics_resource_runtime (candidate, comments only).
; Retain: one proven object combines angle/line tables and scan conversion with elapsed deadlines, DOS
; resource sizing/search, cleanup and critical-error services; narrower naming would hide substantial content.
; Scope: documented public code entries below; two exported data words/bytes are not routines.
; New ASMDOC comments correct older campaign boilerplate retained verbatim below.
; BP offsets are after PUSH BP / MOV BP,SP unless an interrupt/entry-SP offset is explicit.
; Preservation for DOS/BIOS/opaque callbacks is conditional where stated; FLAGS are not C return values.
; Static liveness means an accepted source reference or installed vector/callback; dead means no known inbound
; source edge, not a runtime coverage claim.
; Routine _fatal_error: Print the supplied diagnostic, dispatch cleanup callbacks and abort the process.
; Routine _polang: Compute a table-rounded signed polar angle in 1024 units per turn.
; Routine _sub_2EAD4 -> _timer_read_elapsed_ticks: Read a coherent low/high elapsed-tick snapshot while
; interrupts are masked.
; Routine _set_add_value: Store elapsed ticks plus a supplied 32-bit delay as the input deadline.
; Routine _poll_input_abort: Test whether both elapsed counter words have reached their saved deadline words.
; Routine _wait_for_input_delay: Busy-wait until both elapsed words reach a local elapsed-plus-delay deadline.
; Routine _draw_line_related_alt: Build a nominally unclipped fixed-point line descriptor without drawing
; pixels.
; Routine _draw_line_related: Build a clipped fixed-point line descriptor without drawing pixels.
; Routine _parse_shape2d_helper: Convert a real-mode segment:offset pointer into its unsigned linear byte
; position.
; Routine _parse_shape2d_helper2: Convert a 32-bit byte position to a normalized real-mode pointer.
; Routine _criterr_interrupt_handler: Call the selected critical-error callback and return its AX result to
; DOS.
; Routine _set_criterr_handler: Register cleanup, save INT24 and install a game critical-error callback.
; Routine _criterr_exithandler: Restore the saved DOS critical-error vector unless its segment equals its
; offset.
; Routine _preRender_unk -> _sprite_two_color_poly_list: Select two-color patterned polygon callbacks and
; enter shared scan conversion using a point-list pointer.
; Routine _nopsub_2F424 -> _sprite_two_color_poly_args_alt: Select two-color patterned polygon callbacks and
; enter shared scan conversion using inline arguments.
; Routine _preRender_line: Clip a line into a local descriptor and rasterize it when visible and nonempty.
; Routine _add_exit_handler: Append a unique far cleanup callback to a ten-slot table or abort on overflow.
; Routine _call_exitlist: Invoke all nonzero-segment cleanup callbacks from the highest slot to the lowest.
; Routine _call_exitlist2: Dispatch cleanup callbacks then terminate with CRT exit status zero.
; Routine _file_paras: Read physical file length and return its rounded-up paragraph count.
; Routine _file_paras_nofatal: Read physical file length and return its rounded-up paragraph count with
; nonfatal policy.
; Routine _file_paras_fatal: Read physical file length and return its rounded-up paragraph count with fatal
; policy.
; Routine _file_decomp_paras: Read packed header output length and return its rounded-up paragraph count.
; Routine _file_decomp_paras_nofatal: Read packed header output length and return its rounded-up paragraph
; count with nonfatal policy.
; Routine _file_decomp_paras_fatal: Read packed header output length and return its rounded-up paragraph count
; with fatal policy.
; Routine _file_find: Start a DOS file search and return a shared full path with the matched filename.
; Routine _file_find_next: Continue the shared DOS search and overwrite the prior result filename in place.
; Routine _mulscl: Multiply signed words and return a rounded fixed-point product scaled by 2^14.
; Routine _video_set_mode4: Select BIOS mode 4, program monochrome CRTC timing and clear 32 KiB at B800:0000.
; Private alternate: preRender_unk_mode_1 -> sprite_two_color_poly_args; inline list, mode 1, unreferenced.
; Public DATA: _prerender_max_line_width; _video_mode4_active.
INCLUDE include\platform_hw.inc
DSEG segment byte public 'STUNTSD'
extrn _timer_elapsed_ticks_low:word
extrn _timer_elapsed_ticks_high:word
extrn _line_pattern_bits:word
extrn _prerender_auxiliary_arg:word
extrn _spritefunc:word
extrn _imagefunc:word
DSEG ends
extrn _exit:far
extrn _printf:far
extrn _abort:far
extrn _displaywindow:far
extrn _flush_stdin:far
extrn _drawcline:far
extrn _sprite1:word
extrn _draw_unknown_lines:far
extrn _polycmn:near
_DATA segment word public 'DATA'
; Octant angles: floor(atan(i/256)*512/pi+0.5), i=0..256.
; Nearest integer, positive halves up. 257/257 match; final byte is zero padding (258/258).
_atantable db 00h, 01h, 01h, 02h, 03h, 03h, 04h, 04h, 05h, 06h, 06h, 07h
    db 08h, 08h, 09h, 0Ah, 0Ah, 0Bh, 0Bh, 0Ch, 0Dh, 0Dh, 0Eh, 0Fh
    db 0Fh, 10h, 10h, 11h, 12h, 12h, 13h, 14h, 14h, 15h, 16h, 16h
    db 17h, 17h, 18h, 19h, 19h, 1Ah, 1Bh, 1Bh, 1Ch, 1Ch, 1Dh, 1Eh
    db 1Eh, 1Fh, 1Fh, 20h, 21h, 21h, 22h, 22h, 23h, 24h, 24h, 25h
    db 26h, 26h, 27h, 27h, 28h, 29h, 29h, 2Ah, 2Ah, 2Bh, 2Ch, 2Ch
    db 2Dh, 2Dh, 2Eh, 2Eh, 2Fh, 30h, 30h, 31h, 31h, 32h, 33h, 33h
    db 34h, 34h, 35h, 35h, 36h, 37h, 37h, 38h, 38h, 39h, 39h, 3Ah
    db 3Ah, 3Bh, 3Ch, 3Ch, 3Dh, 3Dh, 3Eh, 3Eh, 3Fh, 3Fh, 40h, 41h
    db 41h, 42h, 42h, 43h, 43h, 44h, 44h, 45h, 45h, 46h, 46h, 47h
    db 47h, 48h, 48h, 49h, 4Ah, 4Ah, 4Bh, 4Bh, 4Ch, 4Ch, 4Dh, 4Dh
    db 4Eh, 4Eh, 4Fh, 4Fh, 50h, 50h, 51h, 51h, 52h, 52h, 53h, 53h
    db 54h, 54h, 54h, 55h, 55h, 56h, 56h, 57h, 57h, 58h, 58h, 59h
    db 59h, 5Ah, 5Ah, 5Bh, 5Bh, 5Bh, 5Ch, 5Ch, 5Dh, 5Dh, 5Eh, 5Eh
    db 5Fh, 5Fh, 60h, 60h, 60h, 61h, 61h, 62h, 62h, 63h, 63h, 63h
    db 64h, 64h, 65h, 65h, 66h, 66h, 66h, 67h, 67h, 68h, 68h, 68h
    db 69h, 69h, 6Ah, 6Ah, 6Ah, 6Bh, 6Bh, 6Ch, 6Ch, 6Ch, 6Dh, 6Dh
    db 6Eh, 6Eh, 6Eh, 6Fh, 6Fh, 70h, 70h, 70h, 71h, 71h, 71h, 72h
    db 72h, 73h, 73h, 73h, 74h, 74h, 74h, 75h, 75h, 76h, 76h, 76h
    db 77h, 77h, 77h, 78h, 78h, 78h, 79h, 79h, 79h, 7Ah, 7Ah, 7Ah
    db 7Bh, 7Bh, 7Bh, 7Ch, 7Ch, 7Ch, 7Dh, 7Dh, 7Dh, 7Eh, 7Eh, 7Eh
    db 7Fh, 7Fh, 7Fh, 80h, 80h, 00h
input_deadline_tick_low dw 0
input_deadline_tick_high dw 0
; Expand each input bit to two mask bits: sum(3*bit(i,b)*4^b), b=0..7, i=0..255.
; Exact integers; verified 256/256 entries, no exceptions.
expanded_8bit_pixel_mask_table label word
mask_index = 0
    REPT 256
mask_word = 0
mask_source = mask_index
mask_pair = 03h
    REPT 8
mask_word = mask_word + (mask_source AND 01h) * mask_pair
mask_source = mask_source SHR 1
mask_pair = mask_pair SHL 2
    ENDM
    dw mask_word
mask_index = mask_index + 1
    ENDM
_prerender_max_line_width dw 00028h
sphere_scanline_profiles dw offset DGROUP:sphere_scanline_profile_00
    dw offset DGROUP:sphere_scanline_profile_00
    dw offset DGROUP:sphere_scanline_profile_01
    dw offset DGROUP:sphere_scanline_profile_02
    dw offset DGROUP:sphere_scanline_profile_03
    dw offset DGROUP:sphere_scanline_profile_04
    dw offset DGROUP:sphere_scanline_profile_05
    dw offset DGROUP:sphere_scanline_profile_06
    dw offset DGROUP:sphere_scanline_profile_07
    dw offset DGROUP:sphere_scanline_profile_08
    dw offset DGROUP:sphere_scanline_profile_09
    dw offset DGROUP:sphere_scanline_profile_10
    dw offset DGROUP:sphere_scanline_profile_11
    dw offset DGROUP:sphere_scanline_profile_12
    dw offset DGROUP:sphere_scanline_profile_13
    dw offset DGROUP:sphere_scanline_profile_14
    dw offset DGROUP:sphere_scanline_profile_15
    dw offset DGROUP:sphere_scanline_profile_16
    dw offset DGROUP:sphere_scanline_profile_17
    dw offset DGROUP:sphere_scanline_profile_18
    dw offset DGROUP:sphere_scanline_profile_19
    dw offset DGROUP:sphere_scanline_profile_20
    dw offset DGROUP:sphere_scanline_profile_21
    dw offset DGROUP:sphere_scanline_profile_22
    dw offset DGROUP:sphere_scanline_profile_23
    dw offset DGROUP:sphere_scanline_profile_24
    dw offset DGROUP:sphere_scanline_profile_25
    dw offset DGROUP:sphere_scanline_profile_26
    dw offset DGROUP:sphere_scanline_profile_27
    dw offset DGROUP:sphere_scanline_profile_28
    dw offset DGROUP:sphere_scanline_profile_29
    dw offset DGROUP:sphere_scanline_profile_30
    dw offset DGROUP:sphere_scanline_profile_31
    dw offset DGROUP:sphere_scanline_profile_32
    dw offset DGROUP:sphere_scanline_profile_33
    dw offset DGROUP:sphere_scanline_profile_34
    dw offset DGROUP:sphere_scanline_profile_35
    dw offset DGROUP:sphere_scanline_profile_36
    dw offset DGROUP:sphere_scanline_profile_37
    dw offset DGROUP:sphere_scanline_profile_38
; Radius 1: floor(1.25*sqrt(1^2-(1-1-i)^2)+0.5), i=0..0.
; Nearest, positive halves up; verified 1/1, no exceptions.
sphere_scanline_profile_00 db 01h
; Radius 2: floor(1.25*sqrt(2^2-(2-1-i)^2)+0.5), i=0..1.
; Nearest, positive halves up; verified 2/2, no exceptions.
sphere_scanline_profile_01 db 02h, 03h
; Radius 3: floor(1.25*sqrt(3^2-(3-1-i)^2)+0.5), i=0..2.
; Nearest, positive halves up; verified 3/3, no exceptions.
sphere_scanline_profile_02 db 03h, 04h, 04h
; Radius 4: floor(1.25*sqrt(4^2-(4-1-i)^2)+0.5), i=0..3.
; Nearest, positive halves up; verified 4/4, no exceptions.
sphere_scanline_profile_03 db 03h, 04h, 05h, 05h
; Radius 5: floor(1.25*sqrt(5^2-(5-1-i)^2)+0.5), i=0..4.
; Nearest, positive halves up; verified 5/5, no exceptions.
sphere_scanline_profile_04 db 04h, 05h, 06h, 06h, 06h
; Radius 6: floor(1.25*sqrt(6^2-(6-1-i)^2)+0.5), i=0..5.
; Nearest, positive halves up; verified 6/6, no exceptions.
sphere_scanline_profile_05 db 04h, 06h, 06h, 07h, 07h, 08h
; Radius 7: floor(1.25*sqrt(7^2-(7-1-i)^2)+0.5), i=0..6.
; Nearest, positive halves up; verified 7/7, no exceptions.
sphere_scanline_profile_06 db 05h, 06h, 07h, 08h, 08h, 09h, 09h
; Radius 8: floor(1.25*sqrt(8^2-(8-1-i)^2)+0.5), i=0..7.
; Nearest, positive halves up; verified 8/8, no exceptions.
sphere_scanline_profile_07 db 05h, 07h, 08h, 09h, 09h, 0Ah, 0Ah, 0Ah
; Radius 9: floor(1.25*sqrt(9^2-(9-1-i)^2)+0.5), i=0..8.
; Nearest, positive halves up; verified 9/9, no exceptions.
sphere_scanline_profile_08 db 05h, 07h, 08h, 09h, 0Ah, 0Bh, 0Bh, 0Bh, 0Bh
; Radius 10: floor(1.25*sqrt(10^2-(10-1-i)^2)+0.5), i=0..9.
; Nearest, positive halves up; verified 10/10, no exceptions.
sphere_scanline_profile_09 db 05h, 08h, 09h, 0Ah, 0Bh, 0Bh, 0Ch, 0Ch, 0Ch, 0Dh
; Radius 11: floor(1.25*sqrt(11^2-(11-1-i)^2)+0.5), i=0..10.
; Nearest, positive halves up; verified 11/11, no exceptions.
sphere_scanline_profile_10 db 06h, 08h, 09h, 0Bh, 0Ch, 0Ch, 0Dh, 0Dh, 0Eh, 0Eh, 0Eh
; Radius 12: floor(1.25*sqrt(12^2-(12-1-i)^2)+0.5), i=0..11.
; Nearest, positive halves up; verified 12/12, no exceptions.
sphere_scanline_profile_11 db 06h, 08h, 0Ah, 0Bh, 0Ch, 0Dh, 0Eh, 0Eh, 0Fh, 0Fh, 0Fh, 0Fh
; Radius 13: floor(1.25*sqrt(13^2-(13-1-i)^2)+0.5), i=0..12.
; Nearest, positive halves up; verified 13/13, no exceptions.
sphere_scanline_profile_12 db 06h, 09h, 0Ah, 0Ch, 0Dh, 0Eh, 0Eh, 0Fh, 0Fh, 10h, 10h, 10h
    db 10h
; Radius 14: floor(1.25*sqrt(14^2-(14-1-i)^2)+0.5), i=0..13.
; Nearest, positive halves up; verified 14/14, no exceptions.
sphere_scanline_profile_13 db 06h, 09h, 0Bh, 0Ch, 0Dh, 0Eh, 0Fh, 10h, 10h, 11h, 11h, 11h
    db 11h, 12h
; Radius 15: floor(1.25*sqrt(15^2-(15-1-i)^2)+0.5), i=0..14.
; Nearest, positive halves up; verified 15/15, no exceptions.
sphere_scanline_profile_14 db 07h, 09h, 0Bh, 0Dh, 0Eh, 0Fh, 10h, 11h, 11h, 12h, 12h, 12h
    db 13h, 13h, 13h
; Radius 16: floor(1.25*sqrt(16^2-(16-1-i)^2)+0.5), i=0..15.
; Nearest, positive halves up; verified 16/16, no exceptions.
sphere_scanline_profile_15 db 07h, 0Ah, 0Ch, 0Dh, 0Fh, 10h, 11h, 11h, 12h, 13h, 13h, 13h
    db 14h, 14h, 14h, 14h
; Radius 17: floor(1.25*sqrt(17^2-(17-1-i)^2)+0.5), i=0..16.
; Nearest, positive halves up; verified 17/17, no exceptions.
sphere_scanline_profile_16 db 07h, 0Ah, 0Ch, 0Eh, 0Fh, 10h, 11h, 12h, 13h, 13h, 14h, 14h
    db 15h, 15h, 15h, 15h, 15h
; Radius 18: floor(1.25*sqrt(18^2-(18-1-i)^2)+0.5), i=0..17.
; Nearest, positive halves up; verified 18/18, no exceptions.
sphere_scanline_profile_17 db 07h, 0Ah, 0Ch, 0Eh, 10h, 11h, 12h, 13h, 13h, 14h, 15h, 15h
    db 16h, 16h, 16h, 16h, 16h, 17h
; Radius 19: floor(1.25*sqrt(19^2-(19-1-i)^2)+0.5), i=0..18.
; Nearest, positive halves up; verified 19/19, no exceptions.
sphere_scanline_profile_18 db 08h, 0Bh, 0Dh, 0Fh, 10h, 11h, 12h, 13h, 14h, 15h, 16h, 16h
    db 17h, 17h, 17h, 17h, 18h, 18h, 18h
; Radius 20: floor(1.25*sqrt(20^2-(20-1-i)^2)+0.5), i=0..19.
; Nearest, positive halves up; verified 20/20, no exceptions.
sphere_scanline_profile_19 db 08h, 0Bh, 0Dh, 0Fh, 11h, 12h, 13h, 14h, 15h, 16h, 16h, 17h
    db 17h, 18h, 18h, 18h, 19h, 19h, 19h, 19h
; Radius 21: floor(1.25*sqrt(21^2-(21-1-i)^2)+0.5), i=0..20.
; Nearest, positive halves up; verified 21/21, no exceptions.
sphere_scanline_profile_20 db 08h, 0Bh, 0Eh, 0Fh, 11h, 12h, 14h, 15h, 16h, 16h, 17h, 18h
    db 18h, 19h, 19h, 19h, 1Ah, 1Ah, 1Ah, 1Ah, 1Ah
; Radius 22: floor(1.25*sqrt(22^2-(22-1-i)^2)+0.5), i=0..21.
; Nearest, positive halves up; verified 22/22, no exceptions.
sphere_scanline_profile_21 db 08h, 0Bh, 0Eh, 10h, 11h, 13h, 14h, 15h, 16h, 17h, 18h, 18h
    db 19h, 1Ah, 1Ah, 1Ah, 1Bh, 1Bh, 1Bh, 1Bh, 1Bh, 1Ch
; Radius 23: floor(1.25*sqrt(23^2-(23-1-i)^2)+0.5), i=0..22.
; Nearest, positive halves up; verified 23/23, no exceptions.
sphere_scanline_profile_22 db 08h, 0Ch, 0Eh, 10h, 12h, 13h, 15h, 16h, 17h, 18h, 19h, 19h
    db 1Ah, 1Ah, 1Bh, 1Bh, 1Ch, 1Ch, 1Ch, 1Dh, 1Dh, 1Dh, 1Dh
; Radius 24: floor(1.25*sqrt(24^2-(24-1-i)^2)+0.5), i=0..23.
; Nearest, positive halves up; verified 24/24, no exceptions.
sphere_scanline_profile_23 db 09h, 0Ch, 0Fh, 11h, 12h, 14h, 15h, 16h, 17h, 18h, 19h, 1Ah
    db 1Bh, 1Bh, 1Ch, 1Ch, 1Dh, 1Dh, 1Dh, 1Eh, 1Eh, 1Eh, 1Eh, 1Eh
; Radius 25: floor(1.25*sqrt(25^2-(25-1-i)^2)+0.5), i=0..24.
; Nearest, positive halves up; verified 25/25, no exceptions.
sphere_scanline_profile_24 db 09h, 0Ch, 0Fh, 11h, 13h, 14h, 16h, 17h, 18h, 19h, 1Ah, 1Bh
    db 1Bh, 1Ch, 1Dh, 1Dh, 1Eh, 1Eh, 1Eh, 1Fh, 1Fh, 1Fh, 1Fh, 1Fh
    db 1Fh
; Radius 26: floor(1.25*sqrt(26^2-(26-1-i)^2)+0.5), i=0..25.
; Nearest, positive halves up; verified 26/26, no exceptions.
sphere_scanline_profile_25 db 09h, 0Dh, 0Fh, 11h, 13h, 15h, 16h, 17h, 19h, 1Ah, 1Bh, 1Bh
    db 1Ch, 1Dh, 1Dh, 1Eh, 1Eh, 1Fh, 1Fh, 20h, 20h, 20h, 20h, 20h
    db 20h, 21h
; Radius 27: floor(1.25*sqrt(27^2-(27-1-i)^2)+0.5), i=0..26.
; Nearest, positive halves up; verified 27/27, no exceptions.
sphere_scanline_profile_26 db 09h, 0Dh, 0Fh, 12h, 14h, 15h, 17h, 18h, 19h, 1Ah, 1Bh, 1Ch
    db 1Dh, 1Eh, 1Eh, 1Fh, 1Fh, 20h, 20h, 21h, 21h, 21h, 21h, 22h
    db 22h, 22h, 22h
; Radius 28: floor(1.25*sqrt(28^2-(28-1-i)^2)+0.5), i=0..27.
; Nearest, positive halves up; verified 28/28, no exceptions.
sphere_scanline_profile_27 db 09h, 0Dh, 10h, 12h, 14h, 16h, 17h, 18h, 1Ah, 1Bh, 1Ch, 1Dh
    db 1Eh, 1Eh, 1Fh, 20h, 20h, 21h, 21h, 22h, 22h, 22h, 22h, 23h
    db 23h, 23h, 23h, 23h
; Radius 29: floor(1.25*sqrt(29^2-(29-1-i)^2)+0.5), i=0..28.
; Nearest, positive halves up; verified 29/29, no exceptions.
sphere_scanline_profile_28 db 09h, 0Dh, 10h, 12h, 14h, 16h, 18h, 19h, 1Ah, 1Bh, 1Ch, 1Dh
    db 1Eh, 1Fh, 20h, 20h, 21h, 22h, 22h, 22h, 23h, 23h, 23h, 24h
    db 24h, 24h, 24h, 24h, 24h
; Radius 30: floor(1.25*sqrt(30^2-(30-1-i)^2)+0.5), i=0..29.
; Nearest, positive halves up; verified 30/30, no exceptions.
sphere_scanline_profile_29 db 0Ah, 0Dh, 10h, 13h, 15h, 17h, 18h, 19h, 1Bh, 1Ch, 1Dh, 1Eh
    db 1Fh, 20h, 20h, 21h, 22h, 22h, 23h, 23h, 24h, 24h, 24h, 25h
    db 25h, 25h, 25h, 25h, 25h, 26h
; Radius 31: floor(1.25*sqrt(31^2-(31-1-i)^2)+0.5), i=0..30.
; Nearest, positive halves up; verified 31/31, no exceptions.
sphere_scanline_profile_30 db 0Ah, 0Eh, 11h, 13h, 15h, 17h, 19h, 1Ah, 1Bh, 1Dh, 1Eh, 1Fh
    db 20h, 20h, 21h, 22h, 23h, 23h, 24h, 24h, 25h, 25h, 25h, 26h
    db 26h, 26h, 26h, 27h, 27h, 27h, 27h
; Radius 32: floor(1.25*sqrt(32^2-(32-1-i)^2)+0.5), i=0..31.
; Nearest, positive halves up; verified 32/32, no exceptions.
sphere_scanline_profile_31 db 0Ah, 0Eh, 11h, 13h, 15h, 17h, 19h, 1Ah, 1Ch, 1Dh, 1Eh, 1Fh
    db 20h, 21h, 22h, 23h, 23h, 24h, 25h, 25h, 26h, 26h, 26h, 27h
    db 27h, 27h, 28h, 28h, 28h, 28h, 28h, 28h
; Radius 33: floor(1.25*sqrt(33^2-(33-1-i)^2)+0.5), i=0..32.
; Nearest, positive halves up; verified 33/33, no exceptions.
sphere_scanline_profile_32 db 0Ah, 0Eh, 11h, 14h, 16h, 18h, 19h, 1Bh, 1Ch, 1Eh, 1Fh, 20h
    db 21h, 22h, 23h, 23h, 24h, 25h, 25h, 26h, 26h, 27h, 27h, 28h
    db 28h, 28h, 29h, 29h, 29h, 29h, 29h, 29h, 29h
; Radius 34: floor(1.25*sqrt(34^2-(34-1-i)^2)+0.5), i=0..33.
; Nearest, positive halves up; verified 34/34, no exceptions.
sphere_scanline_profile_33 db 0Ah, 0Eh, 11h, 14h, 16h, 18h, 1Ah, 1Bh, 1Dh, 1Eh, 1Fh, 20h
    db 21h, 22h, 23h, 24h, 25h, 26h, 26h, 27h, 27h, 28h, 28h, 29h
    db 29h, 29h, 2Ah, 2Ah, 2Ah, 2Ah, 2Ah, 2Ah, 2Ah, 2Bh
; Radius 35: floor(1.25*sqrt(35^2-(35-1-i)^2)+0.5), i=0..34.
; Nearest, positive halves up; verified 35/35, no exceptions.
sphere_scanline_profile_34 db 0Ah, 0Fh, 12h, 14h, 17h, 18h, 1Ah, 1Ch, 1Dh, 1Fh, 20h, 21h
    db 22h, 23h, 24h, 25h, 26h, 26h, 27h, 28h, 28h, 29h, 29h, 2Ah
    db 2Ah, 2Ah, 2Bh, 2Bh, 2Bh, 2Bh, 2Bh, 2Ch, 2Ch, 2Ch, 2Ch
; Radius 36: floor(1.25*sqrt(36^2-(36-1-i)^2)+0.5), i=0..35.
; Nearest, positive halves up; verified 36/36, no exceptions.
sphere_scanline_profile_35 db 0Bh, 0Fh, 12h, 15h, 17h, 19h, 1Bh, 1Ch, 1Eh, 1Fh, 20h, 22h
    db 23h, 24h, 25h, 25h, 26h, 27h, 28h, 28h, 29h, 29h, 2Ah, 2Ah
    db 2Bh, 2Bh, 2Ch, 2Ch, 2Ch, 2Ch, 2Dh, 2Dh, 2Dh, 2Dh, 2Dh, 2Dh
; Radius 37: floor(1.25*sqrt(37^2-(37-1-i)^2)+0.5), i=0..36.
; Nearest, positive halves up; verified 37/37, no exceptions.
sphere_scanline_profile_36 db 0Bh, 0Fh, 12h, 15h, 17h, 19h, 1Bh, 1Dh, 1Eh, 20h, 21h, 22h
    db 23h, 24h, 25h, 26h, 27h, 28h, 28h, 29h, 2Ah, 2Ah, 2Bh, 2Bh
    db 2Ch, 2Ch, 2Dh, 2Dh, 2Dh, 2Dh, 2Eh, 2Eh, 2Eh, 2Eh, 2Eh, 2Eh
    db 2Eh
; Radius 38: floor(1.25*sqrt(38^2-(38-1-i)^2)+0.5), i=0..37.
; Nearest, positive halves up; verified 38/38, no exceptions.
sphere_scanline_profile_37 db 0Bh, 0Fh, 12h, 15h, 18h, 1Ah, 1Bh, 1Dh, 1Fh, 20h, 21h, 23h
    db 24h, 25h, 26h, 27h, 28h, 28h, 29h, 2Ah, 2Ah, 2Bh, 2Ch, 2Ch
    db 2Dh, 2Dh, 2Dh, 2Eh, 2Eh, 2Eh, 2Fh, 2Fh, 2Fh, 2Fh, 2Fh, 2Fh
    db 2Fh, 30h
; Radius 39: floor(1.25*sqrt(39^2-(39-1-i)^2)+0.5), i=0..38.
; Nearest, positive halves up; verified 39/39, no exceptions.
sphere_scanline_profile_38 db 0Bh, 0Fh, 13h, 16h, 18h, 1Ah, 1Ch, 1Eh, 1Fh, 21h, 22h, 23h
    db 24h, 25h, 26h, 27h, 28h, 29h, 2Ah, 2Bh, 2Bh, 2Ch, 2Ch, 2Dh
    db 2Eh, 2Eh, 2Eh, 2Fh, 2Fh, 2Fh, 30h, 30h, 30h, 30h, 30h, 31h
    db 31h, 31h, 31h
_exitlistfuncs db 44 dup (0)
_addexiterr db 'EXIT LIST OVERFLOW', 0Dh, 0Ah, 0, 0
_aSFileError_1 db '%s FILE ERROR', 0
_foundfile db 100 dup (0)
_foundfileptr dw 0
_dtatransbuffer db 30 dup (0)
_foundfilepath db 98 dup (0)
_video_mode4_active db 2 dup (0)
; Literal translation or hardware policy table; no analytic generator is inferred.
video_mode4_register_values db 35h, 28h, 2Ch, 07h, 79h, 02h, 64h, 6Eh, 02h, 02h, 00h, 00h
_DATA ends
DGROUP group _DATA,DSEG
S012_TEXT segment byte public 'CODE'
public _prerender_max_line_width
public _video_mode4_active
assume cs:S012_TEXT
assume es:nothing, ss:nothing, ds:DGROUP
public _abortmessage
public _intatan
public _getbiostick
public _setbiosticks
public _testbiosticks
public _waitbios
public _slopeline
public _clipline
public _segtoadr
public _adrtoseg
public _criterr_interrupt_handler
public _criterr
public _resetcriterr
public _sprite_two_color_poly_list
public _sprite_two_color_poly_args_alt
public _preRender_line
public _addexit
public _call_exitlist
public _dsiexit
public _filesizea
public _filesizez
public _filesize
public _unpacksizea
public _unpacksizez
public _unpacksize
public _findfirst
public _findnext
public _fixedmult
public _video_set_mode4
; -----------------------------------------------------------------------------
; Purpose: Implement the error or input-control service identified by fatal error.
; ABI: NEAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches AX, SP; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _fatal_error / _fatal_error
; Print the supplied diagnostic, dispatch cleanup callbacks and abort the process.
; Convention: Declared NEAR, operational nonreturning FAR C variadic entry; removes four-byte return address
; before
; printf.
; inputs: Entry SS:SP+4: near DS format pointer followed by C variadic words; DS must address the format and
; its
; string arguments.; Requires a FAR call frame despite PROC NEAR; pops both return words.
; outputs: Does not return; no preserved-register contract after termination.
; preserves: none.
; clobbers: AX and stack immediately; all register/global effects of printf, sprite_copy_2_to_1, flush_stdin,
; exit
; callbacks and abort.
; globals_read: _sprite2 (via descriptor copy); _exitlistfuncs; format and variadic argument memory
; globals_written: _sprite1 (via descriptor copy); stdout/stdin and cleanup-owned state
; services: CRT _printf, _flush_stdin, _abort; registered far cleanup callbacks
; Liveness: live; Live: explicit FAR calls throughout resource, memory and file modules; game/audioeng.c:145
; and
; audio error paths.
; Notes: Second printf reuses the still-present format/arguments after cleanup. Trailing DB 0 follows
; nonreturning
; abort; not an ordinary return path.
_abortmessage proc near
; ASMDOC BLOCK: FAR-frame discard: both return words are removed so printf sees the caller format directly.
    pop     ax
    pop     ax
fatal_copy_display:
    call    _displaywindow
fatal_print_message:
    call    _printf
fatal_flush_stdin:
    call    _flush_stdin
fatal_run_exit_handlers:
    call far ptr _call_exitlist
fatal_print_exit_message:
    call    _printf
fatal_release_arguments:
    add     sp, 2
fatal_abort:
    call    _abort
    db 0 ; retained zero fill after the nonreturning abort call
_abortmessage endp
; -----------------------------------------------------------------------------
; Purpose: Return the polar angle for the supplied vector.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_x (word ptr 8); arg_z
; (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, DI, BP, SP, CS; balanced explicit saves/restores observed: DI; path-
; specific inputs and outputs follow the listed operands.
; Globals read: _atantable.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _polang / _polang
; Compute a table-rounded signed polar angle in 1024 units per turn.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] signed z component; [BP+8] signed x component (BP after PUSH BP/MOV BP,SP).; Incoming AX is
; retained
; for (z,x)=(0,0). DS must address _atantable.
; outputs: AX: quadrant-adjusted angle word; the zero vector returns incoming AX.; Signed magnitude
; comparisons and
; unsigned DIV are literal; -32768 may cause divide overflow.
; preserves: SI; DI; BP; DS; ES; SS
; clobbers: AX; BX; CX; DX; arithmetic flags
; globals_read: _atantable; CS:line_angle_quadrant_dispatch
; globals_written: none.
; services: none.
; Liveness: live; Live: game/frontend.c:1632 and numerous vehicle/camera calls; timer and projection ASM call
; _polang.
; Evidence: lib/gfxres.ASM:259
_intatan proc far
    arg_z = word ptr 6
    arg_x = word ptr 8
    push    bp
    mov     bp, sp
    push    di
    xor     di, di
    mov     dx, [bp+arg_z]
    mov     cx, [bp+arg_x]
    or      dx, dx
    jge     short polar_normalize_z
    or      di, 08h
    neg     dx
polar_normalize_z:
    or      cx, cx
    jge     short polar_normalize_x
    or      di, 04h
    neg     cx
polar_normalize_x:
    cmp     dx, cx
    jl      short polar_swap_axes
    jz      short polar_dispatch_quadrant
    xchg    dx, cx
polar_select_ratio:
    or      di, 02h
polar_swap_axes:
    xor     ax, ax
    div     cx
polar_prepare_atan_division:
    mov     bl, ah
    xor     bh, bh
    add     al, 80h
    adc     bx, 0
polar_scale_atan_index:
    mov     al, _atantable[bx]
    xor     ah, ah
polar_read_atan_table:
    jmp     cs:line_angle_quadrant_dispatch[di]
polar_dispatch_quadrant:
    or      dx, dx
    jz      short polar_quadrant_1
    mov     ax, 80h
    jmp     cs:line_angle_quadrant_dispatch[di]
; ASMDOC BLOCK: Inline CS jump table, indexed by even DI quadrant/magnitude flags; never sequentially
; executed.
line_angle_quadrant_dispatch     dw offset polar_quadrant_1
    dw offset polar_handle_zero_x
    dw offset polar_quadrant_4
    dw offset polar_return_angle
    dw offset polar_quadrant_2
    dw offset polar_quadrant_3
    dw offset polar_quadrant_6
    dw offset polar_quadrant_7
polar_handle_zero_x:
    neg     ax
    add     ah, 1
polar_quadrant_1:
    pop     di
    pop     bp
    retf
polar_return_angle:
    add     ah, 1
    jmp     short polar_quadrant_1
polar_quadrant_4:
    neg     ax
    add     ah, 2
    jmp     short polar_quadrant_1
polar_quadrant_2:
    neg     ax
    jmp     short polar_quadrant_1
polar_quadrant_3:
    sub     ah, 1
    jmp     short polar_quadrant_1
polar_quadrant_7:
    add     ah, 1
    neg     ax
    jmp     short polar_quadrant_1
polar_quadrant_6:
    sub     ah, 2
    jmp     short polar_quadrant_1
_intatan endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper sub 2EAD4. Its internal flow uses timer_snapshot_complete,
; timer_snapshot_return.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX; balanced explicit saves/restores observed: none inferred; path-specific inputs
; and outputs follow the listed operands.
; Globals read: _timer_elapsed_ticks_high, _timer_elapsed_ticks_low.
; Globals written: none identified.
; /* PLATFORM(timer): masks or restores interrupts around timer state. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _sub_2EAD4 / _timer_read_elapsed_ticks
; Read a coherent low/high elapsed-tick snapshot while interrupts are masked.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: DS:DGROUP; no stack arguments.
; outputs: DX:AX elapsed ticks; IF=1 on return (STI, not restored incoming IF).
; preserves: BX; CX; SI; DI; BP; DS; ES; SS; arithmetic flags
; clobbers: AX; DX; IF forced enabled
; globals_read: _timer_elapsed_ticks_low; _timer_elapsed_ticks_high
; globals_written: none.
; services: CLI/STI
; Liveness: live; Live: same-module deadline setters, testers and wait loops.
; Evidence: lib/gfxres.ASM:348
_getbiostick proc far
    cli ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
    mov     ax, _timer_elapsed_ticks_low
    mov     dx, _timer_elapsed_ticks_high
timer_snapshot_complete:
    sti ;/* PLATFORM(timer): masks or restores interrupts around timer state. */
timer_snapshot_return:
    retf
_getbiostick endp
; -----------------------------------------------------------------------------
; Purpose: Add a delay to the current timer count and store the resulting deadline.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: input_deadline_tick_high, input_deadline_tick_low.
; Globals written: input_deadline_tick_high, input_deadline_tick_low.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _set_add_value / _set_add_value
; Store elapsed ticks plus a supplied 32-bit delay as the input deadline.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] unsigned delay low word; [BP+8] high word; modulo-2^32 addition. DS:DGROUP.
; outputs: DX:AX computed deadline; writes both deadline words; IF=1.
; preserves: BX; CX; SI; DI; BP; DS; ES; SS
; clobbers: AX; DX; arithmetic flags; IF
; globals_read: _timer_elapsed_ticks_low; _timer_elapsed_ticks_high
; globals_written: input_deadline_tick_low; input_deadline_tick_high
; services: CLI/STI through timer snapshot
; Liveness: live; Live: lib/lineedit.c line editor calls it.
; Notes: An unexported wait loop at line 384 is after RETF and has no inbound source reference; it is not part
; of the
; callable setter path.
_setbiosticks proc far
    arg_delay_low = word ptr 6
    arg_delay_high = word ptr 8
    push    bp
deadline_frame_setup:
    mov     bp, sp
deadline_read_timer:
    call    _getbiostick
deadline_add_low_word:
    add     ax, [bp+arg_delay_low]
deadline_add_high_word:
    adc     dx, [bp+arg_delay_high]
deadline_store_low_word:
    mov     input_deadline_tick_low, ax
deadline_store_high_word:
    mov     input_deadline_tick_high, dx
    pop     bp
    retf
; ASMDOC BLOCK: Unexported dead interior entry after RETF; its two comparisons mirror the wordwise deadline
; test.
deadline_wait_until_reached:
    call    _getbiostick
    cmp     dx, input_deadline_tick_high
    jb      short deadline_wait_until_reached
    cmp     ax, input_deadline_tick_low
    jb      short deadline_wait_until_reached
    retf
_setbiosticks endp
; -----------------------------------------------------------------------------
; Purpose: Poll keyboard and joystick input for the abort command.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, CX, DX; balanced explicit saves/restores observed: none inferred; path-specific
; inputs and outputs follow the listed operands.
; Globals read: input_deadline_tick_high, input_deadline_tick_low.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _poll_input_abort / _poll_input_abort
; Test whether both elapsed counter words have reached their saved deadline words.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: DS:DGROUP; no stack arguments.
; outputs: AX=CX=1 if high>=deadline_high AND low>=deadline_low, else 0; DX=elapsed high; IF=1.
; preserves: BX; SI; DI; BP; DS; ES; SS
; clobbers: AX; CX; DX; arithmetic flags; IF
; globals_read: _timer_elapsed_ticks_low; _timer_elapsed_ticks_high; input_deadline_tick_low;
; input_deadline_tick_high
; globals_written: none.
; services: CLI/STI through snapshot
; Liveness: live; Live: lib/lineedit.c:94; no keyboard/joystick access.
; Notes: Independent unsigned word comparisons, not a lexicographic 32-bit comparison. Public name is retained
; because
; the provided name plan does not rename it.
_testbiosticks proc far
    call    _getbiostick
    xor     cx, cx
    cmp     dx, input_deadline_tick_high
    jb      short deadline_reached_result
    cmp     ax, input_deadline_tick_low
    jb      short deadline_reached_result
    inc     cx
deadline_reached_result:
    mov     ax, cx
    retf
_testbiosticks endp
; -----------------------------------------------------------------------------
; Purpose: Wait for the input deadline while polling for an abort.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _wait_for_input_delay / _wait_for_input_delay
; Busy-wait until both elapsed words reach a local elapsed-plus-delay deadline.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] delay low word; [BP+8] delay high word; DS:DGROUP.
; outputs: DX:AX last elapsed snapshot; IF=1; no logical return value.
; preserves: BX; CX; SI; DI; BP; DS; ES; SS
; clobbers: AX; DX; arithmetic flags; IF
; globals_read: _timer_elapsed_ticks_low; _timer_elapsed_ticks_high
; globals_written: none.
; services: CLI/STI per snapshot
; Liveness: live; Live: game/uires.c:373 calls with 8L.
; Notes: Both unsigned comparisons must pass even if high has already exceeded the target; no input polling or
; abort
; service occurs.
_waitbios proc far
    local_deadline_low = word ptr -4
    local_deadline_high = word ptr -2
    arg_delay_low = word ptr 6
    arg_delay_high = word ptr 8
    push    bp
    mov     bp, sp
    sub     sp, 4
    call    _getbiostick
    add     ax, [bp+arg_delay_low]
    adc     dx, [bp+arg_delay_high]
    mov     [bp+local_deadline_low], ax
    mov     [bp+local_deadline_high], dx
delay_deadline_poll_loop:
    call    _getbiostick
    cmp     dx, [bp+local_deadline_high]
    jb      short delay_deadline_poll_loop
    cmp     ax, [bp+local_deadline_low]
    jb      short delay_deadline_poll_loop
    mov     sp, bp
    pop     bp
    retf
_waitbios endp
; -----------------------------------------------------------------------------
; Purpose: Build line-rendering descriptors using the alternate path.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _draw_line_related_alt / _draw_line_related_alt
; Build a nominally unclipped fixed-point line descriptor without drawing pixels.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] signed start x; [BP+8] start y; [BP+10] end x; [BP+12] end y.; [BP+14] near DS pointer to a
; writable
; 28-byte line descriptor; DS must alias SS for stack-resident callers. +10h color is retained, not
; initialized.
; outputs: AX=0 for a usable descriptor; AX=clip code 1/2/4/8 for rejected/outside geometry, with step count
; 0.;
; Descriptor: +0 x fraction, +2 start x, +4 y fraction, +6 start y, +8 end x, +Ah end y, +Ch slope, +Eh
; inclusive step
; count, +10h retained color, +12h direction byte, +13h clip flags, +14h/+16h left clipped rows, +18h/+1Ah
; right
; clipped rows.; Direction values: 0/1 horizontal, 2 vertical, 3/4 diagonal, 5/6 y-major, 7/8 x-major, 9
; point; all
; arithmetic uses 16-bit wraps and explicit overflow fallback.
; preserves: SI; DI; BP; DS; ES; SS
; clobbers: AX; BX; CX; DX; arithmetic flags
; globals_read: CS:line_slope_table_limit; CS:line_interp_profile_offsets and profiles;
; CS:line_direction_case_dispatch and four direction tables; CS:_sprite1 clip bounds only on large-coordinate
; overflow
; fallback
; globals_written: 28-byte caller descriptor except retained color word
; services: none.
; Liveness: live; Live: prerender_wheel_raster calls both variants; the normal variant is also called by
; _preRender_line and C skybox code.
; Notes: Alternate entry creates identical frame and jumps into normal routine at line_init_output_record. It
; bypasses
; initial clipping, but overflow fallback re-enters code that reads clip bounds. Tables are inline CS data; no
; direct
; hardware services.
_slopeline proc far
    var_4 = byte ptr -4
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     [bp+var_4], 1
    jmp     short line_init_output_record
_slopeline endp
; -----------------------------------------------------------------------------
; Purpose: Build line-rendering descriptors for the requested endpoints.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_8 (word ptr 14);
; arg_endX (word ptr 10); arg_endY (word ptr 12); arg_startX (word ptr
;   6); arg_startY (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, CS; balanced explicit saves/restores observed: DI, SI;
; path-specific inputs and outputs follow the listed operands.
; Globals read: _sprite1.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _draw_line_related / _draw_line_related
; Build a clipped fixed-point line descriptor without drawing pixels.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] signed start x; [BP+8] start y; [BP+10] end x; [BP+12] end y.; [BP+14] near DS pointer to a
; writable
; 28-byte line descriptor; DS must alias SS for stack-resident callers. +10h color is retained, not
; initialized.
; outputs: AX=0 for a usable descriptor; AX=clip code 1/2/4/8 for rejected/outside geometry, with step count
; 0.;
; Descriptor: +0 x fraction, +2 start x, +4 y fraction, +6 start y, +8 end x, +Ah end y, +Ch slope, +Eh
; inclusive step
; count, +10h retained color, +12h direction byte, +13h clip flags, +14h/+16h left clipped rows, +18h/+1Ah
; right
; clipped rows.; Direction values: 0/1 horizontal, 2 vertical, 3/4 diagonal, 5/6 y-major, 7/8 x-major, 9
; point; all
; arithmetic uses 16-bit wraps and explicit overflow fallback.
; preserves: SI; DI; BP; DS; ES; SS
; clobbers: AX; BX; CX; DX; arithmetic flags
; globals_read: CS:line_slope_table_limit; CS:line_interp_profile_offsets and profiles;
; CS:line_direction_case_dispatch and four direction tables; CS:_sprite1 clip top/bottom at +16/+18 and
; left/right at
; +26/+28
; globals_written: 28-byte caller descriptor except retained color word
; services: none.
; Liveness: live; Live: prerender_wheel_raster calls both variants; the normal variant is also called by
; _preRender_line and C skybox code.
; Notes: Alternate entry creates identical frame and jumps into normal routine at line_init_output_record. It
; bypasses
; initial clipping, but overflow fallback re-enters code that reads clip bounds. Tables are inline CS data; no
; direct
; hardware services.
_clipline proc far
    var_4 = byte ptr -4
    var_2 = word ptr -2
    arg_startX = word ptr 6
    arg_startY = word ptr 8
    arg_endX = word ptr 10
    arg_endY = word ptr 12
    arg_line_descriptor = word ptr 14
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     [bp+var_4], 0
; ASMDOC BLOCK: Both public line prologues join here; DS:SI points at the caller 28-byte record; color at +10h
; is retained.
line_init_output_record:
    mov     si, [bp+arg_line_descriptor]
    mov     word ptr [si+12h], 0FFh
    xor     ax, ax
    mov     [si], ax
    mov     [si+4], ax
    mov     [si+14h], ax
    mov     [si+16h], ax
    mov     [si+18h], ax
    mov     [si+1Ah], ax
    mov     ax, [bp+arg_startY]
    mov     bx, [bp+arg_endY]
    mov     cx, [bp+arg_startX]
    mov     dx, [bp+arg_endX]
    cmp     ax, bx
    jg      short line_store_swapped_start
    mov     [si+2], cx
    mov     [si+6], ax
    mov     [si+8], dx
line_store_ordered_end_y:
    mov     [si+0Ah], bx
    jmp     short line_check_horizontal_line
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_store_swapped_start:
    mov     [si+2], dx
    mov     [si+6], bx
    mov     [si+8], cx
    mov     [si+0Ah], ax
line_check_horizontal_line:
    jnz     short line_clip_entry
    jmp     line_horizontal_special_case
line_clip_entry:
    xor     dx, dx
    cmp     [bp+var_4], 0
    jnz     short line_merge_clip_codes
    mov     ax, [si+6]
    mov     bx, cs:[_sprite1+16]
    mov     cx, cs:[_sprite1+18]
    cmp     ax, cx
    jl      short line_start_below_clip_bottom
    mov     dl, 8
    jmp     line_commit_clip_bounds
line_start_below_clip_bottom:
    cmp     ax, bx
    jge     short line_start_above_clip_top
    or      dh, 04h
line_start_above_clip_top:
    mov     ax, [si+0Ah]
    cmp     ax, bx
    jge     short line_end_below_clip_bottom
    mov     dl, 4
    jmp     line_commit_clip_bounds
line_end_below_clip_bottom:
    cmp     ax, cx
    jl      short line_end_above_clip_top
    or      dl, 08h
line_end_above_clip_top:
    mov     bx, cs:[_sprite1+26]
    mov     cx, cs:[_sprite1+28]
    mov     ax, [si+2]
    cmp     ax, bx
    jge     short line_start_left_of_clip
    or      dh, 02h
line_start_left_of_clip:
    cmp     ax, cx
    jl      short line_start_right_of_clip
    or      dh, 01h
line_start_right_of_clip:
    mov     ax, [si+8]
    cmp     ax, bx
    jge     short line_end_left_of_clip
    or      dl, 02h
line_end_left_of_clip:
    cmp     ax, cx
    jl      short line_end_right_of_clip
    or      dl, 01h
line_end_right_of_clip:
    test    dl, dh
    jz      short line_merge_clip_codes
    and     dl, dh
    jmp     line_commit_clip_bounds
line_merge_clip_codes:
    or      dl, dh
    xor     dh, dh
    mov     [bp+var_2], dx
    mov     cx, [si+0Ah]
    sub     cx, [si+6]
    jno     short line_measure_dx
line_overflow_fallback:
    jmp     line_large_coordinate_fallback
line_measure_dx:
    mov     dx, [si+8]
    sub     dx, [si+2]
    jo      short line_overflow_fallback
    jnz     short line_classify_x_slope
    inc     cx
    mov     [si+0Eh], cx
    mov     byte ptr [si+12h], 2
    jmp     short line_dispatch_clip_flags
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_classify_x_slope:
    jl      short line_negative_dy
    cmp     dx, cx
    jb      short line_y_slope_steep_down
    jz      short line_y_slope_equal
    mov     byte ptr [si+12h], 8
line_make_x_major:
    xchg    cx, dx
; ASMDOC BLOCK: Small deltas use inline CS interpolation profiles; larger deltas divide and round the 16.16
; slope.
line_slope_lookup_or_divide:
    cmp     cx, cs:line_slope_table_limit
    jge     short line_slope_divide_fallback
    mov     bx, cx
    shl     bx, 1
    mov     bx, cs:line_interp_profile_offsets[bx]
    add     bx, dx
    add     bx, dx
    mov     ax, cs:[bx]
    jmp     short line_store_slope_and_length
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_slope_divide_fallback:
    xor     ax, ax
    div     cx
    mov     bx, cx
    shr     bx, 1
    sub     bx, dx
    adc     ax, 0
line_store_slope_and_length:
    mov     [si+0Ch], ax
    inc     cx
    jo      short line_overflow_fallback
    mov     [si+0Eh], cx
    jmp     short line_dispatch_clip_flags
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_y_slope_steep_down:
    mov     byte ptr [si+12h], 6
    jmp     short line_slope_lookup_or_divide
line_y_slope_equal:
    mov     byte ptr [si+12h], 4
    jmp     short line_set_primary_length
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_negative_dy:
    neg     dx
    jo      short line_overflow_fallback
    cmp     dx, cx
    jb      short line_x_slope_shallow_negative
    jz      short line_x_slope_equal_negative
    mov     byte ptr [si+12h], 7
    jmp     short line_make_x_major
line_x_slope_shallow_negative:
    mov     byte ptr [si+12h], 5
    jmp     short line_slope_lookup_or_divide
line_x_slope_equal_negative:
    mov     byte ptr [si+12h], 3
line_set_primary_length:
    inc     cx
    mov     [si+0Eh], cx
line_dispatch_clip_flags:
    mov     bx, [bp+var_2]
    shl     bx, 1
    jz      short line_reject_clipped
    jmp     cs:line_direction_case_dispatch[bx]
; ASMDOC BLOCK: CS data embedded in line body: branch targets, not instructions. Several entries intentionally
; alias.
line_direction_case_dispatch     dw offset line_reject_clipped
    dw offset line_dispatch_right_clip
    dw offset line_dispatch_x_clip_flags
    dw offset line_dispatch_x_clip_flags
    dw offset line_clip_top_edge
    dw offset line_clip_top_edge
    dw offset line_clip_top_edge
    dw offset line_clip_top_edge
    dw offset line_clip_bottom_edge
    dw offset line_clip_bottom_edge
    dw offset line_clip_bottom_edge
    dw offset line_clip_bottom_edge
    dw offset line_clip_top_edge
    dw offset line_clip_top_edge
    dw offset line_clip_top_edge
    dw offset line_clip_top_edge
line_reject_clipped:
    xor     ax, ax
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_clip_top_edge:
    mov     ax, [si+6]
    mov     cx, cs:[_sprite1+16]
    mov     [si+6], cx
    sub     cx, ax
    mov     bl, [si+12h]
    shl     bx, 1
    jmp     cs:line_x_step_dispatch[bx]
line_x_step_dispatch     dw 0
    dw 0
    dw offset line_top_case_00_adjust_y
    dw offset line_top_case_01_adjust_xy
    dw offset line_top_case_02_adjust_xy
    dw offset line_top_case_03_shift_xneg
    dw offset line_top_case_04_shift_xpos
    dw offset line_top_case_05_step_back
    dw offset line_top_case_06_step_forward
line_top_case_00_adjust_y:
    sub     [si+0Eh], cx
    jmp     line_continue_bottom_clip
line_top_case_01_adjust_xy:
    sub     [si+2], cx
    sub     [si+0Eh], cx
    jmp     line_continue_bottom_clip
line_top_case_02_adjust_xy:
    add     [si+2], cx
    sub     [si+0Eh], cx
    jmp     line_continue_bottom_clip
line_top_case_03_shift_xneg:
    mov     ax, [si+0Ch]
    mul     cx
    sub     [si], ax
    sbb     [si+2], dx
    sub     [si+0Eh], cx
    jmp     line_continue_bottom_clip
line_top_case_04_shift_xpos:
    mov     ax, [si+0Ch]
    mul     cx
    add     [si], ax
    adc     [si+2], dx
    sub     [si+0Eh], cx
    jmp     line_continue_bottom_clip
line_top_case_05_step_back:
    mov     [si+6], ax
    mov     dx, cx
    xor     ax, ax
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    sub     [si+2], ax
    sub     [si+0Eh], ax
    jle     short line_top_short_span_fallback
    mul     word ptr [si+0Ch]
    add     [si+4], ax
    adc     [si+6], dx
    jmp     line_continue_bottom_clip
line_top_short_span_fallback:
    mov     word ptr [si+0Eh], 1
    mov     ax, cs:[_sprite1+16]
    mov     [si+6], ax
    mov     ax, [si+8]
    mov     [si+2], ax
    jmp     line_continue_bottom_clip
line_top_case_06_step_forward:
    mov     [si+6], ax
    mov     dx, cx
    xor     ax, ax
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    add     [si+2], ax
    sub     [si+0Eh], ax
    jle     short line_top_short_span_fallback
    mul     word ptr [si+0Ch]
    add     [si+4], ax
    adc     [si+6], dx
    jmp     line_continue_bottom_clip
line_clip_bottom_edge:
    mov     cx, [si+0Ah]
    mov     dx, cs:[_sprite1+18]
    dec     dx
    mov     [si+0Ah], dx
    sub     cx, dx
    mov     bl, [si+12h]
    shl     bx, 1
    jmp     cs:line_y_step_dispatch[bx]
line_y_step_dispatch     dw 0
    dw 0
    dw offset line_bottom_case_00_adjust_y
    dw offset line_bottom_case_01_shift_xpos
    dw offset line_bottom_case_02_shift_xneg
    dw offset line_bottom_case_03_ymin
    dw offset line_bottom_case_04_ymax
    dw offset line_bottom_case_05_slope_neg
    dw offset line_bottom_case_06_slope_pos
line_bottom_case_00_adjust_y:
    sub     [si+0Eh], cx
    jmp     line_recompute_y_outcodes
line_bottom_case_01_shift_xpos:
    add     [si+8], cx
    sub     [si+0Eh], cx
    jmp     line_recompute_y_outcodes
line_bottom_case_02_shift_xneg:
    sub     [si+8], cx
    sub     [si+0Eh], cx
    jmp     line_recompute_y_outcodes
line_bottom_case_03_ymin:
    mov     dx, [si+0Eh]
    sub     dx, cx
    mov     [si+0Eh], dx
    dec     dx
    mov     ax, [si+0Ch]
    mul     dx
    mov     bx, [si]
    mov     cx, [si+2]
    sub     bx, ax
    sbb     cx, dx
    add     bx, 8000h
    adc     cx, 0
    mov     [si+8], cx
    jmp     line_recompute_y_outcodes
line_bottom_case_04_ymax:
    mov     dx, [si+0Eh]
    sub     dx, cx
    mov     [si+0Eh], dx
    dec     dx
    mov     ax, [si+0Ch]
    mul     dx
    add     ax, [si]
    adc     dx, [si+2]
    add     ax, 8000h
    adc     dx, 0
    mov     [si+8], dx
    jmp     line_recompute_y_outcodes
line_bottom_case_05_slope_neg:
    xor     ax, ax
    sub     ax, [si+4]
    sbb     dx, [si+6]
    jl      short line_bottom_zero_x_step
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
line_bottom_store_adjusted_y:
    mov     dx, [si+2]
    sub     dx, ax
    mov     [si+8], dx
    inc     ax
    mov     [si+0Eh], ax
    jmp     line_recompute_y_outcodes
line_bottom_zero_x_step:
    xor     ax, ax
    jmp     short line_bottom_store_adjusted_y
line_bottom_case_06_slope_pos:
    xor     ax, ax
    sub     ax, [si+4]
    sbb     dx, [si+6]
    jl      short line_bottom_zero_x_step
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    mov     dx, [si+2]
    add     dx, ax
    mov     [si+8], dx
    inc     ax
    mov     [si+0Eh], ax
    jmp     line_recompute_y_outcodes
line_dispatch_x_clip_flags:
    mov     bl, [si+12h]
    shl     bx, 1
    jmp     cs:line_endpoint_adjust_dispatch[bx]
line_endpoint_adjust_dispatch     dw 0
    dw 0
    dw offset line_clip_initialize_default
    dw offset line_left_case_01_adjust_ymax
    dw offset line_left_case_02_adjust_ymin
    dw offset line_left_case_03_slope_to_edge
    dw offset line_left_case_04_adjust_x
    dw offset line_left_case_05_adjust_x
    dw offset line_left_case_06_adjust_x
line_left_case_01_adjust_ymax:
    mov     cx, cs:[_sprite1+26]
    mov     ax, [si+8]
    mov     [si+8], cx
    sub     cx, ax
    add     [si+16h], cx
    sub     [si+0Eh], cx
    sub     [si+0Ah], cx
    jmp     line_continue_right_clip
line_left_case_02_adjust_ymin:
    mov     ax, [si+2]
    mov     cx, cs:[_sprite1+26]
    mov     [si+2], cx
    sub     cx, ax
    add     [si+14h], cx
    add     [si+6], cx
    sub     [si+0Eh], cx
    jmp     line_continue_right_clip
line_left_case_03_slope_to_edge:
    mov     ax, [si]
    mov     dx, [si+2]
    mov     cx, cs:[_sprite1+26]
    mov     [si+8], cx
    sub     dx, cx
    jl      short line_left_minimum_height
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    inc     ax
line_left_store_adjusted_height:
    mov     cx, [si+0Eh]
    mov     [si+0Eh], ax
    sub     cx, ax
    add     [si+16h], cx
    dec     ax
    add     ax, [si+6]
    mov     [si+0Ah], ax
    jmp     line_continue_right_clip
line_left_minimum_height:
    mov     ax, 1
    jmp     short line_left_store_adjusted_height
line_left_case_04_adjust_x:
    mov     dx, cs:[_sprite1+26]
    xor     ax, ax
    sub     ax, [si]
    sbb     dx, [si+2]
    jl      short line_left_outside_fallback
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    add     [si+6], ax
    add     [si+14h], ax
    sub     [si+0Eh], ax
    jle     short line_clip_initialize_default
    mul     word ptr [si+0Ch]
    add     [si], ax
    adc     [si+2], dx
    jmp     line_continue_right_clip
line_left_outside_fallback:
    jmp     short line_clip_initialize_default
    nop
line_left_case_05_adjust_x:
    mov     ax, [si+2]
    mov     dx, cs:[_sprite1+26]
    mov     [si+8], dx
    sub     ax, dx
    mov     cx, ax
    inc     cx
    mov     [si+0Eh], cx
    mul     word ptr [si+0Ch]
    add     ax, [si+4]
    adc     dx, [si+6]
    add     ax, 8000h
    adc     dx, 0
    mov     ax, [si+0Ah]
    sub     ax, dx
    mov     [si+0Ah], dx
    add     [si+16h], ax
    jmp     line_continue_right_clip
line_left_case_06_adjust_x:
    mov     cx, [si+2]
    mov     ax, cs:[_sprite1+26]
    mov     [si+2], ax
    sub     ax, cx
    sub     [si+0Eh], ax
    mul     word ptr [si+0Ch]
    mov     bx, [si+4]
    mov     cx, [si+6]
    add     ax, bx
    adc     dx, cx
    mov     [si+4], ax
    mov     [si+6], dx
    add     bx, 8000h
    adc     cx, 0
    add     ax, 8000h
    adc     dx, 0
    sub     dx, cx
    add     [si+14h], dx
    jmp     line_continue_right_clip
; ASMDOC BLOCK: Outside/degenerate paths zero length but still record edge codes and clipped-row accounting.
line_clip_initialize_default:
    mov     dx, 2
line_commit_clip_bounds:
    mov     [si+13h], dl
    mov     word ptr [si+0Eh], 0
    mov     al, [si+13h]
    test    al, 04h
    jz      short line_commit_top_edge
    mov     bx, cs:[_sprite1+16]
    mov     [si+6], bx
    mov     word ptr [si+4], 0
    dec     bx
    mov     [si+0Ah], bx
    jmp     short line_finish_return
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_commit_top_edge:
    test    al, 08h
    jz      short line_commit_bottom_edge
    mov     bx, cs:[_sprite1+18]
    mov     [si+6], bx
    mov     word ptr [si+4], 0
    jmp     short line_finish_return
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_commit_bottom_edge:
    mov     cx, [si+0Ah]
    cmp     cx, cs:[_sprite1+18]
    jl      short line_commit_left_edge
    mov     cx, cs:[_sprite1+18]
    dec     cx
line_commit_left_edge:
    mov     dx, [si+6]
    mov     bx, [si+4]
    add     bx, 8000h
    adc     dx, 0
    cmp     dx, cs:[_sprite1+16]
    jge     short line_commit_right_edge
    mov     dx, cs:[_sprite1+16]
line_commit_right_edge:
    mov     [si+6], dx
    mov     word ptr [si+4], 0
    sub     cx, dx
    dec     dx
    inc     cx
    mov     [si+0Ah], dx
    test    al, 02h
    jz      short line_store_remainder_x
    add     [si+16h], cx
    jmp     short line_finish_return
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_store_remainder_x:
    add     [si+1Ah], cx
line_finish_return:
    xor     ah, ah
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_dispatch_right_clip:
    mov     bl, [si+12h]
line_right_case_table_index:
    xor     bh, bh
    shl     bx, 1
    jmp     cs:line_clip_case_dispatch[bx]
line_clip_case_dispatch     dw 0
    dw 0
    dw offset line_vertical_edge_degenerate
    dw offset line_right_case_01
    dw offset line_right_case_02
    dw offset line_right_case_03
    dw offset line_right_case_04
    dw offset line_right_case_05
    dw offset line_right_case_06
line_vertical_edge_degenerate:
    mov     dx, 1
    jmp     line_commit_clip_bounds
line_right_case_01:
    mov     cx, [si+2]
    mov     ax, cs:[_sprite1+28]
    dec     ax
    mov     [si+2], ax
    sub     cx, ax
    add     [si+6], cx
    sub     [si+0Eh], cx
    add     [si+18h], cx
    jmp     line_reject_clipped
line_right_case_02:
    mov     cx, cs:[_sprite1+28]
    dec     cx
    mov     ax, [si+8]
    mov     [si+8], cx
    sub     ax, cx
    add     [si+1Ah], ax
    sub     [si+0Eh], ax
    sub     [si+0Ah], ax
    jmp     line_reject_clipped
line_right_case_03:
    mov     ax, [si]
    mov     dx, [si+2]
    sub     dx, cs:[_sprite1+28]
    inc     dx
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    sub     [si+0Eh], ax
    jle     short line_vertical_edge_degenerate
    add     [si+6], ax
    add     [si+18h], ax
    mul     word ptr [si+0Ch]
    sub     [si], ax
    sbb     [si+2], dx
    jmp     line_reject_clipped
line_right_case_04:
    mov     dx, cs:[_sprite1+28]
    dec     dx
    mov     [si+8], dx
    xor     ax, ax
    sub     ax, [si]
    sbb     dx, [si+2]
    jl      short line_vertical_edge_degenerate
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    inc     ax
    mov     cx, [si+0Eh]
    mov     [si+0Eh], ax
    sub     cx, ax
    add     [si+1Ah], cx
    dec     ax
    add     ax, [si+6]
    mov     [si+0Ah], ax
    jmp     line_reject_clipped
line_right_case_05:
    mov     ax, [si+2]
    mov     cx, cs:[_sprite1+28]
    dec     cx
    sub     ax, cx
    mov     [si+2], cx
    sub     [si+0Eh], ax
    mul     word ptr [si+0Ch]
    mov     bx, [si+4]
    mov     cx, [si+6]
    add     ax, bx
    adc     dx, cx
    mov     [si+4], ax
    mov     [si+6], dx
    add     bx, 8000h
    adc     cx, 0
    add     ax, 8000h
    adc     dx, 0
    sub     dx, cx
    add     [si+18h], dx
    jmp     line_reject_clipped
line_right_case_06:
    mov     ax, cs:[_sprite1+28]
    mov     cx, ax
    dec     cx
    mov     [si+8], cx
    sub     ax, [si+2]
    mov     [si+0Eh], ax
    dec     ax
    mul     word ptr [si+0Ch]
    add     ax, [si+4]
    adc     dx, [si+6]
    add     ax, 8000h
    adc     dx, 0
    mov     ax, [si+0Ah]
    sub     ax, dx
    mov     [si+0Ah], dx
    add     [si+1Ah], ax
    jmp     line_reject_clipped
line_continue_bottom_clip:
    test    [bp+var_2], 08h
    jz      short line_recompute_y_outcodes
    jmp     line_clip_bottom_edge
line_recompute_y_outcodes:
    xor     dx, dx
    mov     ax, [si+2]
    cmp     ax, cs:[_sprite1+26]
    jge     short line_mark_start_below
    or      dh, 02h
line_mark_start_below:
    mov     bx, [si]
    add     bx, 8000h
    adc     ax, 0
    cmp     ax, cs:[_sprite1+28]
    jl      short line_mark_start_above
    or      dh, 01h
line_mark_start_above:
    mov     ax, [si+8]
    cmp     ax, cs:[_sprite1+26]
    jge     short line_mark_end_below
    or      dl, 02h
line_mark_end_below:
    cmp     ax, cs:[_sprite1+28]
    jl      short line_mark_end_above
    or      dl, 01h
line_mark_end_above:
    test    dl, dh
    jz      short line_retest_vertical_codes
    and     dl, dh
    jmp     line_commit_clip_bounds
line_retest_vertical_codes:
    or      dl, dh
    jz      short line_skip_final_reclip
    xor     dh, dh
    mov     [bp+var_2], dx
    jmp     line_dispatch_clip_flags
line_continue_right_clip:
    test    [bp+var_2], 01h
    jz      short line_skip_final_reclip
    jmp     line_dispatch_right_clip
line_skip_final_reclip:
    jmp     line_reject_clipped
line_horizontal_special_case:
    mov     byte ptr [si+12h], 1
    cmp     cx, dx
    jnz     short line_order_horizontal_endpoints
    mov     byte ptr [si+12h], 9
line_order_horizontal_endpoints:
    jle     short line_horizontal_check_y_bounds
    mov     byte ptr [si+12h], 0
    mov     [si+2], dx
    mov     [si+8], cx
    xchg    cx, dx
line_horizontal_check_y_bounds:
    cmp     [bp+var_4], 0
    jnz     short line_horizontal_set_length
    mov     bx, cs:[_sprite1+16]
    cmp     ax, bx
    jge     short line_horizontal_check_bottom
    mov     al, 4
    mov     [si+6], bx
    mov     [si+0Ah], bx
line_horizontal_return:
    mov     [si+13h], al
    mov     word ptr [si+0Eh], 0
    xor     ah, ah
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
line_horizontal_set_length:
    sub     dx, cx
    inc     dx
    mov     [si+0Eh], dx
    jmp     line_reject_clipped
line_horizontal_check_bottom:
    mov     bx, cs:[_sprite1+18]
    cmp     ax, bx
    jl      short line_horizontal_trim_left
    mov     al, 8
    mov     [si+6], bx
    mov     [si+0Ah], bx
    jmp     short line_horizontal_return
line_horizontal_trim_left:
    mov     ax, dx
    sub     ax, cx
    inc     ax
    mov     [si+0Eh], ax
    cmp     dx, cs:[_sprite1+26]
    jge     short line_horizontal_trim_right
    dec     word ptr [si+0Ah]
    mov     word ptr [si+16h], 1
    mov     al, 2
    jmp     short line_horizontal_return
line_horizontal_trim_right:
    cmp     cx, cs:[_sprite1+28]
    jl      short line_horizontal_clip_left
    dec     word ptr [si+0Ah]
    mov     word ptr [si+1Ah], 1
    mov     al, 1
    jmp     short line_horizontal_return
line_horizontal_clip_left:
    mov     ax, cs:[_sprite1+26]
    mov     bx, ax
    sub     ax, cx
    jle     short line_horizontal_clip_right
    mov     [si+2], bx
    sub     [si+0Eh], ax
line_horizontal_clip_right:
    mov     ax, dx
    mov     bx, cs:[_sprite1+28]
    dec     bx
    sub     ax, bx
    jle     short line_horizontal_finish_clipping
    sub     [si+0Eh], ax
    mov     [si+8], bx
line_horizontal_finish_clipping:
    jmp     line_reject_clipped
; ASMDOC BLOCK: Signed subtraction overflow falls back to repeated coarse endpoint reduction before normal
; clipping.
line_large_coordinate_fallback:
    mov     cx, [si+0Ah]
    sar     cx, 1
    mov     ax, [si+6]
    sar     ax, 1
    sub     cx, ax
    sar     cx, 1
    mov     dx, [si+8]
    sar     dx, 1
    mov     ax, [si+2]
    sar     ax, 1
    sub     dx, ax
    sar     dx, 1
line_fallback_clip_y_loop:
    cmp     word ptr [si+6], 0C180h
    jg      short line_fallback_check_start_y
line_fallback_adjust_start_y:
    mov     ax, [si+6]
    add     ax, cx
    mov     [si+6], ax
    mov     bx, ax
    sub     ax, cs:[_sprite1+16]
    jle     short line_fallback_advance_x
    cmp     ax, cx
    jle     short line_fallback_check_bottom_y
    mov     ax, cx
line_fallback_check_bottom_y:
    sub     bx, cs:[_sprite1+18]
    jle     short line_fallback_adjust_start_x
    sub     ax, bx
    jle     short line_fallback_advance_x
line_fallback_adjust_start_x:
    mov     bx, [si+2]
    cmp     bx, cs:[_sprite1+26]
    jge     short line_fallback_record_top_clip
    add     [si+14h], ax
    jmp     short line_fallback_advance_x
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_fallback_record_top_clip:
    add     [si+18h], ax
line_fallback_advance_x:
    add     [si+2], dx
    jmp     short line_fallback_clip_y_loop
line_fallback_check_start_y:
    cmp     word ptr [si+0Ah], 3E80h
    jge     short line_fallback_clip_end_y
    cmp     word ptr [si+2], 0C180h
    jle     short line_fallback_adjust_start_y
    cmp     word ptr [si+2], 3E80h
    jge     short line_fallback_adjust_start_y
    cmp     word ptr [si+8], 0C180h
    jle     short line_fallback_clip_end_y
    cmp     word ptr [si+8], 3E80h
    jge     short line_fallback_clip_end_y
    jmp     line_clip_entry
line_fallback_clip_end_y:
    mov     ax, [si+0Ah]
    sub     ax, cx
    mov     [si+0Ah], ax
    mov     bx, ax
    sub     ax, cs:[_sprite1+18]
    inc     ax
    jge     short line_fallback_retreat_x
    neg     ax
    cmp     ax, cx
    jle     short line_fallback_adjust_end_x
    mov     ax, cx
line_fallback_adjust_end_x:
    sub     bx, cs:[_sprite1+16]
    inc     bx
    jge     short line_fallback_check_end_x
    add     ax, bx
    jle     short line_fallback_retreat_x
line_fallback_check_end_x:
    mov     bx, [si+8]
    cmp     bx, cs:[_sprite1+26]
    jge     short line_fallback_mark_bottom_clip
    add     [si+16h], ax
    jmp     short line_fallback_retreat_x
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
line_fallback_mark_bottom_clip:
    add     [si+1Ah], ax
line_fallback_retreat_x:
    sub     [si+8], dx
    jmp     short line_fallback_check_start_y
_clipline endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by parse shape2d helper.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _parse_shape2d_helper / _parse_shape2d_helper
; Convert a real-mode segment:offset pointer into its unsigned linear byte position.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] unsigned offset; [BP+8] unsigned segment.
; outputs: DX:AX=(segment<<4)+offset (up to 0x10ffef; no 20-bit wrap).
; preserves: BX; CX; SI; DI; BP; DS; ES; SS
; clobbers: AX; DX; arithmetic flags
; globals_read: none.
; globals_written: none.
; services: none.
; Liveness: live; Live: shape parsing inventory has five call leads; pointer-conversion counterpart is called
; by
; src/obj_seg035_group.c.
; Notes: Arithmetic only; does not parse a shape or touch video memory.
_segtoadr proc far
    arg_pointer_offset = word ptr 6
    arg_pointer_segment = word ptr 8
    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_pointer_segment]
    xor     dx, dx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     ax, [bp+arg_pointer_offset]
    adc     dx, 0
    pop     bp
    retf
_segtoadr endp
; -----------------------------------------------------------------------------
; Purpose: Implement the operation identified by parse shape2d helper2.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _parse_shape2d_helper2 / _parse_shape2d_helper2
; Convert a 32-bit byte position to a normalized real-mode pointer.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] position low word; [BP+8] high word.
; outputs: AX=position&15; DX=(position>>4)&0xffff; BX=high_word>>4 (discarded upper quotient). DX:AX is
; normalized
; segment:offset; values above 20 bits are truncated in returned segment.
; preserves: CX; SI; DI; BP; DS; ES; SS
; clobbers: AX; BX; DX; arithmetic flags
; globals_read: none.
; globals_written: none.
; services: none.
; Liveness: live; Live: lib/shp2res.c:112.
; Notes: Four mutable INT24 vector/callback words follow its RETF inside its PROC extent, not executed
; instructions.
_adrtoseg proc far
    arg_linear_low = word ptr 6
    arg_linear_high = word ptr 8
    push    bp
    mov     bp, sp
    mov     bx, [bp+arg_linear_high]
    mov     dx, [bp+arg_linear_low]
    mov     ax, dx
    shr     bx, 1
    rcr     dx, 1
    shr     bx, 1
    rcr     dx, 1
    shr     bx, 1
    rcr     dx, 1
    shr     bx, 1
    rcr     dx, 1
    and     ax, 0Fh
    pop     bp
    retf
; ASMDOC BLOCK: Mutable far vectors/callback words in CODE after RETF; callers intentionally write through CS.
saved_int24_vector_offset     dw 0
saved_int24_vector_segment     dw 0
criterr_handler_offset     dw 0
criterr_handler_segment     dw 0
_adrtoseg endp
; -----------------------------------------------------------------------------
; Purpose: Handle the critical-error interrupt and report its result.
; ABI: FAR entry; source return form: IRET; stack arguments: no named stack arguments referenced.
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, CS, DS, ES, SS; balanced explicit saves/restores observed:
; BX, CX, DI, DS, DX, ES, SI; path-specific inputs and outputs
;   follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(bios): returns from a hardware interrupt. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _criterr_interrupt_handler / _criterr_interrupt_handler
; Call the selected critical-error callback and return its AX result to DOS.
; Convention: FAR interrupt entry, not ordinary C callable; indirect FAR callback then IRET.
; inputs: Interrupt frame at entry SS:SP: IP, CS, FLAGS; DOS supplies critical-error register state.; Callback
; receives DS=SS, AX=SS and BX=offset criterr_handler_offset; CX,DX,SI,DI,ES,BP retain their interrupt values
; until
; callback. No C stack arguments are synthesized.
; outputs: Callback AX (in particular DOS response AL) survives IRET; flags restored from interrupt frame.
; preserves: BX; CX; DX; SI; DI; BP; DS; ES; SS; interrupted FLAGS via IRET
; clobbers: AX; callback global state
; globals_read: CS:criterr_handler_offset/segment
; globals_written: Callback-selected state
; services: DOS INT24 callback; IRET
; Liveness: live; Live: _set_criterr_handler installs this vector with INT21 AX=2524h.
; Notes: Does not forward incoming AX/BX unchanged: both are overwritten before callback. Eight other
; general/segment
; registers are explicitly saved.
_criterr_interrupt_handler proc far
    push    bx
    push    dx
    push    cx
    push    si
    push    di
    push    ds
    push    es
    push    bp
; ASMDOC BLOCK: Callback data segment becomes SS; original AX/BX interrupt details are not preserved as
; callback inputs.
    mov     ax, ss
    mov     ds, ax
    mov     bx, offset criterr_handler_offset
    call    dword ptr cs:[bx]
    pop     bp
    pop     es
    pop     ds
    pop     di
    pop     si
    pop     cx
    pop     dx
    pop     bx
    iret ;/* PLATFORM(bios): returns from a hardware interrupt. */
_criterr_interrupt_handler endp
; -----------------------------------------------------------------------------
; Purpose: Install the critical-error interrupt handler.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, DX, BP, SP, CS, DS, ES; balanced explicit saves/restores observed: DS; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(dos): calls a DOS service. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _set_criterr_handler / _set_criterr_handler
; Register cleanup, save INT24 and install a game critical-error callback.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] callback offset; [BP+8] callback segment; DS:DGROUP; SS must be callback data segment.
; outputs: INT24 points at _criterr_interrupt_handler; callback and prior vector saved; AX is DOS-dependent,
; no
; Boolean result defined.
; preserves: BP; DS; SI/DI through _add_exit_handler on normal return; SS
; clobbers: AX; BX; CX; DX; ES=0; flags
; globals_read: IVT 0000:0090/0092; _exitlistfuncs
; globals_written: CS:criterr_handler_offset/segment; CS:saved_int24_vector_offset/segment; _exitlistfuncs;
; DOS INT24
; vector
; services: Direct IVT read; INT21 AH=25h AL=24h; _add_exit_handler
; Liveness: live; Live: lib/startup.c:265 passes do_dea_textres.
; Notes: Vector and callback words reside in writable CODE storage; routine is non-reentrant.
_criterr proc far
    arg_callback_offset = word ptr 6
    arg_callback_segment = word ptr 8
    push    bp
    mov     bp, sp
    push    ds
    push    cs
    mov     ax, offset _resetcriterr
    push    ax
    call far ptr _addexit
    add     sp, 4
    mov     ax, [bp+arg_callback_offset]
    mov     cs:criterr_handler_offset, ax
    mov     ax, [bp+arg_callback_segment]
    mov     cs:criterr_handler_segment, ax
    xor     ax, ax
    mov     es, ax
    mov     bx, 90h
    mov     ax, es:[bx]
    mov     cs:saved_int24_vector_offset, ax
    mov     ax, es:[bx+2]
    mov     cs:saved_int24_vector_segment, ax
    mov     dx, offset _criterr_interrupt_handler
    mov     ax, cs
    mov     ds, ax
    mov     al, 24h
    mov     ah, 25h
    int     R5HW_DOS_INT ;/* PLATFORM(dos): calls a DOS service. */
    pop     ds
    pop     bp
    retf
_criterr endp
; -----------------------------------------------------------------------------
; Purpose: Restore the previous critical-error vector at process exit.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DX, CS, DS; balanced explicit saves/restores observed: DS; path-specific inputs
; and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; /* PLATFORM(dos): calls a DOS service. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _criterr_exithandler / _criterr_exithandler
; Restore the saved DOS critical-error vector unless its segment equals its offset.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: DS:DGROUP on normal callback entry; saved vector in CS. No stack arguments.
; outputs: DS restored; INT24 restored only if saved_segment-saved_offset != 0.
; preserves: DS; BX/CX/SI/DI/BP subject to documented DOS AH25 preservation; SS
; clobbers: AX; DX when restore is taken; flags
; globals_read: CS:saved_int24_vector_offset/segment
; globals_written: DOS INT24 vector conditionally
; services: INT21 AH=25h AL=24h
; Liveness: live; Live: its far address is registered by _set_criterr_handler.
; Notes: SUB compares segment with offset, not segment|offset against zero: any equal pair skips restoration.
_resetcriterr proc far
    push    ds
; ASMDOC BLOCK: Literal subtraction is surprising: equal nonzero segment:offset also skips vector restoration.
    mov     ax, cs:saved_int24_vector_segment
    sub     ax, cs:saved_int24_vector_offset
    jz      short criterr_restore_previous_vector
    mov     dx, cs:saved_int24_vector_offset
    mov     ds, cs:saved_int24_vector_segment
    mov     al, 24h
    mov     ah, 25h
    int     R5HW_DOS_INT ;/* PLATFORM(dos): calls a DOS service. */
criterr_restore_previous_vector:
    pop     ds
    retf
_resetcriterr endp
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender unk.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: arg_2 (word ptr 8); arg_4 (word ptr 8); arg_6 (word ptr 12);
;   arg_8 (word ptr 14).
; Register ABI: touches AX, SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: _imagefunc, _line_pattern_bits, _prerender_auxiliary_arg, _spritefunc.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _preRender_unk / _sprite_two_color_poly_list
; Select two-color patterned polygon callbacks and enter shared scan conversion using a point-list pointer.
; Convention: FAR C stack entry, caller cleanup; allocates 2000-byte local workspace, saves SI/DI, tail-jumps
; NEAR
; _prerender_shared_setup, which RETFs.
; inputs: [BP+6] pattern word; [BP+8] primary color word; [BP+10] auxiliary color word; [BP+12] signed point
; count.;
; [BP+14] near DS point-list pointer for list entry; inline SS point-list bytes at BP+14 for args entry. DS=SS
; required by shared stack workspace/near pointers.
; outputs: Draws through _draw_unknown_lines / _preRender_line; no defined C return value.; Rewrites caller
; [BP+6]=primary color and [BP+8]=point count before tail jump.
; preserves: BP; SI; DI restored by shared raster epilogue; DS/SS retained
; clobbers: AX; BX; CX; DX; ES through raster callbacks; flags
; globals_read: CS:_sprite1 descriptor via shared rasterizer; point-list memory; _line_pattern_bits via
; callback
; globals_written: _line_pattern_bits; _prerender_auxiliary_arg; _spritefunc (near callback offset);
; _imagefunc (near
; callback offset); caller first two argument words; active sprite pixels through callbacks
; services: No direct hardware; indexed destination may be video memory
; Liveness: live; Live: game/polygon.c:674.
; Notes: Mode byte [BP-10]=0 for inline-alt entry; normal list entry sets 1. An unlabelled, unreferenced list-
; mode-0
; prologue follows line 1524; private preRender_unk_mode_1 at 1575 uses inline list with mode=1 and is also
; unreferenced.
_sprite_two_color_poly_list proc far
    local_merge_mode = byte ptr -10
    arg_pattern = word ptr 6
    arg_primary_color = word ptr 8
    arg_point_count = word ptr 12
    arg_points = word ptr 14
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_points]
    mov     [bp+local_merge_mode], 1
    jmp     short prerender_setup_shared_core
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
; ASMDOC BLOCK: Unlabelled alternate prologue: mode 0 list entry. Normal entry jumps over it; no inbound
; source reference.
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_points]
    mov     [bp+local_merge_mode], 0
prerender_setup_shared_core:
    mov     ax, [bp+arg_pattern]
    mov     _line_pattern_bits, ax
    mov     ax, [bp+0Ah]
    mov     _prerender_auxiliary_arg, ax
    mov     ax, [bp+arg_primary_color]
    mov     [bp+arg_pattern], ax
    mov     ax, [bp+arg_point_count]
    mov     [bp+arg_primary_color], ax
    mov     ax, offset _draw_unknown_lines
    mov     _spritefunc, ax
    mov     ax, offset _preRender_line
    mov     _imagefunc, ax
    jmp     _polycmn
_sprite_two_color_poly_list endp
; -----------------------------------------------------------------------------
; Purpose: Implement the legacy internal helper nopsub 2F424.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: arg_8 (word ptr 14).
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _nopsub_2F424 / _sprite_two_color_poly_args_alt
; Select two-color patterned polygon callbacks and enter shared scan conversion using inline arguments.
; Convention: FAR C stack entry, caller cleanup; allocates 2000-byte local workspace, saves SI/DI, tail-jumps
; NEAR
; _prerender_shared_setup, which RETFs.
; inputs: [BP+6] pattern word; [BP+8] primary color word; [BP+10] auxiliary color word; [BP+12] signed point
; count.;
; [BP+14] near DS point-list pointer for list entry; inline SS point-list bytes at BP+14 for args entry. DS=SS
; required by shared stack workspace/near pointers.
; outputs: Draws through _draw_unknown_lines / _preRender_line; no defined C return value.; Rewrites caller
; [BP+6]=primary color and [BP+8]=point count before tail jump.
; preserves: BP; SI; DI restored by shared raster epilogue; DS/SS retained
; clobbers: AX; BX; CX; DX; ES through raster callbacks; flags
; globals_read: CS:_sprite1 descriptor via shared rasterizer; point-list memory; _line_pattern_bits via
; callback
; globals_written: _line_pattern_bits; _prerender_auxiliary_arg; _spritefunc (near callback offset);
; _imagefunc (near
; callback offset); caller first two argument words; active sprite pixels through callbacks
; services: No direct hardware; indexed destination may be video memory
; Liveness: dead; Dead by static source and supplied rename plan: no incoming reference to this entry; its
; shared body
; is live.
; Notes: Mode byte [BP-10]=0 for inline-alt entry; normal list entry sets 1. An unlabelled, unreferenced list-
; mode-0
; prologue follows line 1524; private preRender_unk_mode_1 at 1575 uses inline list with mode=1 and is also
; unreferenced.
_sprite_two_color_poly_args_alt proc far
    local_merge_mode = byte ptr -10
    arg_inline_points = byte ptr 14
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    lea     si, [bp+arg_inline_points]
    mov     [bp+local_merge_mode], 0
    jmp     short prerender_setup_shared_core
_sprite_two_color_poly_args_alt endp
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender unk mode 1.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: arg_8 (word ptr 14).
; Register ABI: touches SI, DI, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC BLOCK: Private alternate entry, named in existing rename plan; same frame, inline points, mode flag
; 1.
preRender_unk_mode_1 proc far
    local_merge_mode = byte ptr -10
    arg_inline_points = byte ptr 14
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    lea     si, [bp+arg_inline_points]
    mov     [bp+local_merge_mode], 1
    jmp     short prerender_setup_shared_core
; ASMDOC BLOCK: The remainder of this PROC until ENDP is CS slope/profile data, not executable polygon code.
line_slope_table_limit     dw 50
line_interp_profile_offsets     dw offset line_interp_fallback_offset
    dw offset line_interp_fallback_offset
    dw offset line_interp_fallback_offset
    dw offset line_interp_profile_03
    dw offset line_interp_profile_04
    dw offset line_interp_profile_05
    dw offset line_interp_profile_06
    dw offset line_interp_profile_07
    dw offset line_interp_profile_08
    dw offset line_interp_profile_09
    dw offset line_interp_profile_10
    dw offset line_interp_profile_11
    dw offset line_interp_profile_12
    dw offset line_interp_profile_13
    dw offset line_interp_profile_14
    dw offset line_interp_profile_15
    dw offset line_interp_profile_16
    dw offset line_interp_profile_17
    dw offset line_interp_profile_18
    dw offset line_interp_profile_19
    dw offset line_interp_profile_20
    dw offset line_interp_profile_21
    dw offset line_interp_profile_22
    dw offset line_interp_profile_23
    dw offset line_interp_profile_24
    dw offset line_interp_profile_25
    dw offset line_interp_profile_26
    dw offset line_interp_profile_27
    dw offset line_interp_profile_28
    dw offset line_interp_profile_29
    dw offset line_slope_ratio_row_30
    dw offset line_interp_profile_31
    dw offset line_interp_profile_32
    dw offset line_interp_profile_33
    dw offset line_interp_profile_34
    dw offset line_interp_profile_35
    dw offset line_interp_profile_36
    dw offset line_interp_profile_37
    dw offset line_interp_profile_38
    dw offset line_interp_profile_39
    dw offset line_interp_profile_40
    dw offset line_interp_profile_41
    dw offset line_interp_profile_42
    dw offset line_interp_profile_43
    dw offset line_interp_profile_44
    dw offset line_interp_profile_45
    dw offset line_interp_profile_46
    dw offset line_interp_profile_47
    dw offset line_interp_profile_48
line_interp_fallback_offset     dw offset line_interp_profile_49
; Unsigned Q16 fractions: initial 32768 (1/2), then floor(65536*k/n),
; n=3..49, k=1..n-1; truncate toward zero. Verified 1176/1176, no exceptions.
; profile_N labels the LAST fraction for denominator N-1, before fractions for N.
; The final row includes 48/49. Preserve the interior aliases used by the rasterizer.
; Each label retains the final fraction of the preceding denominator.
; Split 65536*k/n through 32768 quotient/remainder to avoid MASM 5.10 overflow.
INTERP_PROFILE MACRO profile_name, denominator, numerator_count
interp_half_step = 32768/(denominator-1)
interp_half_rem = 32768 MOD (denominator-1)
profile_name dw interp_half_step*2*(denominator-2)+(interp_half_rem*2*(denominator-2))/(denominator-1)
interp_half_step = 32768/denominator
interp_half_rem = 32768 MOD denominator
interp_numerator = 1
    REPT numerator_count
    dw interp_half_step*2*interp_numerator+(interp_half_rem*2*interp_numerator)/denominator
interp_numerator = interp_numerator + 1
    ENDM
    ENDM
; Interior word aliases used by the scan converter.
line_interp_p04_word_01 EQU line_interp_profile_04+2
line_interp_p28_word_19 EQU line_interp_profile_28+38
    INTERP_PROFILE line_interp_profile_03, 3, 1
    INTERP_PROFILE line_interp_profile_04, 4, 2
    INTERP_PROFILE line_interp_profile_05, 5, 3
    INTERP_PROFILE line_interp_profile_06, 6, 4
    INTERP_PROFILE line_interp_profile_07, 7, 5
    INTERP_PROFILE line_interp_profile_08, 8, 6
    INTERP_PROFILE line_interp_profile_09, 9, 7
    INTERP_PROFILE line_interp_profile_10, 10, 8
    INTERP_PROFILE line_interp_profile_11, 11, 9
    INTERP_PROFILE line_interp_profile_12, 12, 10
    INTERP_PROFILE line_interp_profile_13, 13, 11
    INTERP_PROFILE line_interp_profile_14, 14, 12
    INTERP_PROFILE line_interp_profile_15, 15, 13
    INTERP_PROFILE line_interp_profile_16, 16, 14
    INTERP_PROFILE line_interp_profile_17, 17, 15
    INTERP_PROFILE line_interp_profile_18, 18, 16
    INTERP_PROFILE line_interp_profile_19, 19, 17
    INTERP_PROFILE line_interp_profile_20, 20, 18
    INTERP_PROFILE line_interp_profile_21, 21, 19
    INTERP_PROFILE line_interp_profile_22, 22, 20
    INTERP_PROFILE line_interp_profile_23, 23, 21
    INTERP_PROFILE line_interp_profile_24, 24, 22
    INTERP_PROFILE line_interp_profile_25, 25, 23
    INTERP_PROFILE line_interp_profile_26, 26, 24
    INTERP_PROFILE line_interp_profile_27, 27, 25
    INTERP_PROFILE line_interp_profile_28, 28, 26
    INTERP_PROFILE line_interp_profile_29, 29, 27
    INTERP_PROFILE line_slope_ratio_row_30, 30, 28
    INTERP_PROFILE line_interp_profile_31, 31, 29
    INTERP_PROFILE line_interp_profile_32, 32, 30
    INTERP_PROFILE line_interp_profile_33, 33, 31
    INTERP_PROFILE line_interp_profile_34, 34, 32
    INTERP_PROFILE line_interp_profile_35, 35, 33
    INTERP_PROFILE line_interp_profile_36, 36, 34
    INTERP_PROFILE line_interp_profile_37, 37, 35
    INTERP_PROFILE line_interp_profile_38, 38, 36
    INTERP_PROFILE line_interp_profile_39, 39, 37
    INTERP_PROFILE line_interp_profile_40, 40, 38
    INTERP_PROFILE line_interp_profile_41, 41, 39
    INTERP_PROFILE line_interp_profile_42, 42, 40
    INTERP_PROFILE line_interp_profile_43, 43, 41
    INTERP_PROFILE line_interp_profile_44, 44, 42
    INTERP_PROFILE line_interp_profile_45, 45, 43
    INTERP_PROFILE line_interp_profile_46, 46, 44
    INTERP_PROFILE line_interp_profile_47, 47, 45
    INTERP_PROFILE line_interp_profile_48, 48, 46
    INTERP_PROFILE line_interp_profile_49, 49, 48
preRender_unk_mode_1 endp
; -----------------------------------------------------------------------------
; Purpose: Prepare rendering data for the operation identified by preRender line.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_color (word ptr 14);
; arg_endX (word ptr 10); arg_endY (word ptr 12); arg_startX (word
;   ptr 6); arg_startY (word ptr 8).
; Register ABI: touches AX, BX, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _preRender_line / _preRender_line
; Clip a line into a local descriptor and rasterize it when visible and nonempty.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] start x; [BP+8] start y; [BP+10] end x; [BP+12] end y (signed words).; [BP+14] color word
; (raster
; uses low byte); DS=SS for the stack-local 28-byte descriptor.
; outputs: Active sprite pixels updated if clip AX=0 and signed step count>0; no defined scalar result.
; preserves: BP; SI; DI through both callees; DS; SS
; clobbers: AX; BX; CX; DX; ES through pixel worker; flags
; globals_read: CS:_sprite1 clip bounds and surface/row table; CS:line interpolation tables
; globals_written: active sprite destination pixels
; services: No direct hardware; pixel destination can be video memory
; Liveness: live; Live: many C line callers and _imagefunc callback set by two-color polygon setup.
; Evidence: lib/gfxres.ASM:2824
_preRender_line proc far
    local_line_descriptor = byte ptr -28
    arg_startX = word ptr 6
    arg_startY = word ptr 8
    arg_endX = word ptr 10
    arg_endY = word ptr 12
    arg_color = word ptr 14
    push    bp
    mov     bp, sp
    sub     sp, 1Ch
    mov     ax, [bp+arg_color]
    lea     bx, [bp+local_line_descriptor]
    mov     [bx+10h], ax
    push    bx
    push    [bp+arg_endY]
    push    [bp+arg_endX]
    push    [bp+arg_startY]
    push    [bp+arg_startX]
    call    _clipline
    add     sp, 0Ah
    or      ax, ax
    jnz     short prerender_line_return
    lea     bx, [bp+local_line_descriptor]
    cmp     word ptr [bx+0Eh], 0
    jle     short prerender_line_return
    push    bx
    call    _drawcline
    add     sp, 2
prerender_line_return:
    mov     sp, bp
    pop     bp
    retf
_preRender_line endp
; -----------------------------------------------------------------------------
; Purpose: Add a far callback to the process exit-handler table.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, BX, CX, DX, BP, SP; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _aExitListOverflow, _exitlistfuncs.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _add_exit_handler / _add_exit_handler
; Append a unique far cleanup callback to a ten-slot table or abort on overflow.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] callback offset; [BP+8] callback segment; DS:DGROUP. Segment zero is an empty-slot marker,
; not a
; usable callback.
; outputs: AX=callback offset; DX=callback segment on normal return; duplicate is ignored; sentinel segment at
; following slot cleared.
; preserves: SI; DI; BP; DS; ES; SS
; clobbers: AX; BX; CX; DX; flags
; globals_read: _exitlistfuncs; _aExitListOverflow on overflow
; globals_written: _exitlistfuncs
; services: _fatal_error on overflow
; Liveness: live; Live: keyboard, timer, video, critical-error and audio setup register callbacks.
; Notes: 44 bytes = ten registration slots plus one sentinel. A zero-segment insertion can overwrite an
; existing empty
; slot repeatedly; normal callers use nonzero code segments.
_addexit proc far
    arg_callback_offset = word ptr 6
    arg_callback_segment = word ptr 8
    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_callback_offset]
    mov     cx, 0Ah
    mov     bx, offset DGROUP:_exitlistfuncs
    mov     dx, [bp+arg_callback_segment]
exitlist_search_callback:
    cmp     [bx], ax
    jnz     short exitlist_check_free_slot
    cmp     [bx+2], dx
    jz      short exitlist_return
exitlist_check_free_slot:
    cmp     word ptr [bx+2], 0
    jz      short exitlist_overflow_error
    add     bx, 4
    loop    exitlist_search_callback
    mov     ax, offset DGROUP:_addexiterr
    push    ax
    call    far ptr _abortmessage
; ASMDOC BLOCK: Free-slot insertion clears the following segment as sentinel; label name overflow_error is
; misleading.
exitlist_overflow_error:
    mov     word ptr [bx+2], 0
    mov     [bx], ax
    mov     word ptr [bx+6], 0
    mov     [bx+2], dx
exitlist_return:
    pop     bp
    retf
_addexit endp
; -----------------------------------------------------------------------------
; Purpose: Call the registered process exit handlers in reverse order.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, DI; balanced explicit saves/restores observed: DI; path-specific inputs and
; outputs follow the listed operands.
; Globals read: _exitlistfuncs.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _call_exitlist / _call_exitlist
; Invoke all nonzero-segment cleanup callbacks from the highest slot to the lowest.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: DS:DGROUP; no stack arguments; callback code must keep DS usable for subsequent table reads.
; outputs: Table retained (callbacks may alter their own state); callback effects otherwise unrestricted.
; preserves: DI explicitly saved around every callback and restored at exit; BP/SI/DS/ES only if callback ABI
; preserves them; SS
; clobbers: AX directly; other registers/flags according to each far callback (not saved by dispatcher)
; globals_read: _exitlistfuncs
; globals_written: callback-owned globals only; dispatcher does not erase table
; services: Indirect FAR calls
; Liveness: live; Live: _fatal_error and _call_exitlist2.
; Notes: Scans eleven physical entries, offsets 40..0 by -4; the last is the sentinel registration keeps zero.
_call_exitlist proc far
    push    di
    mov     di, 28h
exitlist_callback_scan:
    mov     ax, (_exitlistfuncs+2)[di]
    or      ax, ax
    jz      short exitlist_next_callback
    push    di
    call    dword ptr _exitlistfuncs[di]
    pop     di
exitlist_next_callback:
    sub     di, 4
    jl      short exitlist_callbacks_done
    jmp     short exitlist_callback_scan
exitlist_callbacks_done:
    pop     di
    retf
_call_exitlist endp
; -----------------------------------------------------------------------------
; Purpose: Call the exit-handler list and terminate the process.
; ABI: NEAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches AX; balanced explicit saves/restores observed: none inferred; path-specific inputs and
; outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _call_exitlist2 / _call_exitlist2
; Dispatch cleanup callbacks then terminate with CRT exit status zero.
; Convention: Declared NEAR; accepts no arguments and never returns, so FAR callers also work
; (game/uires.c).
; inputs: DS:DGROUP; no parameters.
; outputs: Does not return; _exit(0).
; preserves: none.
; clobbers: All call/cleanup effects; no returning ABI.
; globals_read: _exitlistfuncs
; globals_written: callback-owned state and process termination
; services: _call_exitlist; CRT _exit
; Liveness: live; Live: game/uires.c:1776.
; Notes: DB 0 follows the nonreturning call; no RET instruction.
_dsiexit proc near
    call far ptr _call_exitlist
    xor     ax, ax
    push    ax
    call    far ptr _exit
; ASMDOC BLOCK: Nonreturning CRT exit is followed by a retained DB 0 byte, not a return instruction.
    db 0 ; retained zero fill after the nonreturning exit call
_dsiexit endp
; -----------------------------------------------------------------------------
; Purpose: Return the file size rounded up to DOS memory paragraphs.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: arg_fatal (word ptr 8); arg_filename (word ptr 6).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_paras / _file_paras
; Read physical file length and return its rounded-up paragraph count.
; Convention: FAR C stack entry, caller cleanup; shared body ends RETF.
; inputs: [BP+6] near DS ASCIIZ filename.; [BP+8] fatal policy: zero returns zero on handled failure; nonzero
; calls
; _fatal_error.
; outputs: AX=ceil(DOS seek DX:AX byte length/16) modulo 65536; zero on handled nonfatal failure.;
; Seek/readjust/close
; errors after open are not checked; only open failure selects policy.
; preserves: BP; SI; DI; DS; SS; ES under DOS service ABI
; clobbers: AX; BX; CX; DX; flags
; globals_read: _aSFileError_1 only for fatal reporting
; globals_written: DOS file handle/seek state
; services: INT21 AH=3Dh(open), AX=4202h/4200h(seek), AH=3Eh(close); _fatal_error on policy failure
; Liveness: live; Live: resource_file_io_decompression calls this public entry.
; Notes: Result truncates to 16 bits. Nonfatal and normal prologues JMP over the fatal prologue into its PROC
; extent;
; not natural fallthrough.
_filesizea proc far
    var_fatal = word ptr -6
    var_length = word ptr -4
    var_filehandle = word ptr -2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     ax, [bp+arg_fatal]
    mov     [bp+var_fatal], ax
    jmp     short file_paras_body
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
_filesizea endp
; -----------------------------------------------------------------------------
; Purpose: Return the file size in DOS paragraphs without fatal error handling.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_paras_nofatal / _file_paras_nofatal
; Read physical file length and return its rounded-up paragraph count with nonfatal policy.
; Convention: FAR C stack entry, caller cleanup; shared body ends RETF.
; inputs: [BP+6] near DS ASCIIZ filename.; No policy argument: forces 0 (nonfatal).
; outputs: AX=ceil(DOS seek DX:AX byte length/16) modulo 65536; zero on handled nonfatal failure.;
; Seek/readjust/close
; errors after open are not checked; only open failure selects policy.
; preserves: BP; SI; DI; DS; SS; ES under DOS service ABI
; clobbers: AX; BX; CX; DX; flags
; globals_read: _aSFileError_1 only for fatal reporting
; globals_written: DOS file handle/seek state
; services: INT21 AH=3Dh(open), AX=4202h/4200h(seek), AH=3Eh(close); _fatal_error on policy failure
; Liveness: dead; Dead standalone entry: no incoming source/name-plan/inventory reference; its body is live
; via two
; sibling entries.
; Notes: Result truncates to 16 bits. Nonfatal and normal prologues JMP over the fatal prologue into its PROC
; extent;
; not natural fallthrough.
_filesizez proc far
    local_fatal_policy = word ptr -6
    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+local_fatal_policy], 0
    jmp     short file_paras_body
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
_filesizez endp
; -----------------------------------------------------------------------------
; Purpose: Return the file size in DOS paragraphs with fatal error handling.
; ABI: NEAR entry; source return form: RETF mnemonic for NEAR entry; stack arguments: arg_fatal (word ptr 8);
; arg_filename (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, DS, SS; balanced explicit saves/restores observed: DI,
; DS, DX, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _aSFileError_1.
; /* PLATFORM(file): calls a DOS file service. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_paras_fatal / _file_paras_fatal
; Read physical file length and return its rounded-up paragraph count with fatal policy.
; Convention: Declared NEAR but BP+6 frame and RETF require a FAR C call; caller cleanup.
; inputs: [BP+6] near DS ASCIIZ filename.; No policy argument: forces 1 (fatal).
; outputs: AX=ceil(DOS seek DX:AX byte length/16) modulo 65536; zero on handled nonfatal failure.;
; Seek/readjust/close
; errors after open are not checked; only open failure selects policy.
; preserves: BP; SI; DI; DS; SS; ES under DOS service ABI
; clobbers: AX; BX; CX; DX; flags
; globals_read: _aSFileError_1 only for fatal reporting
; globals_written: DOS file handle/seek state
; services: INT21 AH=3Dh(open), AX=4202h/4200h(seek), AH=3Eh(close); _fatal_error on policy failure
; Liveness: dead; Dead standalone entry: no incoming source/name-plan/inventory reference; its body is live
; via two
; sibling entries.
; Notes: Result truncates to 16 bits. Nonfatal and normal prologues JMP over the fatal prologue into its PROC
; extent;
; not natural fallthrough.
; ASMDOC BLOCK: Declared NEAR is misleading: shared arguments start at BP+6 and all normal exits execute RETF.
_filesize proc near
    var_fatal = word ptr -6
     s = byte ptr 0
    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 1
    var_fatal = word ptr -6
    var_length = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
file_paras_body:
    mov     dx, [bp+arg_filename]
    mov     ah, 3Dh
    xor     al, al
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
    jnb     short file_paras_file_opened
    cmp     [bp+var_fatal], 0
    jnz     short file_paras_open_error
    xor     ax, ax
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
file_paras_open_error:
    mov     ax, offset DGROUP:_aSFileError_1
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    push    ax
    call    far ptr _abortmessage
file_paras_file_opened:
    mov     [bp+var_filehandle], ax
    mov     bx, ax
    sub     cx, cx
    sub     dx, dx
    mov     ax, 4202h
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
    push    dx
    push    ax
    sub     dx, dx
    mov     ax, 4200h
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
    pop     ax
    pop     dx
    xor     bx, bx
    test    ax, 0Fh
    jz      short file_paras_count_from_bytes
    mov     bx, 1
file_paras_count_from_bytes:
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    add     ax, bx
    mov     [bp+var_length], ax
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
    mov     ax, [bp+var_length]
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_filesize endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file decomp paras.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: arg_fatal (word ptr 8); arg_filename (word ptr 6).
; Register ABI: touches AX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_decomp_paras / _file_decomp_paras
; Read packed header output length and return its rounded-up paragraph count.
; Convention: FAR C stack entry, caller cleanup; shared body ends RETF.
; inputs: [BP+6] near DS ASCIIZ filename; DS=SS needed for local packed-header read.; [BP+8] fatal policy:
; zero
; returns zero on handled failure; nonzero calls _fatal_error.
; outputs: AX=ceil(header bytes 1..3/16) modulo 65536; zero on handled nonfatal failure.; No file payload is
; decompressed.
; preserves: BP; SI; DI; DS; SS; ES under DOS service ABI
; clobbers: AX; BX; CX; DX; flags
; globals_read: _aSFileError_1 only for fatal reporting
; globals_written: DOS file handle/seek state; stack-local header
; services: INT21 AH=3Dh(open), 3Fh(read four bytes), 3Eh(close); _fatal_error on policy failure
; Liveness: live; Live: resource_file_io_decompression calls this public entry.
; Notes: Header type byte is ignored; upper byte of 24-bit length contributes to DX before shifting, but
; returned AX
; truncates beyond 65535 paragraphs. Short read is recognized by AX != 4, not CF. Initial MOV DX,filename is
; duplicated.
_unpacksizea proc far
    var_fatal = word ptr -8
    var_6 = byte ptr -6
    var_5 = word ptr -5
    var_filehandle = word ptr -2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    push    si
    push    di
    mov     ax, [bp+arg_fatal]
    mov     [bp+var_fatal], ax
    jmp     short file_decomp_paras_body
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
_unpacksizea endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file decomp paras nofatal.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_decomp_paras_nofatal / _file_decomp_paras_nofatal
; Read packed header output length and return its rounded-up paragraph count with nonfatal policy.
; Convention: FAR C stack entry, caller cleanup; shared body ends RETF.
; inputs: [BP+6] near DS ASCIIZ filename; DS=SS needed for local packed-header read.; No policy argument:
; forces 0
; (nonfatal).
; outputs: AX=ceil(header bytes 1..3/16) modulo 65536; zero on handled nonfatal failure.; No file payload is
; decompressed.
; preserves: BP; SI; DI; DS; SS; ES under DOS service ABI
; clobbers: AX; BX; CX; DX; flags
; globals_read: _aSFileError_1 only for fatal reporting
; globals_written: DOS file handle/seek state; stack-local header
; services: INT21 AH=3Dh(open), 3Fh(read four bytes), 3Eh(close); _fatal_error on policy failure
; Liveness: dead; Dead standalone entry: no incoming source/name-plan/inventory reference; its body is live
; via two
; sibling entries.
; Notes: Header type byte is ignored; upper byte of 24-bit length contributes to DX before shifting, but
; returned AX
; truncates beyond 65535 paragraphs. Short read is recognized by AX != 4, not CF. Initial MOV DX,filename is
; duplicated.
_unpacksizez proc far
    var_fatal = word ptr -8
    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short file_decomp_paras_body
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
_unpacksizez endp
; -----------------------------------------------------------------------------
; Purpose: Perform the resource-file operation identified by file decomp paras fatal.
; ABI: NEAR entry; source return form: RETF mnemonic for NEAR entry; stack arguments: arg_fatal (word ptr 8);
; arg_filename (word ptr 6).
; Register ABI: touches AX, BX, CX, DX, SI, DI, BP, SP, DS, SS; balanced explicit saves/restores observed: DI,
; DS, SI; path-specific inputs and outputs follow the listed
;   operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _aSFileError_1.
; /* PLATFORM(file): calls a DOS file service. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_decomp_paras_fatal / _file_decomp_paras_fatal
; Read packed header output length and return its rounded-up paragraph count with fatal policy.
; Convention: Declared NEAR but BP+6 frame and RETF require a FAR C call; caller cleanup.
; inputs: [BP+6] near DS ASCIIZ filename; DS=SS needed for local packed-header read.; No policy argument:
; forces 1
; (fatal).
; outputs: AX=ceil(header bytes 1..3/16) modulo 65536; zero on handled nonfatal failure.; No file payload is
; decompressed.
; preserves: BP; SI; DI; DS; SS; ES under DOS service ABI
; clobbers: AX; BX; CX; DX; flags
; globals_read: _aSFileError_1 only for fatal reporting
; globals_written: DOS file handle/seek state; stack-local header
; services: INT21 AH=3Dh(open), 3Fh(read four bytes), 3Eh(close); _fatal_error on policy failure
; Liveness: dead; Dead standalone entry: no incoming source/name-plan/inventory reference; its body is live
; via two
; sibling entries.
; Notes: Header type byte is ignored; upper byte of 24-bit length contributes to DX before shifting, but
; returned AX
; truncates beyond 65535 paragraphs. Short read is recognized by AX != 4, not CF. Initial MOV DX,filename is
; duplicated.
; ASMDOC BLOCK: Declared NEAR is misleading here too: entry must use a FAR frame; sibling prologues jump into
; body.
_unpacksize proc near
    var_fatal = word ptr -8
     s = byte ptr 0
    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 1
    var_fatal = word ptr -8
    var_6 = byte ptr -6
    var_5 = word ptr -5
    var_filehandle = word ptr -2
     s = byte ptr 0
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
file_decomp_paras_body:
; ASMDOC BLOCK: Duplicate MOV DX is retained exactly; no extra semantic operation.
    mov     dx, [bp+arg_filename]
    mov     dx, [bp+arg_filename]
    mov     ah, 3Dh
    xor     al, al
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
    jnb     short decomp_file_read_header
    jmp     short decomp_nonfatal_return
    nop ; retained code pad; EVEN is illegal in a BYTE-aligned segment
    var_fatal = word ptr -8
    var_6 = byte ptr -6
    var_5 = word ptr -5
    var_filehandle = word ptr -2
     s = byte ptr 0
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
decomp_file_read_header:
    mov     [bp+var_filehandle], ax
    mov     bx, [bp+var_filehandle]
    lea     dx, [bp+var_6]
    mov     cx, 4
    mov     ah, 3Fh
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
    cmp     ax, cx
    jnz     short decomp_close_short_read
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
    mov     ax, [bp+var_5]
    mov     dx, [bp-3]
    xor     dh, dh
    xor     bx, bx
    test    ax, 0Fh
    jz      short decomp_convert_size_to_paras
    mov     bx, 1
decomp_convert_size_to_paras:
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    shr     dx, 1
    rcr     ax, 1
    add     ax, bx
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
decomp_close_short_read:
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
decomp_nonfatal_return:
    cmp     [bp+var_fatal], 0
    jnz     short decomp_report_fatal_error
    xor     ax, ax
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
decomp_report_fatal_error:
    mov     ax, offset DGROUP:_aSFileError_1
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    push    ax
decomp_call_fatal_handler:
    call    far ptr _abortmessage
_unpacksize endp
; -----------------------------------------------------------------------------
; Purpose: Search for the first file matching the DOS search pattern.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6).
; Register ABI: touches AX, CX, DX, SI, DI, BP, SP, DS, ES; balanced explicit saves/restores observed: DI, DS,
; SI; path-specific inputs and outputs follow the listed operands.
; Globals read: _foundfileptr.
; Globals written: _foundfileptr.
; Global addresses taken: _dtatransbuffer, _foundfile, _foundfilepath.
; /* PLATFORM(dos): calls a DOS service. */
; /* PLATFORM(file): calls a DOS file service. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_find / _file_find
; Start a DOS file search and return a shared full path with the matched filename.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] near DS ASCIIZ search pattern; DS:DGROUP; incoming DF=0 required by initial LODSB/STOSB
; loop.; Only
; CL=6 is set for findfirst attributes: incoming CH must be zero for CX=0006h.
; outputs: AX=near DS pointer to _foundfile on success, 0 on carry failure; ES=DS on success.; Copies pattern
; prefix
; through final colon/backslash, then appends exactly 13 DTA filename bytes; shared storage is overwritten by
; next
; search. DF=0 on success.
; preserves: BP; SI; DI; DS; SS
; clobbers: AX; CX; DX; ES on success; flags/DF on success; BX subject to DOS service
; globals_read: caller pattern; DOS search DTA; _foundfilepath; _foundfileptr during join
; globals_written: DOS current DTA/search state; _dtatransbuffer and adjacent _foundfilepath by DOS;
; _foundfile;
; _foundfileptr
; services: INT21 AH=1Ah(set DTA), AH=4Eh(findfirst)
; Liveness: live; Live: src/obj_seg000 etc and shape/file loaders.
; Notes: DTA begins at a 30-byte block; its filename field deliberately continues into adjacent _foundfilepath
; storage. Does not restore former DOS DTA.
_findfirst proc far
    arg_filename = word ptr 6
    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, offset DGROUP:_dtatransbuffer
    mov     ah, 1Ah
    int     R5HW_DOS_INT ;/* PLATFORM(dos): calls a DOS service. */
    mov     dx, [bp+arg_filename]
; ASMDOC BLOCK: CL alone is initialized: attribute mask also depends on incoming CH (normally zero).
    mov     cl, 6
    mov     ah, 4Eh
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
    jb      short _file_find_err
    mov     di, offset DGROUP:_foundfile
    mov     _foundfileptr, di
    mov     si, [bp+arg_filename]
    mov     cx, 57h
    mov     ax, ds
    mov     es, ax
; ASMDOC BLOCK: Initial string copy assumes DF=0; explicit CLD occurs only in the common filename-append tail.
file_find_copy_pattern_loop:
    lodsb
    stosb
    cmp     al, 0
    jz      short _file_find_ok
    cmp     al, 3Ah
    jz      short file_find_record_path_suffix
    cmp     al, 5Ch
    jnz     short file_find_continue_pattern_copy
file_find_record_path_suffix:
    mov     _foundfileptr, di
file_find_continue_pattern_copy:
    loop    file_find_copy_pattern_loop
_file_find_ok:
    mov     ax, ds
    mov     es, ax
    mov     si, offset DGROUP:_foundfilepath
    mov     di, _foundfileptr
    mov     cx, 0Dh
    cld
    rep movsb
    mov     ax, offset DGROUP:_foundfile
file_find_return:
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
_file_find_err:
    xor     ax, ax
    jmp     short file_find_return
_findfirst endp
; -----------------------------------------------------------------------------
; Purpose: Continue the current DOS file search.
; ABI: FAR entry; source return form: no return opcode in this extent (tail/shared-code exit); stack
; arguments: no named stack arguments referenced.
; Register ABI: touches DX, SI, DI, BP, SP, DS; balanced explicit saves/restores observed: none inferred;
; path-specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; Global addresses taken: _dtatransbuffer.
; /* PLATFORM(dos): calls a DOS service. */
; /* PLATFORM(file): calls a DOS file service. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _file_find_next / _file_find_next
; Continue the shared DOS search and overwrite the prior result filename in place.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: No arguments; DS:DGROUP and successful prior _file_find required for _foundfileptr/search context.
; outputs: AX=near _foundfile pointer or 0; ES=DS and DF=0 on success; prefix retained.
; preserves: BP; SI; DI; DS; SS
; clobbers: AX; CX; DX; ES on success; flags; BX subject to DOS service
; globals_read: DOS DTA/search context; _foundfilepath; _foundfileptr; _foundfile prefix
; globals_written: DOS current DTA/search state; _dtatransbuffer and adjacent _foundfilepath; _foundfile
; suffix
; services: INT21 AH=1Ah, AH=4Fh(findnext)
; Liveness: live; Live: file_find_next_alt source/inventory wrapper.
; Notes: Jumps backward into _file_find_ok/_file_find_err and returns through their epilogue, using the
; identical
; saved frame. DB 0 is unreachable padding.
_findnext proc far
    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, offset DGROUP:_dtatransbuffer
    mov     ah, 1Ah
    int     R5HW_DOS_INT ;/* PLATFORM(dos): calls a DOS service. */
    mov     ah, 4Fh
    int     R5HW_DOS_INT ;/* PLATFORM(file): calls a DOS file service. */
; ASMDOC BLOCK: Both branches use the earlier routine shared result/epilogue, not this PROC extent alone.
    jb      short _file_find_err
    jmp     short _file_find_ok
    db 0 ; retained zero fill at this boundary
_findnext endp
; -----------------------------------------------------------------------------
; Purpose: Multiply two values and apply the fixed-point scaling shift.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: arg_0 (word ptr 6); arg_2
; (word ptr 8).
; Register ABI: touches AX, DX, BP, SP; balanced explicit saves/restores observed: none inferred; path-
; specific inputs and outputs follow the listed operands.
; Globals read: none identified.
; Globals written: none identified.
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _mulscl / _mulscl
; Multiply signed words and return a rounded fixed-point product scaled by 2^14.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: [BP+6] signed multiplicand; [BP+8] signed multiplier.
; outputs: AX=DX=((signed product<<2)>>16)+(carry from original product bit13), modulo 65536; equivalent
; rounded Q14
; product.
; preserves: BX; CX; SI; DI; BP; DS; ES; SS
; clobbers: AX; DX; flags
; globals_read: none.
; globals_written: none.
; services: none.
; Liveness: live; Live: game/carsim.c:944 and lib/sphhelp2.c:38, among many
; accepted C
; callers.
; Notes: Two full SHL/RCL pairs, then SHL AX / ADC DX,0 for rounding. The third shift does not double DX.
_fixedmult proc far
    arg_left_factor = word ptr 6
    arg_right_factor = word ptr 8
    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_left_factor]
    imul    [bp+arg_right_factor]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
; ASMDOC BLOCK: This final low-word shift supplies rounding carry; DX is incremented, not shifted a third
; time.
    shl     ax, 1
    adc     dx, 0
    mov     ax, dx
    pop     bp
    retf
_fixedmult endp
; -----------------------------------------------------------------------------
; Purpose: Configure the MCGA mode-4 display and clear its text page.
; ABI: FAR entry; source return form: RETF mnemonic for FAR entry; stack arguments: no named stack arguments
; referenced.
; Register ABI: touches AX, BX, CX, DX, DI, ES; balanced explicit saves/restores observed: DI; path-specific
; inputs and outputs follow the listed operands.
; Globals read: video_mode4_register_values.
; Globals written: _video_mode4_active.
; /* PLATFORM(video): calls a BIOS video service. */
; /* PLATFORM(video): accesses a VGA/MCGA display register port. */
; /* PLATFORM(video): selects a video-memory segment. */
; /* PLATFORM(video): writes display or sprite memory through ES:DI. */
; -----------------------------------------------------------------------------
; ASMDOC CONTRACT _video_set_mode4 / _video_set_mode4
; Select BIOS mode 4, program monochrome CRTC timing and clear 32 KiB at B800:0000.
; Convention: FAR, C stack arguments, caller cleanup; RETF
; inputs: DS:DGROUP; no arguments.
; outputs: _video_mode4_active low byte=1; ES=B800h; AX=008Ah; CX=0; BX=12; DX=03B8h; DI restored; DF=0.
; preserves: DI; SI/BP/DS under BIOS service ABI; SS
; clobbers: AX; BX; CX; DX; ES; arithmetic flags; DF cleared
; globals_read: BDA 0040:0010 equipment word; video_mode4_register_values
; globals_written: _video_mode4_active; BDA 0040:0010 equipment bits; B800:0000..7fff; display registers
; services: INT10 AH=0 AL=4; OUT 03BFh,03B8h,03B4h/03B5h
; Liveness: live; Live: lib/startup.c:236.
; Notes: Hardware constants denote monochrome CRTC/control ports; not VGA palette setup. Clears 0x4000 WORDS,
; not a 4
; KiB text page.
_video_set_mode4 proc far
    push    di
    mov     _video_mode4_active, 1
    mov     ax, 40h
    mov     es, ax
    mov     ax, es:10h
    and     ax, 0FFCFh
    or      ax, 20h
    mov     es:10h, ax
    mov     ah, 0
    mov     al, 4
    int     R5HW_BIOS_VIDEO_INT ;/* PLATFORM(video): calls a BIOS video service. */
    mov     al, 3
    mov     dx, R5HW_VIDEO_MONO_MISC_PORT
    out     dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    mov     al, 2
    mov     dx, R5HW_VIDEO_MONO_MODE_PORT
    out     dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    mov     cx, 0Ch
    mov     dx, R5HW_VIDEO_MONO_CRTC_INDEX_PORT
    xor     bx, bx
video_mode4_write_register_loop:
    mov     al, bl
    out     dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    inc     dx
    mov     al, video_mode4_register_values[bx]
    out     dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    dec     dx
    inc     bx
    loop    video_mode4_write_register_loop
    cld
    mov     ax, R5HW_VIDEO_TEXT_SEGMENT ;/* PLATFORM(video): selects a video-memory segment. */
    mov     es, ax
    xor     di, di
; ASMDOC BLOCK: 0x4000 words clears 0x8000 bytes in B800 memory.
    mov     cx, 4000h
    xor     ax, ax
    rep stosw ;/* PLATFORM(video): writes display or sprite memory through ES:DI. */
    mov     al, 8Ah
    mov     dx, R5HW_VIDEO_MONO_MODE_PORT
    out     dx, al ;/* PLATFORM(video): accesses a VGA/MCGA display register port. */
    pop     di
    retf
_video_set_mode4 endp
    db 0 ; retained zero fill at this boundary
S012_TEXT ends
end
