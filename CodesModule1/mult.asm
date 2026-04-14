    .cdecls "msp430.h"
    .global main

    .text
main: 
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL

    mov.b   #0x4, R4
    mov.b   #0x5, R5
    clr R6
loop:
    ADD R4, R6
    DEC R5
    JNZ loop

    jmp $
    nop
