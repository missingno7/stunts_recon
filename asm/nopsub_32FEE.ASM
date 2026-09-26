.8086
_TEXT segment word public 'CODE'
assume cs:_TEXT
public _nopsub_32FEE
_nopsub_32FEE proc far
    mov dx,03DAh
wait_retrace_low:
    in al,dx
    test al,8
    jne short wait_retrace_low
wait_retrace_high:
    in al,dx
    test al,8
    je short wait_retrace_high
    retf
_nopsub_32FEE endp
_TEXT ends
end
