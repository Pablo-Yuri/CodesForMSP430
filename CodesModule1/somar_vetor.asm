    .cdecls "msp430.h"
    .global main

    .text
main: 
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL

    mov #v1, R12    ;vetor
    clr R4          ;Soma
    mov.b #0x5, R5  ;len Vetor

loop:
    mov.b @R12, R6  ;Posição N do Vetor
    add R6, R4      ;soma o valor da posicição N
    inc R12         ;Anda o ponteiro
    dec R5          ; len --
    jnz loop        ; Verifica se len == 0
    jmp $           ; jump incodicional para evitar bugs
    nop             ; Boas práticas kk

    .data
v1: .byte 5,2,6,4,5 ; Vetor