    .cdecls "msp430.h"
    .global main

    .text

// Exercício 18: Escreva um programa que encontre o maior e o menor valor de um vetor de 5 bytes. 
// O resultado deve ser armazenado em R12 (maior) e R13 (menor).

main:
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL
    mov R12, vetor      
    mov #5, R13 
    
    call #extremos
    jmp $
    nop
extremos:
      push R4
      push R5
      clr R4
      mov @R12, R5
extremos_loop:
    cmp #0, R13
    jz fim
    cmp @R12, R4    ; --> R4 - @R12, negativo se @R12 for maior que R4 
    JL novo_maior

    cmp @R12, R5     ; -> R5 - @R12, positivo ou zero se R4 maior
    JGE novo_menor

novo_maior:
    mov @R12, R4
    inc R12
    dec R13
    jmp extremos_loop
novo_menor:
    mov @R12, R5
    inc R12
    dec R13
    jmp extremos_loop

fim:
    mov R4, R12
    mov R5, R13
    pop R5
    pop R4
    ret

; --- Dados ---
     .data
vetor: .byte 10, 20, 30, 7, 50  

; ---------------------------------------------------
