    .cdecls "msp430.h"
    .global main

    .text
main: 
        mov.w   #(WDTPW|WDTHOLD), &WDTCTL
        mov   #34, R4
        mov   #-5, R5

compara:
        cmp   R4, R5            ; R5 > R4 ?  --> R5 - R4
        ; JL    r5menor           ; R5 é menor  (R5 < R4) --> R5 - R4
        JGE   r5maiorOuIgual    ; R5 é maior ou igual (R5 >= R4) --> R5 - R4

r5menor:
        JMP   $
r5maiorOuIgual:
        jmp   $

    
    jmp       $
    nop

