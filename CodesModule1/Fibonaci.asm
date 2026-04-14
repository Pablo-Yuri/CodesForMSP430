    .cdecls "msp430.h"
    .global main

    .text
main: 
    ; Rn = Rn-1 + Rn-2
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL
    
    ; mob.b   R4      ; Qtd de valores da sequência
    mov     #1, R5
    mov     #2, R6
    clr     R7

fibonaci:                   ; 1+2+3+5+8+13+...
    mov     R5, R7
    add     R6, R7

    mov     R6, R5
    mov     R7, R6

    jmp     fibonaci
    nop
    
