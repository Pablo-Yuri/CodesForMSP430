        .text
        .def umult8 ; Unsigned multiplication 8 bits
;---------------------------------------------------------

umult8:
        push    R4              ; Backup do valor de R4
        clr     R4              ; R4 = 0

        tst     R12             ; Verifica se R12 == 0
        jz      umult8_ret      ; Se 0 a subrotina para

        tst     R13             ; Verifica se R13 == 0
        jz      umult8_ret      ; Se 0 a subrotina para
umult8_loop:
        add.b   R12, R4
        dec.b   R13
        jnz     umult8_loop

        mov.w   R4, R12
umult8_ret:
        pop     R4
        ret