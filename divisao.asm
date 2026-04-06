    .cdecls "msp430.h"
    .global main

    .text
main: 
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL

    mov.b   #0xC, R4
    mov.b   #0x3, R5
    clr     R6
    clr     R7
loop:          
    cmp R5, R4
    jn fim
    sub R5, R4
    inc R6
    jmp loop

fim:
    mov.b R4, R7
    jmp $
    nop