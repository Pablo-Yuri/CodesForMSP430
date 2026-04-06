    .cdecls "msp430.h"
    .global main

    .text
main: 
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL
    push R5
    clr R5
    ; mov.b R5, &R4
    pop R5
    tst 0(R4)
    mov #addr2, R12
    mov.b @R12, R4
    ; inc R4
    ; mov R4, &addr
    ; push R4
    jmp $
    nop

    .data
addr: .word 1,"PABLO", 0
addr2: .byte "PABLO", 0
addr3: .cstring "PABLO"
