    .cdecls "msp430.h"
    .global main

    .text

// Exercício 4: Escreva um programa que calcule a soma de um vetor de 5 bytes. 
// O resultado deve ser armazenado em R12.
main:
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL
    mov R12, vetor      
    mov #5, R13 
    ; mov #0, R6       
    call #reduceSum8    

    ; jmp fim


reduceSum8:
    push R4             ;
    mov #0, R4           

loop:       
    add.b @R12, R4       
    ; add R6(R12), R4

    ; inc R6
    inc R12             
    dec R13 

    jnz loop
    ; jmp     $
    nop


concluido:
    mov R12, R4         
    pop R4              
    ret                 

; --- Dados ---
     .data
vetor: .byte 10, 20, 30, 40, 50  

; ---------------------------------------------------
