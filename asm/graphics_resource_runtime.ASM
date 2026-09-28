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
extrn _sprite_copy_2_to_1:far
extrn _flush_stdin:far
extrn _putpixel_line1_maybe:far
extrn _sprite1:word
extrn _draw_unknown_lines:far
extrn _loc_3180A:near
_DATA segment word public 'DATA'
_atantable db 0, 1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 8, 9, 00Ah
    db 00Ah, 00Bh, 00Bh, 00Ch, 00Dh, 00Dh, 00Eh, 00Fh, 00Fh, 010h, 010h, 011h, 012h, 012h, 013h, 014h
    db 014h, 015h, 016h, 016h, 017h, 017h, 018h, 019h, 019h, 01Ah, 01Bh, 01Bh, 01Ch, 01Ch, 01Dh, 01Eh
    db 01Eh, 01Fh, 01Fh, 020h, 021h, 021h, 022h, 022h, 023h, 024h, 024h, 025h, 026h, 026h, 027h, 027h
    db 028h, 029h, 029h, 02Ah, 02Ah, 02Bh, 02Ch, 02Ch, 02Dh, 02Dh, 02Eh, 02Eh, 02Fh, 030h, 030h, 031h
    db 031h, 032h, 033h, 033h, 034h, 034h, 035h, 035h, 036h, 037h, 037h, 038h, 038h, 039h, 039h, 03Ah
    db 03Ah, 03Bh, 03Ch, 03Ch, 03Dh, 03Dh, 03Eh, 03Eh, 03Fh, 03Fh, 040h, 041h, 041h, 042h, 042h, 043h
    db 043h, 044h, 044h, 045h, 045h, 046h, 046h, 047h, 047h, 048h, 048h, 049h, 04Ah, 04Ah, 04Bh, 04Bh
    db 04Ch, 04Ch, 04Dh, 04Dh, 04Eh, 04Eh, 04Fh, 04Fh, 050h, 050h, 051h, 051h, 052h, 052h, 053h, 053h
    db 054h, 054h, 054h, 055h, 055h, 056h, 056h, 057h, 057h, 058h, 058h, 059h, 059h, 05Ah, 05Ah, 05Bh
    db 05Bh, 05Bh, 05Ch, 05Ch, 05Dh, 05Dh, 05Eh, 05Eh, 05Fh, 05Fh, 060h, 060h, 060h, 061h, 061h, 062h
    db 062h, 063h, 063h, 063h, 064h, 064h, 065h, 065h, 066h, 066h, 066h, 067h, 067h, 068h, 068h, 068h
    db 069h, 069h, 06Ah, 06Ah, 06Ah, 06Bh, 06Bh, 06Ch, 06Ch, 06Ch, 06Dh, 06Dh, 06Eh, 06Eh, 06Eh, 06Fh
    db 06Fh, 070h, 070h, 070h, 071h, 071h, 071h, 072h, 072h, 073h, 073h, 073h, 074h, 074h, 074h, 075h
    db 075h, 076h, 076h, 076h, 077h, 077h, 077h, 078h, 078h, 078h, 079h, 079h, 079h, 07Ah, 07Ah, 07Ah
    db 07Bh, 07Bh, 07Bh, 07Ch, 07Ch, 07Ch, 07Dh, 07Dh, 07Dh, 07Eh, 07Eh, 07Eh, 07Fh, 07Fh, 07Fh, 080h
    db 080h, 0
input_deadline_tick_low dw 0
input_deadline_tick_high dw 0
expanded_8bit_pixel_mask_table dw 0, 3, 0000Ch, 0000Fh, 00030h, 00033h, 0003Ch, 0003Fh
    dw 000C0h, 000C3h, 000CCh, 000CFh, 000F0h, 000F3h, 000FCh, 000FFh
    dw 00300h, 00303h, 0030Ch, 0030Fh, 00330h, 00333h, 0033Ch, 0033Fh
    dw 003C0h, 003C3h, 003CCh, 003CFh, 003F0h, 003F3h, 003FCh, 003FFh
    dw 00C00h, 00C03h, 00C0Ch, 00C0Fh, 00C30h, 00C33h, 00C3Ch, 00C3Fh
    dw 00CC0h, 00CC3h, 00CCCh, 00CCFh, 00CF0h, 00CF3h, 00CFCh, 00CFFh
    dw 00F00h, 00F03h, 00F0Ch, 00F0Fh, 00F30h, 00F33h, 00F3Ch, 00F3Fh
    dw 00FC0h, 00FC3h, 00FCCh, 00FCFh, 00FF0h, 00FF3h, 00FFCh, 00FFFh
    dw 03000h, 03003h, 0300Ch, 0300Fh, 03030h, 03033h, 0303Ch, 0303Fh
    dw 030C0h, 030C3h, 030CCh, 030CFh, 030F0h, 030F3h, 030FCh, 030FFh
    dw 03300h, 03303h, 0330Ch, 0330Fh, 03330h, 03333h, 0333Ch, 0333Fh
    dw 033C0h, 033C3h, 033CCh, 033CFh, 033F0h, 033F3h, 033FCh, 033FFh
    dw 03C00h, 03C03h, 03C0Ch, 03C0Fh, 03C30h, 03C33h, 03C3Ch, 03C3Fh
    dw 03CC0h, 03CC3h, 03CCCh, 03CCFh, 03CF0h, 03CF3h, 03CFCh, 03CFFh
    dw 03F00h, 03F03h, 03F0Ch, 03F0Fh, 03F30h, 03F33h, 03F3Ch, 03F3Fh
    dw 03FC0h, 03FC3h, 03FCCh, 03FCFh, 03FF0h, 03FF3h, 03FFCh, 03FFFh
    dw 0C000h, 0C003h, 0C00Ch, 0C00Fh, 0C030h, 0C033h, 0C03Ch, 0C03Fh
    dw 0C0C0h, 0C0C3h, 0C0CCh, 0C0CFh, 0C0F0h, 0C0F3h, 0C0FCh, 0C0FFh
    dw 0C300h, 0C303h, 0C30Ch, 0C30Fh, 0C330h, 0C333h, 0C33Ch, 0C33Fh
    dw 0C3C0h, 0C3C3h, 0C3CCh, 0C3CFh, 0C3F0h, 0C3F3h, 0C3FCh, 0C3FFh
    dw 0CC00h, 0CC03h, 0CC0Ch, 0CC0Fh, 0CC30h, 0CC33h, 0CC3Ch, 0CC3Fh
    dw 0CCC0h, 0CCC3h, 0CCCCh, 0CCCFh, 0CCF0h, 0CCF3h, 0CCFCh, 0CCFFh
    dw 0CF00h, 0CF03h, 0CF0Ch, 0CF0Fh, 0CF30h, 0CF33h, 0CF3Ch, 0CF3Fh
    dw 0CFC0h, 0CFC3h, 0CFCCh, 0CFCFh, 0CFF0h, 0CFF3h, 0CFFCh, 0CFFFh
    dw 0F000h, 0F003h, 0F00Ch, 0F00Fh, 0F030h, 0F033h, 0F03Ch, 0F03Fh
    dw 0F0C0h, 0F0C3h, 0F0CCh, 0F0CFh, 0F0F0h, 0F0F3h, 0F0FCh, 0F0FFh
    dw 0F300h, 0F303h, 0F30Ch, 0F30Fh, 0F330h, 0F333h, 0F33Ch, 0F33Fh
    dw 0F3C0h, 0F3C3h, 0F3CCh, 0F3CFh, 0F3F0h, 0F3F3h, 0F3FCh, 0F3FFh
    dw 0FC00h, 0FC03h, 0FC0Ch, 0FC0Fh, 0FC30h, 0FC33h, 0FC3Ch, 0FC3Fh
    dw 0FCC0h, 0FCC3h, 0FCCCh, 0FCCFh, 0FCF0h, 0FCF3h, 0FCFCh, 0FCFFh
    dw 0FF00h, 0FF03h, 0FF0Ch, 0FF0Fh, 0FF30h, 0FF33h, 0FF3Ch, 0FF3Fh
    dw 0FFC0h, 0FFC3h, 0FFCCh, 0FFCFh, 0FFF0h, 0FFF3h, 0FFFCh, 0FFFFh
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
sphere_scanline_profile_00 db 1
sphere_scanline_profile_01 db 2, 3
sphere_scanline_profile_02 db 3, 4, 4
sphere_scanline_profile_03 db 3, 4, 5, 5
sphere_scanline_profile_04 db 4, 5, 6, 6, 6
sphere_scanline_profile_05 db 4, 6, 6, 7, 7, 8
sphere_scanline_profile_06 db 5, 6, 7, 8, 8, 9, 9
sphere_scanline_profile_07 db 5, 7, 8, 9, 9, 00Ah, 00Ah, 00Ah
sphere_scanline_profile_08 db 5, 7, 8, 9, 00Ah, 00Bh, 00Bh, 00Bh, 00Bh
sphere_scanline_profile_09 db 5, 8, 9, 00Ah, 00Bh, 00Bh, 00Ch, 00Ch, 00Ch, 00Dh
sphere_scanline_profile_10 db 6, 8, 9, 00Bh, 00Ch, 00Ch, 00Dh, 00Dh, 00Eh, 00Eh, 00Eh
sphere_scanline_profile_11 db 6, 8, 00Ah, 00Bh, 00Ch, 00Dh, 00Eh, 00Eh, 00Fh, 00Fh, 00Fh, 00Fh
sphere_scanline_profile_12 db 6, 9, 00Ah, 00Ch, 00Dh, 00Eh, 00Eh, 00Fh, 00Fh, 010h, 010h, 010h, 010h
sphere_scanline_profile_13 db 6, 9, 00Bh, 00Ch, 00Dh, 00Eh, 00Fh, 010h, 010h, 011h, 011h, 011h, 011h, 012h
sphere_scanline_profile_14 db 7, 9, 00Bh, 00Dh, 00Eh, 00Fh, 010h, 011h, 011h, 012h, 012h, 012h, 013h, 013h, 013h
sphere_scanline_profile_15 db 7, 00Ah, 00Ch, 00Dh, 00Fh, 010h, 011h, 011h, 012h, 013h, 013h, 013h, 014h, 014h, 014h, 014h
sphere_scanline_profile_16 db 7, 00Ah, 00Ch, 00Eh, 00Fh, 010h, 011h, 012h, 013h, 013h, 014h, 014h, 015h, 015h, 015h, 015h
    db 015h
sphere_scanline_profile_17 db 7, 00Ah, 00Ch, 00Eh, 010h, 011h, 012h, 013h, 013h, 014h, 015h, 015h, 016h, 016h, 016h, 016h
    db 016h, 017h
sphere_scanline_profile_18 db 8, 00Bh, 00Dh, 00Fh, 010h, 011h, 012h, 013h, 014h, 015h, 016h, 016h, 017h, 017h, 017h, 017h
    db 018h, 018h, 018h
sphere_scanline_profile_19 db 8, 00Bh, 00Dh, 00Fh, 011h, 012h, 013h, 014h, 015h, 016h, 016h, 017h, 017h, 018h, 018h, 018h
    db 019h, 019h, 019h, 019h
sphere_scanline_profile_20 db 8, 00Bh, 00Eh, 00Fh, 011h, 012h, 014h, 015h, 016h, 016h, 017h, 018h, 018h, 019h, 019h, 019h
    db 01Ah, 01Ah, 01Ah, 01Ah, 01Ah
sphere_scanline_profile_21 db 8, 00Bh, 00Eh, 010h, 011h, 013h, 014h, 015h, 016h, 017h, 018h, 018h, 019h, 01Ah, 01Ah, 01Ah
    db 01Bh, 01Bh, 01Bh, 01Bh, 01Bh, 01Ch
sphere_scanline_profile_22 db 8, 00Ch, 00Eh, 010h, 012h, 013h, 015h, 016h, 017h, 018h, 019h, 019h, 01Ah, 01Ah, 01Bh, 01Bh
    db 01Ch, 01Ch, 01Ch, 01Dh, 01Dh, 01Dh, 01Dh
sphere_scanline_profile_23 db 9, 00Ch, 00Fh, 011h, 012h, 014h, 015h, 016h, 017h, 018h, 019h, 01Ah, 01Bh, 01Bh, 01Ch, 01Ch
    db 01Dh, 01Dh, 01Dh, 01Eh, 01Eh, 01Eh, 01Eh, 01Eh
sphere_scanline_profile_24 db 9, 00Ch, 00Fh, 011h, 013h, 014h, 016h, 017h, 018h, 019h, 01Ah, 01Bh, 01Bh, 01Ch, 01Dh, 01Dh
    db 01Eh, 01Eh, 01Eh, 01Fh, 01Fh, 01Fh, 01Fh, 01Fh, 01Fh
sphere_scanline_profile_25 db 9, 00Dh, 00Fh, 011h, 013h, 015h, 016h, 017h, 019h, 01Ah, 01Bh, 01Bh, 01Ch, 01Dh, 01Dh, 01Eh
    db 01Eh, 01Fh, 01Fh, 020h, 020h, 020h, 020h, 020h, 020h, 021h
sphere_scanline_profile_26 db 9, 00Dh, 00Fh, 012h, 014h, 015h, 017h, 018h, 019h, 01Ah, 01Bh, 01Ch, 01Dh, 01Eh, 01Eh, 01Fh
    db 01Fh, 020h, 020h, 021h, 021h, 021h, 021h, 022h, 022h, 022h, 022h
sphere_scanline_profile_27 db 9, 00Dh, 010h, 012h, 014h, 016h, 017h, 018h, 01Ah, 01Bh, 01Ch, 01Dh, 01Eh, 01Eh, 01Fh, 020h
    db 020h, 021h, 021h, 022h, 022h, 022h, 022h, 023h, 023h, 023h, 023h, 023h
sphere_scanline_profile_28 db 9, 00Dh, 010h, 012h, 014h, 016h, 018h, 019h, 01Ah, 01Bh, 01Ch, 01Dh, 01Eh, 01Fh, 020h, 020h
    db 021h, 022h, 022h, 022h, 023h, 023h, 023h, 024h, 024h, 024h, 024h, 024h, 024h
sphere_scanline_profile_29 db 00Ah, 00Dh, 010h, 013h, 015h, 017h, 018h, 019h, 01Bh, 01Ch, 01Dh, 01Eh, 01Fh, 020h, 020h, 021h
    db 022h, 022h, 023h, 023h, 024h, 024h, 024h, 025h, 025h, 025h, 025h, 025h, 025h, 026h
sphere_scanline_profile_30 db 00Ah, 00Eh, 011h, 013h, 015h, 017h, 019h, 01Ah, 01Bh, 01Dh, 01Eh, 01Fh, 020h, 020h, 021h, 022h
    db 023h, 023h, 024h, 024h, 025h, 025h, 025h, 026h, 026h, 026h, 026h, 027h, 027h, 027h, 027h
sphere_scanline_profile_31 db 00Ah, 00Eh, 011h, 013h, 015h, 017h, 019h, 01Ah, 01Ch, 01Dh, 01Eh, 01Fh, 020h, 021h, 022h, 023h
    db 023h, 024h, 025h, 025h, 026h, 026h, 026h, 027h, 027h, 027h, 028h, 028h, 028h, 028h, 028h, 028h
sphere_scanline_profile_32 db 00Ah, 00Eh, 011h, 014h, 016h, 018h, 019h, 01Bh, 01Ch, 01Eh, 01Fh, 020h, 021h, 022h, 023h, 023h
    db 024h, 025h, 025h, 026h, 026h, 027h, 027h, 028h, 028h, 028h, 029h, 029h, 029h, 029h, 029h, 029h
    db 029h
sphere_scanline_profile_33 db 00Ah, 00Eh, 011h, 014h, 016h, 018h, 01Ah, 01Bh, 01Dh, 01Eh, 01Fh, 020h, 021h, 022h, 023h, 024h
    db 025h, 026h, 026h, 027h, 027h, 028h, 028h, 029h, 029h, 029h, 02Ah, 02Ah, 02Ah, 02Ah, 02Ah, 02Ah
    db 02Ah, 02Bh
sphere_scanline_profile_34 db 00Ah, 00Fh, 012h, 014h, 017h, 018h, 01Ah, 01Ch, 01Dh, 01Fh, 020h, 021h, 022h, 023h, 024h, 025h
    db 026h, 026h, 027h, 028h, 028h, 029h, 029h, 02Ah, 02Ah, 02Ah, 02Bh, 02Bh, 02Bh, 02Bh, 02Bh, 02Ch
    db 02Ch, 02Ch, 02Ch
sphere_scanline_profile_35 db 00Bh, 00Fh, 012h, 015h, 017h, 019h, 01Bh, 01Ch, 01Eh, 01Fh, 020h, 022h, 023h, 024h, 025h, 025h
    db 026h, 027h, 028h, 028h, 029h, 029h, 02Ah, 02Ah, 02Bh, 02Bh, 02Ch, 02Ch, 02Ch, 02Ch, 02Dh, 02Dh
    db 02Dh, 02Dh, 02Dh, 02Dh
sphere_scanline_profile_36 db 00Bh, 00Fh, 012h, 015h, 017h, 019h, 01Bh, 01Dh, 01Eh, 020h, 021h, 022h, 023h, 024h, 025h, 026h
    db 027h, 028h, 028h, 029h, 02Ah, 02Ah, 02Bh, 02Bh, 02Ch, 02Ch, 02Dh, 02Dh, 02Dh, 02Dh, 02Eh, 02Eh
    db 02Eh, 02Eh, 02Eh, 02Eh, 02Eh
sphere_scanline_profile_37 db 00Bh, 00Fh, 012h, 015h, 018h, 01Ah, 01Bh, 01Dh, 01Fh, 020h, 021h, 023h, 024h, 025h, 026h, 027h
    db 028h, 028h, 029h, 02Ah, 02Ah, 02Bh, 02Ch, 02Ch, 02Dh, 02Dh, 02Dh, 02Eh, 02Eh, 02Eh, 02Fh, 02Fh
    db 02Fh, 02Fh, 02Fh, 02Fh, 02Fh, 030h
sphere_scanline_profile_38 db 00Bh, 00Fh, 013h, 016h, 018h, 01Ah, 01Ch, 01Eh, 01Fh, 021h, 022h, 023h, 024h, 025h, 026h, 027h
    db 028h, 029h, 02Ah, 02Bh, 02Bh, 02Ch, 02Ch, 02Dh, 02Eh, 02Eh, 02Eh, 02Fh, 02Fh, 02Fh, 030h, 030h
    db 030h, 030h, 030h, 031h, 031h, 031h, 031h
_exitlistfuncs db 44 dup (0)
_aExitListOverflow db 'EXIT LIST OVERFLOW', 0Dh, 0Ah, 0, 0
_aSFileError_1 db '%s FILE ERROR', 0
_foundfile db 100 dup (0)
_foundfileptr dw 0
_dtatransbuffer db 30 dup (0)
_foundfilepath db 98 dup (0)
_video_mode4_active db 2 dup (0)
video_mode4_register_values db 035h, 028h, 02Ch, 7, 079h, 2, 064h, 06Eh, 2, 2, 0, 0
_DATA ends
DGROUP group _DATA,DSEG
S012_TEXT segment byte public 'CODE'
public _prerender_max_line_width
public _video_mode4_active
assume cs:S012_TEXT
assume es:nothing, ss:nothing, ds:DGROUP
public _fatal_error
public _polang
public _sub_2EAD4
public _set_add_value
public _poll_input_abort
public _wait_for_input_delay
public _draw_line_related_alt
public _draw_line_related
public _parse_shape2d_helper
public _parse_shape2d_helper2
public _criterr_interrupt_handler
public _set_criterr_handler
public _criterr_exithandler
public _preRender_unk
public _nopsub_2F424
public _preRender_line
public _add_exit_handler
public _call_exitlist
public _call_exitlist2
public _file_paras
public _file_paras_nofatal
public _file_paras_fatal
public _file_decomp_paras
public _file_decomp_paras_nofatal
public _file_decomp_paras_fatal
public _file_find
public _file_find_next
public _mulscl
public _video_set_mode4
_fatal_error proc near
    pop     ax
    pop     ax
fatal_copy_display:
    call    _sprite_copy_2_to_1
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
    db 0
_fatal_error endp
_polang proc far
     s = byte ptr 0
     r = byte ptr 2
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
    or      di, 8
    neg     dx
polar_normalize_z:
    or      cx, cx
    jge     short polar_normalize_x
    or      di, 4
    neg     cx
polar_normalize_x:
    cmp     dx, cx
    jl      short polar_swap_axes
    jz      short polar_dispatch_quadrant
    xchg    dx, cx
polar_select_ratio:
    or      di, 2
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
_polang endp
_sub_2EAD4 proc far
    cli
    mov     ax, _timer_elapsed_ticks_low
    mov     dx, _timer_elapsed_ticks_high
timer_snapshot_complete:
    sti
timer_snapshot_return:
    retf
_sub_2EAD4 endp
_set_add_value proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push    bp
deadline_frame_setup:
    mov     bp, sp
deadline_read_timer:
    call    _sub_2EAD4
deadline_add_low_word:
    add     ax, [bp+arg_0]
deadline_add_high_word:
    adc     dx, [bp+arg_2]
deadline_store_low_word:
    mov     input_deadline_tick_low, ax
deadline_store_high_word:
    mov     input_deadline_tick_high, dx
    pop     bp
    retf
deadline_wait_until_reached:
    call    _sub_2EAD4
    cmp     dx, input_deadline_tick_high
    jb      short deadline_wait_until_reached
    cmp     ax, input_deadline_tick_low
    jb      short deadline_wait_until_reached
    retf
_set_add_value endp
_poll_input_abort proc far
    call    _sub_2EAD4
    xor     cx, cx
    cmp     dx, input_deadline_tick_high
    jb      short deadline_reached_result
    cmp     ax, input_deadline_tick_low
    jb      short deadline_reached_result
    inc     cx
deadline_reached_result:
    mov     ax, cx
    retf
_poll_input_abort endp
_wait_for_input_delay proc far
    var_4 = word ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push    bp
    mov     bp, sp
    sub     sp, 4
    call    _sub_2EAD4
    add     ax, [bp+arg_0]
    adc     dx, [bp+arg_2]
    mov     [bp+var_4], ax
    mov     [bp+var_2], dx
delay_deadline_poll_loop:
    call    _sub_2EAD4
    cmp     dx, [bp+var_2]
    jb      short delay_deadline_poll_loop
    cmp     ax, [bp+var_4]
    jb      short delay_deadline_poll_loop
    mov     sp, bp
    pop     bp
    retf
_wait_for_input_delay endp
_draw_line_related_alt proc far
    var_4 = byte ptr -4
     s = byte ptr 0
     r = byte ptr 2
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     [bp+var_4], 1
    jmp     short line_init_output_record
_draw_line_related_alt endp
_draw_line_related proc far
    var_4 = byte ptr -4
    var_2 = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_startX = word ptr 6
    arg_startY = word ptr 8
    arg_endX = word ptr 10
    arg_endY = word ptr 12
    arg_8 = word ptr 14
    push    bp
    mov     bp, sp
    sub     sp, 4
    push    si
    push    di
    mov     [bp+var_4], 0
line_init_output_record:
    mov     si, [bp+arg_8]
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
    db 144
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
    mov     bx, word ptr cs:[_sprite1+16]
    mov     cx, word ptr cs:[_sprite1+18]
    cmp     ax, cx
    jl      short line_start_below_clip_bottom
    mov     dl, 8
    jmp     line_commit_clip_bounds
line_start_below_clip_bottom:
    cmp     ax, bx
    jge     short line_start_above_clip_top
    or      dh, 4
line_start_above_clip_top:
    mov     ax, [si+0Ah]
    cmp     ax, bx
    jge     short line_end_below_clip_bottom
    mov     dl, 4
    jmp     line_commit_clip_bounds
line_end_below_clip_bottom:
    cmp     ax, cx
    jl      short line_end_above_clip_top
    or      dl, 8
line_end_above_clip_top:
    mov     bx, word ptr cs:[_sprite1+26]
    mov     cx, word ptr cs:[_sprite1+28]
    mov     ax, [si+2]
    cmp     ax, bx
    jge     short line_start_left_of_clip
    or      dh, 2
line_start_left_of_clip:
    cmp     ax, cx
    jl      short line_start_right_of_clip
    or      dh, 1
line_start_right_of_clip:
    mov     ax, [si+8]
    cmp     ax, bx
    jge     short line_end_left_of_clip
    or      dl, 2
line_end_left_of_clip:
    cmp     ax, cx
    jl      short line_end_right_of_clip
    or      dl, 1
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
    db 144
line_classify_x_slope:
    jl      short line_negative_dy
    cmp     dx, cx
    jb      short line_y_slope_steep_down
    jz      short line_y_slope_equal
    mov     byte ptr [si+12h], 8
line_make_x_major:
    xchg    cx, dx
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
    db 144
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
    db 144
line_y_slope_steep_down:
    mov     byte ptr [si+12h], 6
    jmp     short line_slope_lookup_or_divide
line_y_slope_equal:
    mov     byte ptr [si+12h], 4
    jmp     short line_set_primary_length
    db 144
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
    mov     cx, word ptr cs:[_sprite1+16]
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
    mov     ax, word ptr cs:[_sprite1+16]
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
    mov     dx, word ptr cs:[_sprite1+18]
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
    mov     cx, word ptr cs:[_sprite1+26]
    mov     ax, [si+8]
    mov     [si+8], cx
    sub     cx, ax
    add     [si+16h], cx
    sub     [si+0Eh], cx
    sub     [si+0Ah], cx
    jmp     line_continue_right_clip
line_left_case_02_adjust_ymin:
    mov     ax, [si+2]
    mov     cx, word ptr cs:[_sprite1+26]
    mov     [si+2], cx
    sub     cx, ax
    add     [si+14h], cx
    add     [si+6], cx
    sub     [si+0Eh], cx
    jmp     line_continue_right_clip
line_left_case_03_slope_to_edge:
    mov     ax, [si]
    mov     dx, [si+2]
    mov     cx, word ptr cs:[_sprite1+26]
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
    mov     dx, word ptr cs:[_sprite1+26]
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
    mov     dx, word ptr cs:[_sprite1+26]
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
    mov     ax, word ptr cs:[_sprite1+26]
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
line_clip_initialize_default:
    mov     dx, 2
line_commit_clip_bounds:
    mov     [si+13h], dl
    mov     word ptr [si+0Eh], 0
    mov     al, [si+13h]
    test    al, 4
    jz      short line_commit_top_edge
    mov     bx, word ptr cs:[_sprite1+16]
    mov     [si+6], bx
    mov     word ptr [si+4], 0
    dec     bx
    mov     [si+0Ah], bx
    jmp     short line_finish_return
    db 144
line_commit_top_edge:
    test    al, 8
    jz      short line_commit_bottom_edge
    mov     bx, word ptr cs:[_sprite1+18]
    mov     [si+6], bx
    mov     word ptr [si+4], 0
    jmp     short line_finish_return
    db 144
line_commit_bottom_edge:
    mov     cx, [si+0Ah]
    cmp     cx, word ptr cs:[_sprite1+18]
    jl      short line_commit_left_edge
    mov     cx, word ptr cs:[_sprite1+18]
    dec     cx
line_commit_left_edge:
    mov     dx, [si+6]
    mov     bx, [si+4]
    add     bx, 8000h
    adc     dx, 0
    cmp     dx, word ptr cs:[_sprite1+16]
    jge     short line_commit_right_edge
    mov     dx, word ptr cs:[_sprite1+16]
line_commit_right_edge:
    mov     [si+6], dx
    mov     word ptr [si+4], 0
    sub     cx, dx
    dec     dx
    inc     cx
    mov     [si+0Ah], dx
    test    al, 2
    jz      short line_store_remainder_x
    add     [si+16h], cx
    jmp     short line_finish_return
    db 144
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
    mov     ax, word ptr cs:[_sprite1+28]
    dec     ax
    mov     [si+2], ax
    sub     cx, ax
    add     [si+6], cx
    sub     [si+0Eh], cx
    add     [si+18h], cx
    jmp     line_reject_clipped
line_right_case_02:
    mov     cx, word ptr cs:[_sprite1+28]
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
    sub     dx, word ptr cs:[_sprite1+28]
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
    mov     dx, word ptr cs:[_sprite1+28]
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
    mov     cx, word ptr cs:[_sprite1+28]
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
    mov     ax, word ptr cs:[_sprite1+28]
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
    test    [bp+var_2], 8
    jz      short line_recompute_y_outcodes
    jmp     line_clip_bottom_edge
line_recompute_y_outcodes:
    xor     dx, dx
    mov     ax, [si+2]
    cmp     ax, word ptr cs:[_sprite1+26]
    jge     short line_mark_start_below
    or      dh, 2
line_mark_start_below:
    mov     bx, [si]
    add     bx, 8000h
    adc     ax, 0
    cmp     ax, word ptr cs:[_sprite1+28]
    jl      short line_mark_start_above
    or      dh, 1
line_mark_start_above:
    mov     ax, [si+8]
    cmp     ax, word ptr cs:[_sprite1+26]
    jge     short line_mark_end_below
    or      dl, 2
line_mark_end_below:
    cmp     ax, word ptr cs:[_sprite1+28]
    jl      short line_mark_end_above
    or      dl, 1
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
    test    [bp+var_2], 1
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
    mov     bx, word ptr cs:[_sprite1+16]
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
    mov     bx, word ptr cs:[_sprite1+18]
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
    cmp     dx, word ptr cs:[_sprite1+26]
    jge     short line_horizontal_trim_right
    dec     word ptr [si+0Ah]
    mov     word ptr [si+16h], 1
    mov     al, 2
    jmp     short line_horizontal_return
line_horizontal_trim_right:
    cmp     cx, word ptr cs:[_sprite1+28]
    jl      short line_horizontal_clip_left
    dec     word ptr [si+0Ah]
    mov     word ptr [si+1Ah], 1
    mov     al, 1
    jmp     short line_horizontal_return
line_horizontal_clip_left:
    mov     ax, word ptr cs:[_sprite1+26]
    mov     bx, ax
    sub     ax, cx
    jle     short line_horizontal_clip_right
    mov     [si+2], bx
    sub     [si+0Eh], ax
line_horizontal_clip_right:
    mov     ax, dx
    mov     bx, word ptr cs:[_sprite1+28]
    dec     bx
    sub     ax, bx
    jle     short line_horizontal_finish_clipping
    sub     [si+0Eh], ax
    mov     [si+8], bx
line_horizontal_finish_clipping:
    jmp     line_reject_clipped
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
    sub     ax, word ptr cs:[_sprite1+16]
    jle     short line_fallback_advance_x
    cmp     ax, cx
    jle     short line_fallback_check_bottom_y
    mov     ax, cx
line_fallback_check_bottom_y:
    sub     bx, word ptr cs:[_sprite1+18]
    jle     short line_fallback_adjust_start_x
    sub     ax, bx
    jle     short line_fallback_advance_x
line_fallback_adjust_start_x:
    mov     bx, [si+2]
    cmp     bx, word ptr cs:[_sprite1+26]
    jge     short line_fallback_record_top_clip
    add     [si+14h], ax
    jmp     short line_fallback_advance_x
    db 144
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
    sub     ax, word ptr cs:[_sprite1+18]
    inc     ax
    jge     short line_fallback_retreat_x
    neg     ax
    cmp     ax, cx
    jle     short line_fallback_adjust_end_x
    mov     ax, cx
line_fallback_adjust_end_x:
    sub     bx, word ptr cs:[_sprite1+16]
    inc     bx
    jge     short line_fallback_check_end_x
    add     ax, bx
    jle     short line_fallback_retreat_x
line_fallback_check_end_x:
    mov     bx, [si+8]
    cmp     bx, word ptr cs:[_sprite1+26]
    jge     short line_fallback_mark_bottom_clip
    add     [si+16h], ax
    jmp     short line_fallback_retreat_x
    db 144
line_fallback_mark_bottom_clip:
    add     [si+1Ah], ax
line_fallback_retreat_x:
    sub     [si+8], dx
    jmp     short line_fallback_check_start_y
_draw_line_related endp
_parse_shape2d_helper proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_2]
    xor     dx, dx
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    add     ax, [bp+arg_0]
    adc     dx, 0
    pop     bp
    retf
_parse_shape2d_helper endp
_parse_shape2d_helper2 proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push    bp
    mov     bp, sp
    mov     bx, [bp+arg_2]
    mov     dx, [bp+arg_0]
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
saved_int24_vector_offset     dw 0
saved_int24_vector_segment     dw 0
criterr_handler_offset     dw 0
criterr_handler_segment     dw 0
_parse_shape2d_helper2 endp
_criterr_interrupt_handler proc far
     r = byte ptr 0
    push    bx
    push    dx
    push    cx
    push    si
    push    di
    push    ds
    push    es
    push    bp
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
    iret
_criterr_interrupt_handler endp
_set_criterr_handler proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push    bp
    mov     bp, sp
    push    ds
    push    cs
    mov     ax, offset _criterr_exithandler
    push    ax
    call far ptr _add_exit_handler
    add     sp, 4
    mov     ax, [bp+arg_0]
    mov     cs:criterr_handler_offset, ax
    mov     ax, [bp+arg_2]
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
    int     21h
    pop     ds
    pop     bp
    retf
_set_criterr_handler endp
_criterr_exithandler proc far
    push    ds
    mov     ax, cs:saved_int24_vector_segment
    sub     ax, cs:saved_int24_vector_offset
    jz      short criterr_restore_previous_vector
    mov     dx, cs:saved_int24_vector_offset
    mov     ds, cs:saved_int24_vector_segment
    mov     al, 24h
    mov     ah, 25h
    int     21h
criterr_restore_previous_vector:
    pop     ds
    retf
_criterr_exithandler endp
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
    jmp     short prerender_setup_shared_core
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_8]
    mov     [bp+var_A], 0
prerender_setup_shared_core:
    mov     ax, [bp+arg_2]
    mov     _line_pattern_bits, ax
    mov     ax, [bp+0Ah]
    mov     _prerender_auxiliary_arg, ax
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
    jmp     short prerender_setup_shared_core
_nopsub_2F424 endp
preRender_unk_mode_1 proc far
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
    mov     [bp+var_A], 1
    jmp     short prerender_setup_shared_core
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
line_interp_profile_03     dw 32768
    dw 21845
line_interp_profile_04     dw 43690
line_interp_p04_word_01     dw 16384
    dw 32768
line_interp_profile_05     dw 49152
    dw 13107
    dw 26214
    dw 39321
line_interp_profile_06     dw 52428
    dw 10922
    dw 21845
    dw 32768
    dw 43690
line_interp_profile_07     dw 54613
    dw 9362
    dw 18724
    dw 28086
    dw 37449
    dw 46811
line_interp_profile_08     dw 56173
    dw 8192
    dw 16384
    dw 24576
    dw 32768
    dw 40960
    dw 49152
line_interp_profile_09     dw 57344
    dw 7281
    dw 14563
    dw 21845
    dw 29127
    dw 36408
    dw 43690
    dw 50972
line_interp_profile_10     dw 58254
    dw 6553
    dw 13107
    dw 19660
    dw 26214
    dw 32768
    dw 39321
    dw 45875
    dw 52428
line_interp_profile_11     dw 58982
    dw 5957
    dw 11915
    dw 17873
    dw 23831
    dw 29789
    dw 35746
    dw 41704
    dw 47662
    dw 53620
line_interp_profile_12     dw 59578
    dw 5461
    dw 10922
    dw 16384
    dw 21845
    dw 27306
    dw 32768
    dw 38229
    dw 43690
    dw 49152
    dw 54613
line_interp_profile_13     dw 60074
    dw 5041
    dw 10082
    dw 15123
    dw 20164
    dw 25206
    dw 30247
    dw 35288
    dw 40329
    dw 45371
    dw 50412
    dw 55453
line_interp_profile_14     dw 60494
    dw 4681
    dw 9362
    dw 14043
    dw 18724
    dw 23405
    dw 28086
    dw 32768
    dw 37449
    dw 42130
    dw 46811
    dw 51492
    dw 56173
line_interp_profile_15     dw 60854
    dw 4369
    dw 8738
    dw 13107
    dw 17476
    dw 21845
    dw 26214
    dw 30583
    dw 34952
    dw 39321
    dw 43690
    dw 48059
    dw 52428
    dw 56797
line_interp_profile_16     dw 61166
    dw 4096
    dw 8192
    dw 12288
    dw 16384
    dw 20480
    dw 24576
    dw 28672
    dw 32768
    dw 36864
    dw 40960
    dw 45056
    dw 49152
    dw 53248
    dw 57344
line_interp_profile_17     dw 61440
    dw 3855
    dw 7710
    dw 11565
    dw 15420
    dw 19275
    dw 23130
    dw 26985
    dw 30840
    dw 34695
    dw 38550
    dw 42405
    dw 46260
    dw 50115
    dw 53970
    dw 57825
line_interp_profile_18     dw 61680
    dw 3640
    dw 7281
    dw 10922
    dw 14563
    dw 18204
    dw 21845
    dw 25486
    dw 29127
    dw 32768
    dw 36408
    dw 40049
    dw 43690
    dw 47331
    dw 50972
    dw 54613
    dw 58254
line_interp_profile_19     dw 61895
    dw 3449
    dw 6898
    dw 10347
    dw 13797
    dw 17246
    dw 20695
    dw 24144
    dw 27594
    dw 31043
    dw 34492
    dw 37941
    dw 41391
    dw 44840
    dw 48289
    dw 51738
    dw 55188
    dw 58637
line_interp_profile_20     dw 62086
    dw 3276
    dw 6553
    dw 9830
    dw 13107
    dw 16384
    dw 19660
    dw 22937
    dw 26214
    dw 29491
    dw 32768
    dw 36044
    dw 39321
    dw 42598
    dw 45875
    dw 49152
    dw 52428
    dw 55705
    dw 58982
line_interp_profile_21     dw 62259
    dw 3120
    dw 6241
    dw 9362
    dw 12483
    dw 15603
    dw 18724
    dw 21845
    dw 24966
    dw 28086
    dw 31207
    dw 34328
    dw 37449
    dw 40569
    dw 43690
    dw 46811
    dw 49932
    dw 53052
    dw 56173
    dw 59294
line_interp_profile_22     dw 62415
    dw 2978
    dw 5957
    dw 8936
    dw 11915
    dw 14894
    dw 17873
    dw 20852
    dw 23831
    dw 26810
    dw 29789
    dw 32768
    dw 35746
    dw 38725
    dw 41704
    dw 44683
    dw 47662
    dw 50641
    dw 53620
    dw 56599
    dw 59578
line_interp_profile_23     dw 62557
    dw 2849
    dw 5698
    dw 8548
    dw 11397
    dw 14246
    dw 17096
    dw 19945
    dw 22795
    dw 25644
    dw 28493
    dw 31343
    dw 34192
    dw 37042
    dw 39891
    dw 42740
    dw 45590
    dw 48439
    dw 51289
    dw 54138
    dw 56987
    dw 59837
line_interp_profile_24     dw 62686
    dw 2730
    dw 5461
    dw 8192
    dw 10922
    dw 13653
    dw 16384
    dw 19114
    dw 21845
    dw 24576
    dw 27306
    dw 30037
    dw 32768
    dw 35498
    dw 38229
    dw 40960
    dw 43690
    dw 46421
    dw 49152
    dw 51882
    dw 54613
    dw 57344
    dw 60074
line_interp_profile_25     dw 62805
    dw 2621
    dw 5242
    dw 7864
    dw 10485
    dw 13107
    dw 15728
    dw 18350
    dw 20971
    dw 23592
    dw 26214
    dw 28835
    dw 31457
    dw 34078
    dw 36700
    dw 39321
    dw 41943
    dw 44564
    dw 47185
    dw 49807
    dw 52428
    dw 55050
    dw 57671
    dw 60293
line_interp_profile_26     dw 62914
    dw 2520
    dw 5041
    dw 7561
    dw 10082
    dw 12603
    dw 15123
    dw 17644
    dw 20164
    dw 22685
    dw 25206
    dw 27726
    dw 30247
    dw 32768
    dw 35288
    dw 37809
    dw 40329
    dw 42850
    dw 45371
    dw 47891
    dw 50412
    dw 52932
    dw 55453
    dw 57974
    dw 60494
line_interp_profile_27     dw 63015
    dw 2427
    dw 4854
    dw 7281
    dw 9709
    dw 12136
    dw 14563
    dw 16990
    dw 19418
    dw 21845
    dw 24272
    dw 26699
    dw 29127
    dw 31554
    dw 33981
    dw 36408
    dw 38836
    dw 41263
    dw 43690
    dw 46117
    dw 48545
    dw 50972
    dw 53399
    dw 55826
    dw 58254
    dw 60681
line_interp_profile_28     dw 63108
    dw 2340
    dw 4681
    dw 7021
    dw 9362
    dw 11702
    dw 14043
    dw 16384
    dw 18724
    dw 21065
    dw 23405
    dw 25746
    dw 28086
    dw 30427
    dw 32768
    dw 35108
    dw 37449
    dw 39789
    dw 42130
line_interp_p28_word_19     dw 44470
    dw 46811
    dw 49152
    dw 51492
    dw 53833
    dw 56173
    dw 58514
    dw 60854
line_interp_profile_29     dw 63195
    dw 2259
    dw 4519
    dw 6779
    dw 9039
    dw 11299
    dw 13559
    dw 15819
    dw 18078
    dw 20338
    dw 22598
    dw 24858
    dw 27118
    dw 29378
    dw 31638
    dw 33897
    dw 36157
    dw 38417
    dw 40677
    dw 42937
    dw 45197
    dw 47457
    dw 49716
    dw 51976
    dw 54236
    dw 56496
    dw 58756
    dw 61016
line_slope_ratio_row_30     dw 63276
    dw 2184
    dw 4369
    dw 6553
    dw 8738
    dw 10922
    dw 13107
    dw 15291
    dw 17476
    dw 19660
    dw 21845
    dw 24029
    dw 26214
    dw 28398
    dw 30583
    dw 32768
    dw 34952
    dw 37137
    dw 39321
    dw 41506
    dw 43690
    dw 45875
    dw 48059
    dw 50244
    dw 52428
    dw 54613
    dw 56797
    dw 58982
    dw 61166
line_interp_profile_31     dw 63351
    dw 2114
    dw 4228
    dw 6342
    dw 8456
    dw 10570
    dw 12684
    dw 14798
    dw 16912
    dw 19026
    dw 21140
    dw 23254
    dw 25368
    dw 27482
    dw 29596
    dw 31710
    dw 33825
    dw 35939
    dw 38053
    dw 40167
    dw 42281
    dw 44395
    dw 46509
    dw 48623
    dw 50737
    dw 52851
    dw 54965
    dw 57079
    dw 59193
    dw 61307
line_interp_profile_32     dw 63421
    dw 2048
    dw 4096
    dw 6144
    dw 8192
    dw 10240
    dw 12288
    dw 14336
    dw 16384
    dw 18432
    dw 20480
    dw 22528
    dw 24576
    dw 26624
    dw 28672
    dw 30720
    dw 32768
    dw 34816
    dw 36864
    dw 38912
    dw 40960
    dw 43008
    dw 45056
    dw 47104
    dw 49152
    dw 51200
    dw 53248
    dw 55296
    dw 57344
    dw 59392
    dw 61440
line_interp_profile_33     dw 63488
    dw 1985
    dw 3971
    dw 5957
    dw 7943
    dw 9929
    dw 11915
    dw 13901
    dw 15887
    dw 17873
    dw 19859
    dw 21845
    dw 23831
    dw 25817
    dw 27803
    dw 29789
    dw 31775
    dw 33760
    dw 35746
    dw 37732
    dw 39718
    dw 41704
    dw 43690
    dw 45676
    dw 47662
    dw 49648
    dw 51634
    dw 53620
    dw 55606
    dw 57592
    dw 59578
    dw 61564
line_interp_profile_34     dw 63550
    dw 1927
    dw 3855
    dw 5782
    dw 7710
    dw 9637
    dw 11565
    dw 13492
    dw 15420
    dw 17347
    dw 19275
    dw 21202
    dw 23130
    dw 25057
    dw 26985
    dw 28912
    dw 30840
    dw 32768
    dw 34695
    dw 36623
    dw 38550
    dw 40478
    dw 42405
    dw 44333
    dw 46260
    dw 48188
    dw 50115
    dw 52043
    dw 53970
    dw 55898
    dw 57825
    dw 59753
    dw 61680
line_interp_profile_35     dw 63608
    dw 1872
    dw 3744
    dw 5617
    dw 7489
    dw 9362
    dw 11234
    dw 13107
    dw 14979
    dw 16852
    dw 18724
    dw 20597
    dw 22469
    dw 24341
    dw 26214
    dw 28086
    dw 29959
    dw 31831
    dw 33704
    dw 35576
    dw 37449
    dw 39321
    dw 41194
    dw 43066
    dw 44938
    dw 46811
    dw 48683
    dw 50556
    dw 52428
    dw 54301
    dw 56173
    dw 58046
    dw 59918
    dw 61791
line_interp_profile_36     dw 63663
    dw 1820
    dw 3640
    dw 5461
    dw 7281
    dw 9102
    dw 10922
    dw 12743
    dw 14563
    dw 16384
    dw 18204
    dw 20024
    dw 21845
    dw 23665
    dw 25486
    dw 27306
    dw 29127
    dw 30947
    dw 32768
    dw 34588
    dw 36408
    dw 38229
    dw 40049
    dw 41870
    dw 43690
    dw 45511
    dw 47331
    dw 49152
    dw 50972
    dw 52792
    dw 54613
    dw 56433
    dw 58254
    dw 60074
    dw 61895
line_interp_profile_37     dw 63715
    dw 1771
    dw 3542
    dw 5313
    dw 7084
    dw 8856
    dw 10627
    dw 12398
    dw 14169
    dw 15941
    dw 17712
    dw 19483
    dw 21254
    dw 23026
    dw 24797
    dw 26568
    dw 28339
    dw 30111
    dw 31882
    dw 33653
    dw 35424
    dw 37196
    dw 38967
    dw 40738
    dw 42509
    dw 44281
    dw 46052
    dw 47823
    dw 49594
    dw 51366
    dw 53137
    dw 54908
    dw 56679
    dw 58451
    dw 60222
    dw 61993
line_interp_profile_38     dw 63764
    dw 1724
    dw 3449
    dw 5173
    dw 6898
    dw 8623
    dw 10347
    dw 12072
    dw 13797
    dw 15521
    dw 17246
    dw 18970
    dw 20695
    dw 22420
    dw 24144
    dw 25869
    dw 27594
    dw 29318
    dw 31043
    dw 32768
    dw 34492
    dw 36217
    dw 37941
    dw 39666
    dw 41391
    dw 43115
    dw 44840
    dw 46565
    dw 48289
    dw 50014
    dw 51738
    dw 53463
    dw 55188
    dw 56912
    dw 58637
    dw 60362
    dw 62086
line_interp_profile_39     dw 63811
    dw 1680
    dw 3360
    dw 5041
    dw 6721
    dw 8402
    dw 10082
    dw 11762
    dw 13443
    dw 15123
    dw 16804
    dw 18484
    dw 20164
    dw 21845
    dw 23525
    dw 25206
    dw 26886
    dw 28566
    dw 30247
    dw 31927
    dw 33608
    dw 35288
    dw 36969
    dw 38649
    dw 40329
    dw 42010
    dw 43690
    dw 45371
    dw 47051
    dw 48731
    dw 50412
    dw 52092
    dw 53773
    dw 55453
    dw 57133
    dw 58814
    dw 60494
    dw 62175
line_interp_profile_40     dw 63855
    dw 1638
    dw 3276
    dw 4915
    dw 6553
    dw 8192
    dw 9830
    dw 11468
    dw 13107
    dw 14745
    dw 16384
    dw 18022
    dw 19660
    dw 21299
    dw 22937
    dw 24576
    dw 26214
    dw 27852
    dw 29491
    dw 31129
    dw 32768
    dw 34406
    dw 36044
    dw 37683
    dw 39321
    dw 40960
    dw 42598
    dw 44236
    dw 45875
    dw 47513
    dw 49152
    dw 50790
    dw 52428
    dw 54067
    dw 55705
    dw 57344
    dw 58982
    dw 60620
    dw 62259
line_interp_profile_41     dw 63897
    dw 1598
    dw 3196
    dw 4795
    dw 6393
    dw 7992
    dw 9590
    dw 11189
    dw 12787
    dw 14385
    dw 15984
    dw 17582
    dw 19181
    dw 20779
    dw 22378
    dw 23976
    dw 25575
    dw 27173
    dw 28771
    dw 30370
    dw 31968
    dw 33567
    dw 35165
    dw 36764
    dw 38362
    dw 39960
    dw 41559
    dw 43157
    dw 44756
    dw 46354
    dw 47953
    dw 49551
    dw 51150
    dw 52748
    dw 54346
    dw 55945
    dw 57543
    dw 59142
    dw 60740
    dw 62339
line_interp_profile_42     dw 63937
    dw 1560
    dw 3120
    dw 4681
    dw 6241
    dw 7801
    dw 9362
    dw 10922
    dw 12483
    dw 14043
    dw 15603
    dw 17164
    dw 18724
    dw 20284
    dw 21845
    dw 23405
    dw 24966
    dw 26526
    dw 28086
    dw 29647
    dw 31207
    dw 32768
    dw 34328
    dw 35888
    dw 37449
    dw 39009
    dw 40569
    dw 42130
    dw 43690
    dw 45251
    dw 46811
    dw 48371
    dw 49932
    dw 51492
    dw 53052
    dw 54613
    dw 56173
    dw 57734
    dw 59294
    dw 60854
    dw 62415
line_interp_profile_43     dw 63975
    dw 1524
    dw 3048
    dw 4572
    dw 6096
    dw 7620
    dw 9144
    dw 10668
    dw 12192
    dw 13716
    dw 15240
    dw 16765
    dw 18289
    dw 19813
    dw 21337
    dw 22861
    dw 24385
    dw 25909
    dw 27433
    dw 28957
    dw 30481
    dw 32005
    dw 33530
    dw 35054
    dw 36578
    dw 38102
    dw 39626
    dw 41150
    dw 42674
    dw 44198
    dw 45722
    dw 47246
    dw 48770
    dw 50295
    dw 51819
    dw 53343
    dw 54867
    dw 56391
    dw 57915
    dw 59439
    dw 60963
    dw 62487
line_interp_profile_44     dw 64011
    dw 1489
    dw 2978
    dw 4468
    dw 5957
    dw 7447
    dw 8936
    dw 10426
    dw 11915
    dw 13405
    dw 14894
    dw 16384
    dw 17873
    dw 19362
    dw 20852
    dw 22341
    dw 23831
    dw 25320
    dw 26810
    dw 28299
    dw 29789
    dw 31278
    dw 32768
    dw 34257
    dw 35746
    dw 37236
    dw 38725
    dw 40215
    dw 41704
    dw 43194
    dw 44683
    dw 46173
    dw 47662
    dw 49152
    dw 50641
    dw 52130
    dw 53620
    dw 55109
    dw 56599
    dw 58088
    dw 59578
    dw 61067
    dw 62557
line_interp_profile_45     dw 64046
    dw 1456
    dw 2912
    dw 4369
    dw 5825
    dw 7281
    dw 8738
    dw 10194
    dw 11650
    dw 13107
    dw 14563
    dw 16019
    dw 17476
    dw 18932
    dw 20388
    dw 21845
    dw 23301
    dw 24758
    dw 26214
    dw 27670
    dw 29127
    dw 30583
    dw 32039
    dw 33496
    dw 34952
    dw 36408
    dw 37865
    dw 39321
    dw 40777
    dw 42234
    dw 43690
    dw 45147
    dw 46603
    dw 48059
    dw 49516
    dw 50972
    dw 52428
    dw 53885
    dw 55341
    dw 56797
    dw 58254
    dw 59710
    dw 61166
    dw 62623
line_interp_profile_46     dw 64079
    dw 1424
    dw 2849
    dw 4274
    dw 5698
    dw 7123
    dw 8548
    dw 9972
    dw 11397
    dw 12822
    dw 14246
    dw 15671
    dw 17096
    dw 18521
    dw 19945
    dw 21370
    dw 22795
    dw 24219
    dw 25644
    dw 27069
    dw 28493
    dw 29918
    dw 31343
    dw 32768
    dw 34192
    dw 35617
    dw 37042
    dw 38466
    dw 39891
    dw 41316
    dw 42740
    dw 44165
    dw 45590
    dw 47014
    dw 48439
    dw 49864
    dw 51289
    dw 52713
    dw 54138
    dw 55563
    dw 56987
    dw 58412
    dw 59837
    dw 61261
    dw 62686
line_interp_profile_47     dw 64111
    dw 1394
    dw 2788
    dw 4183
    dw 5577
    dw 6971
    dw 8366
    dw 9760
    dw 11155
    dw 12549
    dw 13943
    dw 15338
    dw 16732
    dw 18126
    dw 19521
    dw 20915
    dw 22310
    dw 23704
    dw 25098
    dw 26493
    dw 27887
    dw 29282
    dw 30676
    dw 32070
    dw 33465
    dw 34859
    dw 36253
    dw 37648
    dw 39042
    dw 40437
    dw 41831
    dw 43225
    dw 44620
    dw 46014
    dw 47409
    dw 48803
    dw 50197
    dw 51592
    dw 52986
    dw 54380
    dw 55775
    dw 57169
    dw 58564
    dw 59958
    dw 61352
    dw 62747
line_interp_profile_48     dw 64141
    dw 1365
    dw 2730
    dw 4096
    dw 5461
    dw 6826
    dw 8192
    dw 9557
    dw 10922
    dw 12288
    dw 13653
    dw 15018
    dw 16384
    dw 17749
    dw 19114
    dw 20480
    dw 21845
    dw 23210
    dw 24576
    dw 25941
    dw 27306
    dw 28672
    dw 30037
    dw 31402
    dw 32768
    dw 34133
    dw 35498
    dw 36864
    dw 38229
    dw 39594
    dw 40960
    dw 42325
    dw 43690
    dw 45056
    dw 46421
    dw 47786
    dw 49152
    dw 50517
    dw 51882
    dw 53248
    dw 54613
    dw 55978
    dw 57344
    dw 58709
    dw 60074
    dw 61440
    dw 62805
line_interp_profile_49     dw 64170
    dw 1337
    dw 2674
    dw 4012
    dw 5349
    dw 6687
    dw 8024
    dw 9362
    dw 10699
    dw 12037
    dw 13374
    dw 14712
    dw 16049
    dw 17387
    dw 18724
    dw 20062
    dw 21399
    dw 22736
    dw 24074
    dw 25411
    dw 26749
    dw 28086
    dw 29424
    dw 30761
    dw 32099
    dw 33436
    dw 34774
    dw 36111
    dw 37449
    dw 38786
    dw 40124
    dw 41461
    dw 42799
    dw 44136
    dw 45473
    dw 46811
    dw 48148
    dw 49486
    dw 50823
    dw 52161
    dw 53498
    dw 54836
    dw 56173
    dw 57511
    dw 58848
    dw 60186
    dw 61523
    dw 62861
    dw 64198
preRender_unk_mode_1 endp
_preRender_line proc far
    var_1C = byte ptr -28
     s = byte ptr 0
     r = byte ptr 2
    arg_startX = word ptr 6
    arg_startY = word ptr 8
    arg_endX = word ptr 10
    arg_endY = word ptr 12
    arg_color = word ptr 14
    push    bp
    mov     bp, sp
    sub     sp, 1Ch
    mov     ax, [bp+arg_color]
    lea     bx, [bp+var_1C]
    mov     [bx+10h], ax
    push    bx
    push    [bp+arg_endY]
    push    [bp+arg_endX]
    push    [bp+arg_startY]
    push    [bp+arg_startX]
    call    _draw_line_related
    add     sp, 0Ah
    or      ax, ax
    jnz     short prerender_line_return
    lea     bx, [bp+var_1C]
    cmp     word ptr [bx+0Eh], 0
    jle     short prerender_line_return
    push    bx
    call    _putpixel_line1_maybe
    add     sp, 2
prerender_line_return:
    mov     sp, bp
    pop     bp
    retf
_preRender_line endp
_add_exit_handler proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    mov     cx, 0Ah
    mov     bx, offset DGROUP:_exitlistfuncs
    mov     dx, [bp+arg_2]
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
    mov     ax, offset DGROUP:_aExitListOverflow
    push    ax
    call    far ptr _fatal_error
exitlist_overflow_error:
    mov     word ptr [bx+2], 0
    mov     [bx], ax
    mov     word ptr [bx+6], 0
    mov     [bx+2], dx
exitlist_return:
    pop     bp
    retf
_add_exit_handler endp
_call_exitlist proc far
     r = byte ptr 0
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
_call_exitlist2 proc near
    call far ptr _call_exitlist
    xor     ax, ax
    push    ax
    call    far ptr _exit
    db 0
_call_exitlist2 endp
_file_paras proc far
    var_fatal = word ptr -6
    var_length = word ptr -4
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
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
    db 144
_file_paras endp
_file_paras_nofatal proc far
    var_6 = word ptr -6
     s = byte ptr 0
     r = byte ptr 2
    push    bp
    mov     bp, sp
    sub     sp, 6
    push    ds
    push    si
    push    di
    mov     [bp+var_6], 0
    jmp     short file_paras_body
    db 144
_file_paras_nofatal endp
_file_paras_fatal proc near
    var_fatal = word ptr -6
     s = byte ptr 0
     r = byte ptr 2
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
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
file_paras_body:
    mov     dx, [bp+arg_filename]
    mov     ah, 3Dh
    xor     al, al
    int     21h
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
    call    far ptr _fatal_error
file_paras_file_opened:
    mov     [bp+var_filehandle], ax
    mov     bx, ax
    sub     cx, cx
    sub     dx, dx
    mov     ax, 4202h
    int     21h
    push    dx
    push    ax
    sub     dx, dx
    mov     ax, 4200h
    int     21h
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
    int     21h
    mov     ax, [bp+var_length]
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
_file_paras_fatal endp
_file_decomp_paras proc far
    var_fatal = word ptr -8
    var_6 = byte ptr -6
    var_5 = word ptr -5
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
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
    db 144
_file_decomp_paras endp
_file_decomp_paras_nofatal proc far
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2
    push    bp
    mov     bp, sp
    sub     sp, 8
    push    ds
    push    si
    push    di
    mov     [bp+var_fatal], 0
    jmp     short file_decomp_paras_body
    db 144
_file_decomp_paras_nofatal endp
_file_decomp_paras_fatal proc near
    var_fatal = word ptr -8
     s = byte ptr 0
     r = byte ptr 2
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
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
file_decomp_paras_body:
    mov     dx, [bp+arg_filename]
    mov     dx, [bp+arg_filename]
    mov     ah, 3Dh
    xor     al, al
    int     21h
    jnb     short decomp_file_read_header
    jmp     short decomp_nonfatal_return
    db 144
    var_fatal = word ptr -8
    var_6 = byte ptr -6
    var_5 = word ptr -5
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
decomp_file_read_header:
    mov     [bp+var_filehandle], ax
    mov     bx, [bp+var_filehandle]
    lea     dx, [bp+var_6]
    mov     cx, 4
    mov     ah, 3Fh
    int     21h
    cmp     ax, cx
    jnz     short decomp_close_short_read
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h
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
    int     21h
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
    call    far ptr _fatal_error
_file_decomp_paras_fatal endp
_file_find proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, offset DGROUP:_dtatransbuffer
    mov     ah, 1Ah
    int     21h
    mov     dx, [bp+arg_0]
    mov     cl, 6
    mov     ah, 4Eh
    int     21h
    jb      short _file_find_err
    mov     di, offset DGROUP:_foundfile
    mov     _foundfileptr, di
    mov     si, [bp+arg_0]
    mov     cx, 57h
    mov     ax, ds
    mov     es, ax
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
_file_find endp
_file_find_next proc far
     s = byte ptr 0
     r = byte ptr 2
    push    bp
    mov     bp, sp
    push    ds
    push    si
    push    di
    mov     dx, offset DGROUP:_dtatransbuffer
    mov     ah, 1Ah
    int     21h
    mov     ah, 4Fh
    int     21h
    jb      short _file_find_err
    jmp     short _file_find_ok
    db 0
_file_find_next endp
_mulscl proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push    bp
    mov     bp, sp
    mov     ax, [bp+arg_0]
    imul    [bp+arg_2]
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    rcl     dx, 1
    shl     ax, 1
    adc     dx, 0
    mov     ax, dx
    pop     bp
    retf
_mulscl endp
_video_set_mode4 proc far
     r = byte ptr 0
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
    int     10h
    mov     al, 3
    mov     dx, 3BFh
    out     dx, al
    mov     al, 2
    mov     dx, 3B8h
    out     dx, al
    mov     cx, 0Ch
    mov     dx, 3B4h
    xor     bx, bx
video_mode4_write_register_loop:
    mov     al, bl
    out     dx, al
    inc     dx
    mov     al, video_mode4_register_values[bx]
    out     dx, al
    dec     dx
    inc     bx
    loop    video_mode4_write_register_loop
    cld
    mov     ax, 0B800h
    mov     es, ax
    xor     di, di
    mov     cx, 4000h
    xor     ax, ax
    rep stosw
    mov     al, 8Ah
    mov     dx, 3B8h
    out     dx, al
    pop     di
    retf
_video_set_mode4 endp
    db 0
S012_TEXT ends
end
