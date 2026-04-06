    .cdecls "msp430.h"
    .global main

    .text
main: 
        mov.w   #(WDTPW|WDTHOLD), &WDTCTL

        mov.w   #var32_1, R4
        mov.w   #var32_2, R5

        mov.w   0(R4), R10  ; [ R11 | R10 ]
        mov.w   2(R4), R11  
                            ; +
        mov.w   0(R5), R12  ; [ R13 | R12 ]
        mov.w   2(R5), R13
                            ;-----------------

        add.w   R10, R12
        addc.w  R11, R13

        mov.b   #4, R12
        mov.b   #5, R13
        call    #maior8u

        mov.w   #var32_1, R12
        mov.w   #var32_2, R13
        call    #maior32u


        jmp $
        nop
;----------------------------------------------
; Subrotina maior8u compara R12 e 13 e retorna
;   Compara bytes de 8 bits
;   Se R12 > R13 --> R12 = 1
;   Se R12 == R13 -> R12 = 0
;   Se R12 < R13 --> R12 = -1

maior8u:
        CMP     R12, R13    ; R13 - R12
        JLO     r13menor
        JEQ     r13igual
r13maior:
        mov.b   #-1, R12
        ret
r13menor:
        mov.b   #1,  R12
        ret
r13igual:
        mov.b   #0,  R13
        ret
;----------------------------------------------
; Subrotina maior32u compara var1 e var2 e retorna
;   Compara words de 32 bits
;   Se var1 > var2 --> R12 = 1
;   Se var1 == var2 -> R12 = 0
;   Se var1 < var2 --> R12 = -1

maior32u:
        mov.w   @R12+,   R4     ; var1
        mov.w   @R12,    R5     ; [ R5 | R4 ]

        mov.w   @R13+,  R6      ; var2
        mov.w   @R13,  R7       ; [ R7 | R6 ]

        CMP.w     R5, R7    ; R7 - R6
        JLO     var2menor
        JEQ     var2igual
var2maior:
        mov.w   #-1, R12
        ret
var2menor:
        mov.w   #1,  R12
        ret
var2igual:
        cmp.w     R4, R6
        JEQ     retorno
        JHS     var2maior
        jmp     var2menor
retorno:
        mov.w   #0, R12
        ret
;---------------------------------------------
        .data
var32_1:.long   0x12345678
var32_2:.long   0x1234FFFF
; var32_1:.long   0x00000001
