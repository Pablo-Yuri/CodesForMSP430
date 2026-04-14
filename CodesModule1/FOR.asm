    .cdecls "msp430.h"
    .global main

    .text
main: 
    ; Rn = Rn-1 + Rn-2
    mov.w   #(WDTPW|WDTHOLD), &WDTCTL
    
    clr R4
    ; mov     #v1, R6
    ; mov.b   @R6+, R7
for:
    cmp #5, R4 
    JEQ end
    INC R4
    JMP for

end:
    JMP $
    nop

;     .data
; v1: .byte 4,1,2,3,4
