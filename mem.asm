        .text
        .def mset, mclr
;--------------------------------------------

mset:
        mov.b   R12, 0(R13)
        inc.w   R13
        dec.w   R14
        jnz mset
        ret

;--------------------------------------------

mclr:
        mov.w   R13, R14
        mov.w   R12, R13
        mov.b   #0, R12
        call    #mset
        ret

