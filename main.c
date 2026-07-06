#include <msp430f5529.h>
#include <stdio.h>

#include "lcd.h"
#include "ir.h"

int main(void)
{
    char texto[17];

    WDTCTL = WDTPW | WDTHOLD;

    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    LCD_Init();
    IR_Init();

    LCD_Clear();

    LCD_SetCursor(0,0);
    LCD_WriteString("IR NEC Ready");

    while(1)
    {
        if(IR_Available())
        {
            uint8_t cmd;

            cmd = IR_GetCommand();

            sprintf(texto,"CMD=%02X",cmd);

            LCD_Clear();

            LCD_SetCursor(0,0);
            LCD_WriteString("Recebido:");

            LCD_SetCursor(1,0);
            LCD_WriteString(texto);

            P1OUT ^= BIT0;
        }
    }
}
