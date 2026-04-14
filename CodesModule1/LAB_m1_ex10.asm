    .cdecls "msp430.h"
    .global main

    .text

// Exercício 10: Escreva um programa que calcule a soma de dois números, armazenados em R4 e R5. O resultado deve ser armazenado em R6.

main:
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL

    mov.b #4, R4
    mov.b #5, R5
    clr R6

loop:

    add R5, R6
    dec R4

    jnz loop
    jmp     $
    nop

; ---------------------------------------------------
