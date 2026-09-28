DSEG segment byte public 'STUNTSD'
extrn _word_3F87C:word
extrn _word_3F87E:word
extrn _word_4031E:word
extrn _word_40320:word
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
_word_3F1C2 dw 0
_word_3F1C4 dw 0
word_2F1C6 dw 0, 3, 0000Ch, 0000Fh, 00030h, 00033h, 0003Ch, 0003Fh
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
_word_2F3C6 dw 00028h
off_3F3C8 dw offset DGROUP:byte_3F418, offset DGROUP:byte_3F418, offset DGROUP:byte_3F419, offset DGROUP:byte_3F41B
    dw offset DGROUP:byte_3F41E, offset DGROUP:byte_3F422, offset DGROUP:byte_3F427, offset DGROUP:byte_3F42D
    dw offset DGROUP:byte_3F434, offset DGROUP:byte_3F43C, offset DGROUP:byte_3F445, offset DGROUP:byte_3F44F
    dw offset DGROUP:byte_3F45A, offset DGROUP:byte_3F466, offset DGROUP:byte_3F473, offset DGROUP:byte_3F481
    dw offset DGROUP:byte_3F490, offset DGROUP:byte_3F4A0, offset DGROUP:byte_3F4B1, offset DGROUP:byte_3F4C3
    dw offset DGROUP:byte_3F4D6, offset DGROUP:byte_3F4EA, offset DGROUP:byte_3F4FF, offset DGROUP:byte_3F515
    dw offset DGROUP:byte_3F52C, offset DGROUP:byte_3F544, offset DGROUP:byte_3F55D, offset DGROUP:byte_3F577
    dw offset DGROUP:byte_3F592, offset DGROUP:byte_3F5AE, offset DGROUP:byte_3F5CB, offset DGROUP:byte_3F5E9
    dw offset DGROUP:byte_3F608, offset DGROUP:byte_3F628, offset DGROUP:byte_3F649, offset DGROUP:byte_3F66B
    dw offset DGROUP:byte_3F68E, offset DGROUP:byte_3F6B2, offset DGROUP:byte_3F6D7, offset DGROUP:byte_3F6FD
byte_3F418 db 1
byte_3F419 db 2, 3
byte_3F41B db 3, 4, 4
byte_3F41E db 3, 4, 5, 5
byte_3F422 db 4, 5, 6, 6, 6
byte_3F427 db 4, 6, 6, 7, 7, 8
byte_3F42D db 5, 6, 7, 8, 8, 9, 9
byte_3F434 db 5, 7, 8, 9, 9, 00Ah, 00Ah, 00Ah
byte_3F43C db 5, 7, 8, 9, 00Ah, 00Bh, 00Bh, 00Bh, 00Bh
byte_3F445 db 5, 8, 9, 00Ah, 00Bh, 00Bh, 00Ch, 00Ch, 00Ch, 00Dh
byte_3F44F db 6, 8, 9, 00Bh, 00Ch, 00Ch, 00Dh, 00Dh, 00Eh, 00Eh, 00Eh
byte_3F45A db 6, 8, 00Ah, 00Bh, 00Ch, 00Dh, 00Eh, 00Eh, 00Fh, 00Fh, 00Fh, 00Fh
byte_3F466 db 6, 9, 00Ah, 00Ch, 00Dh, 00Eh, 00Eh, 00Fh, 00Fh, 010h, 010h, 010h, 010h
byte_3F473 db 6, 9, 00Bh, 00Ch, 00Dh, 00Eh, 00Fh, 010h, 010h, 011h, 011h, 011h, 011h, 012h
byte_3F481 db 7, 9, 00Bh, 00Dh, 00Eh, 00Fh, 010h, 011h, 011h, 012h, 012h, 012h, 013h, 013h, 013h
byte_3F490 db 7, 00Ah, 00Ch, 00Dh, 00Fh, 010h, 011h, 011h, 012h, 013h, 013h, 013h, 014h, 014h, 014h, 014h
byte_3F4A0 db 7, 00Ah, 00Ch, 00Eh, 00Fh, 010h, 011h, 012h, 013h, 013h, 014h, 014h, 015h, 015h, 015h, 015h
    db 015h
byte_3F4B1 db 7, 00Ah, 00Ch, 00Eh, 010h, 011h, 012h, 013h, 013h, 014h, 015h, 015h, 016h, 016h, 016h, 016h
    db 016h, 017h
byte_3F4C3 db 8, 00Bh, 00Dh, 00Fh, 010h, 011h, 012h, 013h, 014h, 015h, 016h, 016h, 017h, 017h, 017h, 017h
    db 018h, 018h, 018h
byte_3F4D6 db 8, 00Bh, 00Dh, 00Fh, 011h, 012h, 013h, 014h, 015h, 016h, 016h, 017h, 017h, 018h, 018h, 018h
    db 019h, 019h, 019h, 019h
byte_3F4EA db 8, 00Bh, 00Eh, 00Fh, 011h, 012h, 014h, 015h, 016h, 016h, 017h, 018h, 018h, 019h, 019h, 019h
    db 01Ah, 01Ah, 01Ah, 01Ah, 01Ah
byte_3F4FF db 8, 00Bh, 00Eh, 010h, 011h, 013h, 014h, 015h, 016h, 017h, 018h, 018h, 019h, 01Ah, 01Ah, 01Ah
    db 01Bh, 01Bh, 01Bh, 01Bh, 01Bh, 01Ch
byte_3F515 db 8, 00Ch, 00Eh, 010h, 012h, 013h, 015h, 016h, 017h, 018h, 019h, 019h, 01Ah, 01Ah, 01Bh, 01Bh
    db 01Ch, 01Ch, 01Ch, 01Dh, 01Dh, 01Dh, 01Dh
byte_3F52C db 9, 00Ch, 00Fh, 011h, 012h, 014h, 015h, 016h, 017h, 018h, 019h, 01Ah, 01Bh, 01Bh, 01Ch, 01Ch
    db 01Dh, 01Dh, 01Dh, 01Eh, 01Eh, 01Eh, 01Eh, 01Eh
byte_3F544 db 9, 00Ch, 00Fh, 011h, 013h, 014h, 016h, 017h, 018h, 019h, 01Ah, 01Bh, 01Bh, 01Ch, 01Dh, 01Dh
    db 01Eh, 01Eh, 01Eh, 01Fh, 01Fh, 01Fh, 01Fh, 01Fh, 01Fh
byte_3F55D db 9, 00Dh, 00Fh, 011h, 013h, 015h, 016h, 017h, 019h, 01Ah, 01Bh, 01Bh, 01Ch, 01Dh, 01Dh, 01Eh
    db 01Eh, 01Fh, 01Fh, 020h, 020h, 020h, 020h, 020h, 020h, 021h
byte_3F577 db 9, 00Dh, 00Fh, 012h, 014h, 015h, 017h, 018h, 019h, 01Ah, 01Bh, 01Ch, 01Dh, 01Eh, 01Eh, 01Fh
    db 01Fh, 020h, 020h, 021h, 021h, 021h, 021h, 022h, 022h, 022h, 022h
byte_3F592 db 9, 00Dh, 010h, 012h, 014h, 016h, 017h, 018h, 01Ah, 01Bh, 01Ch, 01Dh, 01Eh, 01Eh, 01Fh, 020h
    db 020h, 021h, 021h, 022h, 022h, 022h, 022h, 023h, 023h, 023h, 023h, 023h
byte_3F5AE db 9, 00Dh, 010h, 012h, 014h, 016h, 018h, 019h, 01Ah, 01Bh, 01Ch, 01Dh, 01Eh, 01Fh, 020h, 020h
    db 021h, 022h, 022h, 022h, 023h, 023h, 023h, 024h, 024h, 024h, 024h, 024h, 024h
byte_3F5CB db 00Ah, 00Dh, 010h, 013h, 015h, 017h, 018h, 019h, 01Bh, 01Ch, 01Dh, 01Eh, 01Fh, 020h, 020h, 021h
    db 022h, 022h, 023h, 023h, 024h, 024h, 024h, 025h, 025h, 025h, 025h, 025h, 025h, 026h
byte_3F5E9 db 00Ah, 00Eh, 011h, 013h, 015h, 017h, 019h, 01Ah, 01Bh, 01Dh, 01Eh, 01Fh, 020h, 020h, 021h, 022h
    db 023h, 023h, 024h, 024h, 025h, 025h, 025h, 026h, 026h, 026h, 026h, 027h, 027h, 027h, 027h
byte_3F608 db 00Ah, 00Eh, 011h, 013h, 015h, 017h, 019h, 01Ah, 01Ch, 01Dh, 01Eh, 01Fh, 020h, 021h, 022h, 023h
    db 023h, 024h, 025h, 025h, 026h, 026h, 026h, 027h, 027h, 027h, 028h, 028h, 028h, 028h, 028h, 028h
byte_3F628 db 00Ah, 00Eh, 011h, 014h, 016h, 018h, 019h, 01Bh, 01Ch, 01Eh, 01Fh, 020h, 021h, 022h, 023h, 023h
    db 024h, 025h, 025h, 026h, 026h, 027h, 027h, 028h, 028h, 028h, 029h, 029h, 029h, 029h, 029h, 029h
    db 029h
byte_3F649 db 00Ah, 00Eh, 011h, 014h, 016h, 018h, 01Ah, 01Bh, 01Dh, 01Eh, 01Fh, 020h, 021h, 022h, 023h, 024h
    db 025h, 026h, 026h, 027h, 027h, 028h, 028h, 029h, 029h, 029h, 02Ah, 02Ah, 02Ah, 02Ah, 02Ah, 02Ah
    db 02Ah, 02Bh
byte_3F66B db 00Ah, 00Fh, 012h, 014h, 017h, 018h, 01Ah, 01Ch, 01Dh, 01Fh, 020h, 021h, 022h, 023h, 024h, 025h
    db 026h, 026h, 027h, 028h, 028h, 029h, 029h, 02Ah, 02Ah, 02Ah, 02Bh, 02Bh, 02Bh, 02Bh, 02Bh, 02Ch
    db 02Ch, 02Ch, 02Ch
byte_3F68E db 00Bh, 00Fh, 012h, 015h, 017h, 019h, 01Bh, 01Ch, 01Eh, 01Fh, 020h, 022h, 023h, 024h, 025h, 025h
    db 026h, 027h, 028h, 028h, 029h, 029h, 02Ah, 02Ah, 02Bh, 02Bh, 02Ch, 02Ch, 02Ch, 02Ch, 02Dh, 02Dh
    db 02Dh, 02Dh, 02Dh, 02Dh
byte_3F6B2 db 00Bh, 00Fh, 012h, 015h, 017h, 019h, 01Bh, 01Dh, 01Eh, 020h, 021h, 022h, 023h, 024h, 025h, 026h
    db 027h, 028h, 028h, 029h, 02Ah, 02Ah, 02Bh, 02Bh, 02Ch, 02Ch, 02Dh, 02Dh, 02Dh, 02Dh, 02Eh, 02Eh
    db 02Eh, 02Eh, 02Eh, 02Eh, 02Eh
byte_3F6D7 db 00Bh, 00Fh, 012h, 015h, 018h, 01Ah, 01Bh, 01Dh, 01Fh, 020h, 021h, 023h, 024h, 025h, 026h, 027h
    db 028h, 028h, 029h, 02Ah, 02Ah, 02Bh, 02Ch, 02Ch, 02Dh, 02Dh, 02Dh, 02Eh, 02Eh, 02Eh, 02Fh, 02Fh
    db 02Fh, 02Fh, 02Fh, 02Fh, 02Fh, 030h
byte_3F6FD db 00Bh, 00Fh, 013h, 016h, 018h, 01Ah, 01Ch, 01Eh, 01Fh, 021h, 022h, 023h, 024h, 025h, 026h, 027h
    db 028h, 029h, 02Ah, 02Bh, 02Bh, 02Ch, 02Ch, 02Dh, 02Eh, 02Eh, 02Eh, 02Fh, 02Fh, 02Fh, 030h, 030h
    db 030h, 030h, 030h, 031h, 031h, 031h, 031h
_exitlistfuncs db 44 dup (0)
_aExitListOverflow db 'EXIT LIST OVERFLOW', 0Dh, 0Ah, 0, 0
_aSFileError_1 db '%s FILE ERROR', 0
_foundfile db 100 dup (0)
_foundfileptr dw 0
_dtatransbuffer db 30 dup (0)
_foundfilepath db 98 dup (0)
_byte_3F85A db 2 dup (0)
_byte_3F85C db 035h, 028h, 02Ch, 7, 079h, 2, 064h, 06Eh, 2, 2, 0, 0
_DATA ends
DGROUP group _DATA,DSEG
S012_TEXT segment byte public 'CODE'
public _word_2F3C6
public _byte_3F85A
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
loc_2EA2C:
    call    _sprite_copy_2_to_1
loc_2EA31:
    call    _printf
loc_2EA36:
    call    _flush_stdin
loc_2EA3B:
    call far ptr _call_exitlist
loc_2EA40:
    call    _printf
loc_2EA45:
    add     sp, 2
loc_2EA48:
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
    jge     short loc_2EA63
    or      di, 8
    neg     dx
loc_2EA63:
    or      cx, cx
    jge     short loc_2EA6C
    or      di, 4
    neg     cx
loc_2EA6C:
    cmp     dx, cx
    jl      short loc_2EA77
    jz      short loc_2EA8F
    xchg    dx, cx
loc_2EA74:
    or      di, 2
loc_2EA77:
    xor     ax, ax
    div     cx
loc_2EA7B:
    mov     bl, ah
    xor     bh, bh
    add     al, 80h
    adc     bx, 0
loc_2EA84:
    mov     al, _atantable[bx]
    xor     ah, ah
loc_2EA8A:
    jmp     cs:off_2EA9B[di]
loc_2EA8F:
    or      dx, dx
    jz      short loc_2EAB0
    mov     ax, 80h
    jmp     cs:off_2EA9B[di]
off_2EA9B     dw offset loc_2EAB0
    dw offset loc_2EAAB
    dw offset loc_2EAB8
    dw offset loc_2EAB3
    dw offset loc_2EABF
    dw offset loc_2EAC3
    dw offset loc_2EACF
    dw offset loc_2EAC8
loc_2EAAB:
    neg     ax
    add     ah, 1
loc_2EAB0:
    pop     di
    pop     bp
    retf
loc_2EAB3:
    add     ah, 1
    jmp     short loc_2EAB0
loc_2EAB8:
    neg     ax
    add     ah, 2
    jmp     short loc_2EAB0
loc_2EABF:
    neg     ax
    jmp     short loc_2EAB0
loc_2EAC3:
    sub     ah, 1
    jmp     short loc_2EAB0
loc_2EAC8:
    add     ah, 1
    neg     ax
    jmp     short loc_2EAB0
loc_2EACF:
    sub     ah, 2
    jmp     short loc_2EAB0
_polang endp
_sub_2EAD4 proc far
    cli
    mov     ax, _word_3F87C
    mov     dx, _word_3F87E
loc_2EADC:
    sti
locret_2EADD:
    retf
_sub_2EAD4 endp
_set_add_value proc far
     s = byte ptr 0
     r = byte ptr 2
    arg_0 = word ptr 6
    arg_2 = word ptr 8
    push    bp
loc_2EADF:
    mov     bp, sp
loc_2EAE1:
    call    _sub_2EAD4
loc_2EAE6:
    add     ax, [bp+arg_0]
loc_2EAE9:
    adc     dx, [bp+arg_2]
loc_2EAEC:
    mov     _word_3F1C2, ax
loc_2EAEF:
    mov     _word_3F1C4, dx
    pop     bp
    retf
loc_2EAF5:
    call    _sub_2EAD4
    cmp     dx, _word_3F1C4
    jb      short loc_2EAF5
    cmp     ax, _word_3F1C2
    jb      short loc_2EAF5
    retf
_set_add_value endp
_poll_input_abort proc far
    call    _sub_2EAD4
    xor     cx, cx
    cmp     dx, _word_3F1C4
    jb      short loc_2EB1B
    cmp     ax, _word_3F1C2
    jb      short loc_2EB1B
    inc     cx
loc_2EB1B:
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
loc_2EB35:
    call    _sub_2EAD4
    cmp     dx, [bp+var_2]
    jb      short loc_2EB35
    cmp     ax, [bp+var_4]
    jb      short loc_2EB35
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
    jmp     short loc_2EB62
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
loc_2EB62:
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
    jg      short loc_2EB9C
    mov     [si+2], cx
    mov     [si+6], ax
    mov     [si+8], dx
loc_2EB96:
    mov     [si+0Ah], bx
    jmp     short loc_2EBA8
    db 144
loc_2EB9C:
    mov     [si+2], dx
    mov     [si+6], bx
    mov     [si+8], cx
    mov     [si+0Ah], ax
loc_2EBA8:
    jnz     short loc_2EBAD
    jmp     loc_2F1A3
loc_2EBAD:
    xor     dx, dx
    cmp     [bp+var_4], 0
    jnz     short loc_2EC1A
    mov     ax, [si+6]
    mov     bx, word ptr cs:[_sprite1+16]
    mov     cx, word ptr cs:[_sprite1+18]
    cmp     ax, cx
    jl      short loc_2EBCB
    mov     dl, 8
    jmp     loc_2EF9B
loc_2EBCB:
    cmp     ax, bx
    jge     short loc_2EBD2
    or      dh, 4
loc_2EBD2:
    mov     ax, [si+0Ah]
    cmp     ax, bx
    jge     short loc_2EBDE
    mov     dl, 4
    jmp     loc_2EF9B
loc_2EBDE:
    cmp     ax, cx
    jl      short loc_2EBE5
    or      dl, 8
loc_2EBE5:
    mov     bx, word ptr cs:[_sprite1+26]
    mov     cx, word ptr cs:[_sprite1+28]
    mov     ax, [si+2]
    cmp     ax, bx
    jge     short loc_2EBF9
    or      dh, 2
loc_2EBF9:
    cmp     ax, cx
    jl      short loc_2EC00
    or      dh, 1
loc_2EC00:
    mov     ax, [si+8]
    cmp     ax, bx
    jge     short loc_2EC0A
    or      dl, 2
loc_2EC0A:
    cmp     ax, cx
    jl      short loc_2EC11
    or      dl, 1
loc_2EC11:
    test    dl, dh
    jz      short loc_2EC1A
    and     dl, dh
    jmp     loc_2EF9B
loc_2EC1A:
    or      dl, dh
    xor     dh, dh
    mov     [bp+var_2], dx
    mov     cx, [si+0Ah]
    sub     cx, [si+6]
    jno     short loc_2EC2C
loc_2EC29:
    jmp     loc_2F253
loc_2EC2C:
    mov     dx, [si+8]
    sub     dx, [si+2]
    jo      short loc_2EC29
    jnz     short loc_2EC41
    inc     cx
    mov     [si+0Eh], cx
    mov     byte ptr [si+12h], 2
    jmp     short loc_2ECAD
    db 144
loc_2EC41:
    jl      short loc_2EC8F
    cmp     dx, cx
    jb      short loc_2EC82
    jz      short loc_2EC88
    mov     byte ptr [si+12h], 8
loc_2EC4D:
    xchg    cx, dx
loc_2EC4F:
    cmp     cx, cs:word_2F448
    jge     short loc_2EC69
    mov     bx, cx
    shl     bx, 1
    mov     bx, cs:off_2F44A[bx]
    add     bx, dx
    add     bx, dx
    mov     ax, cs:[bx]
    jmp     short loc_2EC76
    db 144
loc_2EC69:
    xor     ax, ax
    div     cx
    mov     bx, cx
    shr     bx, 1
    sub     bx, dx
    adc     ax, 0
loc_2EC76:
    mov     [si+0Ch], ax
    inc     cx
    jo      short loc_2EC29
    mov     [si+0Eh], cx
    jmp     short loc_2ECAD
    db 144
loc_2EC82:
    mov     byte ptr [si+12h], 6
    jmp     short loc_2EC4F
loc_2EC88:
    mov     byte ptr [si+12h], 4
    jmp     short loc_2ECA9
    db 144
loc_2EC8F:
    neg     dx
    jo      short loc_2EC29
    cmp     dx, cx
    jb      short loc_2EC9F
    jz      short loc_2ECA5
    mov     byte ptr [si+12h], 7
    jmp     short loc_2EC4D
loc_2EC9F:
    mov     byte ptr [si+12h], 5
    jmp     short loc_2EC4F
loc_2ECA5:
    mov     byte ptr [si+12h], 3
loc_2ECA9:
    inc     cx
    mov     [si+0Eh], cx
loc_2ECAD:
    mov     bx, [bp+var_2]
    shl     bx, 1
    jz      short loc_2ECD9
    jmp     cs:off_2ECB9[bx]
off_2ECB9     dw offset loc_2ECD9
    dw offset loc_2F01F
    dw offset loc_2EE78
    dw offset loc_2EE78
    dw offset loc_2ECE1
    dw offset loc_2ECE1
    dw offset loc_2ECE1
    dw offset loc_2ECE1
    dw offset loc_2EDA5
    dw offset loc_2EDA5
    dw offset loc_2EDA5
    dw offset loc_2EDA5
    dw offset loc_2ECE1
    dw offset loc_2ECE1
    dw offset loc_2ECE1
    dw offset loc_2ECE1
loc_2ECD9:
    xor     ax, ax
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_2ECE1:
    mov     ax, [si+6]
    mov     cx, word ptr cs:[_sprite1+16]
    mov     [si+6], cx
    sub     cx, ax
    mov     bl, [si+12h]
    shl     bx, 1
    jmp     cs:word_2ECF8[bx]
word_2ECF8     dw 0
    dw 0
    dw offset loc_2ED0A
    dw offset loc_2ED10
    dw offset loc_2ED19
    dw offset loc_2ED22
    dw offset loc_2ED32
    dw offset loc_2ED42
    dw offset loc_2ED7E
loc_2ED0A:
    sub     [si+0Eh], cx
    jmp     loc_2F13E
loc_2ED10:
    sub     [si+2], cx
    sub     [si+0Eh], cx
    jmp     loc_2F13E
loc_2ED19:
    add     [si+2], cx
    sub     [si+0Eh], cx
    jmp     loc_2F13E
loc_2ED22:
    mov     ax, [si+0Ch]
    mul     cx
    sub     [si], ax
    sbb     [si+2], dx
    sub     [si+0Eh], cx
    jmp     loc_2F13E
loc_2ED32:
    mov     ax, [si+0Ch]
    mul     cx
    add     [si], ax
    adc     [si+2], dx
    sub     [si+0Eh], cx
    jmp     loc_2F13E
loc_2ED42:
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
    jle     short loc_2ED69
    mul     word ptr [si+0Ch]
    add     [si+4], ax
    adc     [si+6], dx
    jmp     loc_2F13E
loc_2ED69:
    mov     word ptr [si+0Eh], 1
    mov     ax, word ptr cs:[_sprite1+16]
    mov     [si+6], ax
    mov     ax, [si+8]
    mov     [si+2], ax
    jmp     loc_2F13E
loc_2ED7E:
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
    jle     short loc_2ED69
    mul     word ptr [si+0Ch]
    add     [si+4], ax
    adc     [si+6], dx
    jmp     loc_2F13E
loc_2EDA5:
    mov     cx, [si+0Ah]
    mov     dx, word ptr cs:[_sprite1+18]
    dec     dx
    mov     [si+0Ah], dx
    sub     cx, dx
    mov     bl, [si+12h]
    shl     bx, 1
    jmp     cs:word_2EDBD[bx]
word_2EDBD     dw 0
    dw 0
    dw offset loc_2EDCF
    dw offset loc_2EDD5
    dw offset loc_2EDDE
    dw offset loc_2EDE7
    dw offset loc_2EE0B
    dw offset loc_2EE2A
    dw offset loc_2EE53
loc_2EDCF:
    sub     [si+0Eh], cx
    jmp     loc_2F148
loc_2EDD5:
    add     [si+8], cx
    sub     [si+0Eh], cx
    jmp     loc_2F148
loc_2EDDE:
    sub     [si+8], cx
    sub     [si+0Eh], cx
    jmp     loc_2F148
loc_2EDE7:
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
    jmp     loc_2F148
loc_2EE0B:
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
    jmp     loc_2F148
loc_2EE2A:
    xor     ax, ax
    sub     ax, [si+4]
    sbb     dx, [si+6]
    jl      short loc_2EE4F
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
loc_2EE40:
    mov     dx, [si+2]
    sub     dx, ax
    mov     [si+8], dx
    inc     ax
    mov     [si+0Eh], ax
    jmp     loc_2F148
loc_2EE4F:
    xor     ax, ax
    jmp     short loc_2EE40
loc_2EE53:
    xor     ax, ax
    sub     ax, [si+4]
    sbb     dx, [si+6]
    jl      short loc_2EE4F
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
    jmp     loc_2F148
loc_2EE78:
    mov     bl, [si+12h]
    shl     bx, 1
    jmp     cs:word_2EE82[bx]
word_2EE82     dw 0
    dw 0
    dw offset loc_2EF98
    dw offset loc_2EE94
    dw offset loc_2EEAD
    dw offset loc_2EEC6
    dw offset loc_2EEFE
    dw offset loc_2EF31
    dw offset loc_2EF61
loc_2EE94:
    mov     cx, word ptr cs:[_sprite1+26]
    mov     ax, [si+8]
    mov     [si+8], cx
    sub     cx, ax
    add     [si+16h], cx
    sub     [si+0Eh], cx
    sub     [si+0Ah], cx
    jmp     loc_2F196
loc_2EEAD:
    mov     ax, [si+2]
    mov     cx, word ptr cs:[_sprite1+26]
    mov     [si+2], cx
    sub     cx, ax
    add     [si+14h], cx
    add     [si+6], cx
    sub     [si+0Eh], cx
    jmp     loc_2F196
loc_2EEC6:
    mov     ax, [si]
    mov     dx, [si+2]
    mov     cx, word ptr cs:[_sprite1+26]
    mov     [si+8], cx
    sub     dx, cx
    jl      short loc_2EEF9
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    inc     ax
loc_2EEE4:
    mov     cx, [si+0Eh]
    mov     [si+0Eh], ax
    sub     cx, ax
    add     [si+16h], cx
    dec     ax
    add     ax, [si+6]
    mov     [si+0Ah], ax
    jmp     loc_2F196
loc_2EEF9:
    mov     ax, 1
    jmp     short loc_2EEE4
loc_2EEFE:
    mov     dx, word ptr cs:[_sprite1+26]
    xor     ax, ax
    sub     ax, [si]
    sbb     dx, [si+2]
    jl      short loc_2EF2E
    mov     cx, [si+0Ch]
    div     cx
    shr     cx, 1
    sub     cx, dx
    adc     ax, 0
    add     [si+6], ax
    add     [si+14h], ax
    sub     [si+0Eh], ax
    jle     short loc_2EF98
    mul     word ptr [si+0Ch]
    add     [si], ax
    adc     [si+2], dx
    jmp     loc_2F196
loc_2EF2E:
    jmp     short loc_2EF98
    nop
loc_2EF31:
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
    jmp     loc_2F196
loc_2EF61:
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
    jmp     loc_2F196
loc_2EF98:
    mov     dx, 2
loc_2EF9B:
    mov     [si+13h], dl
    mov     word ptr [si+0Eh], 0
    mov     al, [si+13h]
    test    al, 4
    jz      short loc_2EFBE
    mov     bx, word ptr cs:[_sprite1+16]
    mov     [si+6], bx
    mov     word ptr [si+4], 0
    dec     bx
    mov     [si+0Ah], bx
    jmp     short loc_2F017
    db 144
loc_2EFBE:
    test    al, 8
    jz      short loc_2EFD2
    mov     bx, word ptr cs:[_sprite1+18]
    mov     [si+6], bx
    mov     word ptr [si+4], 0
    jmp     short loc_2F017
    db 144
loc_2EFD2:
    mov     cx, [si+0Ah]
    cmp     cx, word ptr cs:[_sprite1+18]
    jl      short loc_2EFE2
    mov     cx, word ptr cs:[_sprite1+18]
    dec     cx
loc_2EFE2:
    mov     dx, [si+6]
    mov     bx, [si+4]
    add     bx, 8000h
    adc     dx, 0
    cmp     dx, word ptr cs:[_sprite1+16]
    jge     short loc_2EFFB
    mov     dx, word ptr cs:[_sprite1+16]
loc_2EFFB:
    mov     [si+6], dx
    mov     word ptr [si+4], 0
    sub     cx, dx
    dec     dx
    inc     cx
    mov     [si+0Ah], dx
    test    al, 2
    jz      short loc_2F014
    add     [si+16h], cx
    jmp     short loc_2F017
    db 144
loc_2F014:
    add     [si+1Ah], cx
loc_2F017:
    xor     ah, ah
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_2F01F:
    mov     bl, [si+12h]
loc_2F022:
    xor     bh, bh
    shl     bx, 1
    jmp     cs:word_2F02B[bx]
word_2F02B     dw 0
    dw 0
    dw offset loc_2F03D
    dw offset loc_2F043
    dw offset loc_2F05C
    dw offset loc_2F076
    dw offset loc_2F0A3
    dw offset loc_2F0D7
    dw offset loc_2F110
loc_2F03D:
    mov     dx, 1
    jmp     loc_2EF9B
loc_2F043:
    mov     cx, [si+2]
    mov     ax, word ptr cs:[_sprite1+28]
    dec     ax
    mov     [si+2], ax
    sub     cx, ax
    add     [si+6], cx
    sub     [si+0Eh], cx
    add     [si+18h], cx
    jmp     loc_2ECD9
loc_2F05C:
    mov     cx, word ptr cs:[_sprite1+28]
    dec     cx
    mov     ax, [si+8]
    mov     [si+8], cx
    sub     ax, cx
    add     [si+1Ah], ax
    sub     [si+0Eh], ax
    sub     [si+0Ah], ax
    jmp     loc_2ECD9
loc_2F076:
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
    jle     short loc_2F03D
    add     [si+6], ax
    add     [si+18h], ax
    mul     word ptr [si+0Ch]
    sub     [si], ax
    sbb     [si+2], dx
    jmp     loc_2ECD9
loc_2F0A3:
    mov     dx, word ptr cs:[_sprite1+28]
    dec     dx
    mov     [si+8], dx
    xor     ax, ax
    sub     ax, [si]
    sbb     dx, [si+2]
    jl      short loc_2F03D
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
    jmp     loc_2ECD9
loc_2F0D7:
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
    jmp     loc_2ECD9
loc_2F110:
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
    jmp     loc_2ECD9
loc_2F13E:
    test    [bp+var_2], 8
    jz      short loc_2F148
    jmp     loc_2EDA5
loc_2F148:
    xor     dx, dx
    mov     ax, [si+2]
    cmp     ax, word ptr cs:[_sprite1+26]
    jge     short loc_2F157
    or      dh, 2
loc_2F157:
    mov     bx, [si]
    add     bx, 8000h
    adc     ax, 0
    cmp     ax, word ptr cs:[_sprite1+28]
    jl      short loc_2F16A
    or      dh, 1
loc_2F16A:
    mov     ax, [si+8]
    cmp     ax, word ptr cs:[_sprite1+26]
    jge     short loc_2F177
    or      dl, 2
loc_2F177:
    cmp     ax, word ptr cs:[_sprite1+28]
    jl      short loc_2F181
    or      dl, 1
loc_2F181:
    test    dl, dh
    jz      short loc_2F18A
    and     dl, dh
    jmp     loc_2EF9B
loc_2F18A:
    or      dl, dh
    jz      short loc_2F1A0
    xor     dh, dh
    mov     [bp+var_2], dx
    jmp     loc_2ECAD
loc_2F196:
    test    [bp+var_2], 1
    jz      short loc_2F1A0
    jmp     loc_2F01F
loc_2F1A0:
    jmp     loc_2ECD9
loc_2F1A3:
    mov     byte ptr [si+12h], 1
    cmp     cx, dx
    jnz     short loc_2F1AF
    mov     byte ptr [si+12h], 9
loc_2F1AF:
    jle     short loc_2F1BD
    mov     byte ptr [si+12h], 0
    mov     [si+2], dx
    mov     [si+8], cx
    xchg    cx, dx
loc_2F1BD:
    cmp     [bp+var_4], 0
    jnz     short loc_2F1E4
    mov     bx, word ptr cs:[_sprite1+16]
    cmp     ax, bx
    jge     short loc_2F1ED
    mov     al, 4
    mov     [si+6], bx
    mov     [si+0Ah], bx
loc_2F1D4:
    mov     [si+13h], al
    mov     word ptr [si+0Eh], 0
    xor     ah, ah
    pop     di
    pop     si
    mov     sp, bp
    pop     bp
    retf
loc_2F1E4:
    sub     dx, cx
    inc     dx
    mov     [si+0Eh], dx
    jmp     loc_2ECD9
loc_2F1ED:
    mov     bx, word ptr cs:[_sprite1+18]
    cmp     ax, bx
    jl      short loc_2F200
    mov     al, 8
    mov     [si+6], bx
    mov     [si+0Ah], bx
    jmp     short loc_2F1D4
loc_2F200:
    mov     ax, dx
    sub     ax, cx
    inc     ax
    mov     [si+0Eh], ax
    cmp     dx, word ptr cs:[_sprite1+26]
    jge     short loc_2F21B
    dec     word ptr [si+0Ah]
    mov     word ptr [si+16h], 1
    mov     al, 2
    jmp     short loc_2F1D4
loc_2F21B:
    cmp     cx, word ptr cs:[_sprite1+28]
    jl      short loc_2F22E
    dec     word ptr [si+0Ah]
    mov     word ptr [si+1Ah], 1
    mov     al, 1
    jmp     short loc_2F1D4
loc_2F22E:
    mov     ax, word ptr cs:[_sprite1+26]
    mov     bx, ax
    sub     ax, cx
    jle     short loc_2F23E
    mov     [si+2], bx
    sub     [si+0Eh], ax
loc_2F23E:
    mov     ax, dx
    mov     bx, word ptr cs:[_sprite1+28]
    dec     bx
    sub     ax, bx
    jle     short loc_2F250
    sub     [si+0Eh], ax
    mov     [si+8], bx
loc_2F250:
    jmp     loc_2ECD9
loc_2F253:
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
loc_2F26F:
    cmp     word ptr [si+6], 0C180h
    jg      short loc_2F2B0
loc_2F276:
    mov     ax, [si+6]
    add     ax, cx
    mov     [si+6], ax
    mov     bx, ax
    sub     ax, word ptr cs:[_sprite1+16]
    jle     short loc_2F2AB
    cmp     ax, cx
    jle     short loc_2F28D
    mov     ax, cx
loc_2F28D:
    sub     bx, word ptr cs:[_sprite1+18]
    jle     short loc_2F298
    sub     ax, bx
    jle     short loc_2F2AB
loc_2F298:
    mov     bx, [si+2]
    cmp     bx, word ptr cs:[_sprite1+26]
    jge     short loc_2F2A8
    add     [si+14h], ax
    jmp     short loc_2F2AB
    db 144
loc_2F2A8:
    add     [si+18h], ax
loc_2F2AB:
    add     [si+2], dx
    jmp     short loc_2F26F
loc_2F2B0:
    cmp     word ptr [si+0Ah], 3E80h
    jge     short loc_2F2D6
    cmp     word ptr [si+2], 0C180h
    jle     short loc_2F276
    cmp     word ptr [si+2], 3E80h
    jge     short loc_2F276
    cmp     word ptr [si+8], 0C180h
    jle     short loc_2F2D6
    cmp     word ptr [si+8], 3E80h
    jge     short loc_2F2D6
    jmp     loc_2EBAD
loc_2F2D6:
    mov     ax, [si+0Ah]
    sub     ax, cx
    mov     [si+0Ah], ax
    mov     bx, ax
    sub     ax, word ptr cs:[_sprite1+18]
    inc     ax
    jge     short loc_2F30F
    neg     ax
    cmp     ax, cx
    jle     short loc_2F2F0
    mov     ax, cx
loc_2F2F0:
    sub     bx, word ptr cs:[_sprite1+16]
    inc     bx
    jge     short loc_2F2FC
    add     ax, bx
    jle     short loc_2F30F
loc_2F2FC:
    mov     bx, [si+8]
    cmp     bx, word ptr cs:[_sprite1+26]
    jge     short loc_2F30C
    add     [si+16h], ax
    jmp     short loc_2F30F
    db 144
loc_2F30C:
    add     [si+1Ah], ax
loc_2F30F:
    sub     [si+8], dx
    jmp     short loc_2F2B0
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
word_2F354     dw 0
word_2F356     dw 0
word_2F358     dw 0
word_2F35A     dw 0
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
    mov     bx, offset word_2F358
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
    mov     cs:word_2F358, ax
    mov     ax, [bp+arg_2]
    mov     cs:word_2F35A, ax
    xor     ax, ax
    mov     es, ax
    mov     bx, 90h
    mov     ax, es:[bx]
    mov     cs:word_2F354, ax
    mov     ax, es:[bx+2]
    mov     cs:word_2F356, ax
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
    mov     ax, cs:word_2F356
    sub     ax, cs:word_2F354
    jz      short loc_2F3D8
    mov     dx, cs:word_2F354
    mov     ds, cs:word_2F356
    mov     al, 24h
    mov     ah, 25h
    int     21h
loc_2F3D8:
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
    jmp     short loc_2F3FD
    db 144
    push    bp
    mov     bp, sp
    sub     sp, 7D0h
    push    si
    push    di
    mov     si, [bp+arg_8]
    mov     [bp+var_A], 0
loc_2F3FD:
    mov     ax, [bp+arg_2]
    mov     _word_4031E, ax
    mov     ax, [bp+0Ah]
    mov     _word_40320, ax
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
    jmp     short loc_2F3FD
_nopsub_2F424 endp
nopsub_2F436 proc far
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
    jmp     short loc_2F3FD
word_2F448     dw 50
off_2F44A     dw offset off_2F4AC
    dw offset off_2F4AC
    dw offset off_2F4AC
    dw offset word_2F4AE
    dw offset word_2F4B2
    dw offset word_2F4B8
    dw offset word_2F4C0
    dw offset word_2F4CA
    dw offset word_2F4D6
    dw offset word_2F4E4
    dw offset word_2F4F4
    dw offset word_2F506
    dw offset word_2F51A
    dw offset word_2F530
    dw offset word_2F548
    dw offset word_2F562
    dw offset word_2F57E
    dw offset word_2F59C
    dw offset word_2F5BC
    dw offset word_2F5DE
    dw offset word_2F602
    dw offset word_2F628
    dw offset word_2F650
    dw offset word_2F67A
    dw offset word_2F6A6
    dw offset word_2F6D4
    dw offset word_2F704
    dw offset word_2F736
    dw offset word_2F76A
    dw offset word_2F7A0
    dw offset word_2F7D8
    dw offset word_2F812
    dw offset word_2F84E
    dw offset word_2F88C
    dw offset word_2F8CC
    dw offset word_2F90E
    dw offset word_2F952
    dw offset word_2F998
    dw offset word_2F9E0
    dw offset word_2FA2A
    dw offset word_2FA76
    dw offset word_2FAC4
    dw offset word_2FB14
    dw offset word_2FB66
    dw offset word_2FBBA
    dw offset word_2FC10
    dw offset word_2FC68
    dw offset word_2FCC2
    dw offset word_2FD1E
off_2F4AC     dw offset word_2FD7C
word_2F4AE     dw 32768
    dw 21845
word_2F4B2     dw 43690
word_2F4B4     dw 16384
    dw 32768
word_2F4B8     dw 49152
    dw 13107
    dw 26214
    dw 39321
word_2F4C0     dw 52428
    dw 10922
    dw 21845
    dw 32768
    dw 43690
word_2F4CA     dw 54613
    dw 9362
    dw 18724
    dw 28086
    dw 37449
    dw 46811
word_2F4D6     dw 56173
    dw 8192
    dw 16384
    dw 24576
    dw 32768
    dw 40960
    dw 49152
word_2F4E4     dw 57344
    dw 7281
    dw 14563
    dw 21845
    dw 29127
    dw 36408
    dw 43690
    dw 50972
word_2F4F4     dw 58254
    dw 6553
    dw 13107
    dw 19660
    dw 26214
    dw 32768
    dw 39321
    dw 45875
    dw 52428
word_2F506     dw 58982
    dw 5957
    dw 11915
    dw 17873
    dw 23831
    dw 29789
    dw 35746
    dw 41704
    dw 47662
    dw 53620
word_2F51A     dw 59578
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
word_2F530     dw 60074
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
word_2F548     dw 60494
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
word_2F562     dw 60854
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
word_2F57E     dw 61166
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
word_2F59C     dw 61440
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
word_2F5BC     dw 61680
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
word_2F5DE     dw 61895
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
word_2F602     dw 62086
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
word_2F628     dw 62259
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
word_2F650     dw 62415
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
word_2F67A     dw 62557
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
word_2F6A6     dw 62686
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
word_2F6D4     dw 62805
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
word_2F704     dw 62914
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
word_2F736     dw 63015
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
word_2F76A     dw 63108
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
word_2F790     dw 44470
    dw 46811
    dw 49152
    dw 51492
    dw 53833
    dw 56173
    dw 58514
    dw 60854
word_2F7A0     dw 63195
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
word_2F7D8     dw 63276
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
word_2F812     dw 63351
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
word_2F84E     dw 63421
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
word_2F88C     dw 63488
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
word_2F8CC     dw 63550
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
word_2F90E     dw 63608
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
word_2F952     dw 63663
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
word_2F998     dw 63715
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
word_2F9E0     dw 63764
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
word_2FA2A     dw 63811
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
word_2FA76     dw 63855
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
word_2FAC4     dw 63897
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
word_2FB14     dw 63937
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
word_2FB66     dw 63975
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
word_2FBBA     dw 64011
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
word_2FC10     dw 64046
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
word_2FC68     dw 64079
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
word_2FCC2     dw 64111
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
word_2FD1E     dw 64141
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
word_2FD7C     dw 64170
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
nopsub_2F436 endp
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
    jnz     short loc_2FE18
    lea     bx, [bp+var_1C]
    cmp     word ptr [bx+0Eh], 0
    jle     short loc_2FE18
    push    bx
    call    _putpixel_line1_maybe
    add     sp, 2
loc_2FE18:
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
loc_2FE2B:
    cmp     [bx], ax
    jnz     short loc_2FE34
    cmp     [bx+2], dx
    jz      short loc_2FE57
loc_2FE34:
    cmp     word ptr [bx+2], 0
    jz      short loc_2FE48
    add     bx, 4
    loop    loc_2FE2B
    mov     ax, offset DGROUP:_aExitListOverflow
    push    ax
    call    far ptr _fatal_error
loc_2FE48:
    mov     word ptr [bx+2], 0
    mov     [bx], ax
    mov     word ptr [bx+6], 0
    mov     [bx+2], dx
loc_2FE57:
    pop     bp
    retf
_add_exit_handler endp
_call_exitlist proc far
     r = byte ptr 0
    push    di
    mov     di, 28h
loc_2FE5D:
    mov     ax, (_exitlistfuncs+2)[di]
    or      ax, ax
    jz      short loc_2FE6B
    push    di
    call    dword ptr _exitlistfuncs[di]
    pop     di
loc_2FE6B:
    sub     di, 4
    jl      short loc_2FE72
    jmp     short loc_2FE5D
loc_2FE72:
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
    jnb     short loc_2FEDD
    cmp     [bp+var_fatal], 0
    jnz     short loc_2FECD
    xor     ax, ax
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_2FECD:
    mov     ax, offset DGROUP:_aSFileError_1
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    push    ax
    call    far ptr _fatal_error
loc_2FEDD:
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
    jz      short loc_2FF00
    mov     bx, 1
loc_2FF00:
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
    jnb     short loc_2FF68
    jmp     short loc_2FFB5
    db 144
    var_fatal = word ptr -8
    var_6 = byte ptr -6
    var_5 = word ptr -5
    var_filehandle = word ptr -2
     s = byte ptr 0
     r = byte ptr 2
    arg_filename = word ptr 6
    arg_fatal = word ptr 8
loc_2FF68:
    mov     [bp+var_filehandle], ax
    mov     bx, [bp+var_filehandle]
    lea     dx, [bp+var_6]
    mov     cx, 4
    mov     ah, 3Fh
    int     21h
    cmp     ax, cx
    jnz     short loc_2FFAE
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h
    mov     ax, [bp+var_5]
    mov     dx, [bp-3]
    xor     dh, dh
    xor     bx, bx
    test    ax, 0Fh
    jz      short loc_2FF95
    mov     bx, 1
loc_2FF95:
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
loc_2FFAE:
    mov     bx, [bp+var_filehandle]
    mov     ah, 3Eh
    int     21h
loc_2FFB5:
    cmp     [bp+var_fatal], 0
    jnz     short loc_2FFC4
    xor     ax, ax
    pop     di
    pop     si
    pop     ds
    mov     sp, bp
    pop     bp
    retf
loc_2FFC4:
    mov     ax, offset DGROUP:_aSFileError_1
    mov     bx, ss
    mov     ds, bx
    push    [bp+arg_filename]
    push    ax
loc_2FFCF:
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
loc_2FFFD:
    lodsb
    stosb
    cmp     al, 0
    jz      short _file_find_ok
    cmp     al, 3Ah
    jz      short loc_3000B
    cmp     al, 5Ch
    jnz     short loc_3000F
loc_3000B:
    mov     _foundfileptr, di
loc_3000F:
    loop    loc_2FFFD
_file_find_ok:
    mov     ax, ds
    mov     es, ax
    mov     si, offset DGROUP:_foundfilepath
    mov     di, _foundfileptr
    mov     cx, 0Dh
    cld
    rep movsb
    mov     ax, offset DGROUP:_foundfile
loc_30025:
    pop     di
    pop     si
    pop     ds
    pop     bp
    retf
_file_find_err:
    xor     ax, ax
    jmp     short loc_30025
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
    mov     _byte_3F85A, 1
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
loc_30091:
    mov     al, bl
    out     dx, al
    inc     dx
    mov     al, _byte_3F85C[bx]
    out     dx, al
    dec     dx
    inc     bx
    loop    loc_30091
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
