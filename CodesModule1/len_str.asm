    .cdecls "msp430.h"
    .global main

    .text
main: 
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL
    mov #v1, R12
    push R4
    clr R4
loop:
    ; mov.b 0(R12), R5 
    TST 0(R12)
    JZ fim
    inc R12
    inc R4
    jmp loop

fim:
    mov.b R4, R12
    pop R4
    ret

    .data
; v1: .byte "PABLO", 0x00, "0"
v1: .cstring "PABLO"