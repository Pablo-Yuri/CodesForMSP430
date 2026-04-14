     .cdecls "msp430.h"
    .global main

    .text
main: 
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL
    
    mov.b #0x4, R6
    clr R7
    mov #vetor, R5
    ; push R4
    clr R4

loop:

    mov.b @R5, R4
    add R4, R7
    inc R5
    dec R6

    jnz loop
    
    jmp $
    nop

    .data
vetor: .byte 1, 2, 3, 4