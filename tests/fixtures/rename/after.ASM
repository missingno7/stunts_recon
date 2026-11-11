; reviewed_function: the function's own label
; A narrative mention of old_function stays.
extrn _reviewed_function:far
public _caller
CODE segment byte public 'CODE'
_caller proc far
    call _reviewed_function
    db 'old_function', 0
_caller endp
CODE ends
comment !
_old_function must stay in this comment
!
end
