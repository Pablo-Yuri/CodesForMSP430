    .cdecls "msp430.h"
    .global main

    .text

main:
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL

    mov.b   #7, R4
    mov.b   @R4, R6
    mov.b   #9, R5
    add.b   R4, R5

    jmp     $
    nop

; ---------------------------------------------------
