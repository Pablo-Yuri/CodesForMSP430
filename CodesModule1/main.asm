    .cdecls "msp430.h"
    .global main

    .text
    .ref    mset, mclr
    .ref    umult8
    .ref    fibonaci
main: 
    mov     #0x4400, SP
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL

    ; Limpar enderços de memória
    mov     #0x2400, R12    ; Início
    mov     #8,R13          ; Qtd de Bytes
    call    #mclr

    ; Setar um valor em vários endereços de memória
    mov     #0x0FE,  R12    ; -> Dado
    mov     #0x2500, R13    ; -> início
    mov.b   #4, R14         ; -> Oque por no lugar
    call    #mset

    ; Multiplicação de 2 valores sem sinal
    mov.b   #0x2,   R12     ; Valor A / Resultado final
    mov.b   #0x5,   R13     ; Valor B
    call    #umult8         ; Chamar sub rotina. Resultado final --> R12

    ; Calculo de Fibonnaci
    ; mov.b

    jmp $
    nop
