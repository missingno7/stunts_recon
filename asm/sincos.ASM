.8086
S012_TEXT segment word public 'CODE'
S012_TEXT ends
_DATA segment word public 'DATA'
public _sinetable
_sinetable dw 0
    dw 101
    dw 201
    dw 302
    dw 402
    dw 503
    dw 603
    dw 704
    dw 804
    dw 904
    dw 1005
    dw 1105
    dw 1205
    dw 1306
    dw 1406
    dw 1506
    dw 1606
    dw 1706
    dw 1806
    dw 1906
    dw 2006
    dw 2105
    dw 2205
    dw 2305
    dw 2404
    dw 2503
    dw 2603
    dw 2702
    dw 2801
    dw 2900
    dw 2999
    dw 3098
    dw 3196
    dw 3295
    dw 3393
    dw 3492
    dw 3590
    dw 3688
    dw 3786
    dw 3883
    dw 3981
    dw 4078
    dw 4176
    dw 4273
    dw 4370
    dw 4467
    dw 4563
    dw 4660
    dw 4756
    dw 4852
    dw 4948
    dw 5044
    dw 5139
    dw 5235
    dw 5330
    dw 5425
    dw 5520
    dw 5614
    dw 5708
    dw 5803
    dw 5897
    dw 5990
    dw 6084
    dw 6177
    dw 6270
    dw 6363
    dw 6455
    dw 6547
    dw 6639
    dw 6731
    dw 6823
    dw 6914
    dw 7005
    dw 7096
    dw 7186
    dw 7276
    dw 7366
    dw 7456
    dw 7545
    dw 7635
    dw 7723
    dw 7812
    dw 7900
    dw 7988
    dw 8076
    dw 8163
    dw 8250
    dw 8337
    dw 8423
    dw 8509
    dw 8595
    dw 8680
    dw 8765
    dw 8850
    dw 8935
    dw 9019
    dw 9102
    dw 9186
    dw 9269
    dw 9352
    dw 9434
    dw 9516
    dw 9598
    dw 9679
    dw 9760
    dw 9841
    dw 9921
    dw 10001
    dw 10080
    dw 10159
    dw 10238
    dw 10316
    dw 10394
    dw 10471
    dw 10549
    dw 10625
    dw 10702
    dw 10778
    dw 10853
    dw 10928
    dw 11003
    dw 11077
    dw 11151
    dw 11224
    dw 11297
    dw 11370
    dw 11442
    dw 11514
    dw 11585
    dw 11656
    dw 11727
    dw 11797
    dw 11866
    dw 11935
    dw 12004
    dw 12072
    dw 12140
    dw 12207
    dw 12274
    dw 12340
    dw 12406
    dw 12472
    dw 12537
    dw 12601
    dw 12665
    dw 12729
    dw 12792
    dw 12854
    dw 12916
    dw 12978
    dw 13039
    dw 13100
    dw 13160
    dw 13219
    dw 13279
    dw 13337
    dw 13395
    dw 13453
    dw 13510
    dw 13567
    dw 13623
    dw 13678
    dw 13733
    dw 13788
    dw 13842
    dw 13896
    dw 13949
    dw 14001
    dw 14053
    dw 14104
    dw 14155
    dw 14206
    dw 14256
    dw 14305
    dw 14354
    dw 14402
    dw 14449
    dw 14497
    dw 14543
    dw 14589
    dw 14635
    dw 14680
    dw 14724
    dw 14768
    dw 14811
    dw 14854
    dw 14896
    dw 14937
    dw 14978
    dw 15019
    dw 15059
    dw 15098
    dw 15137
    dw 15175
    dw 15213
    dw 15250
    dw 15286
    dw 15322
    dw 15357
    dw 15392
    dw 15426
    dw 15460
    dw 15493
    dw 15525
    dw 15557
    dw 15588
    dw 15619
    dw 15649
    dw 15679
    dw 15707
    dw 15736
    dw 15763
    dw 15791
    dw 15817
    dw 15843
    dw 15868
    dw 15893
    dw 15917
    dw 15941
    dw 15964
    dw 15986
    dw 16008
    dw 16029
    dw 16049
    dw 16069
    dw 16088
    dw 16107
    dw 16125
    dw 16143
    dw 16160
    dw 16176
    dw 16192
    dw 16207
    dw 16221
    dw 16235
    dw 16248
    dw 16261
    dw 16273
    dw 16284
    dw 16295
    dw 16305
    dw 16315
    dw 16324
    dw 16332
    dw 16340
    dw 16347
    dw 16353
    dw 16359
    dw 16364
    dw 16369
    dw 16373
    dw 16376
    dw 16379
    dw 16381
    dw 16383
    dw 16384
    dw 16384
_DATA ends
CONST segment word public 'CONST'
CONST ends
_BSS segment word public 'BSS'
_BSS ends
DGROUP group _DATA, CONST, _BSS
assume cs:S012_TEXT, ds:DGROUP
S012_TEXT segment word public 'CODE'
public _sinfast
public _cosfast
_sinfast proc far
     sin_fast_s = byte ptr 0
     sin_fast_r = byte ptr 2
    sin_fast_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+sin_fast_arg_0]
code_sin_fast_main:
    mov     bl, ah
    xor     ah, ah
    and     bx, 3
    shl     bx, 1
    jmp     cs:sine_quadrant_dispatch[bx]
sine_quadrant_dispatch     dw offset sinfast_finish
    dw offset sinfast_finish_2
    dw offset sinfast_finish_3
    dw offset sinfast_finish_4
sinfast_finish:
    mov     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    pop     bp
    retf
sinfast_finish_2:
    mov     bx, 100h
    sub     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    pop     bp
    retf
sinfast_finish_3:
    mov     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    neg     ax
    pop     bp
    retf
sinfast_finish_4:
    mov     bx, 100h
    sub     bx, ax
    shl     bx, 1
    mov     ax, _sinetable[bx]
    neg     ax
    pop     bp
    retf
_sinfast endp
_cosfast proc far
     cos_fast_s = byte ptr 0
     cos_fast_r = byte ptr 2
    cos_fast_arg_0 = word ptr 6

    push    bp
    mov     bp, sp
    mov     ax, [bp+cos_fast_arg_0]
    add     ax, 100h
    jmp     short code_sin_fast_main

    db 0
_cosfast endp
S012_TEXT ends
end
