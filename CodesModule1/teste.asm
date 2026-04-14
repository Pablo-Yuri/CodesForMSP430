    .cdecls "msp430.h"
    .global main

    .text

main:
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL
    mov.b   #v1, R12
    mov.b   #4, R13
    push R4
    clr R4
    jmp     $
umaior8_loop:
    cmp.B   @R12, R4
    jhs     umaior8_cont
    mov.B   @R12, R4

umaior8_cont:
    inc     R12
    dec     R13
    jnz     umaior8_loop
    mov     R4, R12
    pop R4
    ret


    .data
v1: .byte   1 , 2 , 3 , 4
; ---------------------------------------------------
