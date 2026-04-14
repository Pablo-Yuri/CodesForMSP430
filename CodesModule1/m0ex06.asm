    .cdecls "msp430.h"
    .global main

    .text

main:
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL

    mov.b   #v1, R12
    push R4
    clr R4
    
    jpm $
    nop

; ---------------------------------------------------
    .data
v1: .byte   1 , 2 , 3 , 4


